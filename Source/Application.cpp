
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleGame.h"
#include "Application.h"
//#include <stdlib.h>
//#include <crtdbg.h>

/////////////////////////////////////////////////////////////////////////////////////////////////
// START BLOQUE CRT LEAKS 
#if defined(_MSC_VER) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC          
#include <stdlib.h>
#include <crtdbg.h>

// para que NUNCA de "identifier not found"
#define ENABLE_CRT_LEAKS()							 \
do {                                                 \
int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);     \
flags |= _CRTDBG_ALLOC_MEM_DF;                       \
flags |= _CRTDBG_LEAK_CHECK_DF;                      \
_CrtSetDbgFlag(flags);                               \
_CrtSetReportMode(_CRT_WARN,  _CRTDBG_MODE_DEBUG);   \
_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);   \
_CrtSetReportMode(_CRT_ASSERT,_CRTDBG_MODE_DEBUG);   \
} while(0)

#define DUMP_CRT_LEAKS() _CrtDumpMemoryLeaks()
#else
	// En Release/otros 
#define ENABLE_CRT_LEAKS()  do{}while(0)
#define DUMP_CRT_LEAKS()    do{}while(0)
#endif
// FIN BLOQUE CRT LEAKS
/////////////////////////////////////////////////////////////////////////////////////////////////

Application::Application()
{
	window = new ModuleWindow(this);
	renderer = new ModuleRender(this);
	audio = new ModuleAudio(this, true);
	physics = new ModulePhysics(this);
	scene_intro = new ModuleGame(this);
	player = new ModulePlayer(this, true);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	/*AddModule(player);*/
	AddModule(physics);
	AddModule(audio);

	// Scenes
	AddModule(scene_intro);

	// Rendering happens at the end
	AddModule(renderer);
	AddModule(player);
}

Application::~Application()
{
	for (auto it = list_modules.rbegin(); it != list_modules.rend(); ++it)
	{
		Module* item = *it;
		delete item;
	}
	list_modules.clear();
	
}

bool Application::Init()
{
	bool ret = true;
	ENABLE_CRT_LEAKS();

	// Call Init() in all modules
	for (auto it = list_modules.begin(); it != list_modules.end() && ret; ++it)
	{
		Module* module = *it;
		ret = module->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");

	for (auto it = list_modules.begin(); it != list_modules.end() && ret; ++it)
	{
		Module* module = *it;
		ret = module->Start();
	}
	
	SetTargetFPS(60);

	return ret;
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;

	for (auto it = list_modules.begin(); it != list_modules.end() && ret == UPDATE_CONTINUE; ++it)
	{
		Module* module = *it;
		if (module->IsEnabled())
		{
			ret = module->PreUpdate();
		}
	}

	for (auto it = list_modules.begin(); it != list_modules.end() && ret == UPDATE_CONTINUE; ++it)
	{
		Module* module = *it;
		if (module->IsEnabled())
		{
			ret = module->Update();
		}
	}

	for (auto it = list_modules.begin(); it != list_modules.end() && ret == UPDATE_CONTINUE; ++it)
	{
		Module* module = *it;
		if (module->IsEnabled())
		{
			ret = module->PostUpdate();
		}
	}

	if (WindowShouldClose()) ret = UPDATE_STOP;

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	for (auto it = list_modules.rbegin(); it != list_modules.rend() && ret; ++it)
	{
		Module* item = *it;
		ret = item->CleanUp();
	}
	
	DUMP_CRT_LEAKS();
	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.emplace_back(mod);
}