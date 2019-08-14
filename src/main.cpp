#include "basic.hpp"

SDL_Window *myWindow;
SDL_GLContext myContext;
glText *myText;
Shader *myShader;
Model *myModel;
Camera *myCamera;
bool lineMode;

Shader *SSAO;
GLuint FBO, VAO, buffer;
GLuint fboTex[3];

struct SAMPLE_POINTS
{
    glm::vec4 points[256];
    glm::vec4 random_vectors[256];
};

SAMPLE_POINTS points_data;

static unsigned int seed = 0x13371337;
static inline float random_float()
{
    float res;
    unsigned tmp;
    seed *= 16807;
    tmp = seed ^ (seed >> 4) ^ (seed << 15);
    *((unsigned *)&res) = (tmp >> 9) | 0x3F800000;
    return (res - 1.0f);
}

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
    
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glEnable(GL_DEPTH_TEST);

    static const float black[] = {0.0f, 0.0f, 0.0f, 1.0f};
    static const float one = 1.0f;

    glClearBufferfv(GL_COLOR, 0, black);
    glClearBufferfv(GL_COLOR, 1, black);
    glClearBufferfv(GL_DEPTH, 0, &one);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, buffer);
    myShader->use();
    glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "proj_mat"), 1, GL_FALSE, glm::value_ptr(proj_mat));
    glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "mv_mat"), 1, GL_FALSE, glm::value_ptr(mv_mat));
    myModel->draw(myShader->programID);
    myShader->disuse();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    SSAO->use();
    glUniform1f(glGetUniformLocation(SSAO->programID, "ssao_radius"), 5.0f * (float)w/1000.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboTex[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fboTex[1]);
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    SSAO->disuse();

    char fpsNow[10];
    sprintf(fpsNow, "FPS: %.2f", fps);
    myText->render(std::string(fpsNow), 10.0f, (float)h-30.0f, 0.5f, glm::vec3(0.0f, 0.0f, 0.0f), true);

    SDL_GL_SwapWindow(myWindow);
}

int main(int argc, char *argv[])
{
    initAll();

    myText = new glText("./fonts/roboto/Roboto-Regular.ttf", 48);
    myModel = new Model("./models/crysis_nano_suit_2");
    myCamera = new Camera(glm::vec3(0.0f, 10.0f, 20.0f));

    myShader = new Shader();
    myShader->add("./shaders/render.vert", GL_VERTEX_SHADER);
    myShader->add("./shaders/render.frag", GL_FRAGMENT_SHADER);
    myShader->compile(false);

    SSAO = new Shader();
    SSAO->add("./shaders/ssao.vert", GL_VERTEX_SHADER);
    SSAO->add("./shaders/ssao.frag", GL_FRAGMENT_SHADER);
    SSAO->compile(false);

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glGenTextures(3, fboTex);

    glBindTexture(GL_TEXTURE_2D, fboTex[0]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, 2048, 2048);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, fboTex[1]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, 2048, 2048);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, fboTex[2]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 2048, 2048);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fboTex[0], 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, fboTex[1], 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  fboTex[2], 0);

    static const GLenum drawBuffs[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, drawBuffs);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenVertexArrays(1, &VAO);

    for(unsigned i = 0; i < 256; i++)
    {
        do
        {
            points_data.points[i] = glm::vec4(random_float() * 2.0f - 1.0f,
                                              random_float() * 2.0f - 1.0f,
                                              random_float(), 0.0f);
        }
        while(glm::length(points_data.points[i]) > 1.0f);
        points_data.points[i] = glm::normalize(points_data.points[i]);
    }
    for(unsigned i = 0; i < 256; i++)
    {
        points_data.random_vectors[i] = glm::vec4(random_float(), random_float(),
                                                  random_float(), random_float());
    }

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SAMPLE_POINTS), &points_data, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

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