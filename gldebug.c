#define GLEW_STATIC
#define GLEW_NO_GLU
#if !defined(_WIN32)
#define GLEWAPIENTRY __attribute__((__stdcall__))
#define GLAPIENTRY __attribute__((__stdcall__))
#endif
#include <GL/glew.h>

#include <stdlib.h> 
#include "log.h"

#include "xbuf.h"

#ifndef _WIN32
#include <unistd.h>
#else
#pragma warning(disable: 4996) // unsafe methods
#endif



/* BMP HEADER */
#pragma pack(push)
#pragma pack(1)
typedef struct {
   unsigned short bfType;            /* Magic identifier            */
   unsigned long  bfSize;            /* File size in bytes          */
   unsigned short bfReserved1;
   unsigned short bfReserved2;
   unsigned long  bfOffBits;         /* Offset to image data, bytes */
} HEADER;

typedef struct {
   unsigned long biSize;            /* Header size in bytes      */
   long biWidth;                     /* Width and height of image */
   long biHeight;
   short biPlanes;                   /* Number of colour planes   */
   short biBitCount;                 /* Bits per pixel            */
   unsigned long biCompression;      /* Compression type          */
   unsigned long biSizeImage;        /* Image size in bytes       */
   long biXPelsPerMeter;             /* Pixels per meter          */
   long biYPelsPerMeter;
   unsigned long biClrUsed;         /* Number of colours         */
   unsigned long biClrImportant;    /* Important colours         */
} INFOHEADER;
#pragma pack(pop)


// privates methods
int LoadBMP(const char * imagepath, HEADER* pHeader, INFOHEADER* pInfoHeader, unsigned char ** pData );
int SaveBMP(const char * imagepath, int width, int height, int pitch, unsigned char* data);



int SaveBMPFromXBUFF(const char * imagepath, Buffer_t * input) {

    if (NULL==input) {
        DBGERROR("SaveBMP: Buffer Error\n");
        return 1;
    }

    return SaveBMP(imagepath, input->m.width, input->m.height, input->m.pitch, input->payload);
}

int SaveBMPFromGLTEXTURE(const char * imagepath, GLuint textureID)
{
    int errorcode = 0;
    GLint width, height;
    int pitch = 0;
    unsigned char* data = NULL;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    if (width*height==0) {
        DBGERROR("SaveBMP: LoadTexture Error\n");
        return 1;
    }
    
    pitch = buf4BytesAlign(width*3);
    data = (unsigned char*)malloc(height*pitch);
    if (NULL==data) {
        DBGERROR("SaveBMP: No more memory\n");
        return 2;
    }
        
    glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    errorcode = SaveBMP(imagepath, width, height, pitch, data);
    free(data);
    
    return errorcode;
}

int SaveBMP(const char * imagepath, int width, int height, int pitch, unsigned char* data)
{
    int errorcode = 0;
    HEADER header = {0};
    INFOHEADER infoHeader = {0};
    FILE * file;

    DBGMSG("Saving image '%s'\n", imagepath);

    infoHeader.biSize = 40; // 40bytes DIP INFO
    infoHeader.biWidth = width;
    infoHeader.biHeight = height;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 24; // 24bits RGB
    infoHeader.biCompression = 0; // no compression
    infoHeader.biSizeImage = pitch * infoHeader.biHeight;
    infoHeader.biXPelsPerMeter = 2834; // 2834 pixels per meters (why not)
    infoHeader.biYPelsPerMeter = 2834; // 2834 pixels per meters (why not)
    infoHeader.biClrUsed = 0; // no palette
    infoHeader.biClrImportant = 0; // no palette
    
    header.bfType = 0x4d42;
    header.bfOffBits = sizeof(HEADER) + sizeof(INFOHEADER) + infoHeader.biSizeImage;
    header.bfOffBits = sizeof(HEADER) + sizeof(INFOHEADER);

    file = fopen(imagepath, "wb");
    if (!file) {
        DBGERROR("SaveBMP: Can't create %s\n", imagepath);        
        return 1;
    }

    for (;;)
    {        
        // Write headers
        if ((fwrite(&header, 1, sizeof(header), file) != sizeof(header)) || (fwrite(&infoHeader, 1, sizeof(infoHeader), file) != sizeof(infoHeader)) ) {
            DBGERROR("SaveBMP: Write BMP header failed\n");
            errorcode = 1;
            break;
        }

        if (NULL==data) {
            printf("SaveBMP: No more memory\n");
            errorcode = 2;
            break;
        }
        
        

        // Write datas
        {
           int x,y;
           size_t r;

           for (y = 0; y<infoHeader.biHeight; y++) {
                for (x = 0; x<infoHeader.biWidth; x++) {
                    unsigned char* dst = &data[x*3 + y*pitch];
                    unsigned char* src = &data[x*3 + y*pitch];
                
                    /* RGB */
                    unsigned char R = src[0];
                    unsigned char G = src[1];
                    unsigned char B = src[2];
                
                    /* BMP format */
                    dst[0] = B; //BMP blue
                    dst[1] = G; //BMP Green
                    dst[2] = R; //BMP Red
                }
            }

            r = fwrite(data, 1, infoHeader.biSizeImage, file);
            if (r!=infoHeader.biSizeImage) {
                DBGERROR("Image error\n");
                errorcode = 3;
                break;
            }
        }

        DBGMSG("Save image success %dx%d (%d bytes)\n", infoHeader.biWidth, infoHeader.biHeight, infoHeader.biSizeImage);        
    
        break;
    }
    
    fclose(file);
    
    if (0!=errorcode) {
        unlink(imagepath);
    }
    
    return errorcode;
}



