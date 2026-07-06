#include "ultra64.h"
#include "bondtypes.h"

PadRecord padlist[];
BoundPadRecord pad3dlist[];
s32 propDefs[];
s32 intro[];
waygroup pathsets[];
waypoint pathwaypoints[];
PathRecord patrolpaths[];
AIListRecord ailists[];

stagesetup UsetuptestZ = {
    &pathwaypoints,
    &pathsets,
    &intro,
    &propDefs,
    &patrolpaths,
    &ailists,
    &padlist,
    &pad3dlist,
    NULL,
    NULL,
};

PadRecord padlist[] = {
    {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, "p1a", 0},
    {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, NULL, 0},
};

BoundPadRecord pad3dlist[] = {
    {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, NULL, 0,
     {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
};

s32 propDefs[] = {
    _mkword(0, _mkshort(0, 48)),
};

s32 intro[] = {
    _mkword(0, _mkshort(0, 0)), 0, 0,
    _mkword(0, _mkshort(0, 5)), 2,
    _mkword(0, _mkshort(0, 1)), 0, -1, 0,
    _mkword(0, _mkshort(0, 9)),
};

waygroup pathsets[] = {
    {NULL, NULL, 0},
};

waypoint pathwaypoints[] = {
    {0xffffffff, NULL, 0x00000000, 0x00000000},
};

PathRecord patrolpaths[] = {
    {NULL, 0x00, 0x00, 0x0000},
};

AIListRecord ailists[] = {
    {NULL, 0x00000000},
};
