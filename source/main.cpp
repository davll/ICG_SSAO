#include "Render.h"
#include "HUDConsole.h"
#if defined(_MSC_VER)
#  include <direct.h>
#endif

static int gFinalPassSwitch = 0;
static bool gToggleSSAO = true;
static bool gToggleBlur = false;
static bool gToggleDilate = false;

static char gModelFileList[16][128];
static float gModelFileRotX[16];
static int gModelFileBaseLine[16];
static int gModelFileNumTextures[16];
static char gModelFileTextureFilenames[16][8][128];
static int gModelSwitch = 0;
static char gModelListSize = 0;

static Matrix4 gViewMatrix;

static Render* gRender = 0;
static HUDConsole gHUDC;

static void resetView(){
  gViewMatrix = Matrix4::lookAt(Point3(0.0f, -30.0f, 00.0f), Point3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
}

static void onDisplay(){
}

static void onIdle(){
  gRender->renderScene(gToggleSSAO, gToggleBlur, gToggleDilate, gFinalPassSwitch, gViewMatrix, gModelFileNumTextures[gModelSwitch]>0);
  
  const char* strBoolean[] = { "NO", "YES" };
  const char* strSwitch[] = { "Final", "AO", "Depth", "Normal" };
  
  gHUDC.begin();
  gHUDC.println("Output                [1]: %s", strSwitch[gFinalPassSwitch]);
  gHUDC.println("SSAO                  [2]: %s", strBoolean[gToggleSSAO]);
  gHUDC.println("Dilate filter of SSAO [3]: %s", strBoolean[gToggleDilate]);
  gHUDC.println("Blur filter of SSAO   [4]: %s", strBoolean[gToggleBlur]);
  gHUDC.println("[w/s/a/d]: move camera");
  gHUDC.println("[r/f]    : move up/down camera");
  gHUDC.println("[q]      : reset camera");
  gHUDC.println("Model [z]: %s", gModelFileList[gModelSwitch]);
  gHUDC.println("AO Cap              [5/t]: %f", gRender->aoParameters[0]);
  gHUDC.println("AO Multiplier       [6/y]: %f", gRender->aoParameters[1]);
  gHUDC.println("AO depth tolerance  [7/u]: %f", gRender->aoParameters[2]);
  gHUDC.println("AO Z Range          [8/i]: %f", gRender->aoParameters[3]);
  gHUDC.println("AO Weight           [9/o]: %f", gRender->aoParameters[4]);
  gHUDC.println("Noise weight        [0/p]: %f", gRender->aoParameters[5]);
  gHUDC.end();
  
  glutSwapBuffers();
}

static void onReshape(int width, int height){
  gRender->resize(width, height);
  
  gHUDC.resize(width, height);
}

static void loadModel(){
  const char* filename = gModelFileList[gModelSwitch];
  float rotX = gModelFileRotX[gModelSwitch];
  int baseL = gModelFileBaseLine[gModelSwitch];
  
  gRender->loadModel(filename, rotX, baseL);
  for(int i=0;i<gModelFileNumTextures[gModelSwitch];i++){
    gRender->setModelTexture(i, gModelFileTextureFilenames[gModelSwitch][i]);
  }
}

static void onKeyDown(unsigned char k, int x, int y){
  switch(k){
    case '1':
      gFinalPassSwitch = (gFinalPassSwitch+1)%4;
      break;
    case '2':
      gToggleSSAO = !gToggleSSAO;
      break;
    case '3':
      gToggleDilate = !gToggleDilate;
      break;
    case '4':
      gToggleBlur = !gToggleBlur;
      break;
    case 'z':
      gModelSwitch = (gModelSwitch + 1) % gModelListSize;
      loadModel();
      break;
    case 'q':
      resetView();
      break;
    case 'w':
      gViewMatrix = Matrix4::translation(Vector3(0.0f, 0.0f, 1.0f)) * gViewMatrix;
      break;
    case 's':
      gViewMatrix = Matrix4::translation(Vector3(0.0f, 0.0f, -1.0f)) * gViewMatrix;
      break;
    case 'a':
      gViewMatrix = Matrix4::translation(Vector3(1.0f, 0.0f, 0.0f)) * gViewMatrix;
      break;
    case 'd':
      gViewMatrix = Matrix4::translation(Vector3(-1.0f, 0.0f, 0.0f)) * gViewMatrix;
      break;
    case 'r':
      gViewMatrix = Matrix4::translation(Vector3(0.0f, -1.0f, 0.0f)) * gViewMatrix;
      break;
    case 'f':
      gViewMatrix = Matrix4::translation(Vector3(0.0f, 1.0f, 0.0f)) * gViewMatrix;
      break;
    case '5':
      gRender->aoParameters[0] += 0.1;
      break;
    case 't':
      gRender->aoParameters[0] -= 0.1;
      break;
    case '6':
      gRender->aoParameters[1] += 0.1;
      break;
    case 'y':
      gRender->aoParameters[1] -= 0.1;
      break;
    case '7':
      gRender->aoParameters[2] += 0.001;
      break;
    case 'u':
      gRender->aoParameters[2] -= 0.001;
      break;
    case '8':
      gRender->aoParameters[3] += 0.05;
      break;
    case 'i':
      gRender->aoParameters[3] -= 0.05;
      break;
    case '9':
      gRender->aoParameters[4] += 0.1;
      break;
    case 'o':
      gRender->aoParameters[4] -= 0.1;
      break;
    case '0':
      gRender->aoParameters[5] += 0.1;
      break;
    case 'p':
      gRender->aoParameters[5] -= 0.1;
      break;
  }
}

static void onSpecialDown(int k, int x, int y){
}

static void _onMouseTracking(bool pressed, int x, int y){
  static int tx = x, ty = y;
  float dx = x - tx, dy = y - ty;
  dx *= 0.001, dy *= 0.001;
  if(pressed){
    Matrix4 tmp = Matrix4::rotationY(dx) * Matrix4::rotationX(dy);
    gViewMatrix = tmp * gViewMatrix;
  }
  tx = x, ty = y;
}

static void onMouseMotion(int x, int y){
  _onMouseTracking(true, x, y);
}
static void onMousePassiveMotion(int x, int y){
  _onMouseTracking(false, x, y);
}

static void onInitialization(){
  /* Where to initialize OpenGL objects */
  /* Yes, you can stream external files here */
  
  puts((char*)glGetString(GL_VERSION));
  puts((char*)glGetString(GL_VENDOR));
  puts((char*)glGetString(GL_RENDERER));
  //puts((char*)glGetString(GL_EXTENSIONS));
  
  {
    FILE* fp = fopen("models/modellist.txt", "r");
    if(!fp)
      abort();
    char buffer[64];
    float rotX;
    int baseLine, ntex;
    for(gModelListSize=0; fscanf(fp, "%s %f %d %d", buffer, &rotX, &baseLine, &ntex) != EOF ;gModelListSize++){
      strcpy(gModelFileList[gModelListSize], "models/");
      strcat(gModelFileList[gModelListSize], buffer);
      gModelFileRotX[gModelListSize] = rotX;
      gModelFileBaseLine[gModelListSize] = baseLine;
      gModelFileNumTextures[gModelListSize] = ntex;
      for(int i=0;i<ntex;i++){
        fscanf(fp, "%s", buffer);
        strcpy(gModelFileTextureFilenames[gModelListSize][i], "models/tex/");
        strcat(gModelFileTextureFilenames[gModelListSize][i], buffer);
      }
    }
    fclose(fp);
  }
  
  gRender = new Render();
  
  loadModel();
  
  resetView();
}

static void onShutdown(){
  delete gRender;
#if defined(OS_WIN)
  if(0){
    OutputDebugString(TEXT("Shutdown!\n"));
    MessageBox(NULL, TEXT("SHUTDOWN"), TEXT("DEBUG"), MB_OK);
  }
#endif
}

int main(int argc, char** argv){
  glutInit(&argc, argv);
  
#if defined(OS_MAC)
  {
    char* p = strrchr(argv[0], '/');
    *p = 0;
    chdir(argv[0]);
    //puts(getcwd(NULL, 0));
  }
#endif
#if defined(OS_WIN)
  OutputDebugString(TEXT("Boot up!\n"));
#endif
  
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(1280, 720);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
#if defined(OS_WIN)
  glutCreateWindow_ATEXIT_HACK("ICG SSAO");
#else
  glutCreateWindow("ICG SSAO");
#endif
  
  glutDisplayFunc(onDisplay);
  glutIdleFunc(onIdle);
  glutReshapeFunc(onReshape);
  glutKeyboardFunc(onKeyDown);
  glutSpecialFunc(onSpecialDown);
  glutMotionFunc(onMouseMotion);
  glutPassiveMotionFunc(onMousePassiveMotion);
  
  glewInit();
  
  /* ENABLE V-SYNC */
  int vsync = 1;
#if defined(OS_MAC)
  CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &vsync);
#elif defined(OS_WIN)
  if(WGLEW_EXT_swap_control)
    wglSwapIntervalEXT(vsync);
#else
  if(GLXEW_EXT_swap_control)
    glXSwapIntervalEXT(glXGetCurrentDisplay(), glXGetCurrentDrawable(), vsync);
#endif
  
  onInitialization();
#if !defined(__MINGW32__)
  atexit(onShutdown);
#endif
  
  /* NO RETURN */
  glutMainLoop();
  
  return 0;
}