Buffer_t* LoadBMPToXBUFF(const char * imagepath)
{
    HEADER header = {0};
    INFOHEADER infoHeader = {0};
    unsigned char * data = NULL;

    int pitch = 0;

    if(LoadBMP(imagepath, &header, &infoHeader, &data)) {
        DBGERROR("Failed to LoadBMP\n");
        return NULL;
    }
        
    pitch = buf4BytesAlign(infoHeader.biWidth*3);
    return CreateBuffer3(infoHeader.biWidth, infoHeader.biHeight, 3, pitch, data);    
}

GLuint LoadBMPToGLTEXTURE(const char * imagepath)
{
    HEADER header = {0};
    INFOHEADER infoHeader = {0};
    unsigned char * data = NULL;
    GLuint textureID; 

    if(LoadBMP(imagepath, &header, &infoHeader, &data)) {
        DBGERROR("Failed to LoadBMP\n");
        return 0;
    }
   
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, infoHeader.biWidth, infoHeader.biHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    free(data); data = NULL; // OpenGL has now copied the data.
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
        
    return textureID;    
}

int LoadBMP(const char * imagepath, HEADER* pHeader, INFOHEADER* pInfoHeader, unsigned char ** pData )
{
    int errocode = 0;
    HEADER header = {0};
    INFOHEADER infoHeader = {0};
    unsigned char * data = NULL;
    FILE * file;

    DBGMSG("Reading image '%s'\n", imagepath);

    file = fopen(imagepath, "rb");
    if (!file) {
        DBGERROR("%s not found\n", imagepath);
        return 1;
    }

    for (;;)
    {
        // Read Headers
        if ( (fread(&header, 1, sizeof(header), file) != sizeof(header)) || (fread(&infoHeader, 1, sizeof(infoHeader), file) != sizeof(infoHeader)) ) {
            DBGERROR("Invalid BMP header\n");
            errocode = 2;
            break; 
        }
        if (header.bfType!= 0x4d42) {
            DBGERROR("Not BM Windows 3.1x, 95, NT bmp type\n");
            errocode = 3;
            break;
        }
        if (infoHeader.biSize!= 40) {
            DBGERROR("DIB header not supported (expected 40 bytes BMP INFO)\n");
            errocode = 4;
            break;
        }
        if (infoHeader.biPlanes!= 1) {
            DBGERROR("Invalid number of Planes\n");
            errocode = 5;
            break;
        }
        if (infoHeader.biBitCount!= 24) {
            DBGERROR("24bits RGB images only\n");
            errocode = 6;
            break;
        }
        if (infoHeader.biCompression!= 0) {
            DBGERROR("Compression not supported\n");
            errocode = 7;
            break;
        }
        if (infoHeader.biClrUsed!= 0) {
            DBGERROR("Color Palette not supported\n");
            errocode = 8;
            break;
        }

        // Guess missing information
        if (infoHeader.biSizeImage == 0)    infoHeader.biSizeImage = infoHeader.biWidth * infoHeader.biHeight * 3;
        if (header.bfOffBits == 0)          header.bfOffBits = sizeof(HEADER) + sizeof(INFOHEADER);

        // Read datas
        {
            int x,y;
            int pitch;
            size_t r;
            
            fseek(file, header.bfOffBits, SEEK_SET);
            data = (unsigned char*)malloc(infoHeader.biSizeImage);
            if (NULL==data) {        
                DBGERROR("No more memory\n");
                errocode = 9;
                break;
            }

            r=fread(data, 1, infoHeader.biSizeImage, file);
            if (r!=infoHeader.biSizeImage) {
                free(data);
                DBGERROR("Image error\n");
                errocode = 10;
                break;
            }

            pitch = buf4BytesAlign(infoHeader.biWidth*3);

            for (y = 0; y<infoHeader.biHeight; y++) {
                for (x = 0; x<infoHeader.biWidth; x++) {
                    unsigned char* dst = &data[x*3 + y*pitch];
                    unsigned char* src = &data[x*3 + y*pitch];
                
                    /* BMP format */
                    unsigned char B = src[0]; //BMP blue
                    unsigned char G = src[1]; //BMP Green
                    unsigned char R = src[2]; //BMP Red
                
                    /* RGB */
                    dst[0] = R;
                    dst[1] = G;
                    dst[2] = B;
                }
            }
        }
        break;
    }
    
    fclose(file);   
    if (0 == errocode) {
        *pHeader = header;
        *pInfoHeader = infoHeader;
        *pData = data;        
        DBGMSG("Read image success %dx%d\n", infoHeader.biWidth,infoHeader.biHeight);
    }
    return errocode;
}