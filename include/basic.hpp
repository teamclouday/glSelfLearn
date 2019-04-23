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

#include "shader.hpp"
#include "myerror.hpp"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE  "glSelfLearn"

#define SPF (1000 / 60)

void initAll();
void timer(Uint32 *now, Uint32 *prev);
bool pollEvents();
void destroyAll();
void renderAll();
GLuint loadTexture(std::string path);