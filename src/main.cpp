#include "basic.hpp"

SDL_Window *myWindow;
SDL_GLContext myContext;
glText *myText;
Shader *myShader;
Model *myModel;
Camera *myCamera;

Shader *lightCubeShader;
Shader *lightShader;
GLuint lightVAO, sceneVAO;

GLuint shadowFBO, shadowTex, shadowColorTex;
#define DEPTH_TEX_WIDTH     512
#define DEPTH_TEX_HEIGHT    512

Model *otherModel;

void renderAll(float deltaT, float fps)
{
    int w, h;
    SDL_GetWindowSize(myWindow, &w, &h);

    myCamera->update(deltaT, false);

    // float tt = (float)SDL_GetTicks() * 0.001f;

    glm::mat4 view_mat = myCamera->GetViewMatrix();
    glm::mat4 proj_mat = glm::perspective(glm::radians(50.0f), (float)w/(float)h, 0.1f, 500.0f);
    glm::vec3 lightPos = glm::vec3(0.0f, 10.0f, -20.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::mat4 light_model_mat(1.0f);
    light_model_mat = glm::translate(glm::scale(light_model_mat, glm::vec3(myCamera->mv_zoom)), lightPos);
    static const glm::mat4 light_proj_mat = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 200.0f);
    static const glm::mat4 light_view_mat = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    static const glm::mat4 scale_bias_mat = glm::mat4(glm::vec4(0.5f, 0.0f, 0.0f, 0.0f),
                                                      glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
                                                      glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
                                                      glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    glm::mat4 shadow_mat = scale_bias_mat * light_proj_mat * light_view_mat;
    glm::mat4 model_mat(1.0f);
    model_mat = glm::scale(model_mat, glm::vec3(myCamera->mv_zoom));
    model_mat = glm::translate(model_mat, glm::vec3(0.0f, -30.0f, -150.0f));
    glm::mat4 model_mat2(1.0f);
    model_mat2 = glm::scale(model_mat2, glm::vec3(0.01f));


    // render for shadow depth
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glViewport(0, 0, DEPTH_TEX_WIDTH, DEPTH_TEX_HEIGHT);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(4.0f, 4.0f);
    lightShader->use();
    static const GLenum buffs[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, buffs);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniformMatrix4fv(glGetUniformLocation(lightShader->programID, "mvp"), 1, GL_FALSE, glm::value_ptr(light_proj_mat * light_view_mat * model_mat));
    myModel->draw(lightShader->programID);

    glUniformMatrix4fv(glGetUniformLocation(lightShader->programID, "mvp"), 1, GL_FALSE, glm::value_ptr(light_proj_mat * light_view_mat * model_mat2));
    otherModel->draw(lightShader->programID);

    glDisable(GL_POLYGON_OFFSET_FILL);
    lightShader->disuse();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glViewport(0, 0, w, h);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw model in the scene
    myShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glDrawBuffer(GL_BACK);
    glClearDepth(1);
    glClear(GL_DEPTH_BUFFER_BIT);

    glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "mv_mat"), 1, GL_FALSE, glm::value_ptr(view_mat * model_mat));
    glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "proj_mat"), 1, GL_FALSE, glm::value_ptr(proj_mat));
    glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "shadow_mat"), 1, GL_FALSE, glm::value_ptr(shadow_mat));
    glUniform3fv(glGetUniformLocation(myShader->programID, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(myShader->programID, "lightColor"), 1, glm::value_ptr(lightColor));
    myModel->draw(myShader->programID);
    glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "mv_mat"), 1, GL_FALSE, glm::value_ptr(view_mat * model_mat2));
    otherModel->draw(myShader->programID);
    glBindTexture(GL_TEXTURE_2D, 0);
    myShader->disuse();

    // draw the light in the scene
    lightCubeShader->use();
    glm::mat4 light_mat = proj_mat * view_mat * light_model_mat;
    glUniformMatrix4fv(glGetUniformLocation(lightCubeShader->programID, "light_mat"), 1, GL_FALSE, glm::value_ptr(light_mat));
    glUniform3fv(glGetUniformLocation(lightCubeShader->programID, "lightColor"), 1, glm::value_ptr(lightColor));
    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    lightCubeShader->disuse();

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
    myCamera = new Camera(glm::vec3(0.0f, 10.0f, 20.0f));

    myShader = new Shader();
    myShader->add("./shaders/model.vert", GL_VERTEX_SHADER);
    myShader->add("./shaders/model.frag", GL_FRAGMENT_SHADER);
    myShader->compile(false);

    lightShader = new Shader();
    lightShader->add("./shaders/light.vert", GL_VERTEX_SHADER);
    lightShader->add("./shaders/light.frag", GL_FRAGMENT_SHADER);
    lightShader->compile(false);

    lightCubeShader = new Shader();
    lightCubeShader->add("./shaders/lightcube.vert", GL_VERTEX_SHADER);
    lightCubeShader->add("./shaders/lightcube.frag", GL_FRAGMENT_SHADER);
    lightCubeShader->compile(false);

    otherModel = new Model("./models/chocho_club");

    glGenVertexArrays(1, &lightVAO);

    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

    glGenTextures(1, &shadowTex);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, DEPTH_TEX_WIDTH, DEPTH_TEX_HEIGHT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTex, 0);

    glGenTextures(1, &shadowColorTex);
    glBindTexture(GL_TEXTURE_2D, shadowColorTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, DEPTH_TEX_WIDTH, DEPTH_TEX_HEIGHT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, shadowColorTex, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenVertexArrays(1, &sceneVAO);

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