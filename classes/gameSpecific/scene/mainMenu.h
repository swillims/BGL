#pragma once
// libs/tools
#include <GLFW/glfw3.h>
#include "singleton/staticDraw.h"
#include "singleton/staticSound.h"
#include "singleton/staticWrite.h"
#include "singleton/staticInput.h"

// scenes
#include "frogHop.h"
#include "optionsScreen.h"

/*
    ---> IMPORTANT - READ THIS <---
    If you make your own menus, this scene is an ok example of how to make one.
    - "onLoad" functions loads all things used by this scene, and sets up other init tasks.
    - "aspectChange" runs whenever aspect ratio changes. It should be called by onLoad because it sets up several init things as well.
    - "handle" runs once per frame or PhysicsFrameRate. It should be used to handle/track any game/menu logic
    - "render" runs once per frame or GraphicFrameRate. It should be used to Render anything that needs to be drawn using the "Static" libs the engine provides.
    - "clean" should be used when cleaning up a scene. It should unload things but this specific scene doesn't

    The cleanest way to make a starting menu for a new game is to modify this scene.
    - Make a class that inherits from the scene class and then replace this classes scenes with that one.
    - "render" has a variable named a. Change it specify the amount of buttons.
    - If the menu changes, make sure to change the text <- text is updated at aspectChange()
    - If the menu changes, make sure to change the code in buttonPress() to change what happens when a button is pressed.
    - If a modified version of this scene does not use a resource, remove the code to load it.
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
    unsigned int buttonHover = -1; // technically wraps to a large number and is not negative
    unsigned int buttonStart = -1;
    bool click;
    bool loadAntiClick;

    // writer
    StaticWrite* writer;

    // math I don't remember
    int a = 4; // <-- change this number to get a different number of buttons.
    float b = 1.0f / a;
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

        //StaticAudio::applyTags(bwoo, { "soundEffect" });
        StaticAudio::updateSounds();

        // declared on StaticDraw Init
        shaderSimpleRef = StaticDraw::getShader("simple");

        // set up write
        writer = StaticWrite::singleton; // this is going to be reused a lot so better to just save the ref instead of looking up every time
        writer->destroyChannels(); // reset channels
        
        // do yCords for menu button here instead of aspect change because they do not change when ratio changes
        yCords = {};
        int aa = a + 1;
        float af = a;
        // I had to brute force this. I don't know correct math for doing this.
        // - It's ok to be slightly not efficient here because it is in onLoad and not handle/render
        float bfh = buttonHeightExtra;
        for (float i = 1.5f; i < aa * 2 - 1; i++) 
        {
            bfh *= -1; // this is a hack
            yCords.push_back((i - aa) / af + bfh);
        }
        
        aspectChange(); // aspect change hangles text regeneration and generation
        
        // stop autoclick on menu load
        loadAntiClick = true;

        // play background music
        StaticAudio::playSoundLoop(backgroundMusic);

        //StaticInput::Track(GLFW_MOUSE_BUTTON_LEFT);
        StaticInput::MouseTrack(GLFW_MOUSE_BUTTON_LEFT);
    }
    void render(float time = 0, bool updateDisplay = true)
    {
        //std::cout << "keyBoard" << glfwGetKey(window, GLFW_MOUSE_BUTTON_LEFT) << "\n";
        //std::cout << "mouseMethod" << glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) << "\n";
        
        // clear previous render
        //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        //glClear(GL_COLOR_BUFFER_BIT);

        // set shader to simple <- this is big
        StaticDraw::useShader(shaderSimpleRef);
        StaticDraw::backGroundImageRepeat(backgroundRef, 3.5);

        // draw buttons
        StaticDraw::multiDraw(buttonImageRef, batch); // batch is set in aspectChange()

        // render draw channels
        writer->startWrite(); // needed to start writing
        for (int i = 0; i < a; i++)
        {
            writer->drawChannel(i, glm::vec3(0.0f, 0.0f, 0.0f)); // channels are set in aspectChange()
        }

        // make hovered button white
        if (buttonHover != -1 || buttonStart != -1)
        {
            StaticDraw::useShader(colorShaderRef); // this is just white.
            // 0 is used because it doesn't matter what texture is used here because colorShaderRef ignores texture.
            // hover batch is set when handle detects a button is hovered
            StaticDraw::multiDraw(0, hoverBatch); 
        }

        // call super
        Scene::render(time, updateDisplay);
    };

    void handle(float time = 0)
    {
        // this scene does not do anything outside of processing user input
        processInput();
    }

    // aspectChange() is an inherited scene method that runs every time aspect ratio changes
    void aspectChange()
    {
        StaticDraw::updateView(); // need for proper aspect ratio update
        
        // update window - needed for mouse
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
            float h = b * (i*2-a) + b/2; // I don't remember this math but it works
            if(i==0){ StaticWrite::AppendText(i, "Exit", -.5, h, fontXScale, fontYScale); }
            else if(i == 1){ StaticWrite::AppendText(i, "Options", -.5, h, fontXScale, fontYScale); }
            else if(i == 2){ StaticWrite::AppendText(i, "...", -.5, h, fontXScale, fontYScale); }
            else if(i == 3){ StaticWrite::AppendText(i, "Frog Hop", -.5, h, fontXScale, fontYScale); }
        }
    }

    void processInput()
    {
        // call super
        // unnessary because window is already bound in onload and all super does is specify window
        // done as a demo because the posted version of this is a demo of game with the engine
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
        //glfwGetCursorPos(window, &mouseX, &mouseY);
        StaticInput::GetMouse(mouseX, mouseY);
        // convert mouse vars from window size to -1.0 to 1.0 engine and openGl readable cords
        //mouseX = (mouseX / winWidth) * 2.0 - 1.0;
        //mouseY = -((mouseY / winHeight) * 2.0 - 1.0);

        // All buttons have same width. Check if mouse is in that range.
        if (mouseX < -0.8 || mouseX > 0.8)
        {
            buttonHover = -1;
            return;
        }
        // Check if mouse is over button y cords
        for (int i = 0; i < a; i++)
        {
            // i starts at lowest button
            // if mouseY is lower than current button's bottom cord return because not on a button
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
                    // using batching because it simplifies the problem
                    // there is a complicated solution that is easy read using StaticDraw::halfDimImage
                    // and a solution that uses batching but looks more complicated than it is
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
        // logically it should be above top button here
        buttonHover = -1;
    }

    void buttonPress(int x)
    {
        if(x==0){ glfwSetWindowShouldClose(window, true); }
        else if (x==1)
        {
            OptionsScreen* os = new OptionsScreen();
            os->previous = this;
            DataHolder::SceneQueue(os, false);
        }
        //else if(x==2){ DataHolder::changeSceneTwo(new mainMenu()); }
        else if (x == 3) { DataHolder::SceneQueue(new FrogHop()); }
    }

    void clean() // unload assets and turn off music
    {
        // Turn off music
        StaticAudio::stopSound(backgroundMusic);
        // As an alternative to stopping the music, it can also be unloaded.
        //StaticAudio::unLoad(backgroundMusic);
        // unloading also stops the sound

    }; 
};