#include "basic.hpp"

SDL_Window *myWindow;
SDL_GLContext myContext;
glText *myText;
Shader *myShader;
Model *myModel;
Camera *myCamera;

void renderAll(Uint32 deltaT)
{
    int w, h;
    SDL_GetWindowSize(myWindow, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myCamera->update((float)deltaT);

    SDL_GL_SwapWindow(myWindow);
}

int main(int argc, char *argv[])
{
    initAll();

    myText = new glText("./fonts/roboto/Roboto-Regular.ttf", 48);

    myShader = new Shader();
    myShader->add("./shaders/filter.vert", GL_VERTEX_SHADER);
    myShader->add("./shaders/filter.frag", GL_FRAGMENT_SHADER);
    myShader->compile(false);

    myModel = new Model("./models/cooper_house");

    myCamera = new Camera(glm::vec3(0.0f, 0.0f, 10.0f));
    
    printf("%s\n", glGetString(GL_RENDERER));

    Uint32 tNow = SDL_GetTicks();
    Uint32 tPrev = SDL_GetTicks();
    bool quit = false;
    while(!quit)
    {
        quit = pollEvents();
        timer(&tNow, &tPrev);
        renderAll(tNow - tPrev);
    }

    destroyAll();
    return 0;
}