#include "basic.hpp"

SDL_Window *myWindow;
SDL_GLContext myContext;
glText *myText;
Shader *myShader;
Model *myModel;
Camera *myCamera;
bool lineMode;

GLuint tex_toon;

void renderAll(float deltaT, float fps)
{
    int w, h;
    SDL_GetWindowSize(myWindow, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myCamera->update(deltaT, false);

    glm::mat4 proj_mat = glm::perspective(glm::radians(45.0f), (float)w/(float)h, 0.1f, 1000.0f);
    glm::mat4 mv_mat = myCamera->GetViewMatrix();
    myShader->use();
    glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "mv_mat"), 1, GL_FALSE, glm::value_ptr(mv_mat));
    glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "proj_mat"), 1, GL_FALSE, glm::value_ptr(proj_mat));
    myModel->draw(myShader->programID);
    myShader->disuse(); 

    char fpsNow[10];
    sprintf(fpsNow, "FPS: %.2f", fps);
    myText->render(std::string(fpsNow), 10.0f, (float)h-30.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f), true);

    SDL_GL_SwapWindow(myWindow);
}

int main(int argc, char *argv[])
{
    initAll();

    myText = new glText("./fonts/roboto/Roboto-Regular.ttf", 48);
    myModel = new Model("./models/medieval_village");
    myCamera = new Camera(glm::vec3(0.0f, 10.0f, 0.0f));

    myShader = new Shader();
    myShader->add("./shaders/toon.vert", GL_VERTEX_SHADER);
    myShader->add("./shaders/toon.frag", GL_FRAGMENT_SHADER);
    myShader->compile(false);

    static const GLubyte toon_tex_data[] = {
        0x44, 0x00, 0x00, 0x00,
        0x88, 0x00, 0x00, 0x00,
        0xCC, 0x00, 0x00, 0x00,
        0xFF, 0x00, 0x00, 0x00,
    };
    glGenTextures(1, &tex_toon);
    glBindTexture(GL_TEXTURE_1D, tex_toon);
    glTexStorage1D(GL_TEXTURE_1D, 1, GL_RGB8, sizeof(toon_tex_data) / 4);
    glTexSubImage1D(GL_TEXTURE_1D, 0, 0, sizeof(toon_tex_data) / 4, GL_RGBA, GL_UNSIGNED_BYTE, toon_tex_data);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    lineMode = false;

    printf("%s\n", glGetString(GL_RENDERER));

    Uint32 tNow = SDL_GetTicks();
    Uint32 tPrev = SDL_GetTicks();
    Uint32 tFrame = SDL_GetTicks();
    bool quit = false;
    unsigned frames = 0;
    float fpsNow = 0.0f;
    while(!quit)
    {
        quit = pollEvents();
        float deltaT = timer(&tNow, &tPrev);
        renderAll(deltaT, fpsNow);
        frames++;
        if((tNow - tFrame) > 1000)
        {
            fpsNow = frames / ((tNow - tFrame) / 1000.0f);
            tFrame = tNow;
            frames = 0;
        }
    }

    destroyAll();
    return 0;
}