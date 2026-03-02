#include "frogHop.h"

#include "singleton/dataHolder.h"
#include "singleton/staticDraw.h"
#include "singleton/staticSound.h"
#include "singleton/staticWrite.h"
#include <array>
#include <random>
#include "util.h"
#include "vectorFUtil.h"
#include "defeatScreen.h"

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
		StaticDraw::loadImage("assets/gamespecific/png/froghop/frogsprite1x2.png", "frog");
	}
	frogImage = StaticDraw::imageFileRefs["frog"];
	if (!StaticDraw::imageFileRefs.contains("frogBlock"))
	{
		StaticDraw::loadImage("assets/gamespecific/png/froghop/block.png", "frogBlock");
	}
	block = StaticDraw::imageFileRefs["frogBlock"];
	if (!StaticDraw::imageFileRefs.contains("spike"))
	{
		StaticDraw::loadImage("assets/gamespecific/png/froghop/spike.png", "spike");
	}
	spike = StaticDraw::imageFileRefs["spike"];

	// load sounds
	if (!StaticAudio::soundStringRefs.contains("hopChirp"))
	{
		StaticAudio::load("assets/gameSpecific/sound/hopChirp.wav", "hopChirp", { "soundEffect" });
	}
	hopSound = StaticAudio::soundStringRefs["hopChirp"];
	//StaticAudio::applyTags(hopSound, { "soundEffect" });

	if (!StaticAudio::soundStringRefs.contains("frogMusic"))
	{
		//StaticAudio::load("assets/gameSpecific/sound/frogSong.wav", "frogMusic", { "music" });
		StaticAudio::load("assets/gameSpecific/sound/225simplebass.wav", "frogMusic", { "music" });
	}
	frogMusic = StaticAudio::soundStringRefs["frogMusic"];
	//StaticAudio::applyTags(frogMusic, { "music" });

	StaticAudio::updateSounds();

	// set up text rendering for scene
	// Assumption: StaticWrite::Init should have been ran somewhere else such as main menu
	// Requirements - startWrite/StartWrite needs to be ran every time before writing text to set shader to text shader
	//              - somewhere in the pipeline after writing text, the shader needs to be set back to whatever the default shader is for the scene.
	//              - - This scene sets is to simple(the default shader) at the start of render
	writer = StaticWrite::singleton;
	batch.clear();

	// code to clear the map. If an instance of this scene is reused(see defeatScrean) the blocks need to be reset.
	for (int i = 0; i < width * height; i++)
	{
		blocks[i] = 0;
	}
	for (int i = 0; i < width; i++)
	{
		blocks[i * height] = 1;
		blocks[(i * height) + height - 1] = 1;
	}

	// set variables to default
	// - the variables are messy and unsorted here. They are sorted in the .h file.
	yScale = 1.0f / height;
	visualOffset = 0.0f;
	shift = 0;
	chance = 0.3f;
	currentPath = height / 2;
	pathCount = 3; // DO NOT SET THIS TO 0, IT WILL UNDERFLOW IN "coid handle(...)"
	minPathCount = 2;
	maxPathCount = 3;
	frogX = frogPhysicX = width / 2;
	frogY = frogPhysicY = 2;
	assumePhysics = false;
	frogVelocityX = frogVelocityY = 0;
	gravity = 3.25f;
	bounceValue = -.5f; // negative because go other way when bouncing
	frogRadius = 0.25f;
	frogAngleDisplay = frogAngle = 0;
	frogRotationSpeed = 1;
	resizing = false;
	hopTimer = 0;
	hopTimerCap = .5f;
	jumpSpeed = 5;

	// set music
	StaticAudio::playSoundLoop(frogMusic);


	// set physics framerate to 60
	DataHolder::SetPhysicCap(60);

	// it is good practice to aspectChange whenever loading a scene.
	// - aspectChange() is only called when the aspect ratio is changed or when it is manually called. It is manually called here.
	// -- There are things aspectChange() fixes and it won't fix those things because it is called very rarily(assumption).
	// - This specific scene uses it to handle text generation as well. 
	aspectChange();
}

void FrogHop::jump() // method for jumping
{
	if (hopTimer < 0 && !sHeld)
	{
		hopTimer = hopTimerCap;
		if (true) // check if ground not implemented. Example file is not a complete game.
		{
			StaticAudio::playSoundEffectMulti(hopSound);
			frogVelocityX *= .2f;
			frogVelocityY *= .2f;
			frogVelocityX += jumpSpeed * sin(frogAngle);
			frogVelocityY += jumpSpeed * cos(frogAngle);
		}
	}
}

