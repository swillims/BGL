//#include "glue/textShell.h"
#pragma once
#include <array>
#include "scene/scene.h"
#include "uiHelper.h"
#include "singleton/staticInput.h"
#include "singleton/dataHolder.h"
#include "singleton/staticSound.h"
#include "singleton/staticDraw.h"

struct GraphicsOptions : Scene
{
private:
    enum uiKeys
    {
        uiExit,

        uiX1,
        uiX2,
        uiX3,
        uiX4,

        uiY1,
        uiY2,
        uiY3,
        uiY4,

        uiX1d,
        uiX2d,
        uiX3d,
        uiX4d,

        uiY1d,
        uiY2d,
        uiY3d,
        uiY4d,

        uiFPS1,
        uiFPS2,
        uiFPS3,
        uiFPS4,

        uiFPS1d,
        uiFPS2d,
        uiFPS3d,
        uiFPS4d,

        uiSave,
        uiKeySettings,
        uiSoundSettings
    };

public:
    unsigned int uITex;
    unsigned int shaderSimpleRef;
    unsigned int colorShaderRef;

    // sound;
    unsigned int bwoo;

    // ui vars
    unsigned int buttonHover; // button being hovered
    double mouseCordX, mouseCordY; // mouse cords that need to be normalized

    std::vector<float> batch;
    // uiElement
    UIXRatio ui;//(0.f, 0.f, 1.f, 1.f);

    // uiTextSources
    std::string soundTitle;
    std::string xTitle;
    std::string yTitle;
    std::string fpsTitle;
    std::string strx1;
    std::string strx2;
    std::string strx3;
    std::string strx4;
    std::string stry1;
    std::string stry2;
    std::string stry3;
    std::string stry4;
    std::string strfps1;
    std::string strfps2;
    std::string strfps3;
    std::string strfps4;
    std::string exitText;
    std::string saveText;

    // uiVariables
    int screenX;
    int screenY;

    // scene for backtracking and render
    Scene* previous;

    // -1, -1 is bottom left cornor for draw start and -1 to 1 scale has width and height of 2
    GraphicsOptions() : ui(-1, -1, 2, 2, 1.0, true) {}

    // DO NOT CALL BEFORE StaticDraw::Init
    void onLoad() override
    {
        Scene::onLoad();

        if (!StaticDraw::imageFileRefs.contains("optionsUi.png"))
        {
            StaticDraw::loadImage("assets/core/optionsUi.png");
        }
        uITex = StaticDraw::imageFileRefs["optionsUi.png"];

        // shaders
        if (!StaticDraw::hasShader("colorRef"))
        {
            StaticDraw::compileShader("assets/shaders/simple.vs", "assets/shaders/color.fs", "colorRef");
        }
        colorShaderRef = StaticDraw::getShader("colorRef");
        // set color for color shader
        // -> very important <-
        // engine does not directly handle shader code, so learn how to do your own shaders
        StaticDraw::useShader(colorShaderRef); // selecting shader is needed to modify shader for smoe reason
        GLint colorLoc = glGetUniformLocation(colorShaderRef, "color"); // get uniform location. Uniforms are shader vars
        //glUniform4f(colorLoc, 0.0f, 0.0f, 0.0f, 0.5f);  // change shader unfiorm
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 0.8f);

        // sounds
        if (!StaticAudio::soundStringRefs.contains("menuBloo.wav"))
        {
            StaticAudio::load("assets/core/menuBloo.wav", "menuBloo.wav", { "soundEffect" });
        }
        bwoo = StaticAudio::soundStringRefs["menuBloo.wav"];

        screenX = StaticDraw::w;
        screenY = StaticDraw::h;

        int fps = DataHolder::god.frameCapInt;

        // declared on StaticDraw Init
        shaderSimpleRef = StaticDraw::getShader("simple");

        soundTitle = "Graphic Settings";

        int x1 = screenX % 10;
        int x2 = screenX / 10 % 10;
        int x3 = screenX / 100 % 10;
        int x4 = screenX / 1000 % 10;

        int y1 = screenY % 10;
        int y2 = screenY / 10 % 10;
        int y3 = screenY / 100 % 10;
        int y4 = screenY / 1000 % 10;

        strx1 = std::to_string(x1);
        strx2 = std::to_string(x2);
        strx3 = std::to_string(x3);
        strx4 = std::to_string(x4);

