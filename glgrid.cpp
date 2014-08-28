#define GLEW_STATIC
#define GLEW_NO_GLU
#if !defined(_WIN32)
#define GLEWAPIENTRY __attribute__((__stdcall__))
#define GLAPIENTRY __attribute__((__stdcall__))
#endif
#include <GL/glew.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define LOGENABLE
#include "stdlib.h"
#include "common.h"
#include "glgrid.h"
#include "glctx.h"
#include "gldebug.h"


const GLchar * VERTEXSHADERCODE = 
"#version 330                                                \n"
"                                                            \n"
"layout(location = 0) in vec3 vertexPosition_modelspace;     \n"
"layout(location = 1) in vec2 vertexUV;                      \n"
"                                                            \n"
"uniform mat4 MVP;                                           \n"
"                                                            \n"
"// Output data ; will be interpolated for each fragment.    \n"
"out vec2 UV;                                                \n"
"                                                            \n"
"                                                            \n"
"void main()                                                 \n"
"{                                                           \n"
"    vec4 v = vec4(vertexPosition_modelspace,1);             \n"
"    gl_Position =  MVP * v;                                 \n"
"                                                            \n"
"    // UV of the vertex. No special space for this one.     \n"
"    UV = vertexUV;                                          \n"
"}                                                           \n";


const GLchar * FRAGSHADERCODE = 
"#version 150                                                \n"
"                                                            \n"
"// Interpolated values from the vertex shaders              \n"
"in vec2 UV;                                                 \n"
"                                                            \n"
"// Ouput data                                               \n"
"out vec3 color;                                             \n"
"                                                            \n"
"// Values that stay constant for the whole mesh.            \n"
"uniform sampler2D myTextureSampler;                         \n"
"                                                            \n"
"void main(){                                                \n"
"                                                            \n"
"    color = texture( myTextureSampler, UV ).rgb;            \n"
"}                                                           \n";

GLuint LoadShaders();





void FreeGLGrid(Grid_t* grid) {
    grid->count=0;
    if (NULL!=grid->vertices) free(grid->vertices);
    grid->vertices=NULL;
    if (NULL!=grid->uvs) free(grid->vertices);
    grid->uvs=NULL;
}


#define ADDGRIDPOINT(_X_,_Y_,_UX_,_UY_) do { \
    grid.vertices[i++] = (_X_); \
    grid.vertices[i++] = (_Y_); \
    grid.vertices[i++] = 0;     \
    grid.uvs[j++] = (_UX_); \
    grid.uvs[j++] = (_UY_); \
} while (0)

Grid_t CreateGLGrid(Buffer_t * buffer, int rows)
{
    Grid_t grid;
    grid.src = buffer;
    grid.rect.ix = 0;
    grid.rect.iy = 0;
    grid.rect.iwidth  = buffer->m.width;
    grid.rect.iheight = buffer->m.height;
    IFRectInt2Float(&grid.rect);
    
    float width  = grid.rect.width;
    float height  = grid.rect.height;
    float step = 1 /((float)rows);
    float stepx = width * step;
    float stepy = height * step;
    grid.count = rows * rows * 2 * 3;
    
    DBGMSG("CreateGLGrid() divide %0.1f x %0.1f in %d quad(s) => count=%d, steps=(%f,%f) \n",width,height,rows*rows,grid.count,stepx,stepy);
    
    grid.vertices = (float*)malloc(sizeof(float)* 3 * grid.count);
    if (NULL == grid.vertices) { FreeGLGrid(&grid); return grid; }

    grid.uvs = (float*)malloc(sizeof(float)* 2 * grid.count);
    if (NULL == grid.uvs)  { FreeGLGrid(&grid); return grid; }

    
    int i = 0, j = 0;
    for (int iy = 0; iy < rows; iy++)
    for (float ix = 0; ix < rows; ix++)
    {
        float x  = ix*stepx;
        float y  = iy*stepy;
        float ux = ix*step;
        float uy = iy*step;

        // TRIANGLE1
        ADDGRIDPOINT(x, y, ux, uy);
        ADDGRIDPOINT(x + stepx, y, ux + step, uy);
        ADDGRIDPOINT(x, y + stepy, ux, uy + step);

        // TRIANGLE2
        ADDGRIDPOINT(x, y + stepy, ux, uy + step);
        ADDGRIDPOINT(x + stepx, y, ux + step, uy);
        ADDGRIDPOINT(x + stepx, y + stepy, ux + step, uy + step);
    }

    return grid;
}

