#pragma once
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL/SOIL.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>

#include "shader.hpp"
#include "myerror.hpp"
#include "text.hpp"
#include "model.hpp"
#include "camera.hpp"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE  "glSelfLearn"

#define SPF (1000 / 60)

#define MAX_RECURSION_DEPTH 6

#ifndef MY_ROOT_DIR
#define MY_ROOT_DIR "."
#endif

void initAll();
float timer(Uint32 *now, Uint32 *prev);
bool pollEvents();
void destroyAll();
void renderAll(float deltaT, float fps);