#pragma once

#include <GLFW/glfw3.h>
#include <vector>

//#include <string>
//#include <string>
//#include <glm/glm.hpp>

struct Scene
{
public:
	GLFWwindow* window = nullptr;
	// run on scene change
	virtual void onLoad(){ window = glfwGetCurrentContext(); } 
	// run one per gameplay frame
	virtual void handle(float time = 0){} 
	// run once per graphic frame and returns a TextShell list to be displayed on top layer
	virtual void render(float time = 0, bool updateDisplay = true) { if (updateDisplay) { glfwSwapBuffers(window); } } 
	// processInput is manually invoked and handles inputs
	virtual void processInput(float time = 0, GLFWwindow* ww = nullptr) { if (ww == nullptr) ww = window; }
	// run on screen stretch(assumed manual screen size change)
	virtual void aspectChange() {} 
	// used to clean up assets
	virtual void clean() { };
};