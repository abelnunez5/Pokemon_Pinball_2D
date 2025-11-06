#pragma once
#include "Module.h"
#include "raylib.h"
#include <box2d/box2d.h>

class ModuleGame;

// Categorías de colisión
enum CollisionCategory : uint16 {
    CAT_TABLE = 0x0001, // paredes / tablero
    CAT_BALL = 0x0002, // bola
    CAT_FLIPPER = 0x0004, // palas del flipper
};

enum class BodyType
{
    BASE,
    BALL, 
    BOUNCER,
    PAD
};

class ModulePhysics : public Module, public b2ContactListener {
public:
    ModulePhysics(Application* app, bool start_enabled = true);
    ~ModulePhysics() override;

    bool Start() override;
    update_status PreUpdate() override;
    update_status PostUpdate() override;
    bool CleanUp() override;

    void BeginContact(b2Contact* contact) override;

    // Helpers simples
    b2Body* CreateCircleBody(float mx, float my, float mr, bool dynamic, BodyType type = BodyType::BASE);
    b2Body* CreateBoxBody(float mx, float my, float mw, float mh, bool dynamic, float angleRad = 0.0f, float restitution = -1.0f, BodyType type = BodyType::BASE);
    void    DestroyBody(b2Body* body);
    Vector2 GetBodyPosition(b2Body* body) const;

    void SetDebug(bool enabled) { debug = enabled; }
    void RenderDebug();

    b2Body* CreateChain(int x, int y, int* coordinates, int vertex_count); // Crear cadenas a partir de coords en pixeles

    void CreateThickerChain(int x, int y, int* coordinates, int vertex_count, float thickness_px, float restitution = -1.0f, BodyType type = BodyType::BASE);
   
    struct Flipper 
    {
        b2Body* anchor = nullptr;
        b2Body* blade  = nullptr;
        b2RevoluteJoint* joint = nullptr;
        bool isRight = false;
    };

    int GetBodyCount() const { return world ? world->GetBodyCount() : 0; } // Cuantos cuerpos hay ? 
    Flipper CreateFlipper(float posPxX, float posPxY,
                          float lengthPx, float thicknessPx,
                          bool isRight);

    void SetFlipperPressed(Flipper& f, bool pressed);

private:
    b2World* world = nullptr;
    bool     debug = false;

    double accumulator = 0.0;
    static constexpr float STEP = 1.0f / 60.0f;

public: 
    static constexpr float PIXELS_PER_METER = 50.0f;
    static inline float M2P(float m) { return m * PIXELS_PER_METER; }
    static inline float P2M(float p) { return p / PIXELS_PER_METER; }

private:
    void DebugDrawWorld();
};