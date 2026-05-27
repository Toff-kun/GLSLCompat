// stubs.cpp
// Forwarding stubs for all non-intercepted GL and WGL functions.
// Each stub resolves its target lazily from the system opengl32.dll on first call.
// hRealOpenGL is defined in dllmain.cpp (no longer static, so it is visible here).

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma warning(disable:4273)

// GL types
typedef unsigned int   GLenum;
typedef unsigned int   GLbitfield;
typedef unsigned int   GLuint;
typedef int            GLint;
typedef int            GLsizei;
typedef unsigned char  GLboolean;
typedef signed char    GLbyte;
typedef short          GLshort;
typedef unsigned char  GLubyte;
typedef unsigned short GLushort;
typedef float          GLfloat;
typedef float          GLclampf;
typedef double         GLdouble;
typedef double         GLclampd;
typedef void           GLvoid;

extern HMODULE hRealOpenGL;

static inline void* SP(const char* name) {
    return (void*)GetProcAddress(hRealOpenGL, name);
}

// ============================================================
//  WGL stubs
// ============================================================

extern "C" int WINAPI wglChoosePixelFormat(HDC hdc, const PIXELFORMATDESCRIPTOR* ppfd) {
    typedef int(WINAPI* F)(HDC, const PIXELFORMATDESCRIPTOR*);
    static F f = (F)SP("wglChoosePixelFormat");
    return f ? f(hdc, ppfd) : 0;
}

extern "C" BOOL WINAPI wglCopyContext(HGLRC src, HGLRC dst, UINT mask) {
    typedef BOOL(WINAPI* F)(HGLRC, HGLRC, UINT);
    static F f = (F)SP("wglCopyContext");
    return f ? f(src, dst, mask) : FALSE;
}

extern "C" HGLRC WINAPI wglCreateContext(HDC hdc) {
    typedef HGLRC(WINAPI* F)(HDC);
    static F f = (F)SP("wglCreateContext");
    return f ? f(hdc) : NULL;
}

extern "C" HGLRC WINAPI wglCreateLayerContext(HDC hdc, int layer) {
    typedef HGLRC(WINAPI* F)(HDC, int);
    static F f = (F)SP("wglCreateLayerContext");
    return f ? f(hdc, layer) : NULL;
}

extern "C" BOOL WINAPI wglDeleteContext(HGLRC hglrc) {
    typedef BOOL(WINAPI* F)(HGLRC);
    static F f = (F)SP("wglDeleteContext");
    return f ? f(hglrc) : FALSE;
}

extern "C" BOOL WINAPI wglDescribeLayerPlane(HDC hdc, int pixel, int layer, UINT bytes, LAYERPLANEDESCRIPTOR* lpd) {
    typedef BOOL(WINAPI* F)(HDC, int, int, UINT, LAYERPLANEDESCRIPTOR*);
    static F f = (F)SP("wglDescribeLayerPlane");
    return f ? f(hdc, pixel, layer, bytes, lpd) : FALSE;
}

extern "C" int WINAPI wglDescribePixelFormat(HDC hdc, int pixel, UINT bytes, PIXELFORMATDESCRIPTOR* ppfd) {
    typedef int(WINAPI* F)(HDC, int, UINT, PIXELFORMATDESCRIPTOR*);
    static F f = (F)SP("wglDescribePixelFormat");
    return f ? f(hdc, pixel, bytes, ppfd) : 0;
}

extern "C" HGLRC WINAPI wglGetCurrentContext(void) {
    typedef HGLRC(WINAPI* F)(void);
    static F f = (F)SP("wglGetCurrentContext");
    return f ? f() : NULL;
}

extern "C" HDC WINAPI wglGetCurrentDC(void) {
    typedef HDC(WINAPI* F)(void);
    static F f = (F)SP("wglGetCurrentDC");
    return f ? f() : NULL;
}

extern "C" int WINAPI wglGetLayerPaletteEntries(HDC hdc, int layer, int start, int entries, COLORREF* cr) {
    typedef int(WINAPI* F)(HDC, int, int, int, COLORREF*);
    static F f = (F)SP("wglGetLayerPaletteEntries");
    return f ? f(hdc, layer, start, entries, cr) : 0;
}

extern "C" int WINAPI wglGetPixelFormat(HDC hdc) {
    typedef int(WINAPI* F)(HDC);
    static F f = (F)SP("wglGetPixelFormat");
    return f ? f(hdc) : 0;
}

extern "C" BOOL WINAPI wglMakeCurrent(HDC hdc, HGLRC hglrc) {
    typedef BOOL(WINAPI* F)(HDC, HGLRC);
    static F f = (F)SP("wglMakeCurrent");
    return f ? f(hdc, hglrc) : FALSE;
}

extern "C" BOOL WINAPI wglRealizeLayerPalette(HDC hdc, int layer, BOOL realize) {
    typedef BOOL(WINAPI* F)(HDC, int, BOOL);
    static F f = (F)SP("wglRealizeLayerPalette");
    return f ? f(hdc, layer, realize) : FALSE;
}

