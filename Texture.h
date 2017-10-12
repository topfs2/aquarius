#pragma once
#include <GL/glew.h>
#include "entityx/entityx.h"

class CTexture
{
public:
  CTexture(const char *filepath);
  ~CTexture();

  void Bind(int unit = 0);
  void UnBind(int unit = 0);
private:
  GLuint m_texture;
};

typedef entityx::ptr<CTexture> CTexturePtr;
