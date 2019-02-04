// This code is mainly copied from the book with minor changes
// check https://learnopengl.com to learn more!
#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <SOIL/SOIL.h>
#include <stdio.h>
#include "./Shader.hpp"
#include "./Mesh.hpp"

GLuint loadTextureFromFile(const std::string filename, const std::string directory = "./Images", GLboolean alpha = false);

class Model
{
public:
    // Functions
    Model(std::string path);
    void draw(Shader shader);
    bool exists();
private:
    // Model Data
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;
    // Functions
    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};