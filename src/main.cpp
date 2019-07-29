#include "basic.hpp"

SDL_Window *myWindow;
SDL_GLContext myContext;
glText *myText;
Shader *myShader;
Model *myModel;
GLuint VAO;
GLuint tex;

int display_mode;

void renderAll()
{
    int w, h;
    SDL_GetWindowSize(myWindow, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myShader->use();

    glBindTexture(GL_TEXTURE_2D, tex);
    glActiveTexture(GL_TEXTURE0);

    glUniform1iv(glGetUniformLocation(myShader->programID, "mode"), 1, &display_mode);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    myShader->disuse();

    if(display_mode)
        myText->render("Linear Filter", 10.0f, 10.0f, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f), true);
    else
        myText->render("HQ Filter", 10.0f, 10.0f, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f), true);

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

    glGenVertexArrays(1, &VAO);

    tex = loadTexture("./images/colors.png");
    
    display_mode = 1;
    
    printf("%s\n", glGetString(GL_RENDERER));

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