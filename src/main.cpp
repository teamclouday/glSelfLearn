#include "basic.hpp"

SDL_Window *myWindow = nullptr;
SDL_GLContext myContext = NULL;
Shader *myShader = nullptr;
GLuint VAO;
GLuint buffer;

void renderAll()
{
    glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearBufferfi(GL_DEPTH_STENCIL, 0.0f, 1.0f, 0.0f);

    // float currentTime = (float)SDL_GetTicks() / 1000.0f;

    myShader->use();

    glBindVertexArray(VAO);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

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

    static const GLfloat vertex_data[] = {
        -0.5f,  0.5f, 0.5f, 1.0f,
        0.5f, 0.5f, 0.3f, 1.0f,
         0.5f, -0.5f, 0.5f, 1.0f,
        0.9f, 0.3f, 0.2f, 1.0f,
         0.5f,  0.5f, 0.5f, 1.0f,
        0.3f, 1.0f, 0.4f, 1.0f,
    };

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vertex_data), vertex_data, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    

    Uint32 tNow = SDL_GetTicks();
    Uint32 tPrev = SDL_GetTicks();
    bool quit = false;
    while(!quit)
    {
        quit = pollEvents();
        timer(&tNow, &tPrev);
        renderAll();
    }

    destroyAll();
    return 0;
}