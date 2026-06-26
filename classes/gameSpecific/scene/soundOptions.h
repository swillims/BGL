#pragma once
#include "scene/scene.h"
#include "uiHelper.h"
#include "singleton/staticInput.h"

#include "graphicOptions.h"
#include "keyOptions.h"

struct SoundOptions : Scene
{
private:
    enum uiKeys
    {
        uiExit,
        uiMasterVollume,
        uiMusicVollume,
        uiEffectVollume,
        uiSave,

        uiGraphicSettings,
        uiKeySettings
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
    std::string graphicTitle;
    std::string keySettings;
    std::string soundTitle;
    std::string masterTitle;
    std::string masterValue;
    std::string musicTitle;
    std::string musicValue;
    std::string soundEffectTitle;
    std::string soundEffectValue;
    std::string exitText;
    std::string saveText;

    // uiVariables
    float masterVollumeLeft;
    float masterVollumeWidth;
    float musicVollumeLeft;
    float musicVollumeWidth;
    float effectVollumeLeft;
    float effectVollumeWidth;

    // settingsVariables
    float masterVollume;
    float musicVollume;
    float effectVollume;

    // scene for backtracking and render
    Scene* previous;

    // constructor only used to instantiate a ui
    // -1, -1 is bottom left cornor for draw start and -1 to 1 scale has width and height of 2
    SoundOptions() : ui(-1, -1, 2, 2, 1.0, true) {}

    // DO NOT CALL BEFORE StaticDraw::Init
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
        // -> very important <-
        // engine does not directly handle shader code, so learn how to do your own shaders
        StaticDraw::useShader(colorShaderRef); // selecting shader is needed to modify shader for smoe reason
        GLint colorLoc = glGetUniformLocation(colorShaderRef, "color"); // get uniform location. Uniforms are shader vars
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 0.8f);

        // sounds
        if (!StaticAudio::soundStringRefs.contains("menuBloo.wav"))
        {
            StaticAudio::load("assets/core/menuBloo.wav", "menuBloo.wav", { "soundEffect" });
        }
        bwoo = StaticAudio::soundStringRefs["menuBloo.wav"];

        StaticAudio::updateSounds();

        // declared on StaticDraw Init
        shaderSimpleRef = StaticDraw::getShader("simple");

        graphicTitle = "Graphic Settings";
        keySettings = "Key Bindings";
        soundTitle = "Sound Settings";
        masterTitle = "Master Volume";
        masterVollumeLeft = StaticAudio::masterVolume;
        masterValue = std::to_string(static_cast<int>(masterVollumeLeft * 100.0f + 0.5f));
        masterVollumeWidth = .1;
        musicTitle = "Music Volume";
        musicValue = "na";
        musicVollumeLeft = .0;
        musicVollumeWidth = .1;
        soundEffectTitle = "Sound Effects Volume";
        soundEffectValue = "na";
        effectVollumeLeft = .0;
        effectVollumeWidth = .1;
        exitText = "Exit Settings";
        saveText = "Save Sound Settings";

        if (StaticAudio::tagStringRefs.contains("music"))
        {
            musicVollumeLeft = StaticAudio::tagSettings[StaticAudio::tagStringRefs["music"]];
            musicValue = std::to_string(static_cast<int>(musicVollumeLeft * 100.0f + 0.5f));
        }
        if (StaticAudio::tagStringRefs.contains("soundEffect"))
        {
            effectVollumeLeft = StaticAudio::tagSettings[StaticAudio::tagStringRefs["soundEffect"]];
            soundEffectValue = std::to_string(static_cast<int>(effectVollumeLeft * 100.0f + 0.5f));
        }

        // exists to not have a ref not initialized
        // minimum decalration too large to keep code clean so ref needs to be declared up here
        // - but it had nothing to point to, so currentTarget was added
        //std::vector<std::unique_ptr<UIElement>> currentTarget;  
        //std::vector<std::unique_ptr<UIElement>>& ct = currentTarget;

        // using an i instead of hard coding ints to make it more scalable when modifying
        int i = 0;

