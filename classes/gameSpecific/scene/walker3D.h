#pragma once
#include "scene/scene.h"
#include <glm/glm.hpp>
#include <string>
#include "uiHelper.h"

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

struct Walker3D : Scene
{
    //images
    unsigned int tile;
    unsigned int greenTile;

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

    // vertice storing
    glm::mat4 viewMat4;
    glm::mat4 projectionMat4;
    std::vector<float> floor;
    std::vector<float> uiBatch;

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
