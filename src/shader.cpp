#include "shader.hpp"

Shader::Shader(std::string vpath, std::string fpath)
{
    GLuint vertShader;
    GLuint fragShader;
    
    std::ifstream ifs(vpath);
    if(!ifs.is_open())
    {
        printf("Cannot find shader file: %s\n", vpath.c_str());
        exit(3);
    }
    std::stringstream sstr;
    sstr << ifs.rdbuf();
    std::string vertCont = sstr.str();
    const char *vertContent = vertCont.c_str();
    ifs.close();
    ifs.clear();
    sstr.str("");

    ifs.open(fpath);
    if(!ifs.is_open())
    {
        printf("Cannot find shader file: %s\n", fpath.c_str());
        exit(3);
    }
    sstr << ifs.rdbuf();
    std::string fragCont = sstr.str();
    const char *fragContent = fragCont.c_str();
    ifs.close();

    vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertContent, NULL);
    glCompileShader(vertShader);

    GLint success;
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        GLint infoLen;
        glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &infoLen);
        std::vector<GLchar> infoLog(infoLen+1);
        glGetShaderInfoLog(vertShader, infoLen, NULL, &infoLog[0]);
        printf("Shader %s failed to compile!\nInfo Log: %s\n", vpath.c_str(), &infoLog[0]);
        exit(4);
    }

    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragContent, NULL);
    glCompileShader(fragShader);

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        GLint infoLen;
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &infoLen);
        std::vector<GLchar> infoLog(infoLen+1);
        glGetShaderInfoLog(fragShader, infoLen, NULL, &infoLog[0]);
        printf("Shader %s failed to compile!\nInfo Log: %s\n", fpath.c_str(), &infoLog[0]);
        exit(4);
    }

    this->programID = glCreateProgram();
    glAttachShader(this->programID, vertShader);
    glAttachShader(this->programID, fragShader);
    glLinkProgram(this->programID);

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

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

Shader::~Shader()
{
    glDeleteProgram(this->programID);
}

void Shader::use()
{
    glUseProgram(this->programID);
}