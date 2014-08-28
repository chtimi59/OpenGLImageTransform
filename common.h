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

#endif