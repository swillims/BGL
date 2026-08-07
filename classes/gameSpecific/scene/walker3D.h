#pragma once
#include "scene/scene.h"

struct Walker3D : Scene
{

    // scene methods
    void onLoad();
    void handle(float time = 0) override;
    void render(float time = 0, bool updateDisplay = true);
    void processInput(GLFWwindow* window, float time);
    void aspectChange();
    void clean();

    // Walker3D methods
};
