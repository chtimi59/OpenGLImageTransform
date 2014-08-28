#ifndef XBUFF_HEADER
#define XBUFF_HEADER

#include "common.h"

typedef struct {
    MemoryRect_t m;
    BYTE*        payload;
} Buffer_t;

#ifdef __cplusplus
extern "C" {
#endif

Buffer_t * CreateBuffer(int width, int height, int sample_sz);
Buffer_t * CreateBuffer2(int width, int height, int sample_sz, int pitch);
Buffer_t * CreateBuffer3(int width, int height, int sample_sz, int pitch, void* data);
void FreeBuffer(Buffer_t * buff);

int bufRectCpy(Buffer_t* buffDst, Point_t dstPoint, Buffer_t* buffSrc, Rect_t srcRect);
int bufFillByte(Buffer_t* buf, BYTE v);



// ----------------------------------------------------
// -          MACROS AND INLINE METHODS               -
// ----------------------------------------------------

INLINE_METHOD int buf4BytesAlign(int v) {
   int wpad = v%4;
   if (0!=wpad) v += 4-wpad;
   return v;
}



INLINE_METHOD void bufWritePixel(Buffer_t* buf, int x, int y, Color32_t* c) {
    void* d = &buf->payload[x*buf->m.sample_sz + y*buf->m.pitch];
    memcpy(d,c,buf->m.sample_sz);
}

INLINE_METHOD void bufReadPixel(Buffer_t* buf, int x, int y, Color32_t* c) {
    void* s = &buf->payload[x*buf->m.sample_sz + y*buf->m.pitch];
    memcpy(c,s,buf->m.sample_sz);
}

INLINE_METHOD void bufWritePixelColor(Buffer_t* buf, int x, int y, int r, int g, int b, int a) {
    Color32_t c = {{0}};
    c.RGBA.r = r;
    c.RGBA.g = g;
    c.RGBA.b = b;
    c.RGBA.a = a;
    bufWritePixel(buf, x, y, &c);
}

INLINE_METHOD void bufWriteBlackPixel(Buffer_t* buf, int x, int y) {
    memset(&buf->payload[x*buf->m.sample_sz + y*buf->m.pitch], 0, buf->m.sample_sz);
}

INLINE_METHOD void bufWriteBlackRows(Buffer_t* buf, int y) {
    memset(&buf->payload[y*buf->m.pitch], 0, buf->m.width * buf->m.sample_sz);
}


#ifdef __cplusplus
}
#endif

#endif
