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
    void UpdateBallAnimation(float dt);
    void Draw(float dt);
    bool CleanUp() override;

private:

    ModulePhysics* physics = nullptr;
    ModulePhysics::Flipper leftFlipper{};
    ModulePhysics::Flipper rightFlipper{};
	b2Body* ball = nullptr;
    Texture2D ballTexture{};

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