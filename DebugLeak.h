#pragma once


#if defined(_MSC_VER) && defined(_DEBUG)

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW
#define DBG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DBG_NEW
#endif

inline void EnableCrtLeakChecks()
{
    // Volcado automático de fugas al terminar el proceso
    int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    flags |= _CRTDBG_ALLOC_MEM_DF;
    flags |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(flags);

    // Mandar avisos a la ventana Output de VS
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#ifdef _CRT_ASSERT
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
#endif
}

inline void DumpLeaksNow()
{
    _CrtDumpMemoryLeaks();
}

#else
  // En Release / otros compiladores
inline void EnableCrtLeakChecks() {}
inline void DumpLeaksNow() {}
#endif