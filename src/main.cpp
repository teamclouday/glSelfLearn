#include "basic.hpp"

SDL_Window *myWindow;
SDL_GLContext myContext;
glText *myText;
Shader *myShader;
Model *myModel;
Camera *myCamera;

std::vector<Shader*> myShaders(4, nullptr);
int display_mode;

void renderAll(float deltaT, float fps)
{
    int w, h;
    SDL_GetWindowSize(myWindow, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myCamera->update(deltaT);

    glm::mat4 proj_mat = glm::perspective(45.0f, ((float)w/(float)h), 0.1f, 1000.0f);
    glm::mat4 view_mat = myCamera->GetViewMatrix();
    glm::mat4 mv_mat = glm::mat4(1.0f);
    mv_mat = glm::translate(mv_mat, glm::vec3(0.0f, -40.0f, -100.0f));
    mv_mat = glm::scale(mv_mat, glm::vec3(myCamera->mv_zoom));
    mv_mat = view_mat * mv_mat;

    Shader *thisShader = myShaders[display_mode-1];
    thisShader->use();

    glUniformMatrix4fv(glGetUniformLocation(thisShader->programID, "mv_mat"), 1, GL_FALSE, glm::value_ptr(mv_mat));
    glUniformMatrix4fv(glGetUniformLocation(thisShader->programID, "view_mat"), 1, GL_FALSE, glm::value_ptr(view_mat));
    glUniformMatrix4fv(glGetUniformLocation(thisShader->programID, "proj_mat"), 1, GL_FALSE, glm::value_ptr(proj_mat));

    myModel->draw(thisShader->programID);

    thisShader->disuse();

    switch(display_mode)
    {
        case 1:
            myText->render("Gouraud Shading (vertex based)", 10.0f, 10.0f, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f), true);
            break;
        case 2:
            myText->render("Phong Shading (pixel based)", 10.0f, 10.0f, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f), true);
            break;
        case 3:
            myText->render("Blinn-Phong Lighting (optimized Phong)", 10.0f, 10.0f, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f), true);
            break;
    }

    static char fpsStr[10];
    sprintf(fpsStr, "FPS: %.2f", fps);
    myText->render(std::string(fpsStr), 10.0f, (float)h - 30.0f, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f), true);

    SDL_GL_SwapWindow(myWindow);
}

int main(int argc, char *argv[])
{
    initAll();

    myText = new glText("./fonts/roboto/Roboto-Regular.ttf", 48);
    myModel = new Model("./models/medieval_village");
    myCamera = new Camera(glm::vec3(0.0f, 0.0f, 80.0f));

    // init shaders
    // [1] Gouraud Shading
    myShaders[0] = new Shader();
    myShaders[0]->add("./shaders/gouraud.vert", GL_VERTEX_SHADER);
    myShaders[0]->add("./shaders/gouraud.frag", GL_FRAGMENT_SHADER);
    myShaders[0]->compile(false);
    // [2] Phong Shading
    myShaders[1] = new Shader();
    myShaders[1]->add("./shaders/phong.vert", GL_VERTEX_SHADER);
    myShaders[1]->add("./shaders/phong.frag", GL_FRAGMENT_SHADER);
    myShaders[1]->compile(false);
    // [3] Blinn-Phong Lighting
    myShaders[2] = new Shader();
    myShaders[2]->add("./shaders/phong.vert", GL_VERTEX_SHADER);
    myShaders[2]->add("./shaders/phongblinn.frag", GL_FRAGMENT_SHADER);
    myShaders[2]->compile(false);
    
    display_mode = 1;

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