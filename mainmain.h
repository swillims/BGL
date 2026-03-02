#include <iostream>

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>
#include <string>
#include <vector>
#include <chrono>

//#include <shader.h>
//#include "glue/textShell.h"

#include "singleton/dataHolder.h"
#include "singleton/staticWrite.h"
#include "singleton/staticDraw.h"
#include "singleton/staticSound.h"

#include "scene/scene.h"

// replace mainMenu import if you write your own intro scene. 
#include "gameSpecific/scene/mainMenu.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//const unsigned int SCR_WIDTH = 800;
//const unsigned int SCR_HEIGHT = 600;



static int mainmainmain()
{
    
    std::cout << "mainmain here\n";
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // init dataholder
    DataHolder::init(); // dataholder holder handles scene management. It can have game specific data added to it(code yourself).
    DataHolder& g = DataHolder::god;

    // defaults
    unsigned int SCR_WIDTH = 800;
    unsigned int SCR_HEIGHT = 600;
    
    // load graphics settings if they exist
    std::string settingsFileName = "metadata/graphicsettings";

    if (util::fileExists(settingsFileName))
    {
        // read file and split by line
        // - note: util::split handles Windows (\r\n) and Unix (\n) line endings automatically. Look this up if you don't know it.
        std::cout << "file exists and read init\n";
        std::vector<std::string> lines = util::split(util::readFile(settingsFileName),"\n");

        // handle each line
        for (std::string& line : lines) // it's ok to not use const here because string is not reused and is not a literal
        {
            util::removeComments(line, "#");
            util::removeComments(line, "//");
            std::vector<std::string> data = util::split(line, ":");
            //std::cout << line << "\n";
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
        if (g.uncategorizedData.contains("SCR_WIDTH"))
        {
            SCR_WIDTH = g.getUnCatData<int>("SCR_WIDTH");

            // can also do this but it involves manual converstion with any_cast
            // - SCR_WIDTH = std::any_cast<int>(g.uncategorizedData["SCR_WIDTH"]);
        }
        // using struct method instead of c++ method to show anoher to check if key exists
        if (g.checkKeyUnCatData("SCR_HEIGHT"))
        {
            SCR_HEIGHT = g.getUnCatData<int>("SCR_HEIGHT");
        }
        // if either of the above two things fail to check, it should use the default values of SCR_WIDTH and SCR_HEIGHT
    }
    else
    {
        std::ostringstream write;
        write << "SCR_WIDTH: " << SCR_WIDTH << "\n";
        write << "SCR_HEIGHT: " << SCR_HEIGHT << "\n";
        if(util::writeFile("metadata/graphicsettings", write.str()))
        {
            std::cout << "graphic settings defaulted\n";
        }
        else
        {
            std::cout << "fail to write graphic settings\n";
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
    StaticAudio::init();

    // replace MainMenu if you write your own.
    // cleanest way to implement a new game is to modify Main menu instead of replacing it.
    // main menu is located at assets/gameSpecific/scene/mainMenu.h because it should be modified and should be different for most builds.
    DataHolder::SceneQueue(new MainMenu, false);

    StaticDraw::init();
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

        //g.SceneUpdate(); // S is capitalized because it is static function
        DataHolder::SceneUpdate();

        //std::cout << "framerate: " << 1 / deltaTime << "\n";

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
public:
    static void run()
    {
        mainmainmain();
    }
};