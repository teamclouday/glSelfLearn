#include "basic.hpp"

SDL_Window *myWindow = nullptr;
SDL_GLContext myContext = NULL;
Shader *myShader = nullptr;
Shader *counterShader = nullptr;
GLuint VAO;
GLuint buf;
GLuint buffer;

void renderAll()
{
    glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearBufferfi(GL_DEPTH_STENCIL, 0.0f, 1.0f, 0.0f);

    glBindVertexArray(VAO);

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    counterShader->use();
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buffer);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
    counterShader->disuse();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);

    myShader->use();
    glUniform1f(glGetUniformLocation(myShader->programID, "max_area"), 100000.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    myShader->disuse();
    
    glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);

    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buffer);
    GLuint *data = (GLuint *)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT );
    *data = 0;
    glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

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

    counterShader = new Shader();
    counterShader->add("./shaders/simple.vert", GL_VERTEX_SHADER);
    counterShader->add("./shaders/counter.frag", GL_FRAGMENT_SHADER);
    counterShader->compile();

    static const GLfloat vertex_data[] = {
        -0.5f,  0.5f, 0.5f, 1.0f,
         0.5f, -0.5f, 0.5f, 1.0f,
         0.5f,  0.5f, 0.5f, 1.0f,
    };

    glCreateVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    glGenBuffers(1, &buffer);
    // bind to atomic buffer
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buffer);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, buffer);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buffer);
    GLuint zero = 0;
    glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

    // bind to uniform buffer
    glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

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