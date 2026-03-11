#pragma once
#include <bitset>
#include <GLFW/glfw3.h>

#include "unordered_map";

// hard coded values are GLFW constants
// have to use a normal map instead of bimap because of duplicate int values
// A-Z and 1-9 are omitted becaue they are single char
// - If you want them, they are easy to look up and implement
static const std::unordered_map<std::string, int> keyMap = {
	{"SPACE",32},
	{"APOSTROPHE",39},
	{"COMMA",44},
	{"MINUS",45},
	{"PERIOD",46},
	{"SLASH",47},
	{"SEMICOLON",59},
	{"EQUAL",61},
	{"LEFT_BRACKET",91},
	{"BACKSLASH",92},
	{"RIGHT_BRACKET",93},
	{"GRAVE_ACCENT",96},

	{"WORLD_1",161},
	{"WORLD_2",162},

	{"ESCAPE",256},
	{"ENTER",257},
	{"TAB",258},
	{"BACKSPACE",259},
	{"INSERT",260},
	{"DELETE",261},

	{"RIGHT",262},
	{"LEFT",263},
	{"DOWN",264},
	{"UP",265},

	{"PAGE_UP",266},
	{"PAGE_DOWN",267},
	{"HOME",268},
	{"END",269},

	{"CAPS_LOCK",280},
	{"SCROLL_LOCK",281},
	{"NUM_LOCK",282},
	{"PRINT_SCREEN",283},
	{"PAUSE",284},

	{"F1",290},
	{"F2",291},
	{"F3",292},
	{"F4",293},
	{"F5",294},
	{"F6",295},
	{"F7",296},
	{"F8",297},
	{"F9",298},
	{"F10",299},
	{"F11",300},
	{"F12",301},
	{"F13",302},
	{"F14",303},
	{"F15",304},
	{"F16",305},
	{"F17",306},
	{"F18",307},
	{"F19",308},
	{"F20",309},
	{"F21",310},
	{"F22",311},
	{"F23",312},
	{"F24",313},
	{"F25",314},

	{"KP_0",320},
	{"KP_1",321},
	{"KP_2",322},
	{"KP_3",323},
	{"KP_4",324},
	{"KP_5",325},
	{"KP_6",326},
	{"KP_7",327},
	{"KP_8",328},
	{"KP_9",329},

	{"KP_DECIMAL",330},
	{"KP_DIVIDE",331},
	{"KP_MULTIPLY",332},
	{"KP_SUBTRACT",333},
	{"KP_ADD",334},
	{"KP_ENTER",335},
	{"KP_EQUAL",336},

	{"LEFT_SHIFT",340},
	{"LEFT_CONTROL",341},
	{"LEFT_ALT",342},
	{"LEFT_SUPER",343},
	{"RIGHT_SHIFT",344},
	{"RIGHT_CONTROL",345},
	{"RIGHT_ALT",346},
	{"RIGHT_SUPER",347},

	{"MENU",348},

	{"MOUSE_BUTTON_1",0},
	{"MOUSE_BUTTON_2",1},
	{"MOUSE_BUTTON_3",2},
	{"MOUSE_BUTTON_4",3},
	{"MOUSE_BUTTON_5",4},
	{"MOUSE_BUTTON_6",5},
	{"MOUSE_BUTTON_7",6},
	{"MOUSE_BUTTON_8",7},

	{"MOUSE_LEFT",0},
	{"MOUSE_RIGHT",1},
	{"MOUSE_MIDDLE",2}
};

struct StaticInput
{
	//*
	//static StaticInput singleton;
	static StaticInput singleton;
	GLFWwindow* window;

	// keys
	std::bitset<GLFW_KEY_LAST + 1> tracked;
	std::bitset<GLFW_KEY_LAST + 1> isHeld;
	std::bitset<GLFW_KEY_LAST + 1> isClick;
	std::bitset<GLFW_KEY_LAST + 1> isRelease;

	// buttons
	std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> mouseTracked;
	std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> mouseHeld;
	std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> mouseClick;
	std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> mouseRelease;

