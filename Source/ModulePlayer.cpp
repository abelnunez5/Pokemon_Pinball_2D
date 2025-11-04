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

    const float y = 610.0f;
    const float xLeft = 330.0f;
    const float xRight = 470.0f;
    const float flLen = 90.0f;
    const float flTh = 18.0f;

    leftFlipper = physics->CreateFlipper(xLeft, y, flLen, flTh, false);
    rightFlipper = physics->CreateFlipper(xRight, y, flLen, flTh, true);

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
