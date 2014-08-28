#ifndef COMMON_HEADER
#define COMMON_HEADER

#include <string.h> // memcpy
#include "log.h"


#ifdef _WIN32
    typedef signed char  int8_t;
    typedef signed short int16_t;
    typedef signed int   int32_t;
    typedef unsigned char  uint8_t;
    typedef unsigned short uint16_t;
    typedef unsigned int   uint32_t;
    typedef signed long long   int64_t;
    typedef unsigned long long uint64_t;
    #define INLINE_METHOD __inline
#else
    #include <inttypes.h>
    #include <stdbool.h>
    #define INLINE_METHOD static inline
#endif


#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
typedef unsigned char BYTE;
typedef uint64_t      UINT;
typedef int64_t       INT;
#define PFUI          PRIu64
#define PFI           PRId64



typedef struct {
    int x;
    int y;
} Point_t;

typedef struct {
    int x;
    int y;
    int width;
    int height;
} Rect_t;

typedef union {
    struct {
        BYTE r;
        BYTE g;
        BYTE b;
        BYTE a;
    } RGBA;
    
    uint8_t  u8;    
    uint16_t u16;
    uint32_t u32;
    float    f;
    
} Color32_t;

typedef struct {
    int width;
    int height;
    int sample_sz; 
    int pitch;      // sample_sz * width
    int size;       // pitch * height    
} MemoryRect_t;






typedef enum {
    DBG_HEX,
    DBG_U8,
    DBG_U16,
    DBG_U32,
    DBG_FLOAT,    
} dbgShowBuff_Mode_t;

INLINE_METHOD void dbgShowMemory(MemoryRect_t o, void* p, dbgShowBuff_Mode_t m) {
    int i,x,y;
    for(y=0; y < o.height; y++) {
        for(x=0; x < o.width; x++) {
            Color32_t c = {{0}};
            unsigned char* pC = (unsigned char*)&c;
            long s = (long)p + x * o.sample_sz + y * o.pitch;
            memcpy(&c,(void*)s,o.sample_sz);
            switch(m)
            {
                case DBG_HEX:
                    for(i=0;i<o.sample_sz;i++)
                        fprintf(stderr, "%02X",pC[i]);
                    fprintf(stderr, " ");
                    break;
                case DBG_U8:
                    fprintf(stderr, "%03u ",c.u8);
                    break;
                case DBG_U16:
                    fprintf(stderr, "%03u ",c.u16);
                    break;    
                case DBG_U32:
                    fprintf(stderr, "%03u ",c.u32);
                    break;    
                case DBG_FLOAT:
                    fprintf(stderr, "%0.3f ",c.f);
                    break;    
            }                    
        }
        fprintf(stderr, "\n");
    }
}

#ifdef LOGENABLE
#define DBGMEMORY(_x1_,_x2_,_x3_) dbgShowMemory(_x1_,_x2_,_x3_)
#else
#define DBGMEMORY(_x1_,_x2_,_x3_)
#endif


INLINE_METHOD unsigned long CropRect(int maxX, int maxY, Point_t* origin, Rect_t* inout) {
    const int minX = 0;
    const int minY = 0;
    
    int startX  = origin->x;
    int stopX   = origin->x + inout->width;
    int startY    = origin->y;
    int stopY     = origin->y + inout->height;
    
    if (NULL==inout) return 0;
    startX = MAX(minX, startX);
    startX = MIN(maxX, startX);    
    stopX  = MAX(minX, stopX);
    stopX  = MIN(maxX, stopX);        

    inout->x      += (startX-origin->x);
    origin->x      += (startX-origin->x);
    inout->width  = (stopX-startX);
    

    startY = MAX(minY, startY);
    startY = MIN(maxY, startY);    
    stopY  = MAX(minY, stopY);
    stopY  = MIN(maxY, stopY);    
    
    inout->y      += (startY-origin->y);
    origin->y      += (startY-origin->y);
    inout->height  = (stopY-startY);
    
    return (inout->width * inout->height);
}




typedef enum {
    NOCPY,    /* no copy needed */
    MEMCPY,   /* a unique huge copy can be done */
    ROWCPY,   /* copy line per line */
    PIXCPY    /* copy pixel per pixel */
} CopyMode_t;

INLINE_METHOD CopyMode_t FindCopyMode(INT src_width, INT src_sample_sz, Rect_t* srcRect, INT dst_width, INT dst_sample_sz) {    
    if ((srcRect->width==0) || (srcRect->height ==0)) return NOCPY; // nothing to copy    
    if (src_sample_sz!=dst_sample_sz) return PIXCPY; // conversion needed :(    
    if ((src_width==srcRect->width) && (src_width==dst_width) && (src_width==srcRect->width)) return MEMCPY; // row match !
    return ROWCPY;
}




INLINE_METHOD CopyMode_t rectCopyInit(MemoryRect_t* dst, Point_t* dstPoint, MemoryRect_t* src, Rect_t* srcRect, unsigned long* count)
{
    CopyMode_t mode;
    unsigned long expectedPixelCount;
    Point_t origin;

    // sanity checks
    if (!dst)   { DBGERROR("rectCopyInit() error: no dest\n"); return NOCPY; }
    if (!src)   { DBGERROR("rectCopyInit() error: no src\n"); return NOCPY; }
    if (count) *count = 0;
    
    DBGMSG("CropRect before: src Rect [%d,%d,%d,%d] to (%d,%d) %\n",srcRect->x,srcRect->y,srcRect->width,srcRect->height, dstPoint->x, dstPoint->y);
    
    // Reduce RECT source scope according source and destination size
    CropRect(dst->width, dst->height, dstPoint, srcRect);
    DBGMSG("CropRect fit to dest > src Rect [%d,%d,%d,%d] to (%d,%d) %\n",srcRect->x,srcRect->y,srcRect->width,srcRect->height, dstPoint->x, dstPoint->y);
    origin.x = srcRect->x;
    origin.y = srcRect->y;
    expectedPixelCount = CropRect(src->width, src->height, &origin, srcRect);
    //DBGMSG("CropRect fit to source > src Rect [%d,%d,%d,%d] to (%d,%d) %\n",srcRect->x,srcRect->y,srcRect->width,srcRect->height, dstPoint->x, dstPoint->y);
    if (count) *count = expectedPixelCount;
    DBGMSG("CropRect after: src Rect [%d,%d,%d,%d] to (%d,%d) %\n",srcRect->x,srcRect->y,srcRect->width,srcRect->height, dstPoint->x, dstPoint->y);
    
    // Find right Copy method
    mode = FindCopyMode(src->width, src->sample_sz, srcRect, dst->width, dst->sample_sz);    
    return mode;    
}


INLINE_METHOD void dbgShowMode(CopyMode_t mode) {
    switch(mode) {
        case NOCPY:
            DBGMSG("copy mode: nothing to do\n");            
            break;                    
        case MEMCPY:
            DBGMSG("copy mode: ** FAST FAST FAST **\n");
            break;            
        case ROWCPY: 
            DBGMSG("copy mode: ** ROW ROW ROW **\n");
            break;                        
        case PIXCPY: 
            DBGMSG("copy mode: pixel per pixel\n");
            break;
    }
}    

#ifdef LOGENABLE
#define DBGMODE(_x_) dbgShowMode(_x_)
#else
#define DBGMODE(_x_)
#endif

#endif