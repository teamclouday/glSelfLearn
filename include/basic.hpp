#pragma once
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <stdlib.h>
#include <stdio.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE  "glSelfLearn"

void initAll();
bool pollEvents();
void destroyAll();