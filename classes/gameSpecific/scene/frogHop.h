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

	//controls
	unsigned int rotateCC;
	unsigned int rotateC;
	unsigned int hopButton;
	unsigned int pauseButton;

	// vertice storing
	std::vector<float> batch;

	// scale values for square rendering
	float yScale;
	float xScale;

	// technical things
	bool alreadyLoaded = false;
	bool resizing;

	// offset before shifting and shifting
	float visualOffset;
	unsigned int shift;

	// world vars
	// - gameSpeed
	float scrollSpeed = 1.25f;

	// - map array and size params
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
	bool wHeld;
	float jumpSpeed;

	// meta variables
	// - I'm not going to save score because this is just a demo and the option menus show how to save things
	float score;
	float scoreScale;

	void onLoad();

	void jump();

	void pause();

	void processInput(GLFWwindow* window, float time);

	void handle(float time = 0);

	void render(float time = 0, bool updateDisplay = true);

	void aspectChange();

	void clean();
};
