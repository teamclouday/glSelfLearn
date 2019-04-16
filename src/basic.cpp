#include "basic.hpp"

extern SDL_Window *myWindow;
extern SDL_GLContext myContext;
extern Shader *myShader;

void initAll()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Failed to init SDL2\nSDL Error: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    myWindow = SDL_CreateWindow(WINDOW_TITLE,
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                WINDOW_WIDTH,
                                WINDOW_HEIGHT,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
    myContext = SDL_GL_CreateContext(myWindow);
    SDL_GL_SetSwapInterval(1);
    if(glewInit() != GLEW_OK)
    {
        printf("Failed to init GLEW\n");
        exit(2);
    }
    glewExperimental = GL_TRUE;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void timer(Uint32 *now, Uint32 *prev)
{
    *now = SDL_GetTicks();
    Uint32 delta = *now - *prev;
    if(delta < SPF)
        SDL_Delay((Uint32)SPF - delta);
    *prev = SDL_GetTicks();
}

bool pollEvents()
{
    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        if(e.type == SDL_QUIT)
        {
            return true;
        }
        else if(e.type == SDL_KEYDOWN)
        {
            switch(e.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                return true;
            }
        }
    }
    return false;
}

void destroyAll()
{
    if(!myShader)
        delete myShader;
    SDL_GL_DeleteContext(myContext);
    SDL_DestroyWindow(myWindow);
    SDL_Quit();
}