        ui.appendType<UIYHolder>(5);
        ui[0].appendType<UIBuffer>(.1)
            .appendType<UIXSplits>(std::vector<float>{.2,.6,.2})
                .appendType<UIStack>()
                    .appendType<UIXRatio>(2, true)
                        .appendType<TexUVNode>(0, .25, 0, .5,uiKeySettings).back()
                    .back()
                .back()
                .appendType<UITextOneLine>(-111, soundTitle,.35).back()
                .appendType<UIStack>().appendType<UIXRatio>(2, true).appendType<TexUVNode>(.75, 1, 0, .5,uiGraphicSettings).back()
            ;

        i++;
        ui[0].appendType<UIXSplits>(std::vector<float>{ .25f, .6f, .15f }, -1)
            .appendType<UIBuffer>(.1)
                .appendType<UITextOneLine>(-111, masterTitle, .2, XRIGHT);

        // bar
        std::vector<std::unique_ptr<UIElement>>& ct =
        ui[0][i].appendType<UIStack>().setKey(uiMasterVollume)
            .appendType<UIXHolder>()
                .appendSameType<UIXRatio>(10, 1.0, true);
        for (auto& nodePtr : ct) 
        {
            UIElement& node = *nodePtr;
            node.appendType<TexUVNode>(.25, .75, 0, .5);
        }
        ui[0][i].appendType<UIBuffer>(.1)
            .appendType<UITextOneLine>(-111, masterValue, .2, XLEFT)
            .back().back()[1]
            .appendType<UIXShifter>(masterVollumeLeft, masterVollumeWidth)
            .appendType<UIXRatio> (1.0)
            .appendType<TexUVNode>(.75, 1, 0, .5).setKey(11);

        i++;
        ui[0].appendType<UIXSplits>(std::vector<float>{ .25f, .6f, .15f }, -1)
            .appendType<UIBuffer>(.1)
            .appendType<UITextOneLine>(-111, musicTitle, .2, XRIGHT);
        std::vector<std::unique_ptr<UIElement>>& ct2 =
            ui[0][i].appendType<UIStack>().setKey(uiMusicVollume)
            .appendType<UIXHolder>()
            .appendSameType<UIXRatio>(10, 1.0, true);
        for (auto& nodePtr : ct2)
        {
            UIElement& node = *nodePtr;
            node.appendType<TexUVNode>(.25, .75, 0, .5);
        }
        ui[0][i].appendType<UIBuffer>(.1)
            .appendType<UITextOneLine>(-111, musicValue, .2, XLEFT)
            .back().back()[1]
            .appendType<UIXShifter>(musicVollumeLeft, musicVollumeWidth)
            .appendType<UIXRatio>(1.0)
            .appendType<TexUVNode>(.75, 1, 0, .5).setKey(12);

        i++;
        ui[0].appendType<UIXSplits>(std::vector<float>{ .25f, .6f, .15f }, -1)
            .appendType<UIBuffer>(.1)
            .appendType<UITextOneLine>(-111, soundEffectTitle, .2, XRIGHT);
        std::vector<std::unique_ptr<UIElement>>& ct3 =
            ui[0][i].appendType<UIStack>().setKey(3) // key
            .appendType<UIXHolder>()
            .appendSameType<UIXRatio>(10, 1.0, true);
        for (auto& nodePtr : ct3)
        {
            UIElement& node = *nodePtr;
            node.appendType<TexUVNode>(.25, .75, 0, .5);
        }
        ui[0][i].appendType<UIBuffer>(.1)
            .appendType<UITextOneLine>(-111, soundEffectValue, .2, XLEFT)
            .back().back()[1]
            .appendType<UIXShifter>(effectVollumeLeft, effectVollumeWidth)
            .appendType<UIXRatio>(1.0)
            .appendType<TexUVNode>(.75, 1, 0, .5).setKey(13);

        i++;
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
        
