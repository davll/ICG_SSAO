#pragma once

#include "Prerequisites.h"
#include "Framebuffers.h"
#include "ShaderCopy.h"
#include "ShaderModel.h"
#include "ShaderSSAO.h"
#include "ShaderBlur.h"
#include "Model.h"

class Render{
public:
  Render();
  ~Render();
  
  void renderScene(bool bSSAO, bool bSSAOBlur, bool bSSAODilate, int output, const Matrix4& view, bool bTextured);
  void resize(int w, int h);
  void loadModel(const char* filename, float rotationX, int base_elem);
  void setModelTexture(int index, const char* filename);
  
  float aoParameters[6];
  
private:
  Framebuffers* mFramebuffers;
  Model* mModel;
  Model* mFSQuad;
  
  ShaderCopy* mShaderCopy;
  ShaderCopyBW* mShaderCopyBW;
  ShaderModel* mShaderModelGPass;
  ShaderModel* mShaderModelFPass;
  ShaderModel* mShaderModelFPassNoTex;
  ShaderSSAO* mShaderSSAO;
  ShaderBlur* mShaderBlur;
  ShaderDilate* mShaderDilate;
  
  Matrix4 mProjMat;
  Matrix4 mModelWorldMat;
  Matrix4 mFloorWorldMat;
};
