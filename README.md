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

GLM 0.9.5.4
GLEW 1.11.0   +CMAKE 3.0.0 (if you compile glew sources)

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


Others sources files :
======================
* log.c/log.h             : Generic Logging system (with debug level)
* xbuf.c / xbuf.h         : Methods to manage a generic 2D memory buffer
* gldebug.cpp / gldebug.h : Tools to [create|save] openGL texture or XBUF buffer [from|to] BMP files

* glctx.h                 : OS Dependant (use to get an OPEN GL context)
* x11.cpp, w32.cpp, mac.cpp
						  
* common.h                : basic type definitions and common header


Have fun !



