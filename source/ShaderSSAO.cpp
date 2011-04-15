#include "ShaderSSAO.h"
extern "C"{
#include "gli.h"
#include "loadtex.h"
};

ShaderSSAO::ShaderSSAO():ShaderBase("PSSAO", "shader/quad.vert", "shader/ssao.frag"){
  glUseProgram(mProgram);
  glUniform1i(glGetUniformLocation(mProgram, "depthMap"), TEXTURE_DEPTH_MAP);
  glUniform1i(glGetUniformLocation(mProgram, "normalMap"), TEXTURE_NORMAL_MAP);
  glUniform1i(glGetUniformLocation(mProgram, "randomTex"), TEXTURE_RANDOM_TEX);
  glUseProgram(0);
  
  pixelSizeIndex = glGetUniformLocation(mProgram, "pixelSize");
  cameraRangeIndex = glGetUniformLocation(mProgram, "cameraRange");
  aoParametersIndex = glGetUniformLocation(mProgram, "aoParameters");
  aoWeightIndex = glGetUniformLocation(mProgram, "aoWeight");
  noiseInputWeightIndex = glGetUniformLocation(mProgram, "noiseInputWeight");
  
  glGenTextures(1, &randomTexObj);
  
  gliGenericImage* img = readImage("shader/noise.tga");
  if(!img){
	puts("ERROR! cannot load random texture");
	abort();
  }
  
  glBindTexture(GL_TEXTURE_2D, randomTexObj);
  glTexImage2D(GL_TEXTURE_2D, 0, img->internalFormat, img->width, img->height, 0, img->format, img->type, img->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
  gliFree(img);
}

ShaderSSAO::~ShaderSSAO(){
  glDeleteTextures(1, &randomTexObj);
}

void ShaderSSAO::use(){
  ShaderBase::use();
  
  glActiveTexture(GL_TEXTURE0 + TEXTURE_RANDOM_TEX);
  glBindTexture(GL_TEXTURE_2D, randomTexObj);
  
  glUniform2fv(pixelSizeIndex, 1, pixelSizeValue);
  glUniform2fv(cameraRangeIndex, 1, cameraRange);
  glUniform4fv(aoParametersIndex, 1, aoParameters);
  glUniform1f(aoWeightIndex, aoWeight);
  glUniform1f(noiseInputWeightIndex, noiseInputWeight);
}

void ShaderSSAO::resize(int w, int h){
  pixelSizeValue[0] = 1.0f / float(w);
  pixelSizeValue[1] = 1.0f / float(h);
}

void ShaderSSAO::setCameraRange(float znear, float zfar){
  cameraRange[0] = znear;
  cameraRange[1] = zfar;
}

void ShaderSSAO::setAOParams(float cap, float multiplier, float depthTolerance, float range){
  aoParameters[0] = cap;
  aoParameters[1] = multiplier;
  aoParameters[2] = depthTolerance;
  aoParameters[3] = range;
}

void ShaderSSAO::setAOWeight(float w){
  aoWeight = w;
}
void ShaderSSAO::setNoiseInputWeight(float w){
  noiseInputWeight = w;
}
