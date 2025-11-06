#pragma once

#include "Globals.h"
#include "Timer.h"
#include "ModulePlayer.h"
#include <vector>

class Module;
class ModuleWindow;
class ModuleRender;
class ModuleAudio;
class ModulePhysics;
class Player;
class ModuleGame;

class Application
{
public:

	ModuleRender* renderer;
	ModuleWindow* window;
	ModuleAudio* audio;
	ModulePhysics* physics;
	ModuleGame* scene_intro;
	ModulePlayer* player;
	enum GameState : uint16 {
		MENU = 1,
		GAME = 2
	};


private:

	std::vector<Module*> list_modules;
    uint64 frame_count = 0;

	Timer ptimer;
	Timer startup_time;
	Timer frame_time;
	Timer last_sec_frame_time;

	uint32 last_sec_frame_count = 0;
	uint32 prev_last_sec_frame_count = 0;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();
	GameState gameStatus = GameState::MENU;

private:

	void AddModule(Module* module);
};