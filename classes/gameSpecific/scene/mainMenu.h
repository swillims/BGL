#pragma once

#include "singleton/staticDraw.h"
#include "singleton/staticSound.h"
#include "singleton/staticWrite.h"
#include "singleton/staticInput.h"

// scenes
#include "frogHop.h"
#include "soundOptions.h"
#include "walker3D.h"

struct MainMenu : Scene
{
    // image refs
    unsigned int backgroundRef;
    unsigned int buttonImageRef;

    // shader refs
    unsigned int shaderSimpleRef;
    unsigned int colorShaderRef;

    // sound refs
    unsigned int bwoo;
    unsigned int backgroundMusic;

    // writer
    StaticWrite* writer;

    // UI
    UIBase ui;
    std::vector<float> uiBatch;

    unsigned int uiTextChannel = 0;

    // click handling
    int buttonHover = -1;
    int buttonStart = -1;

    void onLoad() override
    {
        Scene::onLoad();

        // images
        if (!StaticDraw::imageFileRefs.contains("mainMenuBackground"))
        {
            StaticDraw::loadImage("assets/core/background.png", "mainMenuBackground", false);
        }
        backgroundRef = StaticDraw::imageFileRefs["mainMenuBackground"];

        if (!StaticDraw::imageFileRefs.contains("button.png"))
        {
            StaticDraw::loadImage("assets/core/button.png");
        }
        buttonImageRef = StaticDraw::imageFileRefs["button.png"];

        // shaders
        shaderSimpleRef = StaticDraw::getShader("simple");

        if (!StaticDraw::hasShader("colorShader"))
        {
            StaticDraw::compileShader("assets/shaders/simple.vs", "assets/shaders/color.fs", "colorRef");
        }
        colorShaderRef = StaticDraw::getShader("colorRef");
        StaticDraw::useShader(colorShaderRef);
        GLint colorLoc = glGetUniformLocation(colorShaderRef, "color");
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 0.5f);

        // sounds
        if (!StaticAudio::soundStringRefs.contains("menuBloo.wav"))
        {
            StaticAudio::load("assets/core/menuBloo.wav","menuBloo.wav",{"soundEffect"});
        }
        bwoo = StaticAudio::soundStringRefs["menuBloo.wav"];

        if (!StaticAudio::soundStringRefs.contains("2151bar"))
        {
            StaticAudio::load("assets/gameSpecific/sound/2151bar.wav","2151bar",{"music"});
        }
        backgroundMusic = StaticAudio::soundStringRefs["2151bar"];

        StaticAudio::updateSounds();

        StaticAudio::playSoundLoop(backgroundMusic);

        // set up writer
        writer = StaticWrite::singleton;
        writer->destroyChannels();

        // set up UI
        ui.nodes.clear();
        ui.appendType<UIYHolder>()
            .appendType<UIBuffer>(0.10f, 0.10f, 0.05f, 0.05f)
                .appendType<UIXRatio>(4)
                    .appendType<UIStack>()
                        .appendType<UITextOneLineConst>(uiTextChannel, "Frog Hop", 0.5f, XCENTER,YCENTER).back()
                        .appendType<TexUVNode>().setKey(3).back()
                        .back()
                    .back()
                .back()
            .appendType<UIBuffer>(0.10f, 0.10f, 0.05f, 0.05f)
                .appendType<UIXRatio>(4)
                    .appendType<UIStack>()
                        .appendType<UITextOneLineConst>(uiTextChannel, "Walk 3D", 0.5f, XCENTER,YCENTER).back()
                        .appendType<TexUVNode>().setKey(2).back()
                        .back()
                    .back()
                .back()
            .appendType<UIBuffer>(0.10f, 0.10f, 0.05f, 0.05f)
                .appendType<UIXRatio>(4)
                    .appendType<UIStack>()
                        .appendType<UITextOneLineConst>(uiTextChannel, "Options", 0.5f, XCENTER,YCENTER).back()
                        .appendType<TexUVNode>().setKey(1).back()
                        .back()
                    .back()
                .back()
            .appendType<UIBuffer>(0.10f, 0.10f, 0.05f, 0.05f)
                .appendType<UIXRatio>(4)
                    .appendType<UIStack>()
                        .appendType<UITextOneLineConst>(uiTextChannel, "Exit", 0.5f, XCENTER,YCENTER).back()
                        .appendType<TexUVNode>().setKey(0).back()
                        .back()
                    .back()
                .back()
        ;

        // set up input
        StaticInput::MouseTrack(GLFW_MOUSE_BUTTON_LEFT);

        buttonStart = -1;
        aspectChange();
    }

    void render(float time = 0, bool updateDisplay = true) override
    {
        // use default shader
        StaticDraw::useShader(shaderSimpleRef);

        // draw background
        StaticDraw::backGroundImageRepeat(backgroundRef, 3.5);

        // draw buttons
        StaticDraw::multiDraw(buttonImageRef, uiBatch);

        // draw letters
        writer->startWrite();
        writer->drawChannel(uiTextChannel, glm::vec3(0.0f, 0.0f, 0.0f));

        // draw highlighted button
        if (buttonHover != -1 || buttonStart != -1)
        {
            int hoverIndex = buttonHover;
            if (buttonStart != -1)
            {
                hoverIndex = buttonStart;
            }
            if (hoverIndex != -1)
            {
                std::vector<float> hoverBatch = ui.findByKey(hoverIndex).getVerts();
                StaticDraw::useShader(colorShaderRef);
                StaticDraw::multiDraw(0,hoverBatch);
            }
        }

        Scene::render(time, updateDisplay);
    }

    void handle(float time = 0) override
    {
        processInput();
    }

    void aspectChange() override
    {
        StaticDraw::updateView();

        uiBatch.clear();

        writer->setUpChannel(uiTextChannel);

        ui.adjustNodeDefault();

        ui.renderVerts(uiBatch);
    }

    void processInput()
    {
        Scene::processInput();
        StaticInput::Tick();
        updateMouseHover();

        if (StaticInput::MouseClick(GLFW_MOUSE_BUTTON_LEFT))
        {
            buttonStart = buttonHover;
        }
        else if (StaticInput::MouseRelease(GLFW_MOUSE_BUTTON_LEFT))
        {
            if (buttonHover == buttonStart)
            {
                buttonPress(buttonStart);
            }
            buttonStart = -1;
        }
    }

    void updateMouseHover()
    {
        double mouseX;
        double mouseY;

        StaticInput::GetMouse(mouseX, mouseY);

        int hoveredKey = ui.findOneHover(static_cast<float>(mouseX), static_cast<float>(mouseY));

        if (hoveredKey != -1)
        {
            if (buttonHover != hoveredKey && buttonStart == -1)
            {
                buttonHover = hoveredKey;
                StaticAudio::playSoundEffectMulti(bwoo);
            }
            return;
        }

        buttonHover = -1;
    }

    void buttonPress(int x)
    {
        if (x == 0)
        {
            glfwSetWindowShouldClose(window, true);
        }
        else if (x == 1)
        {
            SoundOptions* options = new SoundOptions();
            options->previous = this;
            DataHolder::SceneQueue(options, false);
        }
        else if (x == 2)
        {
            DataHolder::SceneQueue(new Walker3D(), true);
        }
        else if (x == 3)
        {
            DataHolder::SceneQueue(new FrogHop(), true);
        }
    }

    void clean() override
    {
        StaticAudio::stopSound(backgroundMusic);
    }
};