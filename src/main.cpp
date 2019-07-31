#include "basic.hpp"

SDL_Window *myWindow;
SDL_GLContext myContext;
glText *myText;
Shader *myShader;
Model *myModel;
Camera *myCamera;

std::vector<Shader*> myShaders(9, nullptr);
int display_mode;
GLuint texSphe, texRect, texCubeMap, cubeVAO;

void renderAll(float deltaT, float fps)
{
    int w, h;
    SDL_GetWindowSize(myWindow, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float tt = (float)SDL_GetTicks() * 0.0001f;

    myCamera->update(deltaT);

    glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), ((float)w/(float)h), 0.1f, 1000.0f);
    // glm::mat4 view_mat = glm::lookAt(glm::vec3(20.0f*sinf(tt), 0.0f, 20.0f*cosf(tt)), glm::vec3(0.0f, -10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 view_mat = myCamera->GetViewMatrix();
    glm::mat4 mv_mat(1.0f);
    mv_mat = glm::rotate(mv_mat, glm::radians(-120.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    mv_mat = glm::translate(mv_mat, glm::vec3(10.0f, -20.0f, -70.0f));
    mv_mat = glm::scale(mv_mat, glm::vec3(0.4f));
    mv_mat = view_mat * mv_mat;

    Shader *thisShader = myShaders[display_mode-1];

    if(display_mode == 7 || display_mode == 8)
    {
        glm::mat4 cube_mat = glm::mat4(glm::mat3(view_mat));
        glDepthMask(GL_FALSE);
        myShader->use();
        glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "view_mat"), 1, GL_FALSE, glm::value_ptr(cube_mat));
        glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "proj_mat"), 1, GL_FALSE, glm::value_ptr(proj_mat));
        glBindTexture(GL_TEXTURE_CUBE_MAP, texCubeMap);
        glBindVertexArray(cubeVAO);
        // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        myShader->disuse();
        glDepthMask(GL_TRUE);
    }

    if(display_mode == 5)
        glBindTexture(GL_TEXTURE_2D, texSphe);
    else if(display_mode == 6)
        glBindTexture(GL_TEXTURE_2D, texRect);

    thisShader->use();

    if(display_mode == 7 || display_mode == 8)
    {
        int indicator = (display_mode == 7) ? 1 : -1;
        glUniform1iv(glGetUniformLocation(thisShader->programID, "indicator"), 1, &indicator);
    }

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
        case 4:
            myText->render("Phong + Rim Lighting", 10.0f, 10.0f, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f), true);
            break;
        case 5:
            myText->render("Spherical Environment Mapping", 10.0f, 10.0f, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f), true);
            break;
        case 6:
            myText->render("Equirectangular Environment Mapping", 10.0f, 10.0f, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f), true);
            break;
        case 7:
            myText->render("Cubemap Environment Mapping (Reflection)", 10.0f, 10.0f, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f), true);
            break;
        case 8:
            myText->render("Cubemap Environment Mapping (Refraction)", 10.0f, 10.0f, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f), true);
            break;
        case 9:
            myText->render("No Shading", 10.0f, 10.0f, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f), true);
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
    myCamera = new Camera(glm::vec3(0.0f, 10.0f, 20.0f));

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
    // [4] Phong + Rim Lighting
    myShaders[3] = new Shader();
    myShaders[3]->add("./shaders/phong.vert", GL_VERTEX_SHADER);
    myShaders[3]->add("./shaders/phongrim.frag", GL_FRAGMENT_SHADER);
    myShaders[3]->compile(false);
    // [5] Spherical Env Mapping
    myShaders[4] = new Shader();
    myShaders[4]->add("./shaders/env.vert", GL_VERTEX_SHADER);
    myShaders[4]->add("./shaders/envsphe.frag", GL_FRAGMENT_SHADER);
    myShaders[4]->compile(false);
    // [6] Equirectangular Env Mapping
    myShaders[5] = new Shader();
    myShaders[5]->add("./shaders/env.vert", GL_VERTEX_SHADER);
    myShaders[5]->add("./shaders/envrect.frag", GL_FRAGMENT_SHADER);
    myShaders[5]->compile(false);
    // [7] Cubemap Env Mapping (reflecting)
    myShaders[6] = new Shader();
    myShaders[6]->add("./shaders/env.vert", GL_VERTEX_SHADER);
    myShaders[6]->add("./shaders/envcube.frag", GL_FRAGMENT_SHADER);
    myShaders[6]->compile(false);
    // [8] refracting
    myShaders[7] = new Shader();
    myShaders[7]->add("./shaders/env.vert", GL_VERTEX_SHADER);
    myShaders[7]->add("./shaders/envcube.frag", GL_FRAGMENT_SHADER);
    myShaders[7]->compile(false);
    // [9] no light
    myShaders[8] = new Shader();
    myShaders[8]->add("./shaders/none.vert", GL_VERTEX_SHADER);
    myShaders[8]->add("./shaders/none.frag", GL_FRAGMENT_SHADER);
    myShaders[8]->compile(false);

    // shader for cubemap rendering
    myShader = new Shader();
    myShader->add("./shaders/cube.vert", GL_VERTEX_SHADER);
    myShader->add("./shaders/cube.frag", GL_FRAGMENT_SHADER);
    myShader->compile(false);

    texSphe = loadTexture("./images/envmap2.jpg");
    glBindTextureUnit(0, texSphe);
    texRect = loadTexture("./images/envmap3.png");
    glBindTextureUnit(0, texRect);

    std::vector<std::string> cubemap_path = {
        "./images/envmap_stormydays/rt.jpg",
        "./images/envmap_stormydays/lf.jpg",
        "./images/envmap_stormydays/up.jpg",
        "./images/envmap_stormydays/dn.jpg",
        "./images/envmap_stormydays/bk.jpg",
        "./images/envmap_stormydays/ft.jpg",
    };
    texCubeMap = loadCubeMap(cubemap_path);
    glBindTextureUnit(0, texCubeMap);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glGenVertexArrays(1, &cubeVAO);
    
    display_mode = 9;

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