extern "C" int WINAPI wglSetLayerPaletteEntries(HDC hdc, int layer, int start, int entries, const COLORREF* cr) {
    typedef int(WINAPI* F)(HDC, int, int, int, const COLORREF*);
    static F f = (F)SP("wglSetLayerPaletteEntries");
    return f ? f(hdc, layer, start, entries, cr) : 0;
}

extern "C" BOOL WINAPI wglSetPixelFormat(HDC hdc, int pixel, const PIXELFORMATDESCRIPTOR* ppfd) {
    typedef BOOL(WINAPI* F)(HDC, int, const PIXELFORMATDESCRIPTOR*);
    static F f = (F)SP("wglSetPixelFormat");
    return f ? f(hdc, pixel, ppfd) : FALSE;
}

extern "C" BOOL WINAPI wglShareLists(HGLRC a, HGLRC b) {
    typedef BOOL(WINAPI* F)(HGLRC, HGLRC);
    static F f = (F)SP("wglShareLists");
    return f ? f(a, b) : FALSE;
}

extern "C" BOOL WINAPI wglSwapBuffers(HDC hdc) {
    typedef BOOL(WINAPI* F)(HDC);
    static F f = (F)SP("wglSwapBuffers");
    return f ? f(hdc) : FALSE;
}

extern "C" BOOL WINAPI wglSwapLayerBuffers(HDC hdc, UINT planes) {
    typedef BOOL(WINAPI* F)(HDC, UINT);
    static F f = (F)SP("wglSwapLayerBuffers");
    return f ? f(hdc, planes) : FALSE;
}

extern "C" BOOL WINAPI wglUseFontBitmapsA(HDC hdc, DWORD first, DWORD count, DWORD listBase) {
    typedef BOOL(WINAPI* F)(HDC, DWORD, DWORD, DWORD);
    static F f = (F)SP("wglUseFontBitmapsA");
    return f ? f(hdc, first, count, listBase) : FALSE;
}

extern "C" BOOL WINAPI wglUseFontBitmapsW(HDC hdc, DWORD first, DWORD count, DWORD listBase) {
    typedef BOOL(WINAPI* F)(HDC, DWORD, DWORD, DWORD);
    static F f = (F)SP("wglUseFontBitmapsW");
    return f ? f(hdc, first, count, listBase) : FALSE;
}

extern "C" BOOL WINAPI wglUseFontOutlinesA(HDC hdc, DWORD first, DWORD count, DWORD listBase,
    FLOAT deviation, FLOAT extrusion, int format, LPGLYPHMETRICSFLOAT lpgmf) {
    typedef BOOL(WINAPI* F)(HDC, DWORD, DWORD, DWORD, FLOAT, FLOAT, int, LPGLYPHMETRICSFLOAT);
    static F f = (F)SP("wglUseFontOutlinesA");
    return f ? f(hdc, first, count, listBase, deviation, extrusion, format, lpgmf) : FALSE;
}

extern "C" BOOL WINAPI wglUseFontOutlinesW(HDC hdc, DWORD first, DWORD count, DWORD listBase,
    FLOAT deviation, FLOAT extrusion, int format, LPGLYPHMETRICSFLOAT lpgmf) {
    typedef BOOL(WINAPI* F)(HDC, DWORD, DWORD, DWORD, FLOAT, FLOAT, int, LPGLYPHMETRICSFLOAT);
    static F f = (F)SP("wglUseFontOutlinesW");
    return f ? f(hdc, first, count, listBase, deviation, extrusion, format, lpgmf) : FALSE;
}

// ============================================================
//  GL stubs
// ============================================================

extern "C" void APIENTRY glArrayElement(GLint i) {
    typedef void(APIENTRY* F)(GLint);
    static F f = (F)SP("glArrayElement");
    if (f) f(i);
}

extern "C" void APIENTRY glBegin(GLenum mode) {
    typedef void(APIENTRY* F)(GLenum);
    static F f = (F)SP("glBegin");
    if (f) f(mode);
}

extern "C" void APIENTRY glBindTexture(GLenum target, GLuint texture) {
    typedef void(APIENTRY* F)(GLenum, GLuint);
    static F f = (F)SP("glBindTexture");
    if (f) f(target, texture);
}

extern "C" void APIENTRY glBlendFunc(GLenum sfactor, GLenum dfactor) {
    typedef void(APIENTRY* F)(GLenum, GLenum);
    static F f = (F)SP("glBlendFunc");
    if (f) f(sfactor, dfactor);
}

extern "C" void APIENTRY glCallList(GLuint list) {
    typedef void(APIENTRY* F)(GLuint);
    static F f = (F)SP("glCallList");
    if (f) f(list);
}

extern "C" void APIENTRY glClear(GLbitfield mask) {
    typedef void(APIENTRY* F)(GLbitfield);
    static F f = (F)SP("glClear");
    if (f) f(mask);
}

extern "C" void APIENTRY glClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    typedef void(APIENTRY* F)(GLfloat, GLfloat, GLfloat, GLfloat);
    static F f = (F)SP("glClearColor");
    if (f) f(r, g, b, a);
}

