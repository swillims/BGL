//#include "glue/textShell.h"
#include <array>
#include "scene/scene.h"
#include "uiHelper.h"
#include "singleton/staticInput.h"
//#include "mainMenu.h"

struct OptionsScreen : Scene
{
    // textures
    unsigned int buttonImageRef;
    unsigned int uITex;
    // shaders
    unsigned int shaderSimpleRef;
    //unsigned int shaderWhiteShadeRef;
    unsigned int colorShaderRef;
    //unsigned int blackFadeShadeRef;

    // sound;
    unsigned int bwoo;

    // ui vars
    unsigned int buttonHover; // button being hovered
    double mouseCordX, mouseCordY; // mouse cords that need to be normalized

    // there are only two button so I'm not using a for loop and I can get away with two hoverBatchs
    std::vector<float> batch;
    //std::vector<float> hoverBatchA;
    //std::vector<float> hoverBatchB;

    // uiElementList
    //std::vector<std::unique_ptr<uiElement>> elements;

    // uiElement
    UIXRatio ui;//(0.f, 0.f, 1.f, 1.f);

    // uiTextSources
    std::string soundTitle;
    std::string masterTitle;
    std::string masterValue;
    std::string musicTitle;
    std::string musicValue;
    std::string soundEffectTitle;
    std::string soundEffectValue;

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
    OptionsScreen() : ui(-1, -1, 2, 2, 1.0, true) {}

    // DO NOT CALL BEFORE StaticDraw::Init
    void onLoad() override
    {
        //myButtonTexture.load("assets/core/button.png");
        //window = glfwGetCurrentContext();
        Scene::onLoad();

        // textures
        if (!StaticDraw::imageFileRefs.contains("button.png"))
        {
            StaticDraw::loadImage("assets/core/button.png");
        }
        buttonImageRef = StaticDraw::imageFileRefs["button.png"];

        if (!StaticDraw::imageFileRefs.contains("optionsUi.png"))
        {
            StaticDraw::loadImage("assets/core/optionsUi.png");
        }
        uITex = StaticDraw::imageFileRefs["optionsUi.png"];



        // shaders
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
        //glUniform4f(colorLoc, 0.0f, 0.0f, 0.0f, 0.5f);  // change shader unfiorm
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 0.8f);

        // sounds
        if (!StaticAudio::soundStringRefs.contains("menuBloo.wav"))
        {
            StaticAudio::load("assets/core/menuBloo.wav", "menuBloo.wav", { "soundEffect" });
        }
        bwoo = StaticAudio::soundStringRefs["menuBloo.wav"];
        //StaticAudio::applyTags(bwoo, { "soundEffect" });

        StaticAudio::updateSounds();

        //if (!StaticDraw::hasShader("blackfadenoimage"))
        //{
        //    StaticDraw::compileShader("assets/shaders/simple.vs", "assets/gameSpecific/shaders/blackfadenoimage.fs", "blackfadenoimage");
        //}

        // declared on StaticDraw Init
        shaderSimpleRef = StaticDraw::getShader("simple");

        //uiY.xMin = 0;
        //uiY.yMin = 0;
        //uiY.xSize = 1;
        //uiY.ySize = 1;
        soundTitle = "Sound Settings";
        masterTitle = "Master Vollume";
        masterValue = "100"; // change later
        masterVollumeLeft = .9;
        masterVollumeWidth = .1;
        musicTitle = "Music Vollume";
        musicValue = "100"; // change later
        musicVollumeLeft = .7;
        musicVollumeWidth = .1;
        soundEffectTitle = "Sound Effects Vollume";
        soundEffectValue = "100"; // change later
        effectVollumeLeft = .5;
        effectVollumeWidth = .1;

        // exists to not have a ref not initialized
        // minimum decalration too large to keep code clean so ref needs to be declared up here
        // - but it had nothing to point to, so currentTarget was added
        //std::vector<std::unique_ptr<UIElement>> currentTarget;  
        //std::vector<std::unique_ptr<UIElement>>& ct = currentTarget;

