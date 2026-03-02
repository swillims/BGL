#include "staticWrite.h"
#include "staticDraw.h" // used for shaders

StaticWrite* StaticWrite::singleton;
//Shader StaticWrite::shader;



void StaticWrite::init(const std::string& font_name, unsigned int texSize)
{
    {
        size = texSize;
        //shader = Shader("assets/shaders/write.vs", "assets/shaders/write.fs");
        shader = StaticDraw::compileShader("assets/shaders/write.vs", "assets/shaders/write.fs", "StaticWrite");

        //glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f);
        glUseProgram(shader);
        //shader.use();
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
}

void StaticWrite::generateAtlas(const std::string& fontPath)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        return; // handle error

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
        return; // handle error

    FT_Set_Pixel_Sizes(face, 0, size);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    int ATLAS_W = 16 * size;
    int ATLAS_H = 16 * size;

    glGenTextures(1, &atlasTexID);
    glBindTexture(GL_TEXTURE_2D, atlasTexID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ATLAS_W, ATLAS_H, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int penX = 0, penY = 0, rowH = 0;

    for (unsigned char c = 0; c < 128; ++c)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            continue;

        FT_Bitmap& bmp = face->glyph->bitmap;

        if (penX + bmp.width >= ATLAS_W)
        {
            penX = 0;
            penY += rowH;
            rowH = 0;
        }

        // copy glyph bitmap into atlas
        glTexSubImage2D(GL_TEXTURE_2D, 0, penX, penY, bmp.width, bmp.rows, GL_RED, GL_UNSIGNED_BYTE, bmp.buffer);

        // store glyph info
        Character ch;
        //ch.size = { bmp.width, bmp.rows };
        ch.size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
        //ch.bearing = { face->glyph->bitmap_left, face->glyph->bitmap_top };
        ch.bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
        //ch.advance = face->glyph->advance.x >> 6; // convert from 1/64 px to px
        ch.advance = static_cast<unsigned int>(face->glyph->advance.x);

        // store UV coordinates (0..1)
        ch.uv0 = { float(penX) / ATLAS_W, float(penY) / ATLAS_H };
        ch.uv1 = { float(penX + bmp.width) / ATLAS_W, float(penY + bmp.rows) / ATLAS_H };

        characters[c] = ch;

        penX += bmp.width;
        rowH = std::max(rowH, int(bmp.rows));
    }

    //glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void StaticWrite::destroyChannels()
{
    channels = std::unordered_map<unsigned int,std::vector<float>>();
}


void StaticWrite::setUpChannel(unsigned int channel)
{
    channels[channel] = std::vector<float>();
}

void StaticWrite::appendChannel(unsigned int channel, const std::vector<float>& vertices)
{
    channels[channel].insert(channels[channel].end(), vertices.begin(), vertices.end());
}




