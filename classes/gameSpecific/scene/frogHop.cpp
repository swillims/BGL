#include "frogHop.h"

#include "singleton/dataHolder.h"
#include "singleton/staticDraw.h"
#include "singleton/staticSound.h"
#include "singleton/staticWrite.h"
#include "singleton/staticInput.h"
#include <random>
#include "util.h"
#include "vectorFUtil.h"
#include "defeatScreen.h"
#include "pauseMenu.h"

#include "keyOptions.h"

StaticWrite* writer;

// load assets
void FrogHop::onLoad()
{
	// inherited from scene

	/*
	The intended way to load assets is in the onLoad function.
	The suggested way to load assets is by checking if the class that handles it has it and then load it if doesn't have it.
	After verifying it exists or creating it, store it with a variable.
	This pattern avoids/reduces dataleaks.
	In addition to loading assets, there are other tasks that also need to be handled in onLoad.
	Set/reset variables that haven't been set yet
	Initialize things
	*/

	// extremely important to call super at start of onLoad to declare window
	Scene::onLoad();
	
	// load shaders
	shaderSimpleRef = StaticDraw::getShader("simple");
	if (!StaticDraw::hasShader("simpleRotation"))
	{
		StaticDraw::compileShader("assets/shaders/rotation.vs", "assets/shaders/simple.fs", "simpleRotation");
	}
	shaderRotationRef = StaticDraw::getShader("simpleRotation");

	// load iamges
	if (!StaticDraw::imageFileRefs.contains("frog"))
	{
		StaticDraw::loadImage("assets/gameSpecific/png/frogHop/frogsprite1x2.png", "frog");
	}
	frogImage = StaticDraw::imageFileRefs["frog"];
	if (!StaticDraw::imageFileRefs.contains("frogBlock"))
	{
		StaticDraw::loadImage("assets/gameSpecific/png/frogHop/block.png", "frogBlock");
	}
	block = StaticDraw::imageFileRefs["frogBlock"];
	if (!StaticDraw::imageFileRefs.contains("spike"))
	{
		StaticDraw::loadImage("assets/gameSpecific/png/frogHop/spike.png", "spike");
	}
	spike = StaticDraw::imageFileRefs["spike"];

	// load sounds
	if (!StaticAudio::soundStringRefs.contains("hopChirp"))
	{
		StaticAudio::load("assets/gameSpecific/sound/hopChirp.wav", "hopChirp", { "soundEffect" });
	}
	hopSound = StaticAudio::soundStringRefs["hopChirp"];

	if (!StaticAudio::soundStringRefs.contains("frogMusic"))
	{
		StaticAudio::load("assets/gameSpecific/sound/225simplebass.wav", "frogMusic", { "music" });
	}
	frogMusic = StaticAudio::soundStringRefs["frogMusic"];

	StaticAudio::updateSounds();

	// Load Controls

	// to get controls working, the class needs key bindings.
	// - to make it possible to rebind keys, alias names are used.
	// -- The Keybinding alias names are specified in keyOptions.h
	// --- copy/pasting and maintaining two or more copies of the same string literal is not scalable for refactoring
	// ---- This tutorial is going to reference the names of the aliases by pulling them from one location(KeyOptions)

	// create a temp class that is not heap allocated and will go away when it goes out of scope
	KeyOptions keyOptions;
	// KeyOptions Class specifically needs a reference scene to function so we feed it a reference
	keyOptions.previous = this;
	// KeyOptions is also a scene. Its onLoad method sets up the needed aliases if they don't exist.
	keyOptions.onLoad();

	// For each control,
	// - Reference the name of the alias where they are defined at(KeyOptions)
	// - Load the controls from StaticInput using the aliases
	rotateC = StaticInput::GetAlias(keyOptions.eTitle);
	rotateCC = StaticInput::GetAlias(keyOptions.qTitle);
	hopButton = StaticInput::GetAlias(keyOptions.wTitle);
	pauseButton = StaticInput::GetAlias(keyOptions.escTitle);

	// not tracking unused keys is a mild optimization.
	// - It would be a bigger optimization if glfw was instead of a wrapper class but this is a tutorial.
	StaticInput::KeyTrackSetAll(false);

	// track keys we need
	StaticInput::KeyTrack(rotateC);
	StaticInput::KeyTrack(rotateCC);
	StaticInput::KeyTrack(hopButton);
	StaticInput::KeyTrack(pauseButton);

	// set up text rendering for scene
	// Assumption: StaticWrite::Init should have been ran somewhere else such as main menu
	// Requirements - startWrite/StartWrite needs to be ran every time before writing text to set shader to text shader
	//              - somewhere in the pipeline after writing text, the shader needs to be set back to whatever the default shader is for the scene.
	//              - - This scene sets is to simple(the default shader) at the start of render
	writer = StaticWrite::singleton;

	if (not alreadyLoaded)
	{
		// code to clear the map. If an instance of this scene is reused(see defeatScrean) the blocks need to be reset.
		alreadyLoaded = true;
		for (int i = 0; i < width * height; i++)
		{
			blocks[i] = 0;
		}
		for (int i = 0; i < width; i++)
		{
			blocks[i * height] = 1;
			blocks[(i * height) + height - 1] = 1;
		}

		// variables that shouldn't be reloaded
		frogX = frogPhysicX = width / 2;
		frogY = frogPhysicY = 2;
		frogVelocityX = frogVelocityY = 0;
		hopTimer = 0;
		visualOffset = 0.0f;
		shift = 0;
		frogAngleDisplay = frogAngle = 0;
		yScale = 1.0f / height;
		chance = 0.3f;
		currentPath = height / 2;
		pathCount = 3; // DO NOT SET THIS TO 0, IT WILL UNDERFLOW IN "handle(...)"
		minPathCount = 2;
		maxPathCount = 3;
		assumePhysics = false;
		gravity = 3.25f;
		bounceValue = -.5f; // negative because go other way when bouncing
		frogRadius = 0.25f;
		frogRotationSpeed = 1.75;
		resizing = false;
		hopTimerCap = .5f;
		jumpSpeed = 5;
		scoreScale = 5.0f;
		score = 0;
	}

	// set music
	StaticAudio::playSoundLoop(frogMusic);

	// set physics framerate to 60
	DataHolder::SetPhysicsCap(60);

	// it is good practice to aspectChange whenever loading a scene.
	// - aspectChange() is only called when the aspect ratio is changed or when it is manually called. It is manually called here.
	// - This scene uses it to handle text generation
	aspectChange();
}

