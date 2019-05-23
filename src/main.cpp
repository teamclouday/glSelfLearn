#include "basic.hpp"

#define POINTS_X 50
#define POINTS_Y 50
#define POINTS_ALL (POINTS_X * POINTS_Y)

#define NUM_ITER 20

SDL_Window *myWindow = nullptr;
SDL_GLContext myContext = NULL;
Shader *renderShader = nullptr;
Shader *updateShader = nullptr;
std::vector<GLuint> VAO(2);
std::vector<GLuint> VBO(5);
std::vector<GLuint> TBO(2);
std::vector<glm::vec4> initial_positions(POINTS_ALL);
std::vector<glm::vec3> initial_velocities(POINTS_ALL);
std::vector<glm::ivec4> connection_vectors(POINTS_ALL);

int iterIdx;

void SetUpPoints();

void renderAll()
{
    updateShader->use();
    glEnable(GL_RASTERIZER_DISCARD);

    for (int i = NUM_ITER; i != 0; i--)
    {
        glBindVertexArray(VAO[iterIdx & 1]);
        glBindTexture(GL_TEXTURE_BUFFER, TBO[iterIdx & 1]);
        iterIdx++;
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, VBO[iterIdx & 1]);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, VBO[2 + (iterIdx & 1)]);
        glBeginTransformFeedback(GL_POINTS);
        glDrawArrays(GL_POINTS, 0, POINTS_ALL);
        glEndTransformFeedback();
    }

    glDisable(GL_RASTERIZER_DISCARD);
    updateShader->disuse();

    int w, h;
    SDL_GetWindowSize(myWindow, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    renderShader->use();
    glPointSize(4.0f);
    glDrawArrays(GL_POINTS, 0, POINTS_ALL);
    renderShader->disuse();

    SDL_GL_SwapWindow(myWindow);
}

int main(int argc, char *argv[])
{
    initAll();

    updateShader = new Shader();
    updateShader->add("./shaders/special.vert", GL_VERTEX_SHADER);
    updateShader->compile(true);
    renderShader = new Shader();
    renderShader->add("./shaders/simple.vert", GL_VERTEX_SHADER);
    renderShader->add("./shaders/simple.frag", GL_FRAGMENT_SHADER);
    renderShader->compile(false);

    SetUpPoints();

    iterIdx = 0;

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

void SetUpPoints()
{
    int n = 0;
    for(int j = 0; j < POINTS_Y; j++)
    {
        float fj = (float)j / (float)POINTS_Y;
        for(int i = 0; i < POINTS_X; i++)
        {
            float fi = (float)i / (float)POINTS_X;
            initial_positions[n] = glm::vec4((fi-0.5f)*(float)POINTS_X,
                                             (fj-0.5f)*(float)POINTS_Y,
                                             0.6f*sinf(fi)*cosf(fj),
                                             1.0f);
            initial_velocities[n] = glm::vec3(0.0f);
            connection_vectors[n] = glm::ivec4(-1);
            if(j != (POINTS_Y - 1))
            {
                if(i != 0)
                    connection_vectors[n][0] = n - 1;
                if(j != 0)
                    connection_vectors[n][1] = n - POINTS_X;
                if(i != (POINTS_X - 1))
                    connection_vectors[n][2] = n + 1;
                if(j != (POINTS_Y - 1))
                    connection_vectors[n][3] = n + POINTS_X;
            }
            n++;
        }
    }

    glGenVertexArrays(2, &VAO[0]);
    glGenBuffers(5, &VBO[0]);
    for(int i = 0; i < 2; i++)
    {
        glBindVertexArray(VAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, POINTS_ALL*sizeof(glm::vec4), glm::value_ptr(initial_positions[0]), GL_DYNAMIC_COPY);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO[2+i]);
        glBufferData(GL_ARRAY_BUFFER, POINTS_ALL*sizeof(glm::vec3), glm::value_ptr(initial_velocities[0]), GL_DYNAMIC_COPY);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
        glBufferData(GL_ARRAY_BUFFER, POINTS_ALL*sizeof(glm::ivec4), glm::value_ptr(connection_vectors[0]), GL_STATIC_DRAW);
        glVertexAttribIPointer(2, 4, GL_INT, 0, NULL);
        glEnableVertexAttribArray(2);
    }

    glGenTextures(2, &TBO[0]);
    glBindTexture(GL_TEXTURE_BUFFER, TBO[0]);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, VBO[0]);
    glBindTexture(GL_TEXTURE_BUFFER, TBO[1]);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, VBO[1]);
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