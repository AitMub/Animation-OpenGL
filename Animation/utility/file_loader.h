#ifndef FILE_LOADER_H
#define FILE_LOADER_H

#include <stb_image.h>
#define STB_IMAGE_IMPLEMENTATION

#include<string>
using std::string;

unsigned int LoadTextureFromFile(const char* path, const string& directory, bool gamma = false) {
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

Texture LoadTextureFromMemory(const aiTexture* texture_in_mem) {
    Texture texture = Texture();
    unsigned int ID;
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    texture.id = ID;
    texture.type = "texture_diffuse";

    unsigned char* image_data = nullptr;
    int width, height, components_per_pixel;
    if (texture_in_mem->mHeight == 0)
    {
        image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture_in_mem->pcData), texture_in_mem->mWidth, &width, &height, &components_per_pixel, 0);
    }
    else
    {
        image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture_in_mem->pcData), texture_in_mem->mWidth * texture_in_mem->mHeight, &width, &height, &components_per_pixel, 0);
    }

    if (components_per_pixel == 3)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    }
    else if (components_per_pixel == 4)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

#endif
