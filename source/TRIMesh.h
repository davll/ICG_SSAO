#pragma once

#include "Prerequisites.h"

struct TRIMesh{
  std::vector<VertexData> vertex_array;
  std::vector<GLuint> index_array;
  
  float max_vert[3];
  float min_vert[3];
  
  void load(FILE* inFile){
	char tmp_string[256] = "";
	for(int i=0;i<3;i++)
	  max_vert[i]=-1e9, min_vert[i]=1e9;
	
	while(fscanf(inFile,"%s",tmp_string) != EOF){
	  float tmp_double[6];
	  int tmp_int[6];
	  fscanf(inFile,"%d%d%d%d%d%d",&tmp_int[0],&tmp_int[1], &tmp_int[2], &tmp_int[3], &tmp_int[4], &tmp_int[5]);
	  VertexData vt;
	  for(int i=0; i< 3;i++){
		vt.color[i] = tmp_int[i]; // Front Color
	  }
	  for(int i = 0; i < 3; i++){
		fscanf(inFile,"%f%f%f%f%f%f",&tmp_double[0],&tmp_double[1], &tmp_double[2], &tmp_double[3], &tmp_double[4], &tmp_double[5]);
		for(int j=0;j<3;j++){
		  vt.pos[j] = tmp_double[j];
		  vt.normal[j] = tmp_double[j+3];
		  
		  max_vert[j] = std::max(max_vert[j], vt.pos[j]);
		  min_vert[j] = std::min(min_vert[j], vt.pos[j]);
		}
		addIndexWithVertex(vt);
	  }
	}
	
	//printf("max %f %f %f\n", max_vert[0], max_vert[1], max_vert[2]);
	//printf("min %f %f %f\n", min_vert[0], min_vert[1], min_vert[2]);
	
  }
  
  void addIndexWithVertex(const VertexData& a){
	for(unsigned i=0;i<vertex_array.size();i++)
	  if(vertex_array[i] == a){
		index_array.push_back(i);
		return;
	  }
	vertex_array.push_back(a);
	int k = vertex_array.size()-1;
	index_array.push_back(k);
  }
  
  
  
};
