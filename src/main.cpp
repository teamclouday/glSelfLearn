#include "basic.hpp"

SDL_Window *myWindow;
SDL_GLContext myContext;
glText *myText;
Shader *myShader;
Shader *drawShader;

GLuint VAO;
GLuint input_tex, output_tex;
GLuint output_buff, output_buff_tex;

int display_mode;

#define TEXTURE_WIDTH   256
#define TEXTURE_HEIGHT  256

void renderAll()
{
    int w, h;
    SDL_GetWindowSize(myWindow, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, input_tex);
    glBindImageTexture(0, output_buff_tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32UI);

    myShader->use();

    // glUniform1ui(glGetUniformLocation(myShader->programID, "uImageWidth"), TEXTURE_WIDTH);

    glDispatchCompute(TEXTURE_WIDTH / 4, TEXTURE_WIDTH / 4, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // unsigned char *ptr = (unsigned char*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, TEXTURE_WIDTH * TEXTURE_HEIGHT / 2, GL_MAP_READ_BIT);
    // glUnmapBuffer(GL_TEXTURE_BUFFER);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, output_buff);
    glBindTexture(GL_TEXTURE_2D, output_tex);
    glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                              TEXTURE_WIDTH, TEXTURE_HEIGHT, GL_COMPRESSED_RED_RGTC1, TEXTURE_WIDTH*TEXTURE_HEIGHT/2, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    myShader->disuse();

    switch(display_mode)
    {
        case 1:
            glBindTexture(GL_TEXTURE_2D, input_tex);
            break;
        case 2:
            glBindTexture(GL_TEXTURE_2D, output_tex);
            break;
    }
    drawShader->use();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    drawShader->disuse();

    SDL_GL_SwapWindow(myWindow);
}

int main(int argc, char *argv[])
{
    initAll();

    myText = new glText("./fonts/roboto/Roboto-Regular.ttf", 48);

    myShader = new Shader();
    myShader->add("./shaders/rgtc.comp", GL_COMPUTE_SHADER);
    myShader->compile(false);

    drawShader = new Shader();
    drawShader->add("./shaders/draw.vert", GL_VERTEX_SHADER);
    drawShader->add("./shaders/draw.frag", GL_FRAGMENT_SHADER);
    drawShader->compile(false);

    input_tex = loadTexture("./images/colors.png");

    glGenTextures(1, &output_tex);
    glBindTexture(GL_TEXTURE_2D, output_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_COMPRESSED_RED_RGTC1, TEXTURE_WIDTH, TEXTURE_HEIGHT);

    glGenBuffers(1, &output_buff);
    glBindBuffer(GL_TEXTURE_BUFFER, output_buff);
    glBufferStorage(GL_TEXTURE_BUFFER, TEXTURE_WIDTH * TEXTURE_HEIGHT / 2, NULL, GL_MAP_READ_BIT);

    glGenTextures(1, &output_buff_tex);
    glBindTexture(GL_TEXTURE_BUFFER, output_buff_tex);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RG32UI, output_buff);

    glGenVertexArrays(1, &VAO);

    display_mode = 1;
    
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