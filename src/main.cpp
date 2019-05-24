#include "basic.hpp"

SDL_Window *myWindow = nullptr;
SDL_GLContext myContext = NULL;
Shader *myShader = nullptr;
GLuint VAO, tex, tex_disp;
float dmap_depth;
bool lineMode;

void renderAll()
{
    int w, h;
    SDL_GetWindowSize(myWindow, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float tt = (float)SDL_GetTicks() * 0.0001f;
    float rr = sinf(tt * 5.37f) * 15.0f + 16.0f;
    float hh = cosf(tt * 4.79f) * 2.0f + 3.2f;
    glm::vec3 pos = glm::vec3(sinf(tt)*rr, hh, cosf(tt)*rr);

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -6.0f, 0.0f));
    glm::mat4 proj_matrix = glm::perspective(45.0f, (float)w / (float)h, 0.1f, 1000.0f);
    glm::mat4 mv_matrix = glm::lookAt(pos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 mvp = proj_matrix * mv_matrix * model;


    myShader->use();
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE1);

    glUniform1fv(glGetUniformLocation(myShader->programID, "dmap_depth"), 1, &dmap_depth);
    glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
    
    glPatchParameteri(GL_PATCH_VERTICES, 4);

    if(lineMode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArraysInstanced(GL_PATCHES, 0, 4, 64*64);

    glBindVertexArray(0);
    myShader->disuse();

    SDL_GL_SwapWindow(myWindow);
}

int main(int argc, char *argv[])
{
    initAll();

    myShader = new Shader();
    myShader->add("./shaders/terrain.vert", GL_VERTEX_SHADER);
    myShader->add("./shaders/terrain.tesc", GL_TESS_CONTROL_SHADER);
    myShader->add("./shaders/terrain.tese", GL_TESS_EVALUATION_SHADER);
    myShader->add("./shaders/terrain.frag", GL_FRAGMENT_SHADER);
    myShader->compile(false);

    glGenVertexArrays(1, &VAO);

    tex = loadTexture("./images/grass.jpg");
    glBindTextureUnit(1, tex);
    tex_disp = loadTexture("./images/terrain.jpg");
    glBindTextureUnit(0, tex_disp);

    dmap_depth = 6.0f;
    lineMode = false;

    printf("%s\n", glGetString(GL_RENDERER));
    printf("Press S to switch between line mode and fill mode\n");

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


GLuint loadTextureArray(std::vector<std::string> path)
{
    std::vector<unsigned char *> data;
    int width, height;
    for(unsigned i = 0; i < path.size(); i++)
    {
        unsigned char *image = SOIL_load_image(path[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
        data.push_back(image);
    }
    GLuint texture;
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texture);
    glTextureStorage3D(texture, 8, GL_RGB8, 256, 256, data.size());
    for(unsigned i = 0; i < data.size(); i++)
    {
        glTextureSubImage3D(texture, 0, 0, 0, i, 256, 256, 1, GL_RGB, GL_UNSIGNED_BYTE, data[i]);
    }
    for(unsigned i = 0; i < data.size(); i++)
        SOIL_free_image_data(data[i]);
    return texture;
}

GLuint loadTexture(std::string path)
{
    GLuint texture;
    int width, height;
    unsigned char *data = SOIL_load_image(path.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
    if(!data)
    {
        printf("Failed to load image: %s\n", path.c_str());
        exit(ERROR_READ_IMAGE);
    }
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA8, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateTextureMipmap(texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(data);
    return texture;
}