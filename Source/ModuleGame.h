#pragma once
#include "Globals.h"
#include "Module.h"

#include "p2Point.h"

#include "raylib.h"
#include <vector>

class PhysBody;
class PhysicEntity;

class ModuleGame : public Module
{
public:
	ModuleGame(Application* app, bool start_enabled = true);
	~ModuleGame();

	bool Start();
	update_status Update();
	bool CleanUp();


public:

	std::vector<PhysicEntity*> entities;

	Texture2D tableroMenu;
	Texture2D tableroGame;
	Texture2D gameOverScreen;

	unsigned int sfx_bouncer;
	unsigned int sfx_pads;

	unsigned int max_game_score = 0;
	unsigned int top_game_score = 0;
	unsigned int current_game_score = 0;
	unsigned int bonus_live_counter = 0;
};
