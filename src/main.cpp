#include "basic.hpp"

SDL_Window *myWindow = nullptr;
SDL_GLContext myContext = NULL;
Shader *myShader = nullptr;
GLuint VAO;
GLuint VBO;


void renderAll()
{
    int w, h;
    SDL_GetWindowSize(myWindow, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myShader->use();
    glBindVertexArray(VAO);
    // glPatchParameteri(GL_PATCH_VERTICES, 4);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_PATCHES, 0, 3);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(0);
    myShader->disuse();

    SDL_GL_SwapWindow(myWindow);
}

int main(int argc, char *argv[])
{
    initAll();

    myShader = new Shader();
    myShader->add("./shaders/simple.vert", GL_VERTEX_SHADER);
    myShader->add("./shaders/simple.tesc", GL_TESS_CONTROL_SHADER);
    myShader->add("./shaders/simple.tese", GL_TESS_EVALUATION_SHADER);
    myShader->add("./shaders/simple.frag", GL_FRAGMENT_SHADER);

    myShader->compile(false);

    printf("%s\n", glGetString(GL_RENDERER));

    static const GLfloat pos[] = {
        // -0.5f, -0.5f,
        //  0.5f, -0.5f,
        // -0.5f,  0.5f,
        //  0.5f,  0.5f,

        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

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