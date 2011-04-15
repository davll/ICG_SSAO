#pragma once

#include "Prerequisites.h"
#include <vector>
#include <algorithm>

struct Vertex{
  GLfloat vertex[3];
  GLfloat normal[3];
  GLfloat texcoord[2];
  GLfloat diffuse[3];
  GLfloat shininess;
  
  void setVertex3fv(const GLfloat* v);
  void setNormal3fv(const GLfloat* n);
  void setTexCoord2fv(const GLfloat* tc);
  void setColor(float dr, float dg, float db, float sh);
  
  Vertex();
  Vertex(const Vertex& ob);
  const Vertex& operator=(const Vertex& ob);
  bool operator==(const Vertex& ob)const;
};

class Model{
public:
  Model();
  ~Model();
  
  bool load(const char* filename);
  void draw(bool bTextured = false, int drawOnlyGroup = -1);
  
  const Vector3& vecMin()const{ return minVec; }
  const Vector3& vecMax()const{ return maxVec; }
  
  bool loadTexture(int index, const char* filename);
  
private:
  bool load_impl(const char* filename, std::vector<Vertex>& verts, std::vector<GLuint>& indis);
  void apply_vbo();
  
  Vector3 minVec;
  Vector3 maxVec;
  
#if USE_MODEL_VAO
  GLuint mVertexArrayObj;
#endif
  union{
    GLuint mBuffers[2];
    struct{
      GLuint mVertexBuffer;
      GLuint mIndexBuffer;
    };
  };
  
  struct Group{
	int count;
	int offset;
  }mGroups[64];
  int mNumGroups;
  
  GLuint mTextures[64];
};
