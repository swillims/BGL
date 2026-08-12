#pragma once
#include "scene/scene.h"

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

    //controls

    // vertice storing
    std::vector<float> batch;

    // scene methods
    void onLoad();
    void handle(float time = 0) override;
    void render(float time = 0, bool updateDisplay = true);
    void processInput(GLFWwindow* window, float time);
    void aspectChange();
    void clean();

    // Walker3D methods
};
