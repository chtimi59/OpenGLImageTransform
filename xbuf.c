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

int bufRectCpy(Buffer_t* buffDst, Point_t dstPoint, Buffer_t* buffSrc, Rect_t srcRect)
{
    MemoryRect_t srcOrga = buffSrc->m;
    MemoryRect_t dstOrga = buffDst->m;
    unsigned long dgbTodoPixelCount = 0;
    unsigned long dgbDonePixelCount = 0;
    INT X,dstX,srcX;
    INT Y,dstY,srcY;
    CopyMode_t mode;

    // sanity checks
    if (!buffSrc) { DBGERROR("bufRectCpy() error: no source\n"); return 1; }
    if (!buffDst) { DBGERROR("bufRectCpy() error: no destination\n"); return 1; }

    // define copy
    srcOrga = buffSrc->m;
    dstOrga = buffDst->m;

    mode = rectCopyInit(&dstOrga, &dstPoint, &srcOrga, &srcRect, &dgbTodoPixelCount);
    
    DBGMODE(mode); 
    
    // Copying...
    for (Y = 0; Y < srcRect.height; Y++)
    {    
        void *d;
        void *s;
        size_t qty;

        srcY = srcRect.y + Y;
        dstY = dstPoint.y + Y;
        
        switch(mode)
        {
            case NOCPY:
                return 0;
                
            case MEMCPY: {
                size_t qty;
                srcX = srcRect.x;
                dstX = dstPoint.x;
                d  = buffDst->payload    + dstX*dstOrga.sample_sz + dstY*dstOrga.pitch;
                s  = buffSrc->payload    + srcX*srcOrga.sample_sz + srcY*srcOrga.pitch;
                qty = srcRect.height*dstOrga.pitch + srcRect.width*dstOrga.sample_sz;
                memcpy(d,s,qty);
                dgbDonePixelCount+=srcRect.width*srcRect.height;    
                return 0;
            }
            
            case ROWCPY: {
                srcX = srcRect.x;
                dstX = dstPoint.x;
                d  = buffDst->payload    + dstX*dstOrga.sample_sz + dstY*dstOrga.pitch;
                s  = buffSrc->payload    + srcX*srcOrga.sample_sz + srcY*srcOrga.pitch;
                qty = srcRect.width*dstOrga.sample_sz;
                memcpy(d,s,qty);
                dgbDonePixelCount+=srcRect.width;    
                break;
            }
            
            case PIXCPY:
            default:
                for (X = 0; X < (srcRect.width); X++) {
                    Color32_t c={{0}};
                    srcX = srcRect.x + X;
                    dstX = dstPoint.x + X;
                    bufReadPixel(buffSrc, (int)srcX, (int)srcY, &c);
                    bufWritePixel(buffDst, (int)dstX, (int)dstY, &c);
                    dgbDonePixelCount++;            
                }
                break;
        }
    }
            
    DBGMSG("bufRectCpy() %lu/%lu pixels done\n",dgbDonePixelCount,dgbTodoPixelCount);
    return 0;
}
