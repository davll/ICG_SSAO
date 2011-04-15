#include "ShaderBlur.h"

ShaderBlur::ShaderBlur() : ShaderBase("PBLUR", "shader/quad.vert", "shader/blur.frag"){
  kernelSamplesIndex = glGetUniformLocation(mProgram, "kernelSamples");
  kernelWeightsIndex = glGetUniformLocation(mProgram, "kernelWeights");
  srcMapIndex = glGetUniformLocation(mProgram, "srcMap");
}

ShaderBlur::~ShaderBlur(){
}

void ShaderBlur::use(){
  ShaderBase::use();
  glUniform1fv(kernelWeightsIndex, 5, kernelWeights);
}

void ShaderBlur::applyKernelSamples(float x0, float x1, float x2, float x3, float dx, float dy){
  kernelSamples[0][0] = x0 * dx, kernelSamples[0][1] = x0 * dy;
  kernelSamples[1][0] = x1 * dx, kernelSamples[1][1] = x1 * dy;
  kernelSamples[2][0] = x2 * dx, kernelSamples[2][1] = x2 * dy;
  kernelSamples[3][0] = x3 * dx, kernelSamples[3][1] = x3 * dy;
  glUniform2fv(kernelSamplesIndex, 4, (float*)kernelSamples);
}

void ShaderBlur::setKernelWeights(float wc, float w0, float w1, float w2, float w3){
  kernelWeights[0] = wc;
  kernelWeights[1] = w0;
  kernelWeights[2] = w1;
  kernelWeights[3] = w2;
  kernelWeights[4] = w3;
}

void ShaderBlur::applySrcMap(int index){
  glUniform1i(srcMapIndex, index);
}

ShaderDilate::ShaderDilate() : ShaderBase("PBLUR", "shader/quad.vert", "shader/dilate.frag"){
  samplesIndex = glGetUniformLocation(mProgram, "samples");
  srcMapIndex = glGetUniformLocation(mProgram, "srcMap");
}

ShaderDilate::~ShaderDilate(){
}

void ShaderDilate::applySamples(float x0, float x1, float x2, float x3, float dx, float dy){
  samples[0][0] = x0 * dx, samples[0][1] = x0 * dy;
  samples[1][0] = x1 * dx, samples[1][1] = x1 * dy;
  samples[2][0] = x2 * dx, samples[2][1] = x2 * dy;
  samples[3][0] = x3 * dx, samples[3][1] = x3 * dy;
  glUniform2fv(samplesIndex, 4, (float*)samples);
}

void ShaderDilate::applySrcMap(int index){
  glUniform1i(srcMapIndex, index);
}
