#include "ModulePhysics.h"
#include "Application.h"
#include "ModuleRender.h"
#include "raylib.h"

// Constructor / Destructor
ModulePhysics::ModulePhysics(Application* app, bool start_enabled)
    : Module(app, start_enabled) {
}

ModulePhysics::~ModulePhysics() {}

bool ModulePhysics::Start() // Crea el mundo de Box2D si aún no existe
{ 
	TraceLog(LOG_INFO, "Creating Physics world"); //que nos diga si ha creado las fisicas 

   b2Vec2 gravity(0.0f, 9.8f);
   world = new b2World(gravity);
   accumulator = 0.0;

   TraceLog(LOG_INFO, "Physics world created successfully",(void*)world); // que nos diga cuando ha creado las fisicas 

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
	//TraceLog(LOG_INFO, "Physics debug: %s", debug ? "ON" : "OFF"); //que nos diga si estan encendidas o apagadas 

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
    bool isRight)
{
    Flipper f{};
    f.isRight = isRight;
    if (!world) return f;

    // Convertir píxeles → metros
    const float x = P2M(posPxX);
    const float y = P2M(posPxY);
    const float halfLen = P2M(lengthPx) * 0.5f;
    const float halfThk = P2M(thicknessPx) * 0.5f;

    //Cuerpo ancla (estático)
    b2BodyDef anchorDef;
    anchorDef.type = b2_staticBody;
    anchorDef.position.Set(x, y);
    f.anchor = world->CreateBody(&anchorDef);
    {
        b2CircleShape c;
        c.m_radius = P2M(2.0f); // círculo pequeño para visualizar el pivote
        b2FixtureDef fd; fd.shape = &c; fd.density = 0.0f;
        fd.isSensor = true;
        f.anchor->CreateFixture(&fd);
    }

    //Cuerpo pala (dinámico, caja)
    b2BodyDef bladeDef;
    bladeDef.type = b2_dynamicBody;
    if (isRight) {
        // flipper derecho: el ancla está en el extremo izquierdo de la pala
        bladeDef.position.Set(x + halfLen, y);
    }
    else {
        // flipper izquierdo: el ancla está en el extremo derecho de la pala
        bladeDef.position.Set(x - halfLen, y);
	}
    f.blade = world->CreateBody(&bladeDef);

    b2PolygonShape box;
    // Caja centrada en el origen del blade (lo anclaremos a un extremo con la junta)
    box.SetAsBox(halfLen, halfThk);
    {
        b2FixtureDef fd;
        fd.shape = &box;
        fd.density = 2.0f;
        fd.friction = 0.3f;
        fd.restitution = 0.0f;
        fd.filter.categoryBits = CAT_FLIPPER;
        fd.filter.maskBits = CAT_BALL;
        f.blade->CreateFixture(&fd);
    }

    // Anclamos el blade al anchor en el extremo apropiado
    b2RevoluteJointDef jd;
	jd.collideConnected = false;    
    jd.bodyA = f.anchor;
    jd.bodyB = f.blade;
    jd.collideConnected = false;

    // Punto de anclaje en mundo (en el ancla)
    jd.localAnchorA.Set(0.0f, 0.0f);

    // Punto de anclaje en la pala: extremo izquierdo o derecho
    // Si la caja del blade está centrada, su extremo está a +/- halfLen en X.
    if (isRight) {
        jd.localAnchorB.Set(-halfLen, 0.0f);
    }
    else {
        jd.localAnchorB.Set(+halfLen, 0.0f);
    }

    // Límites y motor
    jd.enableLimit = true;

    // AJUSTAR RANGOS:
    const float deg = 35.0f * (3.1415926f / 180.0f);
    if (isRight) {
        // flipper derecho suele girar de “abajo” a “arriba” con ángulo negativo
        jd.lowerAngle = -deg; // abajo
        jd.upperAngle = +deg; // arriba
    }
    else {
        // flipper izquierdo
        jd.lowerAngle = -deg;
        jd.upperAngle = +deg;
    }

    jd.enableMotor = true;
    jd.maxMotorTorque = 200.0f; // sube/baja 
    jd.motorSpeed = 0.0f; // en reposo

    f.joint = (b2RevoluteJoint*)world->CreateJoint(&jd);

    return f;
}

void ModulePhysics::SetFlipperPressed(ModulePhysics::Flipper& f, bool pressed)
{
    if (!f.joint) return;
	if (f.blade) f.blade->SetAwake(true); // asegurar que el flipper está despierto

    // Velocidad del motor
    const float upSpeed = 20.0f;
    const float downSpeed = -12.0f;

    if (pressed) {
        // Subir rápido
        f.joint->EnableMotor(true);
		// Orientacion del flipper para saber el signo
        float s = f.isRight ? -upSpeed : upSpeed;
        f.joint->SetMotorSpeed(s);
    }
    else {
        // Bajar por motor 
        f.joint->EnableMotor(true);
		float s = f.isRight ? -downSpeed : downSpeed; // diferente signo
        f.joint->SetMotorSpeed(s);
    }
}