extern "C" void APIENTRY glClearDepth(GLdouble depth) {
    typedef void(APIENTRY* F)(GLdouble);
    static F f = (F)SP("glClearDepth");
    if (f) f(depth);
}

extern "C" void APIENTRY glColor3f(GLfloat r, GLfloat g, GLfloat b) {
    typedef void(APIENTRY* F)(GLfloat, GLfloat, GLfloat);
    static F f = (F)SP("glColor3f");
    if (f) f(r, g, b);
}

extern "C" void APIENTRY glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    typedef void(APIENTRY* F)(GLfloat, GLfloat, GLfloat, GLfloat);
    static F f = (F)SP("glColor4f");
    if (f) f(r, g, b, a);
}

extern "C" void APIENTRY glColorMaterial(GLenum face, GLenum mode) {
    typedef void(APIENTRY* F)(GLenum, GLenum);
    static F f = (F)SP("glColorMaterial");
    if (f) f(face, mode);
}

extern "C" void APIENTRY glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* ptr) {
    typedef void(APIENTRY* F)(GLint, GLenum, GLsizei, const GLvoid*);
    static F f = (F)SP("glColorPointer");
    if (f) f(size, type, stride, ptr);
}

extern "C" void APIENTRY glCopyPixels(GLint x, GLint y, GLsizei w, GLsizei h, GLenum type) {
    typedef void(APIENTRY* F)(GLint, GLint, GLsizei, GLsizei, GLenum);
    static F f = (F)SP("glCopyPixels");
    if (f) f(x, y, w, h, type);
}

extern "C" void APIENTRY glDeleteLists(GLuint list, GLsizei range) {
    typedef void(APIENTRY* F)(GLuint, GLsizei);
    static F f = (F)SP("glDeleteLists");
    if (f) f(list, range);
}

extern "C" void APIENTRY glDeleteTextures(GLsizei n, const GLuint* textures) {
    typedef void(APIENTRY* F)(GLsizei, const GLuint*);
    static F f = (F)SP("glDeleteTextures");
    if (f) f(n, textures);
}

extern "C" void APIENTRY glDepthFunc(GLenum func) {
    typedef void(APIENTRY* F)(GLenum);
    static F f = (F)SP("glDepthFunc");
    if (f) f(func);
}

extern "C" void APIENTRY glDepthMask(GLboolean flag) {
    typedef void(APIENTRY* F)(GLboolean);
    static F f = (F)SP("glDepthMask");
    if (f) f(flag);
}

extern "C" void APIENTRY glDepthRange(GLdouble zNear, GLdouble zFar) {
    typedef void(APIENTRY* F)(GLdouble, GLdouble);
    static F f = (F)SP("glDepthRange");
    if (f) f(zNear, zFar);
}

extern "C" void APIENTRY glDisable(GLenum cap) {
    typedef void(APIENTRY* F)(GLenum);
    static F f = (F)SP("glDisable");
    if (f) f(cap);
}

extern "C" void APIENTRY glDisableClientState(GLenum array) {
    typedef void(APIENTRY* F)(GLenum);
    static F f = (F)SP("glDisableClientState");
    if (f) f(array);
}

extern "C" void APIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count) {
    typedef void(APIENTRY* F)(GLenum, GLint, GLsizei);
    static F f = (F)SP("glDrawArrays");
    if (f) f(mode, first, count);
}

extern "C" void APIENTRY glDrawBuffer(GLenum mode) {
    typedef void(APIENTRY* F)(GLenum);
    static F f = (F)SP("glDrawBuffer");
    if (f) f(mode);
}

extern "C" void APIENTRY glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices) {
    typedef void(APIENTRY* F)(GLenum, GLsizei, GLenum, const GLvoid*);
    static F f = (F)SP("glDrawElements");
    if (f) f(mode, count, type, indices);
}

extern "C" void APIENTRY glDrawPixels(GLsizei w, GLsizei h, GLenum format, GLenum type, const GLvoid* pixels) {
    typedef void(APIENTRY* F)(GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);
    static F f = (F)SP("glDrawPixels");
    if (f) f(w, h, format, type, pixels);
}

extern "C" void APIENTRY glEnable(GLenum cap) {
    typedef void(APIENTRY* F)(GLenum);
    static F f = (F)SP("glEnable");
    if (f) f(cap);
}

extern "C" void APIENTRY glEnableClientState(GLenum array) {
    typedef void(APIENTRY* F)(GLenum);
    static F f = (F)SP("glEnableClientState");
    if (f) f(array);
}

extern "C" void APIENTRY glEnd(void) {
    typedef void(APIENTRY* F)(void);
    static F f = (F)SP("glEnd");
    if (f) f();
}

extern "C" void APIENTRY glEndList(void) {
    typedef void(APIENTRY* F)(void);
    static F f = (F)SP("glEndList");
    if (f) f();
}

