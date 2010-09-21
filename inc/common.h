#pragma once

#ifdef LINUX_BUILD
#define START_READ if(DF && !DF->isSuspended()){ DF->Suspend(); }
#define END_READ if(DF && DF->isSuspended()){ DF->Resume(); }
#else
    #define START_READ
    #define END_READ
#endif

#define DFHACK_WANT_TILETYPES
#include "dfhack/library/include/DFHack.h"
#include "dfhack/library/include/dfhack/modules/WindowIO.h"

struct cursorIdx{
    int32_t x;
    int32_t y;
    int32_t z;
};
struct tileIdx{
    uint32_t x;
    uint32_t y;
    uint32_t z;
};