        processInput(window);
    }

    void aspectChange()
    {
        StaticDraw::updateView(); // need for proper aspect ratio update
        batch.clear();
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
        else if (StaticInput::MouseHeld(GLFW_MOUSE_BUTTON_LEFT))
        {
            // enums reference sliders
            if (buttonHover == uiMasterVollume || buttonHover == uiMusicVollume || buttonHover == uiEffectVollume)
            {
                int newHover = ui.findOneHover(mouseCordX, mouseCordY);
                if (newHover != buttonHover){ buttonHover = -1;}
                else{buttonPress(buttonHover);}
            }
        }
    }

    void buttonPress(int x)
    {
        if (x == uiExit)
        {
            DataHolder::SceneQueue(previous, true);
        }
        else if (x == uiSave)
        {
            saveSetting();
        }
        else if (x == uiMasterVollume)
        {
            UIElement& barHolder = ui.findByKey(uiMasterVollume);
            if (barHolder.key == uiMasterVollume)
            {
                masterVollume = ((mouseCordX - barHolder.xMin) - (masterVollumeWidth * 0.5f)) / (barHolder.xSize - masterVollumeWidth);
                if (masterVollume < 0) { masterVollume = 0; }
                else if (masterVollume > 1) { masterVollume =1; }
                masterValue = std::to_string(static_cast<int>(masterVollume * 100.0f + 0.5f));

                masterVollumeLeft = masterVollume / (1.0 + masterVollumeWidth);

                StaticAudio::setMasterVollume(masterVollume);

                aspectChange();
            }
        }
        else if (x == uiMusicVollume)
        {
            UIElement& barHolder = ui.findByKey(uiMusicVollume);
            if (barHolder.key == uiMusicVollume)
            {
                musicVollume = ((mouseCordX - barHolder.xMin) - (musicVollumeWidth * 0.5f)) / (barHolder.xSize - musicVollumeWidth);
                if (musicVollume < 0) { musicVollume = 0; }
                else if (musicVollume > 1) { musicVollume = 1; }
                musicValue = std::to_string(static_cast<int>(musicVollume * 100.0f + 0.5f));

                musicVollumeLeft = musicVollume / (1.0 + musicVollumeWidth);

                //StaticAudio::setMasterVollume(musicVollume);
                StaticAudio::updateTagVollume("music", musicVollume);
                StaticAudio::updateSounds();

                aspectChange();
            }
        }
        else if (x == uiEffectVollume)
        {
            UIElement& barHolder = ui.findByKey(uiEffectVollume);
            if (barHolder.key == uiEffectVollume)
            {
                effectVollume = ((mouseCordX - barHolder.xMin) - (effectVollumeWidth * 0.5f)) / (barHolder.xSize - effectVollumeWidth);
                if (effectVollume < 0) { effectVollume = 0; }
                else if (effectVollume > 1) { effectVollume = 1; }
                soundEffectValue = std::to_string(static_cast<int>(effectVollume * 100.0f + 0.5f));

                effectVollumeLeft = effectVollume / (1.0 + effectVollumeWidth);

                StaticAudio::updateTagVollume("soundEffect", effectVollume);
                StaticAudio::updateSounds();

                StaticAudio::playSoundEffect(bwoo);

                aspectChange();            }
        }
        else if (x == uiGraphicSettings)
        {
            GraphicsOptions* graphics = new GraphicsOptions();
            graphics->previous = previous;
            DataHolder::SceneQueue(graphics, true);
        }
        else if ( x == uiKeySettings)
        {
            KeyOptions* keys = new KeyOptions();
            keys->previous = previous;
            DataHolder::SceneQueue(keys, true);
        }
    }

    void saveSetting()
    {
        StaticAudio::playSoundEffectMulti(bwoo);
        std::string settingsFileName = "metadata/soundsettings";
        std::ostringstream write;

        write << "SOUND_MASTER:" << static_cast<int>(masterVollume * 100) << "\n";
        write << "SOUND_MUSIC:" << static_cast<int>(musicVollume * 100) << "\n";
        write << "SOUND_EFFECT:" << static_cast<int>(effectVollume * 100);

        if(util::writeFile(settingsFileName, write.str()))
        {
            std::cout << "sound settings saved\n";
        }
        else
        {
            std::cout << "fail to write sound settings\n";
        }
    }
};
