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
Scene's contain and process game info. The engine runs largely on polymorphism and custom scenes that inherit from the scene struct/class are the main building block of the engine. Most of the methods of scene are user implemented by things that inherit from it.
"onLoad()" runs automatically when a scene is loaded. It is good to load things and set default variables in this method. Calling super is recommended because the engine will crash if a window is not set and super sets window.
"clean()" is supposed to be run before a scene is being deleted. It is good to unload things in this method.
"handle()" runs every game step and is meant to handle game logic.
"render()" runs every frame and is meant to handle graphics.
"apsectChange()" should be run every time the screen size changes and it also good practice to manually call it one time in "onLoad()"
In addition to methods, it is good to add variables to classes that inherit scene to store information.

### Scene
Scenes contain and process game information. The engine relies on polymorphism, and custom scenes that inherit from the Scene struct/class are the main building blocks of the engine. The scene methods are intended to be overridden by scenes that inherit from it.
"onLoad()" runs automatically when a scene is loaded. It is intended for loading resources and setting default variables. Calling the base implementation is recommended because it sets the window reference required by the engine. The engine will crash if window reference is not set.
"clean()" is called before a scene is deleted. It is intended for unloading resources the scene loaded. There are exceptions to clean not being run if a scene is deleted in a non-standard way. Non-standard deletion is allowed because of philosophy.
"handle()" runs every game step and is intended for game logic.
"render()" runs every frame and is intended for rendering.
"aspectChange()" should be called whenever the screen size changes. It is also good practice to manually call it once in "onLoad()". Lazy UI/text generation implementation calls aspectChange to remove distortions.
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

#### DataHolder
DataHolder is the main engine state manager. It stores the current scene, manages scene changes, controls game and rendering timing, and handles deferred deletion.
"ChangeScene(Scene* s, bool clean = true)" immediately changes the active scene. "s" is the new scene. "clean" controls whether the previous scene is deleted. If a previous scene is deleted this way, it is a standard deletion and it does run the clean method for the scene.
"SceneQueue(Scene* s, bool clean = true)" is a safer way to change scenes. It passes its information to a deferred changeScene call. This is the safer method for changing scenes because it prevents deleting a scene while it is still processing.
"SetPhysicsCap(float cap)" changes how often handle() runs(see scene). Setting it to 0 removes the cap.
"SetFrameCap(float cap)" changes how often render() runs(see scene). Setting it to 0 removes the cap.
"DelayDelete(void* ptr)" provides deferred deletion. pointers to deleted objects are stored and destroyed after scene handling/rendering completes to prevent objects being deleted while still in use.
"SetUnCatData(string key, any data)" and related methods provide a global key-value storage system. This system is flexible but less type-safe than creating a dedicated structure. It is an option. It is not a good option.

#### StaticDraw
Handles graphics.
Is currently under review.

#### StaticSound
"init()" needs to be ran one time to init sound engine
"playSoundEffect(int/string ref)" clean way to play sounds but does not allow duplicate sounds
"playSoundEffectMulti(int/string ref)" plays sound effect using threading. It allows duplicate sounds.
"playSoundLoop(int/string ref)" plays sound effect looping. Can be used for music or ambient noise. If you make an actual game, it may be better to implement a music player with multiple songs instead of using this.
"stopSound(int ref)" stops sound from playing. Does not work on sounds played with multi.
"applyTags(int/string target, const std::vector<std::string>& tags)" applies tag(s) to a sound. Sounds have tags that are mainly used for sound settings. "target" is the sound that is having a tag applied. "tags" are the tags being applied.
"setMasterVollume(float v, bool power = true)" changes the master vollume. "v" is the vollume. "power" is whether or not v has non-linear math used on it. Human hearing is logorithmic instead of linear.
"static void updateSoundVollume(int soundRef, bool power = true)" uses tag settings to change vollume of "soundRef"'s referenced sound and applies non-linear if power is = true.
"static void updateSounds(bool power = true)" runs updateSoundVollume on all sounds
"load(const char* path, std::string name = "", std::vector<std::string> tags = {})" loads a sound. "path" is file location of the sound being loaded. "name" is an optional variable that sets the loaded sound's name instead of implying it. There is another optional variable called "tags" which is a list of tags to apply to the loaded sound.
"unLoad(unsigned int/string ref)" unloads a sound

#### StaticWrite
Handles text generation
Is currently under review.

### util

## Lib

