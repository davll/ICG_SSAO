#include "HUDConsole.h"
#include <stdarg.h>

HUDConsole::HUDConsole(){
}

HUDConsole::~HUDConsole(){
}

void HUDConsole::resize(int w, int h){
  mWidth = w, mHeight = h;
  mOrthoMatrix = Matrix4::orthographic(0.0f, w, 0.0f, h, -1.0f, 1.0f);
}

void HUDConsole::begin(){
  mYCursor = mHeight;
  
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf((float*)&mOrthoMatrix);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  glDisable(GL_DEPTH_TEST);
}

void HUDConsole::end(){
}

void HUDConsole::putline(const char* s){
  mYCursor -= 20;
  
  int twidth = glutBitmapLength(GLUT_BITMAP_9_BY_15, (const GLubyte*)s);
  
  glColor4f(0.0, 0.0, 0.0, 0.7);
  glRecti(10, mYCursor-5, twidth + 28, mYCursor+15);
  
  glColor4f(1.0, 1.0, 1.0, 1.0);
  glRasterPos3i(18, mYCursor, 0);
  for(const char* p=s; *p; p++)
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *p);
}

void HUDConsole::println(const char* fmt, ...){
  char buffer[1024] = {0};
  va_list vlist;
  va_start(vlist, fmt);
  vsprintf(buffer, fmt, vlist);
  va_end(vlist);
  putline(buffer);
}
