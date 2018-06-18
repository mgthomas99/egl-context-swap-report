#include <EGL/egl.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "./shader/shaders.h"

static const int HEIGHT = 50;
static const int WIDTH  = 50;

static const EGLint CONFIG_ATTRIBS[] = {
    EGL_RENDERABLE_TYPE,    EGL_OPENGL_ES2_BIT,
    EGL_SURFACE_TYPE,       EGL_PBUFFER_BIT,
    EGL_BLUE_SIZE,          16,
    EGL_GREEN_SIZE,         16,
    EGL_RED_SIZE,           16,
    EGL_DEPTH_SIZE,         8,
    EGL_NONE
};

static const EGLint CONTEXT_ATTRIBS[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
};

static const EGLint PBUFFER_ATTRIBS[] = {
    EGL_HEIGHT, 50,
    EGL_WIDTH,  50,
    EGL_NONE
};

/**
 * @return  {long}
 *          The number of microseconds since 00:00 on January 1st, 1970.
 */
unsigned long getCurrentTimeMicroseconds() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return (long) (time.tv_usec + (time.tv_sec * 1000000));
}

int main(int argc, char** argv) {
    // The number of tests to execute.
    int tests = 120;

    // The number of lines to add after every test.
    int lineCountIncrement = 240000 / tests;

    EGLDisplay display;
    EGLConfig config;
    EGLSurface surface;
    EGLContext ctx;
    EGLint configCount;

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, NULL, NULL);
    eglChooseConfig(display, CONFIG_ATTRIBS, &config, 1, &configCount);
    eglBindAPI(EGL_OPENGL_API);

    surface = eglCreatePbufferSurface(display, config, PBUFFER_ATTRIBS);
    ctx = eglCreateContext(display, config, EGL_NO_CONTEXT, CONTEXT_ATTRIBS);
    eglMakeCurrent(display, surface, surface, ctx);

    VGfloat VERTICES[] = { 50, 50, 100, 100 };
    VGfloat STOPS[] = {
        1.0f, 0.0f,    1.0f, 0.0f, 0.0f,
        0.0f, 1.0f,    0.0f, 0.0f, 1.0f
    };
    VGPaint paint = vgCreatePaint();
    vgSetParameteri(paint, VG_PAINT_TYPE, VG_PAINT_TYPE_LINEAR_GRADIENT);
    vgSetParameterfv(paint, VG_PAINT_LINEAR_GRADIENT, 4, VERTICES);

    vgSetParameteri(paint, VG_PAINT_COLOR_RAMP_SPREAD_MODE, VG_COLOR_RAMP_SPREAD_REPEAT);
    vgSetParameteri(paint, VG_PAINT_COLOR_RAMP_PREMULTIPLIED, VG_FALSE);
    vgSetParameterfv(paint, VG_PAINT_COLOR_RAMP_STOPS, 10, STOPS);
    vgSetPaint(paint, VG_FILL_PATH);

    vgSetf(VG_STROKE_LINE_WIDTH, 2.0f);
    vgSeti(VG_STROKE_JOIN_STYLE, VG_JOIN_MITER);

    VGPath path = vgCreatePath(
        VG_PATH_FORMAT_STANDARD,
        VG_PATH_DATATYPE_F,
        1.0f,
        0.0f,
        0,
        0,
        VG_PATH_CAPABILITY_APPEND_TO
    );
    vguLine(path, 50, 50, 100, 100);

    int fps[tests];
    int lineCount = 1;
    int line;
    int i;
    for (i = 1; i <= tests; i++) {
        vgLoadIdentity();
        vgClear(0, 0, 1920, 1080);

        const long a = getCurrentTimeMicroseconds();

        for (line = 0; line < lineCount; line++) {
            vgSetPaint(paint, VG_FILL_PATH);
            vgDrawPath(path, VG_FILL_PATH);
        }
        eglSwapBuffers(display, surface);

        const long elapsed = getCurrentTimeMicroseconds() - a;
        double ifps = ((double) 1000) / ((double) elapsed / 1000);
        fps[i] = (int) ifps;// 1000 / (elapsed > 0 ? elapsed : 1);
        lineCount += lineCountIncrement;
    }

    for (i = 0; i < tests; i++) {
        if ((i * lineCountIncrement) % 5000 != 0) continue;
        printf("%d: %d\n", i * lineCountIncrement, fps[i]);
    }

    eglDestroyContext(display, ctx);
    eglMakeCurrent(display, NULL, NULL, NULL);
    eglTerminate(display);
}
