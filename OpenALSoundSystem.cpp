#include "OpenALSoundSystem.h"
#include "VorbisFileDecoder.h"
#include "Log.h"

#define STREAM_BUFFERS 5
#define STREAM_BUFFER_SIZE (4096)

/* TODO Move to string utils */
bool EndsWith(const std::string& s, const std::string& suffix) {
    if (suffix.size() > s.size()) return false;
    return std::equal(s.begin() + s.size() - suffix.size(), s.end(), suffix.begin());
}

bool StartsWith(const std::string &s, const std::string &prefix) {
    if (prefix.size() > s.size()) return false;
    return std::mismatch(prefix.begin(), prefix.end(), s.begin()).first == prefix.end();
}

using namespace logging;

COpenALSoundSystem::COpenALSoundSystem() {
    alutInit(NULL, NULL);
    alGetError();
}

COpenALSoundSystem::~COpenALSoundSystem() {
    LOG(INFO, "Releasing sources");
    for (auto itr : m_sources) {
        alSourceStop(itr.second);
        alDeleteSources(1, &itr.second);
    }
    m_sources.clear();

    LOG(INFO, "Releasing buffers");
    for (auto itr : m_buffers) {
        alDeleteBuffers(1, &itr.second);
    }
    m_buffers.clear();

    LOG(INFO, "Releasing device");
    alutExit();
}

void COpenALSoundSystem::configure(entityx::ptr<entityx::EventManager> events) {
    events->subscribe<UpdateObserverEvent>(*this);
}

void COpenALSoundSystem::update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt) {
    ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

    float listener_position[3] = { 0.0f, 0.0f, 0.0f };
    float listener_velocity[3] = { 0.0f, 0.0f, 0.0f };

    if (m_observer.valid()) {
        entityx::ptr<Position> position = m_observer.component<Position>();
        entityx::ptr<Velocity> velocity = m_observer.component<Velocity>();

        if (position) {
            listener_position[0] = position->x;
            listener_position[1] = position->y;
//            listener_position[2] = position->z;
        }

        if (velocity) {
            listener_velocity[0] = velocity->x;
            listener_velocity[1] = velocity->y;
//            listener_velocity[2] = velocity->z;
        }
    }

    alListener3f(AL_POSITION, listener_position[0], listener_position[1], listener_position[2]);
    alListener3f(AL_VELOCITY, listener_velocity[0], listener_velocity[1], listener_velocity[2]);
    alListenerfv(AL_ORIENTATION, listenerOri);

    for (StreamSourceMap::iterator itr = m_streams.begin(); itr != m_streams.end(); itr++) {
        int processed;
        bool active = true;

        ALuint source = itr->first;
        IDecoder *decoder = itr->second;

        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
        while(processed > 0)
        {
            ALuint buffer;
            alSourceUnqueueBuffers(source, 1, &buffer);

            if (buffer) {
                int read = FillBuffer(buffer, decoder, STREAM_BUFFER_SIZE);
                if (read > 0) {
                    alSourceQueueBuffers(source, 1, &buffer);
                } else {
                    LOG(INFO, "EOF on stream for source " << source << " " << read);
                    alDeleteBuffers(1, &buffer);
                }
            }

            processed--;
        }
    }

    for (auto entity : entities->entities_with_components<Sound>()) {
        entityx::ptr<Position> position = entity.component<Position>();
        entityx::ptr<Velocity> velocity = entity.component<Velocity>();
        entityx::ptr<Sound> sound = entity.component<Sound>();

        ALuint source = 0;
        SourceMap::iterator itr = m_sources.find(entity.id());
        if (itr == m_sources.end()) {
            ALuint source = CreateSource(sound);
            if (sound->autoplay) {
                LOG(INFO, "Setting source to play " << source);
                alSourcePlay(source);
            }

            m_sources[entity.id()] = source;
        } else {
            source = itr->second;
        }

        if (source) {
            if (position) {
                alSource3f(source, AL_POSITION, position->x, position->y, 0.0f);
            } else {
                alSource3f(source, AL_POSITION, listener_position[0], listener_position[1], listener_position[2]);
            }

            if (velocity) {
                alSource3f(source, AL_VELOCITY, velocity->x, velocity->y, 0.0f);
            } else {
                alSource3f(source, AL_VELOCITY, listener_velocity[0], listener_velocity[1], listener_velocity[2]);
            }

            bool playing = sound->playing;
            ALint source_state;
            alGetSourcei(source, AL_SOURCE_STATE, &source_state);
            sound->playing = source_state == AL_PLAYING;
            if (!sound->playing && playing) {
                LOG(INFO, "Releasing source " << source);
                alSourceStop(source);
                alDeleteSources(1, &source);

                StreamSourceMap::iterator stream = m_streams.find(source);
                if (stream != m_streams.end()) {
                    LOG(INFO, "Releasing decoder for source " << source);
                    delete stream->second;
                    m_streams.erase(stream);
                }

                m_sources[entity.id()] = 0;
            }
        } else {
            sound->playing = false;
        }
    }
}

