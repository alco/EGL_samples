#ifndef EGL_ios_es1_demo_h
#define EGL_ios_es1_demo_h

void createRenderContext(void *view);
void destroyRenderContext();

void setupGL();
void renderFrame();
_Bool swapBuffers();

#endif
