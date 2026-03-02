//#include "glue/textShell.h"
#include <array>
#include "scene/scene.h"
#include "mainMenu.h"

/*
    Most of this scene is poorly implemented. Just copy the scene transition logic in buttonPress().
*/

struct DefeatScreen : Scene
{
    // textures
    unsigned int buttonImageRef;

    // shaders
    unsigned int shaderSimpleRef;
    unsigned int colorShaderRef;
    unsigned int colorShaderRef2;
    //unsigned int blackFadeShadeRef;

    // sound;
    unsigned int bwoo;

    // click handling bools
    bool click;
    bool loadAntiClick;
    unsigned int buttonHover = -1; // technically wraps to a large number and is not negative
    unsigned int buttonStart = -1;

    // render stuffs
    int a = 2; // <-- change this number to get a different number of buttons.
    float buttonHeightExtra = -.05f;
    float textYShift;

    // batches
    std::vector<float> yCords;
    std::vector<float> batch;
    std::vector<float> hoverBatch;

    // window vars
    int winWidth, winHeight;

    // ref used to backtrack to previous scene
    Scene* previous;

    // DO NOT CALL BEFORE StaticDraw::Init
    void onLoad() override
    {
        // extremely important to call super at start of onLoad to declare window
        Scene::onLoad();

        if (!StaticDraw::imageFileRefs.contains("button.png"))
        {
            StaticDraw::loadImage("assets/core/button.png");
        }
        buttonImageRef = StaticDraw::imageFileRefs["button.png"];

        if (!StaticDraw::hasShader("colorShader"))
        {
            StaticDraw::compileShader("assets/shaders/simple.vs", "assets/shaders/color.fs", "colorShader");
        }
        colorShaderRef = StaticDraw::getShader("colorShader");
        // set color for color shader
        // -> very important <-
        // engine does not directly handle shader code, so learn how to do your own shaders
        StaticDraw::useShader(colorShaderRef); // selecting shader is needed to modify shader for some reason
        GLint colorLoc = glGetUniformLocation(colorShaderRef, "color"); // get uniform location. Uniforms are shader vars
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 0.5f);  // change shader unfiorm

        if (!StaticDraw::hasShader("colorRef2"))
        {
            StaticDraw::compileShader("assets/shaders/simple.vs", "assets/shaders/color.fs", "colorRef2");
        }
        colorShaderRef2 = StaticDraw::getShader("colorRef2");
        StaticDraw::useShader(colorShaderRef2); // selecting shader is needed to modify shader for some reason
        colorLoc = glGetUniformLocation(colorShaderRef2, "color"); // get uniform location. Uniforms are shader vars
        glUniform4f(colorLoc, 0.75f, 0.0f, 0.0f, 0.25f);

        if (!StaticAudio::soundStringRefs.contains("menuBloo.wav"))
        {
            StaticAudio::load("assets/core/menuBloo.wav", "menuBloo.wav", { "soundEffect" });
        }
        bwoo = StaticAudio::soundStringRefs["menuBloo.wav"];

        // update sounds should be ran after adding sounds, so that the volumes change
        StaticAudio::updateSounds();

        // declared on StaticDraw Init
        shaderSimpleRef = StaticDraw::getShader("simple");

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

        aspectChange();

        loadAntiClick = false;
    }
    void render(float time = 0, bool updateDisplay = true)
    {
        previous->render(0, false);
        
        StaticDraw::useShader(colorShaderRef2);
        StaticDraw::halfDimImage(0, 0, 0, 1.0f, 1.0f); // 0 as a texture ref is valid because shader doesn't use a texture
        StaticDraw::useShader(shaderSimpleRef);

        // draw buttons
        StaticDraw::multiDraw(buttonImageRef, batch); // batch is set in aspectChange()

        // render draw channels
        StaticWrite::StartWrite(); // needed to start writing
        StaticWrite::DrawChannel(-11, glm::vec3(0.0f, 0.0f, 0.0f)); // channels are set in aspectChange()

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
        processInput();
    }

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

        // math to prepare a variable Y axis center text inside of button
        if (yCords.size() >= 2)
        {
            textYShift = yCords[1] - yCords[0];
            textYShift -= fontYScale;
            textYShift /= 2;
        }

        StaticWrite::SetUpChannel(-11);
        for (int i = 0; i < a; i++)
        {
            // set up channel
            //float h = b * (i * 2 - a) + b / 2;
            
            float h = yCords[i * 2];
            // -11 magic number used for channel to avoid conflict with scene and avoid conflict with other things that are using negative to avoid conflict
            if (i == 0) { StaticWrite::AppendText(-11, "Exit", -.5, h + textYShift, fontXScale, fontYScale); }
            else if (i == 1) { StaticWrite::AppendText(-11, "Retry", -.5, h + textYShift, fontXScale, fontYScale); }
        }

        if (yCords.size() >= 2)
        {
            textYShift = yCords[1] - yCords[0];
        }
 
        previous->aspectChange();
    }

    void processInput()
    {
        Scene::processInput();

        updateMouseHover();

        if (!loadAntiClick)
        {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) // check mouse button
            {
                click = true;
                if (buttonStart == -1)
                {
                    buttonStart = buttonHover;
                }
            }
            else if (click) // mouse button not pressed + click bool==true means release
            {
                if (buttonStart == buttonHover)
                {
                    buttonPress(buttonHover);
                }
                click = false;
                buttonStart = -1;
            }
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS)
        {
            loadAntiClick = false;
            click = false;
        }
    }

    void updateMouseHover()
    {
        //mouse vars
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        // convert mouse vars from window size to -1.0 to 1.0 engine and openGl readable cords
        mouseX = (mouseX / winWidth) * 2.0 - 1.0;
        mouseY = -((mouseY / winHeight) * 2.0 - 1.0);

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
        if (x == 0)
        {
            // correct way to delete things
            DataHolder::DelayDelete(previous); // delayed deletion is better than instant deletion in situations where multiple scenes are rendered(like this one)
            previous->clean(); // clean needs to be ran before canging scenes
            DataHolder::SceneQueue(new MainMenu(), true);
        }
        else if (x == 1) { DataHolder::SceneQueue(previous, true); }
    }
    void clean()
    {
        // not deleting button asser because reused enough
        // not deleting other colorRef because it is reused enough
        if (StaticDraw::hasShader("colorRef2"))
        {
            StaticDraw::unLoadShader("colorRef2");
        }
    }
};