#include "basic.hpp"

SDL_Window *myWindow = nullptr;
SDL_GLContext myContext = NULL;
Shader *myShader = nullptr;
Shader *fboShader = nullptr;
GLuint VAO, VBO, FBO;
GLuint tex_color, tex_depth;
bool lineMode;

void renderAll()
{
    int w, h;
    SDL_GetWindowSize(myWindow, &w, &h);    

    glm::mat4 proj_matrix = glm::perspective(45.0f, (float)w / (float)h, 0.1f, 1000.0f);
    float tt = (float)SDL_GetTicks() * 0.00002f;

    glm::mat4 mv_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f));
    mv_matrix = glm::translate(mv_matrix, glm::vec3(sinf(2.1f*tt)*0.5f,
                                                    cosf(1.7f*tt)*0.5f,
                                                    sinf(1.3f*tt)*cosf(1.5f*tt)*2.0f));
    mv_matrix = glm::rotate(mv_matrix, tt*45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    mv_matrix = glm::rotate(mv_matrix, tt*81.0f, glm::vec3(1.0f, 0.0f, 0.0f));

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, 512, 512);
    glClearColor(0.0f, 0.8f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    fboShader->use();
    glBindVertexArray(VAO);

    glUniformMatrix4fv(glGetUniformLocation(fboShader->programID, "mv_matrix"), 1, GL_FALSE, glm::value_ptr(mv_matrix));
    glUniformMatrix4fv(glGetUniformLocation(fboShader->programID, "proj_matrix"), 1, GL_FALSE, glm::value_ptr(proj_matrix));

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
    fboShader->disuse();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, w, h);
    glClearColor(0.6f, 0.4f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myShader->use();
    glBindVertexArray(VAO);

    glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "mv_matrix"), 1, GL_FALSE, glm::value_ptr(mv_matrix));
    glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "proj_matrix"), 1, GL_FALSE, glm::value_ptr(proj_matrix));

    glBindTexture(GL_TEXTURE_2D, tex_color);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
    myShader->disuse();

    SDL_GL_SwapWindow(myWindow);
}

int main(int argc, char *argv[])
{
    initAll();

    myShader = new Shader();
    myShader->add("./shaders/fbo.vert", GL_VERTEX_SHADER);
    myShader->add("./shaders/fbo2.frag", GL_FRAGMENT_SHADER);
    myShader->compile(false);

    fboShader = new Shader();
    fboShader->add("./shaders/fbo.vert", GL_VERTEX_SHADER);
    fboShader->add("./shaders/fbo1.frag", GL_FRAGMENT_SHADER);
    fboShader->compile(false);

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    // create texture for color buffer
    glGenTextures(1, &tex_color);
    glBindTexture(GL_TEXTURE_2D, tex_color);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512);
    // turn off mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // create texture for depth buffer
    glGenTextures(1, &tex_depth);
    glBindTexture(GL_TEXTURE_2D, tex_depth);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 512, 512);
    // attach color and depth textures to fbo
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex_color, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex_depth, 0);
    // draw into the fbo's first color attachment
    const static GLenum draw_buffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, draw_buffers);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    static const GLfloat vertex_data[] =
    {
        // Position                  Tex Coord
        -0.5f, -0.5f,  0.5f,      0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,      0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,      1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,      1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,      1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,      0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,      0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,      1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,      0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,      1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,      1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,      0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,      1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,      0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,      1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,      0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,      0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,      1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,      1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,      0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,      1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,      0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,      0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,      1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,      0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,      1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,      1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,      1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,      0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,      0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,      0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,      1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,      1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,      1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,      0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,      0.0f, 0.0f,
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)NULL);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

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