#pragma once

#include "singleton/gl_core.h"

#include <bitset>
#include <unordered_map>
#include <vector>

// hard coded values are GLFW constants
// have to use a normal map instead of bimap because of duplicate int values
// Important -> A-Z and 1-9 are omitted because they are single char and handled in StringToInt <-- Important
// ^^^ Important ^^^
// - If you want them, they are easy to look up and implement
static const std::unordered_map<std::string, int> keyMap = {
	{"SPACE", GLFW_KEY_SPACE},
	{"APOSTROPHE", GLFW_KEY_APOSTROPHE},
	{"COMMA", GLFW_KEY_COMMA},
	{"MINUS", GLFW_KEY_MINUS},
	{"PERIOD", GLFW_KEY_PERIOD},
	{"SLASH", GLFW_KEY_SLASH},
	{"SEMICOLON", GLFW_KEY_SEMICOLON},
	{"EQUAL", GLFW_KEY_EQUAL},
	{"LEFT_BRACKET", GLFW_KEY_LEFT_BRACKET},
	{"BACKSLASH", GLFW_KEY_BACKSLASH},
	{"RIGHT_BRACKET", GLFW_KEY_RIGHT_BRACKET},
	{"GRAVE_ACCENT", GLFW_KEY_GRAVE_ACCENT},

	{"WORLD_1", GLFW_KEY_WORLD_1},
	{"WORLD_2", GLFW_KEY_WORLD_2},

	{"ESCAPE", GLFW_KEY_ESCAPE},
	{"ENTER", GLFW_KEY_ENTER},
	{"TAB", GLFW_KEY_TAB},
	{"BACKSPACE", GLFW_KEY_BACKSPACE},
	{"INSERT", GLFW_KEY_INSERT},
	{"DELETE", GLFW_KEY_DELETE},

	{"RIGHT", GLFW_KEY_RIGHT},
	{"LEFT", GLFW_KEY_LEFT},
	{"DOWN", GLFW_KEY_DOWN},
	{"UP", GLFW_KEY_UP},

	{"PAGE_UP", GLFW_KEY_PAGE_UP},
	{"PAGE_DOWN", GLFW_KEY_PAGE_DOWN},
	{"HOME", GLFW_KEY_HOME},
	{"END", GLFW_KEY_END},

	{"CAPS_LOCK", GLFW_KEY_CAPS_LOCK},
	{"SCROLL_LOCK", GLFW_KEY_SCROLL_LOCK},
	{"NUM_LOCK", GLFW_KEY_NUM_LOCK},
	{"PRINT_SCREEN", GLFW_KEY_PRINT_SCREEN},
	{"PAUSE", GLFW_KEY_PAUSE},

	{"F1", GLFW_KEY_F1},
	{"F2", GLFW_KEY_F2},
	{"F3", GLFW_KEY_F3},
	{"F4", GLFW_KEY_F4},
	{"F5", GLFW_KEY_F5},
	{"F6", GLFW_KEY_F6},
	{"F7", GLFW_KEY_F7},
	{"F8", GLFW_KEY_F8},
	{"F9", GLFW_KEY_F9},
	{"F10", GLFW_KEY_F10},
	{"F11", GLFW_KEY_F11},
	{"F12", GLFW_KEY_F12},
	{"F13", GLFW_KEY_F13},
	{"F14", GLFW_KEY_F14},
	{"F15", GLFW_KEY_F15},
	{"F16", GLFW_KEY_F16},
	{"F17", GLFW_KEY_F17},
	{"F18", GLFW_KEY_F18},
	{"F19", GLFW_KEY_F19},
	{"F20", GLFW_KEY_F20},
	{"F21", GLFW_KEY_F21},
	{"F22", GLFW_KEY_F22},
	{"F23", GLFW_KEY_F23},
	{"F24", GLFW_KEY_F24},
	{"F25", GLFW_KEY_F25},

	{"KP_0", GLFW_KEY_KP_0},
	{"KP_1", GLFW_KEY_KP_1},
	{"KP_2", GLFW_KEY_KP_2},
	{"KP_3", GLFW_KEY_KP_3},
	{"KP_4", GLFW_KEY_KP_4},
	{"KP_5", GLFW_KEY_KP_5},
	{"KP_6", GLFW_KEY_KP_6},
	{"KP_7", GLFW_KEY_KP_7},
	{"KP_8", GLFW_KEY_KP_8},
	{"KP_9", GLFW_KEY_KP_9},

	{"KP_DECIMAL", GLFW_KEY_KP_DECIMAL},
	{"KP_DIVIDE", GLFW_KEY_KP_DIVIDE},
	{"KP_MULTIPLY", GLFW_KEY_KP_MULTIPLY},
	{"KP_SUBTRACT", GLFW_KEY_KP_SUBTRACT},
	{"KP_ADD", GLFW_KEY_KP_ADD},
	{"KP_ENTER", GLFW_KEY_KP_ENTER},
	{"KP_EQUAL", GLFW_KEY_KP_EQUAL},

	{"LEFT_SHIFT", GLFW_KEY_LEFT_SHIFT},
	{"LEFT_CONTROL", GLFW_KEY_LEFT_CONTROL},
	{"LEFT_ALT", GLFW_KEY_LEFT_ALT},
	{"LEFT_SUPER", GLFW_KEY_LEFT_SUPER},
	{"RIGHT_SHIFT", GLFW_KEY_RIGHT_SHIFT},
	{"RIGHT_CONTROL", GLFW_KEY_RIGHT_CONTROL},
	{"RIGHT_ALT", GLFW_KEY_RIGHT_ALT},
	{"RIGHT_SUPER", GLFW_KEY_RIGHT_SUPER},

	{"MENU", GLFW_KEY_MENU},

	{"MOUSE_BUTTON_1", GLFW_MOUSE_BUTTON_1},
	{"MOUSE_BUTTON_2", GLFW_MOUSE_BUTTON_2},
	{"MOUSE_BUTTON_3", GLFW_MOUSE_BUTTON_3},
	{"MOUSE_BUTTON_4", GLFW_MOUSE_BUTTON_4},
	{"MOUSE_BUTTON_5", GLFW_MOUSE_BUTTON_5},
	{"MOUSE_BUTTON_6", GLFW_MOUSE_BUTTON_6},
	{"MOUSE_BUTTON_7", GLFW_MOUSE_BUTTON_7},
	{"MOUSE_BUTTON_8", GLFW_MOUSE_BUTTON_8},

	{"MOUSE_LEFT", GLFW_MOUSE_BUTTON_LEFT},
	{"MOUSE_RIGHT", GLFW_MOUSE_BUTTON_RIGHT},
	{"MOUSE_MIDDLE", GLFW_MOUSE_BUTTON_MIDDLE}
};

