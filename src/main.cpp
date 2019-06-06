#include "basic.hpp"

SDL_Window *myWindow;;
SDL_GLContext myContext;
Shader *naiveShader, *exposureShader, *adaptiveShader;
GLuint VAO, VBO;
GLuint texture_hdr;
bool lineMode;
float exposure;

tone_state myState;

void renderAll()
{
    int w, h;
    SDL_GetWindowSize(myWindow, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.6f, 0.8f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    switch (myState)
    {
        case TONE_NAIVE:
            naiveShader->use();
            break;
        case TONE_EXPOSURE:
            exposureShader->use();
            glUniform1fv(glGetUniformLocation(exposureShader->programID, "exposure"), 1, &exposure);
            break;
        case TONE_ADAPTIVE:
            adaptiveShader->use();
            break;
    }
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    SDL_GL_SwapWindow(myWindow);
}

int main(int argc, char *argv[])
{
    initAll();

    naiveShader = new Shader();
    naiveShader->add("./shaders/hdr.vert", GL_VERTEX_SHADER);
    naiveShader->add("./shaders/hdr_naive.frag", GL_FRAGMENT_SHADER);
    naiveShader->compile(false);

    exposureShader = new Shader();
    exposureShader->add("./shaders/hdr.vert", GL_VERTEX_SHADER);
    exposureShader->add("./shaders/hdr_exposure.frag", GL_FRAGMENT_SHADER);
    exposureShader->compile(false);

    adaptiveShader = new Shader();
    adaptiveShader->add("./shaders/hdr.vert", GL_VERTEX_SHADER);
    adaptiveShader->add("./shaders/hdr_adaptive.frag", GL_FRAGMENT_SHADER);
    adaptiveShader->compile(false);

    static const GLfloat positions[] = {
        -0.9f, -0.9f,
         0.9f, -0.9f,
         0.9f,  0.9f,
        -0.9f, -0.9f,
         0.9f,  0.9f,
        -0.9f,  0.9f,
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), NULL);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    texture_hdr = loadTexture("./images/sebastian-pichler-20070-unsplash.jpg");
    glBindTextureUnit(0, texture_hdr);

    myState = TONE_NAIVE;
    exposure = 1.0f;

    lineMode = false;
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