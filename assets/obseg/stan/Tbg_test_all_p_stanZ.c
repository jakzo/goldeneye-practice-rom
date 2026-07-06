#include <ultra64.h>
#include <game/stan.h>

StandTile tile_0;

StandFileHeader Tbg_test_all_p_stanZ = {
    NULL,
    &tile_0,
    {0x00, 0x00, 0x00, 0x00},
};

StandTile tile_0 = {
    0x000100, 0x01,
    0x0,
    0xf, 0xf, 0xf,
    3,
    0x0, 0x1, 0x2,
    {
        {-30000, 0, -30000, 0x0000},
        {     0, 0,  30000, 0x0000},
        { 30000, 0, -30000, 0x0000},
    }
};

u32 tile_end[] = {
    0x00000000,
    0x00000000,
};

StandFileFooter footer = {
    "unstric",
    NULL,
    NULL,
    NULL,
    NULL,
};

u32 *keep_tile_end = tile_end;
