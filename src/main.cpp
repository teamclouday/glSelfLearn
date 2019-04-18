#include "basic.hpp"

SDL_Window *myWindow = nullptr;
SDL_GLContext myContext = NULL;
Shader *myShader = nullptr;
GLuint VAO;
GLuint buffer[2];

void renderAll()
{
    glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // float tc = (float)SDL_GetTicks() / 1000.0f;

    myShader->use();
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    SDL_GL_SwapWindow(myWindow);
}

int main(int argc, char *argv[])
{
    initAll();

    myShader = new Shader();
    myShader->add("./shaders/simple.vert", GL_VERTEX_SHADER);
    myShader->add("./shaders/simple.frag", GL_FRAGMENT_SHADER);
    myShader->compile();

    static const GLfloat positions[] = {
         0.25, -0.25, 0.5, 1.0,
         0.25,  0.25, 0.5, 1.0,
        -0.25, -0.25, 0.5, 1.0,
    };

    static const GLfloat colors[] = {
        0.5, 0.8, 1.0, 1.0,
        0.3, 0.2, 0.6, 1.0,
        0.7, 0.7, 0.2, 1.0,
    };

    glCreateVertexArrays(1, &VAO);

    glCreateBuffers(2, &buffer[0]);

    glNamedBufferStorage(buffer[0], sizeof(positions), positions, 0);
    glVertexArrayVertexBuffer(VAO, 0, buffer[0], 0, sizeof(GLfloat)*4);
    glVertexArrayAttribFormat(VAO, 0, 4, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(VAO, 0, 0);
    glEnableVertexArrayAttrib(VAO, 0);

    glNamedBufferStorage(buffer[1], sizeof(colors), colors, 0);
    glVertexArrayVertexBuffer(VAO, 1, buffer[1], 0, sizeof(GLfloat)*4);
    glVertexArrayAttribFormat(VAO, 1, 4, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(VAO, 1, 1);
    glEnableVertexArrayAttrib(VAO, 1);

    static const GLchar *uniformNames[4] = {
        "TransformBlock.scale",
        "TransformBlock.translation",
        "TransformBlock.rotation",
        "TransformBlock.projection_matrix",
    };
    GLuint uniformIndices[4];
    glGetUniformIndices(myShader->programID, 4, uniformNames, uniformIndices);

    GLint uniformOffsets[4];
    GLint arrayStrides[4];
    GLint matrixStrides[4];
    glGetActiveUniformsiv(myShader->programID, 4,uniformIndices, GL_UNIFORM_OFFSET, uniformOffsets);
    glGetActiveUniformsiv(myShader->programID, 4,uniformIndices, GL_UNIFORM_ARRAY_STRIDE, arrayStrides);
    glGetActiveUniformsiv(myShader->programID, 4,uniformIndices, GL_UNIFORM_MATRIX_STRIDE, matrixStrides);

    Uint32 tNow = SDL_GetTicks();
    Uint32 tPrev = SDL_GetTicks();
    bool quit = false;
    while(!quit)
    {
        timer(&tNow, &tPrev);
        renderAll();
        quit = pollEvents();
    }

    destroyAll();
    return 0;
}