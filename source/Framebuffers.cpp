#include "Framebuffers.h"

#define TEXTURE_TARGET GL_TEXTURE_2D

Framebuffers::Framebuffers(){
  glGenTextures(4, mTexs);
  glGenFramebuffers(2, mFBOs);
}

Framebuffers::~Framebuffers(){
  glDeleteFramebuffers(2, mFBOs);
  glDeleteTextures(4, mTexs);
}

void Framebuffers::resize(int w, int h){
  glViewport(0, 0, w, h);
  mWidth = w, mHeight = h;
  
  float whiteColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  
  glBindTexture(TEXTURE_TARGET, mDepthMap);
#if USE_PACKED_DEPTH_STENCIL
  glTexImage2D(TEXTURE_TARGET, 0, GL_DEPTH24_STENCIL8, w, h, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
#else
  glTexImage2D(TEXTURE_TARGET, 0, GL_DEPTH_COMPONENT32, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
#endif
  glTexParameteri(TEXTURE_TARGET, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(TEXTURE_TARGET, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(TEXTURE_TARGET, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(TEXTURE_TARGET, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameterfv(TEXTURE_TARGET, GL_TEXTURE_BORDER_COLOR, whiteColor);
  
  glBindTexture(TEXTURE_TARGET, mNormalMap);
  glTexImage2D(TEXTURE_TARGET, 0, GL_RGBA8, w, h, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, NULL);
  //glTexImage2D(TEXTURE_TARGET, 0, GL_RGBA16F, w, h, 0, GL_BGRA, GL_HALF_FLOAT, NULL);
  glTexParameteri(TEXTURE_TARGET, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(TEXTURE_TARGET, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(TEXTURE_TARGET, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(TEXTURE_TARGET, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  
  glBindTexture(TEXTURE_TARGET, mAOMap);
  const int aw = (USE_AO_MAP_HALF_RESOLUTION?w/2:w);
  const int ah = (USE_AO_MAP_HALF_RESOLUTION?h/2:h);
#if USE_AO_MAP_R8
  glTexImage2D(TEXTURE_TARGET, 0, GL_R8, aw, ah, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
#else
  glTexImage2D(TEXTURE_TARGET, 0, GL_RGBA8, aw, ah, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
#endif
  glTexParameteri(TEXTURE_TARGET, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(TEXTURE_TARGET, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(TEXTURE_TARGET, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(TEXTURE_TARGET, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameterfv(TEXTURE_TARGET, GL_TEXTURE_BORDER_COLOR, whiteColor);
  
  glBindTexture(TEXTURE_TARGET, mAOMap2);
#if USE_AO_MAP_R8
  glTexImage2D(TEXTURE_TARGET, 0, GL_R8, aw, ah, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
#else
  glTexImage2D(TEXTURE_TARGET, 0, GL_RGBA8, aw, ah, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
#endif
  glTexParameteri(TEXTURE_TARGET, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(TEXTURE_TARGET, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(TEXTURE_TARGET, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(TEXTURE_TARGET, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameterfv(TEXTURE_TARGET, GL_TEXTURE_BORDER_COLOR, whiteColor);
  
  glBindTexture(TEXTURE_TARGET, 0);
  
  glBindFramebuffer(GL_FRAMEBUFFER, mGeomPassFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TEXTURE_TARGET, mNormalMap, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, TEXTURE_TARGET, mDepthMap, 0);
#if USE_PACKED_DEPTH_STENCIL
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, TEXTURE_TARGET, mDepthMap, 0);
#endif
  //printf("G-Pass FBO: %d\n", glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_COMPLETE);
  
  glBindFramebuffer(GL_FRAMEBUFFER, mSSAOPassFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TEXTURE_TARGET, mAOMap, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, TEXTURE_TARGET, mAOMap2, 0);
  //printf("SSAO-Pass FBO: %d\n", glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_COMPLETE);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffers::bindGeomPass(){
  GLenum dbs[1] = { GL_COLOR_ATTACHMENT0 };
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGeomPassFBO);
  glDrawBuffers(1, dbs);
}

void Framebuffers::bindSSAOPass(){
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOPassFBO);
  glDrawBuffer(GL_COLOR_ATTACHMENT0);
  
  if(USE_AO_MAP_HALF_RESOLUTION)
	glViewport(0, 0, mWidth/2, mHeight/2);
  
  glActiveTexture(GL_TEXTURE0 + TEXTURE_DEPTH_MAP);
  glBindTexture(TEXTURE_TARGET, mDepthMap);
  glActiveTexture(GL_TEXTURE0 + TEXTURE_NORMAL_MAP);
  glBindTexture(TEXTURE_TARGET, mNormalMap);
  glActiveTexture(GL_TEXTURE0 + TEXTURE_AO_MAP);
  glBindTexture(TEXTURE_TARGET, mAOMap);
  glActiveTexture(GL_TEXTURE0 + TEXTURE_AO_MAP_2);
  glBindTexture(TEXTURE_TARGET, mAOMap2);
}

void Framebuffers::bindFinalPass(){
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glDrawBuffer(GL_BACK);
  if(USE_AO_MAP_HALF_RESOLUTION)
	glViewport(0, 0, mWidth, mHeight);
}
