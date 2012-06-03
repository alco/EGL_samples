#include <EGL/egl.h>
#include <OpenGLES/ES1/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


static EGLDisplay s_display;
static EGLContext s_context;
static EGLSurface s_surface;


#define iLog(x) fprintf(stderr, "Info: %s\n", x)
#define iError(x) fprintf(stderr, "Error: %s\n", x)


void createRenderContext(void *view)
{
    iLog("Create opengl context");
    
    EGLNativeWindowType hwnd = view;
    EGLNativeDisplayType dc = 0;
    
    EGLDisplay display = eglGetDisplay((dc) ? dc : EGL_DEFAULT_DISPLAY);
    
    if (!display) {
        iError("eglGetDisplay failed");
        return;
    }
    s_display = display;
    
    EGLint major;
    EGLint minor;
    EGLint numFound = 0;
#define MAX_CONFIG 32
    EGLConfig configList[MAX_CONFIG];
    
    iLog("eglInitialize");
    EGLBoolean success = eglInitialize(display, &major, &minor);
    if (!success) {
        iError("eglInitialize failed");
        return;
    }
    
    iLog("eglGetConfigs");
    eglGetConfigs(display, configList, MAX_CONFIG, &numFound);
    if (!numFound) {
        iError("eglGetConfigs failed to find any configs");
        return;
    }
    
    int config = -1;
    for (int i = 0; i < numFound; i++) {
        EGLint renderable = 0;
        EGLint enableSample = 0;
        EGLint sampleLevel = 4;
        
        EGLint r = 5;
        EGLint g = 6;
        EGLint b = 5;
        
        EGLint depth = 16;
        
        eglGetConfigAttrib(display, configList[i], EGL_RENDERABLE_TYPE, &renderable);
        eglGetConfigAttrib(display, configList[i], EGL_SAMPLE_BUFFERS, &enableSample);
        eglGetConfigAttrib(display, configList[i], EGL_SAMPLES, &sampleLevel);
        
        eglGetConfigAttrib(display, configList[i], EGL_RED_SIZE, &r);
        eglGetConfigAttrib(display, configList[i], EGL_GREEN_SIZE, &g);
        eglGetConfigAttrib(display, configList[i], EGL_BLUE_SIZE, &b);
        
        eglGetConfigAttrib(display, configList[i], EGL_DEPTH_SIZE, &depth);
                
        if ((renderable & EGL_OPENGL_ES2_BIT)
            //&& enableSample == 1
            //&& sampleLevel == 4
            && r == 8 && g == 8 && b == 8 && depth == 24)
        {
            config = i;
            break;
        }	
    }
    
    if (config == -1) {
        iError("No GLES2 configs reported. Trying random config");
        config = 0;
    }
    
    int version = 1;
    
    EGLint attribs[] = { EGL_CONTEXT_CLIENT_VERSION, version, EGL_NONE };
    
    iLog("eglCreateContext");
    s_context = eglCreateContext(display, configList[config], 0, attribs);
    if (!s_context) {
        iError("eglCreateContext failed");
        return;
    }
    
    iLog("eglCreateWindowSurface");
    s_surface = eglCreateWindowSurface(display, configList[config], hwnd, 0);
    iLog("eglMakeCurrent");
    eglMakeCurrent(display, s_surface, s_surface, s_context);
    
    //eglSwapInterval(display, 1);	
}

void destroyRenderContext()
{
    eglMakeCurrent(s_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroySurface(s_display, s_surface);
    eglDestroyContext(s_display, s_context);
    eglTerminate(s_display);
}

_Bool swapBuffers()
{
    return eglSwapBuffers(s_display, s_surface) != 0;
}


void setupGL()
{
    float width = 320, height = 480;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    double fW, fH;
    double fovY = 60;
    double pi = M_PI;
    double zNear = 1;
    double zFar = 100;
    double aspect = width / height;
    fH = tan( fovY / 360 * pi ) * zNear;
    fW = fH * aspect;
    
    glFrustumf(-fW, fW, -fH, fH, zNear, zFar);
    
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_DEPTH_TEST);
}

void renderFrame()
{
    glClearColor(1.f, .5f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    static float angle = 0;
    angle += 1;
    
    glLoadIdentity();
    float x = 1.f, y = .8f, z = .2f;
    float len = sqrtf(x*x + y*y + z*z);
    glTranslatef(0, 0, -60);
    glRotatef(angle, x/len, y/len, z/len);
    
    static float vcoords[] = {
        -10, -10, -10,
        10, -10, -10,
        10, 10, -10,
        -10, 10, -10,
        -10, 10, 10,
        10, 10, 10,
        10, -10, 10,
        -10, -10, 10,
    };
    static float color[] = {
        .2, .2, .0, 1,
        .4, .0, .6, 1,
        .0, .2, .7, 1,
        .6, .1, .0, 1,    
        .0, .2, .9, 1,
        .8, .0, .8, 1,
        .9, .0, .0, 1,
        0., .5, .6, 1,    
    };
    static unsigned char indices[] = {
        0, 1, 3,   3, 1, 2,
        1, 5, 2,   1, 6, 5,
        4, 5, 6,   6, 7, 4,
        7, 0, 3,   7, 3, 4,
        3, 2, 4,   2, 5, 4,
        6, 1, 0,   7, 6, 0,
    };
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vcoords);
    glColorPointer(4, GL_FLOAT, 0, color);
    
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);
    glFinish();
}