#pragma once

#include "Prerequisites.h"

class HUDConsole{
public:
  HUDConsole();
  ~HUDConsole();
  
  void resize(int w, int h);
  void begin();
  void end();
  void putline(const char* s);
  void println(const char* fmt, ...);
  
private:
  Matrix4 mOrthoMatrix;
  int mWidth, mHeight;
  int mYCursor;
};