Buffer_t * RenderGLGrid(Grid_t * grid, TextureMode_t mode)
{
    Buffer_t * output = NULL;
    
    for (;;)
    {    
        // 1- Get GL Context
        if (CreateGLCtx(grid->rect.iwidth, grid->rect.iheight)) {
            DBGERROR("RenderGLGrid() Create GL Context failed\n");
            break; 
        }
        
        DBGMSG("RenderGLGrid() GL Context %dx%d OK\n", grid->rect.iwidth, grid->rect.iheight);
        
        // 2- GLEW init
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if (GLEW_OK != err) { DBGERROR("RenderGLGrid() Error: %s\n", glewGetErrorString(err)); break; }

        // 3- Projection matrix (orthographic)
        glm::mat4 Projection = glm::ortho(
            grid->rect.x, grid->rect.x + grid->rect.width,   // left, right
            grid->rect.y, grid->rect.y + grid->rect.height,  // bottom, top
            -5.0f, +5.0f                                     // near, far
            );

        // 4- Camera matrix
        glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 1), // position of camera, in world space
            glm::vec3(0, 0, 0), // look at, in world space
            glm::vec3(0, 1, 0)  // Head is up
            );


        // 5- Model geometry
        GLuint vertexbufferID;
        glGenBuffers(1, &vertexbufferID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferID);
        glBufferData(GL_ARRAY_BUFFER, grid->count * 3 * sizeof(float), grid->vertices, GL_STATIC_DRAW);

        GLuint uvbufferID;
        glGenBuffers(1, &uvbufferID);
        glBindBuffer(GL_ARRAY_BUFFER, uvbufferID);
        glBufferData(GL_ARRAY_BUFFER, grid->count * 2 * sizeof(float), grid->uvs, GL_STATIC_DRAW);

        // 6- Load Shader
        GLuint shaderID = LoadShaders();
        if (!shaderID) { DBGERROR("RenderGLGrid() LoadShaders Error\n"); break; }

        // 7- MVP matrix
        GLuint MatrixID = glGetUniformLocation(shaderID, "MVP");
        glm::mat4 Model = glm::mat4(1.0f);  // Model matrix : an identity
        glm::mat4 MVP = Projection * View * Model;
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        
        // 8- Load Texture
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        switch (mode) {
            case RGB_TEXTURE_MODE:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grid->src->m.width, grid->src->m.height, 0, GL_BGR, GL_UNSIGNED_BYTE, grid->src->payload);
                break;
            case FLOAT_TEXTURE_MODE:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grid->src->m.width, grid->src->m.height, 0, GL_RED, GL_FLOAT, grid->src->payload);
                break;
            default:
                DBGERROR("RenderGLGrid() Invalid Texture mode\n");
                break; 
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);

        GLint textureWidth1, textureHeight1;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &textureWidth1);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &textureHeight1);
        DBGMSG("RenderGLGrid() Input texture size %dx%d\n", textureWidth1, textureHeight1);
        if (textureWidth1*textureHeight1==0) {
            DBGERROR("RenderGLGrid() LoadTexture Error\n"); break; 
        }
        
        

        // FRAMEBUFFER RENDERING

        // 9- FrameBuffer
        GLuint FrameBufferID = 0;
        glGenFramebuffers(1, &FrameBufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferID);

        // 9.1- FrameBuffer - renderTexture
        GLuint FB_TextureID;
        glGenTextures(1, &FB_TextureID);
        glBindTexture(GL_TEXTURE_2D, FB_TextureID);
        switch (mode) {
            case RGB_TEXTURE_MODE:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grid->rect.iwidth, grid->rect.iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
                break;
            case FLOAT_TEXTURE_MODE:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grid->rect.iwidth, grid->rect.iheight, 0, GL_RED, GL_FLOAT, 0);
                break;
            default:
                DBGERROR("RenderGLGrid() Invalid Texture mode\n");
                break; 
        }      
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // 9.1- FrameBuffer - depth buffer
        GLuint FB_DepthID;
        glGenRenderbuffers(1, &FB_DepthID);
        glBindRenderbuffer(GL_RENDERBUFFER, FB_DepthID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, grid->rect.iwidth, grid->rect.iheight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, FB_DepthID);

        // 9.2- FrameBuffer Set "renderedTexture" as our colour attachement #0
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, FB_TextureID, 0);
        GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

        // 9.3- FrameBuffer check
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { DBGERROR("RenderGLGrid() FramebufferStatus Error\n"); break; }

        // 10 - Render to our framebuffer
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 10.1 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferID);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );

        // 10.1 2nd attribute buffer : UVs
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbufferID);
        glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            2,                                // size : U+V => 2
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
            );

        
        // 10.2 Use texture
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // 10.3 Draw the triangles !
        glUseProgram(shaderID);
        glDrawArrays(GL_TRIANGLES, 0, grid->count);

        // 11 READ BACK FRAMEBUFFER
        glBindTexture(GL_TEXTURE_2D, FB_TextureID);
        GLint textureWidth2, textureHeight2;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &textureWidth2);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &textureHeight2);
        DBGMSG("RenderGLGrid() Output texture size %dx%d\n", textureWidth2, textureHeight2);
        if (textureWidth2*textureHeight2==0) {
            DBGERROR("RenderGLGrid() FrameBuffer Texture Error\n"); break; 
        }
        
        int pitch = textureWidth2 * grid->src->m.sample_sz;
        int wpad = pitch % 4;
        if (wpad!=0) {
            pitch += 4-wpad;
        }
        
        output = CreateBuffer2(textureWidth2, textureHeight2, grid->src->m.sample_sz, pitch);    
        if (NULL == output)  { DBGERROR("RenderGLGrid() Output buffer memory Error\n"); break; }
        glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, output->payload);
        
        
        #ifdef DEBUG
        SaveBMPFromGLTEXTURE("dbgTextIn.bmp", textureID);
        SaveBMPFromGLTEXTURE("dbgTextOut.bmp", FB_TextureID);
        #endif
        
        break;
    }

    DeleteGLCtx();    
    return output;
}



/* ----------------------------------------------------------- */
/*                         SHADERs                             */
/* ----------------------------------------------------------- */

GLuint compileShader(GLenum shaderType, const GLchar* code) {
    GLuint shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, &code, NULL);
    glCompileShader(shaderID);
    GLint status;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
    return (status == GL_TRUE)?shaderID:0;
}

GLuint LoadShaders() {
    GLuint vID = compileShader(GL_VERTEX_SHADER, VERTEXSHADERCODE);
    GLuint fID = compileShader(GL_FRAGMENT_SHADER, FRAGSHADERCODE);
    if (vID && fID) {
        GLuint programID = glCreateProgram();
        glAttachShader(programID, vID);
        glAttachShader(programID, fID);
        glLinkProgram(programID);
        glUseProgram(programID);
        return programID;
    }
    return 0;
}









