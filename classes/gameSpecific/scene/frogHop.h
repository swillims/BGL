#pragma once

//#include "glue/textShell.h"
#include <array>
#include "scene/scene.h"

struct FrogHop : Scene
{
	//images
	unsigned int frogImage;
	unsigned int block;
	unsigned int spike;

	//shader
	unsigned int shaderSimpleRef;
	unsigned int shaderRotationRef;

	//sound
	unsigned int hopSound;
	unsigned int frogMusic;

	// vertice storing
	std::vector<float> batch;

	// scale values for square rendering
	float yScale;
	float xScale;

	// is resizing? 
	bool resizing;

	// offset before shifting and shifting
	float visualOffset;
	unsigned int shift;

	// map array and size params
	static const unsigned int height = 8;
	static const unsigned int width = 32;
	unsigned int arrSize = height * width;
	int blocks[height * width];

	// path generation variables
	unsigned int currentPath;
	unsigned int pathCount;
	unsigned int minPathCount, maxPathCount;
	float chance;

	// frog physic data
	// - note: frog is not a struct becasue unnessary with a 1 to 1 relationship
	float frogX;
	float frogY;
	float gravity;
	float bounceValue;
	float frogRadius;
	float frogVelocityX;
	float frogVelocityY;
	float frogAngle;
	float frogRotationSpeed;

	// frog graphic data
	// - graphics are seperated from physic.
	// -- physics need a fixed framerate to be deterministic
	// -- graphics need a higher than physics framerate for smoother appearance while rendering
	float frogPhysicX;
	float frogPhysicY;
	bool assumePhysics; // bool to check if simulate physics on a different framerate seperate from actual physics
	float frogAngleDisplay;
	
	// hop related variables
	float hopTimerCap;
	float hopTimer;
	bool sHeld; // I think that is supposed to be wHeld(wHeld) but its not worth fixing
	float jumpSpeed;

	// method/function header
	// - this class uses method/functions headers in the .h file and definitions only in the .cpp file. It is unnessary but just one way to do things.
	void onLoad();

	void jump();

	void processInput(GLFWwindow* window, float time);

	void handle(float time = 0);

	void render(float time = 0, bool updateDisplay = true);

	void aspectChange();

	void clean();
};
