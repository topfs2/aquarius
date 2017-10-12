#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>
#include <string>
#include "IDecoder.h"

class CVorbisFileDecoder : public IDecoder {
public:
    CVorbisFileDecoder();
    virtual ~CVorbisFileDecoder() { }

    virtual bool Open(const std::string &filename);
    virtual int Read(char *data, size_t length);
    virtual void Close();

    virtual ALuint GetFrequency();
    virtual ALenum GetFormat();
    virtual ALuint GetLength();

private:
    OggVorbis_File *m_vf;
    vorbis_info *m_vorbisInfo;
};