// process input is part of scene class
void FrogHop::processInput(GLFWwindow* window, float time) // control inputs
{
	// calling super does nothing here but it is done here to show how to use it.
	Scene::processInput();
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { frogAngle -= time * frogRotationSpeed; }
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { frogAngle += time * frogRotationSpeed; }
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { jump(); sHeld = true; }
	else { sHeld = false; }
}

// the game logic part of this example code is going to have low documentation because the main focus is how to interact with Game Engine systems.
void FrogHop::handle(float time)
{
	// inherited from scene

	/*
	This function will have low comments.
	The main goal of this class is to be an educational example on how to use game engine and api.
	The game engine is minimalistic. 
	With the philosophy of being minimalistic, most logic is internal to a scene and not an api call.
	It is necessary for graphics/sounds/text/helperfunction to use api calls.
	Most comments should be relating to graphics. Graphics should not be part of handle(). Graphics are part of render().
	This function is handle.
	*/

	if (resizing == true) { time = 0; resizing = false; } // while resizing the mainmain while loop stops(unrelated to this line), and because it stops, the time between frames can get massive can cause jumping through blocks if not zeroed.
	// call process input
	processInput(window, time);
	assumePhysics = false;
	if (hopTimer >= 0) { hopTimer -= time; }

	visualOffset += time / 2;
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

	frogX = frogPhysicX += deltaX;
	frogY = frogPhysicY += deltaY; //with square hitboxes, it is better to adjust 1 before checking the other. These are circles. Circles are not the same as squares.

	// defeat conditions
	if (frogPhysicX - frogRadius < .3f + visualOffset || frogPhysicX + frogRadius > width + visualOffset - 2.3f) // .3 seemed like a good amount of overlap to be fair
	{
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
		frogX += frogVelocityX * time; // smooth physics
		frogY += frogVelocityY * time; // smooth physics
	}
	else
	{
		assumePhysics = true;
	}
	StaticDraw::useShader(shaderSimpleRef);

	//glClearColor(0.2f, 0.35f, 0.85f, 0.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
		
	int column = 0;
	batch.clear(); // whether or not you code you clean batches before or after drawing is up to you
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

	//StaticDraw::spriteRotatedImage(frogImage, 0, frogGraphicY, frogAngleDisplay, xScale, yScale, 0, frame, 1, 2);

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
		//yMax = yf + yScale; // adjust to square top

		// add quads to batch.
		// A quad is a name for two triangles that form a rectangle.
		// The 4 vertices are read as 6 points(3 per triangle).
		// The left two numbers are x,y cordinates.
		// The right two numbers are UV cordinates. UV are texture locations.
		v = vectorFUtil::getVectorFromCornor(xLMin, yMin, twoX, twoY);
		batch.insert(batch.end(), v.begin(), v.end());
		v = vectorFUtil::getVectorFromCornor(xRMin, yMin, -twoX, twoY);
		batch.insert(batch.end(), v.begin(), v.end());
		//StaticDraw::halfDimImage(spike, xL, yf, xScale, yScale); // this works and is easier than batching but is suboptimal performance due to not batching
		//StaticDraw::halfDimImage(spike, xR, yf, -xScale, yScale); // this works and is easier than batching but is suboptimal performance due to not batching
	}
	StaticDraw::multiDraw(spike, batch); // batch drawing StaticDraw call

	// set needed to use writing shader
	writer->startWrite();

	// First variable in drawChannel is the channel being drawn to
	// Second variable drawChannel is a color. RGB
	// The contents/text of the channel aren't declared here
	// - The contents are are declared in aspectChange()
	writer->drawChannel(0, glm::vec3(1.0f, 1.0f, 1.0f));

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
	// The number 0 is arbituary. It is ok to use whatever number as long as it is a valid int and it is deliberate.
	StaticWrite::AppendText(0, "Controls:", -.95f, .9f, xScale * .8, yScale * .8); // add text to channel 0
	StaticWrite::AppendText(0, "Q or E: Rotate Frog", -.9f, .8f, xScale * .8, yScale * .8); // add text to channel 0
	StaticWrite::AppendText(0, "W: Jump", -.9f, .7f, xScale * .8, yScale * .8); // add text to channel 0
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
};
// */
