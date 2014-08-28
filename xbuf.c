#include "stdlib.h"
#include "xbuf.h"

//#define LOGENABLE
#include "log.h"

Buffer_t * CreateBuffer(int width, int height, int sample_sz) {
    Buffer_t* out     = (Buffer_t*)malloc(sizeof(Buffer_t));
    out->m.height     = height;
    out->m.width      = width;
    out->m.sample_sz  = sample_sz;
    out->m.pitch      = out->m.sample_sz * out->m.width;
    out->m.size       = out->m.pitch * out->m.height;
    out->payload      = (BYTE*)malloc(out->m.size);
    if (!out->payload) {
        DBGERROR("out of memory");
        FreeBuffer(out);
        return NULL;
    }
    return out;
}

Buffer_t * CreateBuffer2(int width, int height, int sample_sz, int pitch) {
    Buffer_t* out     = (Buffer_t*)malloc(sizeof(Buffer_t));
    out->m.height     = height;
    out->m.width      = width;
    out->m.sample_sz  = sample_sz;
    out->m.pitch      = pitch;
    out->m.size       = out->m.pitch * out->m.height;
    out->payload      = (BYTE*)malloc(out->m.size);
    if (!out->payload) {
        DBGERROR("out of memory");
        FreeBuffer(out);
        return NULL;
    }
    return out;
}

Buffer_t * CreateBuffer3(int width, int height, int sample_sz, int pitch, void* data) {
    Buffer_t* out     = (Buffer_t*)malloc(sizeof(Buffer_t));
    out->m.height     = height;
    out->m.width      = width;
    out->m.sample_sz  = sample_sz;
    out->m.pitch      = pitch;
    out->m.size       = out->m.pitch * out->m.height;
    out->payload      = (BYTE*)data;    
    return out;
}

void FreeBuffer(Buffer_t * buff) {
    if (!buff) return;
    if (buff->payload) free(buff->payload);
    free(buff);
}

int bufFillByte(Buffer_t* buf, BYTE v)
{
    // sanity checks
    if (!buf)    { DBGERROR("bufFill() error: no buffer\n"); return 1; }

    // fill payload
    memset(buf->payload, v, buf->m.size);
    return 0;
}
