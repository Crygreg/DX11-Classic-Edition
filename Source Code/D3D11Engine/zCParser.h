#include "pch.h"
#include "HookedFunctions.h"
#include "Engine.h"
#include "GothicAPI.h"
#include "zSTRING.h"

#ifndef BUILD_GOTHIC_1_08k
#define zCPARSER_CALL_FUNC(symbolId, ...)  do {reinterpret_cast<void(*)(zCParser*, ...)>(GothicMemoryLocations::zCParser::CallFunc)(zCParser::GetParser(), symbolId, __VA_ARGS__); } while (0)
#else
#define zCPARSER_CALL_FUNC(symbolId, ...)
#endif

class zCParser {
public:
#if defined(BUILD_GOTHIC_1_08k) || defined(BUILD_SPACER)
    static void CallFunc( int symbolId, ... ) {

    }

    static zCParser* GetParser() { return nullptr; }
#else
    static zCParser* GetParser() { return reinterpret_cast<zCParser*>(GothicMemoryLocations::GlobalObjects::zCParser); }
#endif
};
