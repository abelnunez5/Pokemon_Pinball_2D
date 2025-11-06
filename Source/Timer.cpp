// ----------------------------------------------------
// j1Timer.cpp
// Fast timer with milisecons precision
// ----------------------------------------------------

#include "Timer.h"
#include "DebugLeak.h"
#include "raylib.h"

Timer::Timer()
{
	Start();
}

void Timer::Start()
{
	started_at = GetTime();
}

double Timer::ReadSec() const
{
	return (GetTime() - started_at);
}