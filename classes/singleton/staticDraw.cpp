#include "staticDraw.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include <stb_image.h>

void StaticDraw::updateView()
{
    glfwGetFramebufferSize(window, &w, &h);
    aspectRatio = float(w) / float(h);
}

void StaticDraw::unLoadImage(const std::string& ref)
{
    std::cout << "unloading: " << ref << "\n";
    if(imageFileRefs.contains(ref))
    {
        unsigned int id = imageFileRefs[ref];
        glDeleteTextures(1, &id);
        imageFileRefs.erase(ref);
    }
}

void StaticDraw::unLoadImage(unsigned int ref)
{
    std::cout << "unloading: " << ref << "\n";
    if (imageFileRefs.contains(ref))
    {
        unsigned int id = ref;
        glDeleteTextures(1, &id);
        imageFileRefs.erase(ref);
    }
    //glDeleteTextures(1, &ref);
    //imageFileRefs.erase(ref);
    // code for previous version
    //for (const std::pair<const std::string, int>& pair : imageFileRefs)
    //{
    //    if (pair.second == ref)
    //    {
    //        unLoadImage(pair.first);
    //        return;
    //    }
    //}
    //std::cout << "failed to find texture on unload: " << ref << "\n";
}

void StaticDraw::loadImage(std::string fileName, std::string imageName, bool flip)
{
    if (flip)
    {
        stbi_set_flip_vertically_on_load(true);
    }
    else
    {
        stbi_set_flip_vertically_on_load(false);
    }
    std::cout << " image load" << fileName << " " << imageName << "\n";
    unsigned int texture;
    glGenTextures(1, &texture);
    
    //if (imageName.empty()) { imageFileRefs[util::cleanFileName(fileName)] = texture; }
    if (imageName.empty()) { imageFileRefs.insert(texture, util::cleanFileName(fileName)); }
    //else { imageFileRefs[util::cleanFileName(imageName)] = texture; }
    else { imageFileRefs.insert(texture, util::cleanFileName(imageName)); }

    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on currently bound texture)
    //if (clamp)
    //{
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //}
    //else
    //{
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //}
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // load and generate the texture
    int width, height, nrChannels;
    //std::cout << fileName << "\n";
    unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        if (nrChannels == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
}
