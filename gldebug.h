#ifndef GLDEBUG_HEADER
#define GLDEBUG_HEADER

#define GLEW_STATIC
#define GLEW_NO_GLU
#if !defined(_WIN32)
#define GLEWAPIENTRY __attribute__((__stdcall__))
#define GLAPIENTRY __attribute__((__stdcall__))
#endif
#include <GL/glew.h>

#include "xbuf.h"

#ifdef __cplusplus
extern "C" {
#endif

GLuint     LoadBMPToGLTEXTURE(const char * imagepath);
Buffer_t*  LoadBMPToXBUFF(const char * imagepath);
int        SaveBMPFromGLTEXTURE(const char * imagepath, GLuint textureID);
int        SaveBMPFromXBUFF(const char * imagepath, Buffer_t * input);

#ifdef __cplusplus
}
#endif

#endif // GLDEBUG_HEADER