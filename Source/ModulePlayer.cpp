#include "ModulePlayer.h"
#include "Application.h"
#include "ModulePhysics.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled)
    : Module(app, start_enabled) {
}

ModulePlayer::~ModulePlayer() {}

bool ModulePlayer::Start() {

	TraceLog(LOG_INFO, "Player start BGEIN"); // Esta creando los flippers ?
    physics = App->physics;

	TraceLog(LOG_INFO, "App->physics=%p", (void*)physics); //en que punto lo hace ?
    if (!physics) return true;

// Coordenadas medidas directamente sobre la textura (en píxeles):
    const float leftPivotPxX = 163.0f;
    const float leftPivotPxY = 772.0f;
    const float rightPivotPxX = 318.0f;
    const float rightPivotPxY = 772.0f;

    const float flLenPx = 90.0f;
    const float flThPx = 18.0f;

    leftFlipper = physics->CreateFlipper(leftPivotPxX, leftPivotPxY, flLenPx, flThPx, true);
    rightFlipper = physics->CreateFlipper(rightPivotPxX, rightPivotPxY, flLenPx, flThPx, false);

    TraceLog(LOG_INFO, "Left flipper:  anchor=%p blade=%p joint=%p",
        (void*)leftFlipper.anchor, (void*)leftFlipper.blade, (void*)leftFlipper.joint);
    TraceLog(LOG_INFO, "Right flipper: anchor=%p blade=%p joint=%p",
        (void*)rightFlipper.anchor, (void*)rightFlipper.blade, (void*)rightFlipper.joint);


    //Bola
    const float ballx = 100.0f;
    const float bally = 10.0f;

    physics->CreateCircleBody(ballx,bally,0.3f,true);



    return true;
}

update_status ModulePlayer::Update() {

	if (!physics) return UPDATE_CONTINUE; // controla los flippers izquierda y derecha
    bool L = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
    bool R = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
    physics->SetFlipperPressed(leftFlipper, L);
    physics->SetFlipperPressed(rightFlipper, R);

    return UPDATE_CONTINUE;
}

bool ModulePlayer::CleanUp() {
    return true;
}
