#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

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
	/*DrawCircle(40, 40, 10, RED);*/ // prueba de dibujo
	App->physics->RenderDebug();

	return UPDATE_CONTINUE;
}
