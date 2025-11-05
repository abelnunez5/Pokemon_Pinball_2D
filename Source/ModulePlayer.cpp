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

    originalPlungerY = plungery; //aqui


    ball->SetBullet(true);

    return true;
}

update_status ModulePlayer::Update() {
	if (!physics) return UPDATE_CONTINUE; // controla los flippers izquierda y derecha
    float dt = GetFrameTime();
    bool L = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
    bool R = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);

    PlungerMovement(dt); //aqui

    physics->SetFlipperPressed(leftFlipper, L);
    physics->SetFlipperPressed(rightFlipper, R);
    UpdateBallAnimation(dt);    
    Draw(dt);
    return UPDATE_CONTINUE;
}

void ModulePlayer::PlungerMovement(float dt)
{
    
    static bool is_charging = false;
    static double start_time = 0.0;

    
    const float MAX_CHARGE_TIME_S = 1.0f; 
    const float MAX_PULL_DISTANCE_M = 3.0f; 

   
    if (IsKeyPressed(KEY_SPACE))
    {
        if (!is_charging)
        {
            is_charging = true;
            start_time = GetTime();
        }
    }

    
    if (IsKeyDown(KEY_SPACE) && is_charging)
    {
        double current_time = GetTime();
        double current_duration_s = current_time - start_time;

    
        if (current_duration_s > MAX_CHARGE_TIME_S)
        {
            current_duration_s = MAX_CHARGE_TIME_S;
        }

        
        float displacement_factor = (float)(current_duration_s / MAX_CHARGE_TIME_S);

        
        float new_y_pos = originalPlungerY + (displacement_factor * MAX_PULL_DISTANCE_M);

      
        plunger->SetTransform({ plunger->GetPosition().x, new_y_pos }, 0.0f);

       
      
    }


    if (IsKeyReleased(KEY_SPACE))
    {
        if (is_charging)
        {
            is_charging = false;
            double end_time = GetTime();
            double duration_s = end_time - start_time;

         
            duration_s = duration_s > MAX_CHARGE_TIME_S ? MAX_CHARGE_TIME_S : duration_s;

            float charge_duration_ms = (float)(duration_s * 1000.0);

      
            plunger->SetTransform({ plunger->GetPosition().x, originalPlungerY }, 0.0f);

            start_time = 0.0;
        }
    }
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



    App->renderer->Draw(plungerTexture, x_pixeles_plunger, y_pixeles_plunger, &sourceRect_p, 0, 25, 160);
    App->physics->RenderDebug();
}

bool ModulePlayer::CleanUp() {
    UnloadTexture(ballTexture);
    UnloadTexture(plungerTexture);
    return true;
}
