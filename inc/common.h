#pragma once

#define DFHACK_WANT_TILETYPES
#include "dfhack/library/include/DfHack.h"
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