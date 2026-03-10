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

	//std::map<int, bool> held;
	std::bitset<GLFW_KEY_LAST + 1> tracked;
	std::bitset<GLFW_KEY_LAST + 1> isHeld;
	std::bitset<GLFW_KEY_LAST + 1> isClick;
	std::bitset<GLFW_KEY_LAST + 1> isRelease;

	static void Init() 
	{
		singleton.init();
	}
	void init() 
	{
		window = glfwGetCurrentContext();
	}

	static void Tick() { singleton.tick(); }
	void tick()
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

	template<typename T>
	static void Track(T key, bool positive = true) { singleton.track(key, positive); }
	void track(int key, bool positive = true)
	{
		if (key > GLFW_KEY_LAST) { return; }
		tracked[key] = positive;
	}
	void track(const std::string& key, bool positive = true)
	{
		if(key.size()==1) // string size 1 key is char
		{
			track(key.at(0)); // implicit char to int
		}
		//keyMap["UP"];
		if (keyMap.contains(key))
		{
			track(keyMap.find(key)->second);
		}
	}

	template<typename T>
	static void Untrack(T key) { singleton.untrack(key); }
	void untrack(int key) { track(key, false); }
	void untrack(const std::string& key){ track(key, false); }

	//*/
};

