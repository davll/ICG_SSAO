#pragma once

#include "ShaderBase.h"

class ShaderBlur : public ShaderBase{
public:
  ShaderBlur();
  virtual ~ShaderBlur();
  
  virtual void use();
  
  void applyKernelSamples(float x0, float x1, float x2, float x3, float dx, float dy);
  void setKernelWeights(float wc, float w0, float w1, float w2, float w3);
  void applySrcMap(int index);
  
private:
  GLint kernelSamplesIndex;
  GLfloat kernelSamples[4][2];
  GLint kernelWeightsIndex;
  GLfloat kernelWeights[5];
  GLint srcMapIndex;
};

class ShaderDilate : public ShaderBase{
public:
  ShaderDilate();
  virtual ~ShaderDilate();
  
  void applySamples(float x0, float x1, float x2, float x3, float dx, float dy);
  void applySrcMap(int index);
private:
  GLint samplesIndex;
  GLfloat samples[4][2];
  GLint srcMapIndex;
};