	static void Init() 
	{
		singleton.init();
	}
	void init() 
	{
		window = glfwGetCurrentContext();
	}
	
	static void GetMouse(double& x, double& y, bool normalize = true) { singleton.getMouse(x, y, normalize); }
	void getMouse(double& x, double& y, bool normalize=true)
	{
		glfwGetCursorPos(window, &x, &y);
		if (normalize)
		{
			int width, height;
			glfwGetWindowSize(window, &width, &height);
			x = (x / width) * 2.0 - 1;
			y = -((y / height) * 2.0 - 1);
		}
	}

	static void Tick() { singleton.tick(); }
	void tick(bool keyboard=true, bool mouse=true)
	{
		if (keyboard)
		{
			for (int key = 0; key <= GLFW_KEY_LAST; key++)
			{
				if (!tracked[key]) { continue; }

				bool pressed = glfwGetKey(window, key) == GLFW_PRESS;
				isClick[key] = pressed && !isHeld[key];
				isRelease[key] = !pressed && isHeld[key];
				isHeld[key] = pressed;
			}
		}
		if (mouse)
		{
			for (int button = 0; button <= GLFW_MOUSE_BUTTON_LAST; button++)
			{
				if (!mouseTracked[button]) { continue; }

				bool pressed = glfwGetMouseButton(window, button) == GLFW_PRESS;
				mouseClick[button] = pressed && !mouseHeld[button];
				mouseRelease[button] = !pressed && mouseHeld[button];
				mouseHeld[button] = pressed;
			}
		}
	}

	template<typename T>
	static void KeyTrack(T key, bool positive = true) { singleton.keyTrack(key, positive); }
	void keyTrack(int key, bool positive = true)
	{
		if (key > GLFW_KEY_LAST) { return; }
		tracked[key] = positive;
	}
	void keyTrack(const std::string& key, bool positive = true)
	{
		keyTrack(StringToInt(key), positive);
	}

	template<typename T>
	static void KeyUntrack(T key) { singleton.keyUntrack(key); }
	void keyUntrack(int key) { keyTrack(key, false); }
	void keyUntrack(const std::string& key) { keyTrack(key, false); }

	// templates used because will probably add string later
	template<typename T>
	bool KeyClick(T i) { return singleton.keyClick(i); }
	bool keyClick(int i) { return isClick[i]; }

	template<typename T>
	bool KeyHeld(T i) { return singleton.keyHeld(i); }
	bool keyHeld(int i) { return isHeld[i]; }

	template<typename T>
	bool KeyRelease(T i) { return singleton.keyRelease(i); }
	bool keyRelease(int i) { return isRelease[i]; }

	template<typename T>
	static void MouseTrack(T button, bool positive = true){singleton.mouseTrack(button, positive);}
	void mouseTrack(int button, bool positive = true)
	{
		if (button > GLFW_MOUSE_BUTTON_LAST) { return; }
		mouseTracked[button] = positive;
	}
	void mouseTrack(const std::string& button, bool positive = true)
	{
		mouseTrack(StringToInt(button), positive);
	}

	template<typename T>
	static void MouseUntrack(T button){singleton.mouseUntrack(button);}
	void mouseUntrack(int button){mouseTrack(button, false);}
	void mouseUntrack(const std::string& button){mouseTrack(button, false);}

	template<typename T>
	static bool MouseClick(T i){return singleton.mouseClickState(i);}
	bool mouseClickState(int i){return mouseClick[i];}

	template<typename T>
	static bool MouseHeld(T i){return singleton.mouseHeldState(i);}
	bool mouseHeldState(int i){return mouseHeld[i];}

	template<typename T>
	static bool MouseRelease(T i){return singleton.mouseReleaseState(i);}
	bool mouseReleaseState(int i){return mouseRelease[i];}

	static int StringToInt(const std::string& input)
	{
		if (input.size() == 1) // string size 1 key is char
		{
			return input.at(0); // implicit char to int
		}
		auto it = keyMap.find(input);
		if (it != keyMap.end())
		{
			return it->second;
		}
		return -1;
	}
};