extern "C" void APIENTRY glEvalCoord1f(GLfloat u) {
    typedef void(APIENTRY* F)(GLfloat);
    static F f = (F)SP("glEvalCoord1f");
    if (f) f(u);
}

extern "C" void APIENTRY glEvalCoord2f(GLfloat u, GLfloat v) {
    typedef void(APIENTRY* F)(GLfloat, GLfloat);
    static F f = (F)SP("glEvalCoord2f");
    if (f) f(u, v);
}

extern "C" void APIENTRY glEvalMesh1(GLenum mode, GLint i1, GLint i2) {
    typedef void(APIENTRY* F)(GLenum, GLint, GLint);
    static F f = (F)SP("glEvalMesh1");
    if (f) f(mode, i1, i2);
}

extern "C" void APIENTRY glEvalMesh2(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2) {
    typedef void(APIENTRY* F)(GLenum, GLint, GLint, GLint, GLint);
    static F f = (F)SP("glEvalMesh2");
    if (f) f(mode, i1, i2, j1, j2);
}

extern "C" void APIENTRY glEvalPoint1(GLint i) {
    typedef void(APIENTRY* F)(GLint);
    static F f = (F)SP("glEvalPoint1");
    if (f) f(i);
}

extern "C" void APIENTRY glEvalPoint2(GLint i, GLint j) {
    typedef void(APIENTRY* F)(GLint, GLint);
    static F f = (F)SP("glEvalPoint2");
    if (f) f(i, j);
}

extern "C" void APIENTRY glFinish(void) {
    typedef void(APIENTRY* F)(void);
    static F f = (F)SP("glFinish");
    if (f) f();
}

extern "C" void APIENTRY glFlush(void) {
    typedef void(APIENTRY* F)(void);
    static F f = (F)SP("glFlush");
    if (f) f();
}

extern "C" void APIENTRY glFogf(GLenum pname, GLfloat param) {
    typedef void(APIENTRY* F)(GLenum, GLfloat);
    static F f = (F)SP("glFogf");
    if (f) f(pname, param);
}

extern "C" void APIENTRY glFogfv(GLenum pname, const GLfloat* params) {
    typedef void(APIENTRY* F)(GLenum, const GLfloat*);
    static F f = (F)SP("glFogfv");
    if (f) f(pname, params);
}

extern "C" void APIENTRY glFogi(GLenum pname, GLint param) {
    typedef void(APIENTRY* F)(GLenum, GLint);
    static F f = (F)SP("glFogi");
    if (f) f(pname, param);
}

extern "C" void APIENTRY glFogiv(GLenum pname, const GLint* params) {
    typedef void(APIENTRY* F)(GLenum, const GLint*);
    static F f = (F)SP("glFogiv");
    if (f) f(pname, params);
}

extern "C" void APIENTRY glFrontFace(GLenum mode) {
    typedef void(APIENTRY* F)(GLenum);
    static F f = (F)SP("glFrontFace");
    if (f) f(mode);
}

extern "C" void APIENTRY glFrustum(GLdouble l, GLdouble r, GLdouble b, GLdouble t, GLdouble n, GLdouble far_) {
    typedef void(APIENTRY* F)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);
    static F f = (F)SP("glFrustum");
    if (f) f(l, r, b, t, n, far_);
}

extern "C" GLuint APIENTRY glGenLists(GLsizei range) {
    typedef GLuint(APIENTRY* F)(GLsizei);
    static F f = (F)SP("glGenLists");
    return f ? f(range) : 0;
}

extern "C" void APIENTRY glGenTextures(GLsizei n, GLuint* textures) {
    typedef void(APIENTRY* F)(GLsizei, GLuint*);
    static F f = (F)SP("glGenTextures");
    if (f) f(n, textures);
}

extern "C" void APIENTRY glGetBooleanv(GLenum pname, GLboolean* params) {
    typedef void(APIENTRY* F)(GLenum, GLboolean*);
    static F f = (F)SP("glGetBooleanv");
    if (f) f(pname, params);
}

extern "C" void APIENTRY glGetDoublev(GLenum pname, GLdouble* params) {
    typedef void(APIENTRY* F)(GLenum, GLdouble*);
    static F f = (F)SP("glGetDoublev");
    if (f) f(pname, params);
}

extern "C" GLenum APIENTRY glGetError(void) {
    typedef GLenum(APIENTRY* F)(void);
    static F f = (F)SP("glGetError");
    return f ? f() : 0;
}

extern "C" void APIENTRY glGetFloatv(GLenum pname, GLfloat* params) {
    typedef void(APIENTRY* F)(GLenum, GLfloat*);
    static F f = (F)SP("glGetFloatv");
    if (f) f(pname, params);
}

extern "C" void APIENTRY glGetIntegerv(GLenum pname, GLint* params) {
    typedef void(APIENTRY* F)(GLenum, GLint*);
    static F f = (F)SP("glGetIntegerv");
    if (f) f(pname, params);
}

extern "C" void APIENTRY glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLvoid* pixels) {
    typedef void(APIENTRY* F)(GLenum, GLint, GLenum, GLenum, GLvoid*);
    static F f = (F)SP("glGetTexImage");
    if (f) f(target, level, format, type, pixels);
}

