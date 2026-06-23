#line 1 "src/game/gun.c"
#include <ultra64.h>
#include "include/limits.h"
#include <bondconstants.h>
#include <bondtypes.h>
#include <bondgame.h>
#include <music.h>
#include <snd.h>
#include "bondview.h"
#include "bondinv.h"
#include "gun.h"
#include "chrobjdata.h"
#include "game/chrobjhandler.h"
#include "game/objective_status.h"
#include "quaternion.h"
#include "image_bank.h"
#include "bondwalk2.h"
#include "othermodemicrocode.h"
#include "player.h"
#include "lvl.h"
#include "random.h"
#include "math_asinfacosf.h"
#include "loadobjectmodel.h"
#include "objecthandler.h"
#include "image.h"
#include "unk_0CC4C0.h"
#include "debugmenu_handler.h"
#include "fr.h"
#include "assets/obseg/text/LgunE.h"
#include "textrelated.h"

// bss
s32 dword_CODE_bss_80075DB0;
s32 dword_CODE_bss_80075DB4;
ALSoundState* dword_CODE_bss_80075DB8[4];

CasingRecord g_Casings[20];
s32 dword_CODE_bss_80076A48;


#ifdef REFRESH_PAL
    /* PAL */
    #define THROWN_ITEM_REFRESH_RATE                   50
    #define THROWN_ITEM_TIMER_SOLO                     250
    #define THROWN_ITEM_TIMER_MULTI                    150
    #define THROWN_ITEM_TIMER_DEFAULT                  200
    #define DUAL_WIELD_TRIGGER_SWAP_TICKS 24
    #define DUAL_WIELD_SINGLE_TRIGGER_SWAP_TICKS 36
    #define WATCH_SOUND_DURATION_TICKS 250
#else
    /* NTSC */
    #define THROWN_ITEM_REFRESH_RATE                   60
    #define THROWN_ITEM_TIMER_SOLO                     300
    #define THROWN_ITEM_TIMER_MULTI                    180
    #define THROWN_ITEM_TIMER_DEFAULT                  240
    #define DUAL_WIELD_TRIGGER_SWAP_TICKS 20
    #define DUAL_WIELD_SINGLE_TRIGGER_SWAP_TICKS 30
    #define WATCH_SOUND_DURATION_TICKS 300
#endif




// data
////D:80032440
//rgba_u8 D_80032440[] = {
//	{0x96, 0x96, 0x96, 0},
//	{0x96, 0x96, 0x96, 0}
//};
//
////D:80032448
//rgba_u8 D_80032448[] = {
//	{0xFF, 0xFF, 0xFF, 0},
//	{0xFF, 0xFF, 0xFF, 0},
//	{0xB2, 0x4D, 0x2E, 0}
//};
Lights1 D_80032440 = gdSPDefLights1(0x96, 0x96, 0x96, 0xff, 0xff, 0xff, 0xb2, 0x4d, 0x2e);
//D:80032454
//u32 D_80032454 = 0;

//D:80032458
u32 D_80032458 = 0;

//D:8003245C
u32 size_item_buffer[] = {0x14820, 0x14820};

//D:80032464
u32 D_80032464[] ={0x7530, 0x7530};



//D:8003246C
CartridgeModelFileRecord ejected_cartridge[] = {
	{&cartridge_header, "GcartridgeZ"},
	{&cartrifle_header, "GcartrifleZ"},
	{&cartblue_header, "GcartblueZ"},
	{&cartshell_header, "GcartshellZ"},
	{0, ""}
};

#include <assets/obseg/gun/gunWeaponStats.inc.c>

//D:80033924
#include <assets/obseg/gun/gunModelFileRecord.inc.c>

//D:80034C9C
u32 cartridges_eject = 0;
//D:80034CA0
u32 D_80034CA0 = 0;

//D:80034CA4
u32 D_80034CA4[] = {
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,    0x3F000000,
	0x41000000,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	0x3F000000,    0x41000000,           0x0,    0x40C00000,
	0xBFC00000,           0x0,    0x40B487B1,    0x3E70C0AD,
	0x3E0AE536,    0x3F000000,    0x41000000,           0x0,
	0x41480000,    0xC0600000,           0x0,    0x40C159EC,
	0x3D374BC7,    0x3F0E4378,    0x3F000000,    0x41000000,
	       0x0,    0xC1200000,    0xC1300000,           0x0,
	0x3F9ED962,    0x3EA24C40,    0x3F8B0DF1,    0x3F000000,
	0x41000000,           0x0,    0xC1600000,    0xC1700000,
	       0x0,    0x3FEA4780,    0x40C498E3,    0x3FA316D3,
	0x3F000000,    0x41200000,           0x0,    0xBF800000,
	0xC1100000,           0x0,    0x3EC4BBA1,    0x3EB87C42,
	0x3DD75968,    0x3F000000,    0x41200000,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,    0x3F000000,    0x41A00000,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,    0x3F000000,
	0x41A00000,           0x1,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	       0,           0
};

u32 D_80034E0C[] = {
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,    0x3F000000,
	0x41000000,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	0x3F000000,    0x41000000,           0x0,    0xC1080000,
	0xC0C00000,           0x0,    0x40AF7506,    0x40BAB4B9,
	0x40C2A5C2,    0x3F000000,    0x41000000,           0x0,
	0xC0400000,    0xC0600000,           0x0,    0x3ECE08F2,
	0x40B75721,    0x40B62409,    0x3F000000,    0x41000000,
	       0x0,    0xBF000000,    0xC1080000,           0x0,
	0x3F9DFD7A,    0x40B768CD,    0x40B37BDF,    0x3F000000,
	0x41000000,           0x0,    0x40E00000,    0xC1E40000,
	0xBFC00000,    0x3FA74949,    0x40B63EBC,    0x40B6443D,
	0x3F000000,    0x41200000,           0x0,    0xBFC00000,
	0xC1100000,           0x0,    0x3D8ADEEC,    0x40C84E72,
	0x3E506749,    0x3F000000,    0x41200000,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,    0x3F000000,    0x41A00000,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,    0x3F000000,
	0x41A00000,           0x1,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
           0x0,           0x0
};

u32 D_80034F74[] = {
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,    0x3F000000,
	0x41000000,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	0x3F000000,    0x41000000,           0x0,           0x0,
	       0x0,    0x40900000,    0x40B2719D,           0x0,
	       0x0,    0x3F000000,    0x41000000,           0x0,
	       0x0,           0x0,    0x41A40000,    0x40A8630B,
	       0x0,           0x0,    0x3F000000,    0x41000000,
	       0x0,           0x0,    0x40400000,    0x40B00000,
	0x3D008312,           0x0,           0x0,    0x3F000000,
	0x41000000,           0x1,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
           0x0,           0x0
};

u32 D_8003504C[] = {

           0x0,           0x0,           0x0,    0x40900000,
    0x40B2719D,           0x0,           0x0,    0x3F000000,
    0x41000000,           0x0,           0x0,           0x0,
    0x41A40000,    0x40A8630B,           0x0,           0x0,
    0x3F000000,    0x41000000,           0x0,           0x0,
    0x40400000,    0x40B00000,    0x3D008312,           0x0,
           0x0,    0x3F000000,    0x41000000,           0x0,
           0x0,    0xC1A00000,    0x41900000,    0x3F4913C7,
           0x0,           0x0,    0x3F000000,    0x41A00000,
           0x0,           0x0,    0xC1A00000,    0x41900000,
    0x3F4913C7,           0x0,           0x0,    0x3F000000,
    0x41A00000,           0x1,           0x0,           0x0,
           0x0,           0x0,           0x0,           0x0,
           0x0,           0x0
};

u32 D_80035124[] = {
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,    0x3F000000,
	0x40800000,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	0x3F000000,    0x40800000,           0x0,    0x41200000,
	0x41480000,    0x418C0000,           0x0,           0x0,
	       0x0,    0x3F000000,    0x40800000,           0x0,
	0x41200000,    0x420A0000,    0x41CC0000,           0x0,
	       0x0,           0x0,    0x3F000000,    0x41200000,
	       0x0,    0x41200000,    0x420A0000,    0x41CC0000,
	       0x0,           0x0,           0x0,    0x3F000000,
	0x41200000,           0x1,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0
};

u32 D_800351FC[] = {
	       0x0,    0x41200000,    0x420A0000,    0x41CC0000,
	       0x0,           0x0,           0x0,    0x3F000000,
	0x41200000,           0x0,    0x41200000,    0x420A0000,
	0x41CC0000,           0x0,           0x0,           0x0,
	0x3F000000,    0x41200000,           0x0,    0x41200000,
	0x41480000,    0x418C0000,           0x0,           0x0,
	       0x0,    0x3F000000,    0x41200000,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,    0x3F000000,    0x41200000,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,    0x3F000000,
	0x41200000,           0x1,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0
};

u32 D_800352D4[] = {
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,    0x3F000000,
	0x40800000,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	0x3F000000,    0x40800000,           0x0,           0x0,
	       0x0,    0x40900000,    0x40B2719D,           0x0,
	       0x0,    0x3F000000,    0x40800000,           0x0,
	       0x0,           0x0,    0x41A40000,    0x40A8630B,
	       0x0,           0x0,    0x3F000000,    0x41000000,
	       0x0,           0x0,    0x40400000,    0x40B00000,
	0x3D008312,           0x0,           0x0,    0x3F000000,
	0x41000000,           0x1,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0
};

u32 D_800353AC[63] = {
	       0x0,           0x0,           0x0,    0x40900000,
	0x40B2719D,           0x0,           0x0,    0x3F000000,
	0x41000000,           0x0,           0x0,           0x0,
	0x41A40000,    0x40A8630B,           0x0,           0x0,
	0x3F000000,    0x41000000,           0x0,           0x0,
	0x40400000,    0x40B00000,    0x3D008312,           0x0,
	       0x0,    0x3F000000,    0x41000000,           0x0,
	       0x0,    0xC1A00000,    0x41900000,    0x3F4913C7,
	       0x0,           0x0,    0x3F000000,    0x41000000,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,    0x3F000000,
	0x41A00000,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	0x3F000000,    0x41A00000,           0x1,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0
};

u32 D_800354A8[90] = {
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,    0x3F000000,
	0x41200000,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	0x3F000000,    0x41200000,           0x0,    0x40C00000,
	0x41B80000,           0x0,    0x40BD4D94,    0x3DAFC8B0,
	0x3E60BFE4,    0x3F000000,    0x41200000,           0x0,
	0x41900000,    0x420C0000,    0x41180000,    0x409FF132,
	0x3DAC729F,    0x3E89B457,    0x3F000000,    0x41200000,
	       0x0,    0xC1A00000,    0x41CC0000,    0x40800000,
	0x3E012CF1,    0x3E9BCB1D,    0x3F0C4CCF,    0x3F000000,
	0x41200000,           0x0,    0xC1E00000,    0xC0800000,
	0x40000000,    0x3F01BF05,    0x3F03C558,    0x3EF7DBB1,
	0x3F000000,    0x3F800000,           0x0,    0xC1E00000,
	0xC0800000,    0x40000000,    0x3F01BF05,    0x3F03C558,
	0x3EF7DBB1,    0x3F000000,    0x3F800000,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,    0x3F000000,    0x41A00000,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,    0x3F000000,
	0x41A00000,           0x1,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0
};

u32 D_80035610[90] = {
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,    0x3F000000,
	0x41200000,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	0x3F000000,    0x41200000,           0x0,    0xC0C00000,
	0x41B80000,           0x0,    0x40A2C750,    0x40C43392,
	0x40B1199C,    0x3F000000,    0x41200000,           0x0,
	0xC1900000,    0x420C0000,    0x41180000,    0x409C2EAE,
	0x3D902BC7,    0x40B12824,    0x3F000000,    0x41200000,
	       0x0,    0x41000000,    0x41CC0000,    0x40800000,
	0x3DDB927D,    0x40C1FEDC,    0x40ACEF69,    0x3F000000,
	0x41200000,           0x0,    0x41E00000,    0xC0800000,
	0x40000000,    0x3DDB927D,    0x40C1FEDC,    0x40ACEF69,
	0x3F000000,    0x3F800000,           0x0,    0x41E00000,
	0xC0800000,    0x40000000,    0x3DDB927D,    0x40C1FEDC,
	0x40ACEF69,    0x3F000000,    0x3F800000,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,    0x3F000000,    0x41A00000,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,    0x3F000000,
	0x41A00000,           0x1,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0
};

u32 D_80035778[99] = {
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,    0x3F000000,
	0x41100000,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	0x3F000000,    0x41000000,           0x0,    0x41180000,
	0xBF000000,    0x40600000,    0x3E9504E6,    0x40B2B333,
	0x40C6CBA3,    0x3F000000,    0x41000000,           0x0,
	0x41900000,    0x40F00000,    0x40600000,    0x3EE0F55E,
	0x40BE3F16,    0x40BFCC1D,    0x3F000000,    0x41000000,
	       0x0,    0xC1100000,    0x41080000,    0x40B00000,
	0x3F346DF8,    0x3E472043,    0x40C563EB,    0x3F000000,
	0x40E00000,           0x0,    0xC1E80000,    0xC0B00000,
	0x40B00000,    0x40120985,    0x3F8D9CFA,    0x3FBEB841,
	0x3F000000,    0x40E00000,           0x0,    0xC2660000,
	0xC1DC0000,    0x40B00000,    0x40120985,    0x3F8D9CFA,
	0x3FBEB841,    0x3F000000,    0x40E00000,           0x0,
	0xC19C0000,    0xC1A00000,    0x40B00000,    0x3F9CD307,
	0x3F39E0D6,    0x3F9AF8A5,    0x3F000000,    0x41700000,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,    0x3F000000,
	0x41A00000,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	0x3F000000,    0x41A00000,           0x1,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0
};

bondstruct_unk_80035904 D_80035904[11] = {
    { 0, {   0.0f,  0.0f,  0.0f }, {                 0.0f,                 0.0f,                 0.0f }, 0.5f,  9.0f },
    { 0, {   0.0f,  0.0f,  0.0f }, {                 0.0f,                 0.0f,                 0.0f }, 0.5f,  8.0f },
    { 0, { -15.5f,  0.5f, 15.0f }, {  0.9344959855079651f,  0.6256099939346313f,  0.2237969934940338f }, 0.5f,  8.0f },
    { 0, { -23.0f,  2.0f, 12.0f }, {  1.8016400337219238f,  0.9494050145149231f,  0.6307389736175537f }, 0.5f,  8.0f },
    { 0, { -18.0f, -0.5f,  4.0f }, {  0.8478249907493591f,  0.9247649908065796f, 0.07744300365447998f }, 0.5f,  7.0f },
    { 0, {  10.5f,  5.0f,  2.5f }, { 0.22940599918365479f, 0.24570399522781372f, 0.09906300157308578f }, 0.5f,  7.0f },
    { 0, {  18.0f,  5.0f,  2.5f }, { 0.03281300142407417f,    6.20933723449707f,  0.1350640058517456f }, 0.5f,  7.0f },
    { 0, {   9.5f,  3.5f, -1.5f }, {   6.273238182067871f,   6.005795001983643f, 0.08971499651670456f }, 0.5f,  7.0f },
    { 0, {   0.0f,  0.0f,  0.0f }, {                 0.0f,                 0.0f,                 0.0f }, 0.5f, 20.0f },
    { 0, {   0.0f,  0.0f,  0.0f }, {                 0.0f,                 0.0f,                 0.0f }, 0.5f, 20.0f },
    { 1, {   0.0f,  0.0f,  0.0f }, {                 0.0f,                 0.0f,                 0.0f }, 0.0f,  0.0f },
};

u32 D_80035A90[54] = {
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,    0x3F000000,
	0x41000000,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	0x3F000000,    0x41000000,           0x0,    0x3F000000,
	0xC0C00000,    0xC1000000,    0x3EE101F3,    0x3E8EC2AD,
	0x3E47DCBE,    0x3F000000,    0x41000000,           0x0,
	0xC0000000,    0xC1000000,    0xC1200000,    0x3F8D0308,
	0x3EEBE7D1,    0x3F122A80,    0x3F000000,    0x41000000,
	       0x0,    0xC0000000,    0xC1000000,    0xC1200000,
	0x3F8D0308,    0x3EEBE7D1,    0x3F122A80,    0x3F000000,
	0x41000000,           0x1,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0
};

u32 D_80035B68[54] = {
	       0x0,    0xC0000000,    0xC1000000,    0xC1200000,
	0x3F8D0308,    0x3EEBE7D1,    0x3F122A80,    0x3F000000,
	0x41000000,           0x0,    0xC0000000,    0xC1000000,
	0xC1200000,    0x3F8D0308,    0x3EEBE7D1,    0x3F122A80,
	0x3F000000,    0x41000000,           0x0,    0x3F000000,
	0xC0C00000,    0xC1000000,    0x3EE101F3,    0x3E8EC2AD,
	0x3E47DCBE,    0x3F000000,    0x41000000,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,    0x3F000000,    0x41000000,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0,           0x0,    0x3F000000,
	0x41000000,           0x1,           0x0,           0x0,
	       0x0,           0x0,           0x0,           0x0,
	       0x0,           0x0
};


//D:80035C40
u32 D_80035C40 = 0;
//D:80035C44
u32 D_80035C44 = 0;
//D:80035C48
u32 D_80035C48 = 0;
//D:80035C4C
u32 D_80035C4C = 0;
//D:80035C50
u32 D_80035C50 = 0;
//D:80035C54
u32 D_80035C54 = 0;
//D:80035C58
u32 D_80035C58 = 0;
//D:80035C5C
u32 D_80035C5C = 0;

//D:80035C60
f32 D_80035C60 = -1.0;
//D:80035C64
f32 D_80035C64 = 0.0;
//D:80035C68
f32 D_80035C68 = 1.0;
//D:80035C6C
f32 D_80035C6C = 0.0;
//D:80035C70
f32 D_80035C70 = 6.2536321;
//D:80035C74
f32 D_80035C74 = 6.2592888;
//D:80035C78
f32 D_80035C78 = 0.204238;
//D:80035C7C
f32 D_80035C7C = 0.25044999;
//D:80035C80
f32 D_80035C80 = 0.90482301;
//D:80035C84
f32 D_80035C84 = 0.28716999;
//D:80035C88
f32 D_80035C88 = 1.715736;
//D:80035C8C
f32 D_80035C8C = 0.37460899;
//D:80035C90
f32 D_80035C90 = 0.92193699;

//D:80035C94
f32 D_80035C94 = 0;


//D:80035C98
u32 D_80035C98 = 0;
//D:80035C9C
u32 D_80035C9C = 0;
//D:80035CA0
u32 D_80035CA0 = 0;
//D:80035CA4
s32 D_80035CA4 = 0xFFFFFFFF;
//D:80035CA8
u32 D_80035CA8 = 0;
//D:80035CAC
u32 D_80035CAC = 0;
//D:80035CB0
u32 D_80035CB0 = 0;
//D:80035CB4
u32 D_80035CB4 = 0;
//D:80035CB8
u32 D_80035CB8 = 0;
//D:80035CBC
u32 D_80035CBC = 0;
//D:80035CC0
u32 D_80035CC0 = 0;



