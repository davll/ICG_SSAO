#pragma once

#include "Prerequisites.h"

class ShaderBase{
public:
  ShaderBase(const char* _name, const char* vs_name, const char* ps_name);
  virtual ~ShaderBase();
  
  virtual void use();
  
protected:
  GLuint loadShader(GLenum type, const char* filename);
  
  GLuint mProgram;
};
