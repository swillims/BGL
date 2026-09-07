#pragma once
#include "scene/scene.h"
#include <glm/glm.hpp>
#include <string>
#include "uiHelper.h"

#include <algorithm>
#include <limits>

struct Player
{
    glm::vec3 position{glm::vec3(0.0f, 1.0f, 0.0f)};
    glm::vec3 direction{glm::vec3(0.0f, 0.0f, 0.0f)};
    float camYaw{};
    float camPitch{};
    float moveSpeed = 2.5f;
    float rotateSpeed = 1.0f;
    float playerVX = 0.0f;
    float playerVZ = 0.0f;
};

struct TwoDThreeDImg
{
    // world coordinates
    float x,y,z;
    float deltaX, height;
    // screen coordinates
    float xMin, xMax;
    float yMin, yMax;

    // I don't think I use vertices. I'm not deleting it because I might change the render technique.
    // - I need to calculate vertices CPU side for figuring out ray trace/mouse hover thing and I have two bad options relating to it
    // -- I can calculate coordinates twice. Having it recalculated GPU side would allow clipping.
    // --- Doing this option atm. Vertices would be if I decided to calculate one time.
    // -- I can switch to using manual vertice calls which disable clipping but be higher performance.
    std::vector<float> vertices;

    TwoDThreeDImg(float x, float y, float z, float deltaX, float height) : x(x), y(y), z(z), deltaX(deltaX/2), height(height) {}

    void updatePosition(float newX, float newY, float newZ)
    {
        x=newX;
        y=newY;
        z=newZ;
    }

    void updateVertices(const glm::mat4& view, const glm::mat4&projection)
    {
        glm::vec3 right = glm::vec3(view[0]);

        glm::vec3 position(x, y, z);

        glm::vec3 bottomLeft = position - right * deltaX;
        glm::vec3 bottomRight = position + right * deltaX;
        glm::vec3 topLeft = bottomLeft + glm::vec3(0.0f, height, 0.0f);
        glm::vec3 topRight = bottomRight + glm::vec3(0.0f, height, 0.0f);

        glm::mat4 vp = projection * view;

        glm::vec4 projected[4] =
        {
            vp * glm::vec4(bottomLeft,  1.0f),
            vp * glm::vec4(bottomRight, 1.0f),
            vp * glm::vec4(topLeft,     1.0f),
            vp * glm::vec4(topRight,    1.0f)
        };

        float minX =  std::numeric_limits<float>::max();
        float maxX = -std::numeric_limits<float>::max();
        float minY =  std::numeric_limits<float>::max();
        float maxY = -std::numeric_limits<float>::max();

        for (const glm::vec4& p : projected)
        {
            float screenX = p.x / p.w;
            float screenY = p.y / p.w;

            minX = std::min(minX, screenX);
            maxX = std::max(maxX, screenX);

            minY = std::min(minY, screenY);
            maxY = std::max(maxY, screenY);
        }

        xMin = minX;
        xMax = maxX;
        yMin = minY;
        yMax = maxY;

        float yh = y + height;
        vertices =
        {
            // x, y, z, u, v, delta
            x, y, z, 0.0f, 0.0f, -deltaX,
            x, y, z, 1.0f, 0.0f, deltaX,
            x, yh, z, 1.0f, 1.0f, deltaX,

            x, y, z, 0.0f, 0.0f, -deltaX,
            x, yh, z, 1.0f, 1.0f, deltaX,
            x, yh, z, 0.0f, 1.0f, -deltaX,
        };
    }
};

struct Walker3D : Scene
{
    //images
    unsigned int tile;
    unsigned int greenTile;
    unsigned int multiTile;

    //vao
    unsigned int baseVao;
    unsigned int tileVaoRef;
    unsigned int tileVaoCount;
    unsigned int threeDTwoDRef;
    unsigned int threeDTwoDCount;

    //shader
    unsigned int shaderSimpleRef;
    unsigned int shader3DSimple;
    unsigned int shader3DProjection;
    unsigned int shader3d2d;

    //ubo
    unsigned int projectionUboRef;
    unsigned int viewUboRef;

    // controls
    unsigned int qKey;
    unsigned int wKey;
    unsigned int eKey;
    unsigned int aKey;
    unsigned int sKey;
    unsigned int dKey;
    unsigned int escKey;

    bool rotateQ;
    bool walkW;
    bool rotateE;
    bool walkA;
    bool walkS;
    bool walkD;
    bool menuEsc;

    // graphic math and vertice storing
    glm::mat4 viewMat4;
    glm::mat4 projectionMat4;
    std::vector<float> floor;
    std::vector<float> uiBatch;
    std::vector<TwoDThreeDImg> images;
    std::vector<float> imageBatch;

    // game logic
    bool alreadyLoaded = false;
    Player player;

    // hover logic
    unsigned int hoveredImage;
    unsigned int hoveredTileX;
    unsigned int hoveredTileY;

    // ui
    UIBase ui;
    std::string menuString;
    std::string playerWalkString;
    std::string rotateString;
    std::string hoverString;
    unsigned int uiTextChannel = 1;

    // scene methods
    void onLoad();
    void handle(float time = 0) override;
    void render(float time = 0, bool updateDisplay = true);
    void processInput(GLFWwindow* window, float time);
    void aspectChange();
    void clean();

    // Walker3D methods
    int getHoveredImage();
    int getTile();
    void updateUI();
};