struct StaticInput
{
	//static StaticInput singleton;
	static StaticInput singleton;
	GLFWwindow* window;

	// keys
	std::bitset<GLFW_KEY_LAST + 1> tracked;
	std::bitset<GLFW_KEY_LAST + 1> isHeld;
	std::bitset<GLFW_KEY_LAST + 1> isClick;
	std::bitset<GLFW_KEY_LAST + 1> isRelease;

	// mouse buttons
	std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> mouseTracked;
	std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> mouseHeld;
	std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> mouseClick;
	std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> mouseRelease;

	std::unordered_map<std::string, int> aliasList;

	static void Init() {singleton.init();}
	void init() {window = glfwGetCurrentContext();}
	
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

	static void KeyTrackSetAll(bool on = true){singleton.keyTrackSetAll(on);}
	void keyTrackSetAll(bool on = true)
	{
		if (on){tracked.set();}
		else {tracked.reset();}
	}
	// templates used because will probably add string later
	template<typename T>
	static bool KeyClick(T i) { return singleton.keyClick(i); }
	bool keyClick(int i) { return isClick[i]; }

	template<typename T>
	static bool KeyHeld(T i) { return singleton.keyHeld(i); }
	bool keyHeld(int i) { return isHeld[i]; }

	template<typename T>
	static bool KeyRelease(T i) { return singleton.keyRelease(i); }
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

	void mouseTrackSetAll(bool on = true)
	{
		if (on){mouseTracked.set();}
		else {mouseTracked.reset();}
	}

