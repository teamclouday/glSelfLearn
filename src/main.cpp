#include "basic.hpp"

SDL_Window *myWindow;
SDL_GLContext myContext;
glText *myText;
Shader *myShader;
Model *myModel;
Camera *myCamera;

GLuint terrColor, terrShape;
GLuint VAO;

bool lineMode;

void renderAll(float deltaT, float fps)
{
    int w, h;
    SDL_GetWindowSize(myWindow, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myCamera->update(deltaT, false);

    glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), (float)w/(float)h, 0.1f, 1000.0f);
    glm::mat4 mv_mat = myCamera->GetViewMatrix();

    myShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrShape);
    glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "proj_mat"), 1, GL_FALSE, glm::value_ptr(proj_mat));
    glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "mv_mat"), 1, GL_FALSE, glm::value_ptr(mv_mat));
    glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "mvp_mat"), 1, GL_FALSE, glm::value_ptr(proj_mat * mv_mat));
    glBindVertexArray(VAO);
    if(lineMode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);
    myShader->disuse();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    char fpsNow[10];
    sprintf(fpsNow, "FPS: %.2f", fps);
    myText->render(std::string(fpsNow), 10.0f, (float)h-30.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f), true);

    SDL_GL_SwapWindow(myWindow);
}

int main(int argc, char *argv[])
{
    initAll();

    myText = new glText("./fonts/roboto/Roboto-Regular.ttf", 48);
    // myModel = new Model("./models/medieval_village");
    myCamera = new Camera(glm::vec3(0.0f, 10.0f, 20.0f));

    myShader = new Shader();
    myShader->add("./shaders/fog.vert", GL_VERTEX_SHADER);
    myShader->add("./shaders/fog.tesc", GL_TESS_CONTROL_SHADER);
    myShader->add("./shaders/fog.tese", GL_TESS_EVALUATION_SHADER);
    myShader->add("./shaders/fog.frag", GL_FRAGMENT_SHADER);
    myShader->compile(false);

    glGenVertexArrays(1, &VAO);

    glPatchParameteri(GL_PATCH_VERTICES, 4);

    terrShape = loadTexture("./images/terrain.jpg");
    terrColor = loadTexture("./images/face.png");
    glBindTextureUnit(0, terrShape);
    glBindTextureUnit(1, terrColor);

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