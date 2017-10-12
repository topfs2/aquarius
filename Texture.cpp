#include "Texture.h"
#include "Log.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using namespace logging;

CTexture::CTexture(const char *filepath) : m_texture(0)
{
    LOG(INFO, "Loading " << filepath);

    SDL_Surface *surface = IMG_Load(filepath);
    if (surface)
    {
        GLint bpp = surface->format->BytesPerPixel;
        GLenum texture_format = 0;
        if (bpp == 4)
            texture_format = (surface->format->Rmask == 0x000000ff) ? GL_RGBA : GL_BGRA;
        else if (bpp == 3)
            texture_format = (surface->format->Rmask == 0x000000ff) ? GL_RGB : GL_BGR;

        if (texture_format)
        { 
            glGenTextures(1, &m_texture);
            glBindTexture(GL_TEXTURE_2D, m_texture);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTexImage2D(GL_TEXTURE_2D, 0, bpp, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);
        }

        SDL_FreeSurface(surface);
    }
    else
    {
        LOG(WARNING, "Failed to load " << filepath);
    }
}

CTexture::~CTexture()
{
    glDeleteTextures(1, &m_texture);
    m_texture = 0;
}

void CTexture::Bind(int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

void CTexture::UnBind(int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, 0);
}
