# BGL
BGL is an ultra minimalist OpenGL based game engine. It is set up to be 2d but can be customized.

## Philosophy
BGL is minimalistic and developer controlled. Minimalism and developer control are the highest priority of the engine.
A problem with other engines is that they are over engineered and awkward to work with. This game engine is on the opposite side of the spectrum intentionally. The intended user for this game engine is somebody who has a history of fighting game engines or wants to self implement things instead of going through tutorial hell.This engine does the bare minimum to be functional. The coding and architectural learning curve is significantly higher than other game engines but the learning curve to learn the engine should be lower due to minimalism. Other game engines have their own developer tools/IDEs. This game engine has to be compiled from an IDE such as visual studio or CLion and does not have a GUI because minimalism and scope.

In addition to minimalism, the engine also has a focus on user control. The code is open source. Core functionalities are handled by classes inside of the "singleton" folder. Changing core game engine code is highly recommended. Customization is a good thing. 

## Hierarchy
BGL is minimalistic. There is no implemented scene hierarchy. The engine only tracks one main scene at a time. Each scene is its own self contained entity. Although scene hierarchy is not supported it is possible for scenes to contain other scenes. It is possible to have a main scene that calls the handle and render method on other scenes that are attached to it. Scene hierarchy is possible and encouraged, it just is not supported. Scene hierarchy is not a forced feature because of philosophy.

GameObjects do not exist. Most of everything is self implemented. A different game engine may have GameObject attached to scene and the GameObject would have a script attached to it. The forced existence of GameObjects goes against this engines philosophy, it is over engineered, and it is completely unnecessary. Instead of having GameObjects as a forced object, the user can make it if they think it is a valid solution. 

## Running on different operating systems/environments
This project is open and source and the author loves to share. The author is the main target audience. I am one person. The game engine has primary support for whichever operating system the author of the tool is currently using. Setting up the engine to work on other operating systems would require updating "CMakeLists.txt" and creating equivalent files from windows/linux folders to the new operating system. 

## Notable Classes/Structs
### Scene
Scenes contain and process game information. The engine relies on polymorphism, and custom scenes that inherit from the Scene struct/class are the main building blocks of the engine. The scene methods are intended to be overridden by scenes that inherit from it.

"onLoad()" runs automatically when a scene is loaded. It is intended for loading resources and setting default variables. Calling the base implementation is recommended because it sets the window reference required by the engine. The engine will crash if window reference is not set.

"clean()" is called before a scene is deleted. It is intended for unloading resources the scene loaded. There are exceptions to clean not being run if a scene is deleted in a non-standard way. Non-standard deletion is allowed because of philosophy.

"handle(float time = 0)" runs every game step and is intended for game logic.

"render(float time = 0, bool updateDisplay = true)" runs every frame and is intended for rendering.

"aspectChange()" should be called whenever the screen size changes. It is also good practice to manually call it once in "onLoad()". Lazy UI/text generation implementation calls aspectChange to remove distortions.

"processInput(float time = 0, GLFWwindow* ww = nullptr)" is manually called. It is intended to be called from render manually and to move input handling to another window.

In addition to inherited methods, derived scenes should create their own variables and methods scene-specific functionality.

