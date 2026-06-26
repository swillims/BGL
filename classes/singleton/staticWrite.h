#pragma once

#include "singleton/gl_core.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <unordered_map>
#include <filesystem>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>


//*
struct Character
{
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   size;      // Size of glyph
    glm::ivec2   bearing;   // Offset from baseline to left/top of glyph
    unsigned int advance;   // Horizontal offset to advance to next glyph
    glm::vec2 uv0;        // top-left UV in atlas
    glm::vec2 uv1;        // bottom-right UV in atlas
};

struct StaticWrite
{
    static StaticWrite* singleton;

    std::map<GLchar, Character> characters;
    std::unordered_map<unsigned int, std::vector<float>> channels;

    unsigned int VAO, VBO;
    unsigned int atlasTexID;
    unsigned int size = 1;

    int shader;

    // IMPORTANT
    // if you are using the static Init on multiple fonts, you need to be careful of leaks because it heap allocates the fonts.
    // single font is the only intended usage of Init and the engine. It allows multiple fonts but not well and using different instances.
    // implement your own storage/tracking for references to other instances if multiple fonts are uses.
    // StaticWrite is not tested for multiple fonts. It will probably not work correctly.
    // there is also no clean up on StaticWrite instances, so deleting one would require removing all openGL loaded assets(texture/shader/etc) to avoid a dataleak.
    static void Init(const std::string& font_name, unsigned int size, bool heapAllocate = true)
    {
        if (heapAllocate) { singleton = new StaticWrite(); }
        singleton->init(font_name, size);
    }
    void init(const std::string& font_name, unsigned int texSize);

    // call once per several draws
    static void StartWrite() { singleton->startWrite(); }
    void startWrite()
    {
        glUseProgram(shader);
        //shader.use();
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindTexture(GL_TEXTURE_2D, atlasTexID);
    }
    //*/
    static void DrawChannel(unsigned int channel, glm::vec3 color) { singleton->drawChannel(channel, color); }
    void drawChannel(unsigned int channel, glm::vec3 color)
    {
        //shader.use();
        glUniform3f(glGetUniformLocation(shader, "textColor"), color.x, color.y, color.z);
        glBindVertexArray(VAO);
        glBufferData(
            GL_ARRAY_BUFFER,
            channels[channel].size() * sizeof(float),
            channels[channel].data(),
            GL_STATIC_DRAW
        );
        glDrawArrays(GL_TRIANGLES, 0, channels[channel].size() / 4);
        glBindVertexArray(0);
    }
    void generateAtlas(const std::string& fontPath);
    void destroyChannels();
    static void DestroyChannels() { singleton->destroyChannels(); }
    void setUpChannel(unsigned int channel);
    static void SetUpChannel(unsigned int channel) { singleton->setUpChannel(channel); }
    std::vector<float>generateVertices(const std::string& text, float x, float y, float xScale, float yScale)
    {
        xScale /= size;
        yScale /= size;
        std::string::const_iterator c;
        std::vector<float> vertices;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = characters[*c];

            float xpos = x + ch.bearing.x * xScale;
            float ypos = y - (ch.size.y - ch.bearing.y) * yScale;

            float w = ch.size.x * xScale;
            float h = ch.size.y * yScale;
            vertices.insert(
                vertices.end(),
                {
                    xpos,     ypos + h, ch.uv0.x, ch.uv0.y, // v0
                    xpos,     ypos,     ch.uv0.x, ch.uv1.y, // v1
                    xpos + w, ypos,     ch.uv1.x, ch.uv1.y, // v2

                    xpos,     ypos + h, ch.uv0.x, ch.uv0.y, // v0
                    xpos + w, ypos,     ch.uv1.x, ch.uv1.y, // v2
                    xpos + w, ypos + h, ch.uv1.x, ch.uv0.y  // v3
                }
            );
            x += (ch.advance >> 6) * xScale;
        }
        return vertices;
    }
    static std::vector<float> GenerateVertices(const std::string& text, float x, float y, float xScale, float yScale)
    {
        return singleton->generateVertices(text,x,y,xScale,yScale);
    }
    void appendChannel(unsigned int channel, const std::vector<float>& vertices); // this is untested but works in theory.
    static void AppendChannel(unsigned int channel, const std::vector<float>& vertices) { singleton->appendChannel(channel, vertices); }
    void appendText(unsigned int channel, const std::string& text, float x, float y, float xScale, float yScale)
    {
        xScale /= size;
        yScale /= size;
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = characters[*c];

            float xpos = x + ch.bearing.x * xScale;
            float ypos = y - (ch.size.y - ch.bearing.y) * yScale;

            float w = ch.size.x * xScale;
            float h = ch.size.y * yScale;
            channels[channel].insert(
                channels[channel].end(),
                {
                    xpos,     ypos + h, ch.uv0.x, ch.uv0.y, // v0
                    xpos,     ypos,     ch.uv0.x, ch.uv1.y, // v1
                    xpos + w, ypos,     ch.uv1.x, ch.uv1.y, // v2

                    xpos,     ypos + h, ch.uv0.x, ch.uv0.y, // v0
                    xpos + w, ypos,     ch.uv1.x, ch.uv1.y, // v2
                    xpos + w, ypos + h, ch.uv1.x, ch.uv0.y  // v3
                }
            );
            x += (ch.advance >> 6) * xScale;
        }
    }
    static void AppendText(unsigned int channel, const std::string& text, float x, float y, float xScale, float yScale) { singleton->appendText(channel, text, x, y, xScale, yScale); }
};
