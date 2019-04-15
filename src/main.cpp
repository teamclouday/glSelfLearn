#include "basic.hpp"

SDL_Window *myWindow = nullptr;
SDL_GLContext myContext = NULL;
Shader *myShader = nullptr;
GLuint VAO;

void renderAll()
{
    glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float tc = (float)SDL_GetTicks() / 1000.0f;
    GLfloat offset[] = {
        sin(tc)*0.5f,
        cos(tc)*0.5f,
        0.0f, 0.0f
    };

    myShader->use();
    
    glBindVertexArray(VAO);
    //glPatchParameteri(GL_PATCH_VERTICES, 3);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glVertexAttrib4fv(0, offset);
    //glPointSize(8.0f);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    SDL_GL_SwapWindow(myWindow);
}

int main(int argc, char *argv[])
{
    initAll();

    myShader = new Shader("./shaders/simple.vert",
                          //"./shaders/simple.tesc",
                          //"./shaders/simple.tese",
                          //"./shaders/simple.geom",
                          "./shaders/simple.frag");
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