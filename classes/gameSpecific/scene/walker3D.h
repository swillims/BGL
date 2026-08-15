#pragma once
#include "scene/scene.h"
#include <glm/glm.hpp>

struct Player
{
    glm::vec3 position{glm::vec3(0.0f, 1.0f, 0.0f)};
    glm::vec3 direction{glm::vec3(0.0f, 0.0f, 0.0f)};
    float camYaw{};
    float camPitch{};
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

    //shader
    unsigned int shaderSimpleRef;
    unsigned int shader3DSimple;
    unsigned int shader3DProjection;

    //ubo
    unsigned int projectionUboRef;
    unsigned int viewUboRef;

    // controls

    // vertice storing
    glm::mat4 viewMat4;
    glm::mat4 projectionMat4;
    std::vector<float> floor;
    std::vector<float> batch;

    // game logic
    Player player;

    // scene methods
    void onLoad();
    void handle(float time = 0) override;
    void render(float time = 0, bool updateDisplay = true);
    void processInput(GLFWwindow* window, float time);
    void aspectChange();
    void clean();

    // Walker3D methods
};