extern "C" void APIENTRY glHint(GLenum target, GLenum mode) {
    typedef void(APIENTRY* F)(GLenum, GLenum);
    static F f = (F)SP("glHint");
    if (f) f(target, mode);
}

extern "C" GLboolean APIENTRY glIsList(GLuint list) {
    typedef GLboolean(APIENTRY* F)(GLuint);
    static F f = (F)SP("glIsList");
    return f ? f(list) : (GLboolean)0;
}

extern "C" void APIENTRY glLightModelf(GLenum pname, GLfloat param) {
    typedef void(APIENTRY* F)(GLenum, GLfloat);
    static F f = (F)SP("glLightModelf");
    if (f) f(pname, param);
}

extern "C" void APIENTRY glLightModelfv(GLenum pname, const GLfloat* params) {
    typedef void(APIENTRY* F)(GLenum, const GLfloat*);
    static F f = (F)SP("glLightModelfv");
    if (f) f(pname, params);
}

extern "C" void APIENTRY glLightModeli(GLenum pname, GLint param) {
    typedef void(APIENTRY* F)(GLenum, GLint);
    static F f = (F)SP("glLightModeli");
    if (f) f(pname, param);
}

extern "C" void APIENTRY glLightModeliv(GLenum pname, const GLint* params) {
    typedef void(APIENTRY* F)(GLenum, const GLint*);
    static F f = (F)SP("glLightModeliv");
    if (f) f(pname, params);
}

extern "C" void APIENTRY glLightf(GLenum light, GLenum pname, GLfloat param) {
    typedef void(APIENTRY* F)(GLenum, GLenum, GLfloat);
    static F f = (F)SP("glLightf");
    if (f) f(light, pname, param);
}

extern "C" void APIENTRY glLightfv(GLenum light, GLenum pname, const GLfloat* params) {
    typedef void(APIENTRY* F)(GLenum, GLenum, const GLfloat*);
    static F f = (F)SP("glLightfv");
    if (f) f(light, pname, params);
}

extern "C" void APIENTRY glLighti(GLenum light, GLenum pname, GLint param) {
    typedef void(APIENTRY* F)(GLenum, GLenum, GLint);
    static F f = (F)SP("glLighti");
    if (f) f(light, pname, param);
}

extern "C" void APIENTRY glLightiv(GLenum light, GLenum pname, const GLint* params) {
    typedef void(APIENTRY* F)(GLenum, GLenum, const GLint*);
    static F f = (F)SP("glLightiv");
    if (f) f(light, pname, params);
}

extern "C" void APIENTRY glLineWidth(GLfloat width) {
    typedef void(APIENTRY* F)(GLfloat);
    static F f = (F)SP("glLineWidth");
    if (f) f(width);
}

extern "C" void APIENTRY glLoadIdentity(void) {
    typedef void(APIENTRY* F)(void);
    static F f = (F)SP("glLoadIdentity");
    if (f) f();
}

extern "C" void APIENTRY glMap1d(GLenum target, GLdouble u1, GLdouble u2,
    GLint stride, GLint order, const GLdouble* points) {
    typedef void(APIENTRY* F)(GLenum, GLdouble, GLdouble, GLint, GLint, const GLdouble*);
    static F f = (F)SP("glMap1d");
    if (f) f(target, u1, u2, stride, order, points);
}

extern "C" void APIENTRY glMap1f(GLenum target, GLfloat u1, GLfloat u2,
    GLint stride, GLint order, const GLfloat* points) {
    typedef void(APIENTRY* F)(GLenum, GLfloat, GLfloat, GLint, GLint, const GLfloat*);
    static F f = (F)SP("glMap1f");
    if (f) f(target, u1, u2, stride, order, points);
}

