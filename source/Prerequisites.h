#pragma once

#if defined(__APPLE__)
#  define OS_MAC
#elif defined(_WIN32)||defined(_WIN64)
#  define OS_WIN
#  define WIN32_LEAN_AND_MEAN
#  define NOMINMAX
#else
#  error UNKOWN OS
#endif

/* INCLUDE OPENGL */

extern "C"{

#include "GLEW/glew.h"
#if defined(OS_WIN)
#  include "GLEW/wglew.h"
#endif


};


#ifdef __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#  include <unistd.h>
#else
#  define GLUT_NO_LIB_PRAGMA
#  include <GL/glut.h>
#endif

/* Sony Vectormath */
#include "vectormath/cpp/vectormath_aos.h"
using namespace Vectormath;
using namespace Vectormath::Aos;

/* Standard Headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Configure */

#define USE_PACKED_DEPTH_STENCIL	0
#define USE_AO_MAP_R8				0
#define USE_MODEL_VAO				0
#define USE_AO_MAP_HALF_RESOLUTION	0

enum{
  VERTEX_INPUT_POSITION = 0,
  VERTEX_INPUT_NORMAL = 1,
  VERTEX_INPUT_TEXCOORD = 2,
  VERTEX_INPUT_COLOR = 3,
};

enum{
  TEXTURE_RANDOM_TEX = 3,
  TEXTURE_DEPTH_MAP = 4,
  TEXTURE_NORMAL_MAP = 5,
  TEXTURE_AO_MAP = 6,
  TEXTURE_AO_MAP_2 = 7
};
