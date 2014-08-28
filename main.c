#include "stdlib.h"
#include "math.h"
#define LOGENABLE
#include "log.h"

#include "glgrid.h"
#include "gldebug.h"





void dummy_transform(Grid_t* grid)
{
    // Rotation center
    float x0 = grid->rect.width/2;
    float y0 = grid->rect.height/2;

    float minX = 0;
    float maxX = 0;
    float minY = 0;
    float maxY = 0;
    int i;
    for (i = 0; i < grid->count; i++)
    {
        float * pX = &grid->vertices[3 * i + 0];
        float * pY = &grid->vertices[3 * i + 1];

        float x = *pX;
        float y = *pY;

        float r = (float)sqrt((x - x0)*(x - x0) + (y - y0)*(y - y0));
        float a = (float)atan2((x - x0),(y - y0));
        
        r =  r * 0.07f;
        r =  r * r;
        a += 0.2f;
        if (r>800) r = 800;
        
        *pX = x0 + r*(float)sin(a);
        *pY = y0 + r*(float)cos(a);

        if (i == 0) {
            minX = *pX;
            maxX = *pX;
            minY = *pY;
            maxY = *pY;
        } else {
            if (minX>*pX)  minX = *pX;
            if (maxX<*pX)  maxX = *pX;
            if (minY>*pY)  minY = *pY;
            if (maxY<*pY)  maxY = *pY;
        }
    }

    grid->rect.x = minX;
    grid->rect.y = minY;
    grid->rect.width = maxX - minX;
    grid->rect.height = maxY - minY;
    IFRectFloat2Int(&grid->rect);
}



int main(int argc, char** argv)
{
    int errorcode = 0;
    Buffer_t* in = NULL;
    Grid_t grid = {0};
    Buffer_t* out = NULL;

    LOG_SET(LOG_MAXLVL);

    for (;;) {
    
        // 1- read an image
        in = LoadBMPToXBUFF("image.bmp");
        if (NULL==in) {
            STDERR("Invalid input Image\n");
            errorcode = 1;
            break;
        }
        
        // 2- create a grid geometry (20x20)
        grid = CreateGLGrid(in, 20);
        if (0==grid.count) {
            STDERR("Invalid Grid\n");
            errorcode = 2;
            break;
        }
        
        // 3- deform grid
        dummy_transform(&grid);
        
        // 4- generate new buffer
        out = RenderGLGrid(&grid, RGB_TEXTURE_MODE);
        if (NULL==in) {
            STDERR("No output\n");
            errorcode = 3;
            break;
        }
        
        // 5- save output
        if (SaveBMPFromXBUFF("result.bmp", out)) {
            STDERR("Invalid output Image\n");
            errorcode = 1;
            break;
        }
        
        STDOUT("result.bmp generated !\n");
        break;
    }
    
    FreeGLGrid(&grid);    
    if (NULL!=in) FreeBuffer(in);
    if (NULL!=out) FreeBuffer(out);
    return errorcode;
}