#include "basic.hpp"

SDL_Window *myWindow;;
SDL_GLContext myContext;
Shader *myShader;
GLuint VAO, VBO;

bool shot;

void screenShot()
{
    int w, h;
    SDL_GetWindowSize(myWindow, &w, &h);
    int row_size = ((w*3+3)&~3);
    int data_size = row_size * h;
    unsigned char *data = new unsigned char[data_size];

    #pragma pack (push, 1)
    struct
    {
        unsigned char identsize;    // size of ID field
        unsigned char cmaptype;     // color map type 0 = none
        unsigned char imagetype;    // image type 2 = rgb
        short cmapstart;
        short cmapsize;
        unsigned char cmapbpp;
        short xorigin;
        short yorigin;
        short width;
        short height;
        unsigned char bpp;
        unsigned char descriptor;
    } tga_header;
    #pragma pack (pop)

    glReadPixels(0, 0, w, h, GL_BGR, GL_UNSIGNED_BYTE, data);
    memset(&tga_header, 0, sizeof(tga_header));
    tga_header.imagetype = 2;
    tga_header.width = (short)w;
    tga_header.height = (short)h;
    tga_header.bpp = 24;
    FILE *f_out = fopen("screenshot.tga", "wb");
    fwrite(&tga_header, sizeof(tga_header), 1, f_out);
    fwrite(data, data_size, 1, f_out);
    fclose(f_out);

    delete[] data;
}

void renderAll()
{
    int w, h;
    SDL_GetWindowSize(myWindow, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myShader->use();
    glBindVertexArray(VAO);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glDrawArrays(GL_POINTS, 0, 4);

    glBindVertexArray(0);
    myShader->disuse();

    if(shot)
    {
        screenShot();
        shot = false;
    }

    SDL_GL_SwapWindow(myWindow);
}

int main(int argc, char *argv[])
{
    initAll();

    myShader = new Shader();
    myShader->add("./shaders/point.vert", GL_VERTEX_SHADER);
    myShader->add("./shaders/point.frag", GL_FRAGMENT_SHADER);
    myShader->compile(false);

    const static GLfloat positions[] =
    {
         0.5f,  0.5f,
         0.5f, -0.5f,
        -0.5f, -0.5f,
        -0.5f,  0.5f,
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid*)NULL);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    shot = false;

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