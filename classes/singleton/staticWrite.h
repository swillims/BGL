#pragma once

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <iostream> // test strings
#include <unordered_map>
#include <filesystem>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include "shader.h"
//#include "staticDraw.h" // used for shaders
//#include "glue/textShell.h"


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

/*
struct storeChar {
    float xpos, ypos;
    float width, height;
    GLuint textureID;
    //float u0, v0, u1, v1; // texture coords (optional if always full)
};
//*/

struct StaticWrite
{
public:
    static StaticWrite* singleton;

    //std::map<GLchar, Character> Characters;
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
    /* 
    {
        size = texSize;
        //shader = Shader("assets/shaders/write.vs", "assets/shaders/write.fs");
        

        //glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f);
        shader.use();
        //glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        generateAtlas(font_name);  // uploat library of characters as a texture atlas
        destroyChannels(); // destroy channels sets channels up as an empty map. It should already be blank but init should be called sparingly, so neglible.
        setUpChannel(0); // connect default channel. If setUp channel is called a second time, it should write over default channel so there is 0 concern for default getting in the way.

        // setup VAO/VBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // the next line is from an older version. just ignore it. It should be deleted later.
        //StaticWrite::singleton = this;
    }
    //*/

    // call once per several draws
    static void StartWrite() { singleton->startWrite(); }
    void startWrite()
    {
        glUseProgram(shader);
        //shader.use();
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindTexture(GL_TEXTURE_2D, atlasTexID);
    }
    /*
    static void RenderText(TextShell ts)
    {
        if (ts.wrap) { RenderWrapText(ts); }
        else { RenderText(ts.text, ts.x, ts.y, ts.scale, ts.color); }
    }
    
    //void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color)
    static void RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color)
    {
        singleton->renderText(text,x,y,scale,color);
    }
    void renderText(const std::string& text, float x, float y, float scale, glm::vec3 color)
    {
        glBindTexture(GL_TEXTURE_2D, atlasTexID);
        // activate corresponding render state	
        shader.use();
        glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        // iterate through all characters
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            //Character ch = Characters[*c];
            Character ch = characters[*c];

            float xpos = x + ch.bearing.x * scale;
            float ypos = y - (ch.size.y - ch.bearing.y) * scale;

            float w = ch.size.x * scale;
            float h = ch.size.y * scale;
            // update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   ch.uv0.x, ch.uv0.y },
                { xpos,     ypos,       ch.uv0.x, ch.uv1.y },
                { xpos + w, ypos,       ch.uv1.x, ch.uv1.y },

                { xpos,     ypos + h,   ch.uv0.x, ch.uv0.y },
                { xpos + w, ypos,       ch.uv1.x, ch.uv1.y },
                { xpos + w, ypos + h,   ch.uv1.x, ch.uv0.y }
            };
            // render glyph texture over quad
           //glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            //glBindTexture(GL_TEXTURE_2D, atlasTexID);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    static void RenderWrapText(TextShell ts) { singleton->renderWrapText(ts); }
    void renderWrapText(TextShell ts)
    {
        std::string text = ts.text;
        float finalX = ts.x;
        float x = finalX;
        float wrapWidth = ts.wrapWidth + finalX;

        float y = ts.y;
        float ySpacing = ts.ySpacing;
        float scale = ts.scale;
        bool newLine;
        glm::vec3 color = ts.color;
        // activate corresponding render state	
        shader.use();
        glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        // iterate through all characters
        int lastSpace = 0;
        int yCount = 0;
        int count = lastSpace;
        std::string::const_iterator c;
        std::vector< storeChar> preRend;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = Characters[*c];

            storeChar sc;
            sc.xpos = x + ch.bearing.x * scale;
            sc.ypos = y - (ch.size.y - ch.bearing.y) * scale;
            sc.width = ch.size.x * scale;
            sc.height = ch.size.y * scale;
            sc.textureID = ch.TextureID;

            if (*c == ' ') { lastSpace = count; }
            else if (*c == '\n')
            {
                lastSpace = count;
                x = wrapWidth;
                newLine = true;
            }
            else { preRend.push_back(sc); }

            x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
            count++;
            if (x > wrapWidth)
            {
                if (lastSpace != 0)
                {
                    //std::cout << "Space detected: " << lastSpace << " " << count << " " << *c << "\n";
                    int n = count - lastSpace;
                    c -= n;
                    c++;
                    //if (newLine) { newLine = false; c++; }
                    while (n > 1)
                    {
                        preRend.pop_back();
                        n--;
                    }
                }
                for (storeChar const pr : preRend)
                {
                    float vertices[6][4] = {
                    { pr.xpos,     pr.ypos + pr.height,   0.0f, 0.0f }, // looks ugly because reverse engineered from dumb ass tutorial code written by a boomer and was too lazy to realign
                    { pr.xpos,     pr.ypos,       0.0f, 1.0f },
                    { pr.xpos + pr.width, pr.ypos,       1.0f, 1.0f },

                    { pr.xpos,     pr.ypos + pr.height,   0.0f, 0.0f },
                    { pr.xpos + pr.width, pr.ypos,       1.0f, 1.0f },
                    { pr.xpos + pr.width, pr.ypos + pr.height,   1.0f, 0.0f }
                    };
                    glBindTexture(GL_TEXTURE_2D, pr.textureID);
                    glBindBuffer(GL_ARRAY_BUFFER, VBO);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                }
                preRend.clear();
                x = finalX;
                count = 0;
                yCount++;
                y = y - ySpacing;
            }
        }

        //DataHolder::god.intChannel[ts.channel] = yCount;
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
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
    void appendChannel(unsigned int channel, const std::vector<float>& vertices); // this is untested but works in theory.
    //static void AppendTextTs(unsigned int channel, TextShell ts) { channel, singleton->appendTextTs(channel, ts); } // bad code but rework later if TextShell is kept post rework
    //void appendTextTs(unsigned int channel, TextShell ts)
    //{
    //    appendText(channel, ts.text, ts.x, ts.y, ts.scale, ts.scale);
    //}
    static void AppendText(unsigned int channel, const std::string& text, float x, float y, float xScale, float yScale){ singleton->appendText(channel, text, x, y, xScale, yScale); }
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
    
};
