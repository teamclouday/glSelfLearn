#include "basic.hpp"

SDL_Window *myWindow = nullptr;
SDL_GLContext myContext = NULL;
Shader *myShader = nullptr;
GLuint VAO;
GLuint buf;
GLuint buffer;

void renderAll()
{
    glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearBufferfi(GL_DEPTH_STENCIL, 0.0f, 1.0f, 0.0f);

    glBindVertexArray(VAO);
    myShader->use();
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
    myShader->disuse();
    glBindVertexArray(0);

    glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);

    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buffer);
    GLuint *data = (GLuint *)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), GL_MAP_WRITE_BIT);
    *data = 0;
    glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
    
    SDL_GL_SwapWindow(myWindow);
}

int main(int argc, char *argv[])
{
    initAll();

    myShader = new Shader();
    myShader->add("./shaders/simple.vert", GL_VERTEX_SHADER);
    myShader->add("./shaders/simple.frag", GL_FRAGMENT_SHADER);
    myShader->compile();

    static const GLfloat vertex_data[] = {
        -0.5f,  0.5f, 0.5f, 1.0f,
         0.5f, -0.5f, 0.5f, 1.0f,
         0.5f,  0.5f, 0.5f, 1.0f,

        -0.5f,  0.5f, 0.5f, 1.0f,
        -0.5f, -0.5f, 0.5f, 1.0f,
         0.5f, -0.5f, 0.5f, 1.0f,
    };

    glCreateVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    GLuint zero = 0;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buffer);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &zero, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, buffer);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

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