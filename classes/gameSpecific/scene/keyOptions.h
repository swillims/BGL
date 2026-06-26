#pragma once

#include "scene/scene.h"
#include "uiHelper.h"
#include "singleton/staticInput.h"
#include "singleton/dataHolder.h"
#include "singleton/staticSound.h"
#include "singleton/staticDraw.h"

struct KeyOptions : Scene
{
private:
    enum uiKeys
    {
        uiExit,

        uiQ,
        uiW,
        uiE,
        uiEsc,

        uiResetKeys,

        uiSave,
        uiGraphicsSettings,
        uiSoundSettings
    };

public:
    // textures
    unsigned int uITex;
    // shaders
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
    std::string keyTitle;

    std::string qTitle;
    std::string qValue;
    std::string wTitle;
    std::string wValue;
    std::string eTitle;
    std::string eValue;
    std::string escTitle;
    std::string escValue;

    std::string exitText;
    std::string saveText;

    // scene for backtracking and render
    Scene* previous;

    // -1, -1 is bottom left cornor for draw start and -1 to 1 scale has width and height of 2
    KeyOptions() : ui(-1, -1, 2, 2, 1.0, true) {}

    void onLoad() override
    {
        Scene::onLoad();

        // textures
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
        StaticDraw::useShader(colorShaderRef); // selecting shader is needed to modify shader in opengl
        GLint colorLoc = glGetUniformLocation(colorShaderRef, "color"); // get uniform location. Uniforms are shader vars
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 0.8f);

        // sounds
        if (!StaticAudio::soundStringRefs.contains("menuBloo.wav"))
        {
            StaticAudio::load("assets/core/menuBloo.wav", "menuBloo.wav", { "soundEffect" });
        }
        bwoo = StaticAudio::soundStringRefs["menuBloo.wav"];

        int fps = DataHolder::god.frameCapInt;

        // declared on StaticDraw Init
        shaderSimpleRef = StaticDraw::getShader("simple");

        keyTitle = "Key Bindings";

        qTitle = "<-Rotate";
        wTitle = "Hop";
        eTitle = "Rotate->";
        escTitle = "Menu";

        if (!StaticInput::HasAlias<std::string>(qTitle)){StaticInput::AssignAlias(qTitle,'Q');}
        if (!StaticInput::HasAlias<std::string>(wTitle)){StaticInput::AssignAlias(wTitle,'W');}
        if (!StaticInput::HasAlias<std::string>(eTitle)){StaticInput::AssignAlias(eTitle,'E');}
        if (!StaticInput::HasAlias<std::string>(escTitle)){StaticInput::AssignAlias(escTitle,"ESCAPE");}

        qValue = StaticInput::GetStringAlias(qTitle);
        wValue = StaticInput::GetStringAlias(wTitle);
        eValue = StaticInput::GetStringAlias(eTitle);
        escValue = StaticInput::GetStringAlias(escTitle);

        exitText = "Exit Settings";
        saveText = "Save Key Bindings";

        // This scene changes scenes without deleting itself when keybinding.
        // - because it navigates back to itself without deleting, it has to either clear the nodes or not add new nodes if they already exist
        // -- deleting the old nodes is better for readability than a massive if statement
        ui.nodes.clear();

        ui.appendType<UIYHolder>(5);
        ui[0].appendType<UIBuffer>(.1)
            .appendType<UIXSplits>(std::vector<float>{.2,.6,.2})
                .appendType<UIStack>().appendType<UIXRatio>(2, true)
                        .appendType<TexUVNode>(0, .25, 0, .5,uiGraphicsSettings).back()
                    .back()
            .back()
            .appendType<UITextOneLine>(-111, keyTitle,.35).back()
            .appendType<UIStack>().appendType<UIXRatio>(2, true).appendType<TexUVNode>(.75, 1, 0, .5,uiSoundSettings);

        ui[0].appendType<UIXHolder>()
            .appendType<UIXHolder>()
                .appendType<UITextOneLine>(-111, qTitle,.25).back()
                .appendType<UIXRatio>(1,true)
                    .appendType<UIStack>()
                        .appendType<TexUVNode>(0, 1, .5, 1,uiQ).back()
                        .appendType<UITextOneLine>(-111, qValue,.15)
            .back().back().back().back()
            .appendType<UIXHolder>()
                .appendType<UITextOneLine>(-111, eTitle,.25).back()
                .appendType<UIXRatio>(1,true)
                    .appendType<UIStack>()
                        .appendType<TexUVNode>(0, 1, .5, 1,uiE).back()
                        .appendType<UITextOneLine>(-111, eValue,.15);

        ui[0].appendType<UIXHolder>()
            .appendType<UIXHolder>()
                .appendType<UITextOneLine>(-111, wTitle,.25).back()
                .appendType<UIXRatio>(1,true)
                    .appendType<UIStack>()
                        .appendType<TexUVNode>(0, 1, .5, 1,uiW).back()
                        .appendType<UITextOneLine>(-111, wValue,.15).back()
            .back().back().back()
            .appendType<UIEmpty>();

        ui[0].appendType<UIXHolder>()
            .appendType<UIXHolder>()
                .appendType<UITextOneLine>(-111, escTitle,.25).back()
                .appendType<UIXRatio>(1,true)
                    .appendType<UIStack>()
                        .appendType<TexUVNode>(0, 1, .5, 1,uiEsc).back()
                        .appendType<UITextOneLine>(-111, escValue,.15).back()
                .back().back().back()
                .appendType<UIEmpty>();

        ui[0].appendType<UIXHolder>()
            .appendType<UIStack>().appendType<UIXRatio>(2, true).appendType<TexUVNode>(0,1,.5,1,uiExit).back()
            .appendType<UITextOneLine>(-111, exitText, .2, XCENTER).back().back().back()
            .appendType<UIStack>().appendType<UIXRatio>(2, true).appendType<TexUVNode>(0,1,.5,1,uiSave).back()
            .appendType<UITextOneLine>(-111, saveText, .2, XCENTER);

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
        //std::cout << "start search\n";

        processInput(window);
    }

    void aspectChange()
    {
        StaticDraw::updateView();

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

        std::string settingsFileName = "metadata/keybindsettings";
        std::ostringstream write;

        write << qTitle << ":" << qValue << "\n";
        write << eTitle << ":" << eValue << "\n";
        write << wTitle << ":" << wValue << "\n";
        write << escTitle << ":" << escValue;

        if(util::writeFile(settingsFileName, write.str()))
        {
            std::cout << "keybind settings saved\n";
        }
        else
        {
            std::cout << "fail to write keybind settings\n";
        }
    }
};