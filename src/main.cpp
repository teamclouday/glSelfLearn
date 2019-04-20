#include "basic.hpp"

SDL_Window *myWindow = nullptr;
SDL_GLContext myContext = NULL;
Shader *myShader = nullptr;
GLuint VAO;
GLuint buffer;
GLuint buf;

void renderAll()
{
    glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearBufferfi(GL_DEPTH_STENCIL, 0.0f, 1.0f, 0.0f);

    // float currentTime = (float)SDL_GetTicks() / 1000.0f;

    myShader->use();

    glBindVertexArray(VAO);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

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

    static const GLfloat vertex_data[] = {
        -0.5f,  0.5f, 0.5f, 1.0f,
        0.5f, 0.5f, 0.3f, 1.0f,
         0.5f, -0.5f, 0.5f, 1.0f,
        0.9f, 0.3f, 0.2f, 1.0f,
         0.5f,  0.5f, 0.5f, 1.0f,
        0.3f, 1.0f, 0.4f, 1.0f,
    };

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vertex_data), vertex_data, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &buf);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buf);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, 16*sizeof(GLuint), NULL, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 3, buf);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buf);
    const GLuint zero = 0;
    // method 1
    glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 2*sizeof(GLuint), sizeof(GLuint), &zero);

    // method 2
    GLuint *data = (GLuint *)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, 16*sizeof(GLuint), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
    data[2] = 0;
    glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

    // method 3
    glClearBufferSubData(GL_ATOMIC_COUNTER_BUFFER, GL_R32UI, 2*sizeof(GLuint), sizeof(GLuint), GL_RED_INTEGER, GL_UNSIGNED_INT, &zero);

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