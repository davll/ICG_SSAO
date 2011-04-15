#pragma once

#include "ShaderBase.h"

class ShaderCopy : public ShaderBase{
public:
  ShaderCopy();
  virtual ~ShaderCopy();
  
  virtual void use();
  void setSourceTextureStage(int i);
  
private:
  GLint sourceTexIndex;
  GLint sourceTexValue;
};

class ShaderCopyBW : public ShaderBase{
public:
  ShaderCopyBW();
  virtual ~ShaderCopyBW();
  
  virtual void use();
  void setSourceTextureStage(int i);
  
private:
  GLint sourceTexIndex;
  GLint sourceTexValue;
};
