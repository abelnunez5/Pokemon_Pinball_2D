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

	Texture2D texFlipperL = { 0 }; //textura flipper izquierdo
	Texture2D texFlipperR = { 0 }; //textura flipper derecho

	float flLenPx = 65.0f; //Largo de flipper
	float flThPx = 18.0f;   //Grosor de flipper

	float pivotInsetL_texX = 25.0f; // Inseto pivote flipper izquierdo en textura
	float pivotInsetR_texX = 0.0f; // Inseto pivote flipper derecho en textura

	float pivotInsetL_texY = -2.0f; // Inseto pivote flippers en textura
	float pivotInsetR_texY = -2.0f; // Inseto pivote flippers en textura

    bool Start() override;
    update_status Update() override;
    void PlungerMovement(float dt);
    void UpdateBallAnimation(float dt);
    void Draw(float dt);
	void DrawFlippers();
    bool CleanUp() override;

private:

    ModulePhysics* physics = nullptr;
    ModulePhysics::Flipper leftFlipper{};
    ModulePhysics::Flipper rightFlipper{};
	b2Body* ball = nullptr;
	b2Body* plunger = nullptr;
    float originalPlungerY = 0.0f; 
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