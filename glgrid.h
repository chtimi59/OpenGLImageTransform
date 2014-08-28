#ifndef GLGRID_HEADER
#define GLGRID_HEADER

#include "common.h"
#include "math.h"
#include "xbuf.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ----------------------------------------------------------- */
/*                         IFRect methods                      */
/*                                                             */
/*  Abstract:                                                  */
/*  IFRect_t is a C struct which contains a Rect with FLOAT    */
/*  and INT format                                             */
/*                                                             */
/*  use IFRectInt2Float or IFRectFloat2Int to fill IFRect_t    */
/*  accordignly                                                */
/*                                                             */
/* ----------------------------------------------------------- */

typedef struct {
    float x;
    float y;
    float width;
    float height;

    int   ix;
    int   iy;
    int   iwidth;
    int   iheight;
} IFRect_t;

INLINE_METHOD void IFRectInt2Float(IFRect_t* rect) {
    rect->x = (float)rect->ix;
    rect->y = (float)rect->iy;
    rect->width = (float)rect->iwidth;
    rect->height = (float)rect->iheight;
}

INLINE_METHOD void IFRectFloat2Int(IFRect_t* rect) {
    rect->ix = (int)floor(rect->x);
    rect->iy = (int)floor(rect->y);
    rect->iwidth = (int)ceil(rect->width);
    rect->iheight = (int)ceil(rect->height);
}



/* ----------------------------------------------------------- */
/*                         Grid_t methods                      */
/* ----------------------------------------------------------- */

typedef struct {
    Buffer_t * src;
    IFRect_t   rect;
    int        count;
    float*     vertices; // count*3 : X Y Z
    float*     uvs;      // count*2 : U V    
} Grid_t;

typedef enum {
    RGB_TEXTURE_MODE,
    FLOAT_TEXTURE_MODE
} TextureMode_t;


Grid_t CreateGLGrid(Buffer_t * buffer, int rows);
void FreeGLGrid(Grid_t* grid);
Buffer_t * RenderGLGrid(Grid_t * grid, TextureMode_t mode);


#ifdef __cplusplus
}
#endif

#endif //GLGRID_HEADER