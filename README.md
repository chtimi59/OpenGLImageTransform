OpenGLImageTransform
====================

An OpenGL headless command line to transform images

Tested on Windows (MSVisual 2010 + Cygwin)

Suppose to work on any platform

Abstract
========

A grid geometry is generated with an openGL texture
then by modifying this geometry openGL do magically the trick
the output is garbed from openGL FBO

This method gives excellent performance as it uses GPU and is headless


History
=======

2014/08 : Jan dOrgeville - first issue

Prequists
==========

- GLM 0.9.5.4
- GLEW 1.11.0   +CMAKE 3.0.0 (if you compile glew sources)

Note about Linux/Cygwin
=======================

The project makefile, may needs some adjustment according your setup
please adjusts "OPENGL_INCLUDE" "OPENGL_LIB" accordingly

one good practice is also create some locals symblink
```
ln -s your_install_dir/glew-1.11.0 glew
ln -s your_install_dir/glm-0.9.5.4/ glm
```

Note about Visual Studio
=========================

vs2010 project needs to be adjust according your setup
```
in "VC++ directories" section
> Include Directories:
    your_install_dir\openGL\glm-0.9.5.4
    your_install_dir\glew-1.11.0\include
> Lib Directories:
	your_install_dir\\glew-1.11.0\lib\Release\Win32
```

```
in "Linker" section
> Input:
	glew32s 
	opengl32
```
	
Sources :
=========
* glgrid.cpp/ glgrid.h    : 2D Buffer deformation system with a grid
* main.c                  : Very simple demo test


# Others sources files :
* log.c/log.h               : Generic Logging system (with debug level)
* xbuf.c / xbuf.h           : Methods to manage a generic 2D memory buffer
* gldebug.cpp / gldebug.h   : Tools to [create|save] openGL texture or XBUF buffer [from|to] BMP files
* glctx.h                   : OS Dependant (use to get an OPEN GL context)
* x11.cpp, w32.cpp, mac.cpp	: OS implementation of glctx methods
* common.h                  : basic type definitions and common header

Example :
=========

![Source Image](/image.bmp)

```c
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
        
		// The dummy transform
        r =  r * 0.07f;
        r =  r * r;
        a += 0.2f;
        if (r>800) r = 800;
        
        *pX = x0 + r*(float)sin(a);
        *pY = y0 + r*(float)cos(a);

		// Compute new dimensions bounding box
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

	// Update grid dimension
    grid->rect.x = minX;
    grid->rect.y = minY;
    grid->rect.width = maxX - minX;
    grid->rect.height = maxY - minY;
    IFRectFloat2Int(&grid->rect);
}
```

![Output Image](/result.bmp)

Have fun !



