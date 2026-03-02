#pragma once

#include "scene/scene.h"
#include<string>
#include<unordered_map>;
#include<any>;
//#include <map>

struct DataHolder
{
	// logic is handled in .cpp file. 
public:
	// local vars
	// // scene management
	Scene* currentScene;
	Scene* nextScene;
	bool queue = false; // used for checking if need to delete scene on sceneQueue
	bool queueDelete = true;
	
	// physics framerate
	float physicsTick;
	float physicCap;
	float frameTick;
	float frameCap;

	// static vars
	static DataHolder god;

	// uncategorized data
	// - it is generally better to use a type safe solution than void points but this technically works
	// - it is dangerious to call or use this directly. The getter and setter methods are better approach.
	//std::unordered_map<std::string, void*> uncategorizedData; = {};
	std::unordered_map<std::string, std::any> uncategorizedData;

	// trash collection
	std::vector<void*> trashList = {}; // trashList is for things that are dangerious to delete mid handle/render

	// static funcs
	static void init();

	static void ChangeScene(Scene* s, bool clean = true); // direct way to change scene
	static void SceneQueue(Scene* s, bool clean = true); // safe and delayed way to change scene most of the time
	static void SceneUpdate(); // part 2 of sceneQueue(sceneQueue is a delayed scene change)

	void handleScene(float time=0); // scene render and scene handle are called here.
	
	// framerate function
	static void SetPhysicCap(float cap);
	static void SetFrameCap(float cap);
	
	// uncategorized data management
	// - If you don't use setters and getters, just go ahead and delete these. 
	static void SetUnCatData(std::string key, std::any data) { god.setUnCatData(key, data); };
	void setUnCatData(std::string key, std::any data);
	static bool CheckKeyUnCatData(std::string key) { return god.checkKeyUnCatData(key); };
	bool checkKeyUnCatData(std::string key);
	template<typename T>
	static T GetUnCatData(std::string key) { return god.getUnCatData(key); };
	template<typename T>
	T getUnCatData(std::string key) { return std::any_cast<T>(uncategorizedData[key]); }
	static void DeleteUnCatData(std::string key) { god.deleteUnCatData(key); };
	void deleteUnCatData(std::string key);

	//trash collection functions
	// - delayDelete is called manually. It is a safer way to delete things than using delete keyword.
	// - trashEmpty is called after scene handle and render to avoid crashes from things getting destroyed to early.
	static void DelayDelete(void* trash) { god.trashList.push_back(trash); }; 
	void trashEmpty();
};