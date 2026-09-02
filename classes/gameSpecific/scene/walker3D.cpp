#include "walker3D.h"

#include "singleton/staticDraw.h"
#include "singleton/staticInput.h"
#include "singleton/staticSound.h"

#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

#include "3dHelper.h"

#include "walkerKeyOptions.h"
#include "pauseMenu.h"

/*
 * This engine is 2d first.
 * This 3d section is proof of concept.
 * If it works it works.
 * The math is weird and convoluted. I(the author) had to look up everything.
 * If you have the math skills to do this, awesome.
 * For normal people, it is better to use a professional engine or stick with 2d.
 * Math is hard.
*/


void Walker3D::onLoad()
{
    Scene::onLoad();
    StaticDraw::set3DEnabled();

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

    if (!StaticDraw::hasShader("3d2d"))
    {
        StaticDraw::compileShader("assets/shaders/3d2d.vs", "assets/shaders/simple.fs", "3d2d");
    }
    shader3d2d = StaticDraw::getShader("3d2d");

    // create ubo and default mat4 values
    viewMat4 = glm::mat4(1.0f);
    projectionMat4 = glm::mat4(1.0f);
    if (!StaticDraw::hasSharedShaderVariable("view"))
    {
        StaticDraw::createSharedShaderVariable(viewMat4, "view").ref;
    }
    viewUboRef = StaticDraw::getSharedShaderVariable("view").ref;
    if (!StaticDraw::hasSharedShaderVariable("projection"))
    {
        StaticDraw::createSharedShaderVariable(projectionMat4, "projection").ref;
    }
    projectionUboRef = StaticDraw::getSharedShaderVariable("projection").ref;
    StaticDraw::bindSharedShaderVariable(shader3DProjection, "view");
    StaticDraw::bindSharedShaderVariable(shader3DProjection, "projection");
    StaticDraw::bindSharedShaderVariable(shader3d2d, "view");
    StaticDraw::bindSharedShaderVariable(shader3d2d, "projection");

    // load vaos
    baseVao = StaticDraw::VAOSimple;

    if (!StaticDraw::hasVAO("3dSimple"))
    {
        StaticDraw::CreateVAO({3, 2}, "3dSimple");
    }
    tileVaoRef = StaticDraw::getVAO("3dSimple").ref;
    tileVaoCount = StaticDraw::getVAO("3dSimple").floatCount;

    if (!StaticDraw::hasVAO("3d2d"))
    {
        StaticDraw::CreateVAO({3, 2, 1}, "3d2d");
    }
    threeDTwoDRef = StaticDraw::getVAO("3d2d").ref;
    threeDTwoDCount = StaticDraw::getVAO("3d2d").floatCount;

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
}

void Walker3D::render(float time, bool updateDisplay)
{
    // clearing before a draw is correct
    // - clear3D also resets a depth buffer and the depth buffer needs to be set to draw in 3D
    StaticDraw::clear3D();

    // Camera Things
    // - idk what any of this camera stuff is. I looked up how to do it. I'm learning while I make this.
    player.direction.x = std::sin(player.camYaw);
    player.direction.y = std::sin(player.camPitch);
    player.direction.z = std::cos(player.camYaw);

    glm::vec3 forward = glm::normalize(player.direction);
    glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), forward));
    glm::vec3 up = glm::cross(forward, right);

    viewMat4 = glm::mat4(1.0f);

    viewMat4[0] = glm::vec4(right, 0.0f);
    viewMat4[1] = glm::vec4(up, 0.0f);
    viewMat4[2] = glm::vec4(-forward, 0.0f);

    viewMat4[3] = glm::vec4(
        -glm::dot(right, player.position),
        -glm::dot(up, player.position),
         glm::dot(forward, player.position),
        1.0f
    );

    StaticDraw::updateSharedShaderVariable(viewUboRef, viewMat4);

    StaticDraw::useShader(shader3DProjection);

    StaticDraw::multiDraw(greenTile, floor, tileVaoRef, tileVaoCount);

    StaticDraw::useShader(shader3d2d);
    StaticDraw::multiDraw
    (
        tile,
        {
            0.0f, 2.0f, 5.0f, 0.0f, 0.0f, -1.0f,
            0.0f, 2.0f, 5.0f, 1.0f, 0.0f,  1.0f,
            0.0f, 4.0f, 5.0f, 1.0f, 1.0f,  1.0f,

            0.0f, 2.0f, 5.0f, 0.0f, 0.0f, -1.0f,
            0.0f, 4.0f, 5.0f, 1.0f, 1.0f,  1.0f,
            0.0f, 4.0f, 5.0f, 0.0f, 1.0f, -1.0f,
        },
        threeDTwoDRef,
        threeDTwoDCount
    );

    hoveredImage = getHoveredImage();
    if (hoveredImage!=-1)
    {

    }
    else
    {
        getTile();
    }

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

    float delta = time * player.moveSpeed;
    float controlX = 0.0f;
    float controlZ = 0.0f;
    controlX -= walkA;
    controlX += walkD;
    controlZ += walkW;
    controlZ -= walkS;

    float rotatedX = (controlX * std::cos(player.camYaw)) + (controlZ * std::sin(player.camYaw));
    float rotatedZ = -(controlX * std::sin(player.camYaw)) + (controlZ * std::cos(player.camYaw));

    glm::vec3 normalized(rotatedX, 0.0f, rotatedZ);

    player.playerVX = normalized.x * delta;
    player.playerVZ = normalized.z * delta;

    float controlCam = 0.0f;
    controlCam -= rotateQ;
    controlCam += rotateE;
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

    updateUI();
}

void Walker3D::clean()
{

}

int Walker3D::getHoveredImage()
{
    return -1;
}

int Walker3D::getTile()
{
    // I don't know how to do a raycast so I looked it up
    double mouseX;
    double mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    // Mouse -> NDC
    float x = (2.0f * mouseX) / StaticDraw::w - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / StaticDraw::h;

    // NDC -> world ray
    glm::vec4 nearPoint = glm::inverse(projectionMat4) * glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 farPoint  = glm::inverse(projectionMat4) * glm::vec4(x, y,  1.0f, 1.0f);

    nearPoint /= nearPoint.w;
    farPoint  /= farPoint.w;

    nearPoint = glm::inverse(viewMat4) * nearPoint;
    farPoint  = glm::inverse(viewMat4) * farPoint;

    glm::vec3 rayOrigin = glm::vec3(nearPoint);
    glm::vec3 rayDirection = glm::normalize(glm::vec3(farPoint - nearPoint));

    // Ray -> Y = 0
    float distance = -rayOrigin.y / rayDirection.y;

    glm::vec3 hit = rayOrigin + rayDirection * distance;

    // The 2D tile coordinate
    int tileX = static_cast<int>(std::floor(hit.x));
    int tileZ = static_cast<int>(std::floor(hit.z));

    std::cout << tileX << " " << tileZ << "\n";

    return -1;
}

void Walker3D::updateUI()
{
    uiBatch.clear();
    StaticWrite::SetUpChannel(uiTextChannel);
    ui.adjustNodeDefault();
    ui.renderVerts(uiBatch);
}
