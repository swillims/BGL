#include "pauseMenu.h"

#include "scene/scene.h"
#include "singleton/dataHolder.h"
#include "singleton/staticInput.h"

#include "soundOptions.h"
#include "mainMenu.h"

void PauseMenu::onLoad()
{
    // call super because super sets window*
    Scene::onLoad();

    // images
    if (!StaticDraw::imageFileRefs.contains("optionsUi.png"))
    {
        StaticDraw::loadImage("assets/core/optionsUi.png");
    }
    uITex = StaticDraw::imageFileRefs["optionsUi.png"];

    // shaders
    shaderSimpleRef = StaticDraw::getShader("simple");

    if (!StaticDraw::hasShader("colorRef"))
    {
        StaticDraw::compileShader("assets/shaders/simple.vs", "assets/shaders/color.fs", "colorRef");
    }
    colorShaderRef = StaticDraw::getShader("colorRef");

    StaticDraw::useShader(colorShaderRef); // selecting shader is needed to modify shader in opengl
    GLint colorLoc = glGetUniformLocation(colorShaderRef, "color"); // get uniform location. Uniforms are shader vars
    glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 0.8f);

    returnStr = "Return to Game";
    optionsStr = "Open Options";
    mainMenuStr = "Return to Main Menu";
    exitStr = "Exit Game";

    ui.appendType<UIYHolder>()
    .appendType<UIXRatio>(3, true)
        .appendType<UIStack>()
            .appendType<TexUVNode>(0, 1, .5, 1, uiReturnToGame).back()
            .appendType<UITextOneLine>(writeChannel, returnStr, .25).back()
        .back()
    .back()

    .appendType<UIXRatio>(3, true)
        .appendType<UIStack>()
            .appendType<TexUVNode>(0, 1, .5, 1, uiOptions).back()
            .appendType<UITextOneLine>(writeChannel, optionsStr, .25).back()
        .back()
    .back()

    .appendType<UIXRatio>(3, true)
        .appendType<UIStack>()
            .appendType<TexUVNode>(0, 1, .5, 1, uiMainMenu).back()
            .appendType<UITextOneLine>(writeChannel, mainMenuStr, .25).back()
        .back()
    .back()

    .appendType<UIXRatio>(3, true)
        .appendType<UIStack>()
            .appendType<TexUVNode>(0, 1, .5, 1, uiCloseProgram).back()
            .appendType<UITextOneLine>(writeChannel, exitStr, .25);

    aspectChange();
}

void PauseMenu::render(float time, bool updateDisplay)
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
    StaticWrite::DrawChannel(writeChannel, glm::vec3(0.0f, 0.0f, 0.0f));

    // call super
    Scene::render(time, updateDisplay);
}

void PauseMenu::handle(float time)
{
    processInput();
}

void PauseMenu::aspectChange()
{
    StaticDraw::updateView();

    batch.clear();

    StaticWrite::SetUpChannel(writeChannel);
    ui.adjustNodeDefault();
    ui.renderVerts(batch);

    previous->aspectChange();
}

void PauseMenu::processInput()
{
    Scene::processInput();

    StaticInput::Tick();
    if (StaticInput::MouseClick(GLFW_MOUSE_BUTTON_LEFT))
    {
        double x,y;
        StaticInput::GetMouse(x,y);
        int buttonHover = ui.findOneHover(x, y);
        buttonPress(buttonHover);
    }
}

void PauseMenu::buttonPress(int x)
{
    if (x==uiReturnToGame)
    {
        DataHolder::SceneQueue(previous,true);
    }
    else if (x==uiOptions)
    {
        SoundOptions* options = new SoundOptions();
        options->previous = this;
        DataHolder::SceneQueue(options,false);
    }
    else if (x==uiMainMenu)
    {
        MainMenu* menu = new MainMenu();
        previous->clean();
        DataHolder::DelayDelete(previous);
        DataHolder::SceneQueue(menu,true);
    }
    else if (x==uiCloseProgram)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void PauseMenu::setTextChannel(int x)
{
    writeChannel=x;
    ui.nodes.clear(); // clear nodes
    onLoad(); // reload the scene but with different text channel set in nodes
}

