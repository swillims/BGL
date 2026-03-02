//#include "glue/textShell.h"
#include <array>
#include "scene/scene.h"
#include "uiHelper.h"
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

    unsigned int buttonHover;//  = true;

    // click handling bools
    bool click;
    bool loadAntiClick;

    // there are only two button so I'm not using a for loop and I can get away with two hoverBatchs
    std::vector<float> batch;
    //std::vector<float> hoverBatchA;
    //std::vector<float> hoverBatchB;

    // uiElementList
    //std::vector<std::unique_ptr<uiElement>> elements;

    // uiElement
    UIYHolder uiY;//(0.f, 0.f, 1.f, 1.f);

    // scene for backtracking and render
    Scene* previous;

    // constructor only used to instantiate a ui
    // -1, -1 is bottom left cornor for draw start and -1 to 1 scale has width and height of 2
    OptionsScreen() : uiY(-1,-1,2,2) {}

    // DO NOT CALL BEFORE StaticDraw::Init
    void onLoad() override
    {
        //myButtonTexture.load("assets/core/button.png");
        //window = glfwGetCurrentContext();
        Scene::onLoad();

        if (!StaticDraw::imageFileRefs.contains("button.png"))
        {
            StaticDraw::loadImage("assets/core/button.png");
        }
        buttonImageRef = StaticDraw::imageFileRefs["button.png"];

        std::cout << StaticDraw::imageFileRefs.contains("optionsUi.png") << " debug\n";
        if (!StaticDraw::imageFileRefs.contains("optionsUi.png"))
        {
            StaticDraw::loadImage("assets/core/optionsUi.png");
        }
        uITex = StaticDraw::imageFileRefs["optionsUi.png"];
        /*if (!StaticDraw::hasShader("simpleWhiteShade"))
        {
            StaticDraw::compileShader("simpleWhiteShade", "assets/shaders/simple.vs", "assets/shaders/simplemixwhite.fs");
        }
        shaderWhiteShadeRef = StaticDraw::getShader("simpleWhiteShade").ID;
        */
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
        glUniform4f(colorLoc, 0.0f, 0.0f, 0.0f, 0.5f);  // change shader unfiorm


        if (!StaticAudio::soundStringRefs.contains("menuBloo.wav"))
        {
            StaticAudio::load("assets/core/menuBloo.wav", "menuBloo.wav", { "soundEffect" });
        }
        bwoo = StaticAudio::soundStringRefs["menuBloo.wav"];
        //StaticAudio::applyTags(bwoo, { "soundEffect" });

        StaticAudio::updateSounds();

        if (!StaticDraw::hasShader("blackfadenoimage"))
        {
            StaticDraw::compileShader("assets/shaders/simple.vs", "assets/gameSpecific/shaders/blackfadenoimage.fs", "blackfadenoimage");
        }

        // declared on StaticDraw Init
        shaderSimpleRef = StaticDraw::getShader("simple");

        //uiY.xMin = 0;
        //uiY.yMin = 0;
        //uiY.xSize = 1;
        //uiY.ySize = 1;

        uiY.appendNode(std::make_unique<TexUVNode>(0,.25,0,.5));
        uiY.appendNode(std::make_unique<TexUVNode>(0, 1, .5, 1));
        uiY.appendNode(std::make_unique<TexUVNode>(0, .25, 0, .5));
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
        uiY.adjustNode(uiY.xMin, uiY.yMin, uiY.xSize, uiY.ySize);
        uiY.renderVerts(batch);
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
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            if (!click) { buttonPress(buttonHover); }
            click = true;
            //std::cout << "click\n";
        }
        else { click = false; }
        //if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        //    glfwSetWindowShouldClose(window, true);
    }

    void buttonPress(int x)
    {
        if (x == 0)
        {
            // correct way to delete things
            //DataHolder::DelayDelete(previous); // delayed deletion is better than instant deletion in situations where multiple scenes are rendered(like this one)
            //previous->clean(); // clean needs to be ran before canging scenes
            //DataHolder::SceneQueue(new (), true);
        }
        else if (x == 1) { DataHolder::SceneQueue(previous, true); }
    }
};