void FrogHop::jump() // method for jumping
{
	if (hopTimer < 0)
	{
		hopTimer = hopTimerCap;
		if (true)
		{
			StaticAudio::playSoundEffectMulti(hopSound);
			frogVelocityX *= .2f;
			frogVelocityY *= .2f;
			frogVelocityX += jumpSpeed * sin(frogAngle);
			frogVelocityY += jumpSpeed * cos(frogAngle);
		}
	}
}

void FrogHop::pause()
{
	// create a pause menu object and use the constructor to set this scene as "previous"
	PauseMenu* menu = new PauseMenu(this);
	// Set pause menu has a function for setting text channel.
	// - The text channel needs to be a number not used by this scene or it will draw this scenes text over the manu.
	menu->setTextChannel(-1111);
	// Change scenes to menu and set clean to false to avoid deleting this scene
	DataHolder::SceneQueue(menu, false);
}

// process input is part of scene class
void FrogHop::processInput(GLFWwindow* window, float time) // control inputs
{
	if (StaticInput::KeyHeld(rotateC)){frogAngle += time * frogRotationSpeed;}
	if (StaticInput::KeyHeld(rotateCC)){frogAngle -= time * frogRotationSpeed;}
	if (StaticInput::KeyClick(hopButton)){jump();}
	if (StaticInput::KeyHeld(pauseButton)){pause();}
}

