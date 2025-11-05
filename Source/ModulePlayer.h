#pragma once
#include "Module.h"
#include "ModulePhysics.h"
#include <string.h>
#include "raylib.h"

class Application;

class ModulePlayer : public Module { //Declaracion de los flippers
public:

    ModulePlayer(Application* app, bool start_enabled = true);
    ~ModulePlayer() override;

    bool Start() override;
    update_status Update() override;
    void PlungerMovement(float dt);
    void UpdateBallAnimation(float dt);
    void Draw(float dt);
    bool CleanUp() override;

private:

    ModulePhysics* physics = nullptr;
    ModulePhysics::Flipper leftFlipper{};
    ModulePhysics::Flipper rightFlipper{};
	b2Body* ball = nullptr;
	b2Body* plunger = nullptr;
    float originalPlungerY = 0.0f; //aqui
    bool plungerIsShooting = false;  
    double shootStartTime = 0.0;     
    float shootDurationS = 0.0f;   
    bool canPlunger = true;
    Texture2D ballTexture{};
    Texture2D plungerTexture{};

    enum class BallState {
        RECTA,
        INCIZQRECT,
        IZQLADO,
        INCIZQABAJO,
        INCDERRECT,
        DERLADO,
		INCDERABAJO,
        ABAJO
    };

    BallState currentBallState = BallState::RECTA;

    BallState pendingBallState = BallState::RECTA;
    float stateChangeTimer = 0.0f;
};