#include "ModulePlayer.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleRender.h"



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
    const float ballx = 1.0f;
    const float bally = 1.0f;

    ballTexture = LoadTexture("Assets/pokeball3.png");
    ball = physics->CreateCircleBody(ballx,bally,0.45f,true);

    //Plunger
    const float plungerx = 10.0f;
    const float plungery = 13.5f;
    const float plungerw = 1.0f;
    const float plungerh = 6.5f;

    plungerTexture = LoadTexture("Assets/Plunger.png");
    plunger = physics->CreateBoxBody(plungerx, plungery, plungerw, plungerh, false, 0.0f);


    ball->SetBullet(true);

    return true;
}

update_status ModulePlayer::Update() {
	if (!physics) return UPDATE_CONTINUE; // controla los flippers izquierda y derecha
    float dt = GetFrameTime();
    bool L = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
    bool R = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
    physics->SetFlipperPressed(leftFlipper, L);
    physics->SetFlipperPressed(rightFlipper, R);
    UpdateBallAnimation(dt);    
    Draw(dt);
    return UPDATE_CONTINUE;
}

void ModulePlayer::UpdateBallAnimation(float dt)
{ 
    const float ANIMATION_DELAY = 0.05f;
    float horizontalVelocity = ball->GetLinearVelocity().x;

    BallState targetState = currentBallState;

    switch (currentBallState)
    {
    case ModulePlayer::BallState::RECTA:
        if (horizontalVelocity > 0.1f)
        {
            targetState = BallState::INCDERRECT; 
        }
        else if (horizontalVelocity < -0.1f)
        {
            targetState = BallState::INCIZQRECT; 
        }
        break;

    case ModulePlayer::BallState::INCIZQRECT:
        if (horizontalVelocity > 0.1f)
        {
            targetState = BallState::RECTA; 
        }
        else if (horizontalVelocity < -0.1f)
        {
            targetState = BallState::IZQLADO; 
        }
        break;

    case ModulePlayer::BallState::IZQLADO:
        if (horizontalVelocity > 0.1f)
        {
            targetState = BallState::INCIZQRECT; 
        }
        else if (horizontalVelocity < -0.1f)
        {
            targetState = BallState::INCIZQABAJO; 
        }
        break;

    case ModulePlayer::BallState::INCIZQABAJO:
        if (horizontalVelocity > 0.1f)
        {
            targetState = BallState::IZQLADO; 
        }
        else if (horizontalVelocity < -0.1f)
        {
            targetState = BallState::ABAJO; 
        }
        break;

    case ModulePlayer::BallState::INCDERRECT:
        if (horizontalVelocity > 0.1f)
        {
            targetState = BallState::DERLADO; 
        }
        else if (horizontalVelocity < -0.1f)
        {
            targetState = BallState::RECTA; 
        }
        break;

    case ModulePlayer::BallState::DERLADO:
        if (horizontalVelocity > 0.1f)
        {
            targetState = BallState::INCDERABAJO; 
        }
        else if (horizontalVelocity < -0.1f)
        {
            targetState = BallState::INCDERRECT;
        }
        break;

    case ModulePlayer::BallState::INCDERABAJO:
        if (horizontalVelocity > 0.1f)
        {
            targetState = BallState::ABAJO;
        }
        else if (horizontalVelocity < -0.1f)
        {
            targetState = BallState::DERLADO; 
        }
        break;

    case ModulePlayer::BallState::ABAJO:
        if (horizontalVelocity > 0.1f)
        {
            targetState = BallState::INCIZQABAJO;
        }
        else if (horizontalVelocity < -0.1f)
        {
            targetState = BallState::INCDERABAJO;
        }
        break;

    default:
        break;
    }

    if (targetState != currentBallState)
    {
     
        if (targetState != pendingBallState)
        {
            pendingBallState = targetState;
            stateChangeTimer = 0.0f;
        }
        else
        {
            stateChangeTimer += dt;
        }

        if (stateChangeTimer > ANIMATION_DELAY)
        {
            currentBallState = pendingBallState;
            stateChangeTimer = 0.0f;
        }
    }
    else
    {
        stateChangeTimer = 0.0f;
        pendingBallState = currentBallState;
    }
}

void ModulePlayer::Draw(float dt) {
	if (!physics || !ball) return;
   //rect ball
   Rectangle sourceRect;

    switch (currentBallState)
    {
    case ModulePlayer::BallState::RECTA:        sourceRect = { 216.0f, 0.0f, 48.0f, 48.0f }; break;
    case ModulePlayer::BallState::INCIZQRECT:   sourceRect = { 162.0f, 0.0f, 48.0f, 48.0f }; break;
    case ModulePlayer::BallState::IZQLADO:      sourceRect = { 108.0f, 0.0f, 48.0f, 48.0f }; break;
    case ModulePlayer::BallState::INCIZQABAJO:  sourceRect = { 54.0f, 0.0f, 48.0f, 48.0f };  break;
    case ModulePlayer::BallState::INCDERRECT:   sourceRect = { 270.0f, 0.0f, 48.0f, 48.0f }; break;
    case ModulePlayer::BallState::DERLADO:      sourceRect = { 324.0f, 0.0f, 48.0f, 48.0f }; break;
    case ModulePlayer::BallState::INCDERABAJO:  sourceRect = { 378.0f, 0.0f, 48.0f, 48.0f }; break;
    case ModulePlayer::BallState::ABAJO:        sourceRect = { 0.0f, 0.0f, 48.0f, 48.0f };   break;
    default:                                    sourceRect = { 216.0f, 0.0f, 48.0f, 48.0f }; break;
    }

    //rect plunger
    Rectangle sourceRect_p = { 0.0f, 0.0f, 56.0f, 322.0f };

    float x_pixeles_ball = ModulePhysics::M2P(ball->GetPosition().x);
    float y_pixeles_ball = ModulePhysics::M2P(ball->GetPosition().y);

    float draw_x_ball = x_pixeles_ball - (sourceRect.width / 2.0f);
    float draw_y_ball = y_pixeles_ball - (sourceRect.height / 2.0f);

    App->renderer->Draw(ballTexture, draw_x_ball, draw_y_ball, &sourceRect, 0, 0, 0);
    App->physics->RenderDebug();


    float x_pixeles_plunger = ModulePhysics::M2P(plunger->GetPosition().x);
    float y_pixeles_plunger = ModulePhysics::M2P(plunger->GetPosition().y); 

    float draw_x_plunger = x_pixeles_plunger - (sourceRect_p.width / 2.0f);
    float draw_y_plunger = y_pixeles_plunger - (sourceRect_p.height / 2.0f);

    App->renderer->Draw(plungerTexture, x_pixeles_plunger, y_pixeles_plunger, &sourceRect_p, 0, 25, 160);
    App->physics->RenderDebug();
}

bool ModulePlayer::CleanUp() {
    UnloadTexture(ballTexture);
    return true;
}
