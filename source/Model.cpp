#include "Model.h"
#include <vector>
#include <algorithm>
#include <float.h>
#include "glm.h"
extern "C"{
#include "gli.h"
#include "loadtex.h"
};

void Vertex::setVertex3fv(const GLfloat* v){
  memcpy(vertex, v, sizeof(vertex));
}
void Vertex::setNormal3fv(const GLfloat* n){
  memcpy(normal, n, sizeof(normal));
}
void Vertex::setTexCoord2fv(const GLfloat* tc){
  if(!tc)
    memset(texcoord, 0, sizeof(texcoord));
  else
    memcpy(texcoord, tc, sizeof(texcoord));
}
void Vertex::setColor(float dr, float dg, float db, float sh){
  diffuse[0] = dr;
  diffuse[1] = dg;
  diffuse[2] = db;
  shininess = sh;
}
Vertex::Vertex(){
}
Vertex::Vertex(const Vertex& ob){
  memcpy(this, &ob, sizeof(Vertex));
}
const Vertex& Vertex::operator=(const Vertex& ob){
  memcpy(this, &ob, sizeof(Vertex));
}
bool Vertex::operator==(const Vertex& ob)const{
  return (memcmp(vertex, ob.vertex, sizeof(vertex))==0) && (memcmp(normal, ob.normal, sizeof(normal))==0) && (memcmp(texcoord, ob.texcoord, sizeof(texcoord))==0);
}

Model::Model(){
  glGenBuffers(2, mBuffers);
#if USE_MODEL_VAO
  glGenVertexArrays(1, &mVertexArrayObj);
#endif
  glGenTextures(8, mTextures);
}

Model::~Model(){
  glDeleteTextures(8, mTextures);
#if USE_MODEL_VAO
  glDeleteVertexArrays(1, &mVertexArrayObj);
#endif
  glDeleteBuffers(2, mBuffers);
}

bool Model::load(const char* filename){
#if USE_MODEL_VAO
  glBindVertexArray(0);
#endif
  
  std::vector<Vertex> verts;
  std::vector<GLuint> indis;
  if(!load_impl(filename, verts, indis))
	return false;
  
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
  
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*verts.size(), &verts.front(), GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indis.size(), &indis.front(), GL_STATIC_DRAW);
  
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  
  
#if USE_MODEL_VAO
  glBindVertexArray(mVertexArrayObj);
  apply_vbo();
  glBindVertexArray(0);
#endif
  
  return true;
}

void Model::draw(bool bTextured, int drawOnlyGroup){
#if USE_MODEL_VAO
  glBindVertexArray(mVertexArrayObj);
#else
  apply_vbo();
#endif
  
  glActiveTexture(GL_TEXTURE0);
  
  if(drawOnlyGroup < 0){
    for(int i=0;i<mNumGroups;i++){
	  if(bTextured){
	    glBindTexture(GL_TEXTURE_2D, mTextures[i]);
	  }
      glDrawElements(GL_TRIANGLES, mGroups[i].count, GL_UNSIGNED_INT, (GLvoid*)(mGroups[i].offset*sizeof(GLuint)));
    }
  }else{
    if(bTextured){
	  glBindTexture(GL_TEXTURE_2D, mTextures[drawOnlyGroup]);
	}
	glDrawElements(GL_TRIANGLES, mGroups[drawOnlyGroup].count, GL_UNSIGNED_INT, (GLvoid*)(mGroups[drawOnlyGroup].offset*sizeof(GLuint)));
  }
}

bool Model::load_impl(const char* filename, std::vector<Vertex>& verts, std::vector<GLuint>& indis){
  GLMmodel* model = glmReadOBJ(filename);
  if(!model)
	return false;
  
  minVec = Vector3(+1e9);
  maxVec = Vector3(-1e9);
  
  mNumGroups = 0;
  for(GLMgroup* group = model->groups;group;group = group->next, mNumGroups++){
	mGroups[mNumGroups].count = group->numtriangles * 3;
	mGroups[mNumGroups].offset = indis.size();
	for(int i=0;i<group->numtriangles;i++){
	  GLMtriangle& triangle = model->triangles[group->triangles[i]];
	  
	  Vertex vtx[3];
	  for(int i=0;i<3;i++){
	    vtx[i].setVertex3fv(&model->vertices[3 * triangle.vindices[i]]);
		vtx[i].setNormal3fv(&model->normals[3 * triangle.nindices[i]]);
		if(model->texcoords)
		  vtx[i].setTexCoord2fv(&model->texcoords[2 * triangle.tindices[i]]);
		else
		  vtx[i].setTexCoord2fv(NULL);
		
		if(model->materials){
		  GLMmaterial& material = model->materials[group->material];
		  vtx[i].setColor(material.diffuse[0], material.diffuse[1], material.diffuse[2], material.shininess);
		}else{
		  vtx[i].setColor(1,1,1,1);
		}
		
		Vector3 vx = Vector3(vtx[i].vertex[0], vtx[i].vertex[1], vtx[i].vertex[2]);
		minVec = minPerElem(minVec, vx);
		maxVec = maxPerElem(maxVec, vx);
		
		// add to verts, indis
		if(0){
		  unsigned j;
		  for(j=0;j<verts.size();j++)
			if(verts[j] == vtx[i]){
			  indis.push_back(j);
			  break;
			}
		  if(j>=verts.size()){
		    verts.push_back(vtx[i]);
			int k = verts.size()-1;
			indis.push_back(k);
		  }
		}else{
		  verts.push_back(vtx[i]);
		  int k = verts.size()-1;
		  indis.push_back(k);
		}
	  }
	}
  }
  
  //printf("verts.size = %d\n", verts.size());
  //printf("indis.size = %d\n", indis.size());
  
  glmDelete(model);
  return true;
}

bool Model::loadTexture(int index, const char* filename){
  if(strcmp(strrchr(filename, '.'), ".tga"))
	return false;
  
  gliGenericImage* img = readImage(filename);
  if(!img)
	return false;
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, mTextures[index]);
  
  glTexImage2D(GL_TEXTURE_2D, 0, img->internalFormat, img->width, img->height, 0, img->format, img->type, img->pixels);
  
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  
  gliFree(img);
  
  return true;
}

void Model::apply_vbo(){
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
  glEnableVertexAttribArray(VERTEX_INPUT_POSITION);
  glEnableVertexAttribArray(VERTEX_INPUT_NORMAL);
  glEnableVertexAttribArray(VERTEX_INPUT_TEXCOORD);
  glEnableVertexAttribArray(VERTEX_INPUT_COLOR);
  glVertexAttribPointer(VERTEX_INPUT_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
  glVertexAttribPointer(VERTEX_INPUT_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)12);
  glVertexAttribPointer(VERTEX_INPUT_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)24);
  glVertexAttribPointer(VERTEX_INPUT_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)32);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
}