        ui.appendType<UIYHolder>(5); // 0
        ui[0].appendType<UIBuffer>(.1) // 0 0
            .appendType<UITextOneLine>(-111, soundTitle,.5); // 0 0 0 title
        ui[0].appendType<UIXSplits>(std::vector<float>{ .25f, .6f, .15f }, -1) //0 1
            .appendType<UIBuffer>(.1) // 0 1 0
            .appendType<UITextOneLine>(-111, masterTitle, .2, XRIGHT);
        // bar
        std::vector<std::unique_ptr<UIElement>>& ct =
        ui[0][1].appendType<UIStack>().setKey(1) // 0 1 1
            .appendType<UIXHolder>() // 0 1 1 0
            //.appendSameType<TexUVNode>(10, 0, .25, 0, .5);
            .appendSameType<UIXRatio>(10, 1.0, true);
        for (auto& nodePtr : ct) 
        {
            UIElement& node = *nodePtr;
            node.appendType<TexUVNode>(.25, .75, 0, .5);
        }
        ui[0][1].appendType<UIBuffer>(.1) // 0 1 2
            .appendType<UITextOneLine>(-111, masterValue, .2, XLEFT);
        ui[0][1][1].appendType<UIXShifter>(masterVollumeLeft, masterVollumeWidth)
            .appendType<UIXRatio> (1.0)
            .appendType<TexUVNode>(.75, 1, 0, .5).setKey(11);

        ui[0].appendType<UIXSplits>(std::vector<float>{ .25f, .6f, .15f }, -1) // 0 2
            .appendType<UIBuffer>(.1)
            .appendType<UITextOneLine>(-111, musicTitle, .2, XRIGHT);
        std::vector<std::unique_ptr<UIElement>>& ct2 =
            ui[0][2].appendType<UIStack>().setKey(2) // key
            .appendType<UIXHolder>()
            .appendSameType<UIXRatio>(10, 1.0, true);
        for (auto& nodePtr : ct2)
        {
            UIElement& node = *nodePtr;
            node.appendType<TexUVNode>(.25, .75, 0, .5);
        }
        ui[0][2].appendType<UIBuffer>(.1)
            .appendType<UITextOneLine>(-111, musicValue, .2, XLEFT);
        ui[0][2][1].appendType<UIXShifter>(musicVollumeLeft, musicVollumeWidth)
            .appendType<UIXRatio>(1.0)
            .appendType<TexUVNode>(.75, 1, 0, .5).setKey(12);

        ui[0].appendType<UIXSplits>(std::vector<float>{ .25f, .6f, .15f }, -1) // 0 3
            .appendType<UIBuffer>(.1)
            .appendType<UITextOneLine>(-111, soundEffectTitle, .2, XRIGHT);
        std::vector<std::unique_ptr<UIElement>>& ct3 =
            ui[0][3].appendType<UIStack>().setKey(3) // key
            .appendType<UIXHolder>()
            .appendSameType<UIXRatio>(10, 1.0, true);
        for (auto& nodePtr : ct3)
        {
            UIElement& node = *nodePtr;
            node.appendType<TexUVNode>(.25, .75, 0, .5);
        }
        ui[0][3].appendType<UIBuffer>(.1)
            .appendType<UITextOneLine>(-111, soundEffectValue, .2, XLEFT);
        ui[0][3][1].appendType<UIXShifter>(effectVollumeLeft, effectVollumeWidth)
            .appendType<UIXRatio>(1.0)
            .appendType<TexUVNode>(.75, 1, 0, .5).setKey(13);

        //ui[0][1][1].debugTreePrint(); // selector/drag here
        
            //.appendSameType<UIXRatio>(10, -1, -1, 2, 2, 1.0, true);
        //ui[0][1][1][0].debugTreePrint();
        //std::cout << "math " << ui[0][1][0][0].size() << "\n";
            //.appendNode(std::make_unique<UIXRatio>(-1, -1, 2, 2, 1.0, true));
            //.appendMultipleNodes(std::make_unique<TexUVNode>(0, .25, 0, .5), 5);
            //.appendMultipleNodes(std::make_unique<UIXRatio>(-1, -1, 2, 2, 1.0, true),10);
        /*    .appendNode(std::make_unique<TexUVNode>(0, .25, 0, .5));
        ui[0][1][1].appendNode(std::make_unique<TexUVNode>(0, .25, 0, .5));
        ui[0][1][1].appendNode(std::make_unique<TexUVNode>(0, .25, 0, .5));
        ui[0][1].appendNode(std::make_unique<UIBuffer>(.1))
            .appendNode(std::make_unique<UITextOneLine>(-111, masterValue, .2));
        */

