#include <iostream>

#ifdef _WIN32
    #include <GLFW/glfw3.h> // windows // comment added to make ignoring in grep easier
    #include <GL/gl.h>
    #include <glad/glad.h>
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glm/gtc/type_ptr.hpp>
#elif __linux__
    #include "singleton/gl_core.h"
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glm/gtc/type_ptr.hpp>
#endif

#include <map>
#include <string>
#include <vector>
#include <chrono>

#include "singleton/dataHolder.h"
#include "singleton/staticWrite.h"
#include "singleton/staticDraw.h"
#include "singleton/staticSound.h"
#include "singleton/staticInput.h"

#include "scene/scene.h"

// replace mainMenu import if you write your own intro scene. 
#include "gameSpecific/scene/mainMenu.h"

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void processInput(GLFWwindow* window);

static int mainmainmain()
{
    
    std::cout << "mainmain here\n";
    // glfw: initialize and configure
    // ------------------------------

    // set root for relative links to executable because it is needed for loading files
#ifdef _win32
    //I Haven't had a need to set current_path to project root in windows yet so this unimplemented
    std::filesystem::path rootPath = std::filesystem::current_path();
#elif __linux__
    std::filesystem::path rootPath = std::filesystem::read_symlink("/proc/self/exe").parent_path();
    std::filesystem::current_path(rootPath);
#endif
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // init dataholder
    DataHolder::init(); // dataholder holder handles scene management. It can have game specific data added to it(code yourself).
    DataHolder& g = DataHolder::god;
    StaticAudio::init();

    // defaults for if a setting doesn't exist or fails to load
    unsigned int SCR_WIDTH = 800;
    unsigned int SCR_HEIGHT = 600;

    // variable for loading settings
    std::string settingsFileName;

    // load graphics settings
    settingsFileName = "metadata/graphicsettings";
    if (util::fileExists(settingsFileName))
    {
        // read file and split by line
        // - note: util::split handles Windows (\r\n) and Unix (\n) line endings automatically. Look this up if you don't know it.
        std::cout << "graphics settings file exists\n";
        std::vector<std::string> lines = util::split(util::readFile(settingsFileName),"\n");

        // handle each line
        for (std::string& line : lines) // it's ok to not use const here because string is not reused and is not a literal
        {
            util::removeComments(line, "#");
            util::removeComments(line, "//");
            std::vector<std::string> data = util::split(line, ":");
            if (data.size() > 1)
            {
                util::sanitizeString(data[1], {" "});
                try
                {
                    int n = std::stoi(data.at(1));
                    g.uncategorizedData[data.at(0)] = n;
                    // it's also possible to do it this way
                    // - g.setUnCatData(data.at(0), n);
                    // and this way
                    // - DataHolder::SetUnCatData(data.at(0), n);
                }
                catch(std::exception e){}
            }
        }
        if (g.uncategorizedData.contains("SCR_WIDTH"))
        {
            SCR_WIDTH = g.getUnCatData<int>("SCR_WIDTH");
            g.deleteUnCatData("SCR_WIDTH");

        }
        // using struct method instead of c++ method to show anoher to check if key exists
        if (g.checkKeyUnCatData("SCR_HEIGHT"))
        {
            SCR_HEIGHT = g.getUnCatData<int>("SCR_HEIGHT");
            g.deleteUnCatData("SCR_HEIGHT");
        }
        if (g.uncategorizedData.contains("FPS_CAP"))
        {
            g.setFrameCap(g.getUnCatData<int>("FPS_CAP"));
            g.deleteUnCatData("FPS_KEY");
        }
    }

    // keybind settings
    settingsFileName = "metadata/keybindsettings";
    if (util::fileExists(settingsFileName))
    {
        // read file and split by line
        std::cout << "keybind settings file exists\n";
        std::vector<std::string> lines = util::split(util::readFile(settingsFileName),"\n");

        // handle each line
        for (std::string& line : lines) // it's ok to not use const here because string is not reused and is not a literal
        {
            util::removeComments(line, "#");
            util::removeComments(line, "//");
            std::vector<std::string> data = util::split(line, ":");
            if (data.size() > 1)
            {
                util::sanitizeString(data[1], {" "});
                StaticInput::AssignAlias(data[0], data[1]);
            }
        }
    }

    // sound settings
    settingsFileName = "metadata/soundsettings";
    if (util::fileExists(settingsFileName))
    {
        std::cout << "sound settings file exists\n";
        std::vector<std::string> lines = util::split(util::readFile(settingsFileName),"\n");

        // handle each line
        for (std::string& line : lines) // it's ok to not use const here because string is not reused and is not a literal
        {
            util::removeComments(line, "#");
            util::removeComments(line, "//");
            std::vector<std::string> data = util::split(line, ":");
            if (data.size() >1 )
            {
                util::sanitizeString(data[1], {" "});
                try
                {
                    int n = std::stoi(data.at(1));
                    g.uncategorizedData[data.at(0)] = n;
                }
                catch(std::exception e){}
            }
        }
        if (g.uncategorizedData.contains("SOUND_MASTER"))
        {
            float m = g.getUnCatData<int>("SOUND_MASTER");
            m /= 100;
            StaticAudio::setMasterVollume(m);
            g.deleteUnCatData("SOUND_MASTER");
        }
        if (g.checkKeyUnCatData("SOUND_MUSIC"))
        {
            float m = g.getUnCatData<int>("SOUND_MUSIC");
            m /= 100;
            StaticAudio::updateTagVollume("music", m);
            g.deleteUnCatData("SOUND_MUSIC");
        }
        if (g.uncategorizedData.contains("SOUND_EFFECT"))
        {
            float m = g.getUnCatData<int>("SOUND_EFFECT");
            m /= 100;
            StaticAudio::updateTagVollume("soundEffect", m);
            g.deleteUnCatData("SOUND_EFFECT");
        }
    }

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "BGL Game Window", NULL, NULL); // change string to change game name
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    //
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    // OpenGL state
    // ------------
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // static write initialized before load screan because required for words.

    // IMPORTANT
    // if you are using the static Init on multiple fonts, you need to be careful of leaks because it heap allocates the fonts.
    // single font is the only intended usage of Init and the engine. It allows multiple fonts but not well and using different instances. 
    // implement your own storage/tracking for references to other instances if multiple fonts are uses.
    StaticWrite::Init("assets/fonts/TeX-Gyre-Heros/texgyreheros-regular.otf", 64); // replace string for different default font
    
    // minimalistic load screan. Replace next four lines if you write your own. Requires Static write init.
    glClearColor(0.25f, 0.25, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    //StaticWrite::RenderText("Loading...", -50.0f, -50.0f, 0.5f, glm::vec3(1, 1, 1));
    StaticWrite::AppendText(0, "Loading...", -.8f, -.2f, 0.006f, 0.006f);
    StaticWrite::StartWrite();
    StaticWrite::DrawChannel(0, glm::vec3(1, 1, 1));
    glfwSwapBuffers(window);

    // inits
    StaticDraw::init();
    StaticDraw::windowSpecify(window);
    //StaticAudio::init();
    // Order of Init calls does matter. StaticInput has to go after window specification.
    // - StaticInput's init doesn't create or change objects, so it is ok to modify it in the load code above.
    StaticInput::Init();
    //StaticInput::singleton;//.init();

    // replace MainMenu if you write your own.
    // cleanest way to implement a new game is to modify Main menu instead of replacing it.
    // main menu is located at assets/gameSpecific/scene/mainMenu.h because it should be modified and should be different for most builds.
    DataHolder::SceneQueue(new MainMenu, false);

    //StaticDraw::init();
    std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        StaticDraw::cleanIndices();
        std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        float deltaTime = elapsed.count(); // in seconds
        lastTime = currentTime; // update for next frame

        DataHolder::SceneUpdate();

        g.handleScene(deltaTime);

        glfwPollEvents();
    }

    glfwTerminate();
    
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
static void processInput(GLFWwindow* window)
{
    //if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    //    glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    try
    {
        DataHolder::god.currentScene->aspectChange();
    }
    catch(std::exception e){}
    glViewport(0, 0, width, height);
}

// main/central method decoupled from entry point. It is at the bottom so it doesn't have to be forward declared
struct mainmain
{
    static void run()
    {
        mainmainmain();
    }
};