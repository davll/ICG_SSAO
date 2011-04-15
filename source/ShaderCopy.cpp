#include "ShaderCopy.h"

ShaderCopy::ShaderCopy():ShaderBase("FSCopy", "shader/quad.vert", "shader/copy.frag"){
  sourceTexIndex = glGetUniformLocation(mProgram, "sourceTex");
}

ShaderCopy::~ShaderCopy(){
}

void ShaderCopy::use(){
  ShaderBase::use();
  glUniform1i(sourceTexIndex, sourceTexValue);
}

void ShaderCopy::setSourceTextureStage(int i){
  sourceTexValue = i;
}

ShaderCopyBW::ShaderCopyBW():ShaderBase("FSCopyBW", "shader/quad.vert", "shader/copybw.frag"){
  sourceTexIndex = glGetUniformLocation(mProgram, "sourceTex");
}

ShaderCopyBW::~ShaderCopyBW(){
}

void ShaderCopyBW::use(){
  ShaderBase::use();
  glUniform1i(sourceTexIndex, sourceTexValue);
}

void ShaderCopyBW::setSourceTextureStage(int i){
  sourceTexValue = i;
}
