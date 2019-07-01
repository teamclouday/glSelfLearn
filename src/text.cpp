#include "text.hpp"

extern SDL_Window *myWindow;

glText::glText(std::string fontpath, int size)
{
    this->fontpath = fontpath;
    this->global_init(size);
    this->load_text128();
    this->load_shaders();
    this->update();

    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*6*4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

glText::~glText()
{
    glDeleteBuffers(1, &this->VBO);
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteProgram(this->programID);
    std::map<GLchar, Chr>::iterator iter;
    for(iter = this->characters.begin(); iter != this->characters.end(); iter++)
    {
        glDeleteTextures(1, &(iter->second.textureID));
    }
}

void glText::global_init(int size)
{
    if(FT_Init_FreeType(&this->freetype))
    {
        printf("Error: could not init freetype!\n");
        exit(ERROR_INIT_FREETYPE);
    }
    if(FT_New_Face(this->freetype, this->fontpath.c_str(), 0, &this->face))
    {
        printf("Error: could not load font %s\n", this->fontpath.c_str());
        exit(ERROR_LOAD_FONT);
    }
    FT_Set_Pixel_Sizes(this->face, 0, size);
}

void glText::load_text128()
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for(GLubyte c = 0; c < 128; c++)
    {
        if(FT_Load_Char(this->face, (FT_ULong)c, FT_LOAD_RENDER))
        {
            printf("Error loading character: %c\n", (char)c);
            continue;
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                     this->face->glyph->bitmap.width,
                     this->face->glyph->bitmap.rows,
                     0, GL_RED, GL_UNSIGNED_BYTE,
                     this->face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Chr character = {
            texture,
            glm::ivec2(this->face->glyph->bitmap.width, this->face->glyph->bitmap.rows),
            glm::ivec2(this->face->glyph->bitmap_left, this->face->glyph->bitmap_top),
            (GLuint)this->face->glyph->advance.x
        };
        this->characters.insert(std::pair<GLchar, Chr>((GLchar)c, character));
    }

    this->global_done();
}

void glText::global_done()
{
    FT_Done_Face(this->face);
    FT_Done_FreeType(this->freetype);
}

void glText::load_shaders()
{
    static const char *vert_shader[] = 
    {
        "#version 330 core                                                      \n"
        "layout (location = 0) in vec4 vertex;                                  \n"
        "out vec2 TexCoords;                                                    \n"
        "uniform mat4 projection;                                               \n"
        "void main()                                                            \n"
        "{                                                                      \n"
        "   gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);               \n"
        "   TexCoords = vertex.zw;                                              \n"
        "}                                                                      \n"
    };

    static const char *frag_shader[] =
    {
        "#version 330 core                                                      \n"
        "in vec2 TexCoords;                                                     \n"
        "out vec4 color;                                                        \n"
        "uniform sampler2D text;                                                \n"
        "uniform vec3 textColor;                                                \n"
        "void main()                                                            \n"
        "{                                                                      \n"   
        "   vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);     \n"
        "   color = vec4(textColor, 1.0) * sampled;                             \n"
        "}                                                                      \n"
    };

    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertShader, 1, vert_shader, NULL);
    glShaderSource(fragShader, 1, frag_shader, NULL);

    glCompileShader(vertShader);
    glCompileShader(fragShader);

    this->programID = glCreateProgram();
    glAttachShader(this->programID, vertShader);
    glAttachShader(this->programID, fragShader);
    glLinkProgram(this->programID);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

void glText::update()
{
    int w, h;
    SDL_GetWindowSize(myWindow, &w, &h);
    this->projection = glm::ortho(0.0f, (float)w, 0.0f, (float)h);
}

void glText::render(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, bool blend)
{
    if(blend)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    }
    this->update();
    glUseProgram(this->programID);
    glUniform3f(glGetUniformLocation(this->programID, "textColor"), color.x, color.y, color.z);
    glUniformMatrix4fv(glGetUniformLocation(this->programID, "projection"), 1, GL_FALSE, glm::value_ptr(this->projection));
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);

    std::string::const_iterator c;
    for(c = text.begin(); c != text.end(); c++)
    {
        Chr ch = this->characters[*c];

        GLfloat xpos = x + ch.bearing.x * scale;
        GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

        GLfloat w = ch.size.x * scale;
        GLfloat h = ch.size.y * scale;
        
        GLfloat vertices[6][4] = {
            {xpos,     ypos + h,   0.0, 0.0},
            {xpos,     ypos,       0.0, 1.0},
            {xpos + w, ypos,       1.0, 1.0},

            {xpos,     ypos + h,   0.0, 0.0},
            {xpos + w, ypos,       1.0, 1.0},
            {xpos + w, ypos + h,   1.0, 0.0}
        };
        
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        x += (ch.advance >> 6) * scale; 
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    if(blend)
    {
        glDisable(GL_BLEND);
    }
}