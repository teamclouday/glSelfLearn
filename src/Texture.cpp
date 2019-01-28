#include "../include/Texture.hpp"

Texture::Texture()
{
    glGenTextures(1, &textureID);
}

Texture::~Texture()
{
    glDeleteTextures(1, &textureID);
}

void Texture::bind()
{
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Texture::load(const std::string imageName)
{
    int width, height;
    unsigned char *image = SOIL_load_image((("./Textures/" + imageName).c_str()), &width, &height, 0, SOIL_LOAD_RGB);
    if(!image)
    {
        printf("Cannot load image: %s!\n", imageName.c_str());
        return false;
    }
    bind();
    // configure texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    // end of configure texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    unbind();
    return true;
}