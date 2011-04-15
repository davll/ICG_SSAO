#pragma once

#include "ShaderBase.h"

class ShaderSSAO : public ShaderBase{
public:
  ShaderSSAO();
  virtual ~ShaderSSAO();
  
  virtual void use();
  
  void resize(int w, int h);
  void setCameraRange(float znear, float zfar);
  void setAOParams(float cap, float multiplier, float depthTolerance, float range);
  void setAOWeight(float w);
  void setNoiseInputWeight(float w);
  
  float getPixelSizeX()const{ return pixelSizeValue[0]; }
  float getPixelSizeY()const{ return pixelSizeValue[1]; }
private:
  GLint pixelSizeIndex;
  GLfloat pixelSizeValue[2];
  GLint cameraRangeIndex;
  GLfloat cameraRange[2];
  
  GLint aoParametersIndex;
  GLfloat aoParameters[4];
  
  GLint aoWeightIndex;
  GLfloat aoWeight;
  
  GLint noiseInputWeightIndex;
  GLfloat noiseInputWeight;
  
  GLuint randomTexObj;
};
