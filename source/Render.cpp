#include "Render.h"

Render::Render(){
  mFramebuffers = 0;
  mModel = 0;
  mFSQuad = 0;
  
  if(!GLEW_VERSION_2_0){
    puts("OpenGL 2.0 Required");
    abort();
  }
  if(!GLEW_ARB_framebuffer_object){
    if(GLEW_EXT_framebuffer_object){
      glFramebufferTexture2D = glFramebufferTexture2DEXT;
      glBindFramebuffer = glBindFramebufferEXT;
      glGenFramebuffers = glGenFramebuffersEXT;
      glDeleteFramebuffers = glDeleteFramebuffersEXT;
      glCheckFramebufferStatus = glCheckFramebufferStatusEXT;
      glGenerateMipmap = glGenerateMipmapEXT;
    }else{
      puts("ARB_framebuffer_object or EXT_framebuffer_object Required");
      abort();
    }
  }
  if(!GLEW_ARB_vertex_array_object){
    if(GLEW_APPLE_vertex_array_object){
      glGenVertexArrays = glGenVertexArraysAPPLE;
      glDeleteVertexArrays = glDeleteVertexArraysAPPLE;
      glBindVertexArray = glBindVertexArrayAPPLE;
      glIsVertexArray = glIsVertexArrayAPPLE;
    }else{
      puts("GL_ARB_vertex_array_object Required.");
      abort();
    }
  }
#if USE_AO_MAP_R8
  if(!GLEW_ARB_texture_rg){
    puts("GL_ARB_texture_rg Required");
    abort();
  }
#endif
#if USE_PACKED_DEPTH_STENCIL
  if(!GLEW_EXT_packed_depth_stencil && !GLEW_ARB_framebuffer_object){
    puts("GL_EXT_packed_depth_stencil required.");
    abort();
  }
#endif
  
  mFramebuffers = new Framebuffers();
  mShaderCopy = new ShaderCopy();
  mShaderCopyBW = new ShaderCopyBW();
  mShaderModelGPass = new ShaderModel("shader/model-gpass.frag");
  mShaderModelFPass = new ShaderModel("shader/model-fpass.frag");
  mShaderModelFPassNoTex = new ShaderModel("shader/model-fpass-notex.frag");
  mShaderSSAO = new ShaderSSAO();
  mShaderBlur = new ShaderBlur();
  mShaderDilate = new ShaderDilate();
  
  mFSQuad = new Model();
  //mFSQuad->loadFSQuad();
  if(!mFSQuad->load("models/fsquad.obj")){
    abort();
  }
  
  mModel = new Model();
  
  aoParameters[0] = 2.0f;
  aoParameters[1] = 100.0f;
  aoParameters[2] = 0.0f;
  aoParameters[3] = 4.0f;
  aoParameters[4] = 2.0f;
  aoParameters[5] = 0.0f;
}

Render::~Render(){
  delete mModel;
  delete mFSQuad;
  
  delete mShaderDilate;
  delete mShaderBlur;
  delete mShaderSSAO;
  delete mShaderModelFPassNoTex;
  delete mShaderModelFPass;
  delete mShaderModelGPass;
  delete mShaderCopyBW;
  delete mShaderCopy;
  delete mFramebuffers;
}

