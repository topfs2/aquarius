#include "VorbisFileDecoder.h"
#include "Log.h"
#include <string.h>

using namespace logging;

CVorbisFileDecoder::CVorbisFileDecoder() : m_vf(NULL), m_vorbisInfo(NULL) { }

bool CVorbisFileDecoder::Open(const std::string &filename) {
    Close();

    FILE *f = fopen(filename.c_str(), "rb");
    if (f == NULL) {
        LOG(ERROR, "fopen() failed: " << strerror(errno));
        return false;
    }

    m_vf = (OggVorbis_File *)malloc(sizeof(OggVorbis_File));
    int result = ov_open(f, m_vf, NULL, 0);
    if(result < 0) {
        fclose(f);
        LOG(ERROR, "Could not open Ogg stream. " << result);// << errorString(result));
        return false;
    }

    m_vorbisInfo = ov_info(m_vf, -1);
    return true;
}

int CVorbisFileDecoder::Read(char *data, size_t length) {
    if (m_vf) {
        int section;
        return ov_read(m_vf, data, length, 0, 2, 1, &section);
    }

    return 0;
}

void CVorbisFileDecoder::Close() {
    if (m_vf) {
        ov_clear(m_vf);
        free(m_vf);
        m_vf = NULL;
        m_vorbisInfo = NULL;
    }
}

ALuint CVorbisFileDecoder::GetFrequency() {
    if (m_vf && m_vorbisInfo) {
        return m_vorbisInfo->rate;
    }

    return 0;
}

ALenum CVorbisFileDecoder::GetFormat() {
    if (m_vf && m_vorbisInfo) {
        return m_vorbisInfo->channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
    }

    return 0;
}

ALuint CVorbisFileDecoder::GetLength() {
    if (m_vf && m_vorbisInfo) {
        return ov_pcm_total(m_vf, -1) * m_vorbisInfo->channels * 2; // pcm size to byte size
    }

    return 0;
}