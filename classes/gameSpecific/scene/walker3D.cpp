#include "walker3D.h"

#include "singleton/staticDraw.h"
#include "singleton/staticInput.h"
#include "singleton/staticSound.h"

#include "glm/ext/matrix_transform.hpp"

#include "3dHelper.h"
#include "glm/ext/matrix_clip_space.hpp"

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

    // set physics framerate to 60
    DataHolder::SetPhysicsCap(60);

    // set mat4 defaults
    viewMat4 = glm::mat4(1.0f);
    projectionMat4 = glm::mat4(1.0f);

    // generate ground
    floor = generateFlatGrid5Vao(-20,20,-20,20,paramX,paramZ);

    aspectChange();
}

float x = 0;

void Walker3D::handle(float time)
{
    processInput(window, time);

    x += time;

    aspectChange();
}

void Walker3D::render(float time, bool updateDisplay)
{
    // clearing before a draw is correct
    // - clear3D also resets a depth buffer and the depth buffer needs to be set to draw in 3D
    StaticDraw::clear3D();
    batch.clear();

    // Camera Things
    player.camYaw += time;

    // when I looked it up, it said to include yaw. I want to try it without and update later.
    player.direction.x = cos(player.camYaw);// * cos(player.camPitch);
    player.direction.y = sin(player.camPitch);
    player.direction.z = sin(player.camYaw);// * cos(player.camPitch);

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


    // call super to render
    Scene::render(time, updateDisplay);
}

void Walker3D::processInput(GLFWwindow *window, float time)
{
    StaticInput::Tick();

}

void Walker3D::aspectChange()
{
    projectionMat4 = glm::perspective(
        glm::radians(45.0f),
        StaticDraw::aspectRatio,
        0.1f,
        100.0f
    );
    StaticDraw::updateSharedShaderVariable(projectionUboRef, projectionMat4);
}

void Walker3D::clean()
{

}
