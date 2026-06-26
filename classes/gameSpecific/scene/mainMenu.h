#pragma once

#include "singleton/staticDraw.h"
#include "singleton/staticSound.h"
#include "singleton/staticWrite.h"
#include "singleton/staticInput.h"

// scenes
#include "frogHop.h"
#include "soundOptions.h"

/*
    This scene was written before the uiHelper util class and StaticInput were made.
    - If you want a better tutorial for how to make uiElements, the options menus are better examples:
    -- keyOptions graphicsOptions, soundOptions

    The easiest way to make a starting menu for a new game is to modify this scene.
    - The cleanest way is to make a new scene written with uiHelper.
*/

struct MainMenu : Scene {

    // image refs
    unsigned int backgroundRef;
    unsigned int buttonImageRef;

    // shader refs
    unsigned int shaderSimpleRef;
    unsigned int colorShaderRef;

    // sound refs
    unsigned int bwoo;
    unsigned int backgroundMusic;

    // click handling tools
    unsigned int buttonHover = -1;
    unsigned int buttonStart = -1;
    bool click;
    bool loadAntiClick;

    // writer
    StaticWrite* writer;

    // math
    int a = 4; // hard coded number of buttons // bad but not worth refactoring
    float b = 1.0f / a; // inverse of "a"
    float buttonHeightExtra = .05f;

    // graphic things
    std::vector<float> yCords;
    std::vector<float> batch;
    std::vector<float> hoverBatch;

    // window vars
    int winWidth, winHeight;

    // ran when scene is loaded
    void onLoad() override
    {
        // extremely important to call super at start of onLoad to declare window
        Scene::onLoad();

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

        if (!StaticDraw::hasShader("colorShader"))
        {
            StaticDraw::compileShader("assets/shaders/simple.vs", "assets/shaders/color.fs", "colorRef");
        }
        colorShaderRef = StaticDraw::getShader("colorRef");
        // set color for color shader
        // -> very important <-
        // engine does not directly handle shader code, so learn how to do your own shaders
        StaticDraw::useShader(colorShaderRef); // selecting shader is needed to modify shader for smoe reason
        GLint colorLoc = glGetUniformLocation(colorShaderRef, "color"); // get uniform location. Uniforms are shader vars
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 0.5f);  // change shader unfiorm

        // load sounds
        if (!StaticAudio::soundStringRefs.contains("menuBloo.wav"))
        {
            StaticAudio::load("assets/core/menuBloo.wav", "menuBloo.wav", { "soundEffect" });
        }
        bwoo = StaticAudio::soundStringRefs["menuBloo.wav"];

        if (!StaticAudio::soundStringRefs.contains("2151bar"))
        {
            StaticAudio::load("assets/gameSpecific/sound/2151bar.wav", "2151bar", { "music" });
        }
        backgroundMusic = StaticAudio::soundStringRefs["2151bar"];

        StaticAudio::updateSounds();

        // declared on StaticDraw Init
        shaderSimpleRef = StaticDraw::getShader("simple");

        // set up write
        writer = StaticWrite::singleton;
        writer->destroyChannels();
        
        // do yCords for menu button here instead of aspect change because they do not change when ratio changes
        yCords = {};
        int aa = a + 1;
        float af = a;
        float bfh = buttonHeightExtra;
        for (float i = 1.5f; i < aa * 2 - 1; i++) 
        {
            bfh *= -1; // this is a hack
            yCords.push_back((i - aa) / af + bfh);
        }
        
        aspectChange();
        
        // stop autoclick on menu load
        loadAntiClick = true;

        // play background music
        StaticAudio::playSoundLoop(backgroundMusic);