//D:80035CC4
u32 D_80035CC4[] =                      { 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,           0,  0};
/* ModelRenderData D_8002CCBC = {NULL,
                                TRUE,
                                0x00000003,
                                NULL,
                                NULL,
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                {0, 0, 0, 0},
                                {0, 0, 0, 0},
                                CULLMODE_BOTH};
*/
//D:80035D00
u32 D_80035D00 = 0;
//D:80035D04
u32 D_80035D04[] = {1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//D:80035D44
u32 D_80035D44[] = {
	1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

//D:0x80035E04
struct RicochetSoundsSmall ricochet_sounds_small = {
    RICO_8_AFDM_A_SFX, RICO_8_AFDM_B_SFX, RICO_8_AFDM_C_SFX, RICO_8_AFDM_D_SFX,
    RICO_8_AFDM_A_SFX, RICO_8_AFDM_B_SFX, RICO_8_AFDM_C_SFX, RICO_8_AFDM_D_SFX,
    RICO_8_AFDM_A_SFX, RICO_8_AFDM_B_SFX, RICO_8_AFDM_C_SFX, RICO_8_AFDM_D_SFX,
    RICO_5_A_SFX,      RICO_5_B_SFX,      RICO_5_C_SFX,      RICO_5_D_SFX,
    RICO_6_HBBA_A_SFX, RICO_6_HBBA_B_SFX, RICO_6_HBBA_C_SFX, RICO_6_HBBA_D_SFX
};

//D:80035E2C
struct PunchSounds punch_sounds = {
    PUNCH1_SFX,
    PUNCH2_SFX,
    PUNCH3_SFX
};

//D:80035E34
struct BulletFleshSounds bullet_flesh_sounds = {
    HIT_BULLET_FLESH_SFX,
    HIT_BULLET_FLESH_SFX
};

struct LaserRichochetSounds laser_ricochet_sounds = {
    RICO_LASER2_SFX,
    RICO_LASER3_SFX
};

struct RicochetSoundsLarge ricochet_sounds_large = {
	RICO_12_GBU_A_SFX, RICO_12_GBU_B_SFX, RICO_12_GBU_C_SFX, RICO_12_GBU_D_SFX,
    RICO_6_TAJ_A_SFX,  RICO_6_TAJ_B_SFX,  RICO_6_TAJ_C_SFX,  RICO_6_TAJ_D_SFX,
    RICO_6_TAJ_A_SFX,  RICO_6_TAJ_B_SFX,  RICO_6_TAJ_C_SFX,  RICO_6_TAJ_D_SFX,
    RICO_6_TAJ_A_SFX,  RICO_6_TAJ_B_SFX,  RICO_6_TAJ_C_SFX,  RICO_6_TAJ_D_SFX,
    RICO_4_A_SFX,      RICO_4_B_SFX,      RICO_4_B_SFX,      RICO_4_C_SFX,
    RICO_4_A_SFX,      RICO_4_B_SFX,      RICO_4_B_SFX,      RICO_4_C_SFX,
    RICO_4_A_SFX,      RICO_4_B_SFX,      RICO_4_B_SFX,      RICO_4_C_SFX,
    RICO_5_A_SFX,      RICO_5_B_SFX,      RICO_5_C_SFX,      RICO_5_D_SFX,
    RICO_6_HBBA_A_SFX, RICO_6_HBBA_B_SFX, RICO_6_HBBA_C_SFX, RICO_6_HBBA_D_SFX
};

//D:80035E84
struct EarWhistleSounds ear_whistle_sounds = {
    RICO_EAR_WHISTLE1_SFX,
    RICO_EAR_WHISTLE2_SFX,
    RICO_EAR_WHISTLE3_SFX,
    RICO_EAR_WHISTLE4_SFX,
    RICO_EAR_WHISTLE5_SFX
};

//D:80035E90
u16 D_80035E90[] = { RICO_LASER2_SFX, RICO_LASER3_SFX };
//D:80035E94
u16 D_80035E94[] = { KNIFE_THROW1_SFX, KNIFE_THROW2_SFX, KNIFE_THROW3_SFX, 0 };
//D:80035E9C
struct unkown_gun_struct D_80035E9C = { 0, 0 };
//D:80035EA0
//u32 D_80035EA0 = 0;
//D:80035EA4
u32 D_80035EA4 = 0;
//D:80035EA8
u32 D_80035EA8 = 0;
//D:80035EAC
u32 D_80035EAC = 0;
//D:80035EB0
u32 D_80035EB0[] = {0, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//D:80035EEC
u32 dword_D_80035EEC = 0;

//D:80035EF0
#define AMMO_RELATED_MAX 30
AmmoStats ammo_related[AMMO_RELATED_MAX]  = {
    {    0x0,       0x0,       0x0,       0x0,       0x0, },
    {  0x320,     0x200,     0xC84,       0x0,       0x0, },
    {   0xC8,       0x0,       0x0,       0x0,       0x0, },
    {  0x190,     0x200,     0xC90,    0xC000,       0x0, },
    {   0x64,     0x200,     0xC9C,       0x0,       0x0, },
    {    0xC,     0x200,     0xCD8,       0x0,       0x0, },
    {    0x3,     0x200,     0xCC0,    0xC000,       0x0, },
    {    0xA,     0x200,     0xCFC,    0x3F80,       0x0, },
    {    0xA,     0x200,     0xD14,    0x3F80,       0x0, },
    {    0xA,     0x200,     0xD08,    0x3F80,       0x0, },
    {    0xA,     0x200,     0xCA8,       0x0,       0x0, },
    {    0xC,     0x200,     0xCB4,       0x0,       0x0, },
    {   0xC8,     0x200,     0xCE4,       0x0,       0x0, },
    {   0x64,     0x200,     0xCF0,       0x0,       0x0, },
    {   0x32,       0x0,       0x0,       0x0,       0x0, },
    {    0xA,       0x0,       0x0,       0x0,       0x0, },
    {    0x2,       0x0,       0x0,       0x0,       0x0, },
    {    0x8,       0x0,       0x0,       0x0,       0x0, },
    {    0x6,       0x0,       0x0,       0x0,       0x0, },
    {    0xA,       0x0,       0x0,       0x0,       0x0, },
    {    0xA,       0x0,       0x0,       0x0,       0x0, },
    {    0xA,       0x0,       0x0,       0x0,       0x0, },
    {    0x1,       0x0,       0x0,       0x0,       0x0, },
    {    0xA,       0x0,       0x0,       0x0,       0x0, },
    {  0x3E8,       0x0,       0x0,       0x0,       0x0, },
    {    0xA,       0x0,       0x0,       0x0,       0x0, },
    {    0xA,       0x0,       0x0,       0x0,       0x0, },
    {    0xA,       0x0,       0x0,       0x0,       0x0, },
    {   0x32,     0x200,     0xD20,    0xBF80,       0x0, },
    {    0x1,       0x0,       0x0,       0x0,       0x0, },
};

//was previously attached to ammo_related[] (array at D:80035EF0)
//D:80036058
u16 D_80036058[] = { 0, 0, 0, 0, };


//i may belong to objecthandler.c
//D:80036060
u32 D_80036060 = 0;




// forward declarations

void bullet_path_from_screen_center(coord3d* arg0, coord3d* arg1, enum GUNHAND arg2);
void sub_GAME_7F05EC1C(struct WeaponObjectRecord *arg0, struct coord3d *arg1, Mtxf *arg2, struct coord3d *arg3, s32 *arg4);

// end forward declarations




void set_cartridges_eject(u32 uParm1) {
    cartridges_eject = uParm1;
}

u32 get_cartridges_eject(void) {
    return cartridges_eject;
}

void nullsub_73(void) {
    return;
}


void sub_GAME_7F05C540(coord3d* pos)
{
    bondstruct_unk_80035904* temp_v0;

    temp_v0 = &D_80035904[D_80034CA0];
    temp_v0->unk04.x += pos->x;
    temp_v0->unk04.y += pos->y;
    temp_v0->unk04.z += pos->z;
}


void sub_GAME_7F05C594(Mtxf* mtxf)
{
    Mtxf sp18;
    matrix_4x4_set_rotation_around_xyz(&D_80035904[D_80034CA0].unk10, &sp18);
    matrix_4x4_multiply_in_place(mtxf, &sp18);
    matrix_4x4_get_rotation_around_xyz(&sp18, &D_80035904[D_80034CA0].unk10);
}


void sub_GAME_7F05C614(void)
{
    if (!cartridges_eject) { return; }

    g_CurrentPlayer->hands[0].field_92C = 1;
    matrix_4x4_set_rotation_around_xyz(&D_80035904[D_80034CA0].unk10, (Mtxf* ) &g_CurrentPlayer->hands[0].field_8EC);
    matrix_4x4_set_position(&D_80035904[D_80034CA0].unk04, (Mtxf* ) &g_CurrentPlayer->hands[0].field_8EC);
    cartridges_eject = 0;
}


void sub_GAME_7F05C6B8(void)
{
    D_80034CA0++;
    if (D_80035904[D_80034CA0].unk00 & 1)
    {
        D_80034CA0 = 0;
    }
}



#ifdef NONMATCHING
void sub_GAME_7F05C6FC(void) {

}
#else

void _asmpp_func1(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func2(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif





WeaponStats *get_ptr_item_statistics(ITEM_IDS item) {
    if (gitem_structs[item].has_no_model == 0) { /* weapon has model, return stats struct */
        return gitem_structs[item].item_weapon_stats;
    }
    return &default_weaponstats; /* no model, return defaults */
}




void copy_item_in_hand(coord3d *pos)
{
    ITEM_IDS item;
    WeaponStats *stats;

    item = getCurrentPlayerWeaponId(0);
    stats = get_ptr_item_statistics(item);

    pos->x = stats->PosX;
    pos->y = stats->PosY;
    pos->z = stats->PosZ;
}


void copy_item_in_hand_to_main_list(coord3d *pos) {

    WeaponStats *stats;
    ITEM_IDS item;

    item = getCurrentPlayerWeaponId(0);
    stats = get_ptr_item_statistics(item);

    stats->PosX = pos->x;
    stats->PosY = pos->y;
    stats->PosZ = pos->z;
}


void bgunCalculateBlend(enum GUNHAND handnum)
{
    s32 sp60[2];
    s32 sp58[2];
    f32 mult = get_ptr_item_statistics(getCurrentPlayerWeaponId(handnum))->Sway;

    sp60[handnum] = (g_CurrentPlayer->hands[handnum].curblendpos + 2) % 4;
    sp58[handnum] = (g_CurrentPlayer->hands[handnum].curblendpos + 1) % 4;
    g_CurrentPlayer->hands[handnum].curblendpos = sp58[handnum];

    g_CurrentPlayer->hands[handnum].blendlook[sp60[handnum]].x = (RANDOMFRAC() - 0.5f) * 0.08f * mult;
    g_CurrentPlayer->hands[handnum].blendlook[sp60[handnum]].y = (RANDOMFRAC() - 0.5f) * 0.1f * mult;
    g_CurrentPlayer->hands[handnum].blendlook[sp60[handnum]].z = -1;

    g_CurrentPlayer->hands[handnum].blendup[sp60[handnum]].x = (RANDOMFRAC() - 0.5f) * 0.1f * mult;
    g_CurrentPlayer->hands[handnum].blendup[sp60[handnum]].y = 1;
    g_CurrentPlayer->hands[handnum].blendup[sp60[handnum]].z = (RANDOMFRAC() - 0.5f) * 0.1f * mult;

    g_CurrentPlayer->hands[handnum].blendpos[sp60[handnum]].x = (RANDOMFRAC() * 0.75f) + 1.5f;
    g_CurrentPlayer->hands[handnum].blendpos[sp60[handnum]].y = (2 + RANDOMFRAC()) * g_CurrentPlayer->hands[handnum].blendscale1;
    g_CurrentPlayer->hands[handnum].blendpos[sp60[handnum]].z = (RANDOMFRAC() - 0.5f) * 2.5f;

    if (g_CurrentPlayer->hands[handnum].sideflag < 0)
    {
        g_CurrentPlayer->hands[handnum].blendpos[sp60[handnum]].x *= -1;

        if (g_CurrentPlayer->hands[handnum].sideflag == -2)
        {
            g_CurrentPlayer->hands[handnum].sideflag = 1;
        }
        else
        {
            g_CurrentPlayer->hands[handnum].sideflag = -2;
        }
    }
    else
    {
        if (g_CurrentPlayer->hands[handnum].sideflag == 2)
        {
            g_CurrentPlayer->hands[handnum].sideflag = -1;
        }
        else
        {
            g_CurrentPlayer->hands[handnum].sideflag = 2;
        }
    }

    g_CurrentPlayer->hands[handnum].blendscale1 = -g_CurrentPlayer->hands[handnum].blendscale1;
}


s32 Gun_hand_without_item(enum GUNHAND arg0)
{
    return g_CurrentPlayer->hand_invisible[arg0] > 0
        || (g_CurrentPlayer->hand_item[arg0] == 0 && g_CurrentPlayer->field_2A44[arg0] < 0);
}

s32 get_itemtype_in_hand(GUNHAND hand)
{
    return g_CurrentPlayer->hand_item[hand];
}


ModelFileHeader *get_ptr_itemheader_in_hand(GUNHAND hand)
{
    return &g_CurrentPlayer->copy_of_body_obj_header[hand];
}




u8 * getPlayerWeaponBufferForHand(GUNHAND hand)
{
    return g_CurrentPlayer->ptr_hand_weapon_buffer[hand];
}


u32 getSizeBufferWeaponInHand(int hand)
{
    return size_item_buffer[hand];
}


void remove_item_in_hand(GUNHAND hand)
{
  g_CurrentPlayer->hand_invisible[hand] = 0;
  g_CurrentPlayer->hand_item[hand] = ITEM_UNARMED;
  g_CurrentPlayer->field_2A44[hand] = -1;
  g_CurrentPlayer->lock_hand_model[hand] = 1;
  return;
}


void place_item_in_hand_swap_and_make_visible(GUNHAND hand, ITEM_IDS item)
{
    if (g_CurrentPlayer->lock_hand_model[hand]) { return; }

    if (g_CurrentPlayer->hand_invisible[hand] >= 0)
    {
        if (item != g_CurrentPlayer->hand_item[hand])
        {
            g_CurrentPlayer->hand_invisible[hand] = -1;
            g_CurrentPlayer->field_2A44[hand] = item;
        }
        return;
    }

    if (item != g_CurrentPlayer->hand_item[hand])
    {
        g_CurrentPlayer->field_2A44[hand] = item;
        return;
    }

    g_CurrentPlayer->hand_invisible[hand] = 1;
}


char * get_ptr_item_text_call_line(ITEM_IDS item)
{
  if (item == ITEM_FIST) {
    item = g_CurrentPlayer->cur_item_weapon_getname;
  }
  return gitem_structs[item].item_file_name;
}


struct ModelFileHeader * get_ptr_weapon_model_header_line(ITEM_IDS weapon)
{
    if (weapon == ITEM_FIST) {
        weapon = g_CurrentPlayer->cur_item_weapon_getname;
    }
    return gitem_structs[weapon].item_header;
}


int getCurrentWeaponOrItem(void)
{
    return g_CurrentPlayer->cur_item_weapon_getname;
}


void used_to_load_1st_person_model_on_demand(enum GUNHAND arg0)
{
    u32 size_buffer_weapon;
    s8* ptr_item_text;
    ModelFileHeader* ptr_weapon_model;
    u8* buffer_weapon;
    enum ITEM_IDS field_2a44;

    if ((g_CurrentPlayer->hand_invisible[arg0] < 0) && (g_CurrentPlayer->lock_hand_model[arg0] == 0))
    {
        if ((g_CurrentPlayer->hand_invisible[arg0] < -2) || (g_CurrentPlayer->hand_item[arg0] == ITEM_UNARMED))
        {
            field_2a44 = g_CurrentPlayer->field_2A44[arg0];
            ptr_item_text = (s8*)get_ptr_item_text_call_line(field_2a44);
            ptr_weapon_model = get_ptr_weapon_model_header_line(field_2a44);

            if ((ptr_item_text != NULL) && (ptr_weapon_model != NULL))
            {
                buffer_weapon = getPlayerWeaponBufferForHand(arg0);
                size_buffer_weapon = getSizeBufferWeaponInHand(arg0);

                g_CurrentPlayer->copy_of_body_obj_header[arg0] = *ptr_weapon_model;

                if (field_2a44 == ITEM_SUIT_LF_HAND)
                {
                    texInitPool(&g_CurrentPlayer->item_related[arg0], buffer_weapon + 0xBD70, size_buffer_weapon + 0xFFFF4290);
                    load_object_fill_header(&g_CurrentPlayer->copy_of_body_obj_header[arg0], (u8* ) ptr_item_text, buffer_weapon, 0xBD70, &g_CurrentPlayer->item_related[arg0]);
                }
                else if ((field_2a44 == ITEM_TRIGGER) || (field_2a44 == ITEM_WATCHLASER))
                {
                    texInitPool(&g_CurrentPlayer->item_related[arg0], buffer_weapon + 0xAFD0, size_buffer_weapon + 0xFFFF5030);
                    load_object_fill_header(&g_CurrentPlayer->copy_of_body_obj_header[arg0], (u8* ) ptr_item_text, buffer_weapon, 0xAFD0, &g_CurrentPlayer->item_related[arg0]);
                }
                else
                {
                    texInitPool(&g_CurrentPlayer->item_related[arg0], &buffer_weapon[D_80032464[arg0]], size_buffer_weapon - D_80032464[arg0]);
                    load_object_fill_header(&g_CurrentPlayer->copy_of_body_obj_header[arg0], (u8* ) ptr_item_text, buffer_weapon, D_80032464[arg0], &g_CurrentPlayer->item_related[arg0]);
                }
            }

            g_CurrentPlayer->hand_invisible[arg0] = 1;
            g_CurrentPlayer->hand_item[arg0] = field_2a44;
            g_CurrentPlayer->field_2A44[arg0] = -1;

        }
        else
        {
            g_CurrentPlayer->hand_invisible[arg0]--;
        }
    }
}


ITEM_IDS sub_GAME_7F05D334(ITEM_IDS item, s32 arg1)
{
    while (arg1 > 0)
    {
        do
        {
            item = (item + 1) % ITEM_BOMBCASE;
        } while (bondinvItemAvailable(item) == 0);
        arg1--;
    }

    while (arg1 < 0)
    {
        do
        {
            item--;
            if (item < 0)
            {
                item = 0x20 - (-(item + 1) % ITEM_BOMBCASE);
            }
        } while (bondinvItemAvailable(item) == 0);
        arg1++;
    }

    return item;
}


ITEM_IDS get_next_weapon_in_cycle_for_hand(GUNHAND hand, s32 direction)
{
	if (g_CurrentPlayer->hands[hand].when_detonating_mines_is_0 == 5) {
		if (
			(direction < 0 && (g_CurrentPlayer->hands[hand].field_8B8 > 0)) ||
			(direction > 0 && (g_CurrentPlayer->hands[hand].field_8B8 < 0)) ) {
			return getCurrentPlayerWeaponId(hand);
		}
		else {
			return g_CurrentPlayer->hands[hand].weapon_next_weapon;
		}

    }
    if (g_CurrentPlayer->hands[hand].when_detonating_mines_is_0 == 6) {
        return g_CurrentPlayer->hands[hand].weapon_next_weapon;
    }
    return getCurrentPlayerWeaponId(hand);
}


void likely_change_weapon_in_hand(enum GUNHAND hand, s32 arg1, s32 arg2)
{
    if ((g_CurrentPlayer->hands[hand].when_detonating_mines_is_0 == 5) || (g_CurrentPlayer->hands[hand].when_detonating_mines_is_0 == 6))
    {
        g_CurrentPlayer->hands[hand].field_8B0 = g_CurrentPlayer->hands[hand].field_890;

#ifdef VERSION_EU
        if (getPlayerCount() == 1) {
            g_CurrentPlayer->hands[hand].field_8B0 += 0xE;
        } else {
            g_CurrentPlayer->hands[hand].field_8B0 += 0xA;
        }
#else
        if (getPlayerCount() == 1) {
            g_CurrentPlayer->hands[hand].field_8B0 += 0x11;
        } else {
            g_CurrentPlayer->hands[hand].field_8B0 += 0xD;
        }
#endif

    }

    if (get_next_weapon_in_cycle_for_hand(hand, 0) != arg1)
    {
        if ((g_CurrentPlayer->hands[hand].when_detonating_mines_is_0 != 5) && (g_CurrentPlayer->hands[hand].when_detonating_mines_is_0 != 6))
        {
            g_CurrentPlayer->hands[hand].weapon_current_animation = 5;
        }
        g_CurrentPlayer->hands[hand].weapon_next_weapon = arg1;
        g_CurrentPlayer->hands[hand].weapon_animation_trigger = 1;
        g_CurrentPlayer->hands[hand].field_8B8 = arg2;
    }
}


void sub_GAME_7F05D610(GUNHAND hand)
{
    likely_change_weapon_in_hand(hand, sub_GAME_7F05D334(get_next_weapon_in_cycle_for_hand(hand, 0), 1), 0);
}


void sub_GAME_7F05D650(GUNHAND hand)
{
    likely_change_weapon_in_hand(hand, sub_GAME_7F05D334(get_next_weapon_in_cycle_for_hand(hand, 0), -1), 0);
}


void sub_GAME_7F05D690(void)
{
    currentPlayerEquipWeaponWrapper(GUNRIGHT, g_CurrentPlayer->hands[GUNRIGHT].previous_weapon);
    currentPlayerEquipWeaponWrapper(GUNLEFT, g_CurrentPlayer->hands[GUNLEFT].previous_weapon);
}


void advance_through_inventory(void)
{
    ITEM_IDS nextright;
    ITEM_IDS nextleft;

    nextright = get_next_weapon_in_cycle_for_hand(GUNRIGHT, 1);
    nextleft = get_next_weapon_in_cycle_for_hand(GUNLEFT, 1);

    if ((nextright >= ITEM_BOMBCASE) || (nextleft >= ITEM_BOMBCASE))
    {
        nextright = g_CurrentPlayer->hands[GUNRIGHT].previous_weapon;
        nextleft = g_CurrentPlayer->hands[GUNLEFT].previous_weapon;
    }
    else
    {
        bondinvCycleForward(&nextright, &nextleft, FALSE);
    }

    likely_change_weapon_in_hand(GUNRIGHT, nextright, 1);
    likely_change_weapon_in_hand(GUNLEFT, nextleft, 1);
}


void backstep_through_inventory(void)
{
    ITEM_IDS nextright;
    ITEM_IDS nextleft;

    nextright = get_next_weapon_in_cycle_for_hand(GUNRIGHT, -1);
    nextleft = get_next_weapon_in_cycle_for_hand(GUNLEFT, -1);

    if ((nextright >= ITEM_BOMBCASE) || (nextleft >= ITEM_BOMBCASE))
    {
        nextright = g_CurrentPlayer->hands[GUNRIGHT].previous_weapon;
        nextleft = g_CurrentPlayer->hands[GUNLEFT].previous_weapon;
    }
    else
    {
        bondinvCycleBackward(&nextright, &nextleft, FALSE);
    }

    likely_change_weapon_in_hand(GUNRIGHT, nextright, -1);
    likely_change_weapon_in_hand(GUNLEFT, nextleft, -1);
}

void autoadvance_on_deplete_all_ammo(void)
{
	ITEM_IDS nextright;
	ITEM_IDS nextleft;
	ITEM_IDS duperight;
	ITEM_IDS dupeleft;

    nextright = get_next_weapon_in_cycle_for_hand(GUNRIGHT, 1);
    duperight = nextright;

    nextleft = get_next_weapon_in_cycle_for_hand(GUNLEFT, 1);
    dupeleft = nextleft;

    if ((duperight >= ITEM_BOMBCASE) || (dupeleft >= ITEM_BOMBCASE))
    {
        duperight = g_CurrentPlayer->hands[GUNRIGHT].previous_weapon;
        dupeleft = g_CurrentPlayer->hands[GUNLEFT].previous_weapon;
    }
    else if ((duperight == ITEM_REMOTEMINE) && ((bondinvItemAvailable(ITEM_TRIGGER))))
    {
        duperight = ITEM_TRIGGER;
        dupeleft = ITEM_UNARMED;
    }
    else
    {
        bondinvCycleForward(&duperight, &dupeleft, TRUE);

        if ((duperight < nextright) || ((duperight == nextright) && (nextleft >= dupeleft)))
        {
			duperight = nextright;
			dupeleft = nextleft;
			bondinvCycleBackward(&duperight, &dupeleft, TRUE);
        }
    }

    likely_change_weapon_in_hand(GUNRIGHT, duperight, 1);
    likely_change_weapon_in_hand(GUNLEFT, dupeleft, 1);
}

s32 currentPlayerEquipWeaponWrapper(GUNHAND hand, s32 next_weapon) {
    g_CurrentPlayer->hands[hand].weapon_current_animation = 5;
    g_CurrentPlayer->hands[hand].weapon_next_weapon = next_weapon;
    g_CurrentPlayer->hands[hand].weapon_animation_trigger = 0;
}

void attempt_reload_item_in_hand(GUNHAND hand) {
    s32 ammo_type = get_ammo_type_for_weapon(getCurrentPlayerWeaponId(hand));
    if (ammo_type != 0) {
        if (g_CurrentPlayer->hands[hand].weapon_current_animation == 0) {
            g_CurrentPlayer->hands[hand].weapon_current_animation = 9;
        }
    }
}

ITEM_IDS getCurrentPlayerWeaponId(GUNHAND hand) {
    return g_CurrentPlayer->hands[hand].weaponnum;
}

void draw_item_in_hand(GUNHAND hand, s32 next_weapon) {
	g_CurrentPlayer->hands[hand].weapon_current_animation = 0xE;
	g_CurrentPlayer->hands[hand].weapon_next_weapon = next_weapon;
}

ITEM_IDS get_item_in_hand_or_watch_menu(GUNHAND hand) {
	if (g_CurrentPlayer->hands[hand].weaponnum_watchmenu >= 0) {
		return g_CurrentPlayer->hands[hand].weaponnum_watchmenu;
	} else {
		return g_CurrentPlayer->hands[hand].weaponnum;
	}
}

void sub_GAME_7F05DA8C(GUNHAND hand, ITEM_IDS weaponnum_watchmenu) {
    place_item_in_hand_swap_and_make_visible(hand, weaponnum_watchmenu);
	g_CurrentPlayer->hands[hand].weaponnum_watchmenu = weaponnum_watchmenu;
}

void sub_GAME_7F05DAE4(GUNHAND hand) {
    if (g_CurrentPlayer->hands[hand].weaponnum_watchmenu >= 0) {
        place_item_in_hand_swap_and_make_visible(hand, g_CurrentPlayer->hands[hand].weaponnum);
		g_CurrentPlayer->hands[hand].weaponnum_watchmenu = -1;
    }
}


void currentPlayerUnEquipWeaponWrapper(enum GUNHAND hand, enum ITEM_IDS weapid)
{
    enum ITEM_IDS weapon_num;
    s32 ammo_type;

    weapon_num = g_CurrentPlayer->hands[hand].weaponnum;
    ammo_type = get_ammo_type_for_weapon(weapon_num);

    if (g_CurrentPlayer->hands[hand].weaponnum_watchmenu < 0)
    {
        place_item_in_hand_swap_and_make_visible(hand, weapid);
    }

    if (g_CurrentPlayer->hands[hand].weapon_ammo_in_magazine > 0)
    {
        g_CurrentPlayer->ammoheldarr[ammo_type] += g_CurrentPlayer->hands[hand].weapon_ammo_in_magazine;
    }

    if (weapon_num < ITEM_BOMBCASE)
    {
        g_CurrentPlayer->hands[hand].previous_weapon = weapon_num;
    }

    if (getPlayerCount() >= 2)
    {
        sub_GAME_7F09B368(hand);
    }

    sub_GAME_7F05FB00(hand);
    g_CurrentPlayer->hands[hand].weaponnum = weapid;
    g_CurrentPlayer->hands[hand].weapon_ammo_in_magazine = 0;
    g_CurrentPlayer->hands[hand].field_A4C = 0;
    g_CurrentPlayer->hands[hand].field_A50 = 0;
    bondinvDetermineEquippedItem();
}


s8 get_hands_firing_status(GUNHAND hand) {
    return g_CurrentPlayer->hands[hand].weapon_firing_status;
}

f32 sub_GAME_7F05DCB8(GUNHAND hand) {
	return g_CurrentPlayer->hands[hand].field_A34;
}

f32 sub_GAME_7F05DCE8(GUNHAND hand)
{
	f32 ret;

	if (hand == GUNRIGHT)
	{
		ret = get_ptr_item_statistics(get_item_in_hand_or_watch_menu(GUNRIGHT))->PosX;
	}
	else
	{
		ret = -get_ptr_item_statistics(get_item_in_hand_or_watch_menu(GUNLEFT))->PosX;
	}

	return ret;
}

f32 get_item_in_hand_zoom(void) {
    if (get_item_in_hand_or_watch_menu(GUNRIGHT) == ITEM_SNIPERRIFLE) {
        return g_CurrentPlayer->sniper_zoom;
    }
    if (get_item_in_hand_or_watch_menu(GUNRIGHT) == ITEM_CAMERA) {
        return g_CurrentPlayer->camera_zoom;
    }
    return get_ptr_item_statistics(get_item_in_hand_or_watch_menu(GUNRIGHT))->Zoom;
}

void camera_sniper_zoom_out(f32 zoom)
{
	if (get_item_in_hand_or_watch_menu(GUNRIGHT) == ITEM_SNIPERRIFLE) {
		g_CurrentPlayer->sniper_zoom *= (1.0f + (zoom * 0.1f));
		if (g_CurrentPlayer->sniper_zoom > 60.0f) {
			g_CurrentPlayer->sniper_zoom = 60.0f;
		}
	}
	else
	{
		if (get_item_in_hand_or_watch_menu(GUNRIGHT) == ITEM_CAMERA) {
			g_CurrentPlayer->camera_zoom *= (1.0f + (zoom * 0.1f));
			if (g_CurrentPlayer->camera_zoom > 60.0f) {
				g_CurrentPlayer->camera_zoom = 60.0f;
			}
		}
	}
}

void camera_sniper_zoom_in(f32 zoom)
{
	if (get_item_in_hand_or_watch_menu(GUNRIGHT) == ITEM_SNIPERRIFLE) {
		g_CurrentPlayer->sniper_zoom /= (1.0f + (zoom * 0.1f));
		if (g_CurrentPlayer->sniper_zoom < 7.0f) {
			g_CurrentPlayer->sniper_zoom = 7.0f;
		}
	}
	else
	{
		if (get_item_in_hand_or_watch_menu(GUNRIGHT) == ITEM_CAMERA) {
			g_CurrentPlayer->camera_zoom /= (1.0f + (zoom * 0.1f));
			if (g_CurrentPlayer->camera_zoom < 7.0f) {
				g_CurrentPlayer->camera_zoom = 7.0f;
			}
		}
	}
}

f32 bondwalkItemGetDestructionAmount(ITEM_IDS item)
{
  return get_ptr_item_statistics(item)->DestructionAmount;
}


f32 bondwalkItemGetForceOfImpact(ITEM_IDS item)
{
	return get_ptr_item_statistics(item)->ForceOfImpact;
}

/**
 * Address 0x7F05DFCC
 */
s8 bondwalkItemGetAutomaticFiringRate(ITEM_IDS item) {
    return get_ptr_item_statistics(item)->AutomaticFiringRate;
}


u8 bondwalkItemGetSoundTriggerRate(ITEM_IDS item) {
    return get_ptr_item_statistics(item)->SoundTriggerRate;
}


u16 bondwalkItemGetSound(ITEM_IDS item)
{
  return get_ptr_item_statistics(item)->Sound;
}


u8 bondwalkItemGetObjectsShootThrough(ITEM_IDS item)
{
  return get_ptr_item_statistics(item)->ObjectsShootThrough;
}


s32 bondwalkItemHasAmmo(ITEM_IDS item)
{
    if (bondwalkItemCheckBitflags(item, WEAPONSTATBITFLAG_HAS_AMMO) != 0)
    {
        if ((get_ammo_type_for_weapon(item) == 0) || (get_ammo_count_for_weapon(item) > 0))
        {
            return 1;
        }
    }
    return 0;
}


u32 bondwalkItemCheckBitflags(ITEM_IDS item, u32 mask)
{
  return ((get_ptr_item_statistics(item)->BitFlags & mask) != 0);
}


void gunSetBondWeaponSway(f32 breathing, f32 arg1, f32 arg2, f32 arg3)
{
    f32 dampt[2];
    s32 i;
    u32 unused[2];
    f32 sp50 = arg2;
    f32 sp4c;
    u32 stack2;
    f32 minbreathing;

    if (sp50 < 0.0f) { sp50 = -sp50; }

    if (arg1 > 0.8f)
    {
        g_CurrentPlayer->gunposamplitude = 1.0f;
    }
    else
    {
        if (arg1 > 0.1f)
        {
            f32 tmp = (1.0f - cosf((arg1 - 0.1f) * M_TAU_F / 2.8f));
            g_CurrentPlayer->gunposamplitude = 0.8f * tmp + 0.2f;
        }
        else
        {
            g_CurrentPlayer->gunposamplitude = 0.1f;
        }
    }

    if (g_CurrentPlayer->gunposamplitude < (bondviewGetBondBreathing() * 0.3f))
    {
        g_CurrentPlayer->gunposamplitude = bondviewGetBondBreathing() * 0.3f;
    }

    if (g_CurrentPlayer->gunposamplitude < 0.5f * sp50)
    {
        g_CurrentPlayer->gunposamplitude = 0.5f * sp50;
    }

    for (i = 0; i < g_ClockTimer; i++)
    {
        g_CurrentPlayer->field_1080 = (g_CurrentPlayer->field_1080 * (PAL ? 0.9403f : 0.95f)) + g_CurrentPlayer->gunposamplitude;
    }

    g_CurrentPlayer->gunposamplitude = g_CurrentPlayer->field_1080 * (PAL ? 0.059700012f : 0.050000012f);

    minbreathing = 0.016666668f * sp50;
    if (breathing < minbreathing)
    {
        breathing = minbreathing;
    }

    for (i = 0; i < g_ClockTimer; i++)
    {
        g_CurrentPlayer->field_107C = (g_CurrentPlayer->field_107C * (PAL ? 0.9403f : 0.95f)) + breathing;
    }

    breathing = g_CurrentPlayer->field_107C * (PAL ? 0.059700012f : 0.050000012f);

    sp4c = breathing * g_GlobalTimerDelta;
    dampt[0] = g_CurrentPlayer->hands[0].dampt + sp4c;

    while (dampt[0] >= 1.0f)
    {
        bgunCalculateBlend(GUNRIGHT);
        dampt[0] -= 1.0f;
        g_CurrentPlayer->syncoffset++;
    }

    g_CurrentPlayer->synccount += g_GlobalTimerDelta;

    if (g_CurrentPlayer->synccount > 60.0f)
    {
        g_CurrentPlayer->synccount = 0.0f;
        g_CurrentPlayer->syncchange = (RANDOMFRAC() - 0.5f) * 0.2f / 60.0f;
    }

    if (g_CurrentPlayer->syncchange + sp4c > 0.0f)
    {
        g_CurrentPlayer->gunsync += g_CurrentPlayer->syncchange;
    }

    if (g_CurrentPlayer->gunsync > 0.5f)
    {
        g_CurrentPlayer->gunsync = 0.5f;
    }
    else if (g_CurrentPlayer->gunsync < -0.5f)
    {
        g_CurrentPlayer->gunsync = -0.5f;
    }
    else if (g_CurrentPlayer->gunsync < 0.1f && g_CurrentPlayer->gunsync > -0.1f)
    {
        if (g_CurrentPlayer->gunsync > 0.0f)
        {
            g_CurrentPlayer->gunsync = -0.1f;
        }
        else
        {
            g_CurrentPlayer->gunsync = 0.1f;
        }
    }

    dampt[1] = dampt[0] + g_CurrentPlayer->syncoffset + g_CurrentPlayer->gunsync;

    while (dampt[1] >= 1.0f)
    {
        bgunCalculateBlend(GUNLEFT);
        dampt[1] -= 1.0f;
        g_CurrentPlayer->syncoffset--;
    }

    for (i = 0; i < 2; i++)
    {
        g_CurrentPlayer->hands[i].dampt = dampt[i];
        g_CurrentPlayer->hands[i].weapon_theta_displacement = -1.75f * arg3;
        g_CurrentPlayer->hands[i].weapon_verta_displacement = -2.0f * arg2;
    }
}


void gunSetOffsetRelated(f32 param_1)
{
    g_CurrentPlayer->hands[GUNRIGHT].field_A30 = (1.0f - cosf(param_1)) * 5.0f;
    g_CurrentPlayer->hands[GUNLEFT].field_A30 = (1.0f - cosf(param_1)) * 5.0f;
}


f32 get_value_if_watch_is_on_hand_or_not(GUNHAND hand)
{
  if ((getCurrentPlayerWeaponId(hand) == ITEM_TRIGGER) || (getCurrentPlayerWeaponId(hand) == ITEM_WATCHLASER))
  {
    return 0.08726647f;
  }
  else
  {
    return 0.17453294f;
  }
}


void sub_GAME_7F05E6B4(enum GUNHAND hand, s32 arg1)
{
    if (arg1 != 0)
    {
        if (g_CurrentPlayer->hands[hand].field_A84 < get_value_if_watch_is_on_hand_or_not(hand))
        {
            g_CurrentPlayer->hands[hand].field_A84 += (0.029088823f * g_GlobalTimerDelta);
        }
        if (g_CurrentPlayer->hands[hand].field_A84 > get_value_if_watch_is_on_hand_or_not(hand)) {
            g_CurrentPlayer->hands[hand].field_A84 = get_value_if_watch_is_on_hand_or_not(hand);
        }
    }
    else
    {
        if (g_CurrentPlayer->hands[hand].field_A84 > 0.0f)
        {
            g_CurrentPlayer->hands[hand].field_A84 -= (0.017453294f * g_GlobalTimerDelta);
        }
        if (g_CurrentPlayer->hands[hand].field_A84 < 0.0f)
        {
            g_CurrentPlayer->hands[hand].field_A84 = 0.0f;
        }
    }
}


void sub_GAME_7F05E808(GUNHAND hand) {
	g_CurrentPlayer->hands[hand].field_A8C = 1;
}


void sub_GAME_7F05E83C(GUNHAND hand)
{
    f32 recoil_back;

    recoil_back = get_ptr_item_statistics(get_item_in_hand_or_watch_menu(hand))->BoltRecoilBack;

    if (g_CurrentPlayer->hands[hand].field_A8C != 0)
    {
        if (g_CurrentPlayer->hands[hand].field_A88 < recoil_back)
        {
            g_CurrentPlayer->hands[hand].field_A88 = (g_CurrentPlayer->hands[hand].field_A88 + (recoil_back * 0.25f * g_GlobalTimerDelta));

        }
        if (recoil_back <= g_CurrentPlayer->hands[hand].field_A88) {
            g_CurrentPlayer->hands[hand].field_A88 = recoil_back;
            g_CurrentPlayer->hands[hand].field_A8C = 0;
        }
    }
    else if (g_CurrentPlayer->hands[hand].weapon_ammo_in_magazine > 0)
    {
        if (g_CurrentPlayer->hands[hand].field_A88 > 0.0f)
        {
            g_CurrentPlayer->hands[hand].field_A88 = (g_CurrentPlayer->hands[hand].field_A88 - (recoil_back * 0.16666667f * g_GlobalTimerDelta));

        }
        if (g_CurrentPlayer->hands[hand].field_A88 < 0.0f)
        {
            g_CurrentPlayer->hands[hand].field_A88 = 0.0f;
        }
    }
}


void sub_GAME_7F05E978(Model* model, s32 val)
{
    if (model->obj->Switches[8] != NULL)
    {
        modelGetNodeRwData(model, model->obj->Switches[8])->DisplayList.unk00 = val;
    }

    if (model->obj->Switches[9] != NULL)
    {
        modelGetNodeRwData(model, model->obj->Switches[9])->DisplayList.unk00 = val;
    }

    if (model->obj->Switches[10] != NULL)
    {
        modelGetNodeRwData(model, model->obj->Switches[10])->DisplayList.unk00 = val;
    }

    if (model->obj->Switches[11] != NULL)
    {
        modelGetNodeRwData(model, model->obj->Switches[11])->DisplayList.unk00 = val;
    }

    if (model->obj->Switches[12] != NULL)
    {
        modelGetNodeRwData(model, model->obj->Switches[12])->DisplayList.unk00 = val;
    }

    if (model->obj->Switches[13] != NULL)
    {
        modelGetNodeRwData(model, model->obj->Switches[13])->DisplayList.unk00 = val;
    }

    if (model->obj->numSwitches >= 0x24)
    {
        if (model->obj->Switches[35] != NULL)
        {
            modelGetNodeRwData(model, model->obj->Switches[35])->DisplayList.unk00 = val;
        }
    }
}


void sub_GAME_7F05EA94(Model* model, s32 val)
{
    ModelNode* switch_14;
    ModelNode* switch_15;

    if (model->obj->numSwitches >= 0x10)
    {
        switch_14 = model->obj->Switches[14];
        if (switch_14 != NULL)
        {
            // Guessing DisplayList here
            modelGetNodeRwData(model, switch_14)->DisplayList.unk00 = val;
        }

        switch_15 = model->obj->Switches[15];
        if (switch_15 != NULL)
        {
            // Guessing DisplayList here
            modelGetNodeRwData(model, switch_15)->DisplayList.unk00 = val;
        }
    }
}


/**
 * Address 0x7F05EB0C.
*/
void sub_GAME_7F05EB0C(ObjectRecord *obj, coord3d *pos, StandTile *stan, Mtxf *matrix, coord3d *arg4, Mtxf *arg5, PropRecord *arg6)
{
    PropRecord *temp_s1;
    Projectile *temp_v0;

    temp_s1 = obj->prop;

    if (temp_s1 != NULL)
    {
        chrpropActivate(temp_s1);
        chrpropEnable(temp_s1);
        matrix_scalar_multiply(obj->model->scale, matrix);
        objChangeShading(obj, pos, matrix, stan);

        // loadobjectmodel.c
        setupUpdateObjectRoomPosition(obj);

        chrobjCollisionRelated(obj);
        sub_GAME_7F03FDA8(temp_s1);

        if (obj->runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT)
        {
            temp_v0 = obj->projectile;
            temp_v0->flags |= 0x41;
            obj->projectile->ownerprop = arg6;
            projectileSetSticky(temp_s1);
            matrix_4x4_copy(arg5, &obj->projectile->mtx);
            obj->projectile->speed.f[0] = arg4->f[0];
            obj->projectile->speed.f[1] = arg4->f[1];
            obj->projectile->speed.f[2] = arg4->f[2];
            obj->projectile->obj = obj;
            obj->projectile->unkE8 = D_80048380;
        }
    }
}





#ifdef NONMATCHING
void sub_GAME_7F05EC1C(void)
{
    f32 spD0;
    f32 spCC;
    f32 spC8;
    s32 spC4;
    f32 spB8;
    f32 spB4;
    s32 spB0;
    s8  spA9;
    s8  spA8;
    ? sp54;
    ? sp50;
    f32   temp_f0;
    f32   temp_f14;
    void *temp_s0;
    f32   phi_f16;
    f32   phi_f14;
    s32   phi_return;

    phi_return = arg0->unk10;
    if (arg0->unk10 != 0)
    {
        temp_s0 = get_curplayer_positiondata();
        temp_f0 = sub_GAME_7F089778(pPlayer);
        spB0    = 0;
        if (arg1->unk4 < temp_s0->unkC)
        {
            phi_f16 = arg1->unk4 - temp_f0;
            phi_f14 = temp_s0->unkC - temp_f0;
        }
        else
        {
            phi_f16 = temp_s0->unkC - temp_f0;
            phi_f14 = arg1->unk4 - temp_f0;
        }
        spB4 = phi_f16;
        spB8 = phi_f14;
        spC4 = temp_s0->unk14;
        bondviewUpdateGuardTankFlagsRelated(temp_s0->unkC, phi_f14, temp_s0, 0);
        temp_f14 = phi_f14;
        if (stanTestLineUnobstructed(temp_f14,
                              &spC4,
                              temp_s0->unk8,
                              temp_s0->unk10,
                              arg1->unk0,
                              arg1->unk8,
                              CDTYPE_OBJS | CDTYPE_DOORS | CDTYPE_PLAYERS | CDTYPE_CHRS | CDTYPE_PATHBLOCKER,
                              temp_f14,
                              phi_f16,
                              0.0f,
                              1.0f) != 0)
        {
            spC8 = (bitwise f32)arg1->unk0;
            spCC = arg1->unk4;
            spD0 = arg1->unk8;
        }
        else
        {
            spC4 = temp_s0->unk14;
            spC8 = (bitwise f32)temp_s0->unk8;
            spCC = temp_s0->unkC;
            spB0 = 1;
            spD0 = (bitwise f32)temp_s0->unk10;
        }
        bondviewUpdateGuardTankFlagsRelated(temp_s0, 1);
        phi_return =
            sub_GAME_7F05EB0C(arg0, &spC8, spC4, arg2, arg3, arg4, temp_s0);
        if ((arg0->unk64 & 0x80) != 0)
        {
            if (spB0 != 0)
            {
                arg0->unk6C->unk0  = (s32)(arg0->unk6C->unk0 | 0x100);
                arg0->unk6C->unkD4 = (bitwise f32)arg1->unk0;
                arg0->unk6C->unkD8 = (f32)arg1->unk4;
                arg0->unk6C->unkDC = (f32)arg1->unk8;
            }
            spA8       = get_cur_players_room();
            spA9       = (u8)0xFF;
            phi_return = sub_GAME_7F0B4AB4(get_BONDdata_position3(),
                                           &spC8,
                                           &spA8,
                                           arg0->unk6C + 0xCC,
                                           &sp54,
                                           &sp50,
                                           0x14);
        }
    }
    return phi_return;
}
#else

void _asmpp_func3(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func4(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif




/**
 * Address 0x7F05EE24 (NTSC)
 * Address 0x7F05F2DC (PAL)
*/
void generate_player_thrown_grenade(s32 hand)
{
    s32 padding;
    Mtxf spFC;
    struct coord3d throw_speed_vec;
    f32 base_velocity;
    struct coord3d spE0;
    Mtxf spA0_a;
    struct WeaponObjRecord *wor;
    s32 new_prop_type;
    s32 sp94; // sp148
    struct coord3d base_speed_vec; // sp136
    struct PropRecord* player_prop; // sp132
    struct coord3d *bondprevpos;  // sp128
    Mtxf sp40_f;
    ALSoundState *sfx_state;
    s32 current_weapon;
    s32 unused;

    wor = NULL;
    base_velocity = 16.666666f;

    player_prop = get_curplayer_positiondata();
    bondprevpos = get_BONDdata_field408();
    current_weapon = getCurrentPlayerWeaponId(hand);

    sub_GAME_7F057C14(&throw_speed_vec, &spFC);
    bullet_path_from_screen_center(&sp94, &base_speed_vec, hand);
    mtx4RotateVecInPlace(currentPlayerGetMatrix10D4(), (f32*)&base_speed_vec);

    throw_speed_vec.f[0] = (base_speed_vec.f[0] * base_velocity);
    throw_speed_vec.f[1] = (base_speed_vec.f[1] * base_velocity) + 5.0f;
    throw_speed_vec.f[2] = (base_speed_vec.f[2] * base_velocity);

    if (g_ClockTimer > 0)
    {
        throw_speed_vec.f[0] = ((player_prop->pos.f[0] - bondprevpos->f[0]) / g_GlobalTimerDelta) + throw_speed_vec.f[0];
        throw_speed_vec.f[1] = ((player_prop->pos.f[1] - bondprevpos->f[1]) / g_GlobalTimerDelta) + throw_speed_vec.f[1];
        throw_speed_vec.f[2] = ((player_prop->pos.f[2] - bondprevpos->f[2]) / g_GlobalTimerDelta) + throw_speed_vec.f[2];
    }

    spE0.f[0] = g_CurrentPlayer->hands[hand].throw_item_pos_related.m[3][0];
    spE0.f[1] = g_CurrentPlayer->hands[hand].throw_item_pos_related.m[3][1];
    spE0.f[2] = g_CurrentPlayer->hands[hand].throw_item_pos_related.m[3][2];

    matrix_4x4_set_identity(&spA0_a);
    matrix_4x4_copy(&g_CurrentPlayer->hands[hand].throw_item_pos_related, &sp40_f);
    sp40_f.m[3][0] = 0.0f;
    sp40_f.m[3][1] = 0.0f;
    sp40_f.m[3][2] = 0.0f;
    matrix_4x4_multiply_in_place(&sp40_f, &spA0_a);

    wor = create_new_item_instance_of_model(PROP_CHRGRENADE, current_weapon);

    if (wor != NULL)
    {
        wor->timer = THROWN_ITEM_TIMER_DEFAULT - g_CurrentPlayer->last_z_trigger_timer;

        if (wor->timer < 0)
        {
            wor->timer = 0;
        }

        wor->runtime_bitflags &= ~(RUNTIMEBITFLAG_OWNER);
        wor->runtime_bitflags |= get_cur_playernum() << RUNTIMEBITSHIFT_OWNER;

        sub_GAME_7F05EC1C(wor, &spE0, &spA0_a, &throw_speed_vec, &spFC);

        if ((wor->runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT) != 0)
        {
            wor->projectile->flags = (s32) (wor->projectile->flags | 2);

            wor->projectile->unk8C = 0.3f;
            wor->projectile->unk94 = 0.13333333f;
            wor->projectile->refreshrate = THROWN_ITEM_REFRESH_RATE;

            sfx_state = sndPlaySfx((struct ALBankAlt_s *) g_musicSfxBufferPtr, 4, NULL);

            if (sfx_state != NULL)
            {
                chrobjSndCreatePostEventDefault(sfx_state, (struct coord3d *) &wor->runtime_pos);
            }
        }
    }
}


/**
 * Address 0x7F05F09C (NTSC)
 * Address 0x7F05F554 (PAL)
*/
void generate_player_thrown_knife(s32 hand)
{
    struct WeaponObjRecord *wor;
    Mtxf spFC;
    struct coord3d throw_speed_vec;
    f32 base_velocity;
    struct coord3d spE0;
    Mtxf spA0_a;
    s32 padding;
    s32 new_prop_type;
    s32 sp94;
    struct coord3d base_speed_vec;
    Mtxf sp40_f;
    struct PropRecord* player_prop;
    struct coord3d *bondprevpos;

    wor = NULL;
    base_velocity = 25.0f;

    player_prop = get_curplayer_positiondata();
    bondprevpos = get_BONDdata_field408();

    sub_GAME_7F057C14(&throw_speed_vec, &spFC);
    bullet_path_from_screen_center(&sp94, &base_speed_vec, hand);
    mtx4RotateVecInPlace(currentPlayerGetMatrix10D4(), (f32*)&base_speed_vec);

    throw_speed_vec.f[0] = (base_speed_vec.f[0] * base_velocity);
    throw_speed_vec.f[1] = (base_speed_vec.f[1] * base_velocity) + 5.0f;
    throw_speed_vec.f[2] = (base_speed_vec.f[2] * base_velocity);

    if (g_ClockTimer > 0)
    {
        throw_speed_vec.f[0] = ((player_prop->pos.f[0] - bondprevpos->f[0]) / g_GlobalTimerDelta) + throw_speed_vec.f[0];
        throw_speed_vec.f[1] = ((player_prop->pos.f[1] - bondprevpos->f[1]) / g_GlobalTimerDelta) + throw_speed_vec.f[1];
        throw_speed_vec.f[2] = ((player_prop->pos.f[2] - bondprevpos->f[2]) / g_GlobalTimerDelta) + throw_speed_vec.f[2];
    }

    spE0.f[0] = g_CurrentPlayer->hands[hand].throw_item_pos_related.m[3][0];
    spE0.f[1] = g_CurrentPlayer->hands[hand].throw_item_pos_related.m[3][1];
    spE0.f[2] = g_CurrentPlayer->hands[hand].throw_item_pos_related.m[3][2];

    matrix_4x4_set_rotation_around_z(4.712389f, &spA0_a);
    matrix_4x4_set_rotation_around_x(M_PI_F, &sp40_f);
    matrix_4x4_multiply_in_place(&sp40_f, &spA0_a);
    matrix_4x4_copy(&g_CurrentPlayer->hands[hand].throw_item_pos_related, &sp40_f);

    sp40_f.m[3][0] = 0.0f;
    sp40_f.m[3][1] = 0.0f;
    sp40_f.m[3][2] = 0.0f;
    matrix_4x4_multiply_in_place(&sp40_f, &spA0_a);

    guRotateF(&spFC, 360.0f / ((randomGetNext() * (0.5f / (f32)INT_MAX)) + 12.1f), spA0_a.m[1][0], spA0_a.m[1][1], spA0_a.m[1][2]);

    wor = create_new_item_instance_of_model(PROP_CHRKNIFE, ITEM_THROWKNIFE);

    if (wor != NULL)
    {
        wor->runtime_bitflags &= ~(RUNTIMEBITFLAG_OWNER);
        wor->runtime_bitflags |= get_cur_playernum() << RUNTIMEBITSHIFT_OWNER;

        sub_GAME_7F05EC1C(wor, &spE0, &spA0_a, &throw_speed_vec, &spFC);

        if ((wor->runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT) != 0)
        {
            wor->projectile->flags = (s32) (wor->projectile->flags | 2);

            wor->projectile->unk8C = 0.1f;
            wor->projectile->refreshrate = THROWN_ITEM_REFRESH_RATE;

            wor->runtime_bitflags |= RUNTIMEBITFLAG_THROWING_KNIFE_RELATED;
        }

        sub_GAME_7F043650(wor);
    }
}





/**
 * Address 0x7F05F358 (NTSC)
 * Address 0x7F05F810 (PAL)
*/
void generate_player_thrown_object(s32 hand)
{
/*
    else {
        assertPrint_8291E690(".\\ported\\gun.cpp",0x8df,"throwmineremote - Not a mine!");
    }
*/

    s32 padding;
    Mtxf unk_mtxf;
    struct coord3d throw_speed_vec;
    f32 base_velocity;
    struct coord3d spE0;
    Mtxf spA0_a;
    struct WeaponObjRecord *wor;
    s32 new_prop_type;
    s32 sp94; // sp148
    struct coord3d base_speed_vec; // sp136
    struct PropRecord* player_prop; // sp132
    struct coord3d *bondprevpos;  // sp128
    Mtxf sp40_f;
    ALSoundState *sfx_state;
    s32 current_weapon;
    s32 unused;

    wor = NULL;
    base_velocity = 16.666666f;

    player_prop = get_curplayer_positiondata();
    bondprevpos = get_BONDdata_field408();
    current_weapon = getCurrentPlayerWeaponId(hand);

    if (current_weapon == ITEM_GOLDENEYEKEY)
    {
        base_velocity = 6.6666665f;
    }

    sub_GAME_7F057C14(&throw_speed_vec, &unk_mtxf);
    bullet_path_from_screen_center(&sp94, &base_speed_vec, hand);
    mtx4RotateVecInPlace(currentPlayerGetMatrix10D4(), (f32*)&base_speed_vec);

    throw_speed_vec.f[0] = (base_speed_vec.f[0] * base_velocity);
    throw_speed_vec.f[1] = (base_speed_vec.f[1] * base_velocity) + 5.0f;
    throw_speed_vec.f[2] = (base_speed_vec.f[2] * base_velocity);

    if (g_ClockTimer > 0)
    {
        throw_speed_vec.f[0] = ((player_prop->pos.f[0] - bondprevpos->f[0]) / g_GlobalTimerDelta) + throw_speed_vec.f[0];
        throw_speed_vec.f[1] = ((player_prop->pos.f[1] - bondprevpos->f[1]) / g_GlobalTimerDelta) + throw_speed_vec.f[1];
        throw_speed_vec.f[2] = ((player_prop->pos.f[2] - bondprevpos->f[2]) / g_GlobalTimerDelta) + throw_speed_vec.f[2];
    }

    spE0.f[0] = g_CurrentPlayer->hands[hand].throw_item_pos_related.m[3][0];
    spE0.f[1] = g_CurrentPlayer->hands[hand].throw_item_pos_related.m[3][1];
    spE0.f[2] = g_CurrentPlayer->hands[hand].throw_item_pos_related.m[3][2];

    matrix_4x4_set_identity(&spA0_a);
    matrix_4x4_copy(&g_CurrentPlayer->hands[hand].throw_item_pos_related, &sp40_f);
    sp40_f.m[3][0] = 0.0f;
    sp40_f.m[3][1] = 0.0f;
    sp40_f.m[3][2] = 0.0f;
    matrix_4x4_multiply_in_place(&sp40_f, &spA0_a);

    if (current_weapon == ITEM_GOLDENEYEKEY)
    {
        wor = bondinvRemovePropWeaponByID(current_weapon);
        bondinvRemoveItemByID(current_weapon);

        if (wor != NULL)
        {
            objDetach(wor->prop);
        }

        sub_GAME_7F05D690();
    }

    if (wor == NULL)
    {
        new_prop_type = PROP_CHRREMOTEMINE;

        switch (current_weapon)
        {
        case ITEM_PROXIMITYMINE:
            new_prop_type = PROP_CHRPROXIMITYMINE;
            break;
        case ITEM_TIMEDMINE:
            new_prop_type = PROP_CHRTIMEDMINE;
            break;
        case ITEM_BOMBCASE:
            new_prop_type = PROP_CHRBOMBCASE;
            break;
        case ITEM_BUG:
            new_prop_type = PROP_CHRBUG;
            break;
        case ITEM_MICROCAMERA:
            new_prop_type = PROP_CHRMICROCAMERA;
            break;
        case ITEM_GOLDENEYEKEY:
            new_prop_type = PROP_CHRGOLDENEYEKEY;
            break;
        case ITEM_PLASTIQUE:
            new_prop_type = PROP_CHRPLASTIQUE;
            break;
        }

        wor = create_new_item_instance_of_model(new_prop_type, current_weapon);
    }

    if (wor != NULL)
    {
        switch (current_weapon)
        {
            case ITEM_REMOTEMINE:
            if (getPlayerCount() == 1)
            {
                wor->timer = THROWN_ITEM_TIMER_SOLO;
            }
            else
            {
                wor->timer = THROWN_ITEM_TIMER_MULTI;
            }
            break;

            case ITEM_PROXIMITYMINE:
            if (getPlayerCount() == 1)
            {
                wor->timer = THROWN_ITEM_TIMER_SOLO;
            }
            else
            {
                wor->timer = THROWN_ITEM_TIMER_MULTI;
            }
            break;

            case ITEM_TIMEDMINE:
            if (getPlayerCount() == 1)
            {
                wor->timer = THROWN_ITEM_TIMER_SOLO;
            }
            else
            {
                wor->timer = THROWN_ITEM_TIMER_MULTI;
            }
            break;

            case ITEM_BOMBCASE:
            if (getPlayerCount() == 1)
            {
                wor->timer = THROWN_ITEM_TIMER_SOLO;
            }
            else
            {
                wor->timer = THROWN_ITEM_TIMER_MULTI;
            }
            break;

            case ITEM_PLASTIQUE:
            case ITEM_BUG:
            case ITEM_MICROCAMERA:
            case ITEM_GOLDENEYEKEY:
                wor->timer = 1;
            break;

            default:
                wor->timer = THROWN_ITEM_TIMER_DEFAULT;
            break;
        }

        wor->runtime_bitflags &= ~(RUNTIMEBITFLAG_OWNER);
        wor->runtime_bitflags |= get_cur_playernum() << RUNTIMEBITSHIFT_OWNER;

        sub_GAME_7F05EC1C(wor, &spE0, &spA0_a, &throw_speed_vec, &unk_mtxf);

        if ((wor->runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT) != 0)
        {
            wor->projectile->flags = (s32) (wor->projectile->flags | 2);

            wor->projectile->unk8C = 0.1f;
            wor->projectile->refreshrate = THROWN_ITEM_REFRESH_RATE;

            sfx_state = sndPlaySfx((struct ALBankAlt_s *) g_musicSfxBufferPtr, 4, NULL);

            if (sfx_state != NULL)
            {
                chrobjSndCreatePostEventDefault(sfx_state, (struct coord3d *) &wor->runtime_pos);
            }
        }
    }
}




#ifdef NONMATCHING
void sub_GAME_7F05F73C(void) {

}
#else

#if defined(VERSION_US) || defined(VERSION_JP)

void _asmpp_func5(void) {









*(volatile float*)4 = -4.214055994530569e+19f;


*(volatile float*)4 = -4.21405643433522e+19f;


*(volatile float*)4 = -4.214056874139871e+19f;


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func6(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif

#if defined(VERSION_EU)

void _asmpp_func7(void) {









*(volatile float*)4 = -4.214057313944522e+19f;


*(volatile float*)4 = -4.214057753749173e+19f;


*(volatile float*)4 = -4.214058193553824e+19f;


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func8(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif
#endif





#ifdef NONMATCHING
void sub_GAME_7F05F928(void) {

}
#else

#if defined(VERSION_US) || defined(VERSION_JP)

void _asmpp_func9(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif

#if defined(VERSION_EU)

void _asmpp_func10(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif
#endif



void currentPlayerCreateRocket(GUNHAND hand)
{
    struct hand * hand_ptr;
    struct WeaponObjRecord * rocket;

    hand_ptr = &g_CurrentPlayer->hands[hand];

    if ((hand_ptr->rocket == NULL) && (hand_ptr->weapon_ammo_in_magazine > 0))
    {
        rocket = (struct WeaponObjRecord *)create_new_item_instance_of_model(PROP_CHRROCKET, ITEM_NULL86);
        if (rocket != NULL)
        {
            hand_ptr->rocket = (ObjectRecord *)rocket;
            hand_ptr->firedrocket = 0;
            rocket->timer = 1;
        }
    }
}



/* This function frees some sort of ObjectRecord from the given hand */
void sub_GAME_7F05FB00(enum GUNHAND hand)
{
    struct hand* hand_ptr;
    ObjectRecord* hand_obj_record;

    hand_ptr = &g_CurrentPlayer->hands[hand];
    hand_obj_record = hand_ptr->rocket;

    if (hand_obj_record != NULL)
    {
        objFreePermanently(hand_obj_record, 1);
        hand_ptr->rocket = NULL;
    }
}



#ifdef NONMATCHING
void gunFireTankShell(void) {

}
#else

#if defined(VERSION_US) || defined(VERSION_JP)

void _asmpp_func11(void) {







*(volatile float*)4 = -4.214058633358475e+19f;


*(volatile float*)4 = -4.214059073163126e+19f;


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func12(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func13(void) { 

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif

#if defined(VERSION_EU)

void _asmpp_func14(void) {







*(volatile float*)4 = -4.2140595129677775e+19f;


*(volatile float*)4 = -4.214059952772429e+19f;


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func15(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func16(void) { 

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif
#endif





#ifdef NONMATCHING
void handles_firing_or_throwing_weapon_in_hand(void) {

}
#else

#ifdef VERSION_US

void _asmpp_func17(void) {








































































*(volatile float*)4 = -4.21406039257708e+19f;


switch (*(volatile int*)4) { case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39: case 40: ; }








*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func18(void) { 
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func19(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func20(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
 } void _asmpp_large_func21(void) { 

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func22(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func23(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func24(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func25(void) { 

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func26(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func27(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func28(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func29(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func30(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func31(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

 } void _asmpp_large_func32(void) { 
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;


*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func33(void) { 
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif

#ifdef VERSION_JP

void _asmpp_func34(void) {








































































*(volatile float*)4 = -4.214060832381731e+19f;


switch (*(volatile int*)4) { case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39: case 40: ; }








*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func35(void) { 
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func36(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func37(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
 } void _asmpp_large_func38(void) { 

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func39(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func40(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func41(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func42(void) { 

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func43(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func44(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func45(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func46(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func47(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func48(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

 } void _asmpp_large_func49(void) { 
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
 } void _asmpp_large_func50(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif

#ifdef VERSION_EU

void _asmpp_func51(void) {








































































*(volatile float*)4 = -4.214061272186382e+19f;


switch (*(volatile int*)4) { case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39: case 40: ; }








*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func52(void) { 
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func53(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func54(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
 } void _asmpp_large_func55(void) { 

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func56(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func57(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func58(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func59(void) { 

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func60(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func61(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func62(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func63(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func64(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func65(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
 } void _asmpp_large_func66(void) { 
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;


*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
 } void _asmpp_large_func67(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif

#endif



void bondwalkFireBothHands(void)
{
    handles_firing_or_throwing_weapon_in_hand(GUNRIGHT);
    handles_firing_or_throwing_weapon_in_hand(GUNLEFT);
}






/**
 * @param arg0:
 * @param item:
 * @param arg2:
 * @param arg3:
 *
 * Address 0x7F061948.
 *
 * This function adjusts the length of the bullet beam that's rendered on screen.
 * This function is used for both player and guard beams.
 *
 * The watch laser has a very short beam, in accordance with its range.
 * The laser also has a shortened one, but it appears this is to avoid graphical glitches.
 * Other weapons have their bullet beam capped at 10000 max length, otherwise if the player
 * fires into the void, there may be graphical glitches with the beam.
 *
*/
void CapBeamLengthAndDecideIfRendered(struct ChrRecord_f180 *arg0, ITEM_IDS item, coord3d *arg2, coord3d *arg3)
{
    f32 phi_f12_2;

    //arg0->pos.x = arg2->x;
    //arg0->pos.y = arg2->y;
    //arg0->pos.z = arg2->z;

    //arg0->delta.x = arg3->x - arg2->x;
    //arg0->delta.y = arg3->y - arg2->y;
    //arg0->delta.z = arg3->z - arg2->z;

    //phi_f12_2 = sqrtf((arg0->delta.f[0] * arg0->delta.f[0]) + (arg0->delta.f[1] * arg0->delta.f[1]) + (arg0->delta.f[2] * arg0->delta.f[2]));

    //arg0->delta.x *= 1.0f / phi_f12_2;
    //arg0->delta.y *= 1.0f / phi_f12_2;
    //arg0->delta.z *= 1.0f / phi_f12_2;


    arg0->pos.f[0] = arg2->f[0];
    arg0->pos.f[1] = arg2->f[1];
    arg0->pos.f[2] = arg2->f[2];

    arg0->delta.f[0] = arg3->x - arg2->x;
    arg0->delta.f[1] = arg3->f[1] - arg2->f[1];
    arg0->delta.f[2] = arg3->f[2] - arg2->f[2];

    phi_f12_2 = sqrtf((arg0->delta.f[0] * arg0->delta.f[0]) + (arg0->delta.f[1] * arg0->delta.f[1]) + (arg0->delta.f[2] * arg0->delta.f[2]));

    arg0->delta.f[0] *= 1.0f / phi_f12_2;
    arg0->delta.f[1] *= 1.0f / phi_f12_2;
    arg0->delta.f[2] *= 1.0f / phi_f12_2;

    if (item == ITEM_WATCHLASER)
    {
        if (phi_f12_2 > 300.0f)
        {
            phi_f12_2 = 300.0f;
        }
    }
    else
    {
        if (phi_f12_2 > 10000.0f)
        {
            phi_f12_2 = 10000.0f;
        }
    }

    arg0->unk00 = 0;
    arg0->item_id = (s8) item;
    arg0->unk1c = phi_f12_2;

    if (phi_f12_2 < 500.0f)
    {
        phi_f12_2 = 500.0f;
    }

    if (item == ITEM_LASER)
    {
        arg0->unk20 = 0.25f * phi_f12_2;
        arg0->unk24 = 0.6f * phi_f12_2;

        if (arg0->unk24 > 3000.0f)
        {
            arg0->unk24 = 3000.0f;
        }

        // Laser beams are rendered more often than other normal weapons
        arg0->unk28 = (-0.1f - ((f32) (u32)randomGetNext() * (1.0f / UINT_MAX) * 0.3f)) * phi_f12_2;
    }
    else if (item == ITEM_WATCHLASER)
    {
        arg0->unk24 = phi_f12_2;
        arg0->unk20 = 2.0f * phi_f12_2;

        if (phi_f12_2 > 3000.0f)
        {
            arg0->unk24 = 3000.0f;
        }

        // Always render the beam for the watch laser
        arg0->unk28 = 0.0f;
    }
    else
    {
        arg0->unk20 = 0.2f * phi_f12_2;
        arg0->unk24 = arg0->unk20;

        if (arg0->unk20 > 3000.0f)
        {
            arg0->unk24 = 3000.0f;
        }

        // Decide if a beam should be rendered for normal weapon bullets
        arg0->unk28 = ((2.0f * ((f32) (u32)randomGetNext() * (1.0f / UINT_MAX))) - 1.0f) * arg0->unk20;
    }

    if (arg0->unk1c <= arg0->unk28)
    {
        // No beam will be rendered
        arg0->unk00 = -1;
    }
}


void sub_GAME_7F061BF4(enum GUNHAND hand) {
    coord3d *field_2A18;
    Mtxf *player_matrix;
    struct hand *hand_ptr;
    f32 val;
    struct ChrRecord *chr;
    f32 diff1_z;
    f32 diff1_y;
    f32 diff1_x;
    f32 diff2_z;
    f32 diff2_y;
    f32 diff2_x;
    ChrRecord_f180 *field_A54;

    hand_ptr = &g_CurrentPlayer->hands[hand];
    player_matrix = camGetWorldToScreenMtxf();

    val = -((((hand_ptr->item_related.x * player_matrix->m[0][2]) + (hand_ptr->item_related.y * player_matrix->m[1][2])) + (hand_ptr->item_related.z * player_matrix->m[2][2])) + player_matrix->m[3][2]);
    if (val < hand_ptr->field_B64) { return; }

    field_A54 = &hand_ptr->field_A54;
    CapBeamLengthAndDecideIfRendered(
        field_A54,
        getCurrentPlayerWeaponId(hand),
        &hand_ptr->field_B58,
        &hand_ptr->item_related);

    if ((g_CurrentPlayer->prop->chr == NULL) || (getPlayerCount() < 2)) { return; }

    chr = g_CurrentPlayer->prop->chr;

    diff1_x = hand_ptr->item_related.x - g_CurrentPlayer->field_2A18[hand].x;
    diff1_y = hand_ptr->item_related.y - g_CurrentPlayer->field_2A18[hand].y;
    diff1_z = hand_ptr->item_related.z - g_CurrentPlayer->field_2A18[hand].z;
    guNormalize(&diff1_x, &diff1_y, &diff1_z);

    diff2_x = hand_ptr->item_related.x - hand_ptr->field_B58.x;
    diff2_y = hand_ptr->item_related.y - hand_ptr->field_B58.y;
    diff2_z = hand_ptr->item_related.z - hand_ptr->field_B58.z;
    guNormalize(&diff2_x, &diff2_y, &diff2_z);

    val = acosf(
        + (diff2_z * diff1_z)
        + ((diff1_x * diff2_x)
        + (diff1_y * diff2_y)));
    if (val > 0.08726647f) { return; }

    CapBeamLengthAndDecideIfRendered(
        &chr->unk180[hand],
        getCurrentPlayerWeaponId(hand),
        &g_CurrentPlayer->field_2A18[hand],
        &hand_ptr->item_related);
}


#ifdef NONMATCHING
void sub_GAME_7F061E18(void) {

}
#else

#if defined(VERSION_US) || defined(VERSION_JP)

void _asmpp_func68(void) {























*(volatile float*)4 = -4.214061711991033e+19f;


switch (*(volatile int*)4) { case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: ; }








*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

 } void _asmpp_large_func69(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func70(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func71(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func72(void) { 
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func73(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func74(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func75(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func76(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif

#if defined(VERSION_EU)

void _asmpp_func77(void) {























*(volatile float*)4 = -4.214062151795684e+19f;


switch (*(volatile int*)4) { case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: ; }








*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

 } void _asmpp_large_func78(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func79(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func80(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func81(void) { 
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func82(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func83(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func84(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func85(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif
#endif

/*
* Address: 0x7F062B00
*/
void sub_GAME_7F062B00(ChrRecord_f180* arg0)
{
    if (arg0->unk00 >= 0)
    {
        if (g_ClockTimer < 3)
        {
#ifdef VERSION_US
            arg0->unk28 += arg0->unk20 * g_GlobalTimerDelta;
#else
            arg0->unk28 += arg0->unk20 * g_JP_GlobalTimerDelta;
#endif
        }
        else
        {
            arg0->unk28 += arg0->unk20 * (2.0f + ((f32) randomGetNext() * 2.3283064e-10f * 0.5f));
        }

        if (arg0->unk1c <= arg0->unk28)
        {
            arg0->unk00 = -1;
            return;
        }

        arg0->unk00++;
    }
}


#ifdef NONMATCHING
void sub_GAME_7F062BE4(void) {

}
#else

#if defined(VERSION_US) || defined(VERSION_JP)

void _asmpp_func86(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func87(void) { 

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func88(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif

#if defined(VERSION_EU)

void _asmpp_func89(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func90(void) { 

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func91(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif
#endif





#ifdef NONMATCHING
void set_enviro_fog_for_items_in_solo_watch_menu(void) {

}
#else

void _asmpp_func92(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func93(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func94(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
 } void _asmpp_large_func95(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif





#ifdef NONMATCHING
void sub_GAME_7F0634D8(void) {

}
#else

void _asmpp_func96(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif





#ifdef NONMATCHING
void sub_GAME_7F0634FC(void) {

}
#else

void _asmpp_func97(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif



void sub_GAME_7F06351C(struct coord3d* arg0, Mtxf* arg1, Mtxf* arg2, Mtxf* arg3, struct coord3d* arg4, Mtxf* arg5, Mtxf* arg6)
{
    Mtxf sp20;

    matrix_4x4_set_identity_and_position(arg0, arg6);
    matrix_4x4_multiply_in_place(arg1, arg6);
    matrix_4x4_multiply_in_place(arg2, arg6);
    matrix_4x4_multiply_in_place(arg3, arg6);
    matrix_4x4_set_identity_and_position(arg4, &sp20);
    matrix_4x4_multiply_in_place(&sp20, arg6);
    matrix_4x4_multiply_in_place(arg5, arg6);
}



#ifdef NONMATCHING
void sub_GAME_7F06359C(void) {

}
#else

void _asmpp_func98(void) {











































*(volatile float*)4 = -4.214062591600335e+19f;


switch (*(volatile int*)4) { case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: ; }








*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func99(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func100(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func101(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func102(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func103(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func104(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

 } void _asmpp_large_func105(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func106(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif





#ifdef NONMATCHING
void sub_GAME_7F064364(void) {

}
#else

void _asmpp_func107(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif



ALSoundState* sub_GAME_7F0643A0(void)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        if (dword_CODE_bss_80075DB8[i] == 0) {
            return &dword_CODE_bss_80075DB8[i];
        }
    }
    return NULL;
}


void recall_joy2_hits_edit_detail_edit_flag(enum ITEM_IDS item, PropRecord* prop, s32 texture_index)
{
    s32 sp6C;
    u32 rnd1;
    u32 rnd2;
    ALSoundState* sound_state;
    struct RicochetSoundsSmall ricochet_sounds_small_copy;
    struct PunchSounds punch_sounds_copy;
    struct BulletFleshSounds bullet_flesh_sounds_copy;
    u32 sfx_index;

    sp6C = sub_GAME_7F0539E4(&prop->pos);

    rnd1 = randomGetNext();
    rnd2 = randomGetNext();

    D_800483C4 = texture_index;

    if (get_debug_joy2hitsedit_flag() == 0)
    {
        get_debug_joy2detailedit_flag();
    }

    if ((item == ITEM_REMOTEMINE)
        || (item == ITEM_PROXIMITYMINE)
        || (item == ITEM_TIMEDMINE)
        || (item == ITEM_BOMBCASE)
        || (item == ITEM_BUG)
        || (item == ITEM_MICROCAMERA)
        || (item == ITEM_PLASTIQUE)
        || (item == ITEM_WATCHLASER)
        || (item == ITEM_WATCHMAGNETATTRACT))
    {
        return;
    }

#ifdef BUGFIX_R1
    if (g_ClockTimer <= 0) { return; }
#endif

    sound_state = sub_GAME_7F0643A0();
    if (sound_state != NULL)
    {
        if ((prop->type != 3) && (prop->type != 6))
        {
            if (item == ITEM_LASER)
            {
                sndPlaySfx((struct ALBankAlt_s* ) g_musicSfxBufferPtr, 0x5B, sound_state);
            }
            else
            {
                ricochet_sounds_small_copy = ricochet_sounds_small;
                sndPlaySfx((struct ALBankAlt_s* ) g_musicSfxBufferPtr, ricochet_sounds_small_copy.arr[rnd1 % 20], sound_state);
            }

            if (sound_state->link.next != NULL)
            {
                sndCreatePostEvent((ALSoundState* ) sound_state->link.next, 8, sp6C);
            }
        }
        else
        {
            if (item == ITEM_KNIFE)
            {
                sndPlaySfx((struct ALBankAlt_s* ) g_musicSfxBufferPtr, 0x4A, sound_state);
            }
            else if (item == ITEM_FIST)
            {
                punch_sounds_copy = punch_sounds;
                sndPlaySfx((struct ALBankAlt_s* ) g_musicSfxBufferPtr, punch_sounds_copy.arr[rnd1 % 3], sound_state);
            }
            else
            {
                bullet_flesh_sounds_copy = bullet_flesh_sounds;
                sndPlaySfx((struct ALBankAlt_s* ) g_musicSfxBufferPtr, bullet_flesh_sounds_copy.arr[rnd1 % 2], sound_state);
            }

            if (sound_state->link.next != NULL) {
                sndCreatePostEvent((ALSoundState* ) sound_state->link.next, 8, sp6C);
            }
        }
    }

    sound_state = sub_GAME_7F0643A0();
    if ((sound_state != NULL) && (texture_index >= 0))
    {
        if (D_8004E86C[g_Textures[texture_index].hitSound] != NULL)
        {
            if (D_8004E86C[g_Textures[texture_index].hitSound]->sfx_len > 0)
            {
                sfx_index = rnd2 % D_8004E86C[g_Textures[texture_index].hitSound]->sfx_len;
                sndPlaySfx((struct ALBankAlt_s* ) g_musicSfxBufferPtr, D_8004E86C[g_Textures[texture_index].hitSound]->sfx[sfx_index], sound_state);
            }

            if (sound_state->link.next != NULL)
            {
                chrobjSndCreatePostEventDefault((ALSoundState* ) sound_state->link.next, &prop->pos);
            }
        }
    }
}


void sub_GAME_7F064720(coord3d* pos)
{
    ALSoundState* sound;
    ALLink* link;

#ifdef BUGFIX_R1
    if (g_ClockTimer <= 0) { return; }
#endif

    sound = sub_GAME_7F0643A0();

    if (sound != NULL)
    {
        sndPlaySfx((struct ALBankAlt_s* ) g_musicSfxBufferPtr, 0x46, sound);

        link = sound->link.next;
        if (link != NULL)
        {
            chrobjSndCreatePostEventDefault((ALSoundState* ) link, pos);
        }
    }
}


void recall_joy2_hits_edit_flag(enum ITEM_IDS item, coord3d* arg1, s32 texture_index)
{
    ALSoundState *sound_state;
    u32 rnd1;
    u32 rnd2;
    struct LaserRichochetSounds laser_copied;
    struct RicochetSoundsLarge rico_copied;
    u32 sfx_index;
    struct image_sound *img_sound;

    rnd1 = randomGetNext();
    rnd2 = randomGetNext();

    D_800483C4 = texture_index;
    get_debug_joy2hitsedit_flag();

#ifdef BUGFIX_R1
    if (g_ClockTimer <= 0) { return; }
#endif

    sound_state = sub_GAME_7F0643A0();
    if (sound_state != NULL)
    {
        if (item != ITEM_WATCHLASER)
        {
            if (item == ITEM_LASER)
            {
                laser_copied = laser_ricochet_sounds;
                sndPlaySfx((struct ALBankAlt_s* ) g_musicSfxBufferPtr, laser_copied.arr[rnd1 % 2], sound_state);
            }
            else
            {
                rico_copied = ricochet_sounds_large;
                sndPlaySfx((struct ALBankAlt_s* ) g_musicSfxBufferPtr, rico_copied.arr[rnd1 % 36], sound_state);
            }
        }

        if (sound_state->link.next != NULL)
        {
            chrobjSndCreatePostEventDefault((ALSoundState* ) sound_state->link.next, arg1);
        }
    }

    sound_state = sub_GAME_7F0643A0();
    if ((sound_state != NULL) && (texture_index >= 0))
    {
        img_sound = D_8004E86C[g_Textures[texture_index].hitSound];
        if (img_sound->sfx_len > 0)
        {
            if (img_sound != NULL)
            {
                sfx_index = rnd2 % img_sound->sfx_len;
                sndPlaySfx((struct ALBankAlt_s* ) g_musicSfxBufferPtr, img_sound->sfx[sfx_index], sound_state);
            }

            if (sound_state->link.next != NULL)
            {
                chrobjSndCreatePostEventDefault((ALSoundState* ) sound_state->link.next, arg1);
            }
        }
    }
}


void sub_GAME_7F064934(ITEM_IDS item)
{
    struct EarWhistleSounds copied;

#ifdef BUGFIX_R1
    if (g_ClockTimer <= 0) { return; }
#endif
    if ((item != ITEM_LASER) && (item != ITEM_WATCHLASER))
    {
        copied = ear_whistle_sounds;
        sndPlaySfx((struct ALBankAlt_s*) g_musicSfxBufferPtr, copied.arr[randomGetNext() % 5], 0);
    }
}


f32 sub_GAME_7F0649AC(s32 param_1)
{
  f32 fVar1;

  fVar1 = -60.0f;
  if (param_1 == 0x19) {
    fVar1 -= 20.0f;
  }
  return fVar1;
}



void sub_GAME_7F0649D8(enum GUNHAND hand)
{
    struct hand* hand_ptr;
    enum ITEM_IDS item_id;
    s32 ammo_in_magazine;
    s32 ammo_in_hands;
    WeaponStats* item_stats;
    s32 magsize;
    s32 ammo_total;

    hand_ptr = &g_CurrentPlayer->hands[hand];
    item_id = getCurrentPlayerWeaponId(hand);
    ammo_in_magazine = hand_ptr->weapon_ammo_in_magazine;
    ammo_in_hands = get_ammo_in_hands_weapon(hand);
    item_stats = get_ptr_item_statistics(item_id);
    ammo_total = ammo_in_hands + ammo_in_magazine;

    hand_ptr->weapon_ammo_in_magazine = (ammo_total >= item_stats->MagSize)
        ? item_stats->MagSize
        : ammo_total;

    g_CurrentPlayer->ammoheldarr[item_stats->AmmoType] = (bondwalkItemCheckBitflags(item_id, WEAPONSTATBITFLAG_AMMO_CLIP_LIMIT) != 0)
        ? 0
        : (g_CurrentPlayer->ammoheldarr[item_stats->AmmoType] - hand_ptr->weapon_ammo_in_magazine) + ammo_in_magazine;

    if (item_id == ITEM_ROCKETLAUNCH)
    {
        currentPlayerCreateRocket(hand);
        return;
    }

    if ((item_id == ITEM_SHOTGUN) || (item_id == ITEM_AUTOSHOT))
    {
        ammo_in_hands = get_ammo_in_hands_weapon(hand);
        if (ammo_in_hands >= 5)
        {
            hand_ptr->field_8A4 = 5;
            return;
        }
        hand_ptr->field_8A4 = ammo_in_hands;
    }
}



#ifdef NONMATCHING
void handle_weapon_id_values_possibly_1st_person_animation(void) {

}
#else

#ifdef VERSION_US

void _asmpp_func108(void) {











































































































































































































































































































































*(volatile float*)4 = -4.214063031404986e+19f;


switch (*(volatile int*)4) { case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39: case 40: case 41: case 42: case 43: case 44: case 45: case 46: case 47: case 48: case 49: case 50: case 51: case 52: case 53: case 54: case 55: case 56: case 57: case 58: case 59: case 60: case 61: case 62: case 63: case 64: case 65: case 66: case 67: case 68: case 69: case 70: case 71: case 72: case 73: case 74: case 75: case 76: case 77: case 78: case 79: case 80: case 81: case 82: case 83: case 84: case 85: case 86: case 87: case 88: case 89: case 90: case 91: case 92: case 93: case 94: case 95: case 96: case 97: case 98: case 99: case 100: case 101: case 102: case 103: case 104: case 105: case 106: case 107: case 108: case 109: case 110: case 111: case 112: case 113: case 114: case 115: case 116: case 117: case 118: case 119: case 120: case 121: case 122: case 123: case 124: case 125: case 126: case 127: case 128: case 129: case 130: case 131: case 132: case 133: case 134: case 135: case 136: case 137: case 138: case 139: case 140: case 141: case 142: case 143: case 144: case 145: case 146: case 147: case 148: case 149: case 150: case 151: case 152: case 153: case 154: case 155: case 156: case 157: case 158: case 159: case 160: case 161: case 162: case 163: case 164: case 165: case 166: case 167: case 168: case 169: case 170: case 171: case 172: case 173: case 174: case 175: case 176: case 177: case 178: case 179: case 180: case 181: case 182: case 183: case 184: case 185: case 186: case 187: case 188: case 189: case 190: case 191: case 192: case 193: case 194: case 195: case 196: case 197: case 198: case 199: case 200: case 201: case 202: case 203: case 204: case 205: case 206: case 207: case 208: case 209: case 210: case 211: case 212: case 213: case 214: case 215: case 216: case 217: case 218: case 219: case 220: case 221: case 222: case 223: case 224: case 225: case 226: case 227: case 228: case 229: case 230: case 231: case 232: case 233: case 234: case 235: case 236: case 237: case 238: case 239: case 240: case 241: case 242: case 243: case 244: case 245: case 246: case 247: case 248: case 249: case 250: case 251: case 252: case 253: case 254: case 255: case 256: case 257: case 258: case 259: case 260: case 261: case 262: case 263: case 264: case 265: case 266: case 267: case 268: case 269: case 270: case 271: case 272: case 273: case 274: case 275: case 276: case 277: case 278: case 279: case 280: case 281: case 282: case 283: case 284: case 285: case 286: case 287: case 288: case 289: ; }








*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

 } void _asmpp_large_func109(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func110(void) { 

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func111(void) { 

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func112(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;


*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func113(void) { 

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
 } void _asmpp_large_func114(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func115(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func116(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func117(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func118(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func119(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func120(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;


*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func121(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func122(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;


*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func123(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func124(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

 } void _asmpp_large_func125(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func126(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func127(void) { 
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func128(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func129(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func130(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif

#ifdef VERSION_JP

void _asmpp_func131(void) {










































































































































































































































































































































*(volatile float*)4 = -4.2140634712096375e+19f;


switch (*(volatile int*)4) { case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39: case 40: case 41: case 42: case 43: case 44: case 45: case 46: case 47: case 48: case 49: case 50: case 51: case 52: case 53: case 54: case 55: case 56: case 57: case 58: case 59: case 60: case 61: case 62: case 63: case 64: case 65: case 66: case 67: case 68: case 69: case 70: case 71: case 72: case 73: case 74: case 75: case 76: case 77: case 78: case 79: case 80: case 81: case 82: case 83: case 84: case 85: case 86: case 87: case 88: case 89: case 90: case 91: case 92: case 93: case 94: case 95: case 96: case 97: case 98: case 99: case 100: case 101: case 102: case 103: case 104: case 105: case 106: case 107: case 108: case 109: case 110: case 111: case 112: case 113: case 114: case 115: case 116: case 117: case 118: case 119: case 120: case 121: case 122: case 123: case 124: case 125: case 126: case 127: case 128: case 129: case 130: case 131: case 132: case 133: case 134: case 135: case 136: case 137: case 138: case 139: case 140: case 141: case 142: case 143: case 144: case 145: case 146: case 147: case 148: case 149: case 150: case 151: case 152: case 153: case 154: case 155: case 156: case 157: case 158: case 159: case 160: case 161: case 162: case 163: case 164: case 165: case 166: case 167: case 168: case 169: case 170: case 171: case 172: case 173: case 174: case 175: case 176: case 177: case 178: case 179: case 180: case 181: case 182: case 183: case 184: case 185: case 186: case 187: case 188: case 189: case 190: case 191: case 192: case 193: case 194: case 195: case 196: case 197: case 198: case 199: case 200: case 201: case 202: case 203: case 204: case 205: case 206: case 207: case 208: case 209: case 210: case 211: case 212: case 213: case 214: case 215: case 216: case 217: case 218: case 219: case 220: case 221: case 222: case 223: case 224: case 225: case 226: case 227: case 228: case 229: case 230: case 231: case 232: case 233: case 234: case 235: case 236: case 237: case 238: case 239: case 240: case 241: case 242: case 243: case 244: case 245: case 246: case 247: case 248: case 249: case 250: case 251: case 252: case 253: case 254: case 255: case 256: case 257: case 258: case 259: case 260: case 261: case 262: case 263: case 264: case 265: case 266: case 267: case 268: case 269: case 270: case 271: case 272: case 273: case 274: case 275: case 276: case 277: case 278: case 279: case 280: case 281: case 282: case 283: case 284: case 285: case 286: case 287: case 288: case 289: ; }








*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func132(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func133(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func134(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func135(void) { 
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func136(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

 } void _asmpp_large_func137(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func138(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func139(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func140(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

 } void _asmpp_large_func141(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func142(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func143(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func144(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func145(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func146(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func147(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func148(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func149(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func150(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func151(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func152(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func153(void) { 

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif

#ifdef VERSION_EU

void _asmpp_func154(void) {











































































































































































































































































































































*(volatile float*)4 = -4.2140639110142886e+19f;


switch (*(volatile int*)4) { case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39: case 40: case 41: case 42: case 43: case 44: case 45: case 46: case 47: case 48: case 49: case 50: case 51: case 52: case 53: case 54: case 55: case 56: case 57: case 58: case 59: case 60: case 61: case 62: case 63: case 64: case 65: case 66: case 67: case 68: case 69: case 70: case 71: case 72: case 73: case 74: case 75: case 76: case 77: case 78: case 79: case 80: case 81: case 82: case 83: case 84: case 85: case 86: case 87: case 88: case 89: case 90: case 91: case 92: case 93: case 94: case 95: case 96: case 97: case 98: case 99: case 100: case 101: case 102: case 103: case 104: case 105: case 106: case 107: case 108: case 109: case 110: case 111: case 112: case 113: case 114: case 115: case 116: case 117: case 118: case 119: case 120: case 121: case 122: case 123: case 124: case 125: case 126: case 127: case 128: case 129: case 130: case 131: case 132: case 133: case 134: case 135: case 136: case 137: case 138: case 139: case 140: case 141: case 142: case 143: case 144: case 145: case 146: case 147: case 148: case 149: case 150: case 151: case 152: case 153: case 154: case 155: case 156: case 157: case 158: case 159: case 160: case 161: case 162: case 163: case 164: case 165: case 166: case 167: case 168: case 169: case 170: case 171: case 172: case 173: case 174: case 175: case 176: case 177: case 178: case 179: case 180: case 181: case 182: case 183: case 184: case 185: case 186: case 187: case 188: case 189: case 190: case 191: case 192: case 193: case 194: case 195: case 196: case 197: case 198: case 199: case 200: case 201: case 202: case 203: case 204: case 205: case 206: case 207: case 208: case 209: case 210: case 211: case 212: case 213: case 214: case 215: case 216: case 217: case 218: case 219: case 220: case 221: case 222: case 223: case 224: case 225: case 226: case 227: case 228: case 229: case 230: case 231: case 232: case 233: case 234: case 235: case 236: case 237: case 238: case 239: case 240: case 241: case 242: case 243: case 244: case 245: case 246: case 247: case 248: case 249: case 250: case 251: case 252: case 253: case 254: case 255: case 256: case 257: case 258: case 259: case 260: case 261: case 262: case 263: case 264: case 265: case 266: case 267: case 268: case 269: case 270: case 271: case 272: case 273: case 274: case 275: case 276: case 277: case 278: case 279: case 280: case 281: case 282: case 283: case 284: case 285: case 286: case 287: case 288: case 289: ; }








*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func155(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func156(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

 } void _asmpp_large_func157(void) { 

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func158(void) { 
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;


*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func159(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func160(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func161(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func162(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func163(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func164(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func165(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func166(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func167(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;


*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func168(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func169(void) { 

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;


*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func170(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
 } void _asmpp_large_func171(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func172(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

 } void _asmpp_large_func173(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func174(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func175(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
 } void _asmpp_large_func176(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func177(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif


#endif


void analyzeGEKey(void)
{
    if (bondinvHasGEKey())
    {
   	    HUDMESSAGEBOTTOM(langGet(getStringID(LGUN, GUN_STR_D8_ANALYZINGTHEGOLDENEYEKEY_LF))); //Analyzing the GoldenEye key...
    	g_CurrentPlayer->copiedgoldeneye = TRUE;
    	sndPlaySfx(g_musicSfxBufferPtr, 0xf5, 0x0);
    	currentPlayerEquipWeaponWrapper(GUNRIGHT, ITEM_GOLDENEYEKEY);
    	currentPlayerEquipWeaponWrapper(GUNLEFT, ITEM_UNARMED);
  	}
  	else
  	{
	    HUDMESSAGEBOTTOM(langGet(getStringID(LGUN, GUN_STR_D9_YOUDONOTHAVETHEGOLDENEYEKEY_LF))); //You do not have the GoldenEye key.
	    sub_GAME_7F05D690();
  	}
  	return;
}


int get_keyanalyzer_flag(void)
{
  return g_CurrentPlayer->copiedgoldeneye;
}


void give_weapon_case_items(void)
{
  add_ammo_to_inventory(AMMO_KNIFE, 2, 0, 1);
  add_ammo_to_inventory(AMMO_GRENADE, 2, 0, 1);
  bondinvAddInvItem(ITEM_SNIPERRIFLE);
  set_sound_effect_for_weapontype_collection(ITEM_SNIPERRIFLE);
  display_text_for_weapon_in_lower_left_corner(ITEM_SNIPERRIFLE);
  give_cur_player_ammo(sniperrifle_stats.AmmoType, check_cur_player_ammo_amount_in_inventory(sniperrifle_stats.AmmoType) + sniperrifle_stats.MagSize);
  bondinvRemoveItemByID(ITEM_WEAPONCASE);
  currentPlayerEquipWeaponWrapper(GUNRIGHT,ITEM_SNIPERRIFLE);
  currentPlayerEquipWeaponWrapper(GUNLEFT,ITEM_UNARMED);
}


f32 get_vertical_position_solo_watch_menu_main_page_for_item(ITEM_IDS item)
{
  return gitem_structs[item].watch_pos_x;
}


f32 get_lateral_position_solo_watch_menu_main_page_for_item(ITEM_IDS item)
{
  return gitem_structs[item].watch_pos_y;
}


f32 get_depth_on_solo_watch_menu_page_for_item(ITEM_IDS item)
{
  return gitem_structs[item].watch_pos_z;
}


f32 get_xrotation_solo_watch_menu_for_item(ITEM_IDS item)

{
  return gitem_structs[item].x_rotation;
}


f32 get_yrotation_solo_watch_menu_for_item(ITEM_IDS item)
{
  return gitem_structs[item].y_rotation;
}


f32 get_45_degree_angle(s32 unk) {
  return 45.0f;
}


u16 *get_ptr_first_title_line_item(ITEM_IDS item)
{
  return langGet(gitem_structs[item].upper_watch_text);
}


u16 *get_ptr_second_title_line_item(ITEM_IDS item)
{
    return langGet(gitem_structs[item].lower_watch_text);
}


u16 *get_ptr_short_watch_text_for_item(ITEM_IDS item)
{
    return langGet(gitem_structs[item].watch_equipment_text);
}


u16 *get_ptr_long_watch_text_for_item(ITEM_IDS item)
{
    return langGet(gitem_structs[item].weapon_of_choice_text);
}


f32 get_45_degree_angle_0(s32 unk)
{
	return 45.0f;
}


f32 get_horizontal_offset_on_solo_watch_menu_for_item(ITEM_IDS item)
{
  return gitem_structs[item].equip_watch_x;
}


f32 get_vertical_offset_on_solo_watch_menu_for_item(ITEM_IDS item)
{
  return gitem_structs[item].equip_watch_y;
}


f32 get_depth_offset_solo_watch_menu_inventory_page_for_item(ITEM_IDS item)
{
  return gitem_structs[item].equip_watch_z;
}



f32 getCurrentPlayerNoise(GUNHAND hand)
{
    return g_CurrentPlayer->hands[hand].noise;
}


void sub_GAME_7F0671A4(void)
{
    enum ITEM_IDS weapon_id_right;
    enum ITEM_IDS weapon_id_left;
    s32 unused2;
    f32 noise_reduction;
    WeaponStats *item_right;
    WeaponStats *item_left;
    f32 noise_reduction_max;
    s32 unused;

    weapon_id_right = getCurrentPlayerWeaponId(GUNRIGHT);
    weapon_id_left = getCurrentPlayerWeaponId(GUNLEFT);
    item_right = get_ptr_item_statistics(weapon_id_right);
    item_left = get_ptr_item_statistics(weapon_id_left);

    if (weapon_id_right != ITEM_UNARMED && get_hands_firing_status(GUNRIGHT))
    {
        g_CurrentPlayer->hands[GUNRIGHT].noise += item_right->NoiseIncreasePerShot;

        if (item_right->LoudnessMax < g_CurrentPlayer->hands[GUNRIGHT].noise)
        {
            g_CurrentPlayer->hands[GUNRIGHT].noise = item_right->LoudnessMax;
        }
    }

    if (weapon_id_left != ITEM_UNARMED && get_hands_firing_status(GUNLEFT))
    {
        g_CurrentPlayer->hands[GUNLEFT].noise += item_left->NoiseIncreasePerShot;

        if (item_left->LoudnessMax < g_CurrentPlayer->hands[GUNLEFT].noise)
        {
            g_CurrentPlayer->hands[GUNLEFT].noise = item_left->LoudnessMax;
        }
    }

    noise_reduction = (item_right->NoiseIncreasePerShot * g_GlobalTimerDelta) / (item_right->field_60 * 60.0f);
    noise_reduction_max = ((g_CurrentPlayer->hands[GUNRIGHT].noise - item_right->LoudnessMin) * g_GlobalTimerDelta) / (item_right->field_64 * 60.0f);

    if (noise_reduction < noise_reduction_max)
    {
        noise_reduction = noise_reduction_max;
    }

    g_CurrentPlayer->hands[GUNRIGHT].noise -= noise_reduction;

    if (g_CurrentPlayer->hands[GUNRIGHT].noise < item_right->LoudnessMin)
    {
        g_CurrentPlayer->hands[GUNRIGHT].noise = item_right->LoudnessMin;
    }

    noise_reduction = (item_left->NoiseIncreasePerShot * g_GlobalTimerDelta) / (item_left->field_60 * 60.0f);
    noise_reduction_max = ((g_CurrentPlayer->hands[GUNLEFT].noise - item_left->LoudnessMin) * g_GlobalTimerDelta) / (item_left->field_64 * 60.0f);

    if (noise_reduction < noise_reduction_max)
    {
        noise_reduction = noise_reduction_max;
    }

    g_CurrentPlayer->hands[GUNLEFT].noise -= noise_reduction;

    if (g_CurrentPlayer->hands[GUNLEFT].noise < item_left->LoudnessMin)
    {
        g_CurrentPlayer->hands[GUNLEFT].noise = item_left->LoudnessMin;
    }
}



s32 sub_GAME_7F0673B4(enum GUNHAND hand)
{
    return (get_ammo_type_for_weapon(getCurrentPlayerWeaponId(hand)) == 0)
        || (g_CurrentPlayer->hands[hand].weapon_ammo_in_magazine > 0);
}



/**
 * US address 7F067420.
 * Perfect Dark method bgunTickGameplay.
*/
void gunTickGameplay(s32 triggerOn)
{
    struct unkown_gun_struct sp48z;
    enum ITEM_IDS sp44;
    enum ITEM_IDS sp40;
    enum GUNHAND sp3C = GUNLEFT;
    struct rgba_u8 sp38;


    sp48z = D_80035E9C;

    g_CurrentPlayer->field_FD0 = g_CurrentPlayer->field_FCC;
    g_CurrentPlayer->field_FCC = triggerOn;

    if ((g_CurrentPlayer->field_FCC == 0) && (g_CurrentPlayer->field_FD0 != 0))
    {
        g_CurrentPlayer->field_FC8 = 1;
    }

    if (g_CurrentPlayer->field_FCC != 0)
    {
        sp44 = getCurrentPlayerWeaponId(GUNRIGHT);
        sp40 = getCurrentPlayerWeaponId(GUNLEFT);

        g_CurrentPlayer->z_trigger_timer += g_ClockTimer;

        if ((sp44 != ITEM_UNARMED) && (sp40 != ITEM_UNARMED))
        {
            if ((bondwalkItemCheckBitflags(sp44, 0x80U) != 0) && (bondwalkItemCheckBitflags(sp40, 0x80U) != 0))
            {
                if (g_CurrentPlayer->z_trigger_timer > DUAL_WIELD_TRIGGER_SWAP_TICKS)
                {
                    sp48z.arr[g_CurrentPlayer->field_FD8] = sp3C;

                    if (sub_GAME_7F0673B4(1 - g_CurrentPlayer->field_FD8) || g_CurrentPlayer->hands[1 - g_CurrentPlayer->field_FD8].weapon_hold_time)
                    {
                        sp48z.arr[1 - g_CurrentPlayer->field_FD8] = 1;
                    }
                }
                else
                {
                    if ((g_CurrentPlayer->field_FD0 == 0) &&
                        ((sub_GAME_7F0673B4(1 - g_CurrentPlayer->field_FD8) != 0) || (sub_GAME_7F0673B4(g_CurrentPlayer->field_FD8) == 0)))
                    {
                        g_CurrentPlayer->field_FD8 = 1 - g_CurrentPlayer->field_FD8;
                    }

                    sp48z.arr[g_CurrentPlayer->field_FD8] = 1;
                    sp48z.arr[1 - g_CurrentPlayer->field_FD8] = 0;
                }
            }
            else if ((bondwalkItemCheckBitflags(sp44, 0x80U) != 0) || (bondwalkItemCheckBitflags(sp40, 0x80U) != 0))
            {
                if (g_CurrentPlayer->z_trigger_timer > DUAL_WIELD_SINGLE_TRIGGER_SWAP_TICKS)
                {
                    sp48z.arr[g_CurrentPlayer->field_FD8] = sp3C;

                    if ((sub_GAME_7F0673B4(1 - g_CurrentPlayer->field_FD8) != 0) || g_CurrentPlayer->hands[1 - g_CurrentPlayer->field_FD8].weapon_hold_time != 0)
                    {
                        sp48z.arr[1 - g_CurrentPlayer->field_FD8] = 1;
                    }
                }
                else
                {
                    sp3C = bondwalkItemCheckBitflags(sp44, 0x80U) ? GUNRIGHT : GUNLEFT;

                    if (sub_GAME_7F0673B4(sp3C) != 0 || g_CurrentPlayer->hands[sp3C].weapon_hold_time != 0)
                    {
                        g_CurrentPlayer->field_FD8 = sp3C;
                    }
                    else
                    {
                        if ((sub_GAME_7F0673B4(1 - sp3C) != 0) || g_CurrentPlayer->hands[1 - sp3C].weapon_hold_time != 0)
                        {
                            g_CurrentPlayer->field_FD8 = 1 - sp3C;
                        }
                        else
                        {
                            g_CurrentPlayer->field_FD8 = 1 - g_CurrentPlayer->field_FD8;
                        }
                    }

                    sp48z.arr[g_CurrentPlayer->field_FD8] = 1;
                    sp48z.arr[1 - g_CurrentPlayer->field_FD8] = 0;
                }
            }
            else if (g_CurrentPlayer->z_trigger_timer > DUAL_WIELD_SINGLE_TRIGGER_SWAP_TICKS)
            {
                sp48z.arr[g_CurrentPlayer->field_FD8] = sp3C;

                if (sub_GAME_7F0673B4(1 - g_CurrentPlayer->field_FD8) || g_CurrentPlayer->hands[1 - g_CurrentPlayer->field_FD8].weapon_hold_time)
                {
                    sp48z.arr[1 - g_CurrentPlayer->field_FD8] = 1;
                }
            }
            else
            {
                if ((g_CurrentPlayer->field_FD0 == 0) &&
                    ((sub_GAME_7F0673B4(1 - g_CurrentPlayer->field_FD8) != 0) || (sub_GAME_7F0673B4(g_CurrentPlayer->field_FD8) == 0)))
                {
                    g_CurrentPlayer->field_FD8 = 1 - g_CurrentPlayer->field_FD8;
                }

                sp48z.arr[g_CurrentPlayer->field_FD8] = 1;
                sp48z.arr[1 - g_CurrentPlayer->field_FD8] = 0;
            }
        }
        else
        {
            if ((getCurrentPlayerWeaponId(g_CurrentPlayer->field_FD8) == ITEM_UNARMED) && (getCurrentPlayerWeaponId(1 - g_CurrentPlayer->field_FD8) != ITEM_UNARMED))
            {
                g_CurrentPlayer->field_FD8 = 1 - g_CurrentPlayer->field_FD8;
            }

            sp48z.arr[g_CurrentPlayer->field_FD8] = 1;
            sp48z.arr[1 - g_CurrentPlayer->field_FD8] = 0;
        }
    }
    else
    {
        g_CurrentPlayer->z_trigger_timer = 0;
    }

    handle_weapon_id_values_possibly_1st_person_animation(0, sp48z.arr[0]);
    handle_weapon_id_values_possibly_1st_person_animation(1, sp48z.arr[1]);
    used_to_load_1st_person_model_on_demand(0);
    used_to_load_1st_person_model_on_demand(1);
    sub_GAME_7F0671A4();

    if (g_CurrentPlayer->resetshadecol)
    {
        set_color_shading_from_tile(get_curplayer_positiondata(), (struct rgba_u8 *) &g_CurrentPlayer->tileColor);
        g_CurrentPlayer->resetshadecol = FALSE;
    }
    else
    {
        set_color_shading_from_tile(get_curplayer_positiondata(), &sp38);
        update_color_shading(&g_CurrentPlayer->tileColor, &sp38);
    }

    bondinvIncrementHeldTime(getCurrentPlayerWeaponId(GUNRIGHT), getCurrentPlayerWeaponId(GUNLEFT));

    if(1);

    if (g_CurrentPlayer->magnetattracttime >= 0)
    {
        struct hand *sp34 = &g_CurrentPlayer->hands[0];

        g_CurrentPlayer->magnetattracttime += g_ClockTimer;

        if (g_CurrentPlayer->magnetattracttime < WATCH_SOUND_DURATION_TICKS)
        {
            if (sp34->audioHandle == NULL
                || sndGetPlayingState((struct ALSoundState *) sp34->audioHandle) == 0)
            {
                if (lvlGetControlsLockedFlag() == 0)
                {
                    sndPlaySfx(
                        (struct ALBankAlt_s *) g_musicSfxBufferPtr,
                        MAGNETIC_HUM_SFX,
                        (struct ALSoundState *) &sp34->audioHandle);
                }
            }
        }
        else
        {
            g_CurrentPlayer->magnetattracttime = -1;

            if (sp34->audioHandle != NULL)
            {
                if (sndGetPlayingState((struct ALSoundState *) sp34->audioHandle) != 0)
                {
                    sndDeactivate((struct ALSoundState *) sp34->audioHandle);
                }
            }
        }
    }
}







void gunSetAimType(s32 param_1)
{
  g_CurrentPlayer->aimtype = param_1;
}


void sub_GAME_7F067AB4(coord3d *param_1)
{
  g_CurrentPlayer->hands[GUNRIGHT].field_A38 = sub_GAME_7F05DCB8(GUNRIGHT) + param_1->x;
  g_CurrentPlayer->hands[GUNRIGHT].field_A3C = param_1->y;
  g_CurrentPlayer->hands[GUNRIGHT].field_A40 = param_1->z;

  g_CurrentPlayer->hands[GUNLEFT].field_A38 = sub_GAME_7F05DCB8(GUNLEFT) + param_1->x;
  g_CurrentPlayer->hands[GUNLEFT].field_A3C = param_1->y;
  g_CurrentPlayer->hands[GUNLEFT].field_A40 = param_1->z;

}


void sub_GAME_7F067B4C(coord3d* pos)
{
    g_CurrentPlayer->hands[GUNLEFT].item_related.x = g_CurrentPlayer->hands[GUNRIGHT].item_related.x = pos->x;
    g_CurrentPlayer->hands[GUNLEFT].item_related.y = g_CurrentPlayer->hands[GUNRIGHT].item_related.y = pos->y;
    g_CurrentPlayer->hands[GUNLEFT].item_related.z = g_CurrentPlayer->hands[GUNRIGHT].item_related.z = pos->z;
}


void caclulate_gun_crosshair_position_rotation(f32 turn_x, f32 turn_y, f32 guncrossdamp, f32 gunaimdamp)
{
    s32 i;
    f32 screen_width;
    f32 screen_height;
    coord3d coords;

    screen_width = getPlayer_c_screenwidth();
    screen_height = getPlayer_c_screenheight();

    if (guncrossdamp != g_CurrentPlayer->guncrossdamp)
    {
        g_CurrentPlayer->crosshair_x_pos = (g_CurrentPlayer->crosshair_x_pos * (1.0f - g_CurrentPlayer->guncrossdamp)) / (1.0f - guncrossdamp);
        g_CurrentPlayer->crosshair_y_pos = (g_CurrentPlayer->crosshair_y_pos * (1.0f - g_CurrentPlayer->guncrossdamp)) / (1.0f - guncrossdamp);
        g_CurrentPlayer->guncrossdamp = guncrossdamp;
    }

    if (gunaimdamp != g_CurrentPlayer->gunaimdamp)
    {
        g_CurrentPlayer->gun_azimuth_angle = (g_CurrentPlayer->gun_azimuth_angle * (1.0f - g_CurrentPlayer->gunaimdamp)) / (1.0f - gunaimdamp);
        g_CurrentPlayer->gun_azimuth_turning = (g_CurrentPlayer->gun_azimuth_turning * (1.0f - g_CurrentPlayer->gunaimdamp)) / (1.0f - gunaimdamp);
        g_CurrentPlayer->gunaimdamp = gunaimdamp;
    }

    for (i = 0; i < g_ClockTimer; i++)
    {
        g_CurrentPlayer->crosshair_x_pos = (g_CurrentPlayer->crosshair_x_pos * guncrossdamp) + turn_x;
        g_CurrentPlayer->crosshair_y_pos = (g_CurrentPlayer->crosshair_y_pos * guncrossdamp) + turn_y;
    }

    g_CurrentPlayer->crosshair_angle.f[0] = (g_CurrentPlayer->crosshair_x_pos * (1.0f - guncrossdamp) * screen_width * 0.5f) + (screen_width * 0.5f);
    g_CurrentPlayer->crosshair_angle.f[1] = (g_CurrentPlayer->crosshair_y_pos * (1.0f - guncrossdamp) * screen_height * 0.5f) + (screen_height * 0.5f);

    if (g_CurrentPlayer->crosshair_angle.f[0] < 3.0f)
    {
        g_CurrentPlayer->crosshair_angle.f[0] = 3.0f;
    }
    else if ((screen_width - 4.0f) < g_CurrentPlayer->crosshair_angle.f[0])
    {
        g_CurrentPlayer->crosshair_angle.f[0] = screen_width - 4.0f;
    }

    if (g_CurrentPlayer->crosshair_angle.f[1] < 3.0f)
    {
        g_CurrentPlayer->crosshair_angle.f[1] = 3.0f;
    }
    else if ((screen_height - 4.0f) < g_CurrentPlayer->crosshair_angle.f[1])
    {
        g_CurrentPlayer->crosshair_angle.f[1] = (screen_height - 4.0f);
    }

    g_CurrentPlayer->crosshair_angle.f[0] += getPlayer_c_screenleft();
    g_CurrentPlayer->crosshair_angle.f[1] += getPlayer_c_screentop();

    for (i = 0; i < g_ClockTimer; i++)
    {
        g_CurrentPlayer->gun_azimuth_angle = (g_CurrentPlayer->gun_azimuth_angle * gunaimdamp) + turn_x;
        g_CurrentPlayer->gun_azimuth_turning = (g_CurrentPlayer->gun_azimuth_turning * gunaimdamp) + turn_y;
    }

    g_CurrentPlayer->field_FFC.x = (g_CurrentPlayer->gun_azimuth_angle * (1.0f - gunaimdamp) * screen_width * 0.5f) + (screen_width * 0.5f);
    g_CurrentPlayer->field_FFC.y = (g_CurrentPlayer->gun_azimuth_turning * (1.0f - gunaimdamp) * screen_height * 0.5f) + (screen_height * 0.5f);

    g_CurrentPlayer->field_FFC.x += getPlayer_c_screenleft();
    g_CurrentPlayer->field_FFC.y += getPlayer_c_screentop();

    transformAndNormalizeByLength2Dto3D(&g_CurrentPlayer->field_FFC, &coords, 1000.0f);
    sub_GAME_7F067AB4(&coords);
}



void sub_GAME_7F067F58(f32 turn_x, f32 turn_y, f32 max_aim_lock_speed)
{
    f32 aim_lock_speed;

#if defined(VERSION_US) || defined(VERSION_JP)
    aim_lock_speed = get_ptr_item_statistics(getCurrentPlayerWeaponId(GUNRIGHT))->AimLockSpeed;
#elif defined(VERSION_EU)
    aim_lock_speed = get_ptr_item_statistics(getCurrentPlayerWeaponId(GUNRIGHT))->CrosshairSpeed;
#endif

    if (aim_lock_speed < max_aim_lock_speed)
    {
        aim_lock_speed = max_aim_lock_speed;
    }

    caclulate_gun_crosshair_position_rotation(turn_x, turn_y, max_aim_lock_speed, aim_lock_speed);
}



void sub_GAME_7F067FBC(f32 turn_x, f32 turn_y)
{
    WeaponStats * item_stats;
    f32 guncrossdamp;
    f32 gunaimdamp;

    item_stats = get_ptr_item_statistics(getCurrentPlayerWeaponId(GUNRIGHT));

#if defined(VERSION_US)
    guncrossdamp = item_stats->CrosshairSpeed;
    gunaimdamp = item_stats->AimLockSpeed;
#elif defined(VERSION_EU)
    guncrossdamp = 0.7651f;
    gunaimdamp = item_stats->CrosshairSpeed;
#elif defined(VERSION_JP)
    guncrossdamp = 0.8f;
    gunaimdamp = item_stats->AimLockSpeed;
#endif

    caclulate_gun_crosshair_position_rotation(turn_x, turn_y, guncrossdamp, gunaimdamp);
}



/*
* Address: 0x7f068008
*/
void get_bullet_angle(f32* horizontal_angle, f32* vertical_angle) {
	*horizontal_angle = g_CurrentPlayer->crosshair_angle.f[0];
	*vertical_angle = g_CurrentPlayer->crosshair_angle.f[1];
}



void sub_GAME_7F06802C(void)
{
    coord3d coord;
    f32 tmp;

    tmp = getPlayer_c_screenleft() + (getPlayer_c_screenwidth() * 0.5f);
    g_CurrentPlayer->crosshair_angle.x = tmp;
    g_CurrentPlayer->field_FFC.x = tmp;

    tmp = getPlayer_c_screentop() + (getPlayer_c_screenheight() * 0.5f);
    g_CurrentPlayer->crosshair_angle.y = tmp;
    g_CurrentPlayer->field_FFC.y = tmp;

    transformAndNormalizeByLength2Dto3D((coord2d *) &g_CurrentPlayer->field_FFC, &coord, 1000.0f);
    sub_GAME_7F067AB4(&coord);
}



void sub_GAME_7F0680D4(coord3d * coord)
{
    coord3d tmp;

    g_CurrentPlayer->field_1010.x = coord->x;
    g_CurrentPlayer->field_1010.y = coord->y;
    g_CurrentPlayer->field_1010.z = coord->z;
    matrix_4x4_set_rotation_around_xyz(coord->f, &g_CurrentPlayer->field_101C);

    tmp.x = g_CurrentPlayer->field_101C.m[2][0] * 1000.0f;
    tmp.y = g_CurrentPlayer->field_101C.m[2][1] * 1000.0f;
    tmp.z = g_CurrentPlayer->field_101C.m[2][2] * 1000.0f;
    transform3Dto2DCoords(&tmp, (coord3d* ) &g_CurrentPlayer->crosshair_angle);

    g_CurrentPlayer->field_FFC.x = g_CurrentPlayer->crosshair_angle.x;
    g_CurrentPlayer->field_FFC.y = g_CurrentPlayer->crosshair_angle.y;

    sub_GAME_7F067AB4(&tmp);
}



/**
 * Address 0x7F068190.
*/
void sub_GAME_7F068190(coord3d *zeropos, coord3d *vec)
{
    zeropos->x = 0.0f;
    zeropos->y = 0.0f;
    zeropos->z = 0.0f;

    transformAndNormalizeByLength2Dto3D(&g_CurrentPlayer->crosshair_angle, vec, 1.0f);
}

/*
* Address: 0x7f0681cc
* This function computes the angle the player's bullets are fired at
*/
void bullet_path_from_screen_center(coord3d* arg0, coord3d* result, enum GUNHAND arg2)
{
    coord2d crosspos;
    s32 unused;
    f32 inaccuracy;
    f32 scaledspread;
    f32 randfactor;

    inaccuracy = get_ptr_item_statistics(getCurrentPlayerWeaponId(arg2))->Inaccuracy;
    if ((bondwalkItemCheckBitflags(get_item_in_hand_or_watch_menu(arg2), WEAPONSTATBITFLAG_FIRST_SHOT_ACCURACY) != 0) && (g_CurrentPlayer->hands[arg2].volley == 1))
    {
        // Single shots are four times more accurate
        inaccuracy *= 0.25f;
    }

    scaledspread = (120.0f * inaccuracy) / viGetFovY();

    randfactor = (RANDOMFRAC() - 0.5f) * RANDOMFRAC();
    crosspos.x = g_CurrentPlayer->crosshair_angle.f[0] + randfactor * scaledspread * getPlayer_c_screenwidth() * (PAL ? ASPECT_RATIO_PAL : ASPECT_RATIO_SD)
        / (getPlayer_c_perspaspect() * 320.0f);

    randfactor = (RANDOMFRAC() - 0.5f) * RANDOMFRAC();
    crosspos.y =  g_CurrentPlayer->crosshair_angle.f[1] + randfactor * scaledspread * getPlayer_c_screenheight()
        / (PAL ? (f32)(SCREEN_HEIGHT_272) : (f32)(SCREEN_HEIGHT_240));

    arg0->x = 0.0f;
    arg0->y = 0.0f;
    arg0->z = 0.0f;

    // Result is a normalized vector describing the path the bullet will follow
    // Can be used to compute x,y,z displacement off the center of the screen if done for a projectile
    transformAndNormalizeByLength2Dto3D(&crosspos, result, 1.0f);
}


/*
* Address: 0x7f068420
*/
CasingRecord* casingCreate(ModelFileHeader* header, Mtxf* mtx)
{
    CasingRecord* entry = g_Casings;
    CasingRecord* end = g_Casings + ARRAYCOUNT(g_Casings);

    while (entry < end && entry->header != NULL)
    {
        entry++;
    }

    if (entry < end)
    {
        entry->header = header;

        entry->pos.x = mtx->m[3][0];
        entry->pos.y = mtx->m[3][1];
        entry->pos.z = mtx->m[3][2];
#if VERSION_EU
        matrix_7f05842c_eu(mtx, entry->unk1C);
#else
        entry->unk1C.m[0][0] = mtx->m[0][0];
        entry->unk1C.m[0][1] = mtx->m[0][1];
        entry->unk1C.m[0][2] = mtx->m[0][2];
        entry->unk1C.m[0][3] = 0.0f;

        entry->unk1C.m[1][0] = mtx->m[1][0];
        entry->unk1C.m[1][1] = mtx->m[1][1];
        entry->unk1C.m[1][2] = mtx->m[1][2];
        entry->unk1C.m[1][3] = 0.0f;

        entry->unk1C.m[2][0] = mtx->m[2][0];
        entry->unk1C.m[2][1] = mtx->m[2][1];
        entry->unk1C.m[2][2] = mtx->m[2][2];
        entry->unk1C.m[2][3] = 0.0f;

        entry->unk1C.m[3][0] = 0.0f;
        entry->unk1C.m[3][1] = 0.0f;
        entry->unk1C.m[3][2] = 0.0f;
        entry->unk1C.m[3][3] = 1.0f;
#endif
        return entry;
    }

    return NULL;
}


#ifdef NONMATCHING
void sub_GAME_7F068508(void) {

}
#else
#ifndef VERSION_EU

void _asmpp_func178(void) {











































*(volatile float*)4 = -4.21406435081894e+19f;


switch (*(volatile int*)4) { case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: ; }








*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func179(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func180(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

 } void _asmpp_large_func181(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func182(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func183(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif

#ifdef VERSION_EU

void _asmpp_func184(void) {











































*(volatile float*)4 = -4.214064790623591e+19f;


switch (*(volatile int*)4) { case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: ; }








*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func185(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func186(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func187(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func188(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func189(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif


#endif


void update_bullet_casing(CasingRecord* casing)
{
    f32 new_val_y;
    f32 delta;
    s32 i;
    struct player* current_player;

    delta = g_GlobalTimerDelta;
    new_val_y = casing->vel.y - (delta * 0.2777778f);

    casing->pos.y += delta * 0.5f * (casing->vel.y + new_val_y);

    if (casing->pos.y < casing->floor_y_pos)
    {
#if defined(BUGFIX_R1)
        if (dword_CODE_bss_80075DB0 == 0 && (g_ClockTimer > 0))
#else
        if (dword_CODE_bss_80075DB0 == 0)
#endif
        {
            if ((g_CurrentPlayer->hands[0].when_detonating_mines_is_0 != 2) && (g_CurrentPlayer->hands[1].when_detonating_mines_is_0 != 2))
            {
                // Play bullet casing rolling of floor sound
                sndPlaySfx((struct ALBankAlt_s* ) g_musicSfxBufferPtr, 0x7A, (ALSoundState* ) &dword_CODE_bss_80075DB0);
            }
        }

        // This casing is removed and not updated anymore
        casing->header = NULL;
        return;
    }

    casing->vel.y = new_val_y;
    casing->pos.x += delta * casing->vel.x;
    casing->pos.z += delta * casing->vel.z;

    for (i = 0; i < g_ClockTimer; i++)
    {
#if defined(VERSION_US) || defined(VERSION_JP)
        matrix_4x4_multiply_homogeneous_in_place(&casing->unk5C, &casing->unk1C);
#else
        matrix_4x4_multiply_homogeneous_in_place_eu(casing->unk40, casing->unk1C);
#endif
    }
}


void update_bullet_casings(void)
{
    CasingRecord* end = g_Casings + ARRAYCOUNT(g_Casings);
    CasingRecord* entry = g_Casings;
    while (entry < end)
    {
        if (entry->header)
        {
            update_bullet_casing(entry);
        }
        entry++;
    }
}


#ifdef NONMATCHING
void sub_GAME_7F068EC4(void) {

}
#else

#if defined(VERSION_US) || defined(VERSION_JP)

void _asmpp_func190(void) {









*(volatile float*)4 = -4.214065230428242e+19f;


*(volatile float*)4 = -4.214065670232893e+19f;


*(volatile float*)4 = -4.214066110037544e+19f;


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func191(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif

#if defined(VERSION_EU)

void _asmpp_func192(void) {









*(volatile float*)4 = -4.214066549842195e+19f;


*(volatile float*)4 = -4.214066989646846e+19f;


*(volatile float*)4 = -4.2140674294514975e+19f;


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func193(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif
#endif


void sub_GAME_7F06908C(Gfx** arg0)
{
    CasingRecord* end = g_Casings + ARRAYCOUNT(g_Casings);
    CasingRecord* entry = g_Casings;
    while (entry < end)
    {
        if (entry->header)
        {
            sub_GAME_7F068EC4(entry, arg0);
        }
        entry++;
    }
}


void gunSetGunAmmoVisible(s32 reason, bool enable) {

	if (enable)
    {
		g_CurrentPlayer->gunammooff &= ~reason;
		return;
	}

	g_CurrentPlayer->gunammooff |= reason;
}



void give_cur_player_ammo(s32 ammo_type, s32 ammo_amount) {
    enum ITEM_IDS weapon_id;
    s32 max_ammo;

    weapon_id = getCurrentPlayerWeaponId(GUNRIGHT);
    if ((get_ammo_type_for_weapon(weapon_id) == ammo_type) && (bondwalkItemCheckBitflags(weapon_id, WEAPONSTATBITFLAG_AMMO_CLIP_LIMIT) != 0))
    {
        g_CurrentPlayer->hands[0].weapon_ammo_in_magazine += ammo_amount;
        if (get_ptr_item_statistics(weapon_id)->MagSize < g_CurrentPlayer->hands[0].weapon_ammo_in_magazine)
        {
            g_CurrentPlayer->hands[0].weapon_ammo_in_magazine = (s32) get_ptr_item_statistics(weapon_id)->MagSize;
        }
        g_CurrentPlayer->ammoheldarr[ammo_type] = 0;
        return;
    }

    max_ammo = ammo_related[ammo_type].MaxAmmo;
    if (max_ammo < ammo_amount)
    {
        g_CurrentPlayer->ammoheldarr[ammo_type] = max_ammo;
        return;
    }

    g_CurrentPlayer->ammoheldarr[ammo_type] = ammo_amount;
}




s32 check_cur_player_ammo_amount_in_inventory(AMMOTYPE ammotype) {
    return g_CurrentPlayer->ammoheldarr[ammotype];
}

s32 currentPlayerGetAmmoCount(AMMOTYPE ammotype) {

    s32 total_ammo = check_cur_player_ammo_amount_in_inventory(ammotype);

    if (get_ammo_type_for_weapon(getCurrentPlayerWeaponId(GUNRIGHT)) == ammotype) {
        total_ammo += get_ammo_in_hands_magazine(GUNRIGHT);
    }

    if (get_ammo_type_for_weapon(getCurrentPlayerWeaponId(GUNLEFT)) == ammotype) {
        total_ammo += get_ammo_in_hands_magazine(GUNLEFT);
    }

    return total_ammo;
}



s32 get_max_ammo_for_type(s32 arg0)
{
    return ammo_related[arg0].MaxAmmo;
}




void set_max_ammo_for_cur_player(void)
{
    s32 ammo_type;

    for (ammo_type = 0; ammo_type < AMMO_RELATED_MAX; ammo_type++)
    {
        give_cur_player_ammo(ammo_type, ammo_related[ammo_type].MaxAmmo);
    }
}



s32 get_ammo_in_hands_magazine(GUNHAND hand) {
    return g_CurrentPlayer->hands[hand].weapon_ammo_in_magazine;
}



s32 get_ammo_in_hands_weapon(enum GUNHAND hand)
{
    s32 weapon_id;
    s32 ammo_count;

    weapon_id = getCurrentPlayerWeaponId(hand);
    ammo_count = get_ammo_count_for_weapon(weapon_id);

    if ((weapon_id == ITEM_SHOTGUN) || (weapon_id == ITEM_AUTOSHOT))
    {
        s32 other_weapon_id;
        other_weapon_id = getCurrentPlayerWeaponId(1 - hand);

        if ((other_weapon_id == ITEM_SHOTGUN) || (other_weapon_id == ITEM_AUTOSHOT))
        {
            return ammo_count - g_CurrentPlayer->hands[1 - hand].field_8A4;
        }

        /* I don't know why there's an extra return here, but it's needed to match */
        return ammo_count;
    }

    return ammo_count;
}



s32 get_ammo_type_for_weapon(ITEM_IDS weapon) {
    return get_ptr_item_statistics(weapon)->AmmoType;
}

s32 get_ammo_count_for_weapon(ITEM_IDS weapon) {
  WeaponStats *weaponstats = get_ptr_item_statistics(weapon);
  return g_CurrentPlayer->ammoheldarr[weaponstats->AmmoType];
}

void add_ammo_to_weapon(ITEM_IDS weapon, s32 ammo) {
    give_cur_player_ammo(get_ptr_item_statistics(weapon)->AmmoType, ammo);
}

s32 get_max_ammo_for_weapon(enum ITEM_IDS weapon)
{
    return ammo_related[get_ptr_item_statistics(weapon)->AmmoType].MaxAmmo;
}



#ifdef NONMATCHING
void *microcode_generation_ammo_related(void *arg0, void *arg1, f32 arg2, f32 arg3, f32 arg4, s32 arg5, f32 arg6, s32 arg7, ?32 arg8, ?32 arg9, ?32 argA, ?32 argB) {
    f32 spA8;
    f32 spAC;
    f32 spB0;
    f32 spB4;
    f32 temp_f4;
    f32 temp_f4_2;
    f32 temp_f18;
    f32 temp_f10;
    f32 temp_f6;
    f32 temp_f18_2;
    f32 temp_f16;
    s32 phi_t9;
    f32 phi_f4;
    f32 phi_f18;
    s32 phi_t2;
    f32 phi_f18_2;
    f32 phi_f16;
    ? phi_a2;

    // Node 0
    arg0 = (void *) (arg0 + 8);
    arg0->unk4 = 0xc0;
    *arg0 = 0xba000602; //gDPSetColorDither(glistp++, G_CD_MAGICSQ);
    arg0 = (void *) (arg0 + 8);
    arg0->unk4 = 0;
    *arg0 = 0xba001301;//gDPSetTexturePersp(glistp++, G_TP_NONE);
    arg0 = (void *) (arg0 + 8);
    arg0->unk4 = 0;
    *arg0 = 0xb9000002;
    arg0 = (void *) (arg0 + 8);
    arg0->unk4 = 0;
    *arg0 = 0xba001001;//gDPSetTextureLOD(glistp++, G_TL_TILE);
    arg0 = (void *) (arg0 + 8);
    arg0->unk4 = 0;
    *arg0 = 0xba000c02;//gDPSetTextureFilter(glistp++, G_TF_POINT);
    arg0 = (void *) (arg0 + 8);
    arg0->unk4 = 0xc00;
    *arg0 = 0xba000903;//gDPSetTextureConvert(glistp++, G_TC_CONV);
    arg0 = (void *) (arg0 + 8);
    arg0->unk4 = 0;
    *arg0 = 0xba000e02;//gDPSetTextureLUT(glistp++, G_TT_NONE);
    phi_t9 = ((s32) arg1->unk4 >> 1);
    if (arg1->unk4 < 0)
    {
        // Node 1
        phi_t9 = ((s32) (arg1->unk4 + 1) >> 1);
    }
    // Node 2
    temp_f4 = (((f32) (u32) arg1->unk4 * 0.5f) - (f32) phi_t9);
    spB0 = temp_f4;
    if (arg5 != 0)
    {
        // Node 3
        spB0 = (f32) -temp_f4;
    }
    // Node 4
    spB0 = (f32) (spB0 + arg2);
    if (0.0f <= arg3)
    {
        // Node 5
        temp_f4_2 = (f32) arg1->unk5;
        phi_f4 = temp_f4_2;
        if (arg1->unk5 < 0)
        {
            // Node 6
            phi_f4 = (temp_f4_2 + M_U32_MAX_VALUE_F);
        }
        // Node 7
        spB4 = (f32) (arg3 - (phi_f4 * 0.5f));
    }
    else
    {
        // Node 8
        temp_f18 = (f32) arg1->unk5;
        phi_f18 = temp_f18;
        if (arg1->unk5 < 0)
        {
            // Node 9
            phi_f18 = (temp_f18 + M_U32_MAX_VALUE_F);
        }
        // Node 10
        phi_t2 = ((s32) arg1->unk5 >> 1);
        if (arg1->unk5 < 0)
        {
            // Node 11
            phi_t2 = ((s32) (arg1->unk5 + 1) >> 1);
        }
        // Node 12
        temp_f10 = ((phi_f18 * 0.5f) - (f32) phi_t2);
        temp_f6 = (arg4 - temp_f10);
        spB4 = (f32) -temp_f10;
        spB4 = temp_f6;
        spB4 = (f32) (temp_f6 + arg6);
    }
    // Node 13
    temp_f18_2 = (f32) arg1->unk4;
    phi_f18_2 = temp_f18_2;
    if (arg1->unk4 < 0)
    {
        // Node 14
        phi_f18_2 = (temp_f18_2 + M_U32_MAX_VALUE_F);
    }
    // Node 15
    spA8 = (f32) (phi_f18_2 * 0.5f);
    temp_f16 = (f32) arg1->unk5;
    phi_f16 = temp_f16;
    if (arg1->unk5 < 0)
    {
        // Node 16
        phi_f16 = (temp_f16 + M_U32_MAX_VALUE_F);
    }
    // Node 17
    arg0 = (void *) (arg0 + 8);
    spAC = (f32) (phi_f16 * 0.5f);
    arg0->unk4 = 0;
    *arg0 = 0xe7000000;//gDPPipeSync(glistp++);
    arg0 = (void *) (arg0 + 8);
    arg0->unk4 = 0;
    *arg0 = 0xba001402;//gDPSetCyceType(glistp++, G_CYC_1CYCLE);
    arg0 = (void *) (arg0 + 8);
    arg0->unk4 = 0x504240;
    *arg0 = 0xb900031d;//gDPSetRenderMode(glistp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    arg0 = (void *) (arg0 + 8);
    arg0->unk4 = 0xfffdf6fb;
    *arg0 = 0xfcffffff;//gDPSetCombineMode(glistp++, PRIMITIVE, PRIMITIVE2);
    arg0 = (void *) (arg0 + 8);
    arg0->unk4 = 0;
    *arg0 = 0xfa000000;
    arg0 = (void *) (arg0 + 8);
    *arg0 = (s32) ((((((s32) (spAC + spB4) + 1) & 0x3ff) * 4) | 0xf6000000) | ((((s32) (spB0 + spA8) + 1) & 0x3ff) << 0xe));
    arg0->unk4 = (s32) (((((s32) (spB4 - spAC) + -1) & 0x3ff) * 4) | ((((s32) (spB0 - spA8) + -1) & 0x3ff) << 0xe));
    phi_a2 = 1;
    if (arg7 != 0)
    {
        // Node 18
        phi_a2 = 2;
    }
    // Node 19
    texSelect(arg3, arg2, &arg0, arg1, phi_a2, 0, 0);
    display_image_at_position(&arg0, &spB0, &spA8, arg1->unk4, (s32) arg1->unk5, 0, 0, 1, arg8, arg9, argA, argB, (s32) (0 < arg1->unk6), 0);
    arg0 = (void *) (arg0 + 8);
    arg0->unk4 = 0;
    *arg0 = 0xe7000000;//gDPPipeSync(glistp++);
    arg0 = (void *) (arg0 + 8);
    arg0->unk4 = 0x40;
    *arg0 = 0xba000602;//gDPSetColorDither(glistp++, G_CD_BAYER);
    arg0 = (void *) (arg0 + 8);
    arg0->unk4 = 0x80000;
    *arg0 = 0xba001301;//gDPSetTexturePersp(glistp++, G_TP_PERSP);
    arg0 = (void *) (arg0 + 8);
    arg0->unk4 = 0;
    *arg0 = 0xb9000002;
    arg0 = (void *) (arg0 + 8);
    arg0->unk4 = 0x10000;
    *arg0 = 0xba001001;//gDPSetTextureLOD(glistp++, G_TL_LOD);
    arg0 = (void *) (arg0 + 8);
    arg0->unk4 = 0x2000;
    *arg0 = 0xba000c02;//gDPSetTextureFilter(glistp++, G_TF_BILERP);
    arg0 = (void *) (arg0 + 8);
    arg0->unk4 = 0xc00;
    *arg0 = 0xba000903;//gDPSetTextureConvert(glistp++, G_TC_FILT);
    arg0 = (void *) (arg0 + 8);
    arg0->unk4 = 0;
    *arg0 = 0xba000e02;
    return arg0;
}

#else

void _asmpp_func194(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func195(void) { 
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func196(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func197(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif





#ifdef NONMATCHING
void set_rgba_redirect_generate_microcode(? arg2, ? arg3, f32 arg4, ?32 arg5, f32 arg6, ?32 arg7) {
    // Node 0
    return microcode_generation_ammo_related(arg2, arg3, arg2, arg3, arg4, arg5, arg6, arg7, 0xff, 0xff, 0xff, 0xff);
}
#else

void _asmpp_func198(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif


Gfx *gunDrawHudString(Gfx *gdl, s8 *text, s32 x, s32 halign, s32 y, s32 valign, bool glow)
{
    s32 x1;
    s32 y1;
    s32 x2;
    s32 y2;
    s32 textheight;
    s32 textwidth;

    x1 = 0;
    y1 = 0;
    x2 = 0;
    y2 = 0;
    textwidth = 0;
    textheight = 0;

    textMeasure(&textheight, &textwidth, text, ptrFontBankGothicChars, ptrFontBankGothic, 0);

    if (halign == HUDHALIGN_LEFT) { // left
		x2 = x + textwidth;
		x1 = x;
	} else if (halign == HUDHALIGN_RIGHT) { // right
		x1 = x - textwidth;
		x2 = x;
	} else if (halign == HUDHALIGN_MIDDLE) { // middle
		x2 = x + textwidth / 2;
		x1 = x2 - textwidth;
	}

    if (valign == HUDVALIGN_TOP) { // top
		y2 = y + textheight;
		y1 = y;
	} else if (valign == HUDVALIGN_BOTTOM) { // bottom
		y1 = y - textheight;
		y2 = y;
	} else if (valign == HUDVALIGN_MIDDLE) { // middle
		y2 = y + textheight / 2;
		y1 = y2 - textheight;
	}

    gdl = draw_blackbox_to_screen(gdl, &x1, &y1, &x2, &y2);

    if (glow) {
        gdl = textRenderGlow(gdl, &x1, &y1, text, ptrFontBankGothicChars, ptrFontBankGothic, -1, 0x646464FF, (s32) viGetX(), viGetY(), 0, 0);
    } else {
        gdl = textRender(gdl, &x1, &y1, text, ptrFontBankGothicChars, ptrFontBankGothic, 0xFF00B0, (s32) viGetX(), viGetY(), 0, 0);
    }

    return gdl;
}


Gfx *gunDrawHudInteger(Gfx *gdl, s32 value, s32 x, s32 halign, s32 y, s32 valign, bool glow)
{
    char buffer[12];
    sprintf(buffer, "%d\n", value);
    return gunDrawHudString(gdl, buffer, x, halign, y, valign, glow);
}

#ifdef NONMATCHING
s32 generate_ammo_total_microcode(s32 arg0) {
    void *sp28;
    s16 sp2C;
    s16 sp2E;
    f32 sp30;
    f32 sp34;
    s16 sp38;
    s32 sp3C;
    void *sp40;
    s32 sp44;
    void *sp48;
    s32 sp4C;
    s32 sp50;
    s32 sp54;
    s32 sp58;
    s32 sp5C;
    s32 sp60;
    s32 sp64;
    ? temp_ret;
    void *temp_v1;
    s32 temp_v1_3;
    s32 temp_t3;
    ? temp_ret_2;
    void *temp_v1_2;
    s32 temp_v1_4;
    s32 temp_t1;
    s32 phi_v1;
    s32 phi_t9;
    s32 phi_t0;
    s32 phi_v1_2;
    s32 phi_t3;
    s32 phi_t0_2;

    // Node 0
    if (g_CurrentPlayer->unk1064 == 0)
    {
        // Node 1
        if (g_CurrentPlayer->unk29C4 == 0)
        {
            // Node 2
            sp64 = getCurrentPlayerWeaponId(1);
            sp60 = getCurrentPlayerWeaponId(0);
            if (getPlayerCount() < 3)
            {
                // Node 3
                sp58 = 0x3b;
                sp54 = 0x3b;
            }
            else
            {
                // Node 4
                if ((get_cur_playernum() & 1) != 0)
                {
                    // Node 5
                    sp58 = 0x2b;
                    sp54 = 0x7f;
                }
                else
                {
                    // Node 6
                    sp58 = 0x3b;
                    sp54 = 0x6d;
                }
            }
            // Node 7
            if (sp60 != 0)
            {
                // Node 8
                temp_ret = get_ammo_type_for_weapon(sp60);
                sp5C = temp_ret;
                if (temp_ret != 0)
                {
                    // Node 9
                    if (g_CurrentPlayer->unk894 != 7)
                    {
                        // Node 10
                        if (g_CurrentPlayer->unk894 != 7)
                        {
                            // Node 11
                            if (bondwalkItemCheckBitflags(sp60, 0x80000) == 0)
                            {
                                // Node 12
                                temp_v1 = ((sp5C * 0xc) + &ammo_related);
                                sp44 = 5;
                                if (temp_v1->unk4 != 0)
                                {
                                    // Node 13
                                    sp28 = temp_v1;
                                    sp48 = (void *) (temp_v1->unk4 + globalbank_rdram_offset);
                                    sp30 = getPlayer_c_screenleft();
                                    sp34 = getPlayer_c_screenwidth();
                                    sp38 = viGetViewTop();
                                    arg0 = set_rgba_redirect_generate_microcode(arg0, sp48, ((sp34 + sp30) - (f32) sp54), 0xbf800000, (f32) ((viGetViewHeight() + sp38) + -0x14), 0, (f32) sp28->unk8, 1);
                                    sp44 = (s32) sp48->unk4;
                                }
                                // Node 14
                                arg0 = microcode_constructor(arg0);
                                if (bondwalkItemCheckBitflags(sp60, 0x400000) != 0)
                                {
                                    // Node 15
                                    sp4C = 0;
                                    temp_v1_3 = (g_CurrentPlayer->unk89C + (g_CurrentPlayer + (sp5C * 4))->unk1130);
                                    phi_v1 = temp_v1_3;
                                    if (sp64 == sp60)
                                    {
                                        // Node 16
                                        phi_v1 = (temp_v1_3 + g_CurrentPlayer->unkC44);
                                    }
                                    // Node 17
                                    sp50 = (s32) phi_v1;
                                }
                                else
                                {
                                    // Node 18
                                    sp4C = (s32) g_CurrentPlayer->unk89C;
                                    sp50 = (s32) (g_CurrentPlayer + (sp5C * 4))->unk1130;
                                }
                                // Node 19
                                if (bondwalkItemCheckBitflags(sp60, 0x400000) == 0)
                                {
                                    // Node 20
                                    sp2C = viGetViewLeft();
                                    sp2E = viGetViewWidth();
                                    sp38 = viGetViewTop();
                                    viGetViewHeight();
                                    phi_t9 = (sp44 >> 1);
                                    if (sp44 < 0)
                                    {
                                        // Node 21
                                        phi_t9 = ((s32) (sp44 + 1) >> 1);
                                    }
                                    // Node 22
                                    arg0 = gunDrawHudInteger(arg0, sp4C, ((((sp2E + sp2C) - sp54) - phi_t9) + -4), 0);
                                }
                                // Node 23
                                if ((sp50 > 0) || (bondwalkItemCheckBitflags(sp60, 0x400000) != 0))
                                {
                                    // Node 25
                                    sp2C = viGetViewLeft();
                                    sp2E = viGetViewWidth();
                                    sp38 = viGetViewTop();
                                    viGetViewHeight();
                                    temp_t3 = (sp44 + 1);
                                    phi_t0 = (temp_t3 >> 1);
                                    if (temp_t3 < 0)
                                    {
                                        // Node 26
                                        phi_t0 = ((s32) (temp_t3 + 1) >> 1);
                                    }
                                    // Node 27
                                    arg0 = gunDrawHudInteger(arg0, sp50, ((((sp2E + sp2C) - sp54) + phi_t0) + 3), 1);
                                }
                                else
                                {

                                }
                                // Node 28
                                arg0 = combiner_bayer_lod_perspective(arg0);
                            }
                        }
                    }
                }
            }
            // Node 29
            if (sp64 != 0)
            {
                // Node 30
                temp_ret_2 = get_ammo_type_for_weapon(sp64);
                sp5C = temp_ret_2;
                if (temp_ret_2 != 0)
                {
                    // Node 31
                    if (g_CurrentPlayer->unkC3C != 7)
                    {
                        // Node 32
                        if (g_CurrentPlayer->unkC3C != 7)
                        {
                            // Node 33
                            if (bondwalkItemCheckBitflags(sp64, 0x80000) == 0)
                            {
                                // Node 34
                                temp_v1_2 = ((sp5C * 0xc) + &ammo_related);
                                sp3C = 5;
                                if (temp_v1_2->unk4 != 0)
                                {
                                    // Node 35
                                    sp28 = temp_v1_2;
                                    sp40 = (void *) (temp_v1_2->unk4 + globalbank_rdram_offset);
                                    sp34 = getPlayer_c_screenleft();
                                    sp38 = viGetViewTop();
                                    viGetViewHeight();
                                    arg0 = set_rgba_redirect_generate_microcode(sp28->unk8, arg0, sp40, (sp34 + (f32) sp58), 0xbf800000, 1, 1);
                                    sp3C = (s32) sp40->unk4;
                                }
                                // Node 36
                                arg0 = microcode_constructor(arg0);
                                if (bondwalkItemCheckBitflags(sp64, 0x400000) != 0)
                                {
                                    // Node 37
                                    sp4C = 0;
                                    temp_v1_4 = (g_CurrentPlayer->unkC44 + (g_CurrentPlayer + (sp5C * 4))->unk1130);
                                    phi_v1_2 = temp_v1_4;
                                    if (sp64 == sp60)
                                    {
                                        // Node 38
                                        phi_v1_2 = (temp_v1_4 + g_CurrentPlayer->unk89C);
                                    }
                                    // Node 39
                                    sp50 = (s32) phi_v1_2;
                                }
                                else
                                {
                                    // Node 40
                                    sp4C = (s32) g_CurrentPlayer->unkC44;
                                    sp50 = (s32) (g_CurrentPlayer + (sp5C * 4))->unk1130;
                                }
                                // Node 41
                                if (bondwalkItemCheckBitflags(sp64, 0x400000) == 0)
                                {
                                    // Node 42
                                    sp2E = viGetViewLeft();
                                    sp38 = viGetViewTop();
                                    viGetViewHeight();
                                    phi_t3 = (sp3C >> 1);
                                    if (sp3C < 0)
                                    {
                                        // Node 43
                                        phi_t3 = ((s32) (sp3C + 1) >> 1);
                                    }
                                    // Node 44
                                    arg0 = gunDrawHudInteger(arg0, sp4C, (((sp2E + sp58) + phi_t3) + 3), 1);
                                }
                                // Node 45
                                if ((sp50 > 0) || (bondwalkItemCheckBitflags(sp64, 0x400000) != 0))
                                {
                                    // Node 47
                                    sp2E = viGetViewLeft();
                                    sp38 = viGetViewTop();
                                    viGetViewHeight();
                                    temp_t1 = (sp3C + 1);
                                    phi_t0_2 = (temp_t1 >> 1);
                                    if (temp_t1 < 0)
                                    {
                                        // Node 48
                                        phi_t0_2 = ((s32) (temp_t1 + 1) >> 1);
                                    }
                                    // Node 49
                                    arg0 = gunDrawHudInteger(arg0, sp50, (((sp2E + sp58) - phi_t0_2) + -4), 0);
                                }
                                else
                                {

                                }
                                // Node 50
                                arg0 = combiner_bayer_lod_perspective(arg0);
                            }
                        }
                    }
                }
            }
        }
    }
    // Node 51
    return arg0;
}
#else

#if defined(VERSION_US) || defined(VERSION_JP)

void _asmpp_func199(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func200(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func201(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func202(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif

#if defined(VERSION_EU)

void _asmpp_func203(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func204(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func205(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func206(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif
#endif





#ifdef NONMATCHING
s32 sub_GAME_7F06A334(s32 arg0) {
    void *sp30;
    s16 sp34;
    s32 sp3C;
    void *sp40;
    s32 sp44;
    s32 sp48;
    s32 sp4C;
    s32 sp50;
    s32 sp54;
    ? temp_ret;
    ? temp_ret_2;
    void *temp_v0;
    s32 temp_s0;
    s32 temp_v1;
    s32 temp_a2;
    s32 phi_s0;
    s32 phi_v1;
    s32 phi_t4;
    s32 phi_s0_2;
    s32 phi_t9;
    s32 phi_s0_3;
    s32 phi_s0_4;

    // Node 0
    sp54 = getCurrentPlayerWeaponId(1);
    temp_ret = getCurrentPlayerWeaponId(0);
    phi_s0_4 = arg0;
    if (temp_ret != 0)
    {
        // Node 1
        sp50 = temp_ret;
        temp_ret_2 = get_ammo_type_for_weapon(temp_ret);
        sp4C = temp_ret_2;
        phi_s0_4 = arg0;
        if (temp_ret_2 != 0)
        {
            // Node 2
            phi_s0_4 = arg0;
            if (g_CurrentPlayer->unk894 != 7)
            {
                // Node 3
                phi_s0_4 = arg0;
                if (g_CurrentPlayer->unk894 != 7)
                {
                    // Node 4
                    phi_s0_4 = arg0;
                    if (bondwalkItemCheckBitflags(sp50, 0x80000) == 0)
                    {
                        // Node 5
                        temp_v0 = ((sp4C * 0xc) + &ammo_related);
                        sp3C = 5;
                        sp30 = temp_v0;
                        sp40 = (void *) temp_v0->unk4;
                        get_ptr_item_statistics(sp50);
                        phi_s0 = arg0;
                        if (sp40 != 0)
                        {
                            // Node 6
                            sp40 = (void *) (sp40 + globalbank_rdram_offset);
                            sp34 = viGetViewTop();
                            sp3C = (s32) sp40->unk4;
                            phi_s0 = set_rgba_redirect_generate_microcode(arg0, sp40, 0x43480000, 0x43340000, (f32) ((viGetViewHeight() + sp34) + -0x14), 0, (f32) sp30->unk8, 1);
                        }
                        // Node 7
                        temp_s0 = microcode_constructor(phi_s0);
                        if (bondwalkItemCheckBitflags(sp50, 0x400000) != 0)
                        {
                            // Node 8
                            sp44 = 0;
                            temp_v1 = (g_CurrentPlayer->unk89C + (g_CurrentPlayer + (sp4C * 4))->unk1130);
                            phi_v1 = temp_v1;
                            if (sp54 == sp50)
                            {
                                // Node 9
                                phi_v1 = (temp_v1 + g_CurrentPlayer->unkC44);
                            }
                            // Node 10
                            sp48 = (s32) phi_v1;
                        }
                        else
                        {
                            // Node 11
                            sp44 = (s32) g_CurrentPlayer->unk89C;
                            sp48 = (s32) (g_CurrentPlayer + (sp4C * 4))->unk1130;
                        }
                        // Node 12
                        phi_s0_2 = temp_s0;
                        if (bondwalkItemCheckBitflags(sp50, 0x400000) == 0)
                        {
                            // Node 13
                            phi_t4 = (sp3C >> 1);
                            if (sp3C < 0)
                            {
                                // Node 14
                                phi_t4 = ((s32) (sp3C + 1) >> 1);
                            }
                            // Node 15
                            phi_s0_2 = gunDrawHudInteger(temp_s0, sp44, (0xc4 - phi_t4), 0, 0);
                        }
                        // Node 16
                        if ((sp48 > 0) || (bondwalkItemCheckBitflags(sp50, 0x400000) != 0))
                        {
                            // Node 18
                            temp_a2 = (sp3C + 1);
                            phi_t9 = (temp_a2 >> 1);
                            if (temp_a2 < 0)
                            {
                                // Node 19
                                phi_t9 = ((s32) (temp_a2 + 1) >> 1);
                            }
                            // Node 20
                            phi_s0_3 = gunDrawHudInteger(phi_s0_2, sp48, (phi_t9 + 0xcb), 1, 0);
                        }
                        else
                        {

                        }
                        // Node 21
                        phi_s0_4 = combiner_bayer_lod_perspective(phi_s0_3);
                    }
                }
            }
        }
    }
    // Node 22
    return phi_s0_4;
}
#else

#if defined(VERSION_US) || defined(VERSION_JP)

void _asmpp_func207(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func208(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif

#if defined(VERSION_EU)

void _asmpp_func209(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func210(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
}
#endif
#endif




void gunSetSightVisible(s32 reason, bool visible)
{
    if (visible)
    {
        g_CurrentPlayer->gunsightmode &= ~reason;
        return;
    }

    g_CurrentPlayer->gunsightmode |= reason;
}


void gunDrawSight(s32 *gdl) {

    s32 sp54;
    f32 xypos[2];
    f32 halfedxy[2];

    if ((g_CurrentPlayer->gunsightmode == 0) && (g_CurrentPlayer->mpmenuon == FALSE)) {
        sp54 = *gdl;
        texSelect(&sp54, crosshairimage, 4, 0, 0);

        xypos[0] = g_CurrentPlayer->crosshair_angle.f[0];
        xypos[1] = g_CurrentPlayer->crosshair_angle.f[1];
        halfedxy[0] = 16.0f;
        halfedxy[1] = 16.0f;

        if (get_screen_ratio() == SCREEN_RATIO_16_9) {
            halfedxy[0] = halfedxy[0] * 0.75f;
        }
#ifdef VERSION_EU
        halfedxy[1] = halfedxy[1] * 1.19047617912f;
#endif
        display_image_at_position(&sp54, &xypos, &halfedxy, 0x20, 0x20, 0, 0, 1, 0xFF, 0xFF, 0xFF, 0x6E, (crosshairimage->level > 0), 0);
        *gdl = sp54;
    }
}



void inc_curplayer_hitcount_with_weapon(ITEM_IDS item, SHOT_REGISTER shot_register) {

    if (bondwalkItemCheckBitflags(item, WEAPONSTATBITFLAG_PLAYER_STAT_HIT)) {
        g_playerPerm->shot_count[shot_register] = g_playerPerm->shot_count[shot_register]+1;
    }
}

s32 get_curplayer_shot_register(SHOT_REGISTER shot_register)
{
  return g_playerPerm->shot_count[shot_register];
}

void inc_cur_civilian_casualties(void)
{
    g_playerPerm->killed_civilians++;
}

s32 get_civilian_casualties(void)
{
    return g_playerPerm->killed_civilians;
}


//D:80053BF8
const char aSD[] = "%s: %d\n";

void increment_num_kills_display_text_in_MP(void)
{
    s8 buffer[256];
    s32 time_since_kill;
    s32 recent_kill_count;
    s32 mission_time;
    s32 unused; // needed this variable to match

    g_playerPerm->kill_count += 1;
    g_CurrentPlayer->field_29F8 += 1;

    if (getPlayerCount() < 2) { return; }

    mission_time = getMissiontimer();
    sprintf(&buffer, aSD, langGet(getStringID(LGUN, GUN_STR_DA_KILLCOUNT)), g_playerPerm->kill_count); // "kill count"

#if defined(VERSION_US)
    hudmsgBottomShow(&buffer);
#elif defined(VERSION_JP) || defined(VERSION_EU)
    jp_hudmsgBottomShow(&buffer);
#endif

    if (g_playerPerm->kill_count >= 2)
    {
        time_since_kill = mission_time - g_CurrentPlayer->last_kill_time[0];
        if (g_playerPerm->max_time_between_kills < time_since_kill)
        {
            g_playerPerm->max_time_between_kills = time_since_kill;
        }

        if (time_since_kill < g_playerPerm->min_time_between_kills)
        {
            g_playerPerm->min_time_between_kills = time_since_kill;
        }
    }

    recent_kill_count = 1;
    g_CurrentPlayer->last_kill_time[3] = g_CurrentPlayer->last_kill_time[2];
    g_CurrentPlayer->last_kill_time[2] = g_CurrentPlayer->last_kill_time[1];
    g_CurrentPlayer->last_kill_time[1] = g_CurrentPlayer->last_kill_time[0];
    g_CurrentPlayer->last_kill_time[0] = mission_time;

    // I tried to turn this into a loop but it didn't match
    if (g_CurrentPlayer->last_kill_time[1] != -1 && (g_CurrentPlayer->last_kill_time[0] - g_CurrentPlayer->last_kill_time[1]) < 0x78)
    {
        recent_kill_count++;
        if ((g_CurrentPlayer->last_kill_time[2] != -1) && ((g_CurrentPlayer->last_kill_time[0] - g_CurrentPlayer->last_kill_time[2]) < 0x78))
        {
            recent_kill_count++;
            if ((g_CurrentPlayer->last_kill_time[3] != -1) && ((g_CurrentPlayer->last_kill_time[0] - g_CurrentPlayer->last_kill_time[3]) < 0x78))
            {
                recent_kill_count++;
            }
        }
    }

    if (g_playerPerm->most_killed_one_time < recent_kill_count)
    {
        g_playerPerm->most_killed_one_time = recent_kill_count;
    }
}



s32 get_curplay_killcount(void) {
    return g_playerPerm->kill_count;
}

void increment_num_times_killed_MwtGC(void){
    g_playerPerm->killed_gg_owner_count++;
}

s32 get_times_killed_mwtgx(void) {
    return g_playerPerm->killed_gg_owner_count;
}


void increment_num_deaths(void)
{
	char buffer[256];
    g_CurrentPlayer->deathcount = (s32) (g_CurrentPlayer->deathcount + 1);
    if (getPlayerCount() >= 2)
    {
        if (g_CurrentPlayer->deathcount == 1)
        {
            sprintf(buffer, langGet(getStringID(LGUN, GUN_STR_DB_DIEDONCE_LF))); //died once
        }
        else
        {
            sprintf(buffer, "%s %d %s\n", langGet(getStringID(LGUN, GUN_STR_DC_DIED)), g_CurrentPlayer->deathcount, langGet(getStringID(LGUN, GUN_STR_DD_TIMES))); //died times
        }
#if defined(VERSION_JP) || defined(VERSION_EU)
		jp_hudmsgBottomShow(buffer);
#else
		hudmsgBottomShow(buffer);
#endif
    }
}


s32 get_curplayer_numdeaths(void) {
    return g_CurrentPlayer->deathcount;
}

//D:80053C0C
const char aSD_0[] = "%s: %d\n";

void increment_num_suicides_display_MP(void) {
    char buffer[256];
    s32 time_diff;
    s32 recent_kill_count;
    s32 currentTime;

    g_CurrentPlayer->num_suicides += 1;
    if (getPlayerCount() >= 2) {

        currentTime = getMissiontimer();

        sprintf(&buffer, &aSD_0, langGet(getStringID(LGUN, GUN_STR_DE_SUICIDECOUNT)), g_CurrentPlayer->num_suicides); // "suicide count"

#if defined(VERSION_JP) || defined(VERSION_EU)
		jp_hudmsgBottomShow(&buffer);
#else
		hudmsgBottomShow(&buffer);
#endif

        if (g_playerPerm->kill_count >= 2) {
            time_diff = currentTime - g_CurrentPlayer->last_kill_time[0];
            if (g_playerPerm->max_time_between_kills < time_diff) {
                g_playerPerm->max_time_between_kills = time_diff;
            }
            if (time_diff < g_playerPerm->min_time_between_kills) {
                g_playerPerm->min_time_between_kills = time_diff;
            }
        }
        recent_kill_count = 1;
        g_CurrentPlayer->last_kill_time[3] = g_CurrentPlayer->last_kill_time[2];
        g_CurrentPlayer->last_kill_time[2] = g_CurrentPlayer->last_kill_time[1];
        g_CurrentPlayer->last_kill_time[1] = g_CurrentPlayer->last_kill_time[0];
        g_CurrentPlayer->last_kill_time[0] = currentTime;

        if ( g_CurrentPlayer->last_kill_time[1] != -1) {

            if ((g_CurrentPlayer->last_kill_time[0] - g_CurrentPlayer->last_kill_time[1]) < 0x78) {

                recent_kill_count += 1;

                if ((g_CurrentPlayer->last_kill_time[2] != -1) && ((g_CurrentPlayer->last_kill_time[0] - g_CurrentPlayer->last_kill_time[2]) < 0x78)) {

                    recent_kill_count += 1;

                    if ((g_CurrentPlayer->last_kill_time[3] != -1) && ((g_CurrentPlayer->last_kill_time[0] - g_CurrentPlayer->last_kill_time[3]) < 0x78)) {
                        recent_kill_count += 1;
                    }
                }
            }
        }

        if (g_playerPerm->most_killed_one_time < recent_kill_count) {
            g_playerPerm->most_killed_one_time = recent_kill_count;
        }
    }
}

s32 get_curplayer_numsuicides(void) {
    return g_CurrentPlayer->num_suicides;
}
