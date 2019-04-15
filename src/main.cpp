#include "basic.hpp"

SDL_Window *myWindow = nullptr;
SDL_GLContext myContext = NULL;
Shader *myShader = nullptr;
GLuint VAO;

void renderAll()
{
    glClearColor(0.0f, 0.8f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float ct = (float)SDL_GetTicks() / 1000.0f;
    GLfloat attrib[] = {
        (float)sin(ct) * 0.5f,
        (float)cos(ct) * 0.6f,
        0.0f, 0.0f
    };

    GLfloat color[] = {
        (float)sin(ct)*0.5f+0.5f,
        (float)cos(ct)*0.5f+0.5f,
        0.0f, 1.0f
    };

    myShader->use();
    glBindVertexArray(VAO);
    glVertexAttrib4fv(0, attrib);
    glVertexAttrib4fv(1, color);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    SDL_GL_SwapWindow(myWindow);
}

int main(int argc, char *argv[])
{
    initAll();

    myShader = new Shader("./shaders/simple.vert", "./shaders/simple.frag");
    glCreateVertexArrays(1, &VAO);

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