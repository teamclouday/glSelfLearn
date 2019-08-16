#include "basic.hpp"

SDL_Window *myWindow;
SDL_GLContext myContext;
glText *myText;
Shader *myShader;
Model *myModel;
Camera *myCamera;
bool lineMode;

Shader *prepareShader;
Shader *renderShader;

#define MAX_FB_WIDTH            1920
#define MAX_FB_HEIGHT           1080

struct sphere
{
    glm::vec3   center;
    float       radius;
    glm::vec4   color;
};

struct plane
{
    glm::vec3   normal;
    float       d;
};

struct light
{
    glm::vec3   position;
    unsigned    : 32; // pad
};

struct uniform_block
{
    glm::mat4   mv_mat;
    glm::mat4   view_mat;
    glm::mat4   proj_mat;
};

GLuint texComposite;
GLuint rayFBO[MAX_RECURSION_DEPTH];
GLuint texPosition[MAX_RECURSION_DEPTH];
GLuint texReflected[MAX_RECURSION_DEPTH];
GLuint texReflectionIntensity[MAX_RECURSION_DEPTH];
GLuint texRefracted[MAX_RECURSION_DEPTH];
GLuint texRefractionIntensity[MAX_RECURSION_DEPTH];

GLuint VAO, uniformBuffer, sphereBuffer, planeBuffer, lightBuffer;

int maxDepth = 1;

void recurse(int depth)
{
    glBindFramebuffer(GL_FRAMEBUFFER, rayFBO[depth + 1]);
    static const GLenum drawBuffs[] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3,
        GL_COLOR_ATTACHMENT4,
        GL_COLOR_ATTACHMENT5,
    };
    glDrawBuffers(6, drawBuffs);

    glEnablei(GL_BLEND, 0);
    glBlendFunci(0, GL_ONE, GL_ONE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texPosition[depth]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texReflected[depth]);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texReflectionIntensity[depth]);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    if(depth != (maxDepth - 1))
        recurse(depth + 1);
    
    glDisablei(GL_BLEND, 0);
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
    float tt = (float)SDL_GetTicks() * 0.0001f;
    
    // glm::mat4 view_mat = myCamera->GetViewMatrix();
    glm::vec3 view_pos = glm::vec3(sinf(tt * 10 * 0.3234f) * 28.0f,
                                   cosf(tt * 10 * 0.4234f) * 28.0f,
                                   cosf(tt * 10 * 0.1234f) * 28.0f);
    glm::vec3 lookat_pos = glm::vec3(sinf(tt * 10 * 0.214f) * 8.0f,
                                     cosf(tt * 10 * 0.153f) * 8.0f,
                                     sinf(tt * 10 * 0.734f) * 8.0f);
    glm::mat4 view_mat = glm::lookAt(view_pos, lookat_pos, glm::vec3(0.0f, 1.0f, 0.0f));
    // glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), (float)w/(float)h, 0.1f, 1000.0f);
    // glm::mat4 model_mat = glm::scale(glm::mat4(1.0f), glm::vec3(8.0f));

    // glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniformBuffer);
    // uniform_block *block = (uniform_block*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniform_block), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    // block->mv_mat = view_mat * model_mat;
    // block->view_mat = view_mat;
    // block->proj_mat = proj_mat;
    // glUnmapBuffer(GL_UNIFORM_BUFFER);

    glBindBufferBase(GL_UNIFORM_BUFFER, 1, sphereBuffer);
    sphere *s = (sphere*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 128*sizeof(sphere), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    for(unsigned i = 0; i < 128; i++)
    {
        float fi = (float)i / 128.0f;
        s[i].center = glm::vec3(sinf(fi * 123.0f + tt) * 15.75f,
                                cosf(fi * 456.0f + tt) * 15.75f,
                                (sinf(fi * 300.0f + tt) * cosf(fi * 200.0f + tt)) * 20.0f);
        s[i].radius = fi * 2.3f + 3.5f;
        float r = fi * 61.0f;
        float g = r + 0.25f;
        float b = g + 0.25f;
        r = (r - floorf(r)) * 0.8f + 0.2f;
        g = (g - floorf(g)) * 0.8f + 0.2f;
        b = (b - floorf(b)) * 0.8f + 0.2f;
        s[i].color = glm::vec4(r, g, b, 1.0f);
    }
    glUnmapBuffer(GL_UNIFORM_BUFFER);

    glBindBufferBase(GL_UNIFORM_BUFFER, 2, planeBuffer);
    plane *p = (plane*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 6 * sizeof(plane), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    // 6 planes
    p[0].normal = glm::vec3(-1.0f, 0.0f, 0.0f);
    p[0].d = 30.0f;
    p[1].normal = glm::vec3(1.0f, 0.0f, 0.0f);
    p[1].d = 30.0f;
    p[2].normal = glm::vec3(0.0f, -1.0f, 0.0f);
    p[2].d = 30.0f;
    p[3].normal = glm::vec3(0.0f, 1.0f, 0.0f);
    p[3].d = 30.0f;
    p[4].normal = glm::vec3(0.0f, 0.0f, -1.0f);
    p[4].d = 30.0f;
    p[5].normal = glm::vec3(0.0f, 0.0f, 1.0f);
    p[5].d = 30.0f;
    glUnmapBuffer(GL_UNIFORM_BUFFER);

    glBindBufferBase(GL_UNIFORM_BUFFER, 3, lightBuffer);
    light *l = (light*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 128 * sizeof(light), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    for(unsigned i = 0; i < 128; i++)
    {
        float fi = 3.33f - (float)i;
        l[i].position = glm::vec3(sinf(fi * 2.0f - tt) * 15.75f,
                                  cosf(fi * 5.0f - tt) * 5.75f,
                                  (sinf(fi * 3.0f - tt) * cosf(fi * 2.5f - tt)) * 19.4f);
    }
    glUnmapBuffer(GL_UNIFORM_BUFFER);

    glBindVertexArray(VAO);

    prepareShader->use();
    glUniformMatrix4fv(glGetUniformLocation(prepareShader->programID, "ray_lookat"), 1, GL_FALSE, glm::value_ptr(view_mat));
    glUniform3fv(glGetUniformLocation(prepareShader->programID, "ray_origin"), 1, &myCamera->Position[0]);
    glUniform1f(glGetUniformLocation(prepareShader->programID, "aspect"), (float)h/(float)w);
    glBindFramebuffer(GL_FRAMEBUFFER, rayFBO[0]);
    static const GLenum drawBuffs[] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3,
        GL_COLOR_ATTACHMENT4,
        GL_COLOR_ATTACHMENT5,
    };
    glDrawBuffers(6, drawBuffs);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    prepareShader->disuse();

    myShader->use();
    recurse(0);
    myShader->disuse();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    renderShader->use();
    glDrawBuffer(GL_BACK);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texComposite);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    renderShader->disuse();

    glBindVertexArray(0);

    char fpsNow[50];
    sprintf(fpsNow, "FPS: %.2f Ray Recursion Depth: %d", fps, maxDepth);
    myText->render(std::string(fpsNow), 10.0f, (float)h-30.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f), true);

    SDL_GL_SwapWindow(myWindow);
}

