#pragma once
#include "DebugLeak.h"
#include "Module.h"
#include "Globals.h"

#include <limits.h>

class ModuleRender : public Module
{
public:
	ModuleRender(Application* app, bool start_enabled = true);
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

    void SetBackgroundColor(Color color);
	bool Draw(Texture2D texture, int x, int y, const Rectangle* section = NULL, double angle = 0, int pivot_x = 0, int pivot_y = 0, float dest_w = 0.0f, float dest_h = 0.0f) const; // dibujar textura en posicion (x, y) con seccion opcional, rotación de angulo, punto en pivote, y tamaño del destino
	bool DrawText(const char* text, int x, int y, Font font, int spacing, Color tint) const; // dibujar texto en posicion (x, y) con fuente, espaciado y color

public:

	Color background;
    Rectangle camera;
};