// the game logic part of this example code is going to have low documentation because the main focus is how to interact with Game Engine systems.
void FrogHop::handle(float time)
{
	/*
	This function will have low comments because the goal is to be an educational demo and explaining it here would be optimal.
	Handle should handle game logic and physics and render should handle graphics.
	The engine is minimalistic and most logic is contained to the scene.
	*/

	StaticInput::Tick();

	processInput(window, time);

	// assume physics is used by render to approximate movement independent of game logic. It is set to false here because there is no reason to approximate on a game step.
	assumePhysics = false;
	if (hopTimer >= 0) { hopTimer -= time; }

	visualOffset += scrollSpeed * time;
	score += time * scrollSpeed * scoreScale;

	frogAngleDisplay = frogAngle;
	// make the game move
	if (visualOffset > 1)
	{
		visualOffset -= 1;
		std::random_device rd;
		std::mt19937 gen(rd());
		for (int i = 0; i < height; i++)
		{
			int n = i % height;
			if (i == 0 || i == height - 1)
			{
				blocks[height * shift + i] = 1;
				continue;
			}
			std::bernoulli_distribution coin(chance);
			if (coin(gen))
			{
				blocks[height * shift + i] = 1;
				continue;
			}
			blocks[height * shift + i] = 0;
		}
		blocks[height * shift + currentPath] = 0;
		pathCount--;
		if (pathCount == 0)
		{
			std::uniform_int_distribution<int> dist(minPathCount, maxPathCount);
			pathCount = dist(gen);
			std::uniform_int_distribution<int> dist2(1, height - 2);
			int newPath = dist2(gen);
			int start = util::compareMin<unsigned int>(newPath, currentPath);
			int end = util::compareMax<unsigned int>(newPath, currentPath);
			currentPath = newPath;
			for (int i = start; i <= end; ++i)
			{
				blocks[height * shift + i] = 0;
			}
		}
		shift++;
		frogPhysicX -= 1.0f;
		if (shift >= width)
		{
			shift = 0;
		}
	}

	bool checkCorner = true;

	frogVelocityY -= gravity * time;
	float deltaY = frogVelocityY * time;
	int nextY = int(frogPhysicY + deltaY + std::copysign(frogRadius, frogVelocityY));
	float adjustedFrogX = frogPhysicX + shift;// +.5f;
	if (blocks[util::get1DIndexWrap(height, width, nextY, int(adjustedFrogX))] == 1)
	{
		frogVelocityY *= bounceValue;
		deltaY *= bounceValue;
		checkCorner = false;
	}
	float deltaX = frogVelocityX * time;
	int nextX = int(adjustedFrogX + deltaX + std::copysign(frogRadius, frogVelocityX));
	int frogRow = int(frogPhysicY);
	if (blocks[util::get1DIndexWrap(height, width, frogRow, nextX)] == 1)
	{
		frogVelocityX *= bounceValue;
		deltaX *= bounceValue;
		checkCorner = false;
	}
	if (checkCorner)
	{
		int x = std::floor(frogPhysicX);
		int y = std::floor(frogPhysicY);
		for (int i = x-1; i <= x + 1; i+=2)
		{
			for (int j = y-1; j <= y + 1; j+=2)
			{
				if (blocks[util::get1DIndexWrap(height, width, j, i + shift)] == 1)
				{
					float difX = frogPhysicX - x > .5f ? frogPhysicX + deltaX - i : frogPhysicX + deltaX - i -1.0f;
					float difY = frogPhysicY - y > .5f ? frogPhysicY + deltaY - j : frogPhysicY + deltaY - j - 1.0f;
					if (frogRadius * frogRadius > difX * difX + difY * difY)
					{
						float temp = frogVelocityY;
						frogVelocityX = std::copysign(frogVelocityX, difX) * std::abs(bounceValue);
						frogVelocityY = std::copysign(temp, difY) * std::abs(bounceValue);
						deltaY = frogVelocityY * time;
						deltaX = frogVelocityX * time;
					}
				}
			}
		}
	}

	//with square hitboxes, it is better to adjust 1 before checking the other. These are circles. Circles are not the same as squares.
	frogX = frogPhysicX += deltaX;
	frogY = frogPhysicY += deltaY;

	// defeat conditions
	if (frogPhysicX - frogRadius < .3f + visualOffset || frogPhysicX + frogRadius > width + visualOffset - 2.3f) // .3 seemed like a good amount of overlap to be fair
	{
		// onLoad uses already loaded and setting to false enables reloading the level.
		alreadyLoaded = false;
		DefeatScreen* next = new DefeatScreen();
		next->previous = this;
		DataHolder::SceneQueue(next, false);
	}
}

