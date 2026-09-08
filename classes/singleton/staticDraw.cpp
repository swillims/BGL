#include "staticDraw.h"

#include "singleton/gl_core.h"

#ifdef _WIN32
    #define STB_IMAGE_IMPLEMENTATION
    #include <stb_image.h>
#elif __linux__
    #define STB_IMAGE_IMPLEMENTATION
    #include <stb_image.h>
#endif

void StaticDraw::resize(int newW, int newH)
{
    glfwSetWindowSize(window, newW, newH);
    updateView();
}

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

    if (imageName.empty()) { imageFileRefs.insert(texture, util::cleanFileName(fileName)); }
    else { imageFileRefs.insert(texture, util::cleanFileName(imageName)); }

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // load and generate the texture
    int width, height, nrChannels;
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

void StaticDraw::crateLayerImage(GLsizei width, GLsizei height, std::string imageName, unsigned int depth)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    if (imageName.empty()){imageName=texture;} // note: this sets a default name. The default name is a non-printable character if 32 or lower

    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage3D(GL_TEXTURE_2D_ARRAY,0, GL_RGBA, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    imageFileRefs.insert(texture, imageName);

    multiImages.emplace_back(imageName, texture, width, height, depth);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void StaticDraw::loadLayerImage(int multiImageRef, const std::string &fileName, const std::string &imageName, bool flip)
{
    for (MultiImage& m : multiImages)
    {
        if (m.ref==multiImageRef)
        {
            m.addLayer(fileName, imageName, flip);
            return;
        }
    }
    std::cout << "load layer failed due to file to find texture\n";
}

void StaticDraw::loadLayerImage(const std::string &multiImageRef, const std::string &fileName, const std::string &imageName, bool flip)
{
    for (MultiImage& m : multiImages)
    {
        if (m.name==multiImageRef)
        {
            m.addLayer(fileName, imageName, flip);
            return;
        }
    }
    std::cout << "load layer failed due to file to find texture\n";
}

GLuint StaticDraw::MultiImage::addLayer(const std::string& fileName, std::string imageName, bool flip)
{
    stbi_set_flip_vertically_on_load(flip);

    int imageWidth;
    int imageHeight;
    int nrChannels;

    unsigned char* data = stbi_load(fileName.c_str(), &imageWidth, &imageHeight, &nrChannels, 4);

    if (!data)
    {
        std::cout << "Failed to load layer image\n";
        return 0;
    }

    if (imageWidth < width || imageHeight < height)
    {
        std::cout << "Failed to load layer image because dimensions larger than allocated\n";
        stbi_image_free(data);
        return 0;
    }

    GLuint layer = layers;

    if (layers < capacity)
    {
        glBindTexture(GL_TEXTURE_2D_ARRAY, ref);

        glTexSubImage3D( GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

        if (imageName.empty()){imageName = util::cleanFileName(fileName);}
        else {imageName = util::cleanFileName(imageName);}

        layerRefs.insert(layer, imageName);

        layers++;

        stbi_image_free(data);

        return layer;
    }

    GLsizei newCapacity = capacity * 2;

    std::cout << "Caution: At capacity and new layer added so resizing. Resizing Layer Image from " << capacity << " to " << newCapacity << " layers\n";

    // Read the currently allocated texture into CPU memory.
    std::vector<unsigned char> oldData(static_cast<size_t>(width) * static_cast<size_t>(height) * static_cast<size_t>(layers) * 4);

    glBindTexture(GL_TEXTURE_2D_ARRAY, ref);

    if (layers > 0)
    {
        glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, GL_UNSIGNED_BYTE, oldData.data());
    }

    GLuint oldRef = ref;
    GLuint newRef;

    glGenTextures(1, &newRef);
    glBindTexture(GL_TEXTURE_2D_ARRAY, newRef);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, width, height, newCapacity, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    if (layers > 0)
    {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, layers, GL_RGBA, GL_UNSIGNED_BYTE, oldData.data());
    }

    glTexSubImage3D( GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    glDeleteTextures(1, &oldRef);

    imageFileRefs.erase(oldRef);
    imageFileRefs.insert(newRef, name);

    ref = newRef;
    capacity = newCapacity;

    if (imageName.empty()){imageName = util::cleanFileName(fileName);}
    else {imageName = util::cleanFileName(imageName);}

    layerRefs.insert(layer, imageName);

    layers++;

    stbi_image_free(data);

    return layer;
}
