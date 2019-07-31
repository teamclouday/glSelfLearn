#include "basic.hpp"

extern SDL_Window *myWindow;
extern SDL_GLContext myContext;
extern Shader *myShader;
extern glText *myText;
extern Model *myModel;
extern Camera *myCamera;

extern int display_mode;
extern std::vector<Shader*> myShaders;

void initAll()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Failed to init SDL2\nSDL Error: %s\n", SDL_GetError());
        exit(ERROR_SDL_INIT);
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
                                SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
    myContext = SDL_GL_CreateContext(myWindow);
    SDL_GL_SetSwapInterval(1);
    if(glewInit() != GLEW_OK)
    {
        printf("Failed to init GLEW\n");
        exit(ERROR_GLEW_INIT);
    }
    glewExperimental = GL_TRUE;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    // glEnable(GL_DEBUG_OUTPUT);
    // glDebugMessageCallback(MessageCallback, 0);
}

float timer(Uint32 *now, Uint32 *prev)
{
    *now = SDL_GetTicks();
    Uint32 delta = *now - *prev;
    if(delta < SPF)
        SDL_Delay((Uint32)SPF - delta);
    *prev = SDL_GetTicks();
    return (float)delta;
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
                {
                    if(!myCamera->focus)
                        return true;
                    else
                    {
                        myCamera->focus = false;
                        SDL_ShowCursor(SDL_TRUE);
                        SDL_CaptureMouse(SDL_FALSE);
                    }
                    break;
                }
                case SDLK_F11:
                {
                    bool isFullScreen = SDL_GetWindowFlags(myWindow) & SDL_WINDOW_FULLSCREEN_DESKTOP;
                    SDL_SetWindowPosition(myWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                    SDL_SetWindowFullscreen(myWindow, isFullScreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
                    break;
                }
                case SDLK_r:
                {
                    if(myCamera->focus)
                        myCamera->reset();
                    break;
                }
                case SDLK_1:
                    display_mode = 1;
                    break;
                case SDLK_2:
                    display_mode = 2;
                    break;
                case SDLK_3:
                    display_mode = 3;
                    break;
                case SDLK_4:
                    display_mode = 4;
                    break;
                case SDLK_5:
                    display_mode = 5;
                    break;
                case SDLK_6:
                    display_mode = 6;
                    break;
                case SDLK_7:
                    display_mode = 7;
                    break;
                case SDLK_8:
                    display_mode = 8;
                    break;
                case SDLK_w:
                    if(myCamera->focus)
                        myCamera->keyMap[0] = true;
                    break;
                case SDLK_a:
                    if(myCamera->focus)
                        myCamera->keyMap[1] = true;
                    break;
                case SDLK_s:
                    if(myCamera->focus)
                        myCamera->keyMap[2] = true;
                    break;
                case SDLK_d:
                    if(myCamera->focus)
                        myCamera->keyMap[3] = true;
                    break;
                case SDLK_UP:
                    if(myCamera->focus)
                        myCamera->keyMap[4] = true;
                    break;
                case SDLK_DOWN:
                    if(myCamera->focus)
                        myCamera->keyMap[5] = true;
                    break;
            }
        }
        else if(e.type == SDL_KEYUP)
        {
            switch(e.key.keysym.sym)
            {
                case SDLK_w:
                    myCamera->keyMap[0] = false;
                    break;
                case SDLK_a:
                    myCamera->keyMap[1] = false;
                    break;
                case SDLK_s:
                    myCamera->keyMap[2] = false;
                    break;
                case SDLK_d:
                    myCamera->keyMap[3] = false;
                    break;
                case SDLK_UP:
                    myCamera->keyMap[4] = false;
                    break;
                case SDLK_DOWN:
                    myCamera->keyMap[5] = false;
                    break;
            }
        }
        else if(e.type == SDL_WINDOWEVENT)
        {
            switch(e.window.event)
            {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    glViewport(0, 0, e.window.data1, e.window.data2);
            }
        }
        else if(e.type == SDL_MOUSEBUTTONDOWN)
        {
            if(!myCamera->focus)
            {
                SDL_ShowCursor(SDL_FALSE);
                myCamera->focus = true;
                int w, h;
                SDL_GetWindowSize(myWindow, &w, &h);
                myCamera->mousePos = {(int)(w/2), (int)(h/2)};
                SDL_WarpMouseInWindow(myWindow, myCamera->mousePos[0], myCamera->mousePos[1]);
                SDL_CaptureMouse(SDL_TRUE);
            }
        }
    }
    return false;
}

void destroyAll()
{
    if(myShader != nullptr)
        delete myShader;
    if(myText != nullptr)
        delete myText;
    if(myModel != nullptr)
        delete myModel;
    if(myCamera != nullptr)
        delete myCamera;
    for(unsigned i = 0; i < myShaders.size(); i++)
    {
        if(myShaders[i] != nullptr)
            delete myShaders[i];
    }
    SDL_GL_DeleteContext(myContext);
    SDL_DestroyWindow(myWindow);
    SDL_Quit();
}