        StaticInput::MouseTrack(GLFW_MOUSE_BUTTON_LEFT);
    }
    void render(float time = 0, bool updateDisplay = true)
    {
        // set shader to simple <- this is big
        StaticDraw::useShader(shaderSimpleRef);
        StaticDraw::backGroundImageRepeat(backgroundRef, 3.5);

        // draw buttons
        StaticDraw::multiDraw(buttonImageRef, batch); // batch is set in aspectChange()

        // render draw channels
        writer->startWrite();
        for (int i = 0; i < a; i++)
        {
            writer->drawChannel(i, glm::vec3(0.0f, 0.0f, 0.0f));
        }

        // make hovered button white
        if (buttonHover != -1 || buttonStart != -1)
        {
            StaticDraw::useShader(colorShaderRef); // this is just white.
            // 0 is used because it doesn't matter what texture is used here because colorShaderRef ignores texture.
            StaticDraw::multiDraw(0, hoverBatch); 
        }

        Scene::render(time, updateDisplay);
    };

    void handle(float time = 0)
    {
        processInput();
    }

    void aspectChange()
    {
        StaticDraw::updateView();

        glfwGetWindowSize(window, &winWidth, &winHeight);

        // load button batches
        batch = {};
        for (int i = 0; i < a; i++)
        {
            // directly adding vertices to batch instead of using helper functions because I wanted to reuse the y value for something else
            batch.insert(batch.end(),
                {
                .8f,  yCords[i * 2 + 1], 1.0f, 1.0f, // v0
                .8f,  yCords[i * 2], 1.0f, 0.0f, // v1
                -.8f,  yCords[i * 2], 0.0f, 0.0f, // v2

                .8f,  yCords[i * 2 + 1], 1.0f, 1.0f, // v0
                -.8f,  yCords[i * 2], 0.0f, 0.0f, // v2
                -.8f,  yCords[i * 2 + 1], 0.0f, 1.0f, // v3
                });
        }

        // load text // aspect ratio changes it
        float fontYScale = .25; // fontsize
        float fontXScale = fontYScale / StaticDraw::aspectRatio;
        for (int i = 0; i < a; i++)
        {
            // set up channel
            writer->setUpChannel(i);
            float h = b * (i*2-a) + b/2;
            if(i==0){ StaticWrite::AppendText(i, "Exit", -.5, h, fontXScale, fontYScale); }
            else if(i == 1){ StaticWrite::AppendText(i, "Options", -.5, h, fontXScale, fontYScale); }
            else if(i == 2){ StaticWrite::AppendText(i, "...", -.5, h, fontXScale, fontYScale); }
            else if(i == 3){ StaticWrite::AppendText(i, "Frog Hop", -.5, h, fontXScale, fontYScale); }
        }
    }

    void processInput()
    {
        Scene::processInput();

        // Tick is required to check for inputs
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

    // helper function to clean up processInput
    void updateMouseHover()
    {
        //mouse vars
        double mouseX, mouseY;
        StaticInput::GetMouse(mouseX, mouseY);

        // All buttons have same width. Check if mouse is in that range.
        if (mouseX < -0.8 || mouseX > 0.8)
        {
            buttonHover = -1;
            return;
        }
        // Check if mouse is over button y cords
        for (int i = 0; i < a; i++)
        {
            if (mouseY < yCords[i * 2])
            {
                buttonHover = -1;
                return;
            }
            // if passes previous return check and below button top, it is within y range of current button
            if (mouseY < yCords[i * 2 + 1])
            {
                // update current button if it is new and another button isn't held down
                if (buttonHover != i && buttonStart == -1)
                {
                    buttonHover = i;
                    StaticAudio::playSoundEffectMulti(bwoo);
                    hoverBatch.clear();
                    hoverBatch.insert(hoverBatch.end(), {
                        .8f,  yCords[i * 2 + 1], 1.0f, 1.0f,
                        .8f,  yCords[i * 2],     1.0f, 0.0f,
                        -.8f, yCords[i * 2],     0.0f, 0.0f,

                        .8f,  yCords[i * 2 + 1], 1.0f, 1.0f,
                        -.8f, yCords[i * 2],     0.0f, 0.0f,
                        -.8f, yCords[i * 2 + 1], 0.0f, 1.0f
                        });
                }
                return;
            }
        }
        buttonHover = -1;
    }

    void buttonPress(int x)
    {
        if(x==0){ glfwSetWindowShouldClose(window, true); }
        else if (x==1)
        {
            SoundOptions* so = new SoundOptions();
            so->previous = this;
            DataHolder::SceneQueue(so, false);
        }
        //else if(x==2){ DataHolder::changeSceneTwo(new mainMenu()); }
        else if (x == 3) { DataHolder::SceneQueue(new FrogHop()); }
    }

    void clean()
    {
        // Turn off music
        StaticAudio::stopSound(backgroundMusic);
    }; 
};