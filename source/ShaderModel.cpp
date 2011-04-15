#include "ShaderModel.h"


ShaderModel::ShaderModel(const char* pixelShaderFilename):ShaderBase("ModelGPass", "shader/model.vert", pixelShaderFilename){
  projMatrixIndex = glGetUniformLocation(mProgram, "projMatrix");
  mvMatrixIndex = glGetUniformLocation(mProgram, "mvMatrix");
  texture0Index = glGetUniformLocation(mProgram, "texture0");
  glUseProgram(mProgram);
  glUniform1i(glGetUniformLocation(mProgram, "aoMap"), TEXTURE_AO_MAP);
  glUseProgram(0);
}

ShaderModel::~ShaderModel(){
}

void ShaderModel::use(){
  ShaderBase::use();
}

void ShaderModel::applyProjMatrix(const Matrix4& m){
  glUniformMatrix4fv(projMatrixIndex, 1, GL_FALSE, (float*)&m);
}

void ShaderModel::applyModelViewMatrix(const Matrix4& m){
  glUniformMatrix4fv(mvMatrixIndex, 1, GL_FALSE, (float*)&m);
}

void ShaderModel::applyTexture0(int index){
  glUniform1i(texture0Index, index);
}
