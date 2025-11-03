#include "Player.h"
#include "Source/Application.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled)
    : Module(app, start_enabled) {
}

ModulePlayer::~ModulePlayer() {}

bool ModulePlayer::Start() {

	TraceLog(LOG_INFO, "Creating Player flippers"); //pruebas
    physics = App->physics;

	TraceLog(LOG_INFO, "ModulePhysics pointer in ModulePlayer: %p", (void*)physics); //pruebas
    if (!physics) return true;

    const float y = 610.0f;
    const float xLeft = 330.0f;
    const float xRight = 470.0f;
    const float flLen = 90.0f;
    const float flTh = 18.0f;

    leftFlipper = physics->CreateFlipper(xLeft, y, flLen, flTh, false);
    rightFlipper = physics->CreateFlipper(xRight, y, flLen, flTh, true);

    return true;
}

update_status ModulePlayer::Update() {

	if (!physics) return UPDATE_CONTINUE; // controla los flippers
    bool L = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
    bool R = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
    physics->SetFlipperPressed(leftFlipper, L);
    physics->SetFlipperPressed(rightFlipper, R);

    return UPDATE_CONTINUE;
}

bool ModulePlayer::CleanUp() {
    return true;
}
