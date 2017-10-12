#pragma once
#include <map>
#include <string>
#include "entityx/entityx.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include "components.h"
#include "events.h"
#include "IDecoder.h"

class COpenALSoundSystem : public entityx::System<COpenALSoundSystem>, public entityx::Receiver<COpenALSoundSystem>
{
public:
    COpenALSoundSystem();
    ~COpenALSoundSystem();

    void configure(entityx::ptr<entityx::EventManager> events);
    void update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt);

    void receive(const UpdateObserverEvent &e);

private:
    static size_t FillBuffer(ALuint buffer, IDecoder *decoder, size_t length);
    static void SetupSource(ALuint source, entityx::ptr<Sound> sound);

    void CreateStreamingSource(ALuint source, std::string filename);
    ALuint CreateSource(entityx::ptr<Sound> sound);
    ALuint FindBuffer(const std::string &filename, bool create);

    typedef std::map<std::string, ALuint> BufferMap;
    typedef std::map<entityx::Entity::Id, ALuint> SourceMap;
    typedef std::map<ALuint, IDecoder *> StreamSourceMap;

    entityx::Entity m_observer;

    BufferMap m_buffers;
    SourceMap m_sources;
    StreamSourceMap m_streams;
};
