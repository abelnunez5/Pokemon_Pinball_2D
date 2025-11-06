#include "ModulePlayer.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleRender.h"
#include "raylib.h"



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

    const float flLenPx = 65.0f;
    const float flThPx = 18.0f;
    
    leftFlipper = physics->CreateFlipper(leftPivotPxX, leftPivotPxY, flLenPx, flThPx, true); // crea flipper izquierdo
    rightFlipper = physics->CreateFlipper(rightPivotPxX, rightPivotPxY, flLenPx, flThPx, false); // crea flipper derecho

    texFlipperL = LoadTexture("Assets/Palanca_Izda_24x9.png"); // carga textura flipper izquierdo
    texFlipperR = LoadTexture("Assets/Palanca_Dcha_24x9.png"); // carga textura flipper derecho

    TraceLog(LOG_INFO, "Left flipper:  anchor=%p blade=%p joint=%p",
        (void*)leftFlipper.anchor, (void*)leftFlipper.blade, (void*)leftFlipper.joint);  // que nos diga las posiciones de los flippers en consola
    TraceLog(LOG_INFO, "Right flipper: anchor=%p blade=%p joint=%p",
        (void*)rightFlipper.anchor, (void*)rightFlipper.blade, (void*)rightFlipper.joint); // que nos diga las posiciones de los flippers en consola


    //Bola
    const float ballx = 10.2f;
    const float bally = 9.5f;
    ball = physics->CreateCircleBody(ballx, bally, 0.20f, true, BodyType::BALL);

    ballTexture = LoadTexture("Assets/pokeball3.png");
    ball->SetBullet(true);

    //Plunger
    const float plungerx = 10.0f;
    const float plungery = 13.5f;
    const float plungerw = 1.0f;
    const float plungerh = 6.5f;

    plunger = physics->CreateBoxBody(plungerx, plungery, plungerw, plungerh, false, 0.0f);
    plungerTexture = LoadTexture("Assets/Plunger.png");

    originalPlungerY = plungery;
    switch (App->gameStatus) {
        case 1: {

            break;
        }
        case 2: {
            
            break;
        }
    }

	

    return true;
}

update_status ModulePlayer::Update() {
	if (!physics) return UPDATE_CONTINUE; // controla los flippers izquierda y derecha
    float dt = GetFrameTime();
    bool L = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
    bool R = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);

    switch (App->gameStatus) {
    case 1: {
        if (IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_ENTER)) {
            App->gameStatus = Application::GameState::GAME;
            
        }

        break;
    }
    case 2: {
        PlungerMovement(dt);

        physics->SetFlipperPressed(leftFlipper, L);
        physics->SetFlipperPressed(rightFlipper, R);

        // Dibujado de sprites flippers usando el renderer
        auto drawFlipper = [&](const ModulePhysics::Flipper& f, const Texture2D& tex, bool isLeft)
            {
                if (!f.anchor || !f.blade || tex.id == 0) return;

                b2Vec2 ap = f.anchor->GetPosition();
                float ax = ModulePhysics::M2P(ap.x);
                float ay = ModulePhysics::M2P(ap.y);
                float angDeg = f.blade->GetAngle() * (180.0f / 3.1415926f);

                float destW = flLenPx;
                float destH = flThPx;

                float scaleX = destW / (float)tex.width;
                float scaleY = destH / (float)tex.height;

                float insetX_dest = (isLeft ? pivotInsetL_texX : pivotInsetR_texX) * scaleX;
                float insetY_dest = pivotInsetL_texY * scaleY;

                int pivot_x = (int)(isLeft ? (destW - insetX_dest) : insetX_dest);
                int pivot_y = (int)((destH * 0.5f) + insetY_dest);

                int drawX = (int)(isLeft ? ax - pivot_x : ax);
                int drawY = (int)(ay - pivot_y);

                Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };

                App->renderer->Draw(tex, drawX, drawY, &src, angDeg, pivot_x, pivot_y, destW, destH);
            };

        // Llamadas:
        drawFlipper(leftFlipper, texFlipperL, /*isLeft=*/true);
        drawFlipper(rightFlipper, texFlipperR, /*isLeft=*/false);

        const float MAX_SPEED_MS = 20.0f; // Velocidad maxima de la pelota
        const float MAX_SPEED_SQ = MAX_SPEED_MS * MAX_SPEED_MS;

        b2Vec2 velocity = ball->GetLinearVelocity();
        float speedSqr = velocity.LengthSquared();

        if (speedSqr > MAX_SPEED_SQ)
        {
            velocity.Normalize();
            velocity *= MAX_SPEED_MS;
            ball->SetLinearVelocity(velocity);
        }

        UpdateBallAnimation(dt);
        Draw(dt);
        break;
    }
    }

    
    return UPDATE_CONTINUE;
}

