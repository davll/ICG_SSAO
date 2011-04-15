#pragma once

#include "Prerequisites.h"

class Framebuffers{
public:
  Framebuffers();
  ~Framebuffers();
  
  void resize(int w, int h);
  
  void bindGeomPass();
  void bindSSAOPass();
  void bindFinalPass();
  
private:
  union{
	GLuint mFBOs[2];
	struct{
	  GLuint mGeomPassFBO;
	  GLuint mSSAOPassFBO;
	};
  };
  
  union{
	GLuint mTexs[4];
	struct{
	  GLuint mDepthMap;
	  GLuint mNormalMap;
	  GLuint mAOMap;
	  GLuint mAOMap2;
	};
  };
  
  unsigned mWidth, mHeight;
  
};
