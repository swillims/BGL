# BGL
BGL is an ultra minimalist OpenGL based game engine. I have it set up to be 2d.

## Philosophy
BGL is minimalistic and developer controlled. Minimalism and developer control are the highest priority of the engine.
A problem with other engines is that they are over engineered and awkward to work with. This game engine is on the opposite side of the spectrum intentionally. The intended user for this game engine is somebody who has a history of fighting game engines or wants to self implement things instead of going through tutorial hell.This engine does the bare minimum to be functional. The coding and architectural learning curve is higher than other game engines but the learning curve to actually learn the engine is extremely due to minimalism. Other game engines have their own developer tools/IDEs. This game engine has to be compiled from an IDE such as visual studio and does not have a GUI because minimalism.

In addition to minimalism, the engine also has a focus on user control. The code is open source. Core functionalities(sound/textgen/image) are handled by classes inside of the "singleton" folder. Changing core game engine code is highly recommended. 

## Hierarchy
BGL is minimalistic. There is no implemented scene hierarchy. The engine only tracks one main scene at a time. Each scene is its own self contained entity. Although scene hierarchy is not supported it is possible for scenes to contain other scenes. It is possible to have a main scene that calls the handle and render method on other scenes that are attached to it. Scene hierarchy is possible and encouraged, it just is not supported. Scene hierarchy is not a forced feature because of philosophy.

GameObjects do not exist. Most of everything is self implemented. A different game engine may have GameObject attached to scene and the GameObject would have a script attached to it. The game object has XYZ cords, tags, and etc. The forced existence of GameObjects goes against this engines philosophy, it is over engiineered, and it is completely unnessary. Instead of having GameObjects as a forced object, the user can make it if they think it is a valid solution. 
As an example of the point of 'GameObjects should be user implemented instead of engine implemented', read frogHop.cpp or any of the menu files and ask if transforming components into GameObjects would be an improvement in performance or simplicity.

## Running on non-Windows
The engine is set up to be Windows only. Setting the engine up to work on other operating systems should be very easy. It is currently set up for windows only because the author(me) only has a windows PC set up. To set the engine up for more than one operating system, somebody(you) would have to download linux/mac equivelent files for every windows .dll file in the /lib folder. After getting the new files added, the "CMakeLists.txt" file would need to be changed to use the new files when built for the other OS. I'm not teaching c++ or CMake, so this is very surface level. It should be easy to look up how to do.

## Notable Classes/Structs
There are skipped functions. They are not things that users should regularly use.
### Scene
Scene's contain and process game info. Other classes inherit from scene and are used as the engine's primary building block.
"onLoad()" runs automatically when a scene is loaded. It is good to load things and set default variables in this method.
"clean()" is supposed to be run before a scene is being deleted. It is good to unload things in this method.
"handle()" runs every game step and is meant to handle game logic.
"render()" runs every frame and is meant to handle graphics.
"apsectChange()" should be run every time the screen size changes and it also good practice to manually call it one time in "onLoad()"
In addition to methods, it is good to add variables to classes that inherit scene to store information.

### singleton/
#### DataHolder
DataHolder holds the current scene and it also holds the next scene if one is queued.
It has variables to regulate gamestep and frame rate.
"ChangeScene(Scene* s, bool clean = true)" Instantly changes scene. It is the direct way to change scenes. "s" is the next scene. "clean" is whether or not c++ is going to delete previous scene.
"SceneQueue(Scene* s, bool clean = true);" Changes scene at end of cycle. Is the normal way to change scenes. "s" is the next scene. "clean" is whether or not c++ is going to delete previous scene.
"SetPhysicCap(float cap)" changes rate at which handle runs 
"SetFrameCap(float cap)" changes rate at which render runs
"DelayDelete(void* trash)" deletes things at end of cycle

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












