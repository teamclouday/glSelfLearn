#include "basic.hpp"

SDL_Window *myWindow = nullptr;
SDL_GLContext myContext = NULL;
Shader *myShader = nullptr;
GLuint VAO;
GLuint buffer;

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

    glCreateVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glCreateBuffers(1, &buffer);
    glNamedBufferStorage(buffer, 1024*1024, NULL, GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    static const GLfloat data[] = {
         0.25, -0.25, 0.5, 1.0,
         0.25,  0.25, 0.5, 1.0,
        -0.25, -0.25, 0.5, 1.0,
    };

    // glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), data);
    void *ptr = glMapNamedBuffer(buffer, GL_WRITE_ONLY);
    memcpy(ptr, data, sizeof(data));
    glUnmapNamedBuffer(buffer);

    glVertexArrayVertexBuffer(VAO, 0, buffer, 0, sizeof(GLfloat)*4);
    glVertexArrayAttribFormat(VAO, 0, 4, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(VAO, 0);

    glBindVertexArray(0);

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