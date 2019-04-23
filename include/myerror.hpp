#pragma once
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>

#define ERROR_SDL_INIT              1
#define ERROR_GLEW_INIT             2
#define ERROR_SHADER_FILE_NOT_FOUND 3
#define ERROR_SHADER_COMPILE        4
#define ERROR_SHADER_PROGRAM_LINK   5
#define ERROR_READ_IMAGE            6

void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam);