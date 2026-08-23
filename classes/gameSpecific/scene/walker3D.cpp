#include "walker3D.h"

#include "singleton/staticDraw.h"
#include "singleton/staticInput.h"
#include "singleton/staticSound.h"

#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

#include "3dHelper.h"

#include "walkerKeyOptions.h"
#include "pauseMenu.h"

void Walker3D::onLoad()
{
    Scene::onLoad();
    StaticDraw::set3DEnabled();

    // have two of a few things to validate loads

    // load shaders
    shaderSimpleRef = StaticDraw::getShader("simple");

    if (!StaticDraw::hasShader("simple3d"))
    {
        StaticDraw::compileShader("assets/shaders/simple3d.vs", "assets/shaders/simple.fs", "simple3d");
    }
    shader3DSimple = StaticDraw::getShader("simple3d");

    if (!StaticDraw::hasShader("projection3d"))
    {
        StaticDraw::compileShader("assets/shaders/projectionview3d.vs", "assets/shaders/simple.fs", "projection3d");
    }
    shader3DProjection = StaticDraw::getShader("projection3d");

    // create ubo and default mat4 values
    viewMat4 = glm::mat4(1.0f);
    projectionMat4 = glm::mat4(1.0f);
    viewUboRef = StaticDraw::createSharedShaderVariable(viewMat4, shader3DProjection, "view").ref;
    projectionUboRef = StaticDraw::createSharedShaderVariable(projectionMat4, shader3DProjection, "projection").ref;

    // load vaos
    baseVao = StaticDraw::VAOSimple;

    if (!StaticDraw::hasVAO("3dSimple"))
    {
        StaticDraw::CreateVAO({3, 2}, "3dSimple");
    }
    tileVaoRef = StaticDraw::getVAO("3dSimple").ref;
    tileVaoCount = StaticDraw::getVAO("3dSimple").floatCount;

    // load textures
    if (!StaticDraw::imageFileRefs.contains("tile"))
    {
        StaticDraw::loadImage("assets/gameSpecific/png/frogHop/block.png", "tile");
    }
    tile = StaticDraw::imageFileRefs["tile"];

    if (!StaticDraw::imageFileRefs.contains("greenTile"))
    {
        StaticDraw::loadImage("assets/gameSpecific/png/walk3d/green.png", "greenTile");
    }
    greenTile = StaticDraw::imageFileRefs["greenTile"];

    // load sounds
    StaticAudio::updateSounds();

    // load inputs
    // - creating temp keyOptions page to pull values from
    WalkerKeyOptions keyOptions;
    keyOptions.previous = this; // not setting previous makes avoiding crashes easier
    keyOptions.onLoad();
    qKey = StaticInput::GetAlias(keyOptions.qTitle);
    wKey = StaticInput::GetAlias(keyOptions.wTitle);
    eKey = StaticInput::GetAlias(keyOptions.eTitle);
    aKey = StaticInput::GetAlias(keyOptions.aTitle);
    sKey = StaticInput::GetAlias(keyOptions.sTitle);
    dKey = StaticInput::GetAlias(keyOptions.dTitle);
    escKey = StaticInput::GetAlias(keyOptions.escTitle);

    StaticInput::KeyTrackSetAll(false); // not tracking unused keys is a mild optimization.
    StaticInput::KeyTrack(qKey);
    StaticInput::KeyTrack(wKey);
    StaticInput::KeyTrack(eKey);
    StaticInput::KeyTrack(aKey);
    StaticInput::KeyTrack(sKey);
    StaticInput::KeyTrack(dKey);
    StaticInput::KeyTrack(escKey);

    // set strings
    menuString = "Pause: " + StaticInput::IntToString(escKey);
    playerWalkString = "Move: " + StaticInput::IntToString(wKey) + " " + StaticInput::IntToString(aKey) + " " +
        StaticInput::IntToString(sKey) + " " + StaticInput::IntToString(dKey);
    rotateString = "Rotate: " + StaticInput::IntToString(qKey) + " " + StaticInput::IntToString(eKey);

    // set physics framerate to 60
    DataHolder::SetPhysicsCap(60);

    if (!alreadyLoaded)
    {
        // set mat4 defaults
        viewMat4 = glm::mat4(1.0f);
        projectionMat4 = glm::mat4(1.0f);

        // generate ground
        floor = generateFlatGrid5Vao(-20,20,-20,20,paramX,paramZ);

        // ui is completely unnessary here but I have a tool, so I should use it in a tutorial
        ui.appendType<UIYSplits>(std::vector<float>({.1f,.1f,.1f}))
            .appendType<UITextOneLine>(uiTextChannel, menuString, .6f, XLEFT).back()
            .appendType<UITextOneLine>(uiTextChannel, rotateString, .6f, XLEFT).back()
            .appendType<UITextOneLine>(uiTextChannel, playerWalkString, .6f, XLEFT).back()
        ;

        alreadyLoaded = true;
    }

    menuEsc = false;
    aspectChange();
}

void Walker3D::handle(float time)
{
    processInput(window, time);

    if (menuEsc){return;} // stop handle if a return is called

    player.position += glm::vec3(player.playerVX, 0.0f, player.playerVZ);

    //x += time;
}

void Walker3D::render(float time, bool updateDisplay)
{
    // clearing before a draw is correct
    // - clear3D also resets a depth buffer and the depth buffer needs to be set to draw in 3D
    StaticDraw::clear3D();

    // Camera Things
    //player.camYaw += time;

    player.direction.x = std::sin(player.camYaw);
    player.direction.y = std::sin(player.camPitch);
    player.direction.z = std::cos(player.camYaw);

    viewMat4 = glm::lookAt(
        player.position,
        player.position + player.direction,
        glm::vec3(0.0f, 1.0f, 0.0f)   // up
    );

    StaticDraw::updateSharedShaderVariable(viewUboRef, viewMat4);


    //StaticDraw::useShader(shader3DSimple);
    StaticDraw::useShader(shader3DProjection);

    StaticDraw::multiDraw
    (
        tile,
        {
            -0.5f, -0.5f, 1.0f,  0.0f, 0.0f,
            0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
            0.5f,  0.5f, 0.0f,  1.0f, 1.0f
        },
        tileVaoRef,
        tileVaoCount
    );

    StaticDraw::multiDraw
    (
        greenTile,
        {
            0.5f, -0.5f, 1.0f,  0.0f, 0.0f,
            -0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f, 0.0f,  1.0f, 1.0f
        },
        tileVaoRef,
        tileVaoCount
    );

    StaticDraw::multiDraw
    (
        greenTile,
        floor,
        tileVaoRef,
        tileVaoCount
    );

    StaticWrite::StartWrite();
    StaticWrite::DrawChannel(uiTextChannel, glm::vec3(1.0f, 1.0f, 1.0f));

    // call super to render
    Scene::render(time, updateDisplay);
}

void Walker3D::processInput(GLFWwindow *window, float time)
{
    StaticInput::Tick();
    rotateQ = StaticInput::KeyHeld(qKey);
    rotateE = StaticInput::KeyHeld(eKey);
    walkW = StaticInput::KeyHeld(wKey);
    walkA = StaticInput::KeyHeld(aKey);
    walkS = StaticInput::KeyHeld(sKey);
    walkD = StaticInput::KeyHeld(dKey);

    if (StaticInput::KeyClick(escKey))
    {
        PauseMenu* pm = new PauseMenu(this);
        DataHolder::SceneQueue(pm, false);
        menuEsc = true;
        return;
    }

    // the tutorial is technical over practical. For a real game, normalize coordinates.
    float delta = time * player.moveSpeed;
    float controlX = 0.0f;
    float controlZ = 0.0f;
    controlX -= walkA;
    controlX += walkD;
    controlZ += walkW;
    controlZ -= walkS;

    float rotatedX = -(controlX * std::cos(player.camYaw)) + (controlZ * std::sin(player.camYaw));
    float rotatedZ = (controlX * std::sin(player.camYaw)) + (controlZ * std::cos(player.camYaw));

    glm::vec3 normalized(rotatedX, 0.0f, rotatedZ);

    player.playerVX = normalized.x * delta;
    player.playerVZ = normalized.z * delta;

    float controlCam = 0.0f;
    // The math people decided that positive is counterclockwise and negative is clockwise. This looks wrong but is right. Don't be mad at me, be mad at the math people.
    controlCam += rotateQ;
    controlCam -= rotateE;
    controlCam *= time * player.rotateSpeed;
    player.camYaw += controlCam;
}

void Walker3D::aspectChange()
{
    StaticDraw::updateView();
    projectionMat4 = glm::perspective(
        glm::radians(45.0f),
        StaticDraw::aspectRatio,
        0.1f,
        100.0f
    );
    StaticDraw::updateSharedShaderVariable(projectionUboRef, projectionMat4);

    uiBatch.clear();
    StaticWrite::SetUpChannel(uiTextChannel);
    ui.adjustNodeDefault();
    ui.renderVerts(uiBatch);
}

void Walker3D::clean()
{

}
