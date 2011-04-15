#include "ShaderBase.h"
#include "Model.h"

ShaderBase::ShaderBase(const char* _name, const char* vs_name, const char* ps_name){
  GLuint vs = loadShader(GL_VERTEX_SHADER, vs_name);
  GLuint ps = loadShader(GL_FRAGMENT_SHADER, ps_name);
  
  if(vs && ps){
	mProgram = glCreateProgram();
	glAttachShader(mProgram, vs);
	glAttachShader(mProgram, ps);
	
	if(glGetAttribLocation(mProgram, "inVertex")>=0)
      glBindAttribLocation(mProgram, VERTEX_INPUT_POSITION, "inVertex");
	if(glGetAttribLocation(mProgram, "inNormal")>=0)
	  glBindAttribLocation(mProgram, VERTEX_INPUT_NORMAL, "inNormal");
	if(glGetAttribLocation(mProgram, "inTexCoord")>=0)
	  glBindAttribLocation(mProgram, VERTEX_INPUT_TEXCOORD, "inTexCoord");
	if(glGetAttribLocation(mProgram, "inColor")>=0)
	  glBindAttribLocation(mProgram, VERTEX_INPUT_COLOR, "inColor");
	
	glLinkProgram(mProgram);
	
	GLint result;
	glGetProgramiv(mProgram, GL_LINK_STATUS, &result);
	if(result == GL_FALSE){
	  GLchar infolog[1024] = {0};
	  glGetProgramInfoLog(mProgram, 1023, NULL, infolog);
	  printf("LinkERROR:%s:\n%s\n", _name, infolog);
	}
  }else{
	puts("Program is not created.");
	mProgram = 0;
  }
  
  glDeleteShader(vs);
  glDeleteShader(ps);
  
}

ShaderBase::~ShaderBase(){
  if(mProgram)
	glDeleteProgram(mProgram);
}

void ShaderBase::use(){
  glUseProgram(mProgram);
}

GLuint ShaderBase::loadShader(GLenum type, const char* filename){
  GLuint obj = 0;
  
  {
	FILE *fp = fopen(filename, "rb");
	if(!fp){
	  puts("File not found.");
	  return 0;
	}
	
	obj = glCreateShader(type);
	
	fseek(fp, 0, SEEK_END);
	int sz = ftell(fp);
	GLchar* buf = (GLchar*)malloc(sz+1);
	
	fseek(fp, 0, SEEK_SET);
	fread(buf, 1, sz, fp);
	buf[sz] = 0;
	
	glShaderSource(obj, 1, (const GLchar**)&buf, NULL);
	
	free(buf);
    fclose(fp);
  }
  if(!obj)
	return 0;
  
  GLchar infolog[1024] = {0};
  glCompileShader(obj);
  GLint result;
  glGetShaderiv(obj, GL_COMPILE_STATUS, &result);
  if(result == GL_FALSE){
	glGetShaderInfoLog(obj, 1023, NULL, infolog);
	printf("Compile ERROR:%s:\n%s\n",filename,infolog);
	
	glDeleteShader(obj);
	obj = 0;
  }
  return obj;
}