int main(int argc, char *argv[])
{
    initAll();

    myText = new glText("./fonts/roboto/Roboto-Regular.ttf", 48);
    myModel = new Model("./models/crysis_nano_suit_2");
    myCamera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f));

    myShader = new Shader();
    myShader->add("./shaders/raytracer.vert", GL_VERTEX_SHADER);
    myShader->add("./shaders/raytracer.frag", GL_FRAGMENT_SHADER);
    myShader->compile(false);

    prepareShader = new Shader();
    prepareShader->add("./shaders/prepare.vert", GL_VERTEX_SHADER);
    prepareShader->add("./shaders/prepare.frag", GL_FRAGMENT_SHADER);
    prepareShader->compile(false);

    renderShader = new Shader();
    renderShader->add("./shaders/render.vert", GL_VERTEX_SHADER);
    renderShader->add("./shaders/render.frag", GL_FRAGMENT_SHADER);
    renderShader->compile(false);

    glGenBuffers(1, &uniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(uniform_block), NULL, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &sphereBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, sphereBuffer);
    glBufferData(GL_UNIFORM_BUFFER, 128 * sizeof(sphere), NULL, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &planeBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, planeBuffer);
    glBufferData(GL_UNIFORM_BUFFER, 6 * sizeof(plane), NULL, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &lightBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, lightBuffer);
    glBufferData(GL_UNIFORM_BUFFER, 128 * sizeof(light), NULL, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenVertexArrays(1, &VAO);

    glGenFramebuffers(MAX_RECURSION_DEPTH, rayFBO);
    glGenTextures(1, &texComposite);
    glGenTextures(MAX_RECURSION_DEPTH, texPosition);
    glGenTextures(MAX_RECURSION_DEPTH, texReflected);
    glGenTextures(MAX_RECURSION_DEPTH, texRefracted);
    glGenTextures(MAX_RECURSION_DEPTH, texReflectionIntensity);
    glGenTextures(MAX_RECURSION_DEPTH, texRefractionIntensity);

    glBindTexture(GL_TEXTURE_2D, texComposite);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, MAX_FB_WIDTH, MAX_FB_HEIGHT);

    for(unsigned i = 0; i < MAX_RECURSION_DEPTH; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, rayFBO[i]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texComposite, 0);

        glBindTexture(GL_TEXTURE_2D, texPosition[i]);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, MAX_FB_WIDTH, MAX_FB_HEIGHT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, texPosition[i], 0);

        glBindTexture(GL_TEXTURE_2D, texReflected[i]);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, MAX_FB_WIDTH, MAX_FB_HEIGHT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, texReflected[i], 0);

        glBindTexture(GL_TEXTURE_2D, texRefracted[i]);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, MAX_FB_WIDTH, MAX_FB_HEIGHT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, texRefracted[i], 0);

        glBindTexture(GL_TEXTURE_2D, texReflectionIntensity[i]);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, MAX_FB_WIDTH, MAX_FB_HEIGHT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, texReflectionIntensity[i], 0);

        glBindTexture(GL_TEXTURE_2D, texRefractionIntensity[i]);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, MAX_FB_WIDTH, MAX_FB_HEIGHT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, texRefractionIntensity[i], 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

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