void ModulePlayer::PlungerMovement(float dt)
{
    
    if (canPlunger == false)
    {
        return;
    }    

    static bool is_charging = false;
    static double start_time = 0.0;

   
    const float MAX_CHARGE_TIME_S = 1.0f;    
    const float MAX_PULL_DISTANCE_M = 2.5f;  

    
    const float PLUNGER_RISE_TIME_S = 0.2f; 

    float current_duration_s = 0.0f; 

   
    if (IsKeyDown(KEY_SPACE))
    {
        if (IsKeyPressed(KEY_SPACE) && !is_charging)
        {
            is_charging = true;
            start_time = GetTime();
        }

        if (is_charging)
        {
            current_duration_s = GetTime() - start_time;
           
            if (current_duration_s > MAX_CHARGE_TIME_S) current_duration_s = MAX_CHARGE_TIME_S;
            
            float displacement_factor = current_duration_s / MAX_CHARGE_TIME_S;
            float new_y_pos = originalPlungerY + (displacement_factor * MAX_PULL_DISTANCE_M);
            plunger->SetTransform({ plunger->GetPosition().x, new_y_pos }, 0.0f);
        }
    }

   
    else if (IsKeyReleased(KEY_SPACE))
    {
        if (is_charging && !plungerIsShooting)
        {
         
            is_charging = false;
            current_duration_s = GetTime() - start_time;
            if (current_duration_s > MAX_CHARGE_TIME_S) current_duration_s = MAX_CHARGE_TIME_S;
          
            plungerIsShooting = true;
            shootStartTime = GetTime();
            shootDurationS = current_duration_s; 
        }
    }

    
    if (plungerIsShooting)
    {
        double time_since_shoot = GetTime() - shootStartTime;

        
        if (time_since_shoot >= PLUNGER_RISE_TIME_S)
        {
            plungerIsShooting = false;
           
            plunger->SetTransform({ plunger->GetPosition().x, originalPlungerY }, 0.0f);

         
            const float MAX_IMPULSE = 15.0f; 
            float impulse_magnitude = (shootDurationS / MAX_CHARGE_TIME_S) * MAX_IMPULSE;

           
            b2Vec2 impulse_vector(0.0f, -impulse_magnitude);

            
            ball->ApplyLinearImpulseToCenter(impulse_vector, true);

            canPlunger = false;

            return; 
        }

       
        float rise_factor = (float)(time_since_shoot / PLUNGER_RISE_TIME_S); 


        float current_pull = (shootDurationS / MAX_CHARGE_TIME_S) * MAX_PULL_DISTANCE_M; 

        float new_y_pos = (current_pull * (1.0f - rise_factor)) + originalPlungerY;

        plunger->SetTransform({ plunger->GetPosition().x, new_y_pos }, 0.0f);
    }

  
    if (!is_charging && !plungerIsShooting)
    {
   
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
	UnloadTexture(texFlipperL);
	UnloadTexture(texFlipperR);

    return true;
}