void Render::renderScene(bool bSSAO, bool bSSAOBlur, bool bSSAODilate, int output, const Matrix4& view, bool bTextured){
  glEnable(GL_DEPTH_TEST);
  
  Matrix4 floorMV = view * mFloorWorldMat;
  Matrix4 modelMV = view * mModelWorldMat;
  
  mFramebuffers->bindGeomPass();
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mShaderModelGPass->use();
  mShaderModelGPass->applyProjMatrix(mProjMat);
  {
    mShaderModelGPass->applyModelViewMatrix(floorMV);
    mFSQuad->draw();
    mShaderModelGPass->applyModelViewMatrix(modelMV);
    mModel->draw();
  }
  
  glDisable(GL_DEPTH_TEST);
  
  mFramebuffers->bindSSAOPass();
  glClearColor(1, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  if(bSSAO){
    mShaderSSAO->setAOParams(aoParameters[0], aoParameters[1], aoParameters[2], aoParameters[3]);
    mShaderSSAO->setAOWeight(aoParameters[4]);
    mShaderSSAO->setNoiseInputWeight(aoParameters[5]);
    
    mShaderSSAO->use();
    mFSQuad->draw();
    
    if(bSSAODilate){
      mShaderDilate->use();
      
      glDrawBuffer(GL_COLOR_ATTACHMENT1);
      mShaderDilate->applySrcMap(TEXTURE_AO_MAP);
      mShaderDilate->applySamples(0.5f, 1.0f, 0.0f, 0.0f, mShaderSSAO->getPixelSizeX(), 0.0f);
      mFSQuad->draw();
      
      glDrawBuffer(GL_COLOR_ATTACHMENT0);
      mShaderDilate->applySrcMap(TEXTURE_AO_MAP_2);
      mShaderDilate->applySamples(0.5f, 1.0f, 0.0f, 0.0f, 0.0f, mShaderSSAO->getPixelSizeY());
      mFSQuad->draw();
    }
    
    if(bSSAOBlur){
      mShaderBlur->setKernelWeights(0.16, 0.15, 0.12, 0.09, 0.05);
      mShaderBlur->use();
      
      glDrawBuffer(GL_COLOR_ATTACHMENT1);
      mShaderBlur->applySrcMap(TEXTURE_AO_MAP);
      mShaderBlur->applyKernelSamples(1.0f, 2.0f, 3.0f, 4.0f, mShaderSSAO->getPixelSizeX(), 0.0f);
      mFSQuad->draw();
      
      glDrawBuffer(GL_COLOR_ATTACHMENT0);
      mShaderBlur->applySrcMap(TEXTURE_AO_MAP_2);
      mShaderBlur->applyKernelSamples(1.0f, 2.0f, 3.0f, 4.0f, 0.0f, mShaderSSAO->getPixelSizeY());
      mFSQuad->draw();
    }
  }
  
  mFramebuffers->bindFinalPass();
  glClearColor(0.5, 0.5, 0.7, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  switch(output){
    case 1: // Show AO map
      mShaderCopyBW->setSourceTextureStage(TEXTURE_AO_MAP);
      mShaderCopyBW->use();
      mFSQuad->draw();
      break;
    case 2: // Show Depth map
      mShaderCopyBW->setSourceTextureStage(TEXTURE_DEPTH_MAP);
      mShaderCopyBW->use();
      mFSQuad->draw();
      break;
    case 3: // Show Normal map
      mShaderCopy->setSourceTextureStage(TEXTURE_NORMAL_MAP);
      mShaderCopy->use();
      mFSQuad->draw();
      break;
    case 4: // Texture0 Debug
      mShaderCopy->setSourceTextureStage(0);
      mShaderCopy->use();
      mFSQuad->draw();
      break;
    default: // Draw Model with AO
      if(1){
        glEnable(GL_DEPTH_TEST);
        mShaderModelFPassNoTex->use();
        mShaderModelFPassNoTex->applyProjMatrix(mProjMat);
        mShaderModelFPassNoTex->applyModelViewMatrix(floorMV);
        mFSQuad->draw();
        
        if(bTextured){
          mShaderModelFPass->use();
          mShaderModelFPass->applyTexture0(0);
          mShaderModelFPass->applyProjMatrix(mProjMat);
          mShaderModelFPass->applyModelViewMatrix(modelMV);
          mModel->draw(true);
        }else{
          mShaderModelFPassNoTex->applyModelViewMatrix(modelMV);
          mModel->draw();
        }
        
      }
      break;
  };
  
  glUseProgram(0);
  
}

void Render::resize(int w, int h){
  mFramebuffers->resize(w, h);
  mShaderSSAO->resize(w, h);
  
  float znear = 1.0f;
  float zfar = 200.0f;
  float aspect = (float)w / (float)h;
  mProjMat = Matrix4::perspective(0.7854f, aspect, znear, zfar);
  mShaderSSAO->setCameraRange(znear, zfar);
  
}

void Render::loadModel(const char* filename, float rotationX, int base_elem){
  if(!mModel->load(filename)){
    printf("Load %s failed.\n", filename);
    abort();
  }
  
  Vector3 digon = mModel->vecMax()-mModel->vecMin();
  float floor_z = fabs(digon.getElem(base_elem))*0.5f;
  Vector3 center = (mModel->vecMax()+mModel->vecMin()) * 0.5f;
  float radius = length(digon) * 0.5f;
  mModelWorldMat = Matrix4::rotationX(rotationX) * Matrix4::scale(Vector3(10.0f / radius)) * Matrix4::translation(-center);
  floor_z = (floor_z * 10.0f / radius);
  floor_z += 0.001f;
  mFloorWorldMat = Matrix4::translation(Vector3(0.0f, 0.0f, -floor_z))*Matrix4::scale(Vector3(10.0f));
}

void Render::setModelTexture(int index, const char* filename){
  if(!mModel->loadTexture(index, filename)){
    printf("load %s failed.\n", filename);
    abort();
  }
}