void COpenALSoundSystem::receive(const UpdateObserverEvent &e)
{
    m_observer = e.entity;
}

size_t COpenALSoundSystem::FillBuffer(ALuint buffer, IDecoder *decoder, size_t length) {
    if (decoder) {
#ifdef FILL_STACK
        char data[length];
#else
        char *data = new char[length];
#endif
        size_t read = 0;
        bool continue_read = true;
        while (continue_read && read < length && length - read > 0) {
            int result = decoder->Read(data + read, length - read);

            if (result < 0) {
                LOG(ERROR, "OGG failed to read: " << result);// << errorString(result));
                continue_read = false;
                return 0;
            } else if (result > 0) {
                read += result;
            } else {
                continue_read = false;
            }
        }

        alBufferData(buffer, decoder->GetFormat(), data, length, decoder->GetFrequency());

#ifndef FILL_STACK
        delete data;
#endif

        return read;
    }

    LOG(ERROR, "Called FillBuffer without proper decoder");
    return 0;
}

void COpenALSoundSystem::SetupSource(ALuint source, entityx::ptr<Sound> sound) {
    alSourcef(source, AL_PITCH, 1);
    alSourcef(source, AL_GAIN, sound->volume);
    alSource3f(source, AL_POSITION, 0, 0, 0);
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    alSourcei(source, AL_REFERENCE_DISTANCE, 1.0f);
    alSourcei(source, AL_MAX_DISTANCE, 50.0f);

    alSourcei(source, AL_LOOPING, sound->looping ? AL_TRUE : AL_FALSE);
}

void COpenALSoundSystem::CreateStreamingSource(ALuint source, std::string filename) {
    LOG(INFO, "Creating streaming source " << filename);
    IDecoder *decoder = new CVorbisFileDecoder();

    if (decoder->Open(filename)) {
        ALuint buffers[STREAM_BUFFERS];
        alGenBuffers((ALuint)STREAM_BUFFERS, buffers);
        if (alGetError() != AL_NO_ERROR) {
            LOG(ERROR, "Failed to generate buffers");
            delete decoder;
            return;
        }

        int filled = 0;
        bool continue_read = true;
        while (filled < STREAM_BUFFERS && continue_read) {
            int read = FillBuffer(buffers[filled], decoder, STREAM_BUFFER_SIZE);
            if (read > 0) {
                filled++;
            } else {
                continue_read = false;
            }
        }

        if (filled - STREAM_BUFFERS > 0) {
            LOG(INFO, "Only needed " << filled << " buffers to fill source " << source);
            alDeleteBuffers(filled - STREAM_BUFFERS, buffers + filled);
        }

        alSourceQueueBuffers(source, filled, buffers);
        m_streams[source] = decoder;
    }
}

ALuint COpenALSoundSystem::CreateSource(entityx::ptr<Sound> sound) {
    ALuint source;
    alGenSources((ALuint)1, &source);
    if (source == 0) {
        LOG(ERROR, "Failed to create source");
        return 0;
    }

    LOG(INFO, "Creating source " << sound->filename << " as " << source);

    SetupSource(source, sound);

    bool cache = StartsWith(sound->filename, "sound/");
    ALuint buffer = FindBuffer(sound->filename, cache);
    if (!cache && buffer == 0) {
        CreateStreamingSource(source, sound->filename);
    } else if (buffer) {
        alSourcei(source, AL_BUFFER, buffer);
    }

    return source;
}

ALuint COpenALSoundSystem::FindBuffer(const std::string &filename, bool create) {
    ALuint error;

    BufferMap::iterator itr = m_buffers.find(filename);
    if (itr == m_buffers.end() && create) {
        LOG(INFO, "Creating buffer " << filename);
        ALuint buffer = 0;

        if (EndsWith(filename, std::string(".ogg"))) {
            CVorbisFileDecoder decoder;

            if (decoder.Open(filename)) {
                alGenBuffers((ALuint)1, &buffer);
                if (alGetError() != AL_NO_ERROR) {
                    LOG(ERROR, "Failed to generate buffers");
                    return 0;
                }
                ALuint size = decoder.GetLength();
                FillBuffer(buffer, &decoder, size);
            }
        } else if (EndsWith(filename, std::string(".wav"))) {
            buffer = alutCreateBufferFromFile(filename.c_str());
        } else {
            LOG(WARNING, "Unknown file ending " << filename);
        }

        if (buffer) {
            m_buffers[filename] = buffer;
        }

        return buffer;
    } else if (itr != m_buffers.end()) {
        return itr->second;
    }

    return 0;
}
