// code inspired from
// https://learnopengl.com/In-Practice/Text-Rendering

#pragma once

#include <GL/glew.h>

#include <freetype/config/ftheader.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include  <SDL2/SDL.h>

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <map>

#include "myerror.hpp"

struct Chr
{
    GLuint textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    GLuint advance;
};

// a function wrapper class
class glText
{
public:
    glText(std::string fontpath, int size);
    ~glText();
    void render(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, bool blend);

private:
    void global_init(int size);
    void load_text128(); // load 128 chars into texture
    void global_done();
    void load_shaders();
    void update();

public:
    FT_Library freetype;
    FT_Face face;
    std::map<GLchar, Chr> characters;

private:
    std::string fontpath;
    GLuint VAO, VBO;
    GLuint programID;
    glm::mat4 projection;
};