extern "C" void APIENTRY glMap2d(GLenum target,
    GLdouble u1, GLdouble u2, GLint ustride, GLint uorder,
    GLdouble v1, GLdouble v2, GLint vstride, GLint vorder,
    const GLdouble* points) {
    typedef void(APIENTRY* F)(GLenum,
        GLdouble, GLdouble, GLint, GLint,
        GLdouble, GLdouble, GLint, GLint,
        const GLdouble*);
    static F f = (F)SP("glMap2d");
    if (f) f(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
}

extern "C" void APIENTRY glMap2f(GLenum target,
    GLfloat u1, GLfloat u2, GLint ustride, GLint uorder,
    GLfloat v1, GLfloat v2, GLint vstride, GLint vorder,
    const GLfloat* points) {
    typedef void(APIENTRY* F)(GLenum,
        GLfloat, GLfloat, GLint, GLint,
        GLfloat, GLfloat, GLint, GLint,
        const GLfloat*);
    static F f = (F)SP("glMap2f");
    if (f) f(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
}

extern "C" void APIENTRY glMapGrid1d(GLint un, GLdouble u1, GLdouble u2) {
    typedef void(APIENTRY* F)(GLint, GLdouble, GLdouble);
    static F f = (F)SP("glMapGrid1d");
    if (f) f(un, u1, u2);
}

extern "C" void APIENTRY glMapGrid1f(GLint un, GLfloat u1, GLfloat u2) {
    typedef void(APIENTRY* F)(GLint, GLfloat, GLfloat);
    static F f = (F)SP("glMapGrid1f");
    if (f) f(un, u1, u2);
}

extern "C" void APIENTRY glMapGrid2d(GLint un, GLdouble u1, GLdouble u2,
    GLint vn, GLdouble v1, GLdouble v2) {
    typedef void(APIENTRY* F)(GLint, GLdouble, GLdouble, GLint, GLdouble, GLdouble);
    static F f = (F)SP("glMapGrid2d");
    if (f) f(un, u1, u2, vn, v1, v2);
}

extern "C" void APIENTRY glMapGrid2f(GLint un, GLfloat u1, GLfloat u2,
    GLint vn, GLfloat v1, GLfloat v2) {
    typedef void(APIENTRY* F)(GLint, GLfloat, GLfloat, GLint, GLfloat, GLfloat);
    static F f = (F)SP("glMapGrid2f");
    if (f) f(un, u1, u2, vn, v1, v2);
}

extern "C" void APIENTRY glMaterialf(GLenum face, GLenum pname, GLfloat param) {
    typedef void(APIENTRY* F)(GLenum, GLenum, GLfloat);
    static F f = (F)SP("glMaterialf");
    if (f) f(face, pname, param);
}

extern "C" void APIENTRY glMaterialfv(GLenum face, GLenum pname, const GLfloat* params) {
    typedef void(APIENTRY* F)(GLenum, GLenum, const GLfloat*);
    static F f = (F)SP("glMaterialfv");
    if (f) f(face, pname, params);
}

extern "C" void APIENTRY glMateriali(GLenum face, GLenum pname, GLint param) {
    typedef void(APIENTRY* F)(GLenum, GLenum, GLint);
    static F f = (F)SP("glMateriali");
    if (f) f(face, pname, param);
}

extern "C" void APIENTRY glMaterialiv(GLenum face, GLenum pname, const GLint* params) {
    typedef void(APIENTRY* F)(GLenum, GLenum, const GLint*);
    static F f = (F)SP("glMaterialiv");
    if (f) f(face, pname, params);
}

extern "C" void APIENTRY glMatrixMode(GLenum mode) {
    typedef void(APIENTRY* F)(GLenum);
    static F f = (F)SP("glMatrixMode");
    if (f) f(mode);
}

extern "C" void APIENTRY glMultMatrixd(const GLdouble* m) {
    typedef void(APIENTRY* F)(const GLdouble*);
    static F f = (F)SP("glMultMatrixd");
    if (f) f(m);
}

extern "C" void APIENTRY glMultMatrixf(const GLfloat* m) {
    typedef void(APIENTRY* F)(const GLfloat*);
    static F f = (F)SP("glMultMatrixf");
    if (f) f(m);
}

extern "C" void APIENTRY glNewList(GLuint list, GLenum mode) {
    typedef void(APIENTRY* F)(GLuint, GLenum);
    static F f = (F)SP("glNewList");
    if (f) f(list, mode);
}

extern "C" void APIENTRY glNormal3d(GLdouble nx, GLdouble ny, GLdouble nz) {
    typedef void(APIENTRY* F)(GLdouble, GLdouble, GLdouble);
    static F f = (F)SP("glNormal3d");
    if (f) f(nx, ny, nz);
}

extern "C" void APIENTRY glNormal3dv(const GLdouble* v) {
    typedef void(APIENTRY* F)(const GLdouble*);
    static F f = (F)SP("glNormal3dv");
    if (f) f(v);
}

extern "C" void APIENTRY glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz) {
    typedef void(APIENTRY* F)(GLfloat, GLfloat, GLfloat);
    static F f = (F)SP("glNormal3f");
    if (f) f(nx, ny, nz);
}

extern "C" void APIENTRY glNormal3fv(const GLfloat* v) {
    typedef void(APIENTRY* F)(const GLfloat*);
    static F f = (F)SP("glNormal3fv");
    if (f) f(v);
}

extern "C" void APIENTRY glNormalPointer(GLenum type, GLsizei stride, const GLvoid* ptr) {
    typedef void(APIENTRY* F)(GLenum, GLsizei, const GLvoid*);
    static F f = (F)SP("glNormalPointer");
    if (f) f(type, stride, ptr);
}

extern "C" void APIENTRY glOrtho(GLdouble l, GLdouble r, GLdouble b, GLdouble t, GLdouble n, GLdouble far_) {
    typedef void(APIENTRY* F)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);
    static F f = (F)SP("glOrtho");
    if (f) f(l, r, b, t, n, far_);
}

extern "C" void APIENTRY glPixelMapfv(GLenum map, GLsizei size, const GLfloat* values) {
    typedef void(APIENTRY* F)(GLenum, GLsizei, const GLfloat*);
    static F f = (F)SP("glPixelMapfv");
    if (f) f(map, size, values);
}

