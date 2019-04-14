// This code is mainly copied from the book with minor changes
// check https://learnopengl.com to learn more!
#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <string>
#include <vector>
#include <sstream>
#include "./Shader.hpp"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture
{
    GLuint id;
    std::string type;
    aiString path;
};

class Mesh
{
public:
    // Mesh Data
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;
    // Functions
    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
    void draw(Shader shader);
    // Render Data
    GLuint VAO, VBO, EBO;
private:
    // Functions
    void setupMesh();
};