// helpers básicos, !! si se usan en otro lado se pueden eliminar de aquí !!

b2Body* ModulePhysics::CreateCircleBody(float mx, float my, float mr, bool dynamic)
{
    if (!world) return nullptr;
    b2BodyDef bd; bd.type = dynamic ? b2_dynamicBody : b2_staticBody; bd.position.Set(mx, my);
    b2Body* body = world->CreateBody(&bd);
    b2CircleShape sh; sh.m_radius = mr;
    b2FixtureDef fd; fd.shape = &sh; fd.density = dynamic ? 1.0f : 0.0f; fd.friction = 0.2f; fd.restitution = 0.5f;
	fd.filter.categoryBits = CAT_BALL;
	fd.filter.maskBits = CAT_TABLE | CAT_FLIPPER;
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
	if (!dynamic) {fd.filter.categoryBits = CAT_TABLE;
    fd.filter.maskBits = CAT_BALL;
	}
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
    DrawText(TextFormat("Bodies: %d", count), 30, 30, 40, BLACK); 

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
            case b2Shape::e_chain: {
                const b2ChainShape* chain = (const b2ChainShape*)f->GetShape();
                for (int i = 0; i < chain->m_count - 1; ++i)
                {
                    b2Vec2 v1 = b2Mul(xf, chain->m_vertices[i]);
                    b2Vec2 v2 = b2Mul(xf, chain->m_vertices[i + 1]);
                    DrawLine((int)M2P(v1.x), (int)M2P(v1.y), (int)M2P(v2.x), (int)M2P(v2.y), GREEN);
                }
            } break;

            default: break;
            }
        }
    }
}

b2Body* ModulePhysics::CreateChain(int x, int y, int* coordinates, int vertex_count)
{
    if (!world) return nullptr;

    b2Vec2* vertices = new b2Vec2[vertex_count];    // Array temp para guardar los vertices de la cadena en metros

    for (int i = 0; i < vertex_count; i++)
    {
        float x_meters = P2M((float)coordinates[i * 2]);
        float y_meters = P2M((float)coordinates[i * 2 + 1]);

        vertices[i].Set(x_meters, y_meters);
    }

    b2BodyDef bd;
    bd.type = b2_staticBody;
    bd.position.Set(P2M((float)x), P2M((float)y));  // Pos si hubiera un offset
    b2Body* body = world->CreateBody(&bd);

    b2ChainShape shape;
    shape.CreateChain(vertices, vertex_count, b2Vec2_zero, b2Vec2_zero);

    b2FixtureDef fd;
    fd.shape = &shape;
    fd.density = 0.0f;
    fd.friction = 0.5f; 
    fd.restitution = 0.2f;
    body->CreateFixture(&fd);

    delete[] vertices;

    return body;
}

void ModulePhysics::CreateThickerChain(int x, int y, int* coordinates, int vertex_count, float thickness_px)
{
    if (!world) return;

    float thickness_m = P2M(thickness_px);  //  Para pasar el grosor de pixeles a metros

    for (int i = 0; i < vertex_count - 1; ++i)
    {
        float v1_x_px = (float)coordinates[i * 2] + x;  //  x del primer vewrtice en pixeles
        float v1_y_px = (float)coordinates[i * 2 + 1] + y;  //  y del primer vertice en pixeles
        float v2_x_px = (float)coordinates[(i + 1) * 2] + x;    //  x del segundo vertice en pixeles
        float v2_y_px = (float)coordinates[(i + 1) * 2 + 1] + y;    //  y del primer vertice en pixeles

        float center_x_px = (v1_x_px + v2_x_px) / 2.0f; //  coordenada en x del centro de la caja
        float center_y_px = (v1_y_px + v2_y_px) / 2.0f; //  coordenada en x del centro de la caja

        float dx_px = v2_x_px - v1_x_px;
        float dy_px = v2_y_px - v1_y_px;
        float length_px = sqrtf(dx_px * dx_px + dy_px * dy_px);

        float angle_rad = atan2f(dy_px, dx_px); //  angulo en el que debe estar la caja

        float center_mx = P2M(center_x_px);
        float center_my = P2M(center_y_px);
        float length_m = P2M(length_px);

        CreateBoxBody(center_mx, center_my, length_m, thickness_m, false, angle_rad);
    }
}