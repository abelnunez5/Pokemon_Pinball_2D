//#pragma once
//
//
//#if defined(_MSC_VER) && defined(_DEBUG)
//
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//
//// Etiqueta los new con archivo/línea (no afecta al placement-new)
//#ifndef DBG_NEW
//#define DBG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
//#define new DBG_NEW
//#endif
//
//inline void EnableCrtLeakChecks()
//{
//    int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
//    flags |= _CRTDBG_ALLOC_MEM_DF;    // tracking
//    flags |= _CRTDBG_LEAK_CHECK_DF;   // volcado al terminar el proceso
//    _CrtSetDbgFlag(flags);
//
//    // Manda los reportes a la ventana Output (Debug) de VS
//    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
//    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
//#ifdef _CRT_ASSERT
//    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
//#endif
//}
//
//inline void DumpLeaksNow()
//{
//    _CrtDumpMemoryLeaks();
//}
//
//#else
//  // En Release u otros compiladores: no hace nada
//inline void EnableCrtLeakChecks() {}
//inline void DumpLeaksNow() {}
//#endif