extern "C" void APIENTRY glPixelMapuiv(GLenum map, GLsizei size, const GLuint* values) {
    typedef void(APIENTRY* F)(GLenum, GLsizei, const GLuint*);
    static F f = (F)SP("glPixelMapuiv");
    if (f) f(map, size, values);
}

extern "C" void APIENTRY glPixelMapusv(GLenum map, GLsizei size, const GLushort* values) {
    typedef void(APIENTRY* F)(GLenum, GLsizei, const GLushort*);
    static F f = (F)SP("glPixelMapusv");
    if (f) f(map, size, values);
}

extern "C" void APIENTRY glPixelStoref(GLenum pname, GLfloat param) {
    typedef void(APIENTRY* F)(GLenum, GLfloat);
    static F f = (F)SP("glPixelStoref");
    if (f) f(pname, param);
}

extern "C" void APIENTRY glPixelStorei(GLenum pname, GLint param) {
    typedef void(APIENTRY* F)(GLenum, GLint);
    static F f = (F)SP("glPixelStorei");
    if (f) f(pname, param);
}

extern "C" void APIENTRY glPixelTransferf(GLenum pname, GLfloat param) {
    typedef void(APIENTRY* F)(GLenum, GLfloat);
    static F f = (F)SP("glPixelTransferf");
    if (f) f(pname, param);
}

extern "C" void APIENTRY glPixelTransferi(GLenum pname, GLint param) {
    typedef void(APIENTRY* F)(GLenum, GLint);
    static F f = (F)SP("glPixelTransferi");
    if (f) f(pname, param);
}

extern "C" void APIENTRY glPixelZoom(GLfloat xfactor, GLfloat yfactor) {
    typedef void(APIENTRY* F)(GLfloat, GLfloat);
    static F f = (F)SP("glPixelZoom");
    if (f) f(xfactor, yfactor);
}

extern "C" void APIENTRY glPointSize(GLfloat size) {
    typedef void(APIENTRY* F)(GLfloat);
    static F f = (F)SP("glPointSize");
    if (f) f(size);
}

extern "C" void APIENTRY glPolygonMode(GLenum face, GLenum mode) {
    typedef void(APIENTRY* F)(GLenum, GLenum);
    static F f = (F)SP("glPolygonMode");
    if (f) f(face, mode);
}

extern "C" void APIENTRY glPopAttrib(void) {
    typedef void(APIENTRY* F)(void);
    static F f = (F)SP("glPopAttrib");
    if (f) f();
}

extern "C" void APIENTRY glPopClientAttrib(void) {
    typedef void(APIENTRY* F)(void);
    static F f = (F)SP("glPopClientAttrib");
    if (f) f();
}

extern "C" void APIENTRY glPopMatrix(void) {
    typedef void(APIENTRY* F)(void);
    static F f = (F)SP("glPopMatrix");
    if (f) f();
}

extern "C" void APIENTRY glPushAttrib(GLbitfield mask) {
    typedef void(APIENTRY* F)(GLbitfield);
    static F f = (F)SP("glPushAttrib");
    if (f) f(mask);
}

extern "C" void APIENTRY glPushClientAttrib(GLbitfield mask) {
    typedef void(APIENTRY* F)(GLbitfield);
    static F f = (F)SP("glPushClientAttrib");
    if (f) f(mask);
}

extern "C" void APIENTRY glPushMatrix(void) {
    typedef void(APIENTRY* F)(void);
    static F f = (F)SP("glPushMatrix");
    if (f) f();
}

extern "C" void APIENTRY glReadBuffer(GLenum mode) {
    typedef void(APIENTRY* F)(GLenum);
    static F f = (F)SP("glReadBuffer");
    if (f) f(mode);
}

extern "C" void APIENTRY glReadPixels(GLint x, GLint y, GLsizei w, GLsizei h,
    GLenum format, GLenum type, GLvoid* pixels) {
    typedef void(APIENTRY* F)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid*);
    static F f = (F)SP("glReadPixels");
    if (f) f(x, y, w, h, format, type, pixels);
}

extern "C" void APIENTRY glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
    typedef void(APIENTRY* F)(GLfloat, GLfloat, GLfloat, GLfloat);
    static F f = (F)SP("glRotatef");
    if (f) f(angle, x, y, z);
}

extern "C" void APIENTRY glScalef(GLfloat x, GLfloat y, GLfloat z) {
    typedef void(APIENTRY* F)(GLfloat, GLfloat, GLfloat);
    static F f = (F)SP("glScalef");
    if (f) f(x, y, z);
}

extern "C" void APIENTRY glShadeModel(GLenum mode) {
    typedef void(APIENTRY* F)(GLenum);
    static F f = (F)SP("glShadeModel");
    if (f) f(mode);
}

extern "C" void APIENTRY glTexCoord2f(GLfloat s, GLfloat t) {
    typedef void(APIENTRY* F)(GLfloat, GLfloat);
    static F f = (F)SP("glTexCoord2f");
    if (f) f(s, t);
}