void FrogHop::render(float time, bool updateDisplay)
{
	// inherited from scene
	if (assumePhysics)
	{
		frogX += frogVelocityX * time;
		frogY += frogVelocityY * time;
	}
	else
	{
		assumePhysics = true;
	}
	StaticDraw::useShader(shaderSimpleRef);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
		
	int column = 0;
	batch.clear();
	// bad because it draws things not on screen.
	for (unsigned int i = 0; i < arrSize; i++)
	{
		if (i % height == 0)
		{
			column = i / height; // rounds down to nearest column
			if (column < shift) // if column is less than index representing the 0 in circular array
			{
				column += width; // wrap it around by adding width(total column)
			}
		}
			
		if (blocks[i] == 1) // not empty
		{
			int y = i % height; // remainder height is height in abstacted 1d array
			y -= height / 2;
			float yf = y * 2.0f / height;
			float xf = (column - shift - frogX) * xScale * 2;
			float xMax = xf + xScale * 2; // rifht side of square
			float xMin = xf; // left side of square
			float yMax = yf + yScale * 2; // top of square
			float yMin = yf; // bottom of square

			// add quad to batch.
			// A quad is a name for two triangles that form a rectangle.
			// The 4 vertices are read as 6 points(3 per triangle).
			// The left two numbers are x,y cordinates.
			// The right two numbers are UV cordinates. UV are texture locations.
			
			//*
			batch.insert(batch.end(),
				{
				xMax,  yMax, 1.0f, 1.0f, // v0
				xMax,  yMin, 1.0f, 0.0f, // v1
				xMin,  yMin, 0.0f, 0.0f, // v2

				xMax,  yMax, 1.0f, 1.0f, // v0
				xMin,  yMin, 0.0f, 0.0f, // v2
				xMin,  yMax, 0.0f, 1.0f, // v3
				});
			//*/
			// there is another way to append to batch useing help function vectorFUtil::getVectorFromCornor
			// - That way is done for spike drawing. It less efficient than manually calculating vertexs.
			// -- Not relevenat but the helper functio also doesn't directly support sprites
			// this works and is easier than batching but is suboptimal performance due to not batching
			//float xCenter = (column - shift - frogX) * xScale * 2;
			//float yCenter = ((i % height) - height / 2 + 0.5f) * 2.0f / height;
			//StaticDraw::halfDimImage(block, xf, yf, xScale, yScale); 
		}
	}
	StaticDraw::multiDraw(block,batch); // batch drawing StaticDraw call

	// draw frog after blocks because it is a layer above blocks
	float frogGraphicY = frogY - height / 2; // prescaled : turn 0 to n to -1 to n 
	frogGraphicY = ((frogGraphicY) * 2.0f / height); // apply scaling and normalizing etc. spriteRotatedImage
	// calculate frame	
	float frame = 0;
	if (hopTimer > 0) { frame += 1; } // the sprite is only 1x2, so only 1 varaible is need for figuring out which frame to use.


	// use rotation shader
	glUseProgram(shaderRotationRef); // use correct shader for rotation
	// rotationn shader has 3 variables that are required. There is a rotation angle. There is a rotation center that is revolved around. There is an aspect ratio based on screen dimensions.
	// - 1 is obvious because it is required for rotation(radians).
	// - There is a center of rotation. Possibly, if you rework and implement your own solutions, you should consider moving draw location to shader to simplify draw stack.
	// - The aspect ratio is needed to prevent distortions as it rotates. OpenGL uses -1.0f,1.0f regardless of dimensions. Widening the screen/drawport causes a distortion because 1x1 doesn't match 4x3/etc.
	glUniform1f(glGetUniformLocation(shaderRotationRef, "u_rotation"), frogAngleDisplay);
	glUniform2f(glGetUniformLocation(shaderRotationRef, "rotation_center"), 0, frogGraphicY);
	glUniform1f(glGetUniformLocation(shaderRotationRef, "aspect_ratio"), StaticDraw::aspectRatio);

	StaticDraw::spriteImage(frogImage, 0, frogGraphicY, xScale, yScale, 0, frame, 1, 2); // draw 1 object using a regular StaticDraw methods. Rotation is handled by shader and not StaticDraw.

	glUseProgram(shaderSimpleRef); // reset to default shader when done

	// draw spikes
	std::vector<float> v;
	float xLMin = (visualOffset - frogX) * xScale * 2 - xScale; // left side of left spikes
	//float xLMax = (visualOffset - frogX) * xScale * 2 + xScale; // right side of left spike
	float xRMin = (visualOffset + width - frogX - 2) * xScale * 2 + xScale; // left side of right spikes
	//float xRMax = (visualOffset + width - frogX - 2) * xScale * 2 - xScale; // right side of right spikes
	float yMin;
	//float yMax;
	float twoX = xScale * 2;
	float twoY = yScale * 2;
	batch.clear(); // whether or not you code you clean batches before or after drawing is up to you
	for (int i = 0; i < height; i++)
	{
		int y = i; // 0 to height
		y -= height / 2; // convert to -x to x
		float yf = ((y + .5f) * 2.0f / height);	// convert to -1 to 1
		yMin = yf - yScale; // adjust to square bottom

		// add quads to batch.
		// A quad is a name for two triangles that form a rectangle.
		// The 4 vertices are read as 6 points(3 per triangle).
		// The left two numbers are x,y cordinates.
		// The right two numbers are UV cordinates. UV are texture locations.
		v = VectorFUtil::getVectorFromCornor(xLMin, yMin, twoX, twoY);
		batch.insert(batch.end(), v.begin(), v.end());
		v = VectorFUtil::getVectorFromCornor(xRMin, yMin, -twoX, twoY);
		batch.insert(batch.end(), v.begin(), v.end());
	}
	StaticDraw::multiDraw(spike, batch); // batch drawing StaticDraw call

	// set needed to use writing shader
	writer->startWrite();


	// First variable in drawChannel is the channel being drawn to
	// Second variable drawChannel is a color. RGB
	// - The text of channel 0 is defined in aspectChange()
	writer->drawChannel(0, glm::vec3(1.0f, 1.0f, 1.0f));

	StaticWrite::SetUpChannel(1);
	std::string scoreStr = std::to_string(static_cast<int>(score));
	StaticWrite::AppendText(1, "Score: " + scoreStr, .5f, .9f, xScale * .8, yScale * .8);
	writer->drawChannel(1, glm::vec3(1.0f, 1.0f, 1.0f));

	// call super
	Scene::render(time, updateDisplay);
}

