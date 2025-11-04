#pragma once
#include "Module.h"
#include "ModulePhysics.h"


class Application;

class ModulePlayer : public Module { //Declaracion de los flippers
public:

    ModulePlayer(Application* app, bool start_enabled = true);
    ~ModulePlayer() override;

    bool Start() override;
    update_status Update() override;
    bool CleanUp() override;

private:

    ModulePhysics* physics = nullptr;
    ModulePhysics::Flipper leftFlipper{};
    ModulePhysics::Flipper rightFlipper{};
};