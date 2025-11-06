#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include <math.h>

ModuleRender::ModuleRender(Application* app, bool start_enabled) : Module(app, start_enabled)
{
    background = RAYWHITE;
    camera = {0, 0, 0, 0 };
}

// Destructor
ModuleRender::~ModuleRender()
{}

// Called before render is available
bool ModuleRender::Init()
{
	LOG("Creating Renderer context");
	bool ret = true;

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRender::PreUpdate()
{
	return UPDATE_CONTINUE;
}

// Update: debug camera
update_status ModuleRender::Update()
{
    ClearBackground(background);

    // NOTE: This function setups render batching system for
    // maximum performance, all consecutive Draw() calls are
    // not processed until EndDrawing() is called
    BeginDrawing();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRender::PostUpdate()
{
    // Draw everything in our batch!
    DrawFPS(10, 10);

    EndDrawing();

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	return true;
}

void ModuleRender::SetBackgroundColor(Color color)
{
	background = color;
}

// Draw to screen
bool ModuleRender::Draw(Texture2D texture, int x, int y, const Rectangle* section,
    double angle, int pivot_x, int pivot_y,
    float dest_w, float dest_h) const
{
    if (texture.id == 0) return false;

    // Rect fuente
    Rectangle src = { 0.f, 0.f, (float)texture.width, (float)texture.height };
    if (section != NULL) src = *section;

    // Si no se pasan dimensiones destino, usa las del src
    if (dest_w <= 0.0f) dest_w = src.width;
    if (dest_h <= 0.0f) dest_h = src.height;

    // Rect destino
    Rectangle dst = { (float)x + camera.x, (float)y + camera.y, dest_w, dest_h };

    // Pivote de rotación (coordenada local al dst)
    Vector2 origin = { (float)pivot_x, (float)pivot_y };

    DrawTexturePro(texture, src, dst, origin, (float)angle, WHITE);
    return true;
}

bool ModuleRender::DrawText(const char * text, int x, int y, Font font, int spacing, Color tint) const
{
    bool ret = true;

    Vector2 position = { (float)x, (float)y };

    DrawTextEx(font, text, position, (float)font.baseSize, (float)spacing, tint);

    return ret;
}