void FrogHop::aspectChange()
{
	// inherited from scene
	resizing = true;
	DataHolder::god.physicsTick = 0;
	StaticDraw::updateView();
	
	xScale = yScale / StaticDraw::aspectRatio;

	StaticWrite::SetUpChannel(0); // clears channel 0
	std::string q = StaticInput::IntToString(rotateC);
	std::string e = StaticInput::IntToString(rotateCC);
	std::string w = StaticInput::IntToString(hopButton);
	std::string esc = StaticInput::IntToString(pauseButton);
	// The number 0 is arbituary. It is ok to use whatever number as long as it is a valid int and it is deliberate.
	StaticWrite::AppendText(0, "Controls:", -.95f, .9f, xScale * .8, yScale * .8); // add text to channel 0
	StaticWrite::AppendText(0, q + " or " + e + ": Rotate Frog", -.9f, .8f, xScale * .8, yScale * .8); // add text to channel 0
	StaticWrite::AppendText(0, w + ": Jump", -.9f, .7f, xScale * .8, yScale * .8); // add text to channel 0
	StaticWrite::AppendText(0, esc + ": Pause", -.9f, .6f, xScale * .8, yScale * .8);
}

void FrogHop::clean()
{
	// inherited from scene

	// when doing clean up, don't remove things that are used by other scenes. 
	// - It is unnessary loading and unloading.
	// It is valid to use string or int. Both are being used as an example.
	// It is significantly better to use string.

	// unload sounds
	StaticAudio::unLoad("hopChirp");
	StaticAudio::unLoad(frogMusic);

	// Unloading textures is the same as unloading sounds. Both string and int are valid.
	// It is better to use int. The example also using both string and int as example.

	// unload textures
	StaticDraw::unLoadImage("frog");
	StaticDraw::unLoadImage(spike);
	StaticDraw::unLoadImage("frogBlock");

	// set all keys to not be tracked
	// NOTE: mouse clicks are handled seperately from key clicks
	StaticInput::KeyTrackSetAll(false);
};
// */
