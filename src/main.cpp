#include "basic.hpp"

SDL_Window *myWindow = nullptr;
SDL_GLContext myContext = NULL;
Shader *myShader = nullptr;
GLuint VAO;
GLuint buffer;

void renderAll()
{
    glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearBufferfi(GL_DEPTH_STENCIL, 0.0f, 1.0f, 0.0f);

    float currentTime = (float)SDL_GetTicks() / 1000.0f;
    float tc;

    int window_width, window_height;
    SDL_GetWindowSize(myWindow, &window_width, &window_height);
    glm::mat4 proj_matrix = glm::perspective(50.0f, (float)window_width/(float)window_height, 0.1f, 1000.0f);
    glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
    glm::mat4 mv_matrix;

    myShader->use();
    
    glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "proj_matrix"), 1, GL_FALSE, glm::value_ptr(proj_matrix));
    glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "scale_matrix"), 1, GL_FALSE, glm::value_ptr(scale_matrix));

    glBindVertexArray(VAO);

    for(int i = 0; i < 500; i++)
    {
        tc = (float)i + currentTime * 0.3f;
        mv_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -20.0f));
        mv_matrix = glm::translate(mv_matrix, glm::vec3(sinf(2.1f*tc)*2.0f,
                                                        cosf(1.7f*tc)*2.0f,
                                                        sinf(1.3f*tc)*cosf(1.5f*tc)*2.0f));
        mv_matrix = glm::rotate(mv_matrix, currentTime*45.0f*0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
        mv_matrix = glm::rotate(mv_matrix, currentTime*21.0f*0.1f, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(myShader->programID, "mv_matrix"), 1, GL_FALSE, glm::value_ptr(mv_matrix));

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(0);

    SDL_GL_SwapWindow(myWindow);
}

int main(int argc, char *argv[])
{
    initAll();

    myShader = new Shader();
    myShader->add("./shaders/simple.vert", GL_VERTEX_SHADER);
    myShader->add("./shaders/simple.frag", GL_FRAGMENT_SHADER);
    myShader->compile();

    glCreateVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    static const GLfloat vertex_positions[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,
    };

    glCreateBuffers(1, &buffer);
    glNamedBufferStorage(buffer, sizeof(vertex_positions), vertex_positions, 0);
    glVertexArrayVertexBuffer(VAO, 0, buffer, 0, sizeof(GLfloat)*3);
    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(VAO, 0, 0);
    glEnableVertexArrayAttrib(VAO, 0);

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