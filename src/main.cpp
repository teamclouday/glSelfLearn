#include "basic.hpp"

SDL_Window *myWindow;;
SDL_GLContext myContext;
Shader *myShader;
GLuint VAO, VBO;
GLuint starTex;
int num_stars = 5000;

static unsigned seed  = 0x12345678;

static inline float random_float()
{
    float result = 0.0f;
    unsigned tmp;
    seed *= 16807;
    tmp = seed ^ (seed >> 4) ^ (seed << 15);
    *((unsigned *)&result) = (tmp >> 9) | 0x3F800000;
    return (result - 1.0f);
}

void renderAll()
{
    int w, h;
    SDL_GetWindowSize(myWindow, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float tc = (float)SDL_GetTicks() * 0.0001f;
    glm::mat4 proj_matrix = glm::perspective(45.0f, (float)w/(float)h, 0.1f, 1000.0f);
    tc -= std::floor(tc);

    myShader->use();

    glUniform1fv(glGetUniformLocation(myShader->programID, "time"), 1, &tc);
    glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "proj_mat"), 1, GL_FALSE, glm::value_ptr(proj_matrix));

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindVertexArray(VAO);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glDrawArrays(GL_POINTS, 0, num_stars);
    glBindVertexArray(0);
    glDisable(GL_BLEND);

    myShader->disuse();

    SDL_GL_SwapWindow(myWindow);
}

int main(int argc, char *argv[])
{
    initAll();

    myShader = new Shader();
    myShader->add("./shaders/star.vert", GL_VERTEX_SHADER);
    myShader->add("./shaders/star.frag", GL_FRAGMENT_SHADER);
    myShader->compile(false);

    starTex = loadTexture("./images/star.jpg");
    glBindTextureUnit(0, starTex);

    struct star_t
    {
        GLfloat position[3];
        GLfloat color[3];
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, num_stars*sizeof(star_t), NULL, GL_STATIC_DRAW);

    star_t *star = (star_t*)glMapBufferRange(GL_ARRAY_BUFFER, 0, num_stars*sizeof(star_t), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    for(int i = 0; i < num_stars; i++)
    {
        star[i].position[0] = (random_float() * 2.0f - 1.0f) * 100.0f;
        star[i].position[1] = (random_float() * 2.0f - 1.0f) * 100.0f;
        star[i].position[2] = random_float();
        star[i].color[0] = 0.8f + random_float() * 0.2f;
        star[i].color[1] = 0.8f + random_float() * 0.2f;
        star[i].color[2] = 0.8f + random_float() * 0.2f;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(star_t), (GLvoid*)NULL);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(star_t), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    
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