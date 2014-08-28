#include <windows.h>
#include <stdio.h>

HWND  hWnd  = NULL;
HDC   hdc   = NULL;
HGLRC hglrc = NULL;
HINSTANCE hInstance = NULL;

int CreateGLCtx(int width,int height)
{

    hInstance = GetModuleHandle(NULL);
    
    /* create an invisible memory windows with no Title,Btn and stuffs... */
    hWnd = CreateWindowEx(
            WS_EX_TOOLWINDOW,   /* dwExStyle */
            "STATIC",          /* lpClassName */
            NULL,               /* lpWindowName */
            WS_POPUP,           /* dwStyle */
            10,10,width,height,
            NULL,               /* hWndParent */
            NULL,               /* hMenu */
            hInstance,          /* hInstance */
            NULL                /* lpParam */
        );
    if(!hWnd) return 1;
    
    hdc = GetDC(hWnd);
            
    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
        PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
        32,                        //Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                        //Number of bits for the depthbuffer
        8,                        //Number of bits for the stencilbuffer
        0,                        //Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };
    
    int pixelFormat = ChoosePixelFormat(hdc,&pfd);
    SetPixelFormat(hdc,pixelFormat,&pfd);
    
    // CREATE OPEN GL CTX
    hglrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc,wglCreateContext(hdc));
    return 0;
}


void DeleteGLCtx() {
    wglMakeCurrent(hdc,NULL);
    if (hglrc) wglDeleteContext(hglrc);
    if (hWnd)  DestroyWindow(hWnd);
    hWnd = NULL;
    hdc = NULL;
}


  



