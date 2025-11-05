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

	//	Paredes
	float wall_thickness = 4.0f; // 4 píxeles de grosor

	App->physics->CreateThickerChain(0, 0, Main_Wall, 24, wall_thickness);
	App->physics->CreateThickerChain(0, 0, Right_Wall, 16, wall_thickness);
	App->physics->CreateThickerChain(0, 0, Slowpoke_Wall, 24, wall_thickness);
	App->physics->CreateThickerChain(0, 0, Cloyster_Wall, 24, wall_thickness);
	App->physics->CreateThickerChain(0, 0, GreenWall_L, 7, wall_thickness);
	App->physics->CreateThickerChain(0, 0, GreenWall_R, 7, wall_thickness);
	App->physics->CreateThickerChain(0, 0, Pad_L, 4, wall_thickness);
	App->physics->CreateThickerChain(0, 0, Pad_R, 4, wall_thickness);

	//	Obstaculos superiores
	App->physics->CreateBoxBody(ModulePhysics::P2M(204.0f), ModulePhysics::P2M(99.0f), ModulePhysics::P2M(18.0f), ModulePhysics::P2M(54.0f), false);
	App->physics->CreateBoxBody(ModulePhysics::P2M(276.0f), ModulePhysics::P2M(99.0f), ModulePhysics::P2M(18.0f), ModulePhysics::P2M(54.0f), false);

	//	Shellder bouncers 
	App->physics->CreateCircleBody(ModulePhysics::P2M(177.0f), ModulePhysics::P2M(210.0f), ModulePhysics::P2M(24.0f), false);
	App->physics->CreateCircleBody(ModulePhysics::P2M(240.0f), ModulePhysics::P2M(159.0f), ModulePhysics::P2M(24.0f), false);
	App->physics->CreateCircleBody(ModulePhysics::P2M(303.0f), ModulePhysics::P2M(210.0f), ModulePhysics::P2M(24.0f), false);

	//	Boton Izda
	App->physics->CreateBoxBody(ModulePhysics::P2M(81.0f), ModulePhysics::P2M(546.0f), ModulePhysics::P2M(18.0f), ModulePhysics::P2M(48.0f), false);

	//	Psyduck
	App->physics->CreateCircleBody(ModulePhysics::P2M(408.0f), ModulePhysics::P2M(534.0f), ModulePhysics::P2M(24.0f), false);

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
