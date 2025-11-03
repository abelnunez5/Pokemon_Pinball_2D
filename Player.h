#pragma once
#include "Source/Module.h"
#include "Source/ModulePhysics.h"

class Application;

class ModulePlayer : public Module {
public:
    ModulePlayer(Application* app, bool start_enabled = true);
    ~ModulePlayer() override;

    bool Start() override;
    update_status Update() override;
    bool CleanUp() override;

private:
    // añade solo esto
    ModulePhysics* physics = nullptr;
    ModulePhysics::Flipper leftFlipper{};
    ModulePhysics::Flipper rightFlipper{};
};