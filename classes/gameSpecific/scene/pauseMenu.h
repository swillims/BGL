#pragma once
#include <string>
#include <vector>

#include "uiHelper.h"
#include "scene/scene.h"


struct PauseMenu : Scene
{
private:
    enum uiKeys
    {
        uiReturnToGame,
        uiOptions,
        uiMainMenu,
        uiCloseProgram
    };
public:
    // images
    unsigned int uITex;

    // shaders
    unsigned int colorShaderRef;
    unsigned int shaderSimpleRef;

    // batch for rendering
    std::vector<float> batch;

    // text channel for writing
    unsigned int writeChannel;

    // strings
    std::string returnStr;
    std::string optionsStr;
    std::string mainMenuStr;
    std::string exitStr;

    // ui root
    UIXRatio ui;//(0.f, 0.f, 1.f, 1.f);

    // scene to return to
    Scene* previous;

    PauseMenu() : ui(-1, -1, 2, 2, 1.0, true) {}
    PauseMenu(Scene* scene) : ui(-1, -1, 2, 2, 1.0, true), previous(scene) {};


    void onLoad() override;
    void render(float time, bool updateDisplay) override;
    void handle(float time) override;
    void aspectChange() override;
    void processInput();
    void buttonPress(int x);
    void setTextChannel(int x);
};