extern "C" void APIENTRY glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* ptr) {
    typedef void(APIENTRY* F)(GLint, GLenum, GLsizei, const GLvoid*);
    static F f = (F)SP("glTexCoordPointer");
    if (f) f(size, type, stride, ptr);
}

extern "C" void APIENTRY glTexEnvf(GLenum target, GLenum pname, GLfloat param) {
    typedef void(APIENTRY* F)(GLenum, GLenum, GLfloat);
    static F f = (F)SP("glTexEnvf");
    if (f) f(target, pname, param);
}

extern "C" void APIENTRY glTexEnvfv(GLenum target, GLenum pname, const GLfloat* params) {
    typedef void(APIENTRY* F)(GLenum, GLenum, const GLfloat*);
    static F f = (F)SP("glTexEnvfv");
    if (f) f(target, pname, params);
}

extern "C" void APIENTRY glTexEnvi(GLenum target, GLenum pname, GLint param) {
    typedef void(APIENTRY* F)(GLenum, GLenum, GLint);
    static F f = (F)SP("glTexEnvi");
    if (f) f(target, pname, param);
}

extern "C" void APIENTRY glTexEnviv(GLenum target, GLenum pname, const GLint* params) {
    typedef void(APIENTRY* F)(GLenum, GLenum, const GLint*);
    static F f = (F)SP("glTexEnviv");
    if (f) f(target, pname, params);
}

extern "C" void APIENTRY glTexImage1D(GLenum target, GLint level, GLint internalformat,
    GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid* pixels) {
    typedef void(APIENTRY* F)(GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const GLvoid*);
    static F f = (F)SP("glTexImage1D");
    if (f) f(target, level, internalformat, width, border, format, type, pixels);
}

extern "C" void APIENTRY glTexImage2D(GLenum target, GLint level, GLint internalformat,
    GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels) {
    typedef void(APIENTRY* F)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*);
    static F f = (F)SP("glTexImage2D");
    if (f) f(target, level, internalformat, width, height, border, format, type, pixels);
}

extern "C" void APIENTRY glTexParameterf(GLenum target, GLenum pname, GLfloat param) {
    typedef void(APIENTRY* F)(GLenum, GLenum, GLfloat);
    static F f = (F)SP("glTexParameterf");
    if (f) f(target, pname, param);
}

extern "C" void APIENTRY glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params) {
    typedef void(APIENTRY* F)(GLenum, GLenum, const GLfloat*);
    static F f = (F)SP("glTexParameterfv");
    if (f) f(target, pname, params);
}

extern "C" void APIENTRY glTexParameteri(GLenum target, GLenum pname, GLint param) {
    typedef void(APIENTRY* F)(GLenum, GLenum, GLint);
    static F f = (F)SP("glTexParameteri");
    if (f) f(target, pname, param);
}

extern "C" void APIENTRY glTexParameteriv(GLenum target, GLenum pname, const GLint* params) {
    typedef void(APIENTRY* F)(GLenum, GLenum, const GLint*);
    static F f = (F)SP("glTexParameteriv");
    if (f) f(target, pname, params);
}

extern "C" void APIENTRY glTexSubImage2D(GLenum target, GLint level,
    GLint xoff, GLint yoff, GLsizei w, GLsizei h,
    GLenum format, GLenum type, const GLvoid* pixels) {
    typedef void(APIENTRY* F)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);
    static F f = (F)SP("glTexSubImage2D");
    if (f) f(target, level, xoff, yoff, w, h, format, type, pixels);
}

extern "C" void APIENTRY glTranslated(GLdouble x, GLdouble y, GLdouble z) {
    typedef void(APIENTRY* F)(GLdouble, GLdouble, GLdouble);
    static F f = (F)SP("glTranslated");
    if (f) f(x, y, z);
}

extern "C" void APIENTRY glTranslatef(GLfloat x, GLfloat y, GLfloat z) {
    typedef void(APIENTRY* F)(GLfloat, GLfloat, GLfloat);
    static F f = (F)SP("glTranslatef");
    if (f) f(x, y, z);
}

extern "C" void APIENTRY glVertex2f(GLfloat x, GLfloat y) {
    typedef void(APIENTRY* F)(GLfloat, GLfloat);
    static F f = (F)SP("glVertex2f");
    if (f) f(x, y);
}

extern "C" void APIENTRY glVertex3f(GLfloat x, GLfloat y, GLfloat z) {
    typedef void(APIENTRY* F)(GLfloat, GLfloat, GLfloat);
    static F f = (F)SP("glVertex3f");
    if (f) f(x, y, z);
}

extern "C" void APIENTRY glVertex3fv(const GLfloat* v) {
    typedef void(APIENTRY* F)(const GLfloat*);
    static F f = (F)SP("glVertex3fv");
    if (f) f(v);
}

extern "C" void APIENTRY glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* ptr) {
    typedef void(APIENTRY* F)(GLint, GLenum, GLsizei, const GLvoid*);
    static F f = (F)SP("glVertexPointer");
    if (f) f(size, type, stride, ptr);
}
