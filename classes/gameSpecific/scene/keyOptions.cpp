#include "keyOptions.h"
#include "graphicOptions.h"
#include "soundOptions.h"

struct ButtonInput : Scene
{
    std::string aliasTarget;
    std::string message;

    // scene for backtracking and render
    Scene* previous;
    std::vector<float> batch;
    std::vector<int> ints;

    UIXRatio ui;

    ButtonInput(std::string aliasTarget, Scene* previous) : ui(-1, -1, 2, 2, 1.0, true), aliasTarget(aliasTarget), previous(previous) {}

    void onLoad() override
    {
        Scene::onLoad();
        message = "Press a Key";

        ui.appendType<UIBuffer>(.2).appendType<UITextOneLine>(-11111, message, .15);

        StaticInput::KeyTrackSetAll();
        ints = StaticInput::GetTrackedKeys(); // very unnecessary but showing tools

        aspectChange();
    }

    void render(float time = 0, bool updateDisplay = true) override
    {
        // draw background scene
        previous->render(0, false);

        // write text
        StaticWrite::StartWrite();
        StaticWrite::DrawChannel(-11111, glm::vec3(0.0f, 0.0f, 0.0f));

        // call super
        Scene::render(time, updateDisplay);
    }

    void handle(float time = 0) override
    {
        StaticInput::Tick();
        processInput(window);
    }

    void aspectChange()
    {
        StaticDraw::updateView();

        batch.clear();
        ui.adjustNodeDefault();
        ui.renderVerts(batch);

        previous->aspectChange();
    }

    void processInput(GLFWwindow* window)
    {
        for (int i : ints) // a regular for loop would be better but I am using ints to show how to use StaticInput::GetTrackedKeys();
        {
            if (StaticInput::KeyClick(i))
            {
                StaticInput::AssignAlias(aliasTarget, i);
                DataHolder::SceneQueue(previous, true);
                return;
            }
        }
    }
};

void KeyOptions::buttonPress(int x)
{
    //std::cout<<x<<"\n";
    if (x == uiExit)
    {
        DataHolder::SceneQueue(previous, true);
    }
    else if (x == uiSave)
    {
        saveSetting();
    }
    else if (x == uiQ)
    {
        ButtonInput* scene = new ButtonInput(qTitle, this);
        DataHolder::SceneQueue(scene, false);
    }
    else if (x == uiW)
    {
        ButtonInput* scene = new ButtonInput(wTitle, this);
        DataHolder::SceneQueue(scene, false);
    }
    else if (x == uiE)
    {
        ButtonInput* scene = new ButtonInput(eTitle, this);
        DataHolder::SceneQueue(scene, false);
    }
    else if (x == uiEsc)
    {
        ButtonInput* scene = new ButtonInput(escTitle, this);
        DataHolder::SceneQueue(scene, false);
    }
    else if (x==uiGraphicsSettings)
    {
        GraphicsOptions* graphics = new GraphicsOptions();
        graphics->previous = previous;
        DataHolder::SceneQueue(graphics, true);
    }
    else if (x==uiSoundSettings)
    {
        SoundOptions* sound = new SoundOptions();
        sound->previous = previous;
        DataHolder::SceneQueue(sound, true);
    }
}

