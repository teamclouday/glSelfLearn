#include "basic.hpp"

SDL_Window *myWindow;
SDL_GLContext myContext;
glText *myText;
Shader *myShader;
Shader *renderShader;
// GLuint VAO, VBO;
GLuint flock_buffer[2];
GLuint flock_vao[2];
GLuint geom_buffer;

#define WORKGROUP_SIZE  256
#define NUM_WORKGROUPS  16
#define FLOCK_SIZE      (WORKGROUP_SIZE * NUM_WORKGROUPS)

int frameID = 0;

struct flock_mem
{
    glm::vec3 position;
    unsigned aaa = 32;
    glm::vec3 velocity;
    unsigned bbb = 32;
};

glm::vec3 random_vec3()
{
    // between 0 and 1
    float x = (float)(rand()%100001)/100001.0f;
    float y = (float)(rand()%100001)/100001.0f;
    float z = (float)(rand()%100001)/100001.0f;
    return glm::vec3(x, y, z);
}

void renderAll()
{
    int w, h;
    SDL_GetWindowSize(myWindow, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float tt = (float)(SDL_GetTicks()*0.00001f);

    myShader->use();
    // glm::vec3 goal = glm::vec3(sinf(tt*0.34f),
    //                            cosf(tt*0.29f),
    //                            sinf(tt*0.12f)*cosf(tt*0.5f));
    // goal = goal * glm::vec3(35.0f, 25.0f, 60.0f);

    int x, y;
    SDL_GetMouseState(&x, &y);
    glm::vec3 goal = glm::vec3(2*((float)(w-x)/(float)w)-1.0f, 2*((float)(h-y)/(float)h)-1.0f, sinf(tt*0.12f)*cosf(tt*0.5f));
    // printf("%.2f %.2f\n", (2*((float)x/(float)w)-1.0f), (2*((float)(h-y)/(float)h)-1.0f));
    goal = goal * glm::vec3(100.0f, 100.0f, 60.0f);

    glUniform3fv(glGetUniformLocation(myShader->programID, "goal"), 1, glm::value_ptr(goal));

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, flock_buffer[frameID]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, flock_buffer[frameID^1]);

    glDispatchCompute(NUM_WORKGROUPS, 1, 1);

    myShader->disuse();

    renderShader->use();

    glm::mat4 mv_matrix = glm::lookAt(glm::vec3(0.0f, 0.0f, -400.0f),
                                      glm::vec3(0.0f, 0.0f, 0.0f),
                                      glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 proj_matrix = glm::perspective(45.0f, (float)w/(float)h, 0.1f, 3000.0f);
    glm::mat4 mvp = proj_matrix * mv_matrix;

    glUniformMatrix4fv(glGetUniformLocation(renderShader->programID, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));

    glBindVertexArray(flock_vao[frameID]);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 8, FLOCK_SIZE);
    glBindVertexArray(0);

    renderShader->disuse();

    frameID ^= 1;

    SDL_GL_SwapWindow(myWindow);
}

int main(int argc, char *argv[])
{
    initAll();

    myText = new glText("./fonts/roboto/Roboto-Regular.ttf", 48);

    myShader = new Shader();
    myShader->add("./shaders/flock.comp", GL_COMPUTE_SHADER);
    myShader->compile(false);

    renderShader = new Shader();
    renderShader->add("./shaders/flock.vert", GL_VERTEX_SHADER);
    renderShader->add("./shaders/flock.frag", GL_FRAGMENT_SHADER);
    renderShader->compile(false);

    static const GLfloat geometry[] = {
        // positions
        -5.0f, 1.0f, 0.0f,
        -1.0f, 1.5f, 0.0f,
        -1.0f, 1.5f, 7.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 10.0f,
         1.0f, 1.5f, 0.0f,
         1.0f, 1.5f, 7.0f,
         5.0f, 1.0f, 0.0f,
        // normals
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.107f, -0.859f, 0.0f,
        0.832f, 0.554f, 0.0f,
        -0.59f, -0.395f, 0.0f,
        -0.832f, 0.554f, 0.0f,
        0.295f, -0.196f, 0.0f,
        0.124f, 0.992f, 0.0f,
    };

    glGenBuffers(2, flock_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, flock_buffer[0]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, FLOCK_SIZE*sizeof(flock_mem), NULL, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, flock_buffer[1]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, FLOCK_SIZE*sizeof(flock_mem), NULL, GL_DYNAMIC_COPY);

    glGenBuffers(1, &geom_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, geom_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(geometry), geometry, GL_STATIC_DRAW);
    
    glGenVertexArrays(2, flock_vao);

    for(int i = 0; i < 2; i++)
    {
        glBindVertexArray(flock_vao[i]);
        glBindBuffer(GL_ARRAY_BUFFER, geom_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(8*sizeof(glm::vec3)));
        glBindBuffer(GL_ARRAY_BUFFER, flock_buffer[i]);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(flock_mem), NULL);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(flock_mem), (GLvoid*)(sizeof(glm::vec4)));

        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
    }

    glBindBuffer(GL_ARRAY_BUFFER, flock_buffer[0]);
    flock_mem *ptr  = reinterpret_cast<flock_mem*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, 
            FLOCK_SIZE*sizeof(flock_mem), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
    for(int i = 0; i < FLOCK_SIZE; i++)
    {
        ptr[i].position = (random_vec3() - glm::vec3(0.5f)) * 300.0f;
        ptr[i].velocity = (random_vec3() - glm::vec3(0.5f));
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

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