        stry1 = std::to_string(y1);
        stry2 = std::to_string(y2);
        stry3 = std::to_string(y3);
        stry4 = std::to_string(y4);

        strfps1 = std::to_string(fps % 10);
        strfps2 = std::to_string(fps / 10 % 10);
        strfps3 = std::to_string(fps / 100 % 10);
        strfps4 = std::to_string(fps / 1000 % 10);

        xTitle = "X Screen Size";
        yTitle = "Y Screen Size";
        fpsTitle = "FPS Cap";

        exitText = "Exit Settings";
        saveText = "Save Graphic Settings";

        int i = 0;

        ui.appendType<UIYHolder>(5);
        ui[0].appendType<UIBuffer>(.1)
            .appendType<UIXSplits>(std::vector<float>{.2,.6,.2})
                .appendType<UIStack>().appendType<UIXRatio>(2, true)
                    .appendType<TexUVNode>(0, .25, 0, .5,uiSoundSettings).back()
                .back()
            .back()

            .appendType<UITextOneLine>(-111, soundTitle,.35).back()

            .appendType<UIStack>().appendType<UIXRatio>(2, true).appendType<TexUVNode>(.75, 1, 0, .5,uiKeySettings);

        i++;
        ui[0].appendType<UIXHolder>()
            .appendType<UITextOneLine>(-111, xTitle,.35).back()
            .appendType<UIXHolder>()
                .appendType<UIYHolder>()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(.75, 1, 0, .5,uiX1).back().back()
                    .appendType<UITextOneLine>(-111, strx4,.35).back()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(0, .25, 0, .5,uiX1d).back().back()
                .back()
                .appendType<UIYHolder>()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(.75, 1, 0, .5,uiX2).back().back()
                    .appendType<UITextOneLine>(-111, strx3,.35).back()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(0, .25, 0, .5,uiX2d).back().back()
                .back()
                .appendType<UIYHolder>()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(.75, 1, 0, .5,uiX3).back().back()
                    .appendType<UITextOneLine>(-111, strx2,.35).back()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(0, .25, 0, .5,uiX3d).back().back()
                .back()
                .appendType<UIYHolder>()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(.75, 1, 0, .5,uiX4).back().back()
                    .appendType<UITextOneLine>(-111, strx1,.35).back()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(0, .25, 0, .5,uiX4d).back().back()
                .back();

        i++;
        ui[0].appendType<UIXHolder>()
            .appendType<UITextOneLine>(-111, yTitle,.35).back()
            .appendType<UIXHolder>()
                .appendType<UIYHolder>()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(.75, 1, 0, .5,uiY1).back().back()
                    .appendType<UITextOneLine>(-111, stry4,.35).back()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(0, .25, 0, .5,uiY1d).back().back()
                .back()
                .appendType<UIYHolder>()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(.75, 1, 0, .5,uiY2).back().back()
                    .appendType<UITextOneLine>(-111, stry3,.35).back()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(0, .25, 0, .5,uiY2d).back().back()
                .back()
                .appendType<UIYHolder>()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(.75, 1, 0, .5,uiY3).back().back()
                    .appendType<UITextOneLine>(-111, stry2,.35).back()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(0, .25, 0, .5,uiY3d).back().back()
                .back()
                .appendType<UIYHolder>()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(.75, 1, 0, .5,uiY4).back().back()
                    .appendType<UITextOneLine>(-111, stry1,.35).back()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(0, .25, 0, .5,uiY4d).back().back()
                .back();
                // The .back()s at the end of a chain do nothing. They are stylistically left in to look better.

        i++;
        ui[0].appendType<UIXHolder>()
            .appendType<UITextOneLine>(-111, fpsTitle,.35).back()
            .appendType<UIXHolder>()
                .appendType<UIYHolder>()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(.75, 1, 0, .5,uiFPS1).back().back()
                    .appendType<UITextOneLine>(-111, strfps4,.35).back()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(0, .25, 0, .5,uiFPS1d).back().back()
                .back()
                .appendType<UIYHolder>()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(.75, 1, 0, .5,uiFPS2).back().back()
                    .appendType<UITextOneLine>(-111, strfps3,.35).back()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(0, .25, 0, .5,uiFPS2d).back().back()
                .back()
                .appendType<UIYHolder>()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(.75, 1, 0, .5,uiFPS3).back().back()
                    .appendType<UITextOneLine>(-111, strfps2,.35).back()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(0, .25, 0, .5,uiFPS3d).back().back()
                .back()
                .appendType<UIYHolder>()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(.75, 1, 0, .5,uiFPS4).back().back()
                    .appendType<UITextOneLine>(-111, strfps1,.35).back()
                    .appendType<UIXRatio>(1,true).appendType<TexUVNode>(0, .25, 0, .5,uiFPS4d).back().back()
                .back();