	template<typename T>
	static bool MouseClick(T i){return singleton.mouseClickState(i);}
	bool mouseClickState(int i){return mouseClick[i];}

	template<typename T>
	static bool MouseHeld(T i){return singleton.mouseHeldState(i);}
	bool mouseHeldState(int i){return mouseHeld[i];}

	template<typename T>
	static bool MouseRelease(T i){return singleton.mouseReleaseState(i);}
	bool mouseReleaseState(int i){return mouseRelease[i];}

	template<typename T>
	static void AssignAlias(const std::string& name, T ref){singleton.assignAlias(name,ref);}
	void assignAlias(const std::string& name, const std::string& ref){assignAlias(name, StringToInt(ref));}
	void assignAlias(const std::string& name, int ref){aliasList[name] = ref;}

	template<typename T>
	static bool HasAlias(const std::string& name){return singleton.hasAlias(name);}
	bool hasAlias(const std::string& name){return aliasList.find(name) != aliasList.end();}

	static void ClearAllAlias(){singleton.clearAllAlias();}
	void clearAllAlias(){aliasList.clear();}

	template<typename T>
	static int GetAlias(T ref){return singleton.getAlias(ref);}
	int getAlias(const std::string& ref){return aliasList[ref];}

	static std::string GetStringAlias(std::string ref){return singleton.getStrintAlias(ref);}
	std::string getStrintAlias(std::string ref){return IntToString(getAlias(ref));}

	static std::vector<int> GetTrackedKeys(){return singleton.getTrackedKeys();}
	std::vector<int> getTrackedKeys()
	{
		std::vector<int> out;
		for (int i = 0; i < tracked.size(); i++)
		{
			if (tracked[i])
			{
				out.push_back(i);
			}
		}
		return out;
	}

	static int StringToInt(const std::string& input)
	{
		if (input.size() == 1) // a string with size 1 is probably a char
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

	static std::string IntToString(unsigned int input)
	{
		if (input >= '0' && input <= '9')
		{
			return std::string(1, static_cast<char>(input));
		}
		if (input >= 'A' && input <= 'Z')
		{
			return std::string(1, static_cast<char>(input));
		}
		for (const auto& [name, value] : keyMap)
		{
			if (value == input)
			{
				return name;
			}
		}
		if (input >= 32 && input <= 126) // this looks redundant because it is. Its function is to check for single char inputs not in keyMap for if keyMap gets changed. It also recovers ints and letters because no performance cost.
		{
			return std::string(1, static_cast<char>(input));
		}
		return "StaticInput failed to find key in keymap";
	}

	static char keyToChar(int key, bool shift)
	{
		// A-Z
		if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
		{
			char c = 'a' + (key - GLFW_KEY_A);
			return shift ? (c - ('a' - 'A')) : c;
		}

		// 0-9
		if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
		{
			if (!shift)
			{
				return '0' + (key - GLFW_KEY_0);
			}

			const char shiftedNumbers[] = ")!@#$%^&*(";
			return shiftedNumbers[key - GLFW_KEY_0];
		}

		switch(key)
		{
			case GLFW_KEY_SPACE: return ' ';
			case GLFW_KEY_MINUS: return shift ? '_' : '-';
			case GLFW_KEY_EQUAL: return shift ? '+' : '=';
			case GLFW_KEY_COMMA: return shift ? '<' : ',';
			case GLFW_KEY_PERIOD: return shift ? '>' : '.';
			case GLFW_KEY_SLASH: return shift ? '?' : '/';
			case GLFW_KEY_LEFT_BRACKET:  return shift ? '{' : '[';
			case GLFW_KEY_RIGHT_BRACKET: return shift ? '}' : ']';
			case GLFW_KEY_BACKSLASH:     return shift ? '|' : '\\';
			case GLFW_KEY_SEMICOLON:     return shift ? ':' : ';';
			case GLFW_KEY_APOSTROPHE:    return shift ? '"' : '\'';
			case GLFW_KEY_GRAVE_ACCENT: return shift ? '~' : '`';
		}

		return '\0';
	}
};

