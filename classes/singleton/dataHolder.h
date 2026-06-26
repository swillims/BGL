#pragma once

#include "scene/scene.h"
#include<string>
#include<unordered_map>
#include<any>

//#include <map>

struct DataHolder
{
	// local vars
	// // scene management
	Scene* currentScene;
	Scene* nextScene;
	bool queue = false;
	bool queueDelete = true;
	
	// physics framerate
	float physicsTick;
	float physicCap;
	float frameTick;
	int frameCapInt;
	float frameCap;

	// static vars
	static DataHolder god;

	// uncategorized data
	// - it is generally better to use a type safe solution than void pointers but this technically works
	// - it is dangerous to call or use this directly. The getter and setter methods are better approach.
	std::unordered_map<std::string, std::any> uncategorizedData;

	// struct for trash collection
	struct TrashItem
	{
		void* ptr;
		void (*deleter)(void*);
	};

	// trash collection
	std::vector<TrashItem> trashList;


	// static funcs
	static void init();

	static void ChangeScene(Scene* s, bool clean = true); // direct way to change scene - clean is for deleting previous scene
	static void SceneQueue(Scene* s, bool clean = true); // safer and delayed way to change scene - clean is for deleting previous scene
	static void SceneUpdate(); // part 2 of sceneQueue ^^^

	void handleScene(float time=0); // scene render and scene handle are called here.
	
	// framerate function
	static void SetPhysicsCap(float cap){god.setPhysicsCap(cap);}
	void setPhysicsCap(float cap);
	static void SetFrameCap(float cap){god.setFrameCap(cap);}
	void setFrameCap(float cap);

	// uncategorized data management
	static void SetUnCatData(std::string key, std::any data) { god.setUnCatData(key, data); };
	void setUnCatData(std::string key, std::any data);
	static bool CheckKeyUnCatData(std::string key) { return god.checkKeyUnCatData(key); };
	bool checkKeyUnCatData(std::string key);
	template<typename T>
	T getUnCatData(std::string key) { return std::any_cast<T>(uncategorizedData[key]); }
	static void DeleteUnCatData(std::string key) { god.deleteUnCatData(key); };
	void deleteUnCatData(std::string key);

	//trash collection functions

	// delayDelete is called manually. It is a safer way to delete things than using delete keyword.
	template<typename T>
	static void DelayDelete(T* ptr){god.delayDelete<T>(ptr);}
	template<typename T>
	void delayDelete(T* ptr)
	{
		trashList.push_back({
			ptr,
			[](void* p)
			{
				delete static_cast<T*>(p);
			}
		});
	}
	// trashEmpty is called after scene handle and render to avoid crashes from things getting destroyed too early.
	void trashEmpty();
};