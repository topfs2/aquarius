#pragma once
#include <AL/al.h>

class IDecoder {
public:
    IDecoder() { };
    virtual ~IDecoder() { Close(); };

    virtual bool Open(const std::string &filename) = 0;
    virtual int Read(char *data, size_t length) = 0;
    virtual void Close() { }

    virtual ALuint GetFrequency() = 0;
    virtual ALenum GetFormat() = 0;
    virtual ALuint GetLength() = 0;
};