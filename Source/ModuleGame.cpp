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

	tableroMenu = LoadTexture("Assets/Start Screen/TitleScreen_v2_scaled_3x.png");
	tableroGame = LoadTexture("Assets/PokemonPinball_Map.png");
	gameOverScreen = LoadTexture("Assets/GameOver_Screen_v2.png");

	App->audio->LoadFx("Assets/Audio/01 Title Screen.wav");
	App->audio->LoadFx("Assets/Audio/05 Red Field Theme.wav");
	App->audio->LoadFx("Assets/Audio/19 Game Over.wav");

	sfx_bouncer = App->audio->LoadFx("Assets/Audio/SFX/Shellder_SFX.ogg");
	sfx_pads = App->audio->LoadFx("Assets/Audio/SFX/Pad_SFX.ogg");

	App->audio->SetFxVolume(sfx_bouncer - 1, 5.0f);
	App->audio->SetFxVolume(sfx_pads - 1, 5.0f);

	//	Paredes
	float wall_thickness = 4.0f; // 4 píxeles de grosor
	float wall_restitution = 1.25f; // Cambiar esto poara que la bola rebote más

	App->physics->CreateThickerChain(0, 0, Main_Wall, 24, wall_thickness);
	App->physics->CreateThickerChain(0, 0, Right_Wall, 16, wall_thickness);
	App->physics->CreateThickerChain(0, 0, Slowpoke_Wall, 24, wall_thickness);
	App->physics->CreateThickerChain(0, 0, Cloyster_Wall, 24, wall_thickness);
	App->physics->CreateThickerChain(0, 0, GreenWall_L, 7, wall_thickness);
	App->physics->CreateThickerChain(0, 0, GreenWall_R, 7, wall_thickness);
	App->physics->CreateThickerChain(0, 0, Pad_L, 4, wall_thickness, wall_restitution, BodyType::PAD);
	App->physics->CreateThickerChain(0, 0, Pad_R, 4, wall_thickness, wall_restitution, BodyType::PAD);

	//	Obstaculos superiores
	App->physics->CreateBoxBody(ModulePhysics::P2M(204.0f), ModulePhysics::P2M(99.0f), ModulePhysics::P2M(18.0f), ModulePhysics::P2M(50.0f), false);
	App->physics->CreateBoxBody(ModulePhysics::P2M(276.0f), ModulePhysics::P2M(99.0f), ModulePhysics::P2M(18.0f), ModulePhysics::P2M(50.0f), false);

	//	Shellder bouncers 
	App->physics->CreateCircleBody(ModulePhysics::P2M(177.0f), ModulePhysics::P2M(210.0f), ModulePhysics::P2M(21.0f), false, BodyType::BOUNCER);
	App->physics->CreateCircleBody(ModulePhysics::P2M(240.0f), ModulePhysics::P2M(159.0f), ModulePhysics::P2M(21.0f), false, BodyType::BOUNCER);
	App->physics->CreateCircleBody(ModulePhysics::P2M(303.0f), ModulePhysics::P2M(210.0f), ModulePhysics::P2M(21.0f), false, BodyType::BOUNCER);

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
	switch (App->gameStatus) {
		case 1: {
			//MENU
			App->renderer->Draw(tableroMenu, 48, 70, 0, 0, 0, 0);
			App->audio->StopFx(2);
			if (!App->audio->isMenuMusicPlaying)
				App->audio->PlayFx(0, 1);
			break;
		}
		case 2: {
			//GAME
			App->renderer->Draw(tableroGame, 0, 0, 0, 0, 0, 0);
			DrawText(TextFormat("Score: %d", App->scene_intro->current_game_score), 176, 506, 18, BLACK);
			DrawText(TextFormat("Top Score: %d", App->scene_intro->top_game_score), 176, 526, 18, BLACK);
			DrawText(TextFormat("Max Score: %d", App->scene_intro->max_game_score), 176, 546, 18, BLACK);
			DrawText(TextFormat("Lives: %d", App->player->lives), 176, 566, 18, BLACK);

			App->audio->StopFx(0);
			if(!App->audio->isGameMusicPlaying)
				App->audio->PlayFx(1, 1);

			break;
		}
		case 3: {
			App->renderer->Draw(gameOverScreen, 0, 0, 0, 0, 0, 0);
			App->audio->StopFx(1);
			if (!App->audio->isGameOverMusicPlaying)
				App->audio->PlayFx(2, 1);

			break;
		}
	}

	App->physics->RenderDebug();


	return UPDATE_CONTINUE;
}