        ui[0].appendNode(std::make_unique<UIXHolder>());
        ui[0][4].appendNode(std::make_unique<TexUVNode>(0, .25, 0, .5));
        ui[0][4].appendNode(std::make_unique<TexUVNode>(0, .25, 0, .5));
        /*
        ui[0][0].appendNode(std::make_unique<TexUVNode>(0, .25, 0, .5));
        ui[0][0].appendNode(std::make_unique<TexUVNode>(0, .25, 0, .5));
        ui[0].appendNode(std::make_unique<UIXRatio>(1.0, true)).appendNode(std::make_unique<TexUVNode>(0, .25, 0, .5));
        ui[0].appendNode(std::make_unique<TexUVNode>(0, .25, 0, .5));
        ui[0].appendNode(std::make_unique<TexUVNode>(0, 1, .5, 1));
        ui[0].appendNode(std::make_unique<TexUVNode>(0, .25, 0, .5));
        ui[0].appendNode(std::make_unique<UIXRatio>(1.0, true)).appendNode(std::make_unique<TexUVNode>(0, .25, 0, .5));
        */
        aspectChange();
    }
    void render(float time = 0, bool updateDisplay = true) override
    {
        previous->render(0, false);

        StaticDraw::useShader(colorShaderRef);
        //StaticDraw::noSpecifyDraw(0, 0, 1.0f, 1.0f);
        StaticDraw::halfDimImage(0, 0, 0, 1.0f, 1.0f); // 0 as a texture ref is valid because shader doesn't use a texture

        //StaticDraw::noSpecifyDraw(0, 0, 1.0f, 1.0f);
        //StaticDraw::noSpecifyDraw(0, 0, 1.0f, 1.0f);
        
        //StaticDraw::backGroundImageRepeat(backgroundRef, 3.5);

        StaticDraw::useShader(shaderSimpleRef);
        //StaticDraw::halfDimImage(uITex, 0, 0, 1, 1);
        //StaticDraw::spriteImage(uITex, 0, 0, 1, 1, 0, 0, 4, 2);
        StaticDraw::multiDraw(uITex, batch);

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
        StaticDraw::updateView(); // need for proper aspect ratio update
        //float yScale = .25; // fontsize
        //float xScale = yScale / StaticDraw::aspectRatio;

        batch.clear();
        // channel -111 used to avoid conflict. Underflow makes it an absurdly large number
        StaticWrite::SetUpChannel(-111);
        //std::cout << "AAAA\n";
        //ui.adjustNode(ui.xMin, ui.yMin, ui.xSize, ui.ySize);
        ui.adjustNodeDefault();
        ui.renderVerts(batch);
        // non-positive number used for channel to avoid conflict with game allocated channels
        // also underflow logic error is fine
        //StaticWrite::SetUpChannel(-1);

        //float x = -.5;
        //float y = .5;
        //float shift = .1;

        //StaticWrite::AppendText(-1, "Exit", x, -y - shift, xScale, yScale);
        //StaticWrite::AppendText(-1, "Retry", x, y - shift, xScale, yScale);

        previous->aspectChange();
    }

    void processInput(GLFWwindow* window)
    {
        StaticInput::Tick();
        if (StaticInput::MouseClick(GLFW_MOUSE_BUTTON_LEFT))
        {
            buttonHover = ui.findOneHover(mouseCordX, mouseCordY);

            buttonPress(buttonHover);
            //std::cout << "click\n";
            //std::cout << "Mouse Cords: " << mouseCordX << " " << mouseCordY << "\n";
            //std::cout << "hover: " << buttonHover << "\n";
            
            //std::cout << "test find by key\n" << ui.findByKey(buttonHover).key << "\n";
        }
        else if (StaticInput::MouseHeld(GLFW_MOUSE_BUTTON_LEFT))
        {
            // magic numbers are keys for sliders
            // this assumes buttonHover from click
            if (buttonHover == 1 || buttonHover == 2 || buttonHover == 3)
            {
                int newHover = ui.findOneHover(mouseCordX, mouseCordY);
                if (newHover != buttonHover){ buttonHover = -1;}
                else{buttonPress(buttonHover);}
            }
        }
    }

    void buttonPress(int x)
    {
        if (x == 0)
        {
            DataHolder::SceneQueue(previous, true);
            // correct way to delete things
            //DataHolder::DelayDelete(previous); // delayed deletion is better than instant deletion in situations where multiple scenes are rendered(like this one)
            //previous->clean(); // clean needs to be ran before canging scenes
            //DataHolder::SceneQueue(new (), true);
        }
        else if (x == 1)
        {
            UIElement& barHolder = ui.findByKey(1);
            if (barHolder.key == 1)
            {
                //float l = barHolder.xSize;
                //float halfB = masterVollumeWidth / 2;
                //float mouseAdjust = mouseCordX - barHolder.xMin;
                //std::cout << "bar holder xmin" << barHolder.xMin << "\n";
                //float lb = barHolder.xSize - masterVollumeWidth;
                //float mouseAndB = mouseAdjust - halfB;
                // probably what I want
                //std::cout << "attempted solution: " << mouseAndB / lb << "\n";
                //masterVollume = mouseAndB / lb;
                //masterVollume = ((mouseCordX - barHolder.xMin) - (masterVollumeWidth / 2)) / (barHolder.xSize - masterVollumeWidth);
                masterVollume = ((mouseCordX - barHolder.xMin) - (masterVollumeWidth * 0.5f)) / (barHolder.xSize - masterVollumeWidth);
                if (masterVollume < 0) { masterVollume = 0; }
                else if (masterVollume > 1) { masterVollume =1; }
                //masterValue = std::to_string((int)std::round(masterVollume*100));
                masterValue = std::to_string((int)(masterVollume * 100.0f + 0.5f));

                masterVollumeLeft = masterVollume / (1.0 + masterVollumeWidth);

                StaticAudio::setMasterVollume(masterVollume);

                ui.adjustNodeDefault();
                StaticWrite::SetUpChannel(-111);
                batch.clear();
                ui.renderVerts(batch);

                saveSetting();
            }
        }
        else if (x == 2)
        {
            UIElement& barHolder = ui.findByKey(2);
            if (barHolder.key == 2)
            {
                musicVollume = ((mouseCordX - barHolder.xMin) - (musicVollumeWidth * 0.5f)) / (barHolder.xSize - musicVollumeWidth);
                if (musicVollume < 0) { musicVollume = 0; }
                else if (musicVollume > 1) { musicVollume = 1; }
                musicValue = std::to_string((int)(musicVollume * 100.0f + 0.5f));

                musicVollumeLeft = musicVollume / (1.0 + musicVollumeWidth);

                //StaticAudio::setMasterVollume(musicVollume);
                StaticAudio::updateTagVollume("music", musicVollume);
                StaticAudio::updateSounds();

                ui.adjustNodeDefault();
                StaticWrite::SetUpChannel(-111);
                batch.clear();
                ui.renderVerts(batch);

                saveSetting();
            }
        }
        else if (x == 3)
        {
            UIElement& barHolder = ui.findByKey(3);
            if (barHolder.key == 3)
            {
                effectVollume = ((mouseCordX - barHolder.xMin) - (effectVollumeWidth * 0.5f)) / (barHolder.xSize - effectVollumeWidth);
                if (effectVollume < 0) { effectVollume = 0; }
                else if (effectVollume > 1) { effectVollume = 1; }
                soundEffectValue = std::to_string((int)(effectVollume * 100.0f + 0.5f));

                effectVollumeLeft = effectVollume / (1.0 + effectVollumeWidth);

                StaticAudio::updateTagVollume("soundEffect", effectVollume);
                StaticAudio::updateSounds();

                StaticAudio::playSoundEffect(bwoo);

                ui.adjustNodeDefault();
                StaticWrite::SetUpChannel(-111);
                batch.clear();
                ui.renderVerts(batch);

                saveSetting();
            }
        }
    }

    void saveSetting()
    {

    }
};
