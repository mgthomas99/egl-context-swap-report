#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
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

static const GLfloat VERTICES[] = {
    50.0f,  50.0f,  0.0f,
    100.0f, 100.0f, 0.0f
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
    GLuint program, vert, vbo;

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, NULL, NULL);
    eglChooseConfig(display, CONFIG_ATTRIBS, &config, 1, &configCount);
    eglBindAPI(EGL_OPENGL_API);

    surface = eglCreatePbufferSurface(display, config, PBUFFER_ATTRIBS);
    ctx = eglCreateContext(display, config, EGL_NO_CONTEXT, CONTEXT_ATTRIBS);
    eglMakeCurrent(display, surface, surface, ctx);

    program = glCreateProgram();
    glUseProgram(program);
    vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &VERTEX_SHADER_CODE, NULL);
    glCompileShader(vert);
    glAttachShader(program, vert);
    glLinkProgram(program);
    glUseProgram(program);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), VERTICES, GL_STATIC_DRAW);

    GLint posloc = glGetAttribLocation(program, "pos");
    glEnableVertexAttribArray(posloc);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(posloc, 3, GL_FLOAT, GL_FALSE,
            3 * sizeof(float), (void*) 0);

    glViewport(0, 0, WIDTH, HEIGHT);

    int fps[tests];
    int lineCount = 1;
    int line;
    int i;
    for (i = 1; i <= tests; i++) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        const long a = getCurrentTimeMicroseconds();

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        for (line = 0; line < lineCount; line++) {
            glDrawArrays(GL_LINES, 0, 3);
        }

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