[Froghop.h](https://github.com/swillims/BGL/blob/main/classes/gameSpecific/scene/frogHop.h) and [Froghop.cpp](https://github.com/swillims/BGL/blob/main/classes/gameSpecific/scene/frogHop.cpp) are files to study to learn the intended way to implement a game scene for a simple 2d game.

### singleton/
#### DataHolder
DataHolder holds the current scene and it also holds the next scene if one is queued.

It has variables to regulate gamestep and frame rate.

"ChangeScene(Scene* s, bool clean = true)" Instantly changes scene. It is the direct way to change scenes. "s" is the next scene. "clean" is whether or not c++ is going to delete previous scene.

"SceneQueue(Scene* s, bool clean = true);" Changes scene at end of cycle. Is the normal way to change scenes. "s" is the next scene. "clean" is whether or not c++ is going to delete previous scene.

"SetPhysicCap(float cap)" changes rate at which handle runs 

"SetFrameCap(float cap)" changes rate at which render runs

"DelayDelete(void* trash)" deletes things at end of cycle

"SetUnCatData(string key, any data)" and related methods provide a global key-value storage system. This system is flexible but less type-safe than creating a dedicated structure. It is an option. It is not a good option.

#### StaticDraw
StaticDraw handles graphics functionality. It provides simplified wrappers for shaders, textures, VAOs, and rendering methods.

"windowImply()" sets the OpenGL window reference using the currently active GLFW window.

"windowSpecify(GLFWwindow* win)" manually sets the OpenGL window reference using a pointer.

"resize(int newW, int newH)" changes the window size.

"updateView()" updates stored window width, height, and aspect ratio information.

"cleanIndices()" Sets a few things to default.

"init()" initializes the graphics system, loads the default shader, creates the default VAO/VBO/EBO, and sets up the OpenGL state used by the engine.

"compileShader(const char* vertexSource, const char* fragmentSource, std::string shaderName = "")" loads and compiles OpenGL shader programs. It requires a filename for the vertex shader and fragment shader sources. It also accepts an optional name for the shader program to be referenced by.

"useShader(unsigned int shaderRef)" changes the active shader program to a shader program using a reference int.

"useShaderSimple()" switches to the default shader.

"getShader(t)" retrieves a shader reference by name or retrieves a shader name by reference.

"hasShader(t)" checks whether a shader exists by name or reference.

"unLoadShader(t)" removes a shader reference from "shaderRefs".

"printShaderUniforms()" prints all active uniforms in a shader program. This is a debugging tool. It should be deleted because it is for an older version.

"checkCompileErrors(unsigned int shader, const std::string& type)" is another debug tool.

"CreateVAO(const std::vector<int>& attributes, const std::string& vaoString)" creates custom vertex array objects. Attributes are specified as a list of float counts per vertex attribute. Example: {2, 2} could represent position(x,y) and texture(u,v) coordinate attributes. It also has a string reference for name.

"getVAO(t)" retrieves VAO information by name or reference.

"hasVAO(t)" checks whether a VAO exists by name or reference.

"deleteVAO(t)" deletes a VAO by name or reference.

"loadImage(std::string fileName, std::string imageName = "", bool flip = false)" loads an image into OpenGL as a texture. Loaded textures are stored in "imageFileRefs" and can be accessed using either a name or reference. It requires a filename to load from. It has an optional name it loads as and an optional flip to load textures upside down.

"unLoadImage(t)" removes a loaded texture from OpenGL.

"loadFolder(const std::string& folderPath)" loads every image in a folder. The file path is used as the default reference name. This should work but is not recommended or tested.

"halfDimImage(int imageRef, float xCenter, float yCenter, float halfWidth, float halfHeight)" draws an image centered around a position with a specified half width and half height. This is used for simple draws. Batch drawing is better.

"spriteImage(int imageRef, float xCenter, float yCenter, float halfWidth, float halfHeight, int frameX, int frameY, int xFrames, int yFrames)" draws a single frame from a sprite sheet. The frame position and sprite sheet dimensions are used to calculate texture coordinates. This is used for simple draws. Batch drawing is better.

"fractionImage(int imageRef, float xCenter, float yCenter, float halfWidth, float halfHeight, float repeatX, float repeatY)" draws an image using fractional texture coordinates. It can be used for repeating textures or custom texture regions. This is used for simple draws. Batch drawing is better.

"backGroundImageRepeat(int imageRef, float repeat)" draws a repeating background image that automatically adjusts for aspect ratio.

"multiDraw(int imageRef, const std::vector<float>& vertices, GLuint vao = VAOSimple, unsigned floatCount = 4)" is used to draw images from batches. It has optional parameters to change the VAO and number floats used by the VAO. 

"clear(t)" clears the screen with a specified color.

#### StaticAudio
StaticAudio handles audio functionality. It provides wrappers around miniaudio for loading sounds, playing sounds, looping sounds, and controlling volume.

"init()" initializes the audio engine. It needs to be called one time before using audio functionality.

"playSoundFromFile(const char* path)" directly plays a sound file. It does not store the sound reference and is intended for simple playback. This was originally meant as a debug tool but manually playing from the folder(not code) is probably a better way to debug if sound file are valid.

"playSoundEffect(t)" plays a loaded sound effect. It resets the sound position before playing. It does not allow multiple copies of the same sound to play at the same time.

"playSoundEffectMulti(t)" plays a loaded sound effect while allowing multiple copies of the same sound to play simultaneously. It creates a temporary copy of the sound and automatically deletes it after playback finishes.

"playSoundLoop(t)" starts a loaded sound and sets it to loop. It is intended for music or ambient sounds.

"stopSound(int ref)" stops a loaded sound. It does not affect sounds created using "playSoundEffectMulti()".

"createTag(const std::string& tag)" creates an audio tag. Tags are used to group sounds together for shared volume control. As an example: music can share a tag and soundEffects can share a tag.

"applyTags(t, const std::vector<std::string>& tags)" applies one or more volume tags to a loaded sound. Multiple tags can affect the same sound.

"setMasterVollume(float v, bool power = true)" changes the master volume. The value is clamped between 0 and 1. If power is enabled, nonlinear volume scaling is applied because human hearing is logarithmic instead of linear.

"updateTagVollume(int/string tag, float v, bool power = true)" changes the volume setting of a tag. The tag affects every sound assigned to it. If power is enabled, nonlinear volume scaling is applied because human hearing is logarithmic instead of linear.

"updateSoundVollume(int soundRef, bool power = true)" recalculates the volume of a sound using its assigned tags. If power is enabled, nonlinear volume scaling is applied because human hearing is logarithmic instead of linear.

"updateSounds(bool power = true)" updates the volume of every loaded sound. If power is enabled, nonlinear volume scaling is applied because human hearing is logarithmic instead of linear.

"load(const char* path, std::string name = "", std::vector<std::string> tags = {})" loads a sound file into memory. It can optionally assign a custom reference name and apply tags.

"unLoad(t)" unloads a sound.

#### StaticWrite
Handles text generation. StaticWrite handles text generation. Text rendering is significantly more complicated than drawing sprites, so it uses its own rendering pipeline. It has capital and lowercase versions of methods because it makes sense to save more than one font. Lower case refers to one instance and capital refers to whichever instance is loaded as the global singleton. Just using global is fine and intended. Multiple instance/font functionality is not tested or supported.

"Init(const std::string& font_name, unsigned int size, bool heapAllocate = true)" initializes StaticWrite. It creates the StaticWrite instance and loads the specified font. If heapAllocate is true, StaticWrite sets "singleton" to a new heap allocated object. It does not delete the old one if one exists. There is no multiple font support. "If it works it works." The engine is customizable.

"init(const std::string& font_name, unsigned int texSize)" initializes an existing StaticWrite instance. It loads the shader, generates the font atlas, creates the default text channel, and sets up the VAO/VBO used for rendering.

"StartWrite()" sets up the renderer to handle text generation. It sets shader, binds the text vertex buffer, and binds the font texture atlas. TLDR; text generation is complicated and needs a single-channel Atlas for characters and special shader that can handle single-channel characters.

"DrawChannel(unsigned int channel, glm::vec3 color)" draws all text vertices stored in a channel using the specified color. There is not a version with vec4 instead of vec3, so for transparent characters, the user will have to implement their own method.

"GenerateVertices(const std::string& text, float x, float y, float xScale, float yScale)" generates vertex data for text without storing it in a channel. It returns a vector of vertices containing position and texture coordinate data.

"AppendChannel(unsigned int channel, const std::vector<float>& vertices)" appends pre-generated vertex data to an existing text channel.

"AppendText(unsigned int channel, const std::string& text, float x, float y, float xScale, float yScale)" generates text vertices and appends them directly to an existing text channel.

"SetUpChannel(unsigned int channel)" creates or resets a text channel.(sets it to empty)

"DestroyChannels()" removes all currently stored text channels. This is fine because SetUpChannel works even if the channel does npt exist(see previous definiton)

"generateAtlas(const std::string& fontPath)" is called automatically. Manually invoking it is usually unnecessary. It creates a font texture atlas. The font for the path is the specified font.

#### StaticInput
StaticInput handles keyboard and mouse input. It tracks key/button states, provides helper functions for querying input, supports aliases, converts between key names and GLFW key codes, and retrieves mouse position. Capitalized methods operate on the global singleton while lowercase methods operate on a specific instance. Multiple instances/windows is not tested. StaticInput simplifies things and should be used but it is optional. GLFW calls are pretty simple and StaticInput handles GLFW inputs.

"Init()" initializes StaticInput and stores the current GLFW window reference.

"GetMouse(double& x, double& y, bool normalize = true)" and "GetMouseF(float& x, float& y, bool normalize = true)" retrieve the current mouse position. If normalize is true, the returned coordinates are converted to normalized device coordinates in the range [-1, 1].

"Tick(bool keyboard = true, bool mouse = true)" updates the tracked keyboard and mouse states. It determines which tracked keys/buttons were clicked, released, or are currently held. This should normally be called once per game step in processInput. Note: ProcessInput is from scene has to be manually called(preferably in handle)

"KeyTrack(t, bool positive = true)" begins or stops tracking a keyboard key. Keys may be specified either by integer GLFW key code or by string.

"KeyUntrack(t)" stops tracking a keyboard key.

"KeyTrackSetAll(bool on = true)" enables or disables tracking for every keyboard key.

"KeyClick(t)" returns whether a tracked key was pressed during the current update. [Off->On]

"KeyHeld(t)" returns whether a tracked key is currently being held. [On]

"KeyRelease(t)" returns whether a tracked key was released during the current update. [On->Off]

"MouseTrack(t, bool positive = true)" begins or stops tracking a mouse button. Buttons may be specified either by integer GLFW button code or by string.

"MouseUntrack(t)" stops tracking a mouse button.

"mouseTrackSetAll(bool on = true)" enables or disables tracking for every mouse button. (There is currently no capitalized wrapper for this method.)

"MouseClick(t)" returns whether a tracked mouse button was pressed during the current update.

"MouseHeld(t)" returns whether a tracked mouse button is currently being held.

"MouseRelease(t)" returns whether a tracked mouse button was released during the current update.

"AssignAlias(const std::string& name, t)" assigns a custom alias to a key or mouse button. The reference may be specified as either a string or integer. This is used for custom keybinds.

"HasAlias(const std::string& name)" checks whether an alias exists.

"ClearAllAlias()" removes every stored alias.

"GetAlias(t)" retrieves the integer key or mouse button associated with an alias.

"GetStringAlias(std::string ref)" retrieves the string representation of an alias.

"GetTrackedKeys()" returns a vector containing every currently tracked keyboard key.

"StringToInt(const std::string& input)" converts a key or mouse button name into its corresponding GLFW integer value. Single-character strings are automatically converted to their ASCII values. If no mapping exists, -1 is returned.

"IntToString(unsigned int input)" converts a GLFW key or mouse button value back into its string representation. Printable ASCII characters are returned directly. If no mapping exists, an error string is returned.

"keyToChar(int key, bool shift)" converts a GLFW keyboard key into the corresponding printable character, taking the Shift key into account. Keys without printable characters return '\0'.

### util
I'm not writing this right now because I add to it a lot.
