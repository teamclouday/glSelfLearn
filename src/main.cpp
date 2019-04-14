#include "basic.hpp"

SDL_Window *myWindow = nullptr;
SDL_GLContext myContext = NULL;

int main(int argc, char *argv[])
{
    initAll();

    bool quit = false;
    while(!quit)
    {
        quit = pollEvents();
    }

    destroyAll();
    return 0;
}