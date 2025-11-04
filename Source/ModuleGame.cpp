#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "CollidersData.h"

ModuleGame::ModuleGame(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleGame::~ModuleGame()
{}

// Load assets
bool ModuleGame::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	tablero = LoadTexture("Assets/PokemonPinball_Map.png");

	App->physics->CreateChain(0, 0, Main_Wall, 24);
	App->physics->CreateChain(0, 0, Right_Wall, 16);
	App->physics->CreateChain(0, 0, Slowpoke_Wall, 24);
	App->physics->CreateChain(0, 0, Cloyster_Wall, 24);
	App->physics->CreateChain(0, 0, GreenWall_L, 7);
	App->physics->CreateChain(0, 0, GreenWall_R, 7);
	App->physics->CreateChain(0, 0, Pad_L, 4);
	App->physics->CreateChain(0, 0, Pad_R, 4);

	return ret;
}

// Load assets
bool ModuleGame::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleGame::Update()
{
	App->renderer->Draw(tablero, 0,0, 0, 0,0,0);
	App->physics->RenderDebug();

	return UPDATE_CONTINUE;
}