        i++;
        ui[0].appendType<UIXHolder>()
            .appendType<UIStack>().appendType<UIXRatio>(2, true)
                    .appendType<TexUVNode>(0,1,.5,1,uiExit).back()
                    .appendType<UITextOneLine>(-111, exitText, .2, XCENTER).back()
                .back()
            .back()
            .appendType<UIStack>()
                .appendType<UIXRatio>(2, true)
                    .appendType<TexUVNode>(0,1,.5,1,uiSave).back()
                    .appendType<UITextOneLine>(-111, saveText, .2, XCENTER);

        // an earlier comment said that .back()s at the ends are stylistic. Look at how ugly ^^^ this ^^^ without them

        aspectChange();
    }
    void render(float time = 0, bool updateDisplay = true) override
    {
        // draw background scene
        previous->render(0, false);

        // draw background shade
        StaticDraw::useShader(colorShaderRef);
        StaticDraw::halfDimImage(0, 0, 0, 1.0f, 1.0f); // 0 as a texture ref is valid because shader doesn't use a texture

        // switch back to default shader
        StaticDraw::useShader(shaderSimpleRef);

        // draw ui Elements from batch
        StaticDraw::multiDraw(uITex, batch);

        // write text
        StaticWrite::StartWrite();
        StaticWrite::DrawChannel(-111, glm::vec3(0.0f, 0.0f, 0.0f));

        Scene::render(time, updateDisplay);
    };

    void handle(float time = 0) override
    {
        StaticInput::GetMouse(mouseCordX,mouseCordY);
        processInput(window);
    }

    void aspectChange()
    {
        StaticDraw::updateView();

        screenX = StaticDraw::w;
        screenY = StaticDraw::h;
        int fps = DataHolder::god.frameCapInt;

        int x1 = screenX % 10;
        int x2 = screenX / 10 % 10;
        int x3 = screenX / 100 % 10;
        int x4 = screenX / 1000 % 10;

        int y1 = screenY % 10;
        int y2 = screenY / 10 % 10;
        int y3 = screenY / 100 % 10;
        int y4 = screenY / 1000 % 10;

        strx1 = std::to_string(x1);
        strx2 = std::to_string(x2);
        strx3 = std::to_string(x3);
        strx4 = std::to_string(x4);

        stry1 = std::to_string(y1);
        stry2 = std::to_string(y2);
        stry3 = std::to_string(y3);
        stry4 = std::to_string(y4);

        strfps1 = std::to_string(fps % 10);
        strfps2 = std::to_string(fps / 10 % 10);
        strfps3 = std::to_string(fps / 100 % 10);
        strfps4 = std::to_string(fps / 1000 % 10);

        batch.clear();
        // channel -111 used to avoid conflict. Underflow makes it an absurdly large number
        StaticWrite::SetUpChannel(-111);
        ui.adjustNodeDefault();
        ui.renderVerts(batch);

        previous->aspectChange();
    }

    void processInput(GLFWwindow* window)
    {
        StaticInput::Tick();
        if (StaticInput::MouseClick(GLFW_MOUSE_BUTTON_LEFT))
        {
            buttonHover = ui.findOneHover(mouseCordX, mouseCordY);

            buttonPress(buttonHover);
        }
    }

    void buttonPress(int x);

    void saveSetting()
    {
        StaticAudio::playSoundEffectMulti(bwoo);

        std::string settingsFileName = "metadata/graphicsettings";
        std::ostringstream write;

        write << "SCR_WIDTH:" << StaticDraw::w << "\n";
        write << "SCR_HEIGHT:" << StaticDraw::h << "\n";
        write << "FPS_CAP:" << DataHolder::god.frameCapInt;

        if(util::writeFile(settingsFileName, write.str()))
        {
            std::cout << "sound settings saved\n";
        }
        else
        {
            std::cout << "fail to write graphics settings\n";
        }
    }
};