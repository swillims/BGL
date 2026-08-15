#pragma once

#include "scene/scene.h"
#include "uiHelper.h"
#include "singleton/staticInput.h"
#include "singleton/dataHolder.h"
#include "singleton/staticSound.h"
#include "singleton/staticDraw.h"

/*
 * -> IMPORTANT <-
 * The options menus were written while I was still learning and are below standard quality.
 * Do not learn from these.
 * For better examples of how to write UI, go csv writer - https://github.com/swillims/csv-writer
 */

// code may be wrong due to copy pasting from other scene ¯\_(ツ)_/¯
struct WalkerKeyOptions : Scene
{
private:
    enum uiKeys
    {
        uiExit,

        uiQ,
        uiW,
        uiE,
        uiA,
        uiS,
        uiD,
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
    std::string aTitle;
    std::string aValue;
    std::string sTitle;
    std::string sValue;
    std::string dTitle;
    std::string dValue;
    std::string escTitle;
    std::string escValue;

    std::string exitText;
    std::string saveText;

    // scene for backtracking and render
    Scene* previous;

    // -1, -1 is bottom left cornor for draw start and -1 to 1 scale has width and height of 2
    WalkerKeyOptions() : ui(1.0) {}

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

        keyTitle = "Walker 3D Key Bindings";

        qTitle = "3dLRotate";
        wTitle = "3dUp";
        eTitle = "3dRRotate";
        aTitle = "3dLeft";
        sTitle = "3dDown";
        dTitle = "3dRight";

        escTitle = "3dMenu ";

        if (!StaticInput::HasAlias<std::string>(qTitle)){StaticInput::AssignAlias(qTitle,'Q');}
        if (!StaticInput::HasAlias<std::string>(wTitle)){StaticInput::AssignAlias(wTitle,'W');}
        if (!StaticInput::HasAlias<std::string>(eTitle)){StaticInput::AssignAlias(eTitle,'E');}
        if (!StaticInput::HasAlias<std::string>(aTitle)){StaticInput::AssignAlias(aTitle,'A');}
        if (!StaticInput::HasAlias<std::string>(sTitle)){StaticInput::AssignAlias(sTitle,'S');}
        if (!StaticInput::HasAlias<std::string>(dTitle)){StaticInput::AssignAlias(dTitle,'D');}
        if (!StaticInput::HasAlias<std::string>(escTitle)){StaticInput::AssignAlias(escTitle,"ESCAPE");}

        qValue = StaticInput::GetStringAlias(qTitle);
        wValue = StaticInput::GetStringAlias(wTitle);
        eValue = StaticInput::GetStringAlias(eTitle);
        aValue = StaticInput::GetStringAlias(aTitle);
        sValue = StaticInput::GetStringAlias(sTitle);
        dValue = StaticInput::GetStringAlias(dTitle);
        escValue = StaticInput::GetStringAlias(escTitle);

        exitText = "Exit Settings";
        saveText = "Save Key Bindings";

        // This scene changes scenes without deleting itself when keybinding.
        // - because it navigates back to itself without deleting, it has to either clear the nodes or not add new nodes if they already exist
        // -- deleting the old nodes is better for readability than a massive if statement
        ui.nodes.clear();

        ui.appendType<UIYHolder>();
        ui[0].appendType<UIBuffer>(.1)
            .appendType<UIXSplits>(std::vector<float>{.2,.6,.2})
                .appendType<UIStack>().appendType<UIXRatio>(2)
                        .appendType<TexUVNode>(0, .25, 0, .5,uiGraphicsSettings).back()
                    .back()
                .back()
            .appendType<UITextOneLine>(-111, keyTitle,.25).back()
            .appendType<UIStack>().appendType<UIXRatio>(2).appendType<TexUVNode>(.75, 1, 0, .5,uiSoundSettings);

        ui[0].appendType<UIXHolder>()
            .appendType<UIXHolder>()
                .appendType<UITextOneLine>(-111, qTitle,.15).back()
                .appendType<UIXRatio>(1)
                    .appendType<UIStack>()
                        .appendType<TexUVNode>(0, 1, .5, 1,uiQ).back()
                        .appendType<UITextOneLine>(-111, qValue,.15).back()
                        .back()
                    .back()
                .back()
            .appendType<UIXHolder>()
                .appendType<UITextOneLine>(-111, wTitle,.15).back()
                .appendType<UIXRatio>(1)
                    .appendType<UIStack>()
                        .appendType<TexUVNode>(0, 1, .5, 1,uiW).back()
                        .appendType<UITextOneLine>(-111, wValue,.15).back()
                        .back()
                    .back()
                .back()
            .appendType<UIXHolder>()
                .appendType<UITextOneLine>(-111, eTitle,.15).back()
                .appendType<UIXRatio>(1)
                    .appendType<UIStack>()
                        .appendType<TexUVNode>(0, 1, .5, 1,uiE).back()
                        .appendType<UITextOneLine>(-111, eValue,.15).back()
                        .back()
                    .back()
                .back()
        ;
        ui[0].appendType<UIXHolder>()
            .appendType<UIXHolder>()
                .appendType<UITextOneLine>(-111, aTitle,.15).back()
                .appendType<UIXRatio>(1)
                    .appendType<UIStack>()
                        .appendType<TexUVNode>(0, 1, .5, 1,uiA).back()
                        .appendType<UITextOneLine>(-111, aValue,.15).back()
                        .back()
                    .back()
                .back()
            .appendType<UIXHolder>()
                .appendType<UITextOneLine>(-111, sTitle,.15).back()
                .appendType<UIXRatio>(1)
                    .appendType<UIStack>()
                        .appendType<TexUVNode>(0, 1, .5, 1,uiS).back()
                        .appendType<UITextOneLine>(-111, sValue,.15).back()
                        .back()
                    .back()
                .back()
            .appendType<UIXHolder>()
                .appendType<UITextOneLine>(-111, dTitle,.15).back()
                .appendType<UIXRatio>(1)
                    .appendType<UIStack>()
                        .appendType<TexUVNode>(0, 1, .5, 1,uiD).back()
                        .appendType<UITextOneLine>(-111, dValue,.15).back()
                        .back()
                    .back()
                .back()
        ;
        ui[0].appendType<UIXHolder>()
            .appendType<UIEmpty>().back()
            .appendType<UIXHolder>()
                .appendType<UITextOneLine>(-111, escTitle,.15).back()
                .appendType<UIXRatio>(1)
                    .appendType<UIStack>()
                        .appendType<TexUVNode>(0, 1, .5, 1,uiEsc).back()
                        .appendType<UITextOneLine>(-111, escValue,.15).back()
                        .back()
                    .back()
                .back()
            .appendType<UIEmpty>()
        ;

        ui[0].appendType<UIXHolder>()
            .appendType<UIStack>().appendType<UIXRatio>(2).appendType<TexUVNode>(0,1,.5,1,uiExit).back()
            .appendType<UITextOneLine>(-111, exitText, .2, XCENTER).back().back().back()
            .appendType<UIStack>().appendType<UIXRatio>(2).appendType<TexUVNode>(0,1,.5,1,uiSave).back()
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

        std::string settingsFileName = "metadata/keybind3dsettings";
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
