#include "shader.hpp"

Shader::Shader()
{
    
}

Shader::~Shader()
{
    glDeleteProgram(this->programID);
}

void Shader::add(std::string path, GLenum shaderType)
{
    GLuint shader;

    std::ifstream ifs(path);
    if(!ifs.is_open())
    {
        printf("Cannot find shader file: %s\n", path.c_str());
        exit(3);
    }
    std::stringstream sstr;
    sstr << ifs.rdbuf();
    std::string cont = sstr.str();
    const char *content = cont.c_str();
    ifs.close();

    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &content, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        GLint infoLen;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        std::vector<GLchar> infoLog(infoLen+1);
        glGetShaderInfoLog(shader, infoLen, NULL, &infoLog[0]);
        printf("Shader %s failed to compile!\nInfo Log: %s\n", path.c_str(), &infoLog[0]);
        exit(4);
    }

    this->shaderFiles.push_back(shader);
}

void Shader::compile()
{
    this->programID = glCreateProgram();
    for(unsigned i = 0; i < this->shaderFiles.size(); i++)
    {
        glAttachShader(this->programID, this->shaderFiles[i]);
    }
    glLinkProgram(this->programID);

    GLint success;
    glGetProgramiv(this->programID, GL_LINK_STATUS, &success);
    if(!success)
    {
        GLint infoLen;
        glGetProgramiv(this->programID, GL_INFO_LOG_LENGTH, &infoLen);
        std::vector<GLchar> infoLog(infoLen+1);
        glGetProgramInfoLog(this->programID, infoLen, NULL, &infoLog[0]);
        printf("Failed to link shaders!\nInfo Log: %s\n", &infoLog[0]);
        exit(5);
    }

    for(unsigned i = 0; i < this->shaderFiles.size(); i++)
    {
        glDeleteShader(this->shaderFiles[i]);
    }
    this->shaderFiles.clear();
}

void Shader::use()
{
    glUseProgram(this->programID);
}