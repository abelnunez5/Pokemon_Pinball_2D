#include "ModulePhysics.h"
#include "Application.h"
#include "ModuleRender.h"
#include "raylib.h"

// Constructor / Destructor
ModulePhysics::ModulePhysics(Application* app, bool start_enabled)
    : Module(app, start_enabled) {
}

ModulePhysics::~ModulePhysics() {}

// --- IMPLEMENTACIONES MÍNIMAS QUE EL LINKER NECESITA ---

bool ModulePhysics::Start() // Crea el mundo de Box2D si aún no existe
{ 
	TraceLog(LOG_INFO, "Creating Physics world");

   b2Vec2 gravity(0.0f, 9.8f);
   world = new b2World(gravity);
   accumulator = 0.0;

   TraceLog(LOG_INFO, "Physics world created successfully",(void*)world);

   return true;
}

update_status ModulePhysics::PreUpdate()
{
 const double dt = GetFrameTime();
 accumulator += dt;

 while (accumulator >= STEP) {
 world->Step(STEP, 8, 3);
 accumulator -= STEP;
 }
    
 return UPDATE_CONTINUE;
}

update_status ModulePhysics::PostUpdate()
{
    if (IsKeyPressed(KEY_F1)) 
        debug = !debug;
	TraceLog(LOG_INFO, "Physics debug: %s", debug ? "ON" : "OFF");

    if (debug && world) {
        DebugDrawWorld();
    }

    return UPDATE_CONTINUE;
}

bool ModulePhysics::CleanUp()
{
    // Destruye el mundo (evita fugas)
    if (world) {
        for (b2Body* b = world->GetBodyList(); b;) {
            b2Body* next = b->GetNext();
            world->DestroyBody(b);
            b = next;
        }
        delete world;
        world = nullptr;
    }
    return true;
}

ModulePhysics::Flipper ModulePhysics::CreateFlipper(float posPxX, float posPxY,
    float lengthPx, float thicknessPx,
    bool isRight) {
    ModulePhysics::Flipper f{};
    

    return f;
}

void ModulePhysics::SetFlipperPressed(ModulePhysics::Flipper& f, bool pressed) {
    
}

// helpers básicos, si se usan en otro lado se pueden eliminar de aquí

b2Body* ModulePhysics::CreateCircleBody(float mx, float my, float mr, bool dynamic)
{
    if (!world) return nullptr;
    b2BodyDef bd; bd.type = dynamic ? b2_dynamicBody : b2_staticBody; bd.position.Set(mx, my);
    b2Body* body = world->CreateBody(&bd);
    b2CircleShape sh; sh.m_radius = mr;
    b2FixtureDef fd; fd.shape = &sh; fd.density = dynamic ? 1.0f : 0.0f; fd.friction = 0.2f; fd.restitution = 0.5f;
    body->CreateFixture(&fd);
    return body;
}

b2Body* ModulePhysics::CreateBoxBody(float mx, float my, float mw, float mh, bool dynamic, float angleRad)
{
    if (!world) return nullptr;
    b2BodyDef bd; bd.type = dynamic ? b2_dynamicBody : b2_staticBody; bd.position.Set(mx, my); bd.angle = angleRad;
    b2Body* body = world->CreateBody(&bd);
    b2PolygonShape sh; sh.SetAsBox(mw * 0.5f, mh * 0.5f);
    b2FixtureDef fd; fd.shape = &sh; fd.density = dynamic ? 1.0f : 0.0f; fd.friction = 0.5f; fd.restitution = dynamic ? 0.2f : 0.0f;
    body->CreateFixture(&fd);
    return body;
}

void ModulePhysics::DestroyBody(b2Body* body)
{
    if (world && body) world->DestroyBody(body);
}

Vector2 ModulePhysics::GetBodyPosition(b2Body* body) const
{
    if (!body) return { 0,0 };
    b2Vec2 p = body->GetPosition();
    return { M2P(p.x), M2P(p.y) };
}

void ModulePhysics::RenderDebug()
{
    if (!debug || !world) return;

    //Prueba para mostrar cuántos cuerpos hay en el mundo
    int count = world->GetBodyCount();
    DrawText(TextFormat("Bodies: %d", count), 30, 30, 40, WHITE); 

    DebugDrawWorld();
}

void ModulePhysics::DebugDrawWorld()
{
    if (!world) return;
    for (b2Body* b = world->GetBodyList(); b; b = b->GetNext()) {
        const b2Transform& xf = b->GetTransform();
        for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext()) {
            switch (f->GetType()) {
            case b2Shape::e_circle: {
                const b2CircleShape* c = (const b2CircleShape*)f->GetShape();
                b2Vec2 center = b2Mul(xf, c->m_p);
                DrawCircleLines((int)M2P(center.x), (int)M2P(center.y), (int)M2P(c->m_radius), GREEN);
            } break;
            case b2Shape::e_polygon: {
                const b2PolygonShape* poly = (const b2PolygonShape*)f->GetShape();
                for (int i = 0; i < poly->m_count; ++i) {
                    b2Vec2 v1 = b2Mul(xf, poly->m_vertices[i]);
                    b2Vec2 v2 = b2Mul(xf, poly->m_vertices[(i + 1) % poly->m_count]);
                    DrawLine((int)M2P(v1.x), (int)M2P(v1.y), (int)M2P(v2.x), (int)M2P(v2.y), GREEN);
                }
            } break;
            default: break;
            }
        }
    }
}