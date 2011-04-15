#pragma once

#include "ShaderBase.h"

class ShaderModel : public ShaderBase{
public:
  ShaderModel(const char* pixelShaderFilename);
  virtual ~ShaderModel();
  
  virtual void use();
  void applyProjMatrix(const Matrix4& m);
  void applyModelViewMatrix(const Matrix4& m);
  void applyTexture0(int index);
  //void applyDiffuseColor(const Vector3& c);
  //void applyDiffuseDirection(const Vector3& c);
  
private:
  GLint projMatrixIndex;
  GLint mvMatrixIndex;
  GLint texture0Index;
};
