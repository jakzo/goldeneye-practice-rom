/*---------------------------------------------------------------------

	File		propobj.c

	Comments	Prop Objects code.

  ---------------------------------------------------------------------*/

#include <ultra64.h>
#include <math.h>
#include <PR/libaudio.h>
#include <assets/GlobalImageTable.h>
#include <bondgame.h>
#include <boss.h>
#include <snd.h>
#include <music.h>
#include <memp.h>
#include <snd.h>
#include <gbi_extension.h>
#include "bg.h"
#include "bondview.h"
#include "bondinv.h"
#include "chr.h"
#include "chrai.h"
#include "chrlv.h"
#include "chrobjdata.h"
#include "chrobjhandler.h"
#include "explosions.h"
#include "fog.h"
#include "lvl.h"
#include "lvl_text.h"
#include "math_floor.h"
#include "math_asinfacosf.h"
#include "math_atan2f.h"
#include "matrixmath.h"
#include "objecthandler.h"
#include "player.h"
#include "quaternion.h"
#include <limits.h>
#include "stan.h"
#include "assets/obseg/text/LpropobjE.h"
#include "explosions.h"
#include "image_bank.h"
#include "initanitable.h"
#include "random.h"
#include "unk_0B3200.h"
#include "textrelated.h"
#include "gun.h"
#include "fr.h"
#include "objective_status.h"

extern Model *g_CurrentProjectileModel;
extern ModelNode *dword_CODE_bss_80075B74;

void mtxLoadRandomRotation(Mtxf *mtx);
void sub_GAME_7F057DF8(Mtxf *matrix, Mtxf *transform, s32 count);
s32 sub_GAME_7F053894(coord3d *position, f32 low, f32 high);
void sub_GAME_7F053A3C(DoorRecord *door);
void doorSetOpenState(DoorRecord *door, s32 newstate);
bool doorIsClosed(DoorRecord *door);


#ifdef VERSION_EU

#define CHROBJ_TIMETOREGEN   50
#define CHROBJ_TIMETOREGEN_F 50.0f
#define CHROBJ_GAS_TIMER 0x5dc /* 1500 */

#define PLASTIQUE_EXPLOSION_DELAY_TICKS 100

#else

#define CHROBJ_TIMETOREGEN   60
#define CHROBJ_TIMETOREGEN_F 60.0f
#define CHROBJ_GAS_TIMER 0x708 /* 1800 */
#define PLASTIQUE_EXPLOSION_DELAY_TICKS 120

#endif



#define PROXIMITY_MINE_TRIGGER_DISTANCE 62500.0f




/* 0x80030AC8 */ s32 alarm_timer = 0;
/* 0x80030ACC */ s32 *ptr_alarm_sfx = 0;
/* 0x80030AD0 */ f32 toxic_gas_sound_timer = 0.0;
/* 0x80030AD4 */ s32 activate_gas_sound_timer = FALSE;
/* 0x80030AD8 */ coord3d D_80030AD0 = { 0.0f, 0.0f, 0.0f };
/* 0x80030ADC */ s32 D_80030ADC = 0;
/* 0x80030AE0 */ f32 D_80030AE0 = 0.0f;
/* 0x80030AE4 */ ALSoundState *ptr_gas_sound = NULL;
/* 0x80030AE8 */ s32 clock_drawn_flag = 1;
/* 0x80030AEC */ s32 clock_enable = 0;
/* 0x80030AF0 */ f32 clock_time = 0;
/* 0x80030AF4 */ s32 g_RemoteMineOwnerTriggerFlag = 0;
/* 0x80030AF8 */ s32 g_NextWeaponSlot = 0; // numbers between 0 and 30
/* 0x80030AFC */ s32 g_NextHatSlot = 0;
/* 0x80030B00 */ ObjectRecord *g_LevelLoadPropSwitch = NULL;
/* 0x80030B04 */ LockDoorRecord *g_LevelLoadPropLockDoor = NULL;
/* 0x80030B08 */ ObjectRecord *g_LevelLoadPropSafeItem = NULL;
/* 0x80030B0C */ struct PropRecord * D_80030B0C = NULL;
/* 0x80030B10 */ s32 bodypartshot = 0xFFFFFFFF;
/* 0x80030B14 */ f32 F_80030B14 = 1.0;
/* 0x80030B18 */ f32 F_80030B18 = 1.0;
/* 0x80030B1C */ f32 g_AutogunPendingDamageTick = 1.0;
/* 0x80030B20 */ f32 g_AutogunDamageScalar = 1.0;
/* 0x80030B24 */ f32 F_80030B24 = 1.0;

/*
* Set on level load.
*/
f32 g_SoloAmmoMultiplier = 1.0;

s16 Throwing_knife_SFX[] = {0x5F, 0x60, 0x61};

ModelRenderData D_80030B34 = {NULL,
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




//[80030B74	00	Bond]
u32 monAnim00Bond[] = {
    MONUSEIMAGE(IMGBOND),
    MONHORZSCROLL(0x400, 20),
    MONHOLDTIME(20),
    MONVERTSCROLL(0x400, 20),
    MONRGBA(COLOR_BLACK, 20),
    MONHOLDTIME(20),
    MONZOOMSQUARE(0x200, 20),
    MONRGBA(COLOR_WHITE, 20),
    MONHOLDTIME(20),
    MONZOOMSQUARE(0x400, 20),
    MONHOLDTIME(20),
    MONLOOP()
};

//[80030C00	01	Desktops, Satellite]
u32 monAnim01DesktopsSatellite[] = {
     MONUSEIMAGE(IMG2DMATH),
     MONHORZSCROLL(0x400, 20),
     MONHOLDTIME(20),
     MONVERTSCROLL(0x400, 20),
     MONRGBA(COLOR_BLACK, 20),
     MONHOLDTIME(20),
     MONZOOMSQUARE(0x200, 20),
     MONRGBA(COLOR_WHITE, 20),
     MONHOLDTIME(20),
     MONZOOMSQUARE(0x400, 20),
     MONHOLDTIME(20),
     MONUSEIMAGE(IMGSATELLITE),
     MONHORZSCROLL(0x400, 20),
     MONHOLDTIME(20),
     MONVERTSCROLL(0x400, 20),
     MONRGBA(COLOR_BLACK, 20),
     MONHOLDTIME(20),
     MONZOOMSQUARE(0x200, 20),
     MONRGBA(COLOR_WHITE, 20),
     MONHOLDTIME(20),
     MONZOOMSQUARE(0x400, 20),
     MONHOLDTIME(20),
     MONUSEIMAGE(IMGDESKTOP),
     MONHORZSCROLL(0x400, 20),
     MONHOLDTIME(20),
     MONVERTSCROLL(0x400, 20),
     MONRGBA(COLOR_BLACK, 20),
     MONHOLDTIME(20),
     MONZOOMSQUARE(0x200, 20),
     MONRGBA(COLOR_WHITE, 20),
     MONHOLDTIME(20),
     MONZOOMSQUARE(0x400, 20),
     MONHOLDTIME(20),
     MONUSEIMAGE(IMGDESKTOPSTAGGERED),
     MONHORZSCROLL(0x400, 20),
     MONHOLDTIME(20),
     MONVERTSCROLL(0x400, 20),
     MONRGBA(COLOR_BLACK, 20),
     MONHOLDTIME(20),
     MONZOOMSQUARE(0x200, 20),
     MONRGBA(COLOR_WHITE, 20),
     MONHOLDTIME(20),
     MONZOOMSQUARE(0x400, 20),
     MONHOLDTIME(20),
     MONLOOP(),
};

//[80030E24	02	10 screens: astrological]
u32 monAnim02Astrological[] = {
     MONUSEIMAGE(IMGSHUTTLE1), MONHOLDTIME(80),
     MONUSEIMAGE(IMGSHUTTLE2), MONHOLDTIME(80),
     MONUSEIMAGE(IMGEARTHFULL1), MONHOLDTIME(80),
     MONUSEIMAGE(IMGEARTHFULL2), MONHOLDTIME(80),
     MONUSEIMAGE(IMGBLUESTARS), MONHOLDTIME(80),
     MONUSEIMAGE(IMGGALAXY1), MONHOLDTIME(80),
     MONUSEIMAGE(IMGGALAXY2), MONHOLDTIME(80),
     MONUSEIMAGE(IMGEARTHTEXT), MONHOLDTIME(80),
     MONUSEIMAGE(IMGTARGETEARTH), MONHOLDTIME(80),
     MONUSEIMAGE(IMGGALAXY3), MONHOLDTIME(80),
     MONLOOP(),
};

//[80030EC8	0F	7 screens: satellite, targetting, ]
u32 monAnim0FSatelliteTargeting[] = {
     MONUSEIMAGE(IMGEARTH), MONHOLDTIME(80),
     MONUSEIMAGE(IMGDESKTOPBANG), MONHOLDTIME(80),
     MONUSEIMAGE(IMGHEATMAP), MONHOLDTIME(80),
     MONUSEIMAGE(IMG2DMATH), MONHOLDTIME(80),
     MONUSEIMAGE(IMGSATELLITE), MONHOLDTIME(80),
                        MONHOLDTIME(80),
     MONUSEIMAGE(IMGTARGETEARTH), MONHOLDTIME(80),
     MONUSEIMAGE(IMGEARTHFULL2), MONHOLDTIME(80),
     MONLOOP()
};

//[80030F44	03	3 wave patterns]
u32 monAnim03ThreeWavePattern[] = {
     MONRGBA(COLOR_MINESHAFT3, 1),
     MONUSEIMAGE(IMGSINE),
     MONHORZSCROLL(0x800, 120),
     MONHOLDTIME(120),
     MONZOOMWIDTH(0x100, 1),
     MONZOOMHEIGHT(0x200, 60),
     MONHORZSCROLL(0xFFFFE000, 120),
     MONHOLDTIME(120),
     MONZOOMWIDTH(0x400, 1),
     MONZOOMHEIGHT(0x400, 60),
     MONZOOMHEIGHT(0x400, 60),
     MONHORZSCROLL(0x800, 120),
     MONHOLDTIME(120),
     MONZOOMWIDTH(0x80, 1),
     MONZOOMHEIGHT(0x800, 60),
     MONZOOMHEIGHT(0x400, 120),
     MONVERTSCROLL(0x400, 60),
     MONHORZSCROLL(0x200, 120),
     MONHOLDTIME(120),
     MONLOOP()
};

//[80031018	04	wave pattern]
u32 monAnim04WavePattern[] = {
     MONRGBA(COLOR_MINESHAFT3, 1),
     MONUSEIMAGE(IMGSINE),
     MONZOOMWIDTH(0x80, 1),
     MONZOOMHEIGHT(0x800, 60),
     MONZOOMHEIGHT(0x400, 120),
     MONVERTSCROLL(0x400, 10),
     MONHORZSCROLL(0x200, 40),
     MONHOLDTIME(120),
     MONLOOP()
};

//[80031074	05	green text up]
u32 monAnim05GreenTextUp[] = {
     MONUSEIMAGE(IMGTEXT),
     MONRGBA(COLOR_BARELYGREENOPAQUE, 1),
     MONVERTSCROLL(0xFFFFFE00, 80),
     MONHOLDTIME(120),
     MONVERTSCROLL(0xFFFFFF00, 20),
     MONHOLDTIME(120),
     MONVERTSCROLL(0xFFFFFF80, 10),
     MONHOLDTIME(40),
     MONVERTSCROLL(0xFFFFFE00, 40),
     MONHOLDTIME(60),
     MONVERTSCROLL(0xFFFFFFC0, 30),
     MONHOLDTIME(120),
     MONLOOP()
};

//[800310F0	06	red text down]
u32 monAnim06RedTextDown[] = {
     MONUSEIMAGE(IMGTEXT),
     MONRGBA(COLOR_DIESEL, 1),
     MONVERTSCROLL(0x200, 80),
     MONHOLDTIME(120),
     MONVERTSCROLL(0x100, 20),
     MONHOLDTIME(120),
     MONVERTSCROLL(0x80, 10),
     MONHOLDTIME(40),
     MONVERTSCROLL(0x200, 40),
     MONHOLDTIME(60),
     MONVERTSCROLL(0x40, 30),
     MONHOLDTIME(120),
     MONVERTSCROLL(0x100, 20),
     MONHOLDTIME(120),
     MONVERTSCROLL(0x80, 10),
     MONLOOP()
};

//[8003118C	07	d. green text down]
u32 monAnim07GreenTextDown[] = {
     MONUSEIMAGE(IMGTEXT),
     MONRGBA(COLOR_DEEPFIR, 1),
     MONVERTSCROLL(0x200, 80),
     MONHOLDTIME(120),
     MONVERTSCROLL(0x80, 10),
     MONHOLDTIME(40),
     MONVERTSCROLL(0x100, 20),
     MONHOLDTIME(120),
     MONVERTSCROLL(0x80, 10),
     MONHOLDTIME(40),
     MONVERTSCROLL(0x200, 40),
     MONHOLDTIME(60),
     MONVERTSCROLL(0x40, 30),
     MONHOLDTIME(120),
     MONLOOP()
};

//[8003121C	08	red bar graph +]
u32 monAnim08RedBarGraph[] = {
     MONUSEIMAGE(IMGBARS),
     MONRGBA(COLOR_VERDUNGREEN, 1),
     MONHORZSCROLL(0x280, 1),
     MONHOLDTIME(10),
     MONLOOP()
};

//[80031248	09	blue bar graph +]
u32 monAnim09BlueBarGraph[] = {
     MONUSEIMAGE(IMGBARS),
     MONRGBA(COLOR_CYPRUS, 1),
     MONHORZSCROLL(0x280, 1),
     MONHOLDTIME(10),
     MONLOOP()
};

//[80031274	0A	green bar graph -]
u32 monAnim0AGreenBarGraph[] = {
     MONUSEIMAGE(IMGBARS),
     MONRGBA(COLOR_TOMTHUMB, 1),
     MONHORZSCROLL(0xFFFFFD80, 1),
     MONHOLDTIME(10),
     MONLOOP()
};

//[800312A0	subroutine	used by radar]
u32 monAnimRadarSub1[] = {
     MONRGBA(COLOR_GREEN, 20),
     MONJUMPTO(monAnimRadarSub3)
};

//[800312B4	subroutine	used by radar]
u32 monAnimRadarSub2[] = {
     MONRGBA(COLOR_SANFELIX, 20),
     MONJUMPTO(monAnimRadarSub3)
};

//[800312C8	subroutine	used by radar]
u32 monAnimRadarSub3[] = {
     MONROTATEIMAGE(0xB6),
     MONHOLDTIME(1),
     MONJUMPCHANCE(monAnimRadarSub1, TWO_PERCENT_CHANCE),
     MONJUMPCHANCE(monAnimRadarSub2, 0x147A),
     MONLOOP()
};

//[800312F4	0B	radar]
u32 monAnim0BRadar[] = {
     MONUSEIMAGE(IMGTRIANGLE),
     MONRGBA(COLOR_ALMOSTDARKGREEN, 1),
     MONJUMPTO(monAnimRadarSub2)
};

//[80031310	0C	spinning cube]
u32 monAnim0CSpinningCube[] = {
     MONUSEIMAGE(IMGCUBE1),
     MONRGBA(COLOR_MINSK, 30),
     MONHOLDTIME(5),
     MONUSEIMAGE(IMGCUBE2),
     MONHOLDTIME(5),
     MONUSEIMAGE(IMGCUBE3),
     MONHOLDTIME(5),
     MONUSEIMAGE(IMGCUBE4),
     MONHOLDTIME(5),
     MONLOOP()
};

//[80031360	10	global map]
u32 monAnim10GlobalMap[] = {
     MONUSEIMAGE(IMGWORLDMAP),
     MONRGBA(COLOR_SEAGREEN, 30),
     MONHORZSCROLL(0xFFFFFC00, 1024),
     MONHOLDTIME(1440),
     MONHORZSCROLLNA(0x288, 360),
     MONVERTSCROLLNA(0x3AA, 360),
     MONZOOMSQUARE(0x80, 300),
     MONRGBA(COLOR_BLACK, 60),
     MONHOLDTIME(60),
     MONRGBA(COLOR_GRAY, 10),
     MONHOLDTIME(90),
     MONRGBA(COLOR_APPLE2, 30),
     MONHOLDTIME(30),
     MONRGBA(COLOR_LOTUS, 60),
     MONHOLDTIME(60),
     MONRGBA(COLOR_GRAY, 60),
     MONHOLDTIME(60),
     MONHORZSCROLLNA(0x200, 360),
     MONVERTSCROLLNA(0x200, 360),
     MONZOOMSQUARE(0x400, 720),
     MONHOLDTIME(300),
     MONUSEIMAGE(IMGWORLDMAP),
     MONHOLDTIME(420),
     MONRGBA(COLOR_STRONGGREEN, 30),
     MONHOLDTIME(30),
     MONRGBA(COLOR_GREENKELP, 60),
     MONHOLDTIME(60),
     MONLOOP()
};

//[80031490	0D	3 screens: location, weapon armed, ]
u32 monAnim0DLocWeapArmed[] = {
     MONRGBA(COLOR_BLACK, 1),
     MONRGBA(COLOR_SILVER, 400),
     MONUSEIMAGE(1),
     MONHOLDTIME(680),
     MONUSEIMAGE(2),
     MONHOLDTIME(680),
     MONUSEIMAGE(4),
     MONHOLDTIME(180),
     MONRGBA(COLOR_PESTO, 1),
     MONUSEIMAGE(4),
     MONHOLDTIME(200),
     MONLOOP()
};

//[800314F8	0E	red target]
u32 monAnim0ERedTarget[] = {
     MONZOOMSQUARE(0x400, 1),
     MONRGBA(COLOR_THUNDERBIRD, 1),
     MONUSEIMAGE(6),
     MONHOLDTIME(600),
     MONRGBA(COLOR_SILVER, 5),
     MONHOLDTIME(5),
     MONRGBA(COLOR_MINESHAFT, 60),
     MONUSEIMAGE(IMGSTATIC),
     MONRGBA(COLOR_CODGRAY, 100),
     MONHOLDTIME(400),
     MONLOOP()
};

//[8003156C	11	Karl yelling]
u32 monAnim11KarlYelling[] = {
     MONRGBA(COLOR_DARKGREEN, 0),
     MONUSEIMAGE(IMGTALK1),
     MONHOLDTIME(5),
     MONUSEIMAGE(IMGTALK2),
     MONHOLDTIME(5),
     MONUSEIMAGE(IMGTALK3),
     MONHOLDTIME(5),
     MONUSEIMAGE(IMGTALK4),
     MONHOLDTIME(10),
     MONUSEIMAGE(IMGTALK2),
     MONHOLDTIME(5),
     MONLOOP()
};

//[800315CC	12	skateboard]
u32 monAnim12Skateboard[] = {
     MONUSEIMAGE(IMGSKATEBOARD4),
     MONRGBA(COLOR_DARKGREEN, 0),
     MONHOLDTIME(3),
     MONRGBA(COLOR_DARKERGREEN, 0),
     MONHOLDTIME(2),
     MONRGBA(COLOR_DARKGREEN, 0),
     MONHOLDTIME(3),
     MONRGBA(COLOR_DARKERGREEN, 0),
     MONHOLDTIME(2),
     MONHORZSCROLL(0x264, 30),
     MONUSEIMAGE(IMGSKATEBOARD1),
     MONRGBA(COLOR_DARKGREEN, 0),
     MONHOLDTIME(3),
     MONRGBA(COLOR_DARKERGREEN, 0),
     MONHOLDTIME(2),
     MONUSEIMAGE(IMGSKATEBOARD2),
     MONRGBA(COLOR_DARKGREEN, 0),
     MONHOLDTIME(3),
     MONRGBA(COLOR_DARKERGREEN, 0),
     MONHOLDTIME(2),
     MONUSEIMAGE(IMGSKATEBOARD3),
     MONRGBA(COLOR_DARKGREEN, 0),
     MONHOLDTIME(3),
     MONRGBA(COLOR_DARKERGREEN, 0),
     MONHOLDTIME(2),
     MONRGBA(COLOR_DARKGREEN, 0),
     MONHOLDTIME(3),
     MONRGBA(COLOR_DARKERGREEN, 0),
     MONHOLDTIME(2),
     MONRGBA(COLOR_DARKGREEN, 0),
     MONHOLDTIME(3),
     MONRGBA(COLOR_DARKERGREEN, 0),
     MONHOLDTIME(2),
     MONRGBA(COLOR_DARKGREEN, 0),
     MONHOLDTIME(3),
     MONRGBA(COLOR_DARKERGREEN, 0),
     MONHOLDTIME(2),
     MONHORZSCROLL(0x19C, 40),
     MONUSEIMAGE(IMGSKATEBOARD2),
     MONRGBA(COLOR_DARKGREEN, 0),
     MONHOLDTIME(3),
     MONRGBA(COLOR_DARKERGREEN, 0),
     MONHOLDTIME(2),
     MONRGBA(COLOR_DARKGREEN, 0),
     MONHOLDTIME(3),
     MONRGBA(COLOR_DARKERGREEN, 0),
     MONHOLDTIME(2),
     MONUSEIMAGE(IMGSKATEBOARD1),
     MONRGBA(COLOR_DARKGREEN, 0),
     MONHOLDTIME(3),
     MONRGBA(COLOR_DARKERGREEN, 0),
     MONHOLDTIME(2),
     MONRGBA(COLOR_DARKGREEN, 0),
     MONHOLDTIME(3),
     MONRGBA(COLOR_DARKERGREEN, 0),
     MONHOLDTIME(2),
     MONRGBA(COLOR_DARKGREEN, 0),
     MONHOLDTIME(3),
     MONRGBA(COLOR_DARKERGREEN, 0),
     MONHOLDTIME(2),
     MONRGBA(COLOR_DARKGREEN, 0),
     MONHOLDTIME(3),
     MONRGBA(COLOR_DARKERGREEN, 0),
     MONHOLDTIME(2),
     MONLOOP()
};

//[80031848	13	police guy]
u32 monAnim13PoliceGuy[] = {
    MONRGBA(COLOR_DARKGREEN2, 0),
    MONUSEIMAGE(IMGFIST1),
    MONHOLDTIME(5),
    MONUSEIMAGE(IMGFIST2),
    MONHOLDTIME(5),
    MONUSEIMAGE(IMGFIST3),
    MONHOLDTIME(5),
    MONUSEIMAGE(IMGFIST4),
    MONHOLDTIME(5),
    MONLOOP()
};

//[80031898	14	'off']
u32 monAnim14Off[] = {
    MONUSEIMAGE(IMGSINE),
    MONRGBA(COLOR_BARELYGREEN, 1),
    MONHOLDTIME(5),
    MONLOOP()
};

//[800318B8	15	randomly select one of seven animations]
u32 monAnim15RandomSeven[] = {
    MONJUMPCHANCE(monAnim04WavePattern, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monAnim11KarlYelling, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monAnim08RedBarGraph, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monAnim09BlueBarGraph, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monAnim0AGreenBarGraph, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monAnim06RedTextDown, TWENTY_PERCENT_CHANCE),
    MONJUMPCHANCE(monAnim07GreenTextDown, FOURTY_PERCENT_CHANCE),
    MONJUMPCHANCE(monAnim05GreenTextUp, HUNDRED_PERCENT_CHANCE),
    MONLOOP()
};

//[8003191C	16	randomly select random screens + random effects or boring]
u32 monAnim16RandomFour[] = {
    MONJUMPCHANCE(monAnim03ThreeWavePattern, TWO_PERCENT_CHANCE),
    MONJUMPCHANCE(monAnim08RedBarGraph, TWO_PERCENT_CHANCE),
    MONJUMPCHANCE(monAnim05GreenTextUp, TWO_PERCENT_CHANCE),
    MONJUMPCHANCE(monAnim17RandImageEffect, SIXTY_PERCENT_CHANCE),
    MONLOOP()
};

//[80031950	17	Base Function for random screens + random effects]
u32 monAnim17RandImageEffect[] = {
    MONJUMPCHANCE(monRandEffectChanceSHUTTLE1, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monRandEffectChanceSHUTTLE2, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monRandEffectChanceEARTHFULL1, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monRandEffectChanceEARTHFULL2, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monRandEffectChanceBLUESTARS, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monRandEffectChanceGALAXY1, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monRandEffectChanceGALAXY2, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monRandEffectChanceEARTHTEXT, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monRandEffectChanceTARGETEARTH, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monRandEffectChanceGALAXY3, TEN_PERCENT_CHANCE),
    MONHOLDTIME(100),
    MONLOOP()
};

//[800319D4	18	random screens + random effects - set image]
u32 monRandEffectChanceSHUTTLE1[] = {
    MONUSEIMAGE(IMGSHUTTLE1),
    MONHOLDTIME(20),
    MONJUMPCHANCE(monRandChanceScrollOrZoomRandRGBN, HUNDRED_PERCENT_CHANCE)
};

//[800319F0	19	random screens + random effects - set image]
u32 monRandEffectChanceSHUTTLE2[] = {
    MONUSEIMAGE(IMGSHUTTLE2),
    MONHOLDTIME(20),
    MONJUMPCHANCE(monRandChanceScrollOrZoomRandRGBN, HUNDRED_PERCENT_CHANCE)
};

//[80031A0C	1A	random screens + random effects - set image]
u32 monRandEffectChanceEARTHFULL1[] = {
    MONUSEIMAGE(IMGEARTHFULL1),
    MONHOLDTIME(20),
    MONJUMPCHANCE(monRandChanceScrollOrZoomRandRGBN, HUNDRED_PERCENT_CHANCE)
};

//[80031A28	1B	random screens + random effects - set image]
u32 monRandEffectChanceEARTHFULL2[] = {
    MONUSEIMAGE(IMGEARTHFULL2),
    MONHOLDTIME(20),
    MONJUMPCHANCE(monRandChanceScrollOrZoomRandRGBN, HUNDRED_PERCENT_CHANCE)
};

//[80031A44	1C	random screens + random effects - set image]
u32 monRandEffectChanceBLUESTARS[] = {
    MONUSEIMAGE(IMGBLUESTARS),
    MONHOLDTIME(20),
    MONJUMPCHANCE(monRandChanceScrollOrZoomRandRGBN, HUNDRED_PERCENT_CHANCE)
};

//[80031A60	1D	random screens + random effects - set image]
u32 monRandEffectChanceGALAXY1[] = {
    MONUSEIMAGE(IMGGALAXY1),
    MONHOLDTIME(20),
    MONJUMPCHANCE(monRandChanceScrollOrZoomRandRGBN, HUNDRED_PERCENT_CHANCE)
};

//[80031A7C	1E	random screens + random effects - set image]
u32 monRandEffectChanceGALAXY2[] = {
    MONUSEIMAGE(IMGGALAXY2),
    MONHOLDTIME(20),
    MONJUMPCHANCE(monRandChanceScrollOrZoomRandRGBN, HUNDRED_PERCENT_CHANCE)
};

//[80031A98	1F	random screens + random effects - set image]
u32 monRandEffectChanceEARTHTEXT[] = {
    MONUSEIMAGE(IMGEARTHTEXT),
    MONHOLDTIME(20),
    MONJUMPCHANCE(monRandChanceScrollOrZoomRandRGBN, HUNDRED_PERCENT_CHANCE)
};

//[80031AB4	20	random screens + random effects - set image]
u32 monRandEffectChanceTARGETEARTH[] = {
    MONUSEIMAGE(IMGTARGETEARTH),
    MONHOLDTIME(20),
    MONJUMPCHANCE(monRandChanceScrollOrZoomRandRGBN, HUNDRED_PERCENT_CHANCE)
};

//[80031AD0	21	random screens + random effects - set image]
u32 monRandEffectChanceGALAXY3[] = {
    MONUSEIMAGE(IMGGALAXY3),
    MONHOLDTIME(20),
    MONJUMPCHANCE(monRandChanceScrollOrZoomRandRGBN, HUNDRED_PERCENT_CHANCE)
};

//[80031AEC	22	random screens + random effects - colourizer]
u32 monRandChanceScrollOrZoomRandRGBN[] = {
    MONJUMPCHANCE(monRandChanceScrollOrZoomRed, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monRandChanceScrollOrZoomGreen, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monRandChanceScrollOrZoomBlue, TEN_PERCENT_CHANCE),
    MONRGBA(COLOR_SILVER, 60),
    MONJUMPTO(monRandChanceScrollOrZoom)
};

//[80031B24	23	random screens + random effects - colourizer]
u32 monRandChanceScrollOrZoomRed[] = {
    MONRGBA(COLOR_PERSIANRED, 60),
    MONJUMPTO(monRandChanceScrollOrZoom)
};

u32 monRandChanceScrollOrZoomGreen[] = {
    MONRGBA(COLOR_APPLE, 60),
    MONJUMPTO(monRandChanceScrollOrZoom)
};

u32 monRandChanceScrollOrZoomBlue[] = {
    MONRGBA(COLOR_GOVERNORBAY, 60),
    MONJUMPTO(monRandChanceScrollOrZoom)
};

u32 monRandChanceScrollOrZoom[] = {
    MONHOLDTIME(50),
    MONJUMPCHANCE(monAnim27RandomEffectScrollRight, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monAnim28RandomEffectScrollUpFast, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monAnim29RandomEffectScrollUp, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monAnim2ARandEffectScrollZoom1, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monAnim2ARandEffectScrollZoom2, TEN_PERCENT_CHANCE),
    MONHOLDTIME(300),
    MONJUMPTO(monAnim2CRandEffectWaitRoute)
};


//[80031BB4	27	random screens + random effects - scroll right]
u32 monAnim27RandomEffectScrollRight[] = {
    MONHORZSCROLL(0x800, 120),
    MONHOLDTIME(120),
    MONJUMPTO(monAnim2CRandEffectWaitRoute)
};

//[80031BD0	28	random screens + random effects - scroll up fast]
u32 monAnim28RandomEffectScrollUpFast[] = {
    MONVERTSCROLL(0x2000, 50),
    MONHOLDTIME(200),
    MONJUMPTO(monAnim2CRandEffectWaitRoute)
};

//[80031BEC	29	random screens + random effects - scroll up]
u32 monAnim29RandomEffectScrollUp[] = {
    MONVERTSCROLL(0x2000, 200),
    MONHOLDTIME(200),
    MONJUMPTO(monAnim2CRandEffectWaitRoute)
};

//[80031C08	2A	random screens + random effects - scroll and zoom]
u32 monAnim2ARandEffectScrollZoom1[] = {
    MONHORZSCROLLNA(0x288, 300),
    MONVERTSCROLLNA(0x3AA, 300),
    MONZOOMSQUARE(0x80, 200),
    MONHOLDTIME(300),
    MONHORZSCROLLNA(0x200, 50),
    MONVERTSCROLLNA(0x200, 200),
    MONZOOMSQUARE(0x400, 720),
    MONHOLDTIME(600),
    MONJUMPTO(monAnim2CRandEffectWaitRoute)
};

//[80031C80	2B	random screens + random effects - scroll and zoom]
u32 monAnim2ARandEffectScrollZoom2[] = {
    MONHORZSCROLLNA(0x320, 400),
    MONVERTSCROLLNA(0x190, 400),
    MONZOOMSQUARE(0x80, 200),
    MONHOLDTIME(300),
    MONHORZSCROLLNA(0xC8, 200),
    MONVERTSCROLLNA(0x190, 800),
    MONZOOMSQUARE(0x200, 720),
    MONHOLDTIME(800),
    MONZOOMSQUARE(0x400, 720),
    MONHORZSCROLLNA(0x200, 100),
    MONVERTSCROLLNA(0x200, 60),
    MONHOLDTIME(500),
    MONJUMPTO(monAnim2CRandEffectWaitRoute)
};

//[80031D30	2C	random screens + random effects - wait and route]
u32 monAnim2CRandEffectWaitRoute[] = {
    MONHOLDTIME(50),
    MONJUMPCHANCE(monRandChanceScrollOrZoomRandRGBN, TEN_PERCENT_CHANCE),
    MONJUMPCHANCE(monAnim2DRandEffectFlash, TWENTY_PERCENT_CHANCE),
    MONJUMPTO(monAnim17RandImageEffect)
};

//[80031D58	2D	random screens + random effects - flash]
u32 monAnim2DRandEffectFlash[] = {
    MONHOLDTIME(50),
    MONRGBA(COLOR_WHITE, 10),
    MONRGBA(COLOR_BLACK, 5),
    MONRGBA(COLOR_WHITE, 10),
    MONHOLDTIME(25),
    MONRGBA(COLOR_BLACK, 200),
    MONHOLDTIME(500),
    MONJUMPTO(monAnim17RandImageEffect)
};

//[80031DA8	2E	red brightening screen]
u32 monAnim2ERedBrightening[] = {
     MONUSEIMAGE(IMGKEYBOARDKEY),
     MONZOOMSQUARE(0x200, 0),
     MONRGBA(COLOR_ALIZARINCRIMSON, 60),
     MONHOLDTIME(60),
     MONRGBA(COLOR_MINESHAFT2, 10),
     MONHOLDTIME(10),
     MONLOOP()
};

//[80031DF4	2F	green brightening screen]
u32 monAnim2FGreenBrightening[] = {
     MONUSEIMAGE(IMGKEYBOARDKEY),
     MONZOOMSQUARE(0x200, 0),
     MONRGBA(COLOR_APPLE, 60),
     MONHOLDTIME(60),
     MONRGBA(COLOR_MINESHAFT2, 10),
     MONHOLDTIME(10),
     MONLOOP()
};

//[80031E40	30	grey solid]
u32 monAnim30GreySolid[] = {
     MONUSEIMAGE(IMGKEYBOARDKEY),
     MONZOOMSQUARE(0x200, 0),
     MONRGBA(COLOR_MINESHAFT2, 10),
     MONHOLDTIME(10),
     MONLOOP()
};

//[80031E78	31	red solid]
u32 monAnim31RedSolid[] = {
     MONUSEIMAGE(IMGKEYBOARDKEY),
     MONZOOMSQUARE(0x200, 0),
     MONRGBA(COLOR_ALIZARINCRIMSON, 10),
     MONHOLDTIME(10),
     MONLOOP()
};

//[80031EB0	32	green solid]
u32 monAnim32GreenSolid[] = {
     MONUSEIMAGE(IMGKEYBOARDKEY),
     MONZOOMSQUARE(0x200, 0),
     MONRGBA(COLOR_APPLE, 10),
     MONHOLDTIME(10),
     MONLOOP()
};

//[80031EE8	33	black solid]
u32 monAnim33BlackSolid[] = {
     MONUSEIMAGE(0),
     MONRGBA(COLOR_BLACK, 0),
     MONSTOPANIM()
};

//[80031F00	34	???	Not Included in Normal List - linked @ 0x9544]
u32 monAnim34[] = {
     MONZOOMSQUARE(0x400, 0),
     MONHOLDTIME(1),
     MONZOOMSQUARE(0x1000, 20),
     MONHOLDTIME(20),
     MONLOOP()
};

//[80031F44	35	Taser	Not Included in Normal List!]
u32 monAnim35Taser[] = {
     MONUSEIMAGE(IMGBOND),
     MONHORZSCROLL(0x400, 20), MONHOLDTIME(20),
     MONVERTSCROLL(0x400, 20), MONRGBA(COLOR_BLACK, 20), MONHOLDTIME(20),
     MONZOOMSQUARE(0x200, 20), MONRGBA(COLOR_WHITE, 20), MONHOLDTIME(20),
     MONZOOMSQUARE(0x400, 20), MONHOLDTIME(20),
     MONLOOP()
};

/**
 * Address 0x80031FD0.
*/
ModelRenderData D_80031FD0 = {  NULL,
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
                                {0,0,0,0},
                                {0,0,0,0},
                                CULLMODE_BOTH};


// Forward declarations.

s32 updateDoorDisplacement(DoorRecord* door);
s32 objGetShotsTaken(ObjectRecord *);
void sub_GAME_7F04AC20(PropRecord *prop, ModelRenderData *, s32 arg2);
bool chrobjSeparatingAxisTheorem(rect4f* rect1, s32 numvertices0, rect4f* rect2, s32 numvertices1);
void chrobjSndCreatePostEvent(ALSoundState *state, coord3d *pos, f32 low, f32 high);
void sub_GAME_7F050DE8(Model* model);
void remove_obj_from_temp_proxmine_table(WeaponObjRecord* proxy);
void add_obj_to_temp_proxmine_table(WeaponObjRecord* proxy);
s32 sub_GAME_7F042EB4(struct ObjectRecord *arg0, f32 *arg1, struct coord3d *arg2, struct coord3d *arg3, s32 arg4, s32 arg5);
s32 sub_GAME_7F042A0C(struct ObjectRecord *arg0, f32 *arg1, struct coord3d *arg2, struct coord3d *arg3, s32 arg4);
s32 handles_projectile_motion(struct ObjectRecord *arg0, f32 *arg1, struct coord3d *arg2, struct coord3d *arg3, s32 arg4, s32 arg5);
void sub_GAME_7F0431E4(struct ObjectRecord *arg0, struct coord3d *arg1);
void door7F054FB4(struct DoorRecord *arg0);

/* PD: projectileFree (similar but not the same structure) */
void projectileFree(Projectile* projectile)
{
    ALSoundState* sound1;
    ALSoundState* sound2;

    if (projectile->flags & PROJECTILEFLAG_LAUNCHING)
    {
        sound1 = projectile->sound1;
        if ((sound1 != 0) && (sndGetPlayingState((ALSoundState* ) sound1) != 0))
        {
            sndDeactivate((ALSoundState* ) projectile->sound1);
        }

        sound2 = projectile->sound2;
        if ((sound2 != 0) && (sndGetPlayingState((ALSoundState* ) sound2) != 0))
        {
            sndDeactivate((ALSoundState* ) projectile->sound2);
        }
    }
    projectile->flags |= PROJECTILEFLAG_FREE;
}


void projectileReset(Projectile *projectile)
{
    projectile->flags = 0;

    projectile->speed.x = 0.0f;
    projectile->speed.y = 0.0f;
    projectile->speed.z = 0.0f;

    projectile->unk10.x = 0.0f;
    projectile->unk10.y = 0.0f;
    projectile->unk10.z = 0.0f;

    projectile->unk1C = 0.0f;

    projectile->unk60 = 1.0f;
    projectile->ownerprop = NULL;
    projectile->unk8C = 0.05f;
    projectile->unk90 = 0;
    projectile->unk94 = 0.0f;
    projectile->unkA0 = -1;
    projectile->unkA4 = 0;
    projectile->unkA8 = 0;
    projectile->unkAC = -1;
    projectile->droptype = DROPTYPE_DEFAULT;
    projectile->refreshrate = 0;
    projectile->unkC0 = 1.0f;
    projectile->unkC4 = 1.0f;
    projectile->unkC8 = 1.0f;
    projectile->unkE0 = 0;
    projectile->obj = 0;
    projectile->unkE8 = 0;
}


Projectile *projectileAllocate(void)
{
    s32 bestindex;
    s32 i;

    bestindex = -1;

    // Happy path - find one that is already free
    for (i = 0; i < PROJECTILES_ARR_MAX; i++)
    {
        if (g_Projectiles[i].flags & PROJECTILEFLAG_FREE)
        {
            projectileReset(g_Projectiles + i);
            return (g_Projectiles + i);
        }
    }

    // Find one with the lowest unkE8 (some kind of age/timer?)
    // and some other conditions
    for (i = 0; i < PROJECTILES_ARR_MAX; i++)
    {
        if (g_Projectiles[i].obj && (bestindex < 0 || g_Projectiles[i].unkE8 < g_Projectiles[bestindex].unkE8))
        {
            bestindex = i;
        }
    }

    if (bestindex >= 0)
    {
        // Reset and return it
        objFreeEmbedmentOrProjectile(g_Projectiles[bestindex].obj->prop);
        g_Projectiles[bestindex].obj->runtime_bitflags |= RUNTIMEBITFLAG_REMOVE;

        projectileReset(g_Projectiles + bestindex);
        return (g_Projectiles + bestindex);
    }
    else
    {
        return NULL;
    }
}


void sub_GAME_7F03FDA8(PropRecord *prop)
{
    ObjectRecord *obj = prop->obj; //po
    if (obj->runtime_bitflags & RUNTIMEBITFLAG_EMBEDDED)
    {
        #ifdef DEBUG
        //assert(po->move.attach->fallinfo==NULL);
        #endif
        obj->embedment->projectile = projectileAllocate();
    }
    else if ((obj->runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT) == 0)
    {
        obj->projectile = projectileAllocate();

        if (obj->projectile)
        {
            obj->runtime_bitflags |= RUNTIMEBITFLAG_DEPOSIT;
        }
    }
}


void projectileSetSticky(PropRecord *prop)
{
    ObjectRecord *obj = prop->obj;
    Projectile *projectile = NULL;

    if (obj->runtime_bitflags & RUNTIMEBITFLAG_EMBEDDED)
    {
        projectile = obj->embedment->projectile;
    }
    else if (obj->runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT)
    {
        projectile = obj->projectile;
    }

    if (projectile)
    {
        projectile->flags |= PROJECTILEFLAG_STICKY;
        if (prop->stan)
        {
            projectile->unkCC = prop->stan->room;
            projectile->unkCD = 0xFF;
            return;
        }
        projectile->unkCC = 0xFFU;
    }
}


void embedmentFree(Embedment *embedment)
{
    embedment->flags |= EMBEDMENTFLAG_FREE;
}


Embedment *embedmentAllocate(void)
{
    s32 i;

    for (i = 0; i < EMBEDMENT_ARR_MAX; i++)
    {
        if (g_Embedments[i].flags & 1)
        {
            g_Embedments[i].flags = 0;
            g_Embedments[i].projectile = NULL;
            return &g_Embedments[(u32)i];
        }
    }

    return NULL;
}


/**
 * This doesn't exactly return the number of shots taken but it's the best way
 * to describe the behaviour of the function without writing a novel into the
 * function's name.
 *
 * The number returned is 0 when at full health and only ever increments as the
 * object takes damage. While healthy, the number scales from 0 to 4 based on
 * how close it is to being destroyed, where 4 is destroyed. After being
 * destroyed, the number increments at 1 per shot up to a max of 12.
 */

s32 objGetShotsTaken(ObjectRecord *obj)
{
    if (!(obj->state & PROPSTATE_DESTROYED))
    {
        return (obj->maxdamage * 3.0f) / obj->damage;
    }

    return obj->maxdamage + 4.0f;
}


/**
 * Return 0 if not destroyed
 * Return 1 if at destroyed level 1
 * Return 2 if at destroyed level 2
 * Return 3 if at destroyed level 3
 *
 * Each destroyed level is a new phase of visual brokenness. Typically the
 * object is destroyed and it looks broken (level 1), then after a couple of
 * shots it enters level 2, and a few shots later level 3.
 *
 * While healthy, damage goes from 0 to maxdamage (eg. 1000) but this function
 * returns 0 due to the if statement.
 *
 * When destroyed, damage is reset to 0 then incremented at one unit per shot,
 * so four shots causes it to enter a new destroyed level.
 */

s32 objGetDestroyedLevel(ObjectRecord *obj)
{
    if (!(obj->state & PROPSTATE_DESTROYED))
    {
        return 0; //if Not Dead
    }
    return ((s32) obj->maxdamage >> 2) + 1;
}


ModelRoData_BoundingBoxRecord *chrobjGetBboxFromObjFile(ModelFileHeader *obj)
{
    ModelNode *mdlnext;

    if (obj->RootNode->Child)
    {
        //for each next node, check for BBox
        for (mdlnext = obj->RootNode->Child; mdlnext; mdlnext = mdlnext->Next)
        {
            if (mdlnext->Opcode == MODELNODE_OPCODE_BBOX)
            {
                return mdlnext->Data;
            }
        }

        //none found, check FIRST child
        if (obj->RootNode->Child->Child)
        {
            //for each next node, check for BBox
            for (mdlnext = obj->RootNode->Child->Child; mdlnext; mdlnext = mdlnext->Next)
            {
                if (mdlnext->Opcode == MODELNODE_OPCODE_BBOX)
                {
                    return mdlnext->Data;
                }
            }
        }
    }
    return NULL;
}


struct ModelRoData_BoundingBoxRecord* chrobjGetBboxFromObjectRecord(ObjectRecord *arg0)
{
    return (struct ModelRoData_BoundingBoxRecord *)chrobjGetBboxFromObjFile(arg0->model->obj);
}





#ifdef NONMATCHING
void set_color_shading_from_tile(void) {

}
#else
GLOBAL_ASM(
.text
glabel set_color_shading_from_tile
/* 074BCC 7F04009C 27BDFFE8 */  addiu $sp, $sp, -0x18
/* 074BD0 7F0400A0 AFBF0014 */  sw    $ra, 0x14($sp)
/* 074BD4 7F0400A4 AFA40018 */  sw    $a0, 0x18($sp)
/* 074BD8 7F0400A8 00807025 */  move  $t6, $a0
/* 074BDC 7F0400AC 00A03825 */  move  $a3, $a1
/* 074BE0 7F0400B0 8DC50008 */  lw    $a1, 8($t6)
/* 074BE4 7F0400B4 8DC60010 */  lw    $a2, 0x10($t6)
/* 074BE8 7F0400B8 8C840014 */  lw    $a0, 0x14($a0)
/* 074BEC 7F0400BC 0FC2CB0B */  jal   copy_tile_RGB_as_24bit
/* 074BF0 7F0400C0 AFA7001C */   sw    $a3, 0x1c($sp)
/* 074BF4 7F0400C4 8FA7001C */  lw    $a3, 0x1c($sp)
/* 074BF8 7F0400C8 3C013F40 */  li    $at, 0x3F400000 # 0.750000
/* 074BFC 7F0400CC 44814000 */  mtc1  $at, $f8
/* 074C00 7F0400D0 90EB0002 */  lbu   $t3, 2($a3)
/* 074C04 7F0400D4 90ED0000 */  lbu   $t5, ($a3)
/* 074C08 7F0400D8 90FF0001 */  lbu   $ra, 1($a3)
/* 074C0C 7F0400DC 000B7880 */  sll   $t7, $t3, 2
/* 074C10 7F0400E0 000DC080 */  sll   $t8, $t5, 2
/* 074C14 7F0400E4 01EB7821 */  addu  $t7, $t7, $t3
/* 074C18 7F0400E8 030DC021 */  addu  $t8, $t8, $t5
/* 074C1C 7F0400EC 001F7080 */  sll   $t6, $ra, 2
/* 074C20 7F0400F0 0018C100 */  sll   $t8, $t8, 4
/* 074C24 7F0400F4 000F7880 */  sll   $t7, $t7, 2
/* 074C28 7F0400F8 01DF7021 */  addu  $t6, $t6, $ra
/* 074C2C 7F0400FC 000E70C0 */  sll   $t6, $t6, 3
/* 074C30 7F040100 01EB7821 */  addu  $t7, $t7, $t3
/* 074C34 7F040104 030DC023 */  subu  $t8, $t8, $t5
/* 074C38 7F040108 01F8C821 */  addu  $t9, $t7, $t8
/* 074C3C 7F04010C 01DF7023 */  subu  $t6, $t6, $ra
/* 074C40 7F040110 000E7080 */  sll   $t6, $t6, 2
/* 074C44 7F040114 032E1821 */  addu  $v1, $t9, $t6
/* 074C48 7F040118 00037A03 */  sra   $t7, $v1, 8
/* 074C4C 7F04011C 241800FF */  li    $t8, 255
/* 074C50 7F040120 030FC823 */  subu  $t9, $t8, $t7
/* 074C54 7F040124 44992000 */  mtc1  $t9, $f4
/* 074C58 7F040128 240F0001 */  li    $t7, 1
/* 074C5C 7F04012C 3C014F00 */  li    $at, 0x4F000000 # 2147483648.000000
/* 074C60 7F040130 468021A0 */  cvt.s.w $f6, $f4
/* 074C64 7F040134 00001025 */  move  $v0, $zero
/* 074C68 7F040138 00006025 */  move  $t4, $zero
/* 074C6C 7F04013C 46083282 */  mul.s $f10, $f6, $f8
/* 074C70 7F040140 444EF800 */  cfc1  $t6, $31
/* 074C74 7F040144 44CFF800 */  ctc1  $t7, $31
/* 074C78 7F040148 00000000 */  nop
/* 074C7C 7F04014C 46005424 */  cvt.w.s $f16, $f10
/* 074C80 7F040150 444FF800 */  cfc1  $t7, $31
/* 074C84 7F040154 00000000 */  nop
/* 074C88 7F040158 31EF0078 */  andi  $t7, $t7, 0x78
/* 074C8C 7F04015C 51E00013 */  beql  $t7, $zero, .L7F0401AC
/* 074C90 7F040160 440F8000 */   mfc1  $t7, $f16
/* 074C94 7F040164 44818000 */  mtc1  $at, $f16
/* 074C98 7F040168 240F0001 */  li    $t7, 1
/* 074C9C 7F04016C 46105401 */  sub.s $f16, $f10, $f16
/* 074CA0 7F040170 44CFF800 */  ctc1  $t7, $31
/* 074CA4 7F040174 00000000 */  nop
/* 074CA8 7F040178 46008424 */  cvt.w.s $f16, $f16
/* 074CAC 7F04017C 444FF800 */  cfc1  $t7, $31
/* 074CB0 7F040180 00000000 */  nop
/* 074CB4 7F040184 31EF0078 */  andi  $t7, $t7, 0x78
/* 074CB8 7F040188 15E00005 */  bnez  $t7, .L7F0401A0
/* 074CBC 7F04018C 00000000 */   nop
/* 074CC0 7F040190 440F8000 */  mfc1  $t7, $f16
/* 074CC4 7F040194 3C018000 */  lui   $at, 0x8000
/* 074CC8 7F040198 10000007 */  b     .L7F0401B8
/* 074CCC 7F04019C 01E17825 */   or    $t7, $t7, $at
.L7F0401A0:
/* 074CD0 7F0401A0 10000005 */  b     .L7F0401B8
/* 074CD4 7F0401A4 240FFFFF */   li    $t7, -1
/* 074CD8 7F0401A8 440F8000 */  mfc1  $t7, $f16
.L7F0401AC:
/* 074CDC 7F0401AC 00000000 */  nop
/* 074CE0 7F0401B0 05E0FFFB */  bltz  $t7, .L7F0401A0
/* 074CE4 7F0401B4 00000000 */   nop
.L7F0401B8:
/* 074CE8 7F0401B8 44CEF800 */  ctc1  $t6, $31
/* 074CEC 7F0401BC 01BF082A */  slt   $at, $t5, $ra
/* 074CF0 7F0401C0 10200003 */  beqz  $at, .L7F0401D0
/* 074CF4 7F0401C4 A0EF0003 */   sb    $t7, 3($a3)
/* 074CF8 7F0401C8 10000002 */  b     .L7F0401D4
/* 074CFC 7F0401CC 24020001 */   li    $v0, 1
.L7F0401D0:
/* 074D00 7F0401D0 240C0001 */  li    $t4, 1
.L7F0401D4:
/* 074D04 7F0401D4 00E25021 */  addu  $t2, $a3, $v0
/* 074D08 7F0401D8 91460000 */  lbu   $a2, ($t2)
/* 074D0C 7F0401DC 00402825 */  move  $a1, $v0
/* 074D10 7F0401E0 00ECC021 */  addu  $t8, $a3, $t4
/* 074D14 7F0401E4 00CB082A */  slt   $at, $a2, $t3
/* 074D18 7F0401E8 50200005 */  beql  $at, $zero, .L7F040200
/* 074D1C 7F0401EC 93190000 */   lbu   $t9, ($t8)
/* 074D20 7F0401F0 24EA0002 */  addiu $t2, $a3, 2
/* 074D24 7F0401F4 10000009 */  b     .L7F04021C
/* 074D28 7F0401F8 91460000 */   lbu   $a2, ($t2)
/* 074D2C 7F0401FC 93190000 */  lbu   $t9, ($t8)
.L7F040200:
/* 074D30 7F040200 01802825 */  move  $a1, $t4
/* 074D34 7F040204 032B082A */  slt   $at, $t9, $t3
/* 074D38 7F040208 50200004 */  beql  $at, $zero, .L7F04021C
/* 074D3C 7F04020C 240C0002 */   li    $t4, 2
/* 074D40 7F040210 10000002 */  b     .L7F04021C
/* 074D44 7F040214 24050002 */   li    $a1, 2
/* 074D48 7F040218 240C0002 */  li    $t4, 2
.L7F04021C:
/* 074D4C 7F04021C 18C0001B */  blez  $a2, .L7F04028C
/* 074D50 7F040220 00E51821 */   addu  $v1, $a3, $a1
/* 074D54 7F040224 00EC1021 */  addu  $v0, $a3, $t4
/* 074D58 7F040228 904E0000 */  lbu   $t6, ($v0)
/* 074D5C 7F04022C 906F0000 */  lbu   $t7, ($v1)
/* 074D60 7F040230 A0400000 */  sb    $zero, ($v0)
/* 074D64 7F040234 00CE2023 */  subu  $a0, $a2, $t6
/* 074D68 7F040238 01E40019 */  multu $t7, $a0
/* 074D6C 7F04023C 00804825 */  move  $t1, $a0
/* 074D70 7F040240 0000C012 */  mflo  $t8
/* 074D74 7F040244 00000000 */  nop
/* 074D78 7F040248 00000000 */  nop
/* 074D7C 7F04024C 0306001A */  div   $zero, $t8, $a2
/* 074D80 7F040250 00004012 */  mflo  $t0
/* 074D84 7F040254 A0680000 */  sb    $t0, ($v1)
/* 074D88 7F040258 A1440000 */  sb    $a0, ($t2)
/* 074D8C 7F04025C 14C00002 */  bnez  $a2, .L7F040268
/* 074D90 7F040260 00000000 */   nop
/* 074D94 7F040264 0007000D */  break 7
.L7F040268:
/* 074D98 7F040268 2401FFFF */  li    $at, -1
/* 074D9C 7F04026C 14C10004 */  bne   $a2, $at, .L7F040280
/* 074DA0 7F040270 3C018000 */   lui   $at, 0x8000
/* 074DA4 7F040274 17010002 */  bne   $t8, $at, .L7F040280
/* 074DA8 7F040278 00000000 */   nop
/* 074DAC 7F04027C 0006000D */  break 6
.L7F040280:
/* 074DB0 7F040280 90FF0001 */  lbu   $ra, 1($a3)
/* 074DB4 7F040284 90ED0000 */  lbu   $t5, ($a3)
/* 074DB8 7F040288 90EB0002 */  lbu   $t3, 2($a3)
.L7F04028C:
/* 074DBC 7F04028C 000DC842 */  srl   $t9, $t5, 1
/* 074DC0 7F040290 001F7042 */  srl   $t6, $ra, 1
/* 074DC4 7F040294 000B7842 */  srl   $t7, $t3, 1
/* 074DC8 7F040298 A0F90000 */  sb    $t9, ($a3)
/* 074DCC 7F04029C A0EE0001 */  sb    $t6, 1($a3)
/* 074DD0 7F0402A0 A0EF0002 */  sb    $t7, 2($a3)
/* 074DD4 7F0402A4 8FBF0014 */  lw    $ra, 0x14($sp)
/* 074DD8 7F0402A8 27BD0018 */  addiu $sp, $sp, 0x18
/* 074DDC 7F0402AC 03E00008 */  jr    $ra
/* 074DE0 7F0402B0 00000000 */   nop
)
#endif


void sub_GAME_7F0402B4(PropRecord *prop, rgba_u8 *color)
{
    struct DoorRecord *door = prop->door;
    if (door->flags & 0x400 ){ return; }

    set_color_shading_from_tile(prop, color);
    color->r >>= 1;
    color->g >>= 1;
    color->b >>= 1;
}


void update_color_shading(rgba_u8 *dest, rgba_u8 *src)
{
    s32 val_diff;
    s32 val_new;
    s32 i;

    for (i = 0; i < 4; i++)
    {
        val_diff = (src->rgba[i] - dest->rgba[i]);
        val_new = dest->rgba[i] + ((val_diff + 7) >> 3);
        dest->rgba[i] = val_new;
    }
}


void sub_GAME_7F040384(rgba_s32* arg0, s32 arg1, rgba_f32* arg2)
{
    if (arg1 == 1)
    {
        arg2->r *= 255.0f;
        arg2->g *= 255.0f;
        arg2->b *= 255.0f;

        if (1) { arg0->r = (s32)((arg2->a * (arg2->r - arg0->r)) + arg0->r); }
        if (1) { arg0->g = (s32)((arg2->a * (arg2->g - arg0->g)) + arg0->g); }
        if (1) { arg0->b = (s32)((arg2->a * (arg2->b - arg0->b)) + arg0->b); }
        arg0->a = (s32)((arg2->a * (255.0f - arg0->a)) + arg0->a);
    }
}


/**
 * Address 0x7F040484.
*/
void chrobjCollisionRelated(ObjectRecord *obj)
{
    struct ModelRoData_BoundingBoxRecord *bbox;
    Mtxf sp24;

    if (obj->ptr_allocated_collisiondata_block != NULL)
    {
        bbox = chrobjGetBboxFromObjectRecord(obj);
        matrix_4x4_copy(&obj->mtx, &sp24);
        matrix_4x4_set_position(&obj->runtime_pos, &sp24);
        sub_GAME_7F03F540(bbox, &sp24, &obj->ptr_allocated_collisiondata_block->unk04, obj->ptr_allocated_collisiondata_block);

        obj->ptr_allocated_collisiondata_block->unk48 = obj->runtime_pos.f[1] + chrpropSumMatrixPosY(bbox, &sp24);
        obj->ptr_allocated_collisiondata_block->unk44 = obj->runtime_pos.f[1] + chrpropSumMatrixNegY(bbox, &sp24);

        if (obj->type == PROPDEF_AIRCRAFT)
        {
            obj->ptr_allocated_collisiondata_block->unk48 -= 200.0f;
        }
    }
}


PropRecord* objInit(ObjectRecord* obj, ModelFileHeader* model_header, PropRecord* prop, Model* model)
{
    if (prop == NULL)
    {
        prop = chrpropAllocate();
    }

    if (model == NULL)
    {
        if (obj->type == 0x28)
        {
            model = get_aircraft_obj_instance_controller(model_header);
        }
        else
        {
            model = get_obj_instance_controller_for_header(model_header);
        }
    }

    if ((prop != NULL) && (model != NULL))
    {
        obj->model = model;
        obj->ptr_allocated_collisiondata_block = NULL;

        if (obj->flags & 0x100)
        {
            obj->ptr_allocated_collisiondata_block = mempAllocBytesInBank(0x50U, MEMPOOL_STAGE);
            obj->state = (u8) (obj->state | 8);
        }
        else
        {
            obj->state = (u8) (obj->state & 0xFFF7);
        }

        obj->prop = prop;
        obj->projectile = NULL;

        obj->shadecol.r = 0;
        obj->shadecol.g = 0;
        obj->shadecol.b = 0;
        obj->shadecol.a = 0;

        obj->nextcol.r = 0;
        obj->nextcol.g = 0;
        obj->nextcol.b = 0;
        obj->nextcol.a = 0;

        obj->maxdamage = 0.0f;
        *((s16*)&obj->model->unk00) = -1;
        obj->model->chr = NULL;
        modelSetScale(obj->model, PitemZ_entries[obj->obj].scale);
        prop->type = 1;
        prop->obj = obj;
        prop->pos.x = 0.0f;
        obj->runtime_pos.x = 0.0f;
        prop->pos.y = 0.0f;
        obj->runtime_pos.y = 0.0f;
        prop->pos.z = 0.0f;
        obj->runtime_pos.z = 0.0f;
        prop->stan = NULL;
    }
    else
    {
        if (model != NULL)
        {
            if (obj->type == 0x28)
            {
                clear_aircraft_model_obj(model);
            }
            else
            {
                clear_model_obj(model);
            }
        }

        if (prop != NULL)
        {
            chrpropFree(prop);
            prop = NULL;
        }
    }

    return prop;
}


PropRecord* objInitWithModelDef(ObjectRecord* object, ModelFileHeader* header)
{
  return objInit(object, header, 0, 0);
}


PropRecord* objInitWithAutoModel(ObjectRecord* obj)
{
    return objInitWithModelDef(obj, PitemZ_entries[obj->obj].header);
}


// Changes the color shade on the object, e.g. when walking in a darker area or under a colored light.
void objChangeShading(ObjectRecord* obj, coord3d* pos, Mtxf* matrix, StandTile* stan) {

    PropRecord *prop = obj->prop;

    matrix_4x4_copy(matrix, &obj->mtx);

    obj->runtime_pos.x = prop->pos.x = pos->x;
    obj->runtime_pos.y = prop->pos.y = pos->y;
    obj->runtime_pos.z = prop->pos.z = pos->z;

    prop->stan = stan;

    sub_GAME_7F0402B4(obj->prop, &obj->nextcol);

    obj->shadecol.r = obj->nextcol.r;
    obj->shadecol.g = obj->nextcol.g;
    obj->shadecol.b = obj->nextcol.b;
    obj->shadecol.a = obj->nextcol.a;
}


// Unreferenced function (unused)
void sub_GAME_7F0407F4(ObjectRecord* obj, coord3d* pos, Mtxf* matrix, StandTile* stan)
{
    u32 a; // Adds 4 bytes to the stack so it matches. Could be anything 4 bytes long.
    struct ModelRoData_BoundingBoxRecord *modelunk = chrobjGetBboxFromObjFile(obj->model->obj);

    pos->y = stanGetPositionYValue(stan, pos->x, pos->z) + 4.0f;
    pos->y = pos->y - chrpropSumMatrixPosY(modelunk, matrix);

    objChangeShading(obj, pos, matrix, stan);
    chrobjCollisionRelated(obj);
}


//moveToPad
void sub_GAME_7F04088C(ObjectRecord *baseobj, struct coord3d *pos, Mtxf *matrix, StandTile *stan, struct coord3d *pos2)
{
    int padd[1];
    ModelRoData_BoundingBoxRecord *modelBoundingBox;
    f32 xmax;
    f32 ymin;
    coord3d newPos;
    StandTile *mStan;
    Mtxf mtxcopy;

    modelBoundingBox = chrobjGetBboxFromObjFile(baseobj->model->obj);
    xmax = chrpropBBOXGetYmin(modelBoundingBox);
    ymin = chrpropBBOXGetYmax(modelBoundingBox);
    mStan = stan;

    if (baseobj->flags & 4)
    {
        matrix_4x4_set_rotation_around_z(M_PI, &mtxcopy);
        matrix_4x4_multiply_in_place(matrix, &mtxcopy);
        newPos.x = pos2->f[0] - (mtxcopy.m[1][0] * ymin);
        newPos.y = pos2->f[1] - (mtxcopy.m[1][1] * ymin);
        newPos.z = pos2->f[2] - (mtxcopy.m[1][2] * ymin);
    }
    else if (baseobj->flags & 8)
    {
        matrix_4x4_copy(matrix, &mtxcopy);
        newPos.x = pos2->f[0] - (mtxcopy.m[1][0] * xmax);
        newPos.y = pos2->f[1] - (mtxcopy.m[1][1] * xmax);
        newPos.z = pos2->f[2] - (mtxcopy.m[1][2] * xmax);
    }
    else
    {
        ObjectRecord *roomObj;
        f32 distfromTileCenter;
        f32 byrefA;
        f32 byrefB;
        f32 byrefC;
        f32 byrefD;

        distfromTileCenter = stanGetPositionYValue(mStan, pos->f[0], pos->f[2]);

        matrix_4x4_copy(matrix, &mtxcopy);
        newPos.x = pos2->f[0] - (mtxcopy.m[1][0] * xmax);
        newPos.z = pos2->f[2] - (mtxcopy.m[1][2] * xmax);
        roomObj  = sub_GAME_7F03FAB0(pos, stan->room);

        if (roomObj)
        {
            PropRecord *roomObjProp = roomObj->prop;
            chraiGetCollisionBounds(roomObjProp, &byrefA, &byrefB, &byrefC, &byrefD);

            if ((distfromTileCenter < byrefC) && (byrefD < ((mtxcopy.m[1][1] * (ymin - xmax)) + distfromTileCenter + 4.0f)))
            {
                newPos.y = byrefC - (mtxcopy.m[1][1] * xmax);
                baseobj->runtime_bitflags |= RUNTIMEBITFLAG_00008000;
            }
            else
            {
                newPos.y = (distfromTileCenter - (mtxcopy.m[1][1] * xmax)) + 4.0f;
            }
        }
        else
        {
            newPos.y = (distfromTileCenter - (mtxcopy.m[1][1] * xmax)) + 4.0f;
        }
    }

    if (!(baseobj->flags2 & 1) && walkTilesBetweenPoints_NoCallback(&mStan, pos->f[0], pos->f[2], newPos.x, newPos.z))
    {
        objChangeShading(baseobj, &newPos, &mtxcopy, mStan);
    }
    else
    {
        objChangeShading(baseobj, pos, &mtxcopy, stan);
        if ((baseobj->flags2 & 1) || (baseobj->flags & 0x1000))
        {
            baseobj->runtime_pos.x = newPos.x;
            baseobj->runtime_pos.y = newPos.y;
            baseobj->runtime_pos.z = newPos.z;
        }
        #ifdef DEBUG
        else
        {
            osSyncPrintf("prop not positioned correctly!\n");
        }
        #endif
    }

    chrobjCollisionRelated(baseobj);
}


void sub_GAME_7F040BA0(ObjectRecord *obj, coord3d *pos, Mtxf *arg2, StandTile *stan2, coord3d *pos2)
{
    Mtxf *sp6C_ptr;
    f32 (*sp6Cm_ptr)[4];
    f32 spBC;
    coord3d posdiff;
    StandTile *stan;
    Mtxf matrix;
    Mtxf sp2C;

    spBC = chrpropBBOXGetZmin(chrobjGetBboxFromObjFile(obj->model->obj));
    stan = stan2;
    sp6C_ptr = &matrix;

    matrix_4x4_set_rotation_around_x(4.712389f, sp6C_ptr);

    sp6Cm_ptr = matrix.m;

    matrix_4x4_set_rotation_around_y(M_PI_F, &sp2C);
    matrix_4x4_multiply_in_place(&sp2C, sp6C_ptr);
    matrix_4x4_multiply_in_place(arg2, &matrix);

    posdiff.x = pos2->x - (sp6Cm_ptr[2][0] * spBC);
    posdiff.y = pos2->y - (sp6Cm_ptr[2][1] * spBC);
    posdiff.z = pos2->z - (sp6Cm_ptr[2][2] * spBC);

    if ((!(((s32) obj->flags2) & 1)) && (walkTilesBetweenPoints_NoCallback(&stan, pos->x, pos->z, posdiff.x, posdiff.z) != 0))
    {
        objChangeShading(obj, &posdiff, &matrix, stan);
    }
    else
    {
        objChangeShading(obj, pos, &matrix, stan2);
        obj->runtime_pos.x = posdiff.x;
        obj->runtime_pos.y = posdiff.y;
        obj->runtime_pos.z = posdiff.z;
    }

    chrobjCollisionRelated(obj);
}


void objFreeEmbedmentOrProjectile(PropRecord *prop)
{
    ObjectRecord *obj = prop->obj;
    if (obj->runtime_bitflags & RUNTIMEBITFLAG_EMBEDDED)
    {
        if (obj->embedment)
        {
            if (obj->embedment->projectile)
            {
                projectileFree(obj->embedment->projectile);
            }
            #ifdef DEBUG
            else
            {
                osSyncPrintf("ERROR: PROPHIDD_ATTACHED was, but move.attach was NULL\a\n");
                osSyncPrintf("po->obj=%d\n",po->obj);
                osSyncPrintf("p->flags=%08x\n",p->flags);
                osSyncPrintf("po->flags2=%08x\n",po->flags2);
                osSyncPrintf("p->timetoregen=%d\n",p->timetoregen);
            }
            #endif

            embedmentFree(obj->embedment);
        }
        obj->embedment = NULL;
        obj->runtime_bitflags &= ~RUNTIMEBITFLAG_EMBEDDED;
    }
    else if (obj->runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT)
    {
        projectileFree(obj->projectile);
        obj->projectile = NULL;
        obj->runtime_bitflags &= ~RUNTIMEBITFLAG_DEPOSIT;
    }
}


void objFree(ObjectRecord* obj, s32 freeprop, s32 canregen)
{
    PropRecord *child;

    if (obj->type == PROPDEF_AUTOGUN)
    {
        AutogunRecord* record = (AutogunRecord*)obj;
        if ((record->unkC4 != NULL) && (sndGetPlayingState(record->unkC4) != 0))
        {
            sndDeactivate(record->unkC4);
        }

        if ((record->unkC8 != NULL) && (sndGetPlayingState(record->unkC8) != 0))
        {
            sndDeactivate(record->unkC8);
        }
    }
    else if (obj->type == PROPDEF_COLLECTABLE)
    {
        WeaponObjRecord* record = (WeaponObjRecord*) obj;
        WeaponObjRecord* record2 = record->dualweapon;
        if (record2 != NULL)
        {
            record2->dualweapon = NULL;
            record->dualweapon = NULL;
        }
    }
    else if (obj->type == PROPDEF_DOOR)
    {
        DoorRecord* record = (DoorRecord*) obj;
        if ((record->openSoundState != NULL) && (sndGetPlayingState(record->openSoundState) != 0))
        {
            sndDeactivate(record->openSoundState);
        }

        if ((record->closeSoundState != NULL) && (sndGetPlayingState(record->closeSoundState) != 0))
        {
            sndDeactivate(record->closeSoundState);
        }
    }
    else if (obj->type == PROPDEF_TINTED_GLASS)
    {
        TintedGlassRecord* record = (TintedGlassRecord*) obj;
        if (record->portalnum >= 0)
        {
            bgToggleDataPortalsContrlBytes1Bit1(record->portalnum, 1);
        }
    }
    else if (obj->type == PROPDEF_AIRCRAFT)
    {
        AircraftRecord* record = (AircraftRecord*) obj;
        if ((record->Sound != NULL) && (sndGetPlayingState(record->Sound) != 0))
        {
            sndDeactivate(record->Sound);
        }
    }
    else if (obj->type == PROPDEF_VEHICHLE)
    {
        VehichleRecord* record = (VehichleRecord*) obj;
        if ((record->Sound != NULL) && (sndGetPlayingState(record->Sound) != 0))
        {
            sndDeactivate(record->Sound);
        }
    }

    if (obj->prop != NULL)
    {
        explosionClearBulletImpactRoomByFlag(obj->prop, FALSE);
        explosionClearBulletImpactRoomByFlag(obj->prop, TRUE);

        if (canregen == 0)
        {
            objFreeEmbedmentOrProjectile(obj->prop);

            if (obj->prop->parent != NULL)
            {
                objDetach(obj->prop);
            }

            chrpropDeregisterRooms(obj->prop);

            child = obj->prop->child;
            while (child)
            {
                PropRecord* next = child->prev;
                objFreePermanently(child->obj, TRUE);
                child = next;
            }

            if (obj->prop->type != PROP_TYPE_DOOR)
            {
                sub_GAME_7F050DE8(obj->model);
            }

            if (obj->type == PROPDEF_AIRCRAFT)
            {
                clear_aircraft_model_obj(obj->model);
            }
            else
            {
                clear_model_obj(obj->model);
            }

            if (freeprop != 0)
            {
                chrpropDelist(obj->prop);
                chrpropDisable(obj->prop);
                chrpropFree(obj->prop);
            }

            obj->prop = NULL;
        }
    }
}


void objFreePermanently(struct ObjectRecord * obj, bool freeprop)
{
    objFree(obj, freeprop, 0);
}


float objGetWidth(struct ObjectRecord * obj)
{
    if (obj->type == PROP_TYPE_WEAPON)
    {
        return 20.0f;
    }
    return 10.0f;
}


bool sub_GAME_7F041074(coord3d *zeropos, coord3d *pos, coord3d *vec, f32 scale)
{
    vec3d vector;
    f32 dist2rd;

    vector.x = vec->x - zeropos->x;
    vector.y = vec->y - zeropos->y;
    vector.z = vec->z - zeropos->z;

    dist2rd = pos->f[0] * vector.f[0] + pos->f[1] * vector.f[1] + pos->f[2] * vector.f[2];

    if (dist2rd > 0) {
        f32 a = pos->f[0] * pos->f[0] + pos->f[1] * pos->f[1] + pos->f[2] * pos->f[2];
        f32 b = vector.f[0] * vector.f[0] + vector.f[1] * vector.f[1] + vector.f[2] * vector.f[2];

        if ((b - scale * scale) * a <= dist2rd * dist2rd) {
            return 1;
        }
    }

    return 0;
}





#ifdef NONMATCHING
void sub_GAME_7F041160(void) {

}
#else
GLOBAL_ASM(
.text
glabel sub_GAME_7F041160
/* 075C90 7F041160 27BDFF58 */  addiu $sp, $sp, -0xa8
/* 075C94 7F041164 AFBF0044 */  sw    $ra, 0x44($sp)
/* 075C98 7F041168 AFB70040 */  sw    $s7, 0x40($sp)
/* 075C9C 7F04116C AFB6003C */  sw    $s6, 0x3c($sp)
/* 075CA0 7F041170 AFB50038 */  sw    $s5, 0x38($sp)
/* 075CA4 7F041174 AFB40034 */  sw    $s4, 0x34($sp)
/* 075CA8 7F041178 AFB30030 */  sw    $s3, 0x30($sp)
/* 075CAC 7F04117C AFB2002C */  sw    $s2, 0x2c($sp)
/* 075CB0 7F041180 AFB10028 */  sw    $s1, 0x28($sp)
/* 075CB4 7F041184 AFB00024 */  sw    $s0, 0x24($sp)
/* 075CB8 7F041188 AFA700B4 */  sw    $a3, 0xb4($sp)
/* 075CBC 7F04118C 8C970014 */  lw    $s7, 0x14($a0)
/* 075CC0 7F041190 AFA0009C */  sw    $zero, 0x9c($sp)
/* 075CC4 7F041194 AFA00064 */  sw    $zero, 0x64($sp)
/* 075CC8 7F041198 908E0003 */  lbu   $t6, 3($a0)
/* 075CCC 7F04119C 24010001 */  li    $at, 1
/* 075CD0 7F0411A0 00A08825 */  move  $s1, $a1
/* 075CD4 7F0411A4 15C1001A */  bne   $t6, $at, .L7F041210
/* 075CD8 7F0411A8 00C09825 */   move  $s3, $a2
/* 075CDC 7F0411AC 248400D0 */  addiu $a0, $a0, 0xd0
/* 075CE0 7F0411B0 8EE5000C */  lw    $a1, 0xc($s7)
/* 075CE4 7F0411B4 02203025 */  move  $a2, $s1
/* 075CE8 7F0411B8 0FC1D1F4 */  jal   sub_GAME_7F0747D0
/* 075CEC 7F0411BC 02603825 */   move  $a3, $s3
/* 075CF0 7F0411C0 8EEF0008 */  lw    $t7, 8($s7)
/* 075CF4 7F0411C4 00408025 */  move  $s0, $v0
/* 075CF8 7F0411C8 27B4006C */  addiu $s4, $sp, 0x6c
/* 075CFC 7F0411CC 8DF80000 */  lw    $t8, ($t7)
/* 075D00 7F0411D0 27B50068 */  addiu $s5, $sp, 0x68
/* 075D04 7F0411D4 18400025 */  blez  $v0, .L7F04126C
/* 075D08 7F0411D8 AFB8009C */   sw    $t8, 0x9c($sp)
/* 075D0C 7F0411DC 27B60064 */  addiu $s6, $sp, 0x64
/* 075D10 7F0411E0 AFB60018 */  sw    $s6, 0x18($sp)
/* 075D14 7F0411E4 02E02025 */  move  $a0, $s7
/* 075D18 7F0411E8 03002825 */  move  $a1, $t8
/* 075D1C 7F0411EC 02203025 */  move  $a2, $s1
/* 075D20 7F0411F0 02603825 */  move  $a3, $s3
/* 075D24 7F0411F4 AFB40010 */  sw    $s4, 0x10($sp)
/* 075D28 7F0411F8 0FC1366C */  jal   sub_GAME_7F04D9B0
/* 075D2C 7F0411FC AFB50014 */   sw    $s5, 0x14($sp)
/* 075D30 7F041200 1440001A */  bnez  $v0, .L7F04126C
/* 075D34 7F041204 00000000 */   nop
/* 075D38 7F041208 10000018 */  b     .L7F04126C
/* 075D3C 7F04120C 00008025 */   move  $s0, $zero
.L7F041210:
/* 075D40 7F041210 27B60064 */  addiu $s6, $sp, 0x64
/* 075D44 7F041214 27B50068 */  addiu $s5, $sp, 0x68
/* 075D48 7F041218 27B4006C */  addiu $s4, $sp, 0x6c
/* 075D4C 7F04121C 27B2009C */  addiu $s2, $sp, 0x9c
/* 075D50 7F041220 02E02025 */  move  $a0, $s7
.L7F041224:
/* 075D54 7F041224 02202825 */  move  $a1, $s1
/* 075D58 7F041228 02603025 */  move  $a2, $s3
/* 075D5C 7F04122C 0FC1D4BF */  jal   sub_GAME_7F0752FC
/* 075D60 7F041230 02403825 */   move  $a3, $s2
/* 075D64 7F041234 1840000B */  blez  $v0, .L7F041264
/* 075D68 7F041238 00408025 */   move  $s0, $v0
/* 075D6C 7F04123C 02E02025 */  move  $a0, $s7
/* 075D70 7F041240 8FA5009C */  lw    $a1, 0x9c($sp)
/* 075D74 7F041244 02203025 */  move  $a2, $s1
/* 075D78 7F041248 02603825 */  move  $a3, $s3
/* 075D7C 7F04124C AFB40010 */  sw    $s4, 0x10($sp)
/* 075D80 7F041250 AFB50014 */  sw    $s5, 0x14($sp)
/* 075D84 7F041254 0FC1366C */  jal   sub_GAME_7F04D9B0
/* 075D88 7F041258 AFB60018 */   sw    $s6, 0x18($sp)
/* 075D8C 7F04125C 14400003 */  bnez  $v0, .L7F04126C
/* 075D90 7F041260 00000000 */   nop
.L7F041264:
/* 075D94 7F041264 5E00FFEF */  bgtzl $s0, .L7F041224
/* 075D98 7F041268 02E02025 */   move  $a0, $s7
.L7F04126C:
/* 075D9C 7F04126C 1A000059 */  blez  $s0, .L7F0413D4
/* 075DA0 7F041270 00001025 */   move  $v0, $zero
/* 075DA4 7F041274 C7A4006C */  lwc1  $f4, 0x6c($sp)
/* 075DA8 7F041278 8FA500B4 */  lw    $a1, 0xb4($sp)
/* 075DAC 7F04127C 8EF9000C */  lw    $t9, 0xc($s7)
/* 075DB0 7F041280 8FA80068 */  lw    $t0, 0x68($sp)
/* 075DB4 7F041284 E4A40000 */  swc1  $f4, ($a1)
/* 075DB8 7F041288 C7A60070 */  lwc1  $f6, 0x70($sp)
/* 075DBC 7F04128C 00084980 */  sll   $t1, $t0, 6
/* 075DC0 7F041290 03298821 */  addu  $s1, $t9, $t1
/* 075DC4 7F041294 E4A60004 */  swc1  $f6, 4($a1)
/* 075DC8 7F041298 C7A80074 */  lwc1  $f8, 0x74($sp)
/* 075DCC 7F04129C 02202025 */  move  $a0, $s1
/* 075DD0 7F0412A0 0FC1611D */  jal   mtx4TransformVecInPlace
/* 075DD4 7F0412A4 E4A80008 */   swc1  $f8, 8($a1)
/* 075DD8 7F0412A8 0FC1E111 */  jal   currentPlayerGetMatrix10D4
/* 075DDC 7F0412AC 00000000 */   nop
/* 075DE0 7F0412B0 00402025 */  move  $a0, $v0
/* 075DE4 7F0412B4 0FC1611D */  jal   mtx4TransformVecInPlace
/* 075DE8 7F0412B8 8FA500B4 */   lw    $a1, 0xb4($sp)
/* 075DEC 7F0412BC 8FB000B8 */  lw    $s0, 0xb8($sp)
/* 075DF0 7F0412C0 C7AA0078 */  lwc1  $f10, 0x78($sp)
/* 075DF4 7F0412C4 02202025 */  move  $a0, $s1
/* 075DF8 7F0412C8 02002825 */  move  $a1, $s0
/* 075DFC 7F0412CC E60A0000 */  swc1  $f10, ($s0)
/* 075E00 7F0412D0 C7B0007C */  lwc1  $f16, 0x7c($sp)
/* 075E04 7F0412D4 E6100004 */  swc1  $f16, 4($s0)
/* 075E08 7F0412D8 C7B20080 */  lwc1  $f18, 0x80($sp)
/* 075E0C 7F0412DC 0FC160F6 */  jal   mtx4RotateVecInPlace
/* 075E10 7F0412E0 E6120008 */   swc1  $f18, 8($s0)
/* 075E14 7F0412E4 C6020000 */  lwc1  $f2, ($s0)
/* 075E18 7F0412E8 C6640000 */  lwc1  $f4, ($s3)
/* 075E1C 7F0412EC C60C0004 */  lwc1  $f12, 4($s0)
/* 075E20 7F0412F0 C6680004 */  lwc1  $f8, 4($s3)
/* 075E24 7F0412F4 46041182 */  mul.s $f6, $f2, $f4
/* 075E28 7F0412F8 C6000008 */  lwc1  $f0, 8($s0)
/* 075E2C 7F0412FC C6720008 */  lwc1  $f18, 8($s3)
/* 075E30 7F041300 46086282 */  mul.s $f10, $f12, $f8
/* 075E34 7F041304 44807000 */  mtc1  $zero, $f14
/* 075E38 7F041308 46009102 */  mul.s $f4, $f18, $f0
/* 075E3C 7F04130C 460A3400 */  add.s $f16, $f6, $f10
/* 075E40 7F041310 46102200 */  add.s $f8, $f4, $f16
/* 075E44 7F041314 4608703C */  c.lt.s $f14, $f8
/* 075E48 7F041318 00000000 */  nop
/* 075E4C 7F04131C 45000007 */  bc1f  .L7F04133C
/* 075E50 7F041320 00000000 */   nop
/* 075E54 7F041324 46001187 */  neg.s $f6, $f2
/* 075E58 7F041328 46006287 */  neg.s $f10, $f12
/* 075E5C 7F04132C 46000487 */  neg.s $f18, $f0
/* 075E60 7F041330 E6060000 */  swc1  $f6, ($s0)
/* 075E64 7F041334 E60A0004 */  swc1  $f10, 4($s0)
/* 075E68 7F041338 E6120008 */  swc1  $f18, 8($s0)
.L7F04133C:
/* 075E6C 7F04133C 0FC1E111 */  jal   currentPlayerGetMatrix10D4
/* 075E70 7F041340 00000000 */   nop
/* 075E74 7F041344 00402025 */  move  $a0, $v0
/* 075E78 7F041348 0FC160F6 */  jal   mtx4RotateVecInPlace
/* 075E7C 7F04134C 02002825 */   move  $a1, $s0
/* 075E80 7F041350 44807000 */  mtc1  $zero, $f14
/* 075E84 7F041354 C6040000 */  lwc1  $f4, ($s0)
/* 075E88 7F041358 02002025 */  move  $a0, $s0
/* 075E8C 7F04135C 26050004 */  addiu $a1, $s0, 4
/* 075E90 7F041360 46047032 */  c.eq.s $f14, $f4
/* 075E94 7F041364 00000000 */  nop
/* 075E98 7F041368 4500000C */  bc1f  .L7F04139C
/* 075E9C 7F04136C 00000000 */   nop
/* 075EA0 7F041370 C6100004 */  lwc1  $f16, 4($s0)
/* 075EA4 7F041374 46107032 */  c.eq.s $f14, $f16
/* 075EA8 7F041378 00000000 */  nop
/* 075EAC 7F04137C 45000007 */  bc1f  .L7F04139C
/* 075EB0 7F041380 00000000 */   nop
/* 075EB4 7F041384 C6080008 */  lwc1  $f8, 8($s0)
/* 075EB8 7F041388 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 075EBC 7F04138C 46087032 */  c.eq.s $f14, $f8
/* 075EC0 7F041390 00000000 */  nop
/* 075EC4 7F041394 45030006 */  bc1tl .L7F0413B0
/* 075EC8 7F041398 44813000 */   mtc1  $at, $f6
.L7F04139C:
/* 075ECC 7F04139C 0C007DD4 */  jal   guNormalize
/* 075ED0 7F0413A0 26060008 */   addiu $a2, $s0, 8
/* 075ED4 7F0413A4 10000005 */  b     .L7F0413BC
/* 075ED8 7F0413A8 8FAB00BC */   lw    $t3, 0xbc($sp)
/* 075EDC 7F0413AC 44813000 */  mtc1  $at, $f6
.L7F0413B0:
/* 075EE0 7F0413B0 00000000 */  nop
/* 075EE4 7F0413B4 E6060008 */  swc1  $f6, 8($s0)
/* 075EE8 7F0413B8 8FAB00BC */  lw    $t3, 0xbc($sp)
.L7F0413BC:
/* 075EEC 7F0413BC 24020001 */  li    $v0, 1
/* 075EF0 7F0413C0 AD770000 */  sw    $s7, ($t3)
/* 075EF4 7F0413C4 8FAD00C0 */  lw    $t5, 0xc0($sp)
/* 075EF8 7F0413C8 8FAC0064 */  lw    $t4, 0x64($sp)
/* 075EFC 7F0413CC 10000001 */  b     .L7F0413D4
/* 075F00 7F0413D0 ADAC0000 */   sw    $t4, ($t5)
.L7F0413D4:
/* 075F04 7F0413D4 8FBF0044 */  lw    $ra, 0x44($sp)
/* 075F08 7F0413D8 8FB00024 */  lw    $s0, 0x24($sp)
/* 075F0C 7F0413DC 8FB10028 */  lw    $s1, 0x28($sp)
/* 075F10 7F0413E0 8FB2002C */  lw    $s2, 0x2c($sp)
/* 075F14 7F0413E4 8FB30030 */  lw    $s3, 0x30($sp)
/* 075F18 7F0413E8 8FB40034 */  lw    $s4, 0x34($sp)
/* 075F1C 7F0413EC 8FB50038 */  lw    $s5, 0x38($sp)
/* 075F20 7F0413F0 8FB6003C */  lw    $s6, 0x3c($sp)
/* 075F24 7F0413F4 8FB70040 */  lw    $s7, 0x40($sp)
/* 075F28 7F0413F8 03E00008 */  jr    $ra
/* 075F2C 7F0413FC 27BD00A8 */   addiu $sp, $sp, 0xa8
)
#endif





#ifdef NONMATCHING
// PD: func0f06b488
bool sub_GAME_7F041400(PropRecord *prop, coord3d *arg1, coord3d *arg2, coord3d *arg3, coord3d *arg4, coord3d *arg5, f32 *arg6) {

}
#else
bool sub_GAME_7F041400(PropRecord *prop, coord3d *arg1, coord3d *arg2, coord3d *arg3, coord3d *arg4, coord3d *arg5, f32 *arg6);
GLOBAL_ASM(
.text
glabel sub_GAME_7F041400
/* 075F30 7F041400 27BDFF28 */  addiu $sp, $sp, -0xd8
/* 075F34 7F041404 AFB50044 */  sw    $s5, 0x44($sp)
/* 075F38 7F041408 AFB40040 */  sw    $s4, 0x40($sp)
/* 075F3C 7F04140C F7B40028 */  sdc1  $f20, 0x28($sp)
/* 075F40 7F041410 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 075F44 7F041414 00C0A025 */  move  $s4, $a2
/* 075F48 7F041418 00A0A825 */  move  $s5, $a1
/* 075F4C 7F04141C AFBF0054 */  sw    $ra, 0x54($sp)
/* 075F50 7F041420 AFA700E4 */  sw    $a3, 0xe4($sp)
/* 075F54 7F041424 4481A000 */  mtc1  $at, $f20
/* 075F58 7F041428 240EFFFF */  li    $t6, -1
/* 075F5C 7F04142C 27AF00C8 */  addiu $t7, $sp, 0xc8
/* 075F60 7F041430 AFBE0050 */  sw    $fp, 0x50($sp)
/* 075F64 7F041434 AFB7004C */  sw    $s7, 0x4c($sp)
/* 075F68 7F041438 AFB60048 */  sw    $s6, 0x48($sp)
/* 075F6C 7F04143C AFB3003C */  sw    $s3, 0x3c($sp)
/* 075F70 7F041440 AFB20038 */  sw    $s2, 0x38($sp)
/* 075F74 7F041444 AFB10034 */  sw    $s1, 0x34($sp)
/* 075F78 7F041448 AFB00030 */  sw    $s0, 0x30($sp)
/* 075F7C 7F04144C AFA400D8 */  sw    $a0, 0xd8($sp)
/* 075F80 7F041450 AFAE0094 */  sw    $t6, 0x94($sp)
/* 075F84 7F041454 AFAF0010 */  sw    $t7, 0x10($sp)
/* 075F88 7F041458 27A700CC */  addiu $a3, $sp, 0xcc
/* 075F8C 7F04145C 27A500D4 */  addiu $a1, $sp, 0xd4
/* 075F90 7F041460 0FC0F308 */  jal   chraiGetCollisionBounds
/* 075F94 7F041464 27A600D0 */   addiu $a2, $sp, 0xd0
/* 075F98 7F041468 8FB800D0 */  lw    $t8, 0xd0($sp)
/* 075F9C 7F04146C C7A400CC */  lwc1  $f4, 0xcc($sp)
/* 075FA0 7F041470 5B0000CD */  blezl $t8, .L7F0417A8
/* 075FA4 7F041474 00001025 */   move  $v0, $zero
/* 075FA8 7F041478 C6A00004 */  lwc1  $f0, 4($s5)
/* 075FAC 7F04147C C7A800C8 */  lwc1  $f8, 0xc8($sp)
/* 075FB0 7F041480 4600203C */  c.lt.s $f4, $f0
/* 075FB4 7F041484 00000000 */  nop
/* 075FB8 7F041488 45020007 */  bc1fl .L7F0414A8
/* 075FBC 7F04148C 4608003C */   c.lt.s $f0, $f8
/* 075FC0 7F041490 C6860004 */  lwc1  $f6, 4($s4)
/* 075FC4 7F041494 4606203C */  c.lt.s $f4, $f6
/* 075FC8 7F041498 00000000 */  nop
/* 075FCC 7F04149C 450300C2 */  bc1tl .L7F0417A8
/* 075FD0 7F0414A0 00001025 */   move  $v0, $zero
/* 075FD4 7F0414A4 4608003C */  c.lt.s $f0, $f8
.L7F0414A8:
/* 075FD8 7F0414A8 00000000 */  nop
/* 075FDC 7F0414AC 45020007 */  bc1fl .L7F0414CC
/* 075FE0 7F0414B0 C6B00000 */   lwc1  $f16, ($s5)
/* 075FE4 7F0414B4 C68A0004 */  lwc1  $f10, 4($s4)
/* 075FE8 7F0414B8 4608503C */  c.lt.s $f10, $f8
/* 075FEC 7F0414BC 00000000 */  nop
/* 075FF0 7F0414C0 450300B9 */  bc1tl .L7F0417A8
/* 075FF4 7F0414C4 00001025 */   move  $v0, $zero
/* 075FF8 7F0414C8 C6B00000 */  lwc1  $f16, ($s5)
.L7F0414CC:
/* 075FFC 7F0414CC 8FB900D0 */  lw    $t9, 0xd0($sp)
/* 076000 7F0414D0 00008025 */  move  $s0, $zero
/* 076004 7F0414D4 E7B000A0 */  swc1  $f16, 0xa0($sp)
/* 076008 7F0414D8 C6B20008 */  lwc1  $f18, 8($s5)
/* 07600C 7F0414DC E7B200A4 */  swc1  $f18, 0xa4($sp)
/* 076010 7F0414E0 C6840000 */  lwc1  $f4, ($s4)
/* 076014 7F0414E4 E7A40098 */  swc1  $f4, 0x98($sp)
/* 076018 7F0414E8 C6860008 */  lwc1  $f6, 8($s4)
/* 07601C 7F0414EC 1B200043 */  blez  $t9, .L7F0415FC
/* 076020 7F0414F0 E7A6009C */   swc1  $f6, 0x9c($sp)
/* 076024 7F0414F4 00008825 */  move  $s1, $zero
/* 076028 7F0414F8 27BE00B0 */  addiu $fp, $sp, 0xb0
/* 07602C 7F0414FC 27B70098 */  addiu $s7, $sp, 0x98
/* 076030 7F041500 27B600A0 */  addiu $s6, $sp, 0xa0
/* 076034 7F041504 8FA800D0 */  lw    $t0, 0xd0($sp)
.L7F041508:
/* 076038 7F041508 26130001 */  addiu $s3, $s0, 1
/* 07603C 7F04150C 8FA900D4 */  lw    $t1, 0xd4($sp)
/* 076040 7F041510 0268001A */  div   $zero, $s3, $t0
/* 076044 7F041514 01311821 */  addu  $v1, $t1, $s1
/* 076048 7F041518 C46A0000 */  lwc1  $f10, ($v1)
/* 07604C 7F04151C C6AC0000 */  lwc1  $f12, ($s5)
/* 076050 7F041520 C6AE0008 */  lwc1  $f14, 8($s5)
/* 076054 7F041524 8E860000 */  lw    $a2, ($s4)
/* 076058 7F041528 8E870008 */  lw    $a3, 8($s4)
/* 07605C 7F04152C E7AA0010 */  swc1  $f10, 0x10($sp)
/* 076060 7F041530 C4680004 */  lwc1  $f8, 4($v1)
/* 076064 7F041534 00001010 */  mfhi  $v0
/* 076068 7F041538 000250C0 */  sll   $t2, $v0, 3
/* 07606C 7F04153C 012A2021 */  addu  $a0, $t1, $t2
/* 076070 7F041540 E7A80014 */  swc1  $f8, 0x14($sp)
/* 076074 7F041544 C4900000 */  lwc1  $f16, ($a0)
/* 076078 7F041548 15000002 */  bnez  $t0, .L7F041554
/* 07607C 7F04154C 00000000 */   nop
/* 076080 7F041550 0007000D */  break 7
.L7F041554:
/* 076084 7F041554 2401FFFF */  li    $at, -1
/* 076088 7F041558 15010004 */  bne   $t0, $at, .L7F04156C
/* 07608C 7F04155C 3C018000 */   lui   $at, 0x8000
/* 076090 7F041560 16610002 */  bne   $s3, $at, .L7F04156C
/* 076094 7F041564 00000000 */   nop
/* 076098 7F041568 0006000D */  break 6
.L7F04156C:
/* 07609C 7F04156C E7B00018 */  swc1  $f16, 0x18($sp)
/* 0760A0 7F041570 C4920004 */  lwc1  $f18, 4($a0)
/* 0760A4 7F041574 00409025 */  move  $s2, $v0
/* 0760A8 7F041578 0FC2C1A2 */  jal   sub_GAME_7F0B0688
/* 0760AC 7F04157C E7B2001C */   swc1  $f18, 0x1c($sp)
/* 0760B0 7F041580 50400019 */  beql  $v0, $zero, .L7F0415E8
/* 0760B4 7F041584 8FAD00D0 */   lw    $t5, 0xd0($sp)
/* 0760B8 7F041588 8FAB00D4 */  lw    $t3, 0xd4($sp)
/* 0760BC 7F04158C 001260C0 */  sll   $t4, $s2, 3
/* 0760C0 7F041590 02C02025 */  move  $a0, $s6
/* 0760C4 7F041594 01711821 */  addu  $v1, $t3, $s1
/* 0760C8 7F041598 C4640000 */  lwc1  $f4, ($v1)
/* 0760CC 7F04159C 016C1021 */  addu  $v0, $t3, $t4
/* 0760D0 7F0415A0 02E02825 */  move  $a1, $s7
/* 0760D4 7F0415A4 E7A400B0 */  swc1  $f4, 0xb0($sp)
/* 0760D8 7F0415A8 C4660004 */  lwc1  $f6, 4($v1)
/* 0760DC 7F0415AC 03C03025 */  move  $a2, $fp
/* 0760E0 7F0415B0 27A700A8 */  addiu $a3, $sp, 0xa8
/* 0760E4 7F0415B4 E7A600B4 */  swc1  $f6, 0xb4($sp)
/* 0760E8 7F0415B8 C44A0000 */  lwc1  $f10, ($v0)
/* 0760EC 7F0415BC E7AA00A8 */  swc1  $f10, 0xa8($sp)
/* 0760F0 7F0415C0 C4480004 */  lwc1  $f8, 4($v0)
/* 0760F4 7F0415C4 0FC2CC80 */  jal   calculateLineIntersectionFactor
/* 0760F8 7F0415C8 E7A800AC */   swc1  $f8, 0xac($sp)
/* 0760FC 7F0415CC 4614003C */  c.lt.s $f0, $f20
/* 076100 7F0415D0 00000000 */  nop
/* 076104 7F0415D4 45020004 */  bc1fl .L7F0415E8
/* 076108 7F0415D8 8FAD00D0 */   lw    $t5, 0xd0($sp)
/* 07610C 7F0415DC 46000506 */  mov.s $f20, $f0
/* 076110 7F0415E0 AFB00094 */  sw    $s0, 0x94($sp)
/* 076114 7F0415E4 8FAD00D0 */  lw    $t5, 0xd0($sp)
.L7F0415E8:
/* 076118 7F0415E8 02608025 */  move  $s0, $s3
/* 07611C 7F0415EC 26310008 */  addiu $s1, $s1, 8
/* 076120 7F0415F0 026D082A */  slt   $at, $s3, $t5
/* 076124 7F0415F4 5420FFC4 */  bnezl $at, .L7F041508
/* 076128 7F0415F8 8FA800D0 */   lw    $t0, 0xd0($sp)
.L7F0415FC:
/* 07612C 7F0415FC 8FAE0094 */  lw    $t6, 0x94($sp)
/* 076130 7F041600 8FAF00D4 */  lw    $t7, 0xd4($sp)
/* 076134 7F041604 27A40088 */  addiu $a0, $sp, 0x88
/* 076138 7F041608 19C00066 */  blez  $t6, .L7F0417A4
/* 07613C 7F04160C 000EC0C0 */   sll   $t8, $t6, 3
/* 076140 7F041610 8FA800D0 */  lw    $t0, 0xd0($sp)
/* 076144 7F041614 25D90001 */  addiu $t9, $t6, 1
/* 076148 7F041618 01F81021 */  addu  $v0, $t7, $t8
/* 07614C 7F04161C 0328001A */  div   $zero, $t9, $t0
/* 076150 7F041620 4480A000 */  mtc1  $zero, $f20
/* 076154 7F041624 C4500000 */  lwc1  $f16, ($v0)
/* 076158 7F041628 00004810 */  mfhi  $t1
/* 07615C 7F04162C E7B4008C */  swc1  $f20, 0x8c($sp)
/* 076160 7F041630 E7B00088 */  swc1  $f16, 0x88($sp)
/* 076164 7F041634 C4520004 */  lwc1  $f18, 4($v0)
/* 076168 7F041638 000950C0 */  sll   $t2, $t1, 3
/* 07616C 7F04163C 01EA1821 */  addu  $v1, $t7, $t2
/* 076170 7F041640 E7B20090 */  swc1  $f18, 0x90($sp)
/* 076174 7F041644 C4640000 */  lwc1  $f4, ($v1)
/* 076178 7F041648 E7B40080 */  swc1  $f20, 0x80($sp)
/* 07617C 7F04164C 27AB006C */  addiu $t3, $sp, 0x6c
/* 076180 7F041650 E7A4007C */  swc1  $f4, 0x7c($sp)
/* 076184 7F041654 C4660004 */  lwc1  $f6, 4($v1)
/* 076188 7F041658 15000002 */  bnez  $t0, .L7F041664
/* 07618C 7F04165C 00000000 */   nop
/* 076190 7F041660 0007000D */  break 7
.L7F041664:
/* 076194 7F041664 2401FFFF */  li    $at, -1
/* 076198 7F041668 15010004 */  bne   $t0, $at, .L7F04167C
/* 07619C 7F04166C 3C018000 */   li    $at, 0x80000000 # -0.000000
/* 0761A0 7F041670 17210002 */  bne   $t9, $at, .L7F04167C
/* 0761A4 7F041674 00000000 */   nop
/* 0761A8 7F041678 0006000D */  break 6
.L7F04167C:
/* 0761AC 7F04167C AFAB0010 */  sw    $t3, 0x10($sp)
/* 0761B0 7F041680 27A5007C */  addiu $a1, $sp, 0x7c
/* 0761B4 7F041684 02A03025 */  move  $a2, $s5
/* 0761B8 7F041688 8FA700E4 */  lw    $a3, 0xe4($sp)
/* 0761BC 7F04168C 0FC0984C */  jal   chrlvLineLineIntersection
/* 0761C0 7F041690 E7A60084 */   swc1  $f6, 0x84($sp)
/* 0761C4 7F041694 C7AA006C */  lwc1  $f10, 0x6c($sp)
/* 0761C8 7F041698 C6A80000 */  lwc1  $f8, ($s5)
/* 0761CC 7F04169C 8FA500E4 */  lw    $a1, 0xe4($sp)
/* 0761D0 7F0416A0 C7A60070 */  lwc1  $f6, 0x70($sp)
/* 0761D4 7F0416A4 46085401 */  sub.s $f16, $f10, $f8
/* 0761D8 7F0416A8 C6AA0004 */  lwc1  $f10, 4($s5)
/* 0761DC 7F0416AC C4B20000 */  lwc1  $f18, ($a1)
/* 0761E0 7F0416B0 8FA300F0 */  lw    $v1, 0xf0($sp)
/* 0761E4 7F0416B4 460A3201 */  sub.s $f8, $f6, $f10
/* 0761E8 7F0416B8 46128102 */  mul.s $f4, $f16, $f18
/* 0761EC 7F0416BC C4B00004 */  lwc1  $f16, 4($a1)
/* 0761F0 7F0416C0 C7AA0074 */  lwc1  $f10, 0x74($sp)
/* 0761F4 7F0416C4 8FA200E8 */  lw    $v0, 0xe8($sp)
/* 0761F8 7F0416C8 46104482 */  mul.s $f18, $f8, $f16
/* 0761FC 7F0416CC C6A80008 */  lwc1  $f8, 8($s5)
/* 076200 7F0416D0 8FA400EC */  lw    $a0, 0xec($sp)
/* 076204 7F0416D4 46085401 */  sub.s $f16, $f10, $f8
/* 076208 7F0416D8 C46A0000 */  lwc1  $f10, ($v1)
/* 07620C 7F0416DC 46122180 */  add.s $f6, $f4, $f18
/* 076210 7F0416E0 C4A40008 */  lwc1  $f4, 8($a1)
/* 076214 7F0416E4 46102482 */  mul.s $f18, $f4, $f16
/* 076218 7F0416E8 46069000 */  add.s $f0, $f18, $f6
/* 07621C 7F0416EC 460A003C */  c.lt.s $f0, $f10
/* 076220 7F0416F0 00000000 */  nop
/* 076224 7F0416F4 4502002C */  bc1fl .L7F0417A8
/* 076228 7F0416F8 00001025 */   move  $v0, $zero
/* 07622C 7F0416FC E4600000 */  swc1  $f0, ($v1)
/* 076230 7F041700 C7A8006C */  lwc1  $f8, 0x6c($sp)
/* 076234 7F041704 E4480000 */  swc1  $f8, ($v0)
/* 076238 7F041708 C7A40070 */  lwc1  $f4, 0x70($sp)
/* 07623C 7F04170C E4440004 */  swc1  $f4, 4($v0)
/* 076240 7F041710 C7B00074 */  lwc1  $f16, 0x74($sp)
/* 076244 7F041714 E4500008 */  swc1  $f16, 8($v0)
/* 076248 7F041718 C4B20000 */  lwc1  $f18, ($a1)
/* 07624C 7F04171C E4940004 */  swc1  $f20, 4($a0)
/* 076250 7F041720 46009187 */  neg.s $f6, $f18
/* 076254 7F041724 E4860000 */  swc1  $f6, ($a0)
/* 076258 7F041728 C4840000 */  lwc1  $f4, ($a0)
/* 07625C 7F04172C C4AA0008 */  lwc1  $f10, 8($a1)
/* 076260 7F041730 24850004 */  addiu $a1, $a0, 4
/* 076264 7F041734 4604A032 */  c.eq.s $f20, $f4
/* 076268 7F041738 46005207 */  neg.s $f8, $f10
/* 07626C 7F04173C 45000007 */  bc1f  .L7F04175C
/* 076270 7F041740 E4880008 */   swc1  $f8, 8($a0)
/* 076274 7F041744 C4900008 */  lwc1  $f16, 8($a0)
/* 076278 7F041748 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 07627C 7F04174C 4610A032 */  c.eq.s $f20, $f16
/* 076280 7F041750 00000000 */  nop
/* 076284 7F041754 45030006 */  bc1tl .L7F041770
/* 076288 7F041758 44819000 */   mtc1  $at, $f18
.L7F04175C:
/* 07628C 7F04175C 0C007DD4 */  jal   guNormalize
/* 076290 7F041760 24860008 */   addiu $a2, $a0, 8
/* 076294 7F041764 10000005 */  b     .L7F04177C
/* 076298 7F041768 8FAC00D8 */   lw    $t4, 0xd8($sp)
/* 07629C 7F04176C 44819000 */  mtc1  $at, $f18
.L7F041770:
/* 0762A0 7F041770 00000000 */  nop
/* 0762A4 7F041774 E4920008 */  swc1  $f18, 8($a0)
/* 0762A8 7F041778 8FAC00D8 */  lw    $t4, 0xd8($sp)
.L7F04177C:
/* 0762AC 7F04177C 3C018003 */  lui   $at, %hi(D_80030B0C)
/* 0762B0 7F041780 24020001 */  li    $v0, 1
/* 0762B4 7F041784 AC2C0B0C */  sw    $t4, %lo(D_80030B0C)($at)
/* 0762B8 7F041788 3C018003 */  lui   $at, %hi(bodypartshot)
/* 0762BC 7F04178C AC200B10 */  sw    $zero, %lo(bodypartshot)($at)
/* 0762C0 7F041790 3C018007 */  lui   $at, %hi(g_CurrentProjectileModel)
/* 0762C4 7F041794 AC205B70 */  sw    $zero, %lo(g_CurrentProjectileModel)($at)
/* 0762C8 7F041798 3C018007 */  lui   $at, %hi(dword_CODE_bss_80075B74)
/* 0762CC 7F04179C 10000002 */  b     .L7F0417A8
/* 0762D0 7F0417A0 AC205B74 */   sw    $zero, %lo(dword_CODE_bss_80075B74)($at)
.L7F0417A4:
/* 0762D4 7F0417A4 00001025 */  move  $v0, $zero
.L7F0417A8:
/* 0762D8 7F0417A8 8FBF0054 */  lw    $ra, 0x54($sp)
/* 0762DC 7F0417AC D7B40028 */  ldc1  $f20, 0x28($sp)
/* 0762E0 7F0417B0 8FB00030 */  lw    $s0, 0x30($sp)
/* 0762E4 7F0417B4 8FB10034 */  lw    $s1, 0x34($sp)
/* 0762E8 7F0417B8 8FB20038 */  lw    $s2, 0x38($sp)
/* 0762EC 7F0417BC 8FB3003C */  lw    $s3, 0x3c($sp)
/* 0762F0 7F0417C0 8FB40040 */  lw    $s4, 0x40($sp)
/* 0762F4 7F0417C4 8FB50044 */  lw    $s5, 0x44($sp)
/* 0762F8 7F0417C8 8FB60048 */  lw    $s6, 0x48($sp)
/* 0762FC 7F0417CC 8FB7004C */  lw    $s7, 0x4c($sp)
/* 076300 7F0417D0 8FBE0050 */  lw    $fp, 0x50($sp)
/* 076304 7F0417D4 03E00008 */  jr    $ra
/* 076308 7F0417D8 27BD00D8 */   addiu $sp, $sp, 0xd8
)
#endif





#ifdef NONMATCHING
void sub_GAME_7F0417DC(void) {

}
#else
GLOBAL_ASM(
.text
glabel sub_GAME_7F0417DC
/* 07630C 7F0417DC 27BDFFA8 */  addiu $sp, $sp, -0x58
/* 076310 7F0417E0 AFB00028 */  sw    $s0, 0x28($sp)
/* 076314 7F0417E4 00808025 */  move  $s0, $a0
/* 076318 7F0417E8 AFBF0034 */  sw    $ra, 0x34($sp)
/* 07631C 7F0417EC AFB20030 */  sw    $s2, 0x30($sp)
/* 076320 7F0417F0 AFB1002C */  sw    $s1, 0x2c($sp)
/* 076324 7F0417F4 AFA60060 */  sw    $a2, 0x60($sp)
/* 076328 7F0417F8 8C840014 */  lw    $a0, 0x14($a0)
/* 07632C 7F0417FC 00A08825 */  move  $s1, $a1
/* 076330 7F041800 00E09025 */  move  $s2, $a3
/* 076334 7F041804 0FC1B403 */  jal   getinstsize
/* 076338 7F041808 AFA40054 */   sw    $a0, 0x54($sp)
/* 07633C 7F04180C 8E020010 */  lw    $v0, 0x10($s0)
/* 076340 7F041810 44801000 */  mtc1  $zero, $f2
/* 076344 7F041814 8C4E001C */  lw    $t6, 0x1c($v0)
/* 076348 7F041818 55C00013 */  bnezl $t6, .L7F041868
/* 07634C 7F04181C 46000207 */   neg.s $f8, $f0
/* 076350 7F041820 C6040058 */  lwc1  $f4, 0x58($s0)
/* 076354 7F041824 C6260000 */  lwc1  $f6, ($s1)
/* 076358 7F041828 C608005C */  lwc1  $f8, 0x5c($s0)
/* 07635C 7F04182C C62A0004 */  lwc1  $f10, 4($s1)
/* 076360 7F041830 46062301 */  sub.s $f12, $f4, $f6
/* 076364 7F041834 C6460000 */  lwc1  $f6, ($s2)
/* 076368 7F041838 C6240008 */  lwc1  $f4, 8($s1)
/* 07636C 7F04183C 460A4381 */  sub.s $f14, $f8, $f10
/* 076370 7F041840 C6120060 */  lwc1  $f18, 0x60($s0)
/* 076374 7F041844 46066202 */  mul.s $f8, $f12, $f6
/* 076378 7F041848 C64A0004 */  lwc1  $f10, 4($s2)
/* 07637C 7F04184C 46049401 */  sub.s $f16, $f18, $f4
/* 076380 7F041850 C6460008 */  lwc1  $f6, 8($s2)
/* 076384 7F041854 460A7482 */  mul.s $f18, $f14, $f10
/* 076388 7F041858 46124100 */  add.s $f4, $f8, $f18
/* 07638C 7F04185C 46103282 */  mul.s $f10, $f6, $f16
/* 076390 7F041860 46045080 */  add.s $f2, $f10, $f4
/* 076394 7F041864 46000207 */  neg.s $f8, $f0
.L7F041868:
/* 076398 7F041868 C7B20068 */  lwc1  $f18, 0x68($sp)
/* 07639C 7F04186C 4602403E */  c.le.s $f8, $f2
/* 0763A0 7F041870 00000000 */  nop
/* 0763A4 7F041874 45020055 */  bc1fl .L7F0419CC
/* 0763A8 7F041878 00001025 */   move  $v0, $zero
/* 0763AC 7F04187C 46009180 */  add.s $f6, $f18, $f0
/* 0763B0 7F041880 4606103E */  c.le.s $f2, $f6
/* 0763B4 7F041884 00000000 */  nop
/* 0763B8 7F041888 45020050 */  bc1fl .L7F0419CC
/* 0763BC 7F04188C 00001025 */   move  $v0, $zero
/* 0763C0 7F041890 904F0001 */  lbu   $t7, 1($v0)
/* 0763C4 7F041894 8FA5006C */  lw    $a1, 0x6c($sp)
/* 0763C8 7F041898 8FA60070 */  lw    $a2, 0x70($sp)
/* 0763CC 7F04189C 31F80002 */  andi  $t8, $t7, 2
/* 0763D0 7F0418A0 1300002A */  beqz  $t8, .L7F04194C
/* 0763D4 7F0418A4 8FA40054 */   lw    $a0, 0x54($sp)
/* 0763D8 7F0418A8 8FB90078 */  lw    $t9, 0x78($sp)
/* 0763DC 7F0418AC 8FA90080 */  lw    $t1, 0x80($sp)
/* 0763E0 7F0418B0 8FAA0084 */  lw    $t2, 0x84($sp)
/* 0763E4 7F0418B4 02002025 */  move  $a0, $s0
/* 0763E8 7F0418B8 8FA70074 */  lw    $a3, 0x74($sp)
/* 0763EC 7F0418BC AFB90010 */  sw    $t9, 0x10($sp)
/* 0763F0 7F0418C0 AFA90014 */  sw    $t1, 0x14($sp)
/* 0763F4 7F0418C4 0FC10458 */  jal   sub_GAME_7F041160
/* 0763F8 7F0418C8 AFAA0018 */   sw    $t2, 0x18($sp)
/* 0763FC 7F0418CC 1040003E */  beqz  $v0, .L7F0419C8
/* 076400 7F0418D0 8FA30074 */   lw    $v1, 0x74($sp)
/* 076404 7F0418D4 C46A0000 */  lwc1  $f10, ($v1)
/* 076408 7F0418D8 C6240000 */  lwc1  $f4, ($s1)
/* 07640C 7F0418DC C4680004 */  lwc1  $f8, 4($v1)
/* 076410 7F0418E0 C6320004 */  lwc1  $f18, 4($s1)
/* 076414 7F0418E4 46045301 */  sub.s $f12, $f10, $f4
/* 076418 7F0418E8 C6440000 */  lwc1  $f4, ($s2)
/* 07641C 7F0418EC C62A0008 */  lwc1  $f10, 8($s1)
/* 076420 7F0418F0 46124381 */  sub.s $f14, $f8, $f18
/* 076424 7F0418F4 C4660008 */  lwc1  $f6, 8($v1)
/* 076428 7F0418F8 46046202 */  mul.s $f8, $f12, $f4
/* 07642C 7F0418FC C6520004 */  lwc1  $f18, 4($s2)
/* 076430 7F041900 460A3401 */  sub.s $f16, $f6, $f10
/* 076434 7F041904 C6440008 */  lwc1  $f4, 8($s2)
/* 076438 7F041908 46127182 */  mul.s $f6, $f14, $f18
/* 07643C 7F04190C 46064280 */  add.s $f10, $f8, $f6
/* 076440 7F041910 46102482 */  mul.s $f18, $f4, $f16
/* 076444 7F041914 44804000 */  mtc1  $zero, $f8
/* 076448 7F041918 C7A60068 */  lwc1  $f6, 0x68($sp)
/* 07644C 7F04191C 460A9080 */  add.s $f2, $f18, $f10
/* 076450 7F041920 4602403E */  c.le.s $f8, $f2
/* 076454 7F041924 00000000 */  nop
/* 076458 7F041928 45020028 */  bc1fl .L7F0419CC
/* 07645C 7F04192C 00001025 */   move  $v0, $zero
/* 076460 7F041930 4606103E */  c.le.s $f2, $f6
/* 076464 7F041934 8FA8007C */  lw    $t0, 0x7c($sp)
/* 076468 7F041938 24020001 */  li    $v0, 1
/* 07646C 7F04193C 45020023 */  bc1fl .L7F0419CC
/* 076470 7F041940 00001025 */   move  $v0, $zero
/* 076474 7F041944 10000021 */  b     .L7F0419CC
/* 076478 7F041948 E5020000 */   swc1  $f2, ($t0)
.L7F04194C:
/* 07647C 7F04194C 0FC1B403 */  jal   getinstsize
/* 076480 7F041950 AFA2003C */   sw    $v0, 0x3c($sp)
/* 076484 7F041954 44070000 */  mfc1  $a3, $f0
/* 076488 7F041958 02202025 */  move  $a0, $s1
/* 07648C 7F04195C 02402825 */  move  $a1, $s2
/* 076490 7F041960 0FC1041D */  jal   sub_GAME_7F041074
/* 076494 7F041964 26060058 */   addiu $a2, $s0, 0x58
/* 076498 7F041968 10400017 */  beqz  $v0, .L7F0419C8
/* 07649C 7F04196C 8FA30074 */   lw    $v1, 0x74($sp)
/* 0764A0 7F041970 8FA8007C */  lw    $t0, 0x7c($sp)
/* 0764A4 7F041974 C7A40068 */  lwc1  $f4, 0x68($sp)
/* 0764A8 7F041978 02202825 */  move  $a1, $s1
/* 0764AC 7F04197C 02403825 */  move  $a3, $s2
/* 0764B0 7F041980 E5040000 */  swc1  $f4, ($t0)
/* 0764B4 7F041984 8FAB0078 */  lw    $t3, 0x78($sp)
/* 0764B8 7F041988 AFA30010 */  sw    $v1, 0x10($sp)
/* 0764BC 7F04198C 8FA60060 */  lw    $a2, 0x60($sp)
/* 0764C0 7F041990 8FA4003C */  lw    $a0, 0x3c($sp)
/* 0764C4 7F041994 AFA80018 */  sw    $t0, 0x18($sp)
/* 0764C8 7F041998 0FC10500 */  jal   sub_GAME_7F041400
/* 0764CC 7F04199C AFAB0014 */   sw    $t3, 0x14($sp)
/* 0764D0 7F0419A0 10400009 */  beqz  $v0, .L7F0419C8
/* 0764D4 7F0419A4 8FAC0054 */   lw    $t4, 0x54($sp)
/* 0764D8 7F0419A8 8FAD0080 */  lw    $t5, 0x80($sp)
/* 0764DC 7F0419AC 24020001 */  li    $v0, 1
/* 0764E0 7F0419B0 ADAC0000 */  sw    $t4, ($t5)
/* 0764E4 7F0419B4 8D8E0008 */  lw    $t6, 8($t4)
/* 0764E8 7F0419B8 8FB80084 */  lw    $t8, 0x84($sp)
/* 0764EC 7F0419BC 8DCF0000 */  lw    $t7, ($t6)
/* 0764F0 7F0419C0 10000002 */  b     .L7F0419CC
/* 0764F4 7F0419C4 AF0F0000 */   sw    $t7, ($t8)
.L7F0419C8:
/* 0764F8 7F0419C8 00001025 */  move  $v0, $zero
.L7F0419CC:
/* 0764FC 7F0419CC 8FBF0034 */  lw    $ra, 0x34($sp)
/* 076500 7F0419D0 8FB00028 */  lw    $s0, 0x28($sp)
/* 076504 7F0419D4 8FB1002C */  lw    $s1, 0x2c($sp)
/* 076508 7F0419D8 8FB20030 */  lw    $s2, 0x30($sp)
/* 07650C 7F0419DC 03E00008 */  jr    $ra
/* 076510 7F0419E0 27BD0058 */   addiu $sp, $sp, 0x58
)
#endif





#ifdef NONMATCHING
// PD: func0f06b610
bool sub_GAME_7F0419E4(ObjectRecord *obj, coord3d *arg1, coord3d *arg2, coord3d *arg3, f32 arg4, coord3d *arg5, coord3d *arg6, coord3d *arg7, coord3d *arg8, f32 *arg9) {

}
#else
bool sub_GAME_7F0419E4(ObjectRecord *obj, coord3d *arg1, coord3d *arg2, coord3d *arg3, f32 arg4, coord3d *arg5, coord3d *arg6, coord3d *arg7, coord3d *arg8, f32 *arg9);
GLOBAL_ASM(
.text
glabel sub_GAME_7F0419E4
/* 076514 7F0419E4 27BDFF68 */  addiu $sp, $sp, -0x98
/* 076518 7F0419E8 AFBF0064 */  sw    $ra, 0x64($sp)
/* 07651C 7F0419EC AFBE0060 */  sw    $fp, 0x60($sp)
/* 076520 7F0419F0 AFB7005C */  sw    $s7, 0x5c($sp)
/* 076524 7F0419F4 AFB60058 */  sw    $s6, 0x58($sp)
/* 076528 7F0419F8 AFB50054 */  sw    $s5, 0x54($sp)
/* 07652C 7F0419FC AFB40050 */  sw    $s4, 0x50($sp)
/* 076530 7F041A00 AFB3004C */  sw    $s3, 0x4c($sp)
/* 076534 7F041A04 AFB20048 */  sw    $s2, 0x48($sp)
/* 076538 7F041A08 AFB10044 */  sw    $s1, 0x44($sp)
/* 07653C 7F041A0C AFB00040 */  sw    $s0, 0x40($sp)
/* 076540 7F041A10 F7B40038 */  sdc1  $f20, 0x38($sp)
/* 076544 7F041A14 8C830010 */  lw    $v1, 0x10($a0)
/* 076548 7F041A18 C7B400A8 */  lwc1  $f20, 0xa8($sp)
/* 07654C 7F041A1C 8FB700AC */  lw    $s7, 0xac($sp)
/* 076550 7F041A20 8FBE00B0 */  lw    $fp, 0xb0($sp)
/* 076554 7F041A24 27AE008C */  addiu $t6, $sp, 0x8c
/* 076558 7F041A28 27AF0080 */  addiu $t7, $sp, 0x80
/* 07655C 7F041A2C 27B8007C */  addiu $t8, $sp, 0x7c
/* 076560 7F041A30 27B90078 */  addiu $t9, $sp, 0x78
/* 076564 7F041A34 27A80074 */  addiu $t0, $sp, 0x74
/* 076568 7F041A38 00808025 */  move  $s0, $a0
/* 07656C 7F041A3C 00A09825 */  move  $s3, $a1
/* 076570 7F041A40 00C0A025 */  move  $s4, $a2
/* 076574 7F041A44 00E0A825 */  move  $s5, $a3
/* 076578 7F041A48 0000B025 */  move  $s6, $zero
/* 07657C 7F041A4C AFA8002C */  sw    $t0, 0x2c($sp)
/* 076580 7F041A50 AFB90028 */  sw    $t9, 0x28($sp)
/* 076584 7F041A54 AFB80024 */  sw    $t8, 0x24($sp)
/* 076588 7F041A58 AFAF0020 */  sw    $t7, 0x20($sp)
/* 07658C 7F041A5C AFAE001C */  sw    $t6, 0x1c($sp)
/* 076590 7F041A60 AFA3006C */  sw    $v1, 0x6c($sp)
/* 076594 7F041A64 E7B40010 */  swc1  $f20, 0x10($sp)
/* 076598 7F041A68 AFB70014 */  sw    $s7, 0x14($sp)
/* 07659C 7F041A6C 0FC105F7 */  jal   sub_GAME_7F0417DC
/* 0765A0 7F041A70 AFBE0018 */   sw    $fp, 0x18($sp)
/* 0765A4 7F041A74 10400024 */  beqz  $v0, .L7F041B08
/* 0765A8 7F041A78 8FA3006C */   lw    $v1, 0x6c($sp)
/* 0765AC 7F041A7C 8FA900BC */  lw    $t1, 0xbc($sp)
/* 0765B0 7F041A80 C7A4007C */  lwc1  $f4, 0x7c($sp)
/* 0765B4 7F041A84 8FB100B4 */  lw    $s1, 0xb4($sp)
/* 0765B8 7F041A88 C5260000 */  lwc1  $f6, ($t1)
/* 0765BC 7F041A8C 8FB200B8 */  lw    $s2, 0xb8($sp)
/* 0765C0 7F041A90 4606203C */  c.lt.s $f4, $f6
/* 0765C4 7F041A94 00000000 */  nop
/* 0765C8 7F041A98 4502001C */  bc1fl .L7F041B0C
/* 0765CC 7F041A9C 906E0001 */   lbu   $t6, 1($v1)
/* 0765D0 7F041AA0 E5240000 */  swc1  $f4, ($t1)
/* 0765D4 7F041AA4 C7A8008C */  lwc1  $f8, 0x8c($sp)
/* 0765D8 7F041AA8 3C018003 */  lui   $at, %hi(D_80030B0C)
/* 0765DC 7F041AAC 240BFFFF */  li    $t3, -1
/* 0765E0 7F041AB0 E6280000 */  swc1  $f8, ($s1)
/* 0765E4 7F041AB4 C7AA0090 */  lwc1  $f10, 0x90($sp)
/* 0765E8 7F041AB8 24160001 */  li    $s6, 1
/* 0765EC 7F041ABC E62A0004 */  swc1  $f10, 4($s1)
/* 0765F0 7F041AC0 C7B00094 */  lwc1  $f16, 0x94($sp)
/* 0765F4 7F041AC4 E6300008 */  swc1  $f16, 8($s1)
/* 0765F8 7F041AC8 C7B20080 */  lwc1  $f18, 0x80($sp)
/* 0765FC 7F041ACC E6520000 */  swc1  $f18, ($s2)
/* 076600 7F041AD0 C7A60084 */  lwc1  $f6, 0x84($sp)
/* 076604 7F041AD4 E6460004 */  swc1  $f6, 4($s2)
/* 076608 7F041AD8 C7A40088 */  lwc1  $f4, 0x88($sp)
/* 07660C 7F041ADC E6440008 */  swc1  $f4, 8($s2)
/* 076610 7F041AE0 8E0A0010 */  lw    $t2, 0x10($s0)
/* 076614 7F041AE4 8FAC0078 */  lw    $t4, 0x78($sp)
/* 076618 7F041AE8 8FAD0074 */  lw    $t5, 0x74($sp)
/* 07661C 7F041AEC AC2A0B0C */  sw    $t2, %lo(D_80030B0C)($at)
/* 076620 7F041AF0 3C018003 */  lui   $at, %hi(bodypartshot)
/* 076624 7F041AF4 AC2B0B10 */  sw    $t3, %lo(bodypartshot)($at)
/* 076628 7F041AF8 3C018007 */  lui   $at, %hi(g_CurrentProjectileModel)
/* 07662C 7F041AFC AC2C5B70 */  sw    $t4, %lo(g_CurrentProjectileModel)($at)
/* 076630 7F041B00 3C018007 */  lui   $at, %hi(dword_CODE_bss_80075B74)
/* 076634 7F041B04 AC2D5B74 */  sw    $t5, %lo(dword_CODE_bss_80075B74)($at)
.L7F041B08:
/* 076638 7F041B08 906E0001 */  lbu   $t6, 1($v1)
.L7F041B0C:
/* 07663C 7F041B0C 8FB100B4 */  lw    $s1, 0xb4($sp)
/* 076640 7F041B10 8FB200B8 */  lw    $s2, 0xb8($sp)
/* 076644 7F041B14 31CF0002 */  andi  $t7, $t6, 2
/* 076648 7F041B18 51E0001A */  beql  $t7, $zero, .L7F041B84
/* 07664C 7F041B1C 8FBF0064 */   lw    $ra, 0x64($sp)
/* 076650 7F041B20 8C700020 */  lw    $s0, 0x20($v1)
/* 076654 7F041B24 52000017 */  beql  $s0, $zero, .L7F041B84
/* 076658 7F041B28 8FBF0064 */   lw    $ra, 0x64($sp)
/* 07665C 7F041B2C 92180001 */  lbu   $t8, 1($s0)
.L7F041B30:
/* 076660 7F041B30 02602825 */  move  $a1, $s3
/* 076664 7F041B34 02803025 */  move  $a2, $s4
/* 076668 7F041B38 33190002 */  andi  $t9, $t8, 2
/* 07666C 7F041B3C 1320000D */  beqz  $t9, .L7F041B74
/* 076670 7F041B40 02A03825 */   move  $a3, $s5
/* 076674 7F041B44 8E040004 */  lw    $a0, 4($s0)
/* 076678 7F041B48 8FA800BC */  lw    $t0, 0xbc($sp)
/* 07667C 7F041B4C AFB20020 */  sw    $s2, 0x20($sp)
/* 076680 7F041B50 AFB1001C */  sw    $s1, 0x1c($sp)
/* 076684 7F041B54 AFBE0018 */  sw    $fp, 0x18($sp)
/* 076688 7F041B58 AFB70014 */  sw    $s7, 0x14($sp)
/* 07668C 7F041B5C E7B40010 */  swc1  $f20, 0x10($sp)
/* 076690 7F041B60 0FC10679 */  jal   sub_GAME_7F0419E4
/* 076694 7F041B64 AFA80024 */   sw    $t0, 0x24($sp)
/* 076698 7F041B68 50400003 */  beql  $v0, $zero, .L7F041B78
/* 07669C 7F041B6C 8E100024 */   lw    $s0, 0x24($s0)
/* 0766A0 7F041B70 24160001 */  li    $s6, 1
.L7F041B74:
/* 0766A4 7F041B74 8E100024 */  lw    $s0, 0x24($s0)
.L7F041B78:
/* 0766A8 7F041B78 5600FFED */  bnezl $s0, .L7F041B30
/* 0766AC 7F041B7C 92180001 */   lbu   $t8, 1($s0)
/* 0766B0 7F041B80 8FBF0064 */  lw    $ra, 0x64($sp)
.L7F041B84:
/* 0766B4 7F041B84 02C01025 */  move  $v0, $s6
/* 0766B8 7F041B88 8FB60058 */  lw    $s6, 0x58($sp)
/* 0766BC 7F041B8C D7B40038 */  ldc1  $f20, 0x38($sp)
/* 0766C0 7F041B90 8FB00040 */  lw    $s0, 0x40($sp)
/* 0766C4 7F041B94 8FB10044 */  lw    $s1, 0x44($sp)
/* 0766C8 7F041B98 8FB20048 */  lw    $s2, 0x48($sp)
/* 0766CC 7F041B9C 8FB3004C */  lw    $s3, 0x4c($sp)
/* 0766D0 7F041BA0 8FB40050 */  lw    $s4, 0x50($sp)
/* 0766D4 7F041BA4 8FB50054 */  lw    $s5, 0x54($sp)
/* 0766D8 7F041BA8 8FB7005C */  lw    $s7, 0x5c($sp)
/* 0766DC 7F041BAC 8FBE0060 */  lw    $fp, 0x60($sp)
/* 0766E0 7F041BB0 03E00008 */  jr    $ra
/* 0766E4 7F041BB4 27BD0098 */   addiu $sp, $sp, 0x98
)
#endif





#ifdef NONMATCHING
// PD: func0f06c28c
bool sub_GAME_7F041BB8(ChrRecord *chr, coord3d *arg1, coord3d *arg2, f32 arg3, coord3d *arg4, coord3d *arg5, coord3d *arg6, coord3d *arg7, f32 *arg8) {

}
#else
bool sub_GAME_7F041BB8(ChrRecord *chr, coord3d *arg1, coord3d *arg2, f32 arg3, coord3d *arg4, coord3d *arg5, coord3d *arg6, coord3d *arg7, f32 *arg8);
GLOBAL_ASM(
.text
glabel sub_GAME_7F041BB8
/* 0766E8 7F041BB8 27BDFF98 */  addiu $sp, $sp, -0x68
/* 0766EC 7F041BBC AFA70074 */  sw    $a3, 0x74($sp)
/* 0766F0 7F041BC0 AFBF002C */  sw    $ra, 0x2c($sp)
/* 0766F4 7F041BC4 AFB20028 */  sw    $s2, 0x28($sp)
/* 0766F8 7F041BC8 AFB10024 */  sw    $s1, 0x24($sp)
/* 0766FC 7F041BCC AFB00020 */  sw    $s0, 0x20($sp)
/* 076700 7F041BD0 AFA40068 */  sw    $a0, 0x68($sp)
/* 076704 7F041BD4 00803825 */  move  $a3, $a0
/* 076708 7F041BD8 8C920018 */  lw    $s2, 0x18($a0)
/* 07670C 7F041BDC 8C84001C */  lw    $a0, 0x1c($a0)
/* 076710 7F041BE0 00C08025 */  move  $s0, $a2
/* 076714 7F041BE4 00A08825 */  move  $s1, $a1
/* 076718 7F041BE8 0FC1B403 */  jal   getinstsize
/* 07671C 7F041BEC AFA70068 */   sw    $a3, 0x68($sp)
/* 076720 7F041BF0 C6440008 */  lwc1  $f4, 8($s2)
/* 076724 7F041BF4 C6260000 */  lwc1  $f6, ($s1)
/* 076728 7F041BF8 C648000C */  lwc1  $f8, 0xc($s2)
/* 07672C 7F041BFC C62A0004 */  lwc1  $f10, 4($s1)
/* 076730 7F041C00 46062081 */  sub.s $f2, $f4, $f6
/* 076734 7F041C04 C6060000 */  lwc1  $f6, ($s0)
/* 076738 7F041C08 C6240008 */  lwc1  $f4, 8($s1)
/* 07673C 7F041C0C 460A4301 */  sub.s $f12, $f8, $f10
/* 076740 7F041C10 C6520010 */  lwc1  $f18, 0x10($s2)
/* 076744 7F041C14 46061202 */  mul.s $f8, $f2, $f6
/* 076748 7F041C18 C60A0004 */  lwc1  $f10, 4($s0)
/* 07674C 7F041C1C 46049381 */  sub.s $f14, $f18, $f4
/* 076750 7F041C20 C6060008 */  lwc1  $f6, 8($s0)
/* 076754 7F041C24 460A6482 */  mul.s $f18, $f12, $f10
/* 076758 7F041C28 46124100 */  add.s $f4, $f8, $f18
/* 07675C 7F041C2C 460E3282 */  mul.s $f10, $f6, $f14
/* 076760 7F041C30 C7B20074 */  lwc1  $f18, 0x74($sp)
/* 076764 7F041C34 46000207 */  neg.s $f8, $f0
/* 076768 7F041C38 46045400 */  add.s $f16, $f10, $f4
/* 07676C 7F041C3C 4610403E */  c.le.s $f8, $f16
/* 076770 7F041C40 00000000 */  nop
/* 076774 7F041C44 4502006B */  bc1fl .L7F041DF4
/* 076778 7F041C48 00001025 */   move  $v0, $zero
/* 07677C 7F041C4C 46009180 */  add.s $f6, $f18, $f0
/* 076780 7F041C50 4606803E */  c.le.s $f16, $f6
/* 076784 7F041C54 00000000 */  nop
/* 076788 7F041C58 45020066 */  bc1fl .L7F041DF4
/* 07678C 7F041C5C 00001025 */   move  $v0, $zero
/* 076790 7F041C60 924E0001 */  lbu   $t6, 1($s2)
/* 076794 7F041C64 8FB80068 */  lw    $t8, 0x68($sp)
/* 076798 7F041C68 27A40038 */  addiu $a0, $sp, 0x38
/* 07679C 7F041C6C 31CF0002 */  andi  $t7, $t6, 2
/* 0767A0 7F041C70 11E0005F */  beqz  $t7, .L7F041DF0
/* 0767A4 7F041C74 27A70040 */   addiu $a3, $sp, 0x40
/* 0767A8 7F041C78 8F190020 */  lw    $t9, 0x20($t8)
/* 0767AC 7F041C7C 27A8003C */  addiu $t0, $sp, 0x3c
/* 0767B0 7F041C80 AFA80010 */  sw    $t0, 0x10($sp)
/* 0767B4 7F041C84 8FA50078 */  lw    $a1, 0x78($sp)
/* 0767B8 7F041C88 8FA6007C */  lw    $a2, 0x7c($sp)
/* 0767BC 7F041C8C 0FC1B004 */  jal   sub_GAME_7F06C010
/* 0767C0 7F041C90 AFB90038 */   sw    $t9, 0x38($sp)
/* 0767C4 7F041C94 18400056 */  blez  $v0, .L7F041DF0
/* 0767C8 7F041C98 AFA20044 */   sw    $v0, 0x44($sp)
/* 0767CC 7F041C9C 8FA40040 */  lw    $a0, 0x40($sp)
/* 0767D0 7F041CA0 8FA5003C */  lw    $a1, 0x3c($sp)
/* 0767D4 7F041CA4 0FC1B198 */  jal   modelFindNodeMtx
/* 0767D8 7F041CA8 00003025 */   move  $a2, $zero
/* 0767DC 7F041CAC 8FA30078 */  lw    $v1, 0x78($sp)
/* 0767E0 7F041CB0 C44A0030 */  lwc1  $f10, 0x30($v0)
/* 0767E4 7F041CB4 8FA7007C */  lw    $a3, 0x7c($sp)
/* 0767E8 7F041CB8 C4640000 */  lwc1  $f4, ($v1)
/* 0767EC 7F041CBC C4480034 */  lwc1  $f8, 0x34($v0)
/* 0767F0 7F041CC0 C4720004 */  lwc1  $f18, 4($v1)
/* 0767F4 7F041CC4 46045081 */  sub.s $f2, $f10, $f4
/* 0767F8 7F041CC8 C4E40000 */  lwc1  $f4, ($a3)
/* 0767FC 7F041CCC C4460038 */  lwc1  $f6, 0x38($v0)
/* 076800 7F041CD0 46124301 */  sub.s $f12, $f8, $f18
/* 076804 7F041CD4 C46A0008 */  lwc1  $f10, 8($v1)
/* 076808 7F041CD8 46041202 */  mul.s $f8, $f2, $f4
/* 07680C 7F041CDC C4F20004 */  lwc1  $f18, 4($a3)
/* 076810 7F041CE0 460A3381 */  sub.s $f14, $f6, $f10
/* 076814 7F041CE4 C4E40008 */  lwc1  $f4, 8($a3)
/* 076818 7F041CE8 46126182 */  mul.s $f6, $f12, $f18
/* 07681C 7F041CEC 8FA50088 */  lw    $a1, 0x88($sp)
/* 076820 7F041CF0 8FA20080 */  lw    $v0, 0x80($sp)
/* 076824 7F041CF4 460E2482 */  mul.s $f18, $f4, $f14
/* 076828 7F041CF8 8FA40084 */  lw    $a0, 0x84($sp)
/* 07682C 7F041CFC 46064280 */  add.s $f10, $f8, $f6
/* 076830 7F041D00 C4A80000 */  lwc1  $f8, ($a1)
/* 076834 7F041D04 460A9000 */  add.s $f0, $f18, $f10
/* 076838 7F041D08 4608003C */  c.lt.s $f0, $f8
/* 07683C 7F041D0C 00000000 */  nop
/* 076840 7F041D10 45020038 */  bc1fl .L7F041DF4
/* 076844 7F041D14 00001025 */   move  $v0, $zero
/* 076848 7F041D18 E4A00000 */  swc1  $f0, ($a1)
/* 07684C 7F041D1C C6060000 */  lwc1  $f6, ($s0)
/* 076850 7F041D20 C6320000 */  lwc1  $f18, ($s1)
/* 076854 7F041D24 44801000 */  mtc1  $zero, $f2
/* 076858 7F041D28 46003102 */  mul.s $f4, $f6, $f0
/* 07685C 7F041D2C 24850004 */  addiu $a1, $a0, 4
/* 076860 7F041D30 46122280 */  add.s $f10, $f4, $f18
/* 076864 7F041D34 E44A0000 */  swc1  $f10, ($v0)
/* 076868 7F041D38 C6080004 */  lwc1  $f8, 4($s0)
/* 07686C 7F041D3C C6240004 */  lwc1  $f4, 4($s1)
/* 076870 7F041D40 46004182 */  mul.s $f6, $f8, $f0
/* 076874 7F041D44 46043480 */  add.s $f18, $f6, $f4
/* 076878 7F041D48 E4520004 */  swc1  $f18, 4($v0)
/* 07687C 7F041D4C C60A0008 */  lwc1  $f10, 8($s0)
/* 076880 7F041D50 C6260008 */  lwc1  $f6, 8($s1)
/* 076884 7F041D54 46005202 */  mul.s $f8, $f10, $f0
/* 076888 7F041D58 46064100 */  add.s $f4, $f8, $f6
/* 07688C 7F041D5C E4440008 */  swc1  $f4, 8($v0)
/* 076890 7F041D60 C6120000 */  lwc1  $f18, ($s0)
/* 076894 7F041D64 E4820004 */  swc1  $f2, 4($a0)
/* 076898 7F041D68 46009287 */  neg.s $f10, $f18
/* 07689C 7F041D6C E48A0000 */  swc1  $f10, ($a0)
/* 0768A0 7F041D70 C4840000 */  lwc1  $f4, ($a0)
/* 0768A4 7F041D74 C6080008 */  lwc1  $f8, 8($s0)
/* 0768A8 7F041D78 46041032 */  c.eq.s $f2, $f4
/* 0768AC 7F041D7C 46004187 */  neg.s $f6, $f8
/* 0768B0 7F041D80 45000007 */  bc1f  .L7F041DA0
/* 0768B4 7F041D84 E4860008 */   swc1  $f6, 8($a0)
/* 0768B8 7F041D88 C4920008 */  lwc1  $f18, 8($a0)
/* 0768BC 7F041D8C 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 0768C0 7F041D90 46121032 */  c.eq.s $f2, $f18
/* 0768C4 7F041D94 00000000 */  nop
/* 0768C8 7F041D98 45030006 */  bc1tl .L7F041DB4
/* 0768CC 7F041D9C 44815000 */   mtc1  $at, $f10
.L7F041DA0:
/* 0768D0 7F041DA0 0C007DD4 */  jal   guNormalize
/* 0768D4 7F041DA4 24860008 */   addiu $a2, $a0, 8
/* 0768D8 7F041DA8 10000005 */  b     .L7F041DC0
/* 0768DC 7F041DAC 8FA90044 */   lw    $t1, 0x44($sp)
/* 0768E0 7F041DB0 44815000 */  mtc1  $at, $f10
.L7F041DB4:
/* 0768E4 7F041DB4 00000000 */  nop
/* 0768E8 7F041DB8 E48A0008 */  swc1  $f10, 8($a0)
/* 0768EC 7F041DBC 8FA90044 */  lw    $t1, 0x44($sp)
.L7F041DC0:
/* 0768F0 7F041DC0 3C018003 */  lui   $at, %hi(D_80030B0C)
/* 0768F4 7F041DC4 AC320B0C */  sw    $s2, %lo(D_80030B0C)($at)
/* 0768F8 7F041DC8 8FAA0040 */  lw    $t2, 0x40($sp)
/* 0768FC 7F041DCC 3C018003 */  lui   $at, %hi(bodypartshot)
/* 076900 7F041DD0 AC290B10 */  sw    $t1, %lo(bodypartshot)($at)
/* 076904 7F041DD4 3C018007 */  lui   $at, %hi(g_CurrentProjectileModel)
/* 076908 7F041DD8 8FAB003C */  lw    $t3, 0x3c($sp)
/* 07690C 7F041DDC AC2A5B70 */  sw    $t2, %lo(g_CurrentProjectileModel)($at)
/* 076910 7F041DE0 3C018007 */  lui   $at, %hi(dword_CODE_bss_80075B74)
/* 076914 7F041DE4 24020001 */  li    $v0, 1
/* 076918 7F041DE8 10000002 */  b     .L7F041DF4
/* 07691C 7F041DEC AC2B5B74 */   sw    $t3, %lo(dword_CODE_bss_80075B74)($at)
.L7F041DF0:
/* 076920 7F041DF0 00001025 */  move  $v0, $zero
.L7F041DF4:
/* 076924 7F041DF4 8FBF002C */  lw    $ra, 0x2c($sp)
/* 076928 7F041DF8 8FB00020 */  lw    $s0, 0x20($sp)
/* 07692C 7F041DFC 8FB10024 */  lw    $s1, 0x24($sp)
/* 076930 7F041E00 8FB20028 */  lw    $s2, 0x28($sp)
/* 076934 7F041E04 03E00008 */  jr    $ra
/* 076938 7F041E08 27BD0068 */   addiu $sp, $sp, 0x68
)
#endif


bool projectileFindCollidingProp(PropRecord *prop, coord3d *pos1, coord3d *pos2, u32 cdtypes, coord3d *arg4, coord3d *arg5, s32 *rooms)
{
    bool result;
    f32 dist;
    s16 *propnumptr;
    f32 spa8;
    bool spa4;
    coord3d sp98;
    ChrRecord *chr;
    coord3d sp88;
    coord3d sp7c;
    PropRecord *iterprop;
    PropRecord *playerstank;
    ObjectRecord *obj;
    s32 unused;

    result = FALSE;
    spa4 = FALSE;
    playerstank = get_ptr_for_players_tank();

    sp98.x = pos2->x - pos1->x;
    sp98.y = pos2->y - pos1->y;
    sp98.z = pos2->z - pos1->z;

    dist = sqrtf(sp98.f[0] * sp98.f[0] + sp98.f[1] * sp98.f[1] + sp98.f[2] * sp98.f[2]);

    if (dist == 0.0f)
    {
        return FALSE;
    }

    sp98.x *= (1.0f / dist);
    sp98.y *= (1.0f / dist);
    sp98.z *= (1.0f / dist);

    sp88.x = pos1->x;
    sp88.y = pos1->y;
    sp88.z = pos1->z;

    mtx4TransformVecInPlace(camGetWorldToScreenMtxf(), &sp88);

    sp7c.x = sp98.x;
    sp7c.y = sp98.y;
    sp7c.z = sp98.z;

    mtx4RotateVecInPlace(camGetWorldToScreenMtxf(), sp7c.f);

    spa8 = dist;

    if (cdtypes != 0)
    {
        roomGetProps(rooms);

        for (propnumptr = ptr_list_object_lookup_indices; *propnumptr >= 0; propnumptr++)
        {
            iterprop = &pos_data_entry[*propnumptr];

            if (iterprop != prop)
            {
                if (iterprop->type == PROP_TYPE_OBJ
                        || iterprop->type == PROP_TYPE_WEAPON
                        || iterprop->type == PROP_TYPE_DOOR)
                {
                    obj = iterprop->obj;

                    if ((obj->runtime_bitflags & RUNTIMEBITFLAG_ISRETICK) == 0 && (obj->flags2 & PROPFLAG2_THROWTHROUGH) == 0) {
                        if (iterprop->type == PROP_TYPE_DOOR)
                        {
                            if ((cdtypes & CDTYPE_DOORS) == 0 && (propDoorGetCdTypes(iterprop) & cdtypes) == 0)
                            {
                                continue;
                            }
                        }
                        else
                        {
                            if ((cdtypes & CDTYPE_OBJS) == 0)
                            {
                                continue;
                            }
                        }

                        if ((iterprop != playerstank) || !(obj->state & PROPSTATE_20))
                        {
                            if (sub_GAME_7F0419E4(obj, pos1, pos2, &sp98, dist, &sp88, &sp7c, arg4, arg5, &spa8))
                            {
                                spa4 = TRUE;
                            }
                        }
                    }
                } else if (iterprop->type == PROP_TYPE_CHR
                        || (iterprop->type == PROP_TYPE_VIEWER && iterprop->chr))
                {
                    chr = iterprop->chr;

                    if (iterprop->type == PROP_TYPE_VIEWER)
                    {
                        if (!g_playerPointers[getPlayerPointerIndex(iterprop)]->field_AC || (cdtypes & CDTYPE_PLAYERS) == 0)
                        {
                            continue;
                        }
                    }
                    else if (iterprop->type == PROP_TYPE_CHR)
                    {
                        if ((chr->hidden & CHRHIDDEN_MOVING) || (cdtypes & CDTYPE_CHRS) == 0)
                        {
                            continue;
                        }
                    }

                    if (sub_GAME_7F041BB8(chr, pos1, &sp98, dist, &sp88, &sp7c, arg4, arg5, &spa8))
                    {
                        spa4 = TRUE;
                    }
                } else if (iterprop->type == PROP_TYPE_VIEWER
                        && g_playerPointers[getPlayerPointerIndex(iterprop)]->field_AC)
                {
                    if (sub_GAME_7F041400(iterprop, pos1, pos2, &sp98, arg4, arg5, &spa8))
                    {
                        spa4 = TRUE;
                    }
                }
            }
        }
    }

    if (spa4)
    {
        result = TRUE;

        flt_CODE_bss_80075B78.x = sp98.x;
        flt_CODE_bss_80075B78.y = sp98.y;
        flt_CODE_bss_80075B78.z = sp98.z;

        flt_CODE_bss_80075B88.x = sp7c.x;
        flt_CODE_bss_80075B88.y = sp7c.y;
        flt_CODE_bss_80075B88.z = sp7c.z;

    }

    return result;
}


#ifdef NONMATCHING
void handles_projectile_motion(void) {
    //this function contains
    // osSyncPrintf("stanLineObjGfx: %d rooms is more than %d\n",arg0+0x58,20);
}
#else
GLOBAL_ASM(
.late_rodata
glabel D_80052A58
.word 0x3dcccccd /*0.1*/
.text
glabel handles_projectile_motion
/* 076D7C 7F04224C 27BDFD50 */  addiu $sp, $sp, -0x2b0
/* 076D80 7F042250 AFBF003C */  sw    $ra, 0x3c($sp)
/* 076D84 7F042254 AFB30038 */  sw    $s3, 0x38($sp)
/* 076D88 7F042258 AFB20034 */  sw    $s2, 0x34($sp)
/* 076D8C 7F04225C AFB10030 */  sw    $s1, 0x30($sp)
/* 076D90 7F042260 AFB0002C */  sw    $s0, 0x2c($sp)
/* 076D94 7F042264 F7B40020 */  sdc1  $f20, 0x20($sp)
/* 076D98 7F042268 8C820010 */  lw    $v0, 0x10($a0)
/* 076D9C 7F04226C 240F0001 */  li    $t7, 1
/* 076DA0 7F042270 3C018003 */  lui    $at, %hi(D_80030B0C)
/* 076DA4 7F042274 8C4E0014 */  lw    $t6, 0x14($v0)
/* 076DA8 7F042278 AFAF02A4 */  sw    $t7, 0x2a4($sp)
/* 076DAC 7F04227C AC200B0C */  sw    $zero, %lo(D_80030B0C)($at)
/* 076DB0 7F042280 AFAE02A8 */  sw    $t6, 0x2a8($sp)
/* 076DB4 7F042284 C4840058 */  lwc1  $f4, 0x58($a0)
/* 076DB8 7F042288 C4A00000 */  lwc1  $f0, ($a1)
/* 076DBC 7F04228C 00E08025 */  move  $s0, $a3
/* 076DC0 7F042290 00A08825 */  move  $s1, $a1
/* 076DC4 7F042294 46040032 */  c.eq.s $f0, $f4
/* 076DC8 7F042298 00809025 */  move  $s2, $a0
/* 076DCC 7F04229C 00C09825 */  move  $s3, $a2
/* 076DD0 7F0422A0 4502000E */  bc1fl .L7F0422DC
/* 076DD4 7F0422A4 E7A00268 */   swc1  $f0, 0x268($sp)
/* 076DD8 7F0422A8 C4A60004 */  lwc1  $f6, 4($a1)
/* 076DDC 7F0422AC C488005C */  lwc1  $f8, 0x5c($a0)
/* 076DE0 7F0422B0 46083032 */  c.eq.s $f6, $f8
/* 076DE4 7F0422B4 00000000 */  nop
/* 076DE8 7F0422B8 45020008 */  bc1fl .L7F0422DC
/* 076DEC 7F0422BC E7A00268 */   swc1  $f0, 0x268($sp)
/* 076DF0 7F0422C0 C4AA0008 */  lwc1  $f10, 8($a1)
/* 076DF4 7F0422C4 C4900060 */  lwc1  $f16, 0x60($a0)
/* 076DF8 7F0422C8 46105032 */  c.eq.s $f10, $f16
/* 076DFC 7F0422CC 00000000 */  nop
/* 076E00 7F0422D0 450301C6 */  bc1tl .L7F0429EC
/* 076E04 7F0422D4 8FBF003C */   lw    $ra, 0x3c($sp)
/* 076E08 7F0422D8 E7A00268 */  swc1  $f0, 0x268($sp)
.L7F0422DC:
/* 076E0C 7F0422DC C6320004 */  lwc1  $f18, 4($s1)
/* 076E10 7F0422E0 E7B2026C */  swc1  $f18, 0x26c($sp)
/* 076E14 7F0422E4 C6240008 */  lwc1  $f4, 8($s1)
/* 076E18 7F0422E8 E7A40270 */  swc1  $f4, 0x270($sp)
/* 076E1C 7F0422EC 8E580064 */  lw    $t8, 0x64($s2)
/* 076E20 7F0422F0 33190080 */  andi  $t9, $t8, 0x80
/* 076E24 7F0422F4 532001BD */  beql  $t9, $zero, .L7F0429EC
/* 076E28 7F0422F8 8FBF003C */   lw    $ra, 0x3c($sp)
/* 076E2C 7F0422FC 8E49006C */  lw    $t1, 0x6c($s2)
/* 076E30 7F042300 26440058 */  addiu $a0, $s2, 0x58
/* 076E34 7F042304 27A50268 */  addiu $a1, $sp, 0x268
/* 076E38 7F042308 8D2A0000 */  lw    $t2, ($t1)
/* 076E3C 7F04230C 27A70074 */  addiu $a3, $sp, 0x74
/* 076E40 7F042310 27AC007C */  addiu $t4, $sp, 0x7c
/* 076E44 7F042314 314B0004 */  andi  $t3, $t2, 4
/* 076E48 7F042318 116001B3 */  beqz  $t3, .L7F0429E8
/* 076E4C 7F04231C 27AD0260 */   addiu $t5, $sp, 0x260
/* 076E50 7F042320 AFA00260 */  sw    $zero, 0x260($sp)
/* 076E54 7F042324 8E46006C */  lw    $a2, 0x6c($s2)
/* 076E58 7F042328 240E0014 */  li    $t6, 20
/* 076E5C 7F04232C AFAE0018 */  sw    $t6, 0x18($sp)
/* 076E60 7F042330 AFB102B4 */  sw    $s1, 0x2b4($sp)
/* 076E64 7F042334 AFA40048 */  sw    $a0, 0x48($sp)
/* 076E68 7F042338 AFA202AC */  sw    $v0, 0x2ac($sp)
/* 076E6C 7F04233C AFAD0014 */  sw    $t5, 0x14($sp)
/* 076E70 7F042340 AFAC0010 */  sw    $t4, 0x10($sp)
/* 076E74 7F042344 0FC2D2AD */  jal   sub_GAME_7F0B4AB4
/* 076E78 7F042348 24C600CC */   addiu $a2, $a2, 0xcc
/* 076E7C 7F04234C 8FA30260 */  lw    $v1, 0x260($sp)
/* 076E80 7F042350 27B8007C */  addiu $t8, $sp, 0x7c
/* 076E84 7F042354 24050064 */  li    $a1, 100
/* 076E88 7F042358 28610015 */  slti  $at, $v1, 0x15
/* 076E8C 7F04235C 54200003 */  bnezl $at, .L7F04236C
/* 076E90 7F042360 00037880 */   sll   $t7, $v1, 2
/* 076E94 7F042364 24030014 */  li    $v1, 20
/* 076E98 7F042368 00037880 */  sll   $t7, $v1, 2
.L7F04236C:
/* 076E9C 7F04236C 01F82021 */  addu  $a0, $t7, $t8
/* 076EA0 7F042370 0FC2E3A6 */  jal   sub_GAME_7F0B8E98
/* 076EA4 7F042374 AFA30260 */   sw    $v1, 0x260($sp)
/* 076EA8 7F042378 8FA30260 */  lw    $v1, 0x260($sp)
/* 076EAC 7F04237C 2419FFFF */  li    $t9, -1
/* 076EB0 7F042380 27B1007C */  addiu $s1, $sp, 0x7c
/* 076EB4 7F042384 00621821 */  addu  $v1, $v1, $v0
/* 076EB8 7F042388 00034880 */  sll   $t1, $v1, 2
/* 076EBC 7F04238C 03A95021 */  addu  $t2, $sp, $t1
/* 076EC0 7F042390 AD59007C */  sw    $t9, 0x7c($t2)
/* 076EC4 7F042394 8FAB007C */  lw    $t3, 0x7c($sp)
/* 076EC8 7F042398 AFA30260 */  sw    $v1, 0x260($sp)
/* 076ECC 7F04239C 0562009E */  bltzl $t3, .L7F042618
/* 076ED0 7F0423A0 8FAD02A4 */   lw    $t5, 0x2a4($sp)
/* 076ED4 7F0423A4 4480A000 */  mtc1  $zero, $f20
/* 076ED8 7F0423A8 8E260000 */  lw    $a2, ($s1)
/* 076EDC 7F0423AC 8FA40048 */  lw    $a0, 0x48($sp)
.L7F0423B0:
/* 076EE0 7F0423B0 27A50268 */  addiu $a1, $sp, 0x268
/* 076EE4 7F0423B4 0FC2DE9E */  jal   bgTestBulletHitBackground
/* 076EE8 7F0423B8 27A70274 */   addiu $a3, $sp, 0x274
/* 076EEC 7F0423BC 50400092 */  beql  $v0, $zero, .L7F042608
/* 076EF0 7F0423C0 8E260004 */   lw    $a2, 4($s1)
/* 076EF4 7F0423C4 0FC2D20C */  jal   get_room_data_float2
/* 076EF8 7F0423C8 00000000 */   nop
/* 076EFC 7F0423CC C7A60274 */  lwc1  $f6, 0x274($sp)
/* 076F00 7F0423D0 C7AA0278 */  lwc1  $f10, 0x278($sp)
/* 076F04 7F0423D4 C7B2027C */  lwc1  $f18, 0x27c($sp)
/* 076F08 7F0423D8 46003202 */  mul.s $f8, $f6, $f0
/* 076F0C 7F0423DC C7A60268 */  lwc1  $f6, 0x268($sp)
/* 076F10 7F0423E0 46005402 */  mul.s $f16, $f10, $f0
/* 076F14 7F0423E4 C7AA0268 */  lwc1  $f10, 0x268($sp)
/* 076F18 7F0423E8 46009102 */  mul.s $f4, $f18, $f0
/* 076F1C 7F0423EC E7A80274 */  swc1  $f8, 0x274($sp)
/* 076F20 7F0423F0 E7B00278 */  swc1  $f16, 0x278($sp)
/* 076F24 7F0423F4 E7A4027C */  swc1  $f4, 0x27c($sp)
/* 076F28 7F0423F8 C64C0058 */  lwc1  $f12, 0x58($s2)
/* 076F2C 7F0423FC 4606603E */  c.le.s $f12, $f6
/* 076F30 7F042400 00000000 */  nop
/* 076F34 7F042404 4502000A */  bc1fl .L7F042430
/* 076F38 7F042408 460C503E */   c.le.s $f10, $f12
/* 076F3C 7F04240C 4606403E */  c.le.s $f8, $f6
/* 076F40 7F042410 00000000 */  nop
/* 076F44 7F042414 45020006 */  bc1fl .L7F042430
/* 076F48 7F042418 460C503E */   c.le.s $f10, $f12
/* 076F4C 7F04241C 4608603E */  c.le.s $f12, $f8
/* 076F50 7F042420 00000000 */  nop
/* 076F54 7F042424 4503000E */  bc1tl .L7F042460
/* 076F58 7F042428 C642005C */   lwc1  $f2, 0x5c($s2)
/* 076F5C 7F04242C 460C503E */  c.le.s $f10, $f12
.L7F042430:
/* 076F60 7F042430 C7B00274 */  lwc1  $f16, 0x274($sp)
/* 076F64 7F042434 45020074 */  bc1fl .L7F042608
/* 076F68 7F042438 8E260004 */   lw    $a2, 4($s1)
/* 076F6C 7F04243C 4610503E */  c.le.s $f10, $f16
/* 076F70 7F042440 00000000 */  nop
/* 076F74 7F042444 45020070 */  bc1fl .L7F042608
/* 076F78 7F042448 8E260004 */   lw    $a2, 4($s1)
/* 076F7C 7F04244C 460C803E */  c.le.s $f16, $f12
/* 076F80 7F042450 00000000 */  nop
/* 076F84 7F042454 4502006C */  bc1fl .L7F042608
/* 076F88 7F042458 8E260004 */   lw    $a2, 4($s1)
/* 076F8C 7F04245C C642005C */  lwc1  $f2, 0x5c($s2)
.L7F042460:
/* 076F90 7F042460 C7B2026C */  lwc1  $f18, 0x26c($sp)
/* 076F94 7F042464 C7A40278 */  lwc1  $f4, 0x278($sp)
/* 076F98 7F042468 C7A6026C */  lwc1  $f6, 0x26c($sp)
/* 076F9C 7F04246C 4612103E */  c.le.s $f2, $f18
/* 076FA0 7F042470 00000000 */  nop
/* 076FA4 7F042474 4502000A */  bc1fl .L7F0424A0
/* 076FA8 7F042478 4602303E */   c.le.s $f6, $f2
/* 076FAC 7F04247C 4612203E */  c.le.s $f4, $f18
/* 076FB0 7F042480 00000000 */  nop
/* 076FB4 7F042484 45020006 */  bc1fl .L7F0424A0
/* 076FB8 7F042488 4602303E */   c.le.s $f6, $f2
/* 076FBC 7F04248C 4604103E */  c.le.s $f2, $f4
/* 076FC0 7F042490 00000000 */  nop
/* 076FC4 7F042494 4503000E */  bc1tl .L7F0424D0
/* 076FC8 7F042498 C6400060 */   lwc1  $f0, 0x60($s2)
/* 076FCC 7F04249C 4602303E */  c.le.s $f6, $f2
.L7F0424A0:
/* 076FD0 7F0424A0 C7A80278 */  lwc1  $f8, 0x278($sp)
/* 076FD4 7F0424A4 45020058 */  bc1fl .L7F042608
/* 076FD8 7F0424A8 8E260004 */   lw    $a2, 4($s1)
/* 076FDC 7F0424AC 4608303E */  c.le.s $f6, $f8
/* 076FE0 7F0424B0 00000000 */  nop
/* 076FE4 7F0424B4 45020054 */  bc1fl .L7F042608
/* 076FE8 7F0424B8 8E260004 */   lw    $a2, 4($s1)
/* 076FEC 7F0424BC 4602403E */  c.le.s $f8, $f2
/* 076FF0 7F0424C0 00000000 */  nop
/* 076FF4 7F0424C4 45020050 */  bc1fl .L7F042608
/* 076FF8 7F0424C8 8E260004 */   lw    $a2, 4($s1)
/* 076FFC 7F0424CC C6400060 */  lwc1  $f0, 0x60($s2)
.L7F0424D0:
/* 077000 7F0424D0 C7AA0270 */  lwc1  $f10, 0x270($sp)
/* 077004 7F0424D4 C7B0027C */  lwc1  $f16, 0x27c($sp)
/* 077008 7F0424D8 C7B20270 */  lwc1  $f18, 0x270($sp)
/* 07700C 7F0424DC 460A003E */  c.le.s $f0, $f10
/* 077010 7F0424E0 00000000 */  nop
/* 077014 7F0424E4 4502000A */  bc1fl .L7F042510
/* 077018 7F0424E8 4600903E */   c.le.s $f18, $f0
/* 07701C 7F0424EC 460A803E */  c.le.s $f16, $f10
/* 077020 7F0424F0 00000000 */  nop
/* 077024 7F0424F4 45020006 */  bc1fl .L7F042510
/* 077028 7F0424F8 4600903E */   c.le.s $f18, $f0
/* 07702C 7F0424FC 4610003E */  c.le.s $f0, $f16
/* 077030 7F042500 00000000 */  nop
/* 077034 7F042504 4503000E */  bc1tl .L7F042540
/* 077038 7F042508 C7A60274 */   lwc1  $f6, 0x274($sp)
/* 07703C 7F04250C 4600903E */  c.le.s $f18, $f0
.L7F042510:
/* 077040 7F042510 C7A4027C */  lwc1  $f4, 0x27c($sp)
/* 077044 7F042514 4502003C */  bc1fl .L7F042608
/* 077048 7F042518 8E260004 */   lw    $a2, 4($s1)
/* 07704C 7F04251C 4604903E */  c.le.s $f18, $f4
/* 077050 7F042520 00000000 */  nop
/* 077054 7F042524 45020038 */  bc1fl .L7F042608
/* 077058 7F042528 8E260004 */   lw    $a2, 4($s1)
/* 07705C 7F04252C 4600203E */  c.le.s $f4, $f0
/* 077060 7F042530 00000000 */  nop
/* 077064 7F042534 45020034 */  bc1fl .L7F042608
/* 077068 7F042538 8E260004 */   lw    $a2, 4($s1)
/* 07706C 7F04253C C7A60274 */  lwc1  $f6, 0x274($sp)
.L7F042540:
/* 077070 7F042540 C7A80278 */  lwc1  $f8, 0x278($sp)
/* 077074 7F042544 46066032 */  c.eq.s $f12, $f6
/* 077078 7F042548 00000000 */  nop
/* 07707C 7F04254C 4502000A */  bc1fl .L7F042578
/* 077080 7F042550 C7B00274 */   lwc1  $f16, 0x274($sp)
/* 077084 7F042554 46081032 */  c.eq.s $f2, $f8
/* 077088 7F042558 C7AA027C */  lwc1  $f10, 0x27c($sp)
/* 07708C 7F04255C 45020006 */  bc1fl .L7F042578
/* 077090 7F042560 C7B00274 */   lwc1  $f16, 0x274($sp)
/* 077094 7F042564 460A0032 */  c.eq.s $f0, $f10
/* 077098 7F042568 00000000 */  nop
/* 07709C 7F04256C 45030026 */  bc1tl .L7F042608
/* 0770A0 7F042570 8E260004 */   lw    $a2, 4($s1)
/* 0770A4 7F042574 C7B00274 */  lwc1  $f16, 0x274($sp)
.L7F042578:
/* 0770A8 7F042578 AFA002A4 */  sw    $zero, 0x2a4($sp)
/* 0770AC 7F04257C 02002025 */  move  $a0, $s0
/* 0770B0 7F042580 E6700000 */  swc1  $f16, ($s3)
/* 0770B4 7F042584 C7B20278 */  lwc1  $f18, 0x278($sp)
/* 0770B8 7F042588 26050004 */  addiu $a1, $s0, 4
/* 0770BC 7F04258C E6720004 */  swc1  $f18, 4($s3)
/* 0770C0 7F042590 C7A4027C */  lwc1  $f4, 0x27c($sp)
/* 0770C4 7F042594 E6640008 */  swc1  $f4, 8($s3)
/* 0770C8 7F042598 C7A60280 */  lwc1  $f6, 0x280($sp)
/* 0770CC 7F04259C E6060000 */  swc1  $f6, ($s0)
/* 0770D0 7F0425A0 C6100000 */  lwc1  $f16, ($s0)
/* 0770D4 7F0425A4 C7A80284 */  lwc1  $f8, 0x284($sp)
/* 0770D8 7F0425A8 4610A032 */  c.eq.s $f20, $f16
/* 0770DC 7F0425AC E6080004 */  swc1  $f8, 4($s0)
/* 0770E0 7F0425B0 C7AA0288 */  lwc1  $f10, 0x288($sp)
/* 0770E4 7F0425B4 4500000C */  bc1f  .L7F0425E8
/* 0770E8 7F0425B8 E60A0008 */   swc1  $f10, 8($s0)
/* 0770EC 7F0425BC C6120004 */  lwc1  $f18, 4($s0)
/* 0770F0 7F0425C0 4612A032 */  c.eq.s $f20, $f18
/* 0770F4 7F0425C4 00000000 */  nop
/* 0770F8 7F0425C8 45000007 */  bc1f  .L7F0425E8
/* 0770FC 7F0425CC 00000000 */   nop
/* 077100 7F0425D0 C6040008 */  lwc1  $f4, 8($s0)
/* 077104 7F0425D4 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 077108 7F0425D8 4604A032 */  c.eq.s $f20, $f4
/* 07710C 7F0425DC 00000000 */  nop
/* 077110 7F0425E0 45030006 */  bc1tl .L7F0425FC
/* 077114 7F0425E4 44813000 */   mtc1  $at, $f6
.L7F0425E8:
/* 077118 7F0425E8 0C007DD4 */  jal   guNormalize
/* 07711C 7F0425EC 26060008 */   addiu $a2, $s0, 8
/* 077120 7F0425F0 10000005 */  b     .L7F042608
/* 077124 7F0425F4 8E260004 */   lw    $a2, 4($s1)
/* 077128 7F0425F8 44813000 */  mtc1  $at, $f6
.L7F0425FC:
/* 07712C 7F0425FC 00000000 */  nop
/* 077130 7F042600 E6060008 */  swc1  $f6, 8($s0)
/* 077134 7F042604 8E260004 */  lw    $a2, 4($s1)
.L7F042608:
/* 077138 7F042608 26310004 */  addiu $s1, $s1, 4
/* 07713C 7F04260C 04C3FF68 */  bgezl $a2, .L7F0423B0
/* 077140 7F042610 8FA40048 */   lw    $a0, 0x48($sp)
/* 077144 7F042614 8FAD02A4 */  lw    $t5, 0x2a4($sp)
.L7F042618:
/* 077148 7F042618 8FA202B4 */  lw    $v0, 0x2b4($sp)
/* 07714C 7F04261C 55A00037 */  bnezl $t5, .L7F0426FC
/* 077150 7F042620 27AE007C */   addiu $t6, $sp, 0x7c
/* 077154 7F042624 C4480000 */  lwc1  $f8, ($v0)
/* 077158 7F042628 C64A0058 */  lwc1  $f10, 0x58($s2)
/* 07715C 7F04262C 460A4401 */  sub.s $f16, $f8, $f10
/* 077160 7F042630 E7B00064 */  swc1  $f16, 0x64($sp)
/* 077164 7F042634 C644005C */  lwc1  $f4, 0x5c($s2)
/* 077168 7F042638 C4520004 */  lwc1  $f18, 4($v0)
/* 07716C 7F04263C 46049181 */  sub.s $f6, $f18, $f4
/* 077170 7F042640 C7B20064 */  lwc1  $f18, 0x64($sp)
/* 077174 7F042644 E7A60068 */  swc1  $f6, 0x68($sp)
/* 077178 7F042648 C64A0060 */  lwc1  $f10, 0x60($s2)
/* 07717C 7F04264C C4480008 */  lwc1  $f8, 8($v0)
/* 077180 7F042650 46129102 */  mul.s $f4, $f18, $f18
/* 077184 7F042654 C7A60068 */  lwc1  $f6, 0x68($sp)
/* 077188 7F042658 460A4401 */  sub.s $f16, $f8, $f10
/* 07718C 7F04265C 46063202 */  mul.s $f8, $f6, $f6
/* 077190 7F042660 E7B0006C */  swc1  $f16, 0x6c($sp)
/* 077194 7F042664 C7B0006C */  lwc1  $f16, 0x6c($sp)
/* 077198 7F042668 46108482 */  mul.s $f18, $f16, $f16
/* 07719C 7F04266C 46082280 */  add.s $f10, $f4, $f8
/* 0771A0 7F042670 0C007DF8 */  jal   sqrtf
/* 0771A4 7F042674 460A9300 */   add.s $f12, $f18, $f10
/* 0771A8 7F042678 3C018005 */  lui    $at, %hi(D_80052A58) # -0.000000
/* 0771AC 7F04267C C42C2A58 */  lwc1  $f12, %lo(D_80052A58)($at)
/* 0771B0 7F042680 3C013F00 */  li    $at, 0x3F000000 # 0.500000
/* 0771B4 7F042684 C7A40064 */  lwc1  $f4, 0x64($sp)
/* 0771B8 7F042688 4600603C */  c.lt.s $f12, $f0
/* 0771BC 7F04268C 00000000 */  nop
/* 0771C0 7F042690 45020004 */  bc1fl .L7F0426A4
/* 0771C4 7F042694 44811000 */   mtc1  $at, $f2
/* 0771C8 7F042698 10000003 */  b     .L7F0426A8
/* 0771CC 7F04269C 46006083 */   div.s $f2, $f12, $f0
/* 0771D0 7F0426A0 44811000 */  mtc1  $at, $f2
.L7F0426A4:
/* 0771D4 7F0426A4 00000000 */  nop
.L7F0426A8:
/* 0771D8 7F0426A8 46041202 */  mul.s $f8, $f2, $f4
/* 0771DC 7F0426AC C6660000 */  lwc1  $f6, ($s3)
/* 0771E0 7F0426B0 C6720004 */  lwc1  $f18, 4($s3)
/* 0771E4 7F0426B4 46083401 */  sub.s $f16, $f6, $f8
/* 0771E8 7F0426B8 C6680008 */  lwc1  $f8, 8($s3)
/* 0771EC 7F0426BC E6700000 */  swc1  $f16, ($s3)
/* 0771F0 7F0426C0 C7AA0068 */  lwc1  $f10, 0x68($sp)
/* 0771F4 7F0426C4 460A1102 */  mul.s $f4, $f2, $f10
/* 0771F8 7F0426C8 46049181 */  sub.s $f6, $f18, $f4
/* 0771FC 7F0426CC C6640000 */  lwc1  $f4, ($s3)
/* 077200 7F0426D0 E6660004 */  swc1  $f6, 4($s3)
/* 077204 7F0426D4 C7B0006C */  lwc1  $f16, 0x6c($sp)
/* 077208 7F0426D8 46101282 */  mul.s $f10, $f2, $f16
/* 07720C 7F0426DC 460A4481 */  sub.s $f18, $f8, $f10
/* 077210 7F0426E0 E6720008 */  swc1  $f18, 8($s3)
/* 077214 7F0426E4 E7A40268 */  swc1  $f4, 0x268($sp)
/* 077218 7F0426E8 C6660004 */  lwc1  $f6, 4($s3)
/* 07721C 7F0426EC E7A6026C */  swc1  $f6, 0x26c($sp)
/* 077220 7F0426F0 C6700008 */  lwc1  $f16, 8($s3)
/* 077224 7F0426F4 E7B00270 */  swc1  $f16, 0x270($sp)
/* 077228 7F0426F8 27AE007C */  addiu $t6, $sp, 0x7c
.L7F0426FC:
/* 07722C 7F0426FC AFAE0018 */  sw    $t6, 0x18($sp)
/* 077230 7F042700 8FA402AC */  lw    $a0, 0x2ac($sp)
/* 077234 7F042704 8FA50048 */  lw    $a1, 0x48($sp)
/* 077238 7F042708 27A60268 */  addiu $a2, $sp, 0x268
/* 07723C 7F04270C 2407001F */  li    $a3, 31
/* 077240 7F042710 AFB30010 */  sw    $s3, 0x10($sp)
/* 077244 7F042714 0FC10783 */  jal   projectileFindCollidingProp
/* 077248 7F042718 AFB00014 */   sw    $s0, 0x14($sp)
/* 07724C 7F04271C 14400036 */  bnez  $v0, .L7F0427F8
/* 077250 7F042720 8FAB02C4 */   lw    $t3, 0x2c4($sp)
/* 077254 7F042724 8FAF02A4 */  lw    $t7, 0x2a4($sp)
/* 077258 7F042728 8FB802C0 */  lw    $t8, 0x2c0($sp)
/* 07725C 7F04272C 8FAD02A4 */  lw    $t5, 0x2a4($sp)
/* 077260 7F042730 55E00016 */  bnezl $t7, .L7F04278C
/* 077264 7F042734 24100001 */   li    $s0, 1
/* 077268 7F042738 53000014 */  beql  $t8, $zero, .L7F04278C
/* 07726C 7F04273C 24100001 */   li    $s0, 1
/* 077270 7F042740 8FA902C4 */  lw    $t1, 0x2c4($sp)
/* 077274 7F042744 24190002 */  li    $t9, 2
/* 077278 7F042748 AFB902A4 */  sw    $t9, 0x2a4($sp)
/* 07727C 7F04274C 1520000C */  bnez  $t1, .L7F042780
/* 077280 7F042750 8FA40048 */   lw    $a0, 0x48($sp)
/* 077284 7F042754 8E46006C */  lw    $a2, 0x6c($s2)
/* 077288 7F042758 27AA007C */  addiu $t2, $sp, 0x7c
/* 07728C 7F04275C 27AB0260 */  addiu $t3, $sp, 0x260
/* 077290 7F042760 240C0014 */  li    $t4, 20
/* 077294 7F042764 AFAC0018 */  sw    $t4, 0x18($sp)
/* 077298 7F042768 AFAB0014 */  sw    $t3, 0x14($sp)
/* 07729C 7F04276C AFAA0010 */  sw    $t2, 0x10($sp)
/* 0772A0 7F042770 02602825 */  move  $a1, $s3
/* 0772A4 7F042774 27A70074 */  addiu $a3, $sp, 0x74
/* 0772A8 7F042778 0FC2D2AD */  jal   sub_GAME_7F0B4AB4
/* 0772AC 7F04277C 24C600CC */   addiu $a2, $a2, 0xcc
.L7F042780:
/* 0772B0 7F042780 1000003A */  b     .L7F04286C
/* 0772B4 7F042784 8FA902A4 */   lw    $t1, 0x2a4($sp)
/* 0772B8 7F042788 24100001 */  li    $s0, 1
.L7F04278C:
/* 0772BC 7F04278C 11B00003 */  beq   $t5, $s0, .L7F04279C
/* 0772C0 7F042790 8FAE02C4 */   lw    $t6, 0x2c4($sp)
/* 0772C4 7F042794 55C00035 */  bnezl $t6, .L7F04286C
/* 0772C8 7F042798 8FA902A4 */   lw    $t1, 0x2a4($sp)
.L7F04279C:
/* 0772CC 7F04279C 8FAF02A4 */  lw    $t7, 0x2a4($sp)
/* 0772D0 7F0427A0 8FA40048 */  lw    $a0, 0x48($sp)
/* 0772D4 7F0427A4 27A50268 */  addiu $a1, $sp, 0x268
/* 0772D8 7F0427A8 11F0000A */  beq   $t7, $s0, .L7F0427D4
/* 0772DC 7F0427AC 27A70074 */   addiu $a3, $sp, 0x74
/* 0772E0 7F0427B0 8E46006C */  lw    $a2, 0x6c($s2)
/* 0772E4 7F0427B4 27B8007C */  addiu $t8, $sp, 0x7c
/* 0772E8 7F0427B8 27B90260 */  addiu $t9, $sp, 0x260
/* 0772EC 7F0427BC 24090014 */  li    $t1, 20
/* 0772F0 7F0427C0 AFA90018 */  sw    $t1, 0x18($sp)
/* 0772F4 7F0427C4 AFB90014 */  sw    $t9, 0x14($sp)
/* 0772F8 7F0427C8 AFB80010 */  sw    $t8, 0x10($sp)
/* 0772FC 7F0427CC 0FC2D2AD */  jal   sub_GAME_7F0B4AB4
/* 077300 7F0427D0 24C600CC */   addiu $a2, $a2, 0xcc
.L7F0427D4:
/* 077304 7F0427D4 C7A80268 */  lwc1  $f8, 0x268($sp)
/* 077308 7F0427D8 E6480058 */  swc1  $f8, 0x58($s2)
/* 07730C 7F0427DC C7AA0270 */  lwc1  $f10, 0x270($sp)
/* 077310 7F0427E0 E64A0060 */  swc1  $f10, 0x60($s2)
/* 077314 7F0427E4 8FAA02AC */  lw    $t2, 0x2ac($sp)
/* 077318 7F0427E8 C7A0026C */  lwc1  $f0, 0x26c($sp)
/* 07731C 7F0427EC E540000C */  swc1  $f0, 0xc($t2)
/* 077320 7F0427F0 1000001D */  b     .L7F042868
/* 077324 7F0427F4 E640005C */   swc1  $f0, 0x5c($s2)
.L7F0427F8:
/* 077328 7F0427F8 1560001B */  bnez  $t3, .L7F042868
/* 07732C 7F0427FC 27A50268 */   addiu $a1, $sp, 0x268
/* 077330 7F042800 C6520058 */  lwc1  $f18, 0x58($s2)
/* 077334 7F042804 27AC007C */  addiu $t4, $sp, 0x7c
/* 077338 7F042808 27AD0260 */  addiu $t5, $sp, 0x260
/* 07733C 7F04280C E7B20268 */  swc1  $f18, 0x268($sp)
/* 077340 7F042810 C6440060 */  lwc1  $f4, 0x60($s2)
/* 077344 7F042814 240E0014 */  li    $t6, 20
/* 077348 7F042818 8FA40048 */  lw    $a0, 0x48($sp)
/* 07734C 7F04281C E7A40270 */  swc1  $f4, 0x270($sp)
/* 077350 7F042820 8E46006C */  lw    $a2, 0x6c($s2)
/* 077354 7F042824 AFAE0018 */  sw    $t6, 0x18($sp)
/* 077358 7F042828 AFAD0014 */  sw    $t5, 0x14($sp)
/* 07735C 7F04282C AFAC0010 */  sw    $t4, 0x10($sp)
/* 077360 7F042830 27A70074 */  addiu $a3, $sp, 0x74
/* 077364 7F042834 0FC2D2AD */  jal   sub_GAME_7F0B4AB4
/* 077368 7F042838 24C600CC */   addiu $a2, $a2, 0xcc
/* 07736C 7F04283C C7A0026C */  lwc1  $f0, 0x26c($sp)
/* 077370 7F042840 8FAF02AC */  lw    $t7, 0x2ac($sp)
/* 077374 7F042844 24190002 */  li    $t9, 2
/* 077378 7F042848 E5E0000C */  swc1  $f0, 0xc($t7)
/* 07737C 7F04284C E640005C */  swc1  $f0, 0x5c($s2)
/* 077380 7F042850 8FB802C0 */  lw    $t8, 0x2c0($sp)
/* 077384 7F042854 53000004 */  beql  $t8, $zero, .L7F042868
/* 077388 7F042858 AFA002A4 */   sw    $zero, 0x2a4($sp)
/* 07738C 7F04285C 10000002 */  b     .L7F042868
/* 077390 7F042860 AFB902A4 */   sw    $t9, 0x2a4($sp)
/* 077394 7F042864 AFA002A4 */  sw    $zero, 0x2a4($sp)
.L7F042868:
/* 077398 7F042868 8FA902A4 */  lw    $t1, 0x2a4($sp)
.L7F04286C:
/* 07739C 7F04286C 24100001 */  li    $s0, 1
/* 0773A0 7F042870 8FAA02C4 */  lw    $t2, 0x2c4($sp)
/* 0773A4 7F042874 51300004 */  beql  $t1, $s0, .L7F042888
/* 0773A8 7F042878 8E4B006C */   lw    $t3, 0x6c($s2)
/* 0773AC 7F04287C 5540005B */  bnezl $t2, .L7F0429EC
/* 0773B0 7F042880 8FBF003C */   lw    $ra, 0x3c($sp)
/* 0773B4 7F042884 8E4B006C */  lw    $t3, 0x6c($s2)
.L7F042888:
/* 0773B8 7F042888 00004025 */  move  $t0, $zero
/* 0773BC 7F04288C 8D620000 */  lw    $v0, ($t3)
/* 0773C0 7F042890 304C0008 */  andi  $t4, $v0, 8
/* 0773C4 7F042894 15800028 */  bnez  $t4, .L7F042938
/* 0773C8 7F042898 01801025 */   move  $v0, $t4
/* 0773CC 7F04289C 8FA202AC */  lw    $v0, 0x2ac($sp)
/* 0773D0 7F0428A0 27A402A8 */  addiu $a0, $sp, 0x2a8
/* 0773D4 7F0428A4 8C4D0014 */  lw    $t5, 0x14($v0)
/* 0773D8 7F0428A8 AFAD02A8 */  sw    $t5, 0x2a8($sp)
/* 0773DC 7F0428AC C6460060 */  lwc1  $f6, 0x60($s2)
/* 0773E0 7F0428B0 8E470058 */  lw    $a3, 0x58($s2)
/* 0773E4 7F0428B4 8C460010 */  lw    $a2, 0x10($v0)
/* 0773E8 7F0428B8 8C450008 */  lw    $a1, 8($v0)
/* 0773EC 7F0428BC AFA00264 */  sw    $zero, 0x264($sp)
/* 0773F0 7F0428C0 0FC2C2F9 */  jal   walkTilesBetweenPoints_NoCallback
/* 0773F4 7F0428C4 E7A60010 */   swc1  $f6, 0x10($sp)
/* 0773F8 7F0428C8 1450000F */  bne   $v0, $s0, .L7F042908
/* 0773FC 7F0428CC 8FA80264 */   lw    $t0, 0x264($sp)
/* 077400 7F0428D0 8FAE02A8 */  lw    $t6, 0x2a8($sp)
/* 077404 7F0428D4 8FA302AC */  lw    $v1, 0x2ac($sp)
/* 077408 7F0428D8 51C0000C */  beql  $t6, $zero, .L7F04290C
/* 07740C 7F0428DC 8E43006C */   lw    $v1, 0x6c($s2)
/* 077410 7F0428E0 AC6E0014 */  sw    $t6, 0x14($v1)
/* 077414 7F0428E4 C6500058 */  lwc1  $f16, 0x58($s2)
/* 077418 7F0428E8 E4700008 */  swc1  $f16, 8($v1)
/* 07741C 7F0428EC C6480060 */  lwc1  $f8, 0x60($s2)
/* 077420 7F0428F0 E4680010 */  swc1  $f8, 0x10($v1)
/* 077424 7F0428F4 8E4F006C */  lw    $t7, 0x6c($s2)
/* 077428 7F0428F8 8DE20000 */  lw    $v0, ($t7)
/* 07742C 7F0428FC 30580008 */  andi  $t8, $v0, 8
/* 077430 7F042900 1000000D */  b     .L7F042938
/* 077434 7F042904 03001025 */   move  $v0, $t8
.L7F042908:
/* 077438 7F042908 8E43006C */  lw    $v1, 0x6c($s2)
.L7F04290C:
/* 07743C 7F04290C 8C790000 */  lw    $t9, ($v1)
/* 077440 7F042910 37290008 */  ori   $t1, $t9, 8
/* 077444 7F042914 AC690000 */  sw    $t1, ($v1)
/* 077448 7F042918 8FAA02AC */  lw    $t2, 0x2ac($sp)
/* 07744C 7F04291C 914B0001 */  lbu   $t3, 1($t2)
/* 077450 7F042920 356C0008 */  ori   $t4, $t3, 8
/* 077454 7F042924 A14C0001 */  sb    $t4, 1($t2)
/* 077458 7F042928 8E4D006C */  lw    $t5, 0x6c($s2)
/* 07745C 7F04292C 8DA20000 */  lw    $v0, ($t5)
/* 077460 7F042930 304E0008 */  andi  $t6, $v0, 8
/* 077464 7F042934 01C01025 */  move  $v0, $t6
.L7F042938:
/* 077468 7F042938 10400016 */  beqz  $v0, .L7F042994
/* 07746C 7F04293C 8FA40048 */   lw    $a0, 0x48($sp)
/* 077470 7F042940 27A50074 */  addiu $a1, $sp, 0x74
/* 077474 7F042944 00003025 */  move  $a2, $zero
/* 077478 7F042948 0FC2BC83 */  jal   sub_GAME_7F0AF20C
/* 07747C 7F04294C AFA80264 */   sw    $t0, 0x264($sp)
/* 077480 7F042950 8FA80264 */  lw    $t0, 0x264($sp)
/* 077484 7F042954 1040000F */  beqz  $v0, .L7F042994
/* 077488 7F042958 AFA202A8 */   sw    $v0, 0x2a8($sp)
/* 07748C 7F04295C 8FA402AC */  lw    $a0, 0x2ac($sp)
/* 077490 7F042960 2401FFF7 */  li    $at, -9
/* 077494 7F042964 AC820014 */  sw    $v0, 0x14($a0)
/* 077498 7F042968 C64A0058 */  lwc1  $f10, 0x58($s2)
/* 07749C 7F04296C E48A0008 */  swc1  $f10, 8($a0)
/* 0774A0 7F042970 C6520060 */  lwc1  $f18, 0x60($s2)
/* 0774A4 7F042974 E4920010 */  swc1  $f18, 0x10($a0)
/* 0774A8 7F042978 8E43006C */  lw    $v1, 0x6c($s2)
/* 0774AC 7F04297C 8C6F0000 */  lw    $t7, ($v1)
/* 0774B0 7F042980 01E1C024 */  and   $t8, $t7, $at
/* 0774B4 7F042984 AC780000 */  sw    $t8, ($v1)
/* 0774B8 7F042988 90990001 */  lbu   $t9, 1($a0)
/* 0774BC 7F04298C 03214824 */  and   $t1, $t9, $at
/* 0774C0 7F042990 A0890001 */  sb    $t1, 1($a0)
.L7F042994:
/* 0774C4 7F042994 93AB0074 */  lbu   $t3, 0x74($sp)
/* 0774C8 7F042998 240400FF */  li    $a0, 255
/* 0774CC 7F04299C 27AC0074 */  addiu $t4, $sp, 0x74
/* 0774D0 7F0429A0 108B000D */  beq   $a0, $t3, .L7F0429D8
/* 0774D4 7F0429A4 010C1021 */   addu  $v0, $t0, $t4
/* 0774D8 7F0429A8 90430000 */  lbu   $v1, ($v0)
/* 0774DC 7F0429AC 24050007 */  li    $a1, 7
/* 0774E0 7F0429B0 8E4A006C */  lw    $t2, 0x6c($s2)
.L7F0429B4:
/* 0774E4 7F0429B4 24420001 */  addiu $v0, $v0, 1
/* 0774E8 7F0429B8 01486821 */  addu  $t5, $t2, $t0
/* 0774EC 7F0429BC A1A300CC */  sb    $v1, 0xcc($t5)
/* 0774F0 7F0429C0 90430000 */  lbu   $v1, ($v0)
/* 0774F4 7F0429C4 25080001 */  addiu $t0, $t0, 1
/* 0774F8 7F0429C8 50830004 */  beql  $a0, $v1, .L7F0429DC
/* 0774FC 7F0429CC 8E4F006C */   lw    $t7, 0x6c($s2)
/* 077500 7F0429D0 5505FFF8 */  bnel  $t0, $a1, .L7F0429B4
/* 077504 7F0429D4 8E4A006C */   lw    $t2, 0x6c($s2)
.L7F0429D8:
/* 077508 7F0429D8 8E4F006C */  lw    $t7, 0x6c($s2)
.L7F0429DC:
/* 07750C 7F0429DC 240E00FF */  li    $t6, 255
/* 077510 7F0429E0 01E8C021 */  addu  $t8, $t7, $t0
/* 077514 7F0429E4 A30E00CC */  sb    $t6, 0xcc($t8)
.L7F0429E8:
/* 077518 7F0429E8 8FBF003C */  lw    $ra, 0x3c($sp)
.L7F0429EC:
/* 07751C 7F0429EC 8FA202A4 */  lw    $v0, 0x2a4($sp)
/* 077520 7F0429F0 D7B40020 */  ldc1  $f20, 0x20($sp)
/* 077524 7F0429F4 8FB0002C */  lw    $s0, 0x2c($sp)
/* 077528 7F0429F8 8FB10030 */  lw    $s1, 0x30($sp)
/* 07752C 7F0429FC 8FB20034 */  lw    $s2, 0x34($sp)
/* 077530 7F042A00 8FB30038 */  lw    $s3, 0x38($sp)
/* 077534 7F042A04 03E00008 */  jr    $ra
/* 077538 7F042A08 27BD02B0 */   addiu $sp, $sp, 0x2b0
)
#endif





#ifdef NONMATCHING
void sub_GAME_7F042A0C(void) {

}
#else
GLOBAL_ASM(
.late_rodata
glabel D_80052A5C
.word 0x3f7d70a4 /*0.99000001*/
.text
glabel sub_GAME_7F042A0C
/* 07753C 7F042A0C 27BDFF40 */  addiu $sp, $sp, -0xc0
/* 077540 7F042A10 AFBF0034 */  sw    $ra, 0x34($sp)
/* 077544 7F042A14 AFB00030 */  sw    $s0, 0x30($sp)
/* 077548 7F042A18 AFA600C8 */  sw    $a2, 0xc8($sp)
/* 07754C 7F042A1C AFA700CC */  sw    $a3, 0xcc($sp)
/* 077550 7F042A20 8C900010 */  lw    $s0, 0x10($a0)
/* 077554 7F042A24 8E0E0014 */  lw    $t6, 0x14($s0)
/* 077558 7F042A28 AFA500C4 */  sw    $a1, 0xc4($sp)
/* 07755C 7F042A2C AFA400C0 */  sw    $a0, 0xc0($sp)
/* 077560 7F042A30 0FC10411 */  jal   objGetWidth
/* 077564 7F042A34 AFAE00B8 */   sw    $t6, 0xb8($sp)
/* 077568 7F042A38 8FA500C4 */  lw    $a1, 0xc4($sp)
/* 07756C 7F042A3C 240F0001 */  li    $t7, 1
/* 077570 7F042A40 3C018003 */  lui    $at, %hi(D_80030B0C)
/* 077574 7F042A44 E7A000B4 */  swc1  $f0, 0xb4($sp)
/* 077578 7F042A48 AFAF00B0 */  sw    $t7, 0xb0($sp)
/* 07757C 7F042A4C AC200B0C */  sw    $zero, %lo(D_80030B0C)($at)
/* 077580 7F042A50 C6040008 */  lwc1  $f4, 8($s0)
/* 077584 7F042A54 C4A20000 */  lwc1  $f2, ($a1)
/* 077588 7F042A58 46041032 */  c.eq.s $f2, $f4
/* 07758C 7F042A5C 00000000 */  nop
/* 077590 7F042A60 4502000E */  bc1fl .L7F042A9C
/* 077594 7F042A64 E7A20098 */   swc1  $f2, 0x98($sp)
/* 077598 7F042A68 C4A60004 */  lwc1  $f6, 4($a1)
/* 07759C 7F042A6C C608000C */  lwc1  $f8, 0xc($s0)
/* 0775A0 7F042A70 46083032 */  c.eq.s $f6, $f8
/* 0775A4 7F042A74 00000000 */  nop
/* 0775A8 7F042A78 45020008 */  bc1fl .L7F042A9C
/* 0775AC 7F042A7C E7A20098 */   swc1  $f2, 0x98($sp)
/* 0775B0 7F042A80 C4AA0008 */  lwc1  $f10, 8($a1)
/* 0775B4 7F042A84 C6100010 */  lwc1  $f16, 0x10($s0)
/* 0775B8 7F042A88 46105032 */  c.eq.s $f10, $f16
/* 0775BC 7F042A8C 00000000 */  nop
/* 0775C0 7F042A90 45030104 */  bc1tl .L7F042EA4
/* 0775C4 7F042A94 8FBF0034 */   lw    $ra, 0x34($sp)
/* 0775C8 7F042A98 E7A20098 */  swc1  $f2, 0x98($sp)
.L7F042A9C:
/* 0775CC 7F042A9C C4B20004 */  lwc1  $f18, 4($a1)
/* 0775D0 7F042AA0 8FB800C0 */  lw    $t8, 0xc0($sp)
/* 0775D4 7F042AA4 E7B2009C */  swc1  $f18, 0x9c($sp)
/* 0775D8 7F042AA8 C4A40008 */  lwc1  $f4, 8($a1)
/* 0775DC 7F042AAC E7A400A0 */  swc1  $f4, 0xa0($sp)
/* 0775E0 7F042AB0 8F190064 */  lw    $t9, 0x64($t8)
/* 0775E4 7F042AB4 33280080 */  andi  $t0, $t9, 0x80
/* 0775E8 7F042AB8 510000FA */  beql  $t0, $zero, .L7F042EA4
/* 0775EC 7F042ABC 8FBF0034 */   lw    $ra, 0x34($sp)
/* 0775F0 7F042AC0 8E040014 */  lw    $a0, 0x14($s0)
/* 0775F4 7F042AC4 8E050008 */  lw    $a1, 8($s0)
/* 0775F8 7F042AC8 0FC2CA5C */  jal   stanGetPositionYValue
/* 0775FC 7F042ACC 8E060010 */   lw    $a2, 0x10($s0)
/* 077600 7F042AD0 8FA900C0 */  lw    $t1, 0xc0($sp)
/* 077604 7F042AD4 C7AA009C */  lwc1  $f10, 0x9c($sp)
/* 077608 7F042AD8 8D2A006C */  lw    $t2, 0x6c($t1)
/* 07760C 7F042ADC 8D4B0000 */  lw    $t3, ($t2)
/* 077610 7F042AE0 316C0040 */  andi  $t4, $t3, 0x40
/* 077614 7F042AE4 55800008 */  bnezl $t4, .L7F042B08
/* 077618 7F042AE8 C602000C */   lwc1  $f2, 0xc($s0)
/* 07761C 7F042AEC 44809000 */  mtc1  $zero, $f18
/* 077620 7F042AF0 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 077624 7F042AF4 44818000 */  mtc1  $at, $f16
/* 077628 7F042AF8 E7B20090 */  swc1  $f18, 0x90($sp)
/* 07762C 7F042AFC 10000010 */  b     .L7F042B40
/* 077630 7F042B00 E7B0008C */   swc1  $f16, 0x8c($sp)
/* 077634 7F042B04 C602000C */  lwc1  $f2, 0xc($s0)
.L7F042B08:
/* 077638 7F042B08 C7A4009C */  lwc1  $f4, 0x9c($sp)
/* 07763C 7F042B0C 4602503C */  c.lt.s $f10, $f2
/* 077640 7F042B10 00000000 */  nop
/* 077644 7F042B14 45020007 */  bc1fl .L7F042B34
/* 077648 7F042B18 46002181 */   sub.s $f6, $f4, $f0
/* 07764C 7F042B1C 46001401 */  sub.s $f16, $f2, $f0
/* 077650 7F042B20 46005481 */  sub.s $f18, $f10, $f0
/* 077654 7F042B24 E7B00090 */  swc1  $f16, 0x90($sp)
/* 077658 7F042B28 10000005 */  b     .L7F042B40
/* 07765C 7F042B2C E7B2008C */   swc1  $f18, 0x8c($sp)
/* 077660 7F042B30 46002181 */  sub.s $f6, $f4, $f0
.L7F042B34:
/* 077664 7F042B34 46001201 */  sub.s $f8, $f2, $f0
/* 077668 7F042B38 E7A60090 */  swc1  $f6, 0x90($sp)
/* 07766C 7F042B3C E7A8008C */  swc1  $f8, 0x8c($sp)
.L7F042B40:
/* 077670 7F042B40 0FC2C731 */  jal   sub_GAME_7F0B1CC4
/* 077674 7F042B44 00000000 */   nop
/* 077678 7F042B48 8E050008 */  lw    $a1, 8($s0)
/* 07767C 7F042B4C 8E060010 */  lw    $a2, 0x10($s0)
/* 077680 7F042B50 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 077684 7F042B54 44813000 */  mtc1  $at, $f6
/* 077688 7F042B58 C7B000A0 */  lwc1  $f16, 0xa0($sp)
/* 07768C 7F042B5C C7AA0090 */  lwc1  $f10, 0x90($sp)
/* 077690 7F042B60 C7B2008C */  lwc1  $f18, 0x8c($sp)
/* 077694 7F042B64 44802000 */  mtc1  $zero, $f4
/* 077698 7F042B68 240D001F */  li    $t5, 31
/* 07769C 7F042B6C AFAD0014 */  sw    $t5, 0x14($sp)
/* 0776A0 7F042B70 27A400B8 */  addiu $a0, $sp, 0xb8
/* 0776A4 7F042B74 8FA70098 */  lw    $a3, 0x98($sp)
/* 0776A8 7F042B78 E7B00010 */  swc1  $f16, 0x10($sp)
/* 0776AC 7F042B7C E7A60024 */  swc1  $f6, 0x24($sp)
/* 0776B0 7F042B80 E7AA0018 */  swc1  $f10, 0x18($sp)
/* 0776B4 7F042B84 E7B2001C */  swc1  $f18, 0x1c($sp)
/* 0776B8 7F042B88 0FC2C389 */  jal   stanTestLineUnobstructed
/* 0776BC 7F042B8C E7A40020 */   swc1  $f4, 0x20($sp)
/* 0776C0 7F042B90 1040001D */  beqz  $v0, .L7F042C08
/* 0776C4 7F042B94 8FAE00B8 */   lw    $t6, 0xb8($sp)
/* 0776C8 7F042B98 11C0001B */  beqz  $t6, .L7F042C08
/* 0776CC 7F042B9C 27A400B8 */   addiu $a0, $sp, 0xb8
/* 0776D0 7F042BA0 C7A80090 */  lwc1  $f8, 0x90($sp)
/* 0776D4 7F042BA4 C7B0008C */  lwc1  $f16, 0x8c($sp)
/* 0776D8 7F042BA8 240F001F */  li    $t7, 31
/* 0776DC 7F042BAC AFAF0010 */  sw    $t7, 0x10($sp)
/* 0776E0 7F042BB0 8FA50098 */  lw    $a1, 0x98($sp)
/* 0776E4 7F042BB4 8FA600A0 */  lw    $a2, 0xa0($sp)
/* 0776E8 7F042BB8 8FA700B4 */  lw    $a3, 0xb4($sp)
/* 0776EC 7F042BBC E7A80014 */  swc1  $f8, 0x14($sp)
/* 0776F0 7F042BC0 0FC2C62E */  jal   stanTestVolume
/* 0776F4 7F042BC4 E7B00018 */   swc1  $f16, 0x18($sp)
/* 0776F8 7F042BC8 0441000F */  bgez  $v0, .L7F042C08
/* 0776FC 7F042BCC C7AA0098 */   lwc1  $f10, 0x98($sp)
/* 077700 7F042BD0 8FA200C0 */  lw    $v0, 0xc0($sp)
/* 077704 7F042BD4 E44A0058 */  swc1  $f10, 0x58($v0)
/* 077708 7F042BD8 C7B200A0 */  lwc1  $f18, 0xa0($sp)
/* 07770C 7F042BDC E4520060 */  swc1  $f18, 0x60($v0)
/* 077710 7F042BE0 8FB800B8 */  lw    $t8, 0xb8($sp)
/* 077714 7F042BE4 AE180014 */  sw    $t8, 0x14($s0)
/* 077718 7F042BE8 C7A40098 */  lwc1  $f4, 0x98($sp)
/* 07771C 7F042BEC E6040008 */  swc1  $f4, 8($s0)
/* 077720 7F042BF0 C7A600A0 */  lwc1  $f6, 0xa0($sp)
/* 077724 7F042BF4 E6060010 */  swc1  $f6, 0x10($s0)
/* 077728 7F042BF8 C7A0009C */  lwc1  $f0, 0x9c($sp)
/* 07772C 7F042BFC E600000C */  swc1  $f0, 0xc($s0)
/* 077730 7F042C00 100000A7 */  b     .L7F042EA0
/* 077734 7F042C04 E440005C */   swc1  $f0, 0x5c($v0)
.L7F042C08:
/* 077738 7F042C08 27A40080 */  addiu $a0, $sp, 0x80
/* 07773C 7F042C0C 0FC2CA2C */  jal   getCollisionEdge_maybe
/* 077740 7F042C10 27A50074 */   addiu $a1, $sp, 0x74
/* 077744 7F042C14 C7A8007C */  lwc1  $f8, 0x7c($sp)
/* 077748 7F042C18 C7B00088 */  lwc1  $f16, 0x88($sp)
/* 07774C 7F042C1C 8FA400CC */  lw    $a0, 0xcc($sp)
/* 077750 7F042C20 44801000 */  mtc1  $zero, $f2
/* 077754 7F042C24 46104281 */  sub.s $f10, $f8, $f16
/* 077758 7F042C28 24850004 */  addiu $a1, $a0, 4
/* 07775C 7F042C2C E4820004 */  swc1  $f2, 4($a0)
/* 077760 7F042C30 E48A0000 */  swc1  $f10, ($a0)
/* 077764 7F042C34 C4880000 */  lwc1  $f8, ($a0)
/* 077768 7F042C38 C7A40074 */  lwc1  $f4, 0x74($sp)
/* 07776C 7F042C3C C7B20080 */  lwc1  $f18, 0x80($sp)
/* 077770 7F042C40 46081032 */  c.eq.s $f2, $f8
/* 077774 7F042C44 46049181 */  sub.s $f6, $f18, $f4
/* 077778 7F042C48 45000007 */  bc1f  .L7F042C68
/* 07777C 7F042C4C E4860008 */   swc1  $f6, 8($a0)
/* 077780 7F042C50 C4900008 */  lwc1  $f16, 8($a0)
/* 077784 7F042C54 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 077788 7F042C58 46101032 */  c.eq.s $f2, $f16
/* 07778C 7F042C5C 00000000 */  nop
/* 077790 7F042C60 45030006 */  bc1tl .L7F042C7C
/* 077794 7F042C64 44815000 */   mtc1  $at, $f10
.L7F042C68:
/* 077798 7F042C68 0C007DD4 */  jal   guNormalize
/* 07779C 7F042C6C 24860008 */   addiu $a2, $a0, 8
/* 0777A0 7F042C70 10000005 */  b     .L7F042C88
/* 0777A4 7F042C74 C6000008 */   lwc1  $f0, 8($s0)
/* 0777A8 7F042C78 44815000 */  mtc1  $at, $f10
.L7F042C7C:
/* 0777AC 7F042C7C 00000000 */  nop
/* 0777B0 7F042C80 E48A0008 */  swc1  $f10, 8($a0)
/* 0777B4 7F042C84 C6000008 */  lwc1  $f0, 8($s0)
.L7F042C88:
/* 0777B8 7F042C88 C7B20098 */  lwc1  $f18, 0x98($sp)
/* 0777BC 7F042C8C C7A4009C */  lwc1  $f4, 0x9c($sp)
/* 0777C0 7F042C90 C7AA0098 */  lwc1  $f10, 0x98($sp)
/* 0777C4 7F042C94 46009032 */  c.eq.s $f18, $f0
/* 0777C8 7F042C98 00000000 */  nop
/* 0777CC 7F042C9C 4502000F */  bc1fl .L7F042CDC
/* 0777D0 7F042CA0 46005481 */   sub.s $f18, $f10, $f0
/* 0777D4 7F042CA4 C606000C */  lwc1  $f6, 0xc($s0)
/* 0777D8 7F042CA8 C7A800A0 */  lwc1  $f8, 0xa0($sp)
/* 0777DC 7F042CAC 46062032 */  c.eq.s $f4, $f6
/* 0777E0 7F042CB0 00000000 */  nop
/* 0777E4 7F042CB4 45020009 */  bc1fl .L7F042CDC
/* 0777E8 7F042CB8 46005481 */   sub.s $f18, $f10, $f0
/* 0777EC 7F042CBC C6100010 */  lwc1  $f16, 0x10($s0)
/* 0777F0 7F042CC0 8FA200C8 */  lw    $v0, 0xc8($sp)
/* 0777F4 7F042CC4 C7B20098 */  lwc1  $f18, 0x98($sp)
/* 0777F8 7F042CC8 46104032 */  c.eq.s $f8, $f16
/* 0777FC 7F042CCC 00000000 */  nop
/* 077800 7F042CD0 4503006A */  bc1tl .L7F042E7C
/* 077804 7F042CD4 E4520000 */   swc1  $f18, ($v0)
/* 077808 7F042CD8 46005481 */  sub.s $f18, $f10, $f0
.L7F042CDC:
/* 07780C 7F042CDC C7A4009C */  lwc1  $f4, 0x9c($sp)
/* 077810 7F042CE0 C7B000A0 */  lwc1  $f16, 0xa0($sp)
/* 077814 7F042CE4 26040008 */  addiu $a0, $s0, 8
/* 077818 7F042CE8 E7B200A4 */  swc1  $f18, 0xa4($sp)
/* 07781C 7F042CEC C606000C */  lwc1  $f6, 0xc($s0)
/* 077820 7F042CF0 27A500A4 */  addiu $a1, $sp, 0xa4
/* 077824 7F042CF4 8FA600C8 */  lw    $a2, 0xc8($sp)
/* 077828 7F042CF8 46062201 */  sub.s $f8, $f4, $f6
/* 07782C 7F042CFC E7A800A8 */  swc1  $f8, 0xa8($sp)
/* 077830 7F042D00 C60A0010 */  lwc1  $f10, 0x10($s0)
/* 077834 7F042D04 460A8481 */  sub.s $f18, $f16, $f10
/* 077838 7F042D08 0FC09893 */  jal   chrlvStanLineDirIntersection
/* 07783C 7F042D0C E7B200AC */   swc1  $f18, 0xac($sp)
/* 077840 7F042D10 C7A400B4 */  lwc1  $f4, 0xb4($sp)
/* 077844 7F042D14 C7B00080 */  lwc1  $f16, 0x80($sp)
/* 077848 7F042D18 C7AA0088 */  lwc1  $f10, 0x88($sp)
/* 07784C 7F042D1C E7A4005C */  swc1  $f4, 0x5c($sp)
/* 077850 7F042D20 C6060008 */  lwc1  $f6, 8($s0)
/* 077854 7F042D24 C7B20074 */  lwc1  $f18, 0x74($sp)
/* 077858 7F042D28 C7A4007C */  lwc1  $f4, 0x7c($sp)
/* 07785C 7F042D2C E7A60060 */  swc1  $f6, 0x60($sp)
/* 077860 7F042D30 C6080010 */  lwc1  $f8, 0x10($s0)
/* 077864 7F042D34 E7B00054 */  swc1  $f16, 0x54($sp)
/* 077868 7F042D38 E7AA0058 */  swc1  $f10, 0x58($sp)
/* 07786C 7F042D3C E7B2004C */  swc1  $f18, 0x4c($sp)
/* 077870 7F042D40 E7A40050 */  swc1  $f4, 0x50($sp)
/* 077874 7F042D44 E7A80064 */  swc1  $f8, 0x64($sp)
/* 077878 7F042D48 C6080008 */  lwc1  $f8, 8($s0)
/* 07787C 7F042D4C C7A60098 */  lwc1  $f6, 0x98($sp)
/* 077880 7F042D50 C7AA00A0 */  lwc1  $f10, 0xa0($sp)
/* 077884 7F042D54 27A4005C */  addiu $a0, $sp, 0x5c
/* 077888 7F042D58 46083401 */  sub.s $f16, $f6, $f8
/* 07788C 7F042D5C 27A50054 */  addiu $a1, $sp, 0x54
/* 077890 7F042D60 27A6004C */  addiu $a2, $sp, 0x4c
/* 077894 7F042D64 27A70044 */  addiu $a3, $sp, 0x44
/* 077898 7F042D68 E7B00044 */  swc1  $f16, 0x44($sp)
/* 07789C 7F042D6C C6120010 */  lwc1  $f18, 0x10($s0)
/* 0778A0 7F042D70 46125101 */  sub.s $f4, $f10, $f18
/* 0778A4 7F042D74 0FC2CCF7 */  jal   calculateNormalizedLineIntersection
/* 0778A8 7F042D78 E7A40048 */   swc1  $f4, 0x48($sp)
/* 0778AC 7F042D7C C7A80044 */  lwc1  $f8, 0x44($sp)
/* 0778B0 7F042D80 3C018005 */  lui   $at, %hi(D_80052A5C)
/* 0778B4 7F042D84 C4222A5C */  lwc1  $f2, %lo(D_80052A5C)($at)
/* 0778B8 7F042D88 46004402 */  mul.s $f16, $f8, $f0
/* 0778BC 7F042D8C 8E190014 */  lw    $t9, 0x14($s0)
/* 0778C0 7F042D90 C7A4009C */  lwc1  $f4, 0x9c($sp)
/* 0778C4 7F042D94 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 0778C8 7F042D98 AFB900B8 */  sw    $t9, 0xb8($sp)
/* 0778CC 7F042D9C C6060008 */  lwc1  $f6, 8($s0)
/* 0778D0 7F042DA0 E7A4006C */  swc1  $f4, 0x6c($sp)
/* 0778D4 7F042DA4 46028282 */  mul.s $f10, $f16, $f2
/* 0778D8 7F042DA8 C7B00048 */  lwc1  $f16, 0x48($sp)
/* 0778DC 7F042DAC 2408001F */  li    $t0, 31
/* 0778E0 7F042DB0 27A400B8 */  addiu $a0, $sp, 0xb8
/* 0778E4 7F042DB4 460A3480 */  add.s $f18, $f6, $f10
/* 0778E8 7F042DB8 46008182 */  mul.s $f6, $f16, $f0
/* 0778EC 7F042DBC C7B00090 */  lwc1  $f16, 0x90($sp)
/* 0778F0 7F042DC0 E7B20068 */  swc1  $f18, 0x68($sp)
/* 0778F4 7F042DC4 C6080010 */  lwc1  $f8, 0x10($s0)
/* 0778F8 7F042DC8 8FA70068 */  lw    $a3, 0x68($sp)
/* 0778FC 7F042DCC 46023282 */  mul.s $f10, $f6, $f2
/* 077900 7F042DD0 C7A6008C */  lwc1  $f6, 0x8c($sp)
/* 077904 7F042DD4 460A4480 */  add.s $f18, $f8, $f10
/* 077908 7F042DD8 44815000 */  mtc1  $at, $f10
/* 07790C 7F042DDC 44804000 */  mtc1  $zero, $f8
/* 077910 7F042DE0 E7B20070 */  swc1  $f18, 0x70($sp)
/* 077914 7F042DE4 8E060010 */  lw    $a2, 0x10($s0)
/* 077918 7F042DE8 8E050008 */  lw    $a1, 8($s0)
/* 07791C 7F042DEC C7A40070 */  lwc1  $f4, 0x70($sp)
/* 077920 7F042DF0 AFA80014 */  sw    $t0, 0x14($sp)
/* 077924 7F042DF4 E7B00018 */  swc1  $f16, 0x18($sp)
/* 077928 7F042DF8 E7A6001C */  swc1  $f6, 0x1c($sp)
/* 07792C 7F042DFC E7AA0024 */  swc1  $f10, 0x24($sp)
/* 077930 7F042E00 E7A80020 */  swc1  $f8, 0x20($sp)
/* 077934 7F042E04 0FC2C389 */  jal   stanTestLineUnobstructed
/* 077938 7F042E08 E7A40010 */   swc1  $f4, 0x10($sp)
/* 07793C 7F042E0C 1040001F */  beqz  $v0, .L7F042E8C
/* 077940 7F042E10 8FA900B8 */   lw    $t1, 0xb8($sp)
/* 077944 7F042E14 1120001D */  beqz  $t1, .L7F042E8C
/* 077948 7F042E18 27A400B8 */   addiu $a0, $sp, 0xb8
/* 07794C 7F042E1C C7B20090 */  lwc1  $f18, 0x90($sp)
/* 077950 7F042E20 C7A4008C */  lwc1  $f4, 0x8c($sp)
/* 077954 7F042E24 240A001F */  li    $t2, 31
/* 077958 7F042E28 AFAA0010 */  sw    $t2, 0x10($sp)
/* 07795C 7F042E2C 8FA50068 */  lw    $a1, 0x68($sp)
/* 077960 7F042E30 8FA60070 */  lw    $a2, 0x70($sp)
/* 077964 7F042E34 8FA700B4 */  lw    $a3, 0xb4($sp)
/* 077968 7F042E38 E7B20014 */  swc1  $f18, 0x14($sp)
/* 07796C 7F042E3C 0FC2C62E */  jal   stanTestVolume
/* 077970 7F042E40 E7A40018 */   swc1  $f4, 0x18($sp)
/* 077974 7F042E44 04410011 */  bgez  $v0, .L7F042E8C
/* 077978 7F042E48 C7B00068 */   lwc1  $f16, 0x68($sp)
/* 07797C 7F042E4C 8FAB00C0 */  lw    $t3, 0xc0($sp)
/* 077980 7F042E50 E5700058 */  swc1  $f16, 0x58($t3)
/* 077984 7F042E54 C7A60070 */  lwc1  $f6, 0x70($sp)
/* 077988 7F042E58 E5660060 */  swc1  $f6, 0x60($t3)
/* 07798C 7F042E5C 8FAC00B8 */  lw    $t4, 0xb8($sp)
/* 077990 7F042E60 AE0C0014 */  sw    $t4, 0x14($s0)
/* 077994 7F042E64 C7A80068 */  lwc1  $f8, 0x68($sp)
/* 077998 7F042E68 E6080008 */  swc1  $f8, 8($s0)
/* 07799C 7F042E6C C7AA0070 */  lwc1  $f10, 0x70($sp)
/* 0779A0 7F042E70 10000006 */  b     .L7F042E8C
/* 0779A4 7F042E74 E60A0010 */   swc1  $f10, 0x10($s0)
/* 0779A8 7F042E78 E4520000 */  swc1  $f18, ($v0)
.L7F042E7C:
/* 0779AC 7F042E7C C7A4009C */  lwc1  $f4, 0x9c($sp)
/* 0779B0 7F042E80 E4440004 */  swc1  $f4, 4($v0)
/* 0779B4 7F042E84 C7B000A0 */  lwc1  $f16, 0xa0($sp)
/* 0779B8 7F042E88 E4500008 */  swc1  $f16, 8($v0)
.L7F042E8C:
/* 0779BC 7F042E8C C7A0006C */  lwc1  $f0, 0x6c($sp)
/* 0779C0 7F042E90 E600000C */  swc1  $f0, 0xc($s0)
/* 0779C4 7F042E94 8FAD00C0 */  lw    $t5, 0xc0($sp)
/* 0779C8 7F042E98 E5A0005C */  swc1  $f0, 0x5c($t5)
/* 0779CC 7F042E9C AFA000B0 */  sw    $zero, 0xb0($sp)
.L7F042EA0:
/* 0779D0 7F042EA0 8FBF0034 */  lw    $ra, 0x34($sp)
.L7F042EA4:
/* 0779D4 7F042EA4 8FA200B0 */  lw    $v0, 0xb0($sp)
/* 0779D8 7F042EA8 8FB00030 */  lw    $s0, 0x30($sp)
/* 0779DC 7F042EAC 03E00008 */  jr    $ra
/* 0779E0 7F042EB0 27BD00C0 */   addiu $sp, $sp, 0xc0
)
#endif




/**
 * US address 7F042EB4.
*/
s32 sub_GAME_7F042EB4(struct ObjectRecord *arg0, f32 *arg1, struct coord3d *arg2, struct coord3d *arg3, s32 arg4, s32 arg5)
{
    if ((arg0->runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT) && arg0->projectile->flags & 4)
    {
        return handles_projectile_motion(arg0, arg1, arg2, arg3, arg4, arg5);
    }

    return sub_GAME_7F042A0C(arg0, arg1, arg2, arg3, arg4);
}




/**
 * Update a speed and distance travelled, factoring in acceleration,
 * deceleration and the global update multiplier.
 *
 * The new speed and distance done are written back to those pointers.
 *
 * offsets: 077A48, 7F042F18
 * (copied from Perfect Dark)
 */
void chrobjApplySpeed(f32 *openPosition, f32 maxFrac, f32 *speedPtr, f32 accel, f32 decel, f32 maxSpeed)
{
    f32 speed = *speedPtr;
    s32 i;

    for (i = 0; i < g_ClockTimer; i++)
    {
        f32 limit = speed * speed * 0.5f / decel;
        f32 distRemaining = maxFrac - *openPosition;
        if (distRemaining > 0.0f)
        {
            if (speed > 0.0f && distRemaining <= limit)
            {
                // Slow down for end
                speed -= decel;

                if (speed < decel)
                {
                    speed = decel;
                }
            }
            else if (speed < maxSpeed)
            {
                // Accelerate
                if (speed < 0.0f)
                {
                    speed += decel;
                }
                else
                {
                    speed += accel;
                }

                if (speed > maxSpeed)
                {
                    speed = maxSpeed;
                }
            }

            if (speed >= distRemaining)
            {
                *openPosition = maxFrac;
                break;
            }

            *openPosition += speed;
        }
        else
        {
            if (speed < 0.0f && -distRemaining <= limit)
            {
                speed += decel;

                if (speed > -decel)
                {
                    speed = -decel;
                }
            }
            else if (speed > -maxSpeed)
            {
                if (speed > 0.0f)
                {
                    speed -= decel;
                }
                else
                {
                    speed -= accel;
                }

                if (speed < -maxSpeed)
                {
                    speed = -maxSpeed;
                }
            }

            if (speed <= distRemaining)
            {
                *openPosition = maxFrac;
                break;
            }

            *openPosition += speed;
        }
    }
    *speedPtr = speed;
}





/**
 * Address 0x7F04310C.
*/
void chrobjCallsApplySpeed(f32 *openPosition, f32 maxFrac, f32 *speedPtr, f32 accel, f32 decel, f32 maxSpeed)
{
    if (maxFrac - *openPosition < -M_PI_F)
    {
        maxFrac += M_TAU_F;
    }
    else if (maxFrac - *openPosition >= M_PI_F)
    {
        maxFrac -= M_TAU_F;
    }

    chrobjApplySpeed(openPosition, maxFrac, speedPtr, accel, decel, maxSpeed);

    if (*openPosition < 0.0f)
    {
        *openPosition = *openPosition + M_TAU_F;
    }

    if (*openPosition >= M_TAU_F)
    {
        *openPosition = *openPosition - M_TAU_F;
    }
}





#ifdef NONMATCHING
void sub_GAME_7F0431E4(void) {

}
#else
GLOBAL_ASM(
.late_rodata
glabel D_80052A74
.word 0x40c90fdb /*6.2831855*/
glabel D_80052A78
.word 0x3d4ccccd /*0.050000001*/
glabel D_80052A7C
.word 0x40c90fdb /*6.2831855*/
glabel D_80052A80
.word 0x3d4ccccd /*0.050000001*/
glabel D_80052A84
.word 0x3cf5c28f /*0.029999999*/
glabel D_80052A88
.word 0x3e19999a /*0.15000001*/
.text
glabel sub_GAME_7F0431E4
/* 077D14 7F0431E4 27BDFEE8 */  addiu $sp, $sp, -0x118
/* 077D18 7F0431E8 AFBF001C */  sw    $ra, 0x1c($sp)
/* 077D1C 7F0431EC AFB10018 */  sw    $s1, 0x18($sp)
/* 077D20 7F0431F0 AFB00014 */  sw    $s0, 0x14($sp)
/* 077D24 7F0431F4 AFA5011C */  sw    $a1, 0x11c($sp)
/* 077D28 7F0431F8 8C8E0064 */  lw    $t6, 0x64($a0)
/* 077D2C 7F0431FC 3C01FFFE */  li    $at, 0xFFFE0000 # -nan
/* 077D30 7F043200 3421FFFF */  ori   $at, (0xFFFEFFFF & 0xFFFF) # ori $at, $at, 0xffff
/* 077D34 7F043204 01C17824 */  and   $t7, $t6, $at
/* 077D38 7F043208 31F90080 */  andi  $t9, $t7, 0x80
/* 077D3C 7F04320C 00808825 */  move  $s1, $a0
/* 077D40 7F043210 1320010A */  beqz  $t9, .L7F04363C
/* 077D44 7F043214 AC8F0064 */   sw    $t7, 0x64($a0)
/* 077D48 7F043218 90880003 */  lbu   $t0, 3($a0)
/* 077D4C 7F04321C 8C90006C */  lw    $s0, 0x6c($a0)
/* 077D50 7F043220 24010001 */  li    $at, 1
/* 077D54 7F043224 15010009 */  bne   $t0, $at, .L7F04324C
/* 077D58 7F043228 26240018 */   addiu $a0, $s1, 0x18
/* 077D5C 7F04322C 0FC0FEDC */  jal   projectileFree
/* 077D60 7F043230 02002025 */   move  $a0, $s0
/* 077D64 7F043234 8E290064 */  lw    $t1, 0x64($s1)
/* 077D68 7F043238 2401FF7F */  li    $at, -129
/* 077D6C 7F04323C AE20006C */  sw    $zero, 0x6c($s1)
/* 077D70 7F043240 01215024 */  and   $t2, $t1, $at
/* 077D74 7F043244 100000FD */  b     .L7F04363C
/* 077D78 7F043248 AE2A0064 */   sw    $t2, 0x64($s1)
.L7F04324C:
/* 077D7C 7F04324C 8E0B0000 */  lw    $t3, ($s0)
/* 077D80 7F043250 2401FFFE */  li    $at, -2
/* 077D84 7F043254 AE000088 */  sw    $zero, 0x88($s0)
/* 077D88 7F043258 01616024 */  and   $t4, $t3, $at
/* 077D8C 7F04325C 2401FFFB */  li    $at, -5
/* 077D90 7F043260 AE0C0000 */  sw    $t4, ($s0)
/* 077D94 7F043264 01817024 */  and   $t6, $t4, $at
/* 077D98 7F043268 AE0E0000 */  sw    $t6, ($s0)
/* 077D9C 7F04326C AFA40028 */  sw    $a0, 0x28($sp)
/* 077DA0 7F043270 0FC16218 */  jal   matrix_4x4_get_rotation_around_xyz
/* 077DA4 7F043274 27A5010C */   addiu $a1, $sp, 0x10c
/* 077DA8 7F043278 27A4010C */  addiu $a0, $sp, 0x10c
/* 077DAC 7F04327C 0FC161C5 */  jal   matrix_4x4_set_rotation_around_xyz
/* 077DB0 7F043280 27A500CC */   addiu $a1, $sp, 0xcc
/* 077DB4 7F043284 26050068 */  addiu $a1, $s0, 0x68
/* 077DB8 7F043288 AFA50024 */  sw    $a1, 0x24($sp)
/* 077DBC 7F04328C 0FC16CFD */  jal   quaternion_set_rotation_around_xyzf
/* 077DC0 7F043290 27A4010C */   addiu $a0, $sp, 0x10c
/* 077DC4 7F043294 27A400CC */  addiu $a0, $sp, 0xcc
/* 077DC8 7F043298 0FC1674E */  jal   matrix_4x4_set_rotation_inverse
/* 077DCC 7F04329C 27A5008C */   addiu $a1, $sp, 0x8c
/* 077DD0 7F0432A0 27A4008C */  addiu $a0, $sp, 0x8c
/* 077DD4 7F0432A4 8FA50028 */  lw    $a1, 0x28($sp)
/* 077DD8 7F0432A8 0FC16032 */  jal   matrix_4x4_multiply
/* 077DDC 7F0432AC 27A6004C */   addiu $a2, $sp, 0x4c
/* 077DE0 7F0432B0 C7A2004C */  lwc1  $f2, 0x4c($sp)
/* 077DE4 7F0432B4 C7AE0050 */  lwc1  $f14, 0x50($sp)
/* 077DE8 7F0432B8 C7A00054 */  lwc1  $f0, 0x54($sp)
/* 077DEC 7F0432BC 46021102 */  mul.s $f4, $f2, $f2
/* 077DF0 7F0432C0 00000000 */  nop
/* 077DF4 7F0432C4 460E7182 */  mul.s $f6, $f14, $f14
/* 077DF8 7F0432C8 46062200 */  add.s $f8, $f4, $f6
/* 077DFC 7F0432CC 46000282 */  mul.s $f10, $f0, $f0
/* 077E00 7F0432D0 0C007DF8 */  jal   sqrtf
/* 077E04 7F0432D4 46085300 */   add.s $f12, $f10, $f8
/* 077E08 7F0432D8 E60000C0 */  swc1  $f0, 0xc0($s0)
/* 077E0C 7F0432DC C7A4005C */  lwc1  $f4, 0x5c($sp)
/* 077E10 7F0432E0 C7AA0060 */  lwc1  $f10, 0x60($sp)
/* 077E14 7F0432E4 46042182 */  mul.s $f6, $f4, $f4
/* 077E18 7F0432E8 00000000 */  nop
/* 077E1C 7F0432EC 460A5202 */  mul.s $f8, $f10, $f10
/* 077E20 7F0432F0 C7AA0064 */  lwc1  $f10, 0x64($sp)
/* 077E24 7F0432F4 46083100 */  add.s $f4, $f6, $f8
/* 077E28 7F0432F8 460A5182 */  mul.s $f6, $f10, $f10
/* 077E2C 7F0432FC 0C007DF8 */  jal   sqrtf
/* 077E30 7F043300 46043300 */   add.s $f12, $f6, $f4
/* 077E34 7F043304 E60000C4 */  swc1  $f0, 0xc4($s0)
/* 077E38 7F043308 C7A8006C */  lwc1  $f8, 0x6c($sp)
/* 077E3C 7F04330C C7A60070 */  lwc1  $f6, 0x70($sp)
/* 077E40 7F043310 46084282 */  mul.s $f10, $f8, $f8
/* 077E44 7F043314 00000000 */  nop
/* 077E48 7F043318 46063102 */  mul.s $f4, $f6, $f6
/* 077E4C 7F04331C C7A60074 */  lwc1  $f6, 0x74($sp)
/* 077E50 7F043320 46045200 */  add.s $f8, $f10, $f4
/* 077E54 7F043324 46063282 */  mul.s $f10, $f6, $f6
/* 077E58 7F043328 0C007DF8 */  jal   sqrtf
/* 077E5C 7F04332C 46085300 */   add.s $f12, $f10, $f8
/* 077E60 7F043330 E60000C8 */  swc1  $f0, 0xc8($s0)
/* 077E64 7F043334 44808000 */  mtc1  $zero, $f16
/* 077E68 7F043338 C6220018 */  lwc1  $f2, 0x18($s1)
/* 077E6C 7F04333C C62E0020 */  lwc1  $f14, 0x20($s1)
/* 077E70 7F043340 46101032 */  c.eq.s $f2, $f16
/* 077E74 7F043344 00000000 */  nop
/* 077E78 7F043348 45000005 */  bc1f  .L7F043360
/* 077E7C 7F04334C 00000000 */   nop
/* 077E80 7F043350 46107032 */  c.eq.s $f14, $f16
/* 077E84 7F043354 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 077E88 7F043358 45030013 */  bc1tl .L7F0433A8
/* 077E8C 7F04335C 44817000 */   mtc1  $at, $f14
.L7F043360:
/* 077E90 7F043360 46021102 */  mul.s $f4, $f2, $f2
/* 077E94 7F043364 E7A20048 */  swc1  $f2, 0x48($sp)
/* 077E98 7F043368 E7AE0044 */  swc1  $f14, 0x44($sp)
/* 077E9C 7F04336C 460E7182 */  mul.s $f6, $f14, $f14
/* 077EA0 7F043370 0C007DF8 */  jal   sqrtf
/* 077EA4 7F043374 46062300 */   add.s $f12, $f4, $f6
/* 077EA8 7F043378 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 077EAC 7F04337C 44819000 */  mtc1  $at, $f18
/* 077EB0 7F043380 C7A20048 */  lwc1  $f2, 0x48($sp)
/* 077EB4 7F043384 C7AE0044 */  lwc1  $f14, 0x44($sp)
/* 077EB8 7F043388 46009303 */  div.s $f12, $f18, $f0
/* 077EBC 7F04338C 44808000 */  mtc1  $zero, $f16
/* 077EC0 7F043390 460C1082 */  mul.s $f2, $f2, $f12
/* 077EC4 7F043394 00000000 */  nop
/* 077EC8 7F043398 460C7382 */  mul.s $f14, $f14, $f12
/* 077ECC 7F04339C 10000004 */  b     .L7F0433B0
/* 077ED0 7F0433A0 00000000 */   nop
/* 077ED4 7F0433A4 44817000 */  mtc1  $at, $f14
.L7F0433A8:
/* 077ED8 7F0433A8 44819000 */  mtc1  $at, $f18
/* 077EDC 7F0433AC 46008086 */  mov.s $f2, $f16
.L7F0433B0:
/* 077EE0 7F0433B0 E7A2008C */  swc1  $f2, 0x8c($sp)
/* 077EE4 7F0433B4 E7B00090 */  swc1  $f16, 0x90($sp)
/* 077EE8 7F0433B8 E7AE0094 */  swc1  $f14, 0x94($sp)
/* 077EEC 7F0433BC E7B00098 */  swc1  $f16, 0x98($sp)
/* 077EF0 7F0433C0 C62A002C */  lwc1  $f10, 0x2c($s1)
/* 077EF4 7F0433C4 3C01BF80 */  li    $at, 0xBF800000 # -1.000000
/* 077EF8 7F0433C8 27A4008C */  addiu $a0, $sp, 0x8c
/* 077EFC 7F0433CC 460A803E */  c.le.s $f16, $f10
/* 077F00 7F0433D0 27A5010C */  addiu $a1, $sp, 0x10c
/* 077F04 7F0433D4 4502000C */  bc1fl .L7F043408
/* 077F08 7F0433D8 44812000 */   mtc1  $at, $f4
/* 077F0C 7F0433DC 46007207 */  neg.s $f8, $f14
/* 077F10 7F0433E0 E7B0009C */  swc1  $f16, 0x9c($sp)
/* 077F14 7F0433E4 E7B200A0 */  swc1  $f18, 0xa0($sp)
/* 077F18 7F0433E8 E7B000A4 */  swc1  $f16, 0xa4($sp)
/* 077F1C 7F0433EC E7B000A8 */  swc1  $f16, 0xa8($sp)
/* 077F20 7F0433F0 E7A800AC */  swc1  $f8, 0xac($sp)
/* 077F24 7F0433F4 E7B000B0 */  swc1  $f16, 0xb0($sp)
/* 077F28 7F0433F8 E7A200B4 */  swc1  $f2, 0xb4($sp)
/* 077F2C 7F0433FC 1000000B */  b     .L7F04342C
/* 077F30 7F043400 E7B000B8 */   swc1  $f16, 0xb8($sp)
/* 077F34 7F043404 44812000 */  mtc1  $at, $f4
.L7F043408:
/* 077F38 7F043408 46001187 */  neg.s $f6, $f2
/* 077F3C 7F04340C E7B0009C */  swc1  $f16, 0x9c($sp)
/* 077F40 7F043410 E7B000A4 */  swc1  $f16, 0xa4($sp)
/* 077F44 7F043414 E7B000A8 */  swc1  $f16, 0xa8($sp)
/* 077F48 7F043418 E7AE00AC */  swc1  $f14, 0xac($sp)
/* 077F4C 7F04341C E7B000B0 */  swc1  $f16, 0xb0($sp)
/* 077F50 7F043420 E7A600B4 */  swc1  $f6, 0xb4($sp)
/* 077F54 7F043424 E7B000B8 */  swc1  $f16, 0xb8($sp)
/* 077F58 7F043428 E7A400A0 */  swc1  $f4, 0xa0($sp)
.L7F04342C:
/* 077F5C 7F04342C E7B000BC */  swc1  $f16, 0xbc($sp)
/* 077F60 7F043430 E7B000C0 */  swc1  $f16, 0xc0($sp)
/* 077F64 7F043434 E7B000C4 */  swc1  $f16, 0xc4($sp)
/* 077F68 7F043438 0FC16218 */  jal   matrix_4x4_get_rotation_around_xyz
/* 077F6C 7F04343C E7B200C8 */   swc1  $f18, 0xc8($sp)
/* 077F70 7F043440 26050078 */  addiu $a1, $s0, 0x78
/* 077F74 7F043444 AFA50028 */  sw    $a1, 0x28($sp)
/* 077F78 7F043448 0FC16CFD */  jal   quaternion_set_rotation_around_xyzf
/* 077F7C 7F04344C 27A4010C */   addiu $a0, $sp, 0x10c
/* 077F80 7F043450 8FA50028 */  lw    $a1, 0x28($sp)
/* 077F84 7F043454 0FC16F84 */  jal   quaternion_ensure_shortest_path
/* 077F88 7F043458 8FA40024 */   lw    $a0, 0x24($sp)
/* 077F8C 7F04345C 44808000 */  mtc1  $zero, $f16
/* 077F90 7F043460 00000000 */  nop
/* 077F94 7F043464 E6100060 */  swc1  $f16, 0x60($s0)
/* 077F98 7F043468 C7A800CC */  lwc1  $f8, 0xcc($sp)
/* 077F9C 7F04346C C7AA008C */  lwc1  $f10, 0x8c($sp)
/* 077FA0 7F043470 C7A60090 */  lwc1  $f6, 0x90($sp)
/* 077FA4 7F043474 46085102 */  mul.s $f4, $f10, $f8
/* 077FA8 7F043478 C7AA00D0 */  lwc1  $f10, 0xd0($sp)
/* 077FAC 7F04347C 460A3202 */  mul.s $f8, $f6, $f10
/* 077FB0 7F043480 C7AA00D4 */  lwc1  $f10, 0xd4($sp)
/* 077FB4 7F043484 46082180 */  add.s $f6, $f4, $f8
/* 077FB8 7F043488 C7A40094 */  lwc1  $f4, 0x94($sp)
/* 077FBC 7F04348C 46045202 */  mul.s $f8, $f10, $f4
/* 077FC0 7F043490 0FC16B2C */  jal   acosf
/* 077FC4 7F043494 46064300 */   add.s $f12, $f8, $f6
/* 077FC8 7F043498 44808000 */  mtc1  $zero, $f16
/* 077FCC 7F04349C 46000486 */  mov.s $f18, $f0
/* 077FD0 7F0434A0 4600803C */  c.lt.s $f16, $f0
/* 077FD4 7F0434A4 00000000 */  nop
/* 077FD8 7F0434A8 45020018 */  bc1fl .L7F04350C
/* 077FDC 7F0434AC 4600803C */   c.lt.s $f16, $f0
/* 077FE0 7F0434B0 C622001C */  lwc1  $f2, 0x1c($s1)
/* 077FE4 7F0434B4 8FA2011C */  lw    $v0, 0x11c($sp)
/* 077FE8 7F0434B8 4602803C */  c.lt.s $f16, $f2
/* 077FEC 7F0434BC 00000000 */  nop
/* 077FF0 7F0434C0 45020012 */  bc1fl .L7F04350C
/* 077FF4 7F0434C4 4600803C */   c.lt.s $f16, $f0
/* 077FF8 7F0434C8 C44A0004 */  lwc1  $f10, 4($v0)
/* 077FFC 7F0434CC 3C014080 */  li    $at, 0x40800000 # 4.000000
/* 078000 7F0434D0 4602503C */  c.lt.s $f10, $f2
/* 078004 7F0434D4 00000000 */  nop
/* 078008 7F0434D8 4502000C */  bc1fl .L7F04350C
/* 07800C 7F0434DC 4600803C */   c.lt.s $f16, $f0
/* 078010 7F0434E0 44812000 */  mtc1  $at, $f4
/* 078014 7F0434E4 3C018005 */  lui   $at, %hi(D_80052A74)
/* 078018 7F0434E8 C4262A74 */  lwc1  $f6, %lo(D_80052A74)($at)
/* 07801C 7F0434EC 46040202 */  mul.s $f8, $f0, $f4
/* 078020 7F0434F0 3C018005 */  lui   $at, %hi(D_80052A78)
/* 078024 7F0434F4 C4242A78 */  lwc1  $f4, %lo(D_80052A78)($at)
/* 078028 7F0434F8 46064283 */  div.s $f10, $f8, $f6
/* 07802C 7F0434FC 460A2203 */  div.s $f8, $f4, $f10
/* 078030 7F043500 10000037 */  b     .L7F0435E0
/* 078034 7F043504 E6080064 */   swc1  $f8, 0x64($s0)
/* 078038 7F043508 4600803C */  c.lt.s $f16, $f0
.L7F04350C:
/* 07803C 7F04350C 8FA2011C */  lw    $v0, 0x11c($sp)
/* 078040 7F043510 C622001C */  lwc1  $f2, 0x1c($s1)
/* 078044 7F043514 45000013 */  bc1f  .L7F043564
/* 078048 7F043518 C44E0004 */   lwc1  $f14, 4($v0)
/* 07804C 7F04351C 4610103C */  c.lt.s $f2, $f16
/* 078050 7F043520 00000000 */  nop
/* 078054 7F043524 45020010 */  bc1fl .L7F043568
/* 078058 7F043528 C62A0020 */   lwc1  $f10, 0x20($s1)
/* 07805C 7F04352C 460E103C */  c.lt.s $f2, $f14
/* 078060 7F043530 3C014080 */  li    $at, 0x40800000 # 4.000000
/* 078064 7F043534 4502000C */  bc1fl .L7F043568
/* 078068 7F043538 C62A0020 */   lwc1  $f10, 0x20($s1)
/* 07806C 7F04353C 44813000 */  mtc1  $at, $f6
/* 078070 7F043540 3C018005 */  lui   $at, %hi(D_80052A7C)
/* 078074 7F043544 C42A2A7C */  lwc1  $f10, %lo(D_80052A7C)($at)
/* 078078 7F043548 46060102 */  mul.s $f4, $f0, $f6
/* 07807C 7F04354C 3C018005 */  lui   $at, %hi(D_80052A80)
/* 078080 7F043550 C4262A80 */  lwc1  $f6, %lo(D_80052A80)($at)
/* 078084 7F043554 460A2203 */  div.s $f8, $f4, $f10
/* 078088 7F043558 46083103 */  div.s $f4, $f6, $f8
/* 07808C 7F04355C 10000020 */  b     .L7F0435E0
/* 078090 7F043560 E6040064 */   swc1  $f4, 0x64($s0)
.L7F043564:
/* 078094 7F043564 C62A0020 */  lwc1  $f10, 0x20($s1)
.L7F043568:
/* 078098 7F043568 C4460008 */  lwc1  $f6, 8($v0)
/* 07809C 7F04356C C4440000 */  lwc1  $f4, ($v0)
/* 0780A0 7F043570 8E2F0014 */  lw    $t7, 0x14($s1)
/* 0780A4 7F043574 46065202 */  mul.s $f8, $f10, $f6
/* 0780A8 7F043578 C62A0018 */  lwc1  $f10, 0x18($s1)
/* 0780AC 7F04357C C5E00014 */  lwc1  $f0, 0x14($t7)
/* 0780B0 7F043580 E7B20030 */  swc1  $f18, 0x30($sp)
/* 0780B4 7F043584 460A2182 */  mul.s $f6, $f4, $f10
/* 0780B8 7F043588 00000000 */  nop
/* 0780BC 7F04358C 46027102 */  mul.s $f4, $f14, $f2
/* 0780C0 7F043590 46043280 */  add.s $f10, $f6, $f4
/* 0780C4 7F043594 46000102 */  mul.s $f4, $f0, $f0
/* 0780C8 7F043598 460A4180 */  add.s $f6, $f8, $f10
/* 0780CC 7F04359C 0FC16B2C */  jal   acosf
/* 0780D0 7F0435A0 46043303 */   div.s $f12, $f6, $f4
/* 0780D4 7F0435A4 44808000 */  mtc1  $zero, $f16
/* 0780D8 7F0435A8 C7B20030 */  lwc1  $f18, 0x30($sp)
/* 0780DC 7F0435AC 3C018005 */  lui    $at, %hi(g_GlobalTimerDelta)
/* 0780E0 7F0435B0 C4288378 */  lwc1  $f8, %lo(g_GlobalTimerDelta)($at)
/* 0780E4 7F0435B4 46109032 */  c.eq.s $f18, $f16
/* 0780E8 7F0435B8 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 0780EC 7F0435BC 46080083 */  div.s $f2, $f0, $f8
/* 0780F0 7F0435C0 45030005 */  bc1tl .L7F0435D8
/* 0780F4 7F0435C4 44813000 */   mtc1  $at, $f6
/* 0780F8 7F0435C8 46121283 */  div.s $f10, $f2, $f18
/* 0780FC 7F0435CC 10000004 */  b     .L7F0435E0
/* 078100 7F0435D0 E60A0064 */   swc1  $f10, 0x64($s0)
/* 078104 7F0435D4 44813000 */  mtc1  $at, $f6
.L7F0435D8:
/* 078108 7F0435D8 00000000 */  nop
/* 07810C 7F0435DC E6060064 */  swc1  $f6, 0x64($s0)
.L7F0435E0:
/* 078110 7F0435E0 C6000064 */  lwc1  $f0, 0x64($s0)
/* 078114 7F0435E4 3C018005 */  lui   $at, %hi(D_80052A84)
/* 078118 7F0435E8 4610003C */  c.lt.s $f0, $f16
/* 07811C 7F0435EC 00000000 */  nop
/* 078120 7F0435F0 45000004 */  bc1f  .L7F043604
/* 078124 7F0435F4 00000000 */   nop
/* 078128 7F0435F8 46000107 */  neg.s $f4, $f0
/* 07812C 7F0435FC E6040064 */  swc1  $f4, 0x64($s0)
/* 078130 7F043600 C6000064 */  lwc1  $f0, 0x64($s0)
.L7F043604:
/* 078134 7F043604 C4222A84 */  lwc1  $f2, %lo(D_80052A84)($at)
/* 078138 7F043608 3C018005 */  lui   $at, %hi(D_80052A88)
/* 07813C 7F04360C 4602003C */  c.lt.s $f0, $f2
/* 078140 7F043610 00000000 */  nop
/* 078144 7F043614 45000003 */  bc1f  .L7F043624
/* 078148 7F043618 00000000 */   nop
/* 07814C 7F04361C 10000007 */  b     .L7F04363C
/* 078150 7F043620 E6020064 */   swc1  $f2, 0x64($s0)
.L7F043624:
/* 078154 7F043624 C4222A88 */  lwc1  $f2, %lo(D_80052A88)($at)
/* 078158 7F043628 4600103C */  c.lt.s $f2, $f0
/* 07815C 7F04362C 00000000 */  nop
/* 078160 7F043630 45020003 */  bc1fl .L7F043640
/* 078164 7F043634 8FBF001C */   lw    $ra, 0x1c($sp)
/* 078168 7F043638 E6020064 */  swc1  $f2, 0x64($s0)
.L7F04363C:
/* 07816C 7F04363C 8FBF001C */  lw    $ra, 0x1c($sp)
.L7F043640:
/* 078170 7F043640 8FB00014 */  lw    $s0, 0x14($sp)
/* 078174 7F043644 8FB10018 */  lw    $s1, 0x18($sp)
/* 078178 7F043648 03E00008 */  jr    $ra
/* 07817C 7F04364C 27BD0118 */   addiu $sp, $sp, 0x118
)
#endif





#ifdef NONMATCHING
void sub_GAME_7F043650(void) {

}
#else

#if defined(LEFTOVERDEBUG)
GLOBAL_ASM(
.text
glabel sub_GAME_7F043650
/* 078180 7F043650 27BDFFC0 */  addiu $sp, $sp, -0x40
/* 078184 7F043654 AFBF001C */  sw    $ra, 0x1c($sp)
/* 078188 7F043658 AFB00018 */  sw    $s0, 0x18($sp)
/* 07818C 7F04365C 8C820064 */  lw    $v0, 0x64($a0)
/* 078190 7F043660 00808025 */  move  $s0, $a0
/* 078194 7F043664 304E0080 */  andi  $t6, $v0, 0x80
/* 078198 7F043668 51C0006F */  beql  $t6, $zero, .L7F043828
/* 07819C 7F04366C 8FBF001C */   lw    $ra, 0x1c($sp)
/* 0781A0 7F043670 8C83006C */  lw    $v1, 0x6c($a0)
/* 0781A4 7F043674 2401FFDF */  li    $at, -33
/* 0781A8 7F043678 8C6F0000 */  lw    $t7, ($v1)
/* 0781AC 7F04367C 31F80001 */  andi  $t8, $t7, 1
/* 0781B0 7F043680 53000052 */  beql  $t8, $zero, .L7F0437CC
/* 0781B4 7F043684 00414024 */   and   $t0, $v0, $at
/* 0781B8 7F043688 8C790090 */  lw    $t9, 0x90($v1)
/* 0781BC 7F04368C 30480020 */  andi  $t0, $v0, 0x20
/* 0781C0 7F043690 5F20004E */  bgtzl $t9, .L7F0437CC
/* 0781C4 7F043694 00414024 */   and   $t0, $v0, $at
/* 0781C8 7F043698 1100004B */  beqz  $t0, .L7F0437C8
/* 0781CC 7F04369C 3C0A8003 */   lui   $t2, %hi(Throwing_knife_SFX)
/* 0781D0 7F0436A0 254A0B2C */  addiu $t2, %lo(Throwing_knife_SFX) # addiu $t2, $t2, 0xb2c
/* 0781D4 7F0436A4 8D410000 */  lw    $at, ($t2)
/* 0781D8 7F0436A8 27A90038 */  addiu $t1, $sp, 0x38
/* 0781DC 7F0436AC AD210000 */  sw    $at, ($t1)
/* 0781E0 7F0436B0 95410004 */  lhu   $at, 4($t2)
/* 0781E4 7F0436B4 A5210004 */  sh    $at, 4($t1)
/* 0781E8 7F0436B8 8C8D006C */  lw    $t5, 0x6c($a0)
/* 0781EC 7F0436BC 8DAE00A4 */  lw    $t6, 0xa4($t5)
/* 0781F0 7F0436C0 0C002914 */  jal   randomGetNext
/* 0781F4 7F0436C4 AFAE0034 */   sw    $t6, 0x34($sp)
/* 0781F8 7F0436C8 24010003 */  li    $at, 3
/* 0781FC 7F0436CC 0041001B */  divu  $zero, $v0, $at
/* 078200 7F0436D0 00007810 */  mfhi  $t7
/* 078204 7F0436D4 AFAF0030 */  sw    $t7, 0x30($sp)
/* 078208 7F0436D8 8E03006C */  lw    $v1, 0x6c($s0)
/* 07820C 7F0436DC 3C198005 */  lui   $t9, %hi(g_GlobalTimer)
/* 078210 7F0436E0 8F39837C */  lw    $t9, %lo(g_GlobalTimer)($t9)
/* 078214 7F0436E4 8C7800A0 */  lw    $t8, 0xa0($v1)
/* 078218 7F0436E8 8FA70034 */  lw    $a3, 0x34($sp)
/* 07821C 7F0436EC 2728FFFA */  addiu $t0, $t9, -6
/* 078220 7F0436F0 0308082A */  slt   $at, $t8, $t0
/* 078224 7F0436F4 1020000D */  beqz  $at, .L7F04372C
/* 078228 7F0436F8 00075880 */   sll   $t3, $a3, 2
/* 07822C 7F0436FC 006B4821 */  addu  $t1, $v1, $t3
/* 078230 7F043700 8D240098 */  lw    $a0, 0x98($t1)
/* 078234 7F043704 5080000A */  beql  $a0, $zero, .L7F043730
/* 078238 7F043708 8FA70034 */   lw    $a3, 0x34($sp)
/* 07823C 7F04370C 0C00237C */  jal   sndGetPlayingState
/* 078240 7F043710 AFAB002C */   sw    $t3, 0x2c($sp)
/* 078244 7F043714 10400005 */  beqz  $v0, .L7F04372C
/* 078248 7F043718 8FA7002C */   lw    $a3, 0x2c($sp)
/* 07824C 7F04371C 8E0A006C */  lw    $t2, 0x6c($s0)
/* 078250 7F043720 01476021 */  addu  $t4, $t2, $a3
/* 078254 7F043724 0C002408 */  jal   sndDeactivate
/* 078258 7F043728 8D840098 */   lw    $a0, 0x98($t4)
.L7F04372C:
/* 07825C 7F04372C 8FA70034 */  lw    $a3, 0x34($sp)
.L7F043730:
/* 078260 7F043730 8E0E006C */  lw    $t6, 0x6c($s0)
/* 078264 7F043734 00076880 */  sll   $t5, $a3, 2
/* 078268 7F043738 01CD7821 */  addu  $t7, $t6, $t5
/* 07826C 7F04373C 8DF90098 */  lw    $t9, 0x98($t7)
/* 078270 7F043740 57200039 */  bnezl $t9, .L7F043828
/* 078274 7F043744 8FBF001C */   lw    $ra, 0x1c($sp)
/* 078278 7F043748 0FC2FF01 */  jal   lvlGetControlsLockedFlag
/* 07827C 7F04374C AFAD002C */   sw    $t5, 0x2c($sp)
/* 078280 7F043750 14400034 */  bnez  $v0, .L7F043824
/* 078284 7F043754 8FA7002C */   lw    $a3, 0x2c($sp)
/* 078288 7F043758 8FB80030 */  lw    $t8, 0x30($sp)
/* 07828C 7F04375C 8E0B006C */  lw    $t3, 0x6c($s0)
/* 078290 7F043760 3C048006 */  lui   $a0, %hi(g_musicSfxBufferPtr)
/* 078294 7F043764 00184040 */  sll   $t0, $t8, 1
/* 078298 7F043768 03A82821 */  addu  $a1, $sp, $t0
/* 07829C 7F04376C 84A50038 */  lh    $a1, 0x38($a1)
/* 0782A0 7F043770 01673021 */  addu  $a2, $t3, $a3
/* 0782A4 7F043774 24C60098 */  addiu $a2, $a2, 0x98
/* 0782A8 7F043778 AFA7002C */  sw    $a3, 0x2c($sp)
/* 0782AC 7F04377C 0C002382 */  jal   sndPlaySfx
/* 0782B0 7F043780 8C843720 */   lw    $a0, %lo(g_musicSfxBufferPtr)($a0)
/* 0782B4 7F043784 8FA7002C */  lw    $a3, 0x2c($sp)
/* 0782B8 7F043788 8E09006C */  lw    $t1, 0x6c($s0)
/* 0782BC 7F04378C 8E050010 */  lw    $a1, 0x10($s0)
/* 0782C0 7F043790 01275021 */  addu  $t2, $t1, $a3
/* 0782C4 7F043794 8D440098 */  lw    $a0, 0x98($t2)
/* 0782C8 7F043798 0FC14E84 */  jal   chrobjSndCreatePostEventDefault
/* 0782CC 7F04379C 24A50008 */   addiu $a1, $a1, 8
/* 0782D0 7F0437A0 3C0C8005 */  lui   $t4, %hi(g_GlobalTimer)
/* 0782D4 7F0437A4 8D8C837C */  lw    $t4, %lo(g_GlobalTimer)($t4)
/* 0782D8 7F0437A8 8E0D006C */  lw    $t5, 0x6c($s0)
/* 0782DC 7F0437AC 240F0001 */  li    $t7, 1
/* 0782E0 7F0437B0 ADAC00A0 */  sw    $t4, 0xa0($t5)
/* 0782E4 7F0437B4 8FAE0034 */  lw    $t6, 0x34($sp)
/* 0782E8 7F0437B8 8E18006C */  lw    $t8, 0x6c($s0)
/* 0782EC 7F0437BC 01EEC823 */  subu  $t9, $t7, $t6
/* 0782F0 7F0437C0 10000018 */  b     .L7F043824
/* 0782F4 7F0437C4 AF1900A4 */   sw    $t9, 0xa4($t8)
.L7F0437C8:
/* 0782F8 7F0437C8 00414024 */  and   $t0, $v0, $at
.L7F0437CC:
/* 0782FC 7F0437CC AE080064 */  sw    $t0, 0x64($s0)
/* 078300 7F0437D0 8C640098 */  lw    $a0, 0x98($v1)
/* 078304 7F0437D4 50800009 */  beql  $a0, $zero, .L7F0437FC
/* 078308 7F0437D8 8E09006C */   lw    $t1, 0x6c($s0)
/* 07830C 7F0437DC 0C00237C */  jal   sndGetPlayingState
/* 078310 7F0437E0 00000000 */   nop
/* 078314 7F0437E4 50400005 */  beql  $v0, $zero, .L7F0437FC
/* 078318 7F0437E8 8E09006C */   lw    $t1, 0x6c($s0)
/* 07831C 7F0437EC 8E0B006C */  lw    $t3, 0x6c($s0)
/* 078320 7F0437F0 0C002408 */  jal   sndDeactivate
/* 078324 7F0437F4 8D640098 */   lw    $a0, 0x98($t3)
/* 078328 7F0437F8 8E09006C */  lw    $t1, 0x6c($s0)
.L7F0437FC:
/* 07832C 7F0437FC 8D24009C */  lw    $a0, 0x9c($t1)
/* 078330 7F043800 50800009 */  beql  $a0, $zero, .L7F043828
/* 078334 7F043804 8FBF001C */   lw    $ra, 0x1c($sp)
/* 078338 7F043808 0C00237C */  jal   sndGetPlayingState
/* 07833C 7F04380C 00000000 */   nop
/* 078340 7F043810 50400005 */  beql  $v0, $zero, .L7F043828
/* 078344 7F043814 8FBF001C */   lw    $ra, 0x1c($sp)
/* 078348 7F043818 8E0A006C */  lw    $t2, 0x6c($s0)
/* 07834C 7F04381C 0C002408 */  jal   sndDeactivate
/* 078350 7F043820 8D44009C */   lw    $a0, 0x9c($t2)
.L7F043824:
/* 078354 7F043824 8FBF001C */  lw    $ra, 0x1c($sp)
.L7F043828:
/* 078358 7F043828 8FB00018 */  lw    $s0, 0x18($sp)
/* 07835C 7F04382C 27BD0040 */  addiu $sp, $sp, 0x40
/* 078360 7F043830 03E00008 */  jr    $ra
/* 078364 7F043834 00000000 */   nop
)
#endif

#if !defined(LEFTOVERDEBUG)
GLOBAL_ASM(
.text
glabel sub_GAME_7F043650
/* 076100 7F043710 27BDFFC0 */  addiu $sp, $sp, -0x40
/* 076104 7F043714 AFBF001C */  sw    $ra, 0x1c($sp)
/* 076108 7F043718 AFB00018 */  sw    $s0, 0x18($sp)
/* 07610C 7F04371C 8C820064 */  lw    $v0, 0x64($a0)
/* 076110 7F043720 00808025 */  move  $s0, $a0
/* 076114 7F043724 304E0080 */  andi  $t6, $v0, 0x80
/* 076118 7F043728 51C0006F */  beql  $t6, $zero, .L7F0438E8
/* 07611C 7F04372C 8FBF001C */   lw    $ra, 0x1c($sp)
/* 076120 7F043730 8C83006C */  lw    $v1, 0x6c($a0)
/* 076124 7F043734 2401FFDF */  li    $at, -33
/* 076128 7F043738 8C6F0000 */  lw    $t7, ($v1)
/* 07612C 7F04373C 31F80001 */  andi  $t8, $t7, 1
/* 076130 7F043740 53000052 */  beql  $t8, $zero, .L7F04388C
/* 076134 7F043744 00414024 */   and   $t0, $v0, $at
/* 076138 7F043748 8C790090 */  lw    $t9, 0x90($v1)
/* 07613C 7F04374C 30480020 */  andi  $t0, $v0, 0x20
/* 076140 7F043750 5F20004E */  bgtzl $t9, .L7F04388C
/* 076144 7F043754 00414024 */   and   $t0, $v0, $at
/* 076148 7F043758 1100004B */  beqz  $t0, .L7F043888
/* 07614C 7F04375C 3C0A8003 */   lui   $t2, %hi(Throwing_knife_SFX) # $t2, 0x8003
/* 076150 7F043760 254AC07C */  addiu $t2, %lo(Throwing_knife_SFX) # addiu $t2, $t2, -0x3f84
/* 076154 7F043764 8D410000 */  lw    $at, ($t2)
/* 076158 7F043768 27A90038 */  addiu $t1, $sp, 0x38
/* 07615C 7F04376C AD210000 */  sw    $at, ($t1)
/* 076160 7F043770 95410004 */  lhu   $at, 4($t2)
/* 076164 7F043774 A5210004 */  sh    $at, 4($t1)
/* 076168 7F043778 8C8D006C */  lw    $t5, 0x6c($a0)
/* 07616C 7F04377C 8DAE00A4 */  lw    $t6, 0xa4($t5)
/* 076170 7F043780 0C00262C */  jal   randomGetNext
/* 076174 7F043784 AFAE0034 */   sw    $t6, 0x34($sp)
/* 076178 7F043788 24010003 */  li    $at, 3
/* 07617C 7F04378C 0041001B */  divu  $zero, $v0, $at
/* 076180 7F043790 00007810 */  mfhi  $t7
/* 076184 7F043794 AFAF0030 */  sw    $t7, 0x30($sp)
/* 076188 7F043798 8E03006C */  lw    $v1, 0x6c($s0)
/* 07618C 7F04379C 3C198004 */  lui   $t9, %hi(g_GlobalTimer) # $t9, 0x8004
/* 076190 7F0437A0 8F390FFC */  lw    $t9, %lo(g_GlobalTimer)($t9)
/* 076194 7F0437A4 8C7800A0 */  lw    $t8, 0xa0($v1)
/* 076198 7F0437A8 8FA70034 */  lw    $a3, 0x34($sp)
/* 07619C 7F0437AC 2728FFFB */  addiu $t0, $t9, -5
/* 0761A0 7F0437B0 0308082A */  slt   $at, $t8, $t0
/* 0761A4 7F0437B4 1020000D */  beqz  $at, .L7F0437EC
/* 0761A8 7F0437B8 00075880 */   sll   $t3, $a3, 2
/* 0761AC 7F0437BC 006B4821 */  addu  $t1, $v1, $t3
/* 0761B0 7F0437C0 8D240098 */  lw    $a0, 0x98($t1)
/* 0761B4 7F0437C4 5080000A */  beql  $a0, $zero, .L7F0437F0
/* 0761B8 7F0437C8 8FA70034 */   lw    $a3, 0x34($sp)
/* 0761BC 7F0437CC 0C002094 */  jal   sndGetPlayingState
/* 0761C0 7F0437D0 AFAB002C */   sw    $t3, 0x2c($sp)
/* 0761C4 7F0437D4 10400005 */  beqz  $v0, .L7F0437EC
/* 0761C8 7F0437D8 8FA7002C */   lw    $a3, 0x2c($sp)
/* 0761CC 7F0437DC 8E0A006C */  lw    $t2, 0x6c($s0)
/* 0761D0 7F0437E0 01476021 */  addu  $t4, $t2, $a3
/* 0761D4 7F0437E4 0C002120 */  jal   sndDeactivate
/* 0761D8 7F0437E8 8D840098 */   lw    $a0, 0x98($t4)
.L7F0437EC:
/* 0761DC 7F0437EC 8FA70034 */  lw    $a3, 0x34($sp)
.L7F0437F0:
/* 0761E0 7F0437F0 8E0E006C */  lw    $t6, 0x6c($s0)
/* 0761E4 7F0437F4 00076880 */  sll   $t5, $a3, 2
/* 0761E8 7F0437F8 01CD7821 */  addu  $t7, $t6, $t5
/* 0761EC 7F0437FC 8DF90098 */  lw    $t9, 0x98($t7)
/* 0761F0 7F043800 57200039 */  bnezl $t9, .L7F0438E8
/* 0761F4 7F043804 8FBF001C */   lw    $ra, 0x1c($sp)
/* 0761F8 7F043808 0FC2FC1E */  jal   lvlGetControlsLockedFlag
/* 0761FC 7F04380C AFAD002C */   sw    $t5, 0x2c($sp)
/* 076200 7F043810 14400034 */  bnez  $v0, .L7F0438E4
/* 076204 7F043814 8FA7002C */   lw    $a3, 0x2c($sp)
/* 076208 7F043818 8FB80030 */  lw    $t8, 0x30($sp)
/* 07620C 7F04381C 8E0B006C */  lw    $t3, 0x6c($s0)
/* 076210 7F043820 3C048005 */  lui   $a0, %hi(g_musicSfxBufferPtr) # $a0, 0x8005
/* 076214 7F043824 00184040 */  sll   $t0, $t8, 1
/* 076218 7F043828 03A82821 */  addu  $a1, $sp, $t0
/* 07621C 7F04382C 84A50038 */  lh    $a1, 0x38($a1)
/* 076220 7F043830 01673021 */  addu  $a2, $t3, $a3
/* 076224 7F043834 24C60098 */  addiu $a2, $a2, 0x98
/* 076228 7F043838 AFA7002C */  sw    $a3, 0x2c($sp)
/* 07622C 7F04383C 0C00209A */  jal   sndPlaySfx
/* 076230 7F043840 8C846900 */   lw    $a0, %lo(g_musicSfxBufferPtr)($a0)
/* 076234 7F043844 8FA7002C */  lw    $a3, 0x2c($sp)
/* 076238 7F043848 8E09006C */  lw    $t1, 0x6c($s0)
/* 07623C 7F04384C 8E050010 */  lw    $a1, 0x10($s0)
/* 076240 7F043850 01275021 */  addu  $t2, $t1, $a3
/* 076244 7F043854 8D440098 */  lw    $a0, 0x98($t2)
/* 076248 7F043858 0FC14F3C */  jal   chrobjSndCreatePostEventDefault
/* 07624C 7F04385C 24A50008 */   addiu $a1, $a1, 8
/* 076250 7F043860 3C0C8004 */  lui   $t4, %hi(g_GlobalTimer) # $t4, 0x8004
/* 076254 7F043864 8D8C0FFC */  lw    $t4, %lo(g_GlobalTimer)($t4)
/* 076258 7F043868 8E0D006C */  lw    $t5, 0x6c($s0)
/* 07625C 7F04386C 240F0001 */  li    $t7, 1
/* 076260 7F043870 ADAC00A0 */  sw    $t4, 0xa0($t5)
/* 076264 7F043874 8FAE0034 */  lw    $t6, 0x34($sp)
/* 076268 7F043878 8E18006C */  lw    $t8, 0x6c($s0)
/* 07626C 7F04387C 01EEC823 */  subu  $t9, $t7, $t6
/* 076270 7F043880 10000018 */  b     .L7F0438E4
/* 076274 7F043884 AF1900A4 */   sw    $t9, 0xa4($t8)
.L7F043888:
/* 076278 7F043888 00414024 */  and   $t0, $v0, $at
.L7F04388C:
/* 07627C 7F04388C AE080064 */  sw    $t0, 0x64($s0)
/* 076280 7F043890 8C640098 */  lw    $a0, 0x98($v1)
/* 076284 7F043894 50800009 */  beql  $a0, $zero, .L7F0438BC
/* 076288 7F043898 8E09006C */   lw    $t1, 0x6c($s0)
/* 07628C 7F04389C 0C002094 */  jal   sndGetPlayingState
/* 076290 7F0438A0 00000000 */   nop
/* 076294 7F0438A4 50400005 */  beql  $v0, $zero, .L7F0438BC
/* 076298 7F0438A8 8E09006C */   lw    $t1, 0x6c($s0)
/* 07629C 7F0438AC 8E0B006C */  lw    $t3, 0x6c($s0)
/* 0762A0 7F0438B0 0C002120 */  jal   sndDeactivate
/* 0762A4 7F0438B4 8D640098 */   lw    $a0, 0x98($t3)
/* 0762A8 7F0438B8 8E09006C */  lw    $t1, 0x6c($s0)
.L7F0438BC:
/* 0762AC 7F0438BC 8D24009C */  lw    $a0, 0x9c($t1)
/* 0762B0 7F0438C0 50800009 */  beql  $a0, $zero, .L7F0438E8
/* 0762B4 7F0438C4 8FBF001C */   lw    $ra, 0x1c($sp)
/* 0762B8 7F0438C8 0C002094 */  jal   sndGetPlayingState
/* 0762BC 7F0438CC 00000000 */   nop
/* 0762C0 7F0438D0 50400005 */  beql  $v0, $zero, .L7F0438E8
/* 0762C4 7F0438D4 8FBF001C */   lw    $ra, 0x1c($sp)
/* 0762C8 7F0438D8 8E0A006C */  lw    $t2, 0x6c($s0)
/* 0762CC 7F0438DC 0C002120 */  jal   sndDeactivate
/* 0762D0 7F0438E0 8D44009C */   lw    $a0, 0x9c($t2)
.L7F0438E4:
/* 0762D4 7F0438E4 8FBF001C */  lw    $ra, 0x1c($sp)
.L7F0438E8:
/* 0762D8 7F0438E8 8FB00018 */  lw    $s0, 0x18($sp)
/* 0762DC 7F0438EC 27BD0040 */  addiu $sp, $sp, 0x40
/* 0762E0 7F0438F0 03E00008 */  jr    $ra
/* 0762E4 7F0438F4 00000000 */   nop
)
#endif
#endif


void sub_GAME_7F043838(coord3d *arg0, Mtxf *arg1)
{
    f32 sp124;
    f32 sp120;
    f32 sp11c;
    f32 sp118;
    f32 sp114;
    f32 f0;
    f32 sp10c;
    f32 sp108;
    f32 sp104;
    f32 a;
    f32 b;
    f32 stack;
    f32 spf4;
    f32 spf0;
    Mtxf spb0;
    Mtxf sp70;
    Mtxf sp30;
    coord3d sp24;

    f0 = sqrtf(arg0->f[0] * arg0->f[0] + arg0->f[1] * arg0->f[1] + arg0->f[2] * arg0->f[2]);

    sp10c = arg0->x / f0;
    sp108 = arg0->y / f0;
    sp104 = arg0->z / f0;

    if (sp10c == 0.0f && sp104 == 0.0f)
    {
        sp124 = 0.0f;
        sp120 = 0.0f;
        sp11c = sp108;
        sp118 = 1.0f;
        sp114 = 0.0f;
    }
    else
    {
        a = sqrtf(sp10c * sp10c + sp104 * sp104);
        b = sp10c / a;

        sp118 = sp104 / a;
        sp114 = -b;

        sp124 = sp108 * b;
        sp120 = -a;
        sp11c = sp108 * sp118;
    }

    spf4 = atan2f(sp118, sp114);

    matrix_4x4_set_rotation_around_y(-spf4, &spb0);

    sp24.x = sp124;
    sp24.y = sp120;
    sp24.z = sp11c;

    mtx4RotateVecInPlace(&spb0, sp24.f);

    spf0 = atan2f(sp24.x, sp24.y);

    matrix_4x4_set_rotation_around_y(-1.5707964f + spf4, &sp70);
    matrix_4x4_set_rotation_around_x(-1.5707964f - spf0, &sp30);

    matrix_4x4_multiply(&sp70, &sp30, arg1);
}


void sub_GAME_7F0439B8(ObjectRecord* obj, coord3d* pos, StandTile* stan, coord3d* arg3)
{
    Mtxf matrix;
    f32 temp_f0;

    sub_GAME_7F043838(arg3, &matrix);
    matrix_scalar_multiply(obj->model->scale, matrix.m[0]);
    objChangeShading(obj, pos, &matrix, stan);

    temp_f0 = chrpropBBOXGetYmin(chrobjGetBboxFromObjFile(obj->model->obj));

    obj->runtime_pos.f[0] -= temp_f0 * obj->mtx.m[1][0];
    obj->runtime_pos.f[1] -= temp_f0 * obj->mtx.m[1][1];
    obj->runtime_pos.f[2] -= temp_f0 * obj->mtx.m[1][2];

    chrobjCollisionRelated(obj);
}


bool objEmbed(PropRecord *prop, PropRecord *parent, Model *model, ModelNode *node)
{
    if (parent->flags & PROPFLAG_ONSCREEN)
    {
        ObjectRecord *obj = prop->obj;

        Mtxf mtx1;
        Mtxf mtx2;
        Mtxf mtx3;
        Mtxf* nodemtx;

        obj->embedment = embedmentAllocate();

        if (obj->embedment)
        {
            nodemtx = modelFindNodeMtx(model, node, 0);

            obj->runtime_bitflags |= RUNTIMEBITFLAG_EMBEDDED;

            chrpropDeregisterRooms(prop);
            chrpropDelist(prop);
            chrpropDisable(prop);

            obj->model->attachedto = model;
            obj->model->attachedto_objinst = node;

            chrpropReparent(prop, parent);

            matrix_4x4_copy(&obj->mtx, &mtx1);
            matrix_4x4_set_position(&obj->runtime_pos, &mtx1);
            matrix_4x4_multiply_homogeneous(currentPlayerGetMatrix10D4(), nodemtx, &mtx2);
            sub_GAME_7F059FB8((f32 (*)[4]) &mtx2.m, (f32 (*)[4]) &mtx3.m);
            matrix_4x4_multiply_homogeneous((Mtxf* ) &mtx3.m, &mtx1, &obj->embedment->matrix);

            return TRUE;
        }
    }

    return FALSE;
}


/**
 * Named same as Perfect Dark.
*/
#if defined(VERSION_JP) || defined(VERSION_EU)
s32 propExplode(PropRecord *prop, s32 /* enum EXPLOSION_DEF */ explosionType)
#else
void propExplode(PropRecord *prop, s32 /* enum EXPLOSION_DEF */ explosionType)
#endif
{
    ObjectRecord *prop_obj; // sp92
    s32 playernum; // sp88
#if defined(VERSION_JP) || defined(VERSION_EU)
    s32 ret;
#endif
    struct PropRecord *parent;
    struct StandTile *stan; // sp80
    struct coord3d pos;
    Mtxf *mtx;

    prop_obj = prop->obj;
    playernum = (prop_obj->runtime_bitflags & RUNTIMEBITFLAG_OWNER) >> RUNTIMEBITSHIFT_OWNER;

    if (prop->parent)
    {
        parent = prop->parent;

        while (parent->parent)
        {
			parent = parent->parent;
		}

        stan = parent->stan;

        if (prop->flags & PROPFLAG_ONSCREEN)
        {
            mtx = getsubmatrix(prop_obj->model);

            pos.x = mtx->m[3][0];
			pos.y = mtx->m[3][1];
			pos.z = mtx->m[3][2];

            mtx4TransformVecInPlace(currentPlayerGetMatrix10D4(), &pos);
        }
        else
        {
            pos.x = parent->pos.x;
			pos.y = parent->pos.y;
			pos.z = parent->pos.z;
        }

        if ((parent->flags & PROPFLAG_00000008) == 0
            && walkTilesBetweenPoints_NoCallback(&stan, parent->pos.f[0], parent->pos.f[2], pos.x, pos.z))
        {
#if defined(VERSION_JP) || defined(VERSION_EU)
    ret =
#endif
            explosionCreate(0, &pos, stan, (s16) explosionType, (prop_obj->flags & 0xE) == 0, playernum, parent->rooms, 0);
        }
        else
        {
#if defined(VERSION_JP) || defined(VERSION_EU)
    ret =
#endif
            explosionCreate(0, &pos, stan, (s16) explosionType, 0, playernum, parent->rooms, 1);
        }
    }
    else
    {
#if defined(VERSION_JP) || defined(VERSION_EU)
    ret =
#endif
        explosionCreate(
            0,
            &prop_obj->runtime_pos,
            prop->stan,
            (s16) explosionType,
            (prop_obj->flags & 0xE) == 0 && (prop->flags & PROPFLAG_00000008) == 0,
            playernum,
            prop->rooms,
            (prop->flags & PROPFLAG_00000008) != 0);
    }

#if defined(VERSION_JP) || defined(VERSION_EU)
    return ret;
#endif
}



/**
 * US address 7F043D70.
 * JP address 7F044074.
 * EU address 7F043E34.
 *
 * Seems to be a subset of Perfect Dark weaponTick.
*/
void chrobjWeaponTick(struct PropRecord* prop)
{
    struct ObjectRecord* obj;
    struct WeaponObjRecord *weapon;
#if defined(VERSION_US)
    u32 owner_player_number;
    u32 owner_player_as_bitflag;
#else
    s32 exp_result;
    u32 owner_player_number;
    s32 p1;
    u32 owner_player_as_bitflag;
#endif
    struct PropRecord* player_prop;
    f32 diff_x;
    f32 diff_z;
    f32 diff_y;

    obj = prop->obj;

    if (sub_GAME_7F09B4D8(get_cur_playernum()) != 0)
    {
        return;
    }

    if (obj->type == PROP_TYPE_EXPLOSION) // 7
    {
        if (obj->flags & PROPFLAG_IS_DRONE_GUN)
        {
            propExplode(prop, EXPLOSION_DEF_DRONE);
            obj->runtime_bitflags |= RUNTIMEBITFLAG_REMOVE;
        }

        return;
    }

    if (obj->type == PROP_TYPE_SMOKE) // 8
    {
        weapon = prop->weapon;

        if (((weapon->weaponnum == ITEM_GRENADE) || (weapon->weaponnum == ITEM_NULL87)) && (weapon->timer >= 0))
        {
            weapon->timer -= g_ClockTimer;

            if (weapon->timer < 0)
            {
                propExplode(prop, (obj->flags2 & PROPFLAG2_DOOR_ALTCOORDSYSTEM) ? EXPLOSION_DEF_MASSIVE : EXPLOSION_DEF_STANDARD);
                obj->runtime_bitflags |= RUNTIMEBITFLAG_REMOVE;
            }
        }
        else if (weapon->weaponnum == ITEM_NULL86)
        {
            if (weapon->timer == 0)
            {
                propExplode(prop, (obj->flags2 & PROPFLAG2_DOOR_ALTCOORDSYSTEM) ? EXPLOSION_DEF_MASSIVE : EXPLOSION_DEF_STANDARD);
                obj->runtime_bitflags |= RUNTIMEBITFLAG_REMOVE;
            }
        }
        else if (weapon->weaponnum == ITEM_PLASTIQUE)
        {
            if (weapon->timer == 0)
            {
                propExplode(prop, EXPLOSION_DEF_MASSIVE);
                obj->runtime_bitflags |= RUNTIMEBITFLAG_REMOVE;
                SurroundWithExplosions(PLASTIQUE_EXPLOSION_DELAY_TICKS);
                countdownTimerSetVisible(2, FALSE);
            }
        }
        else if (((weapon->weaponnum == ITEM_TIMEDMINE) || (weapon->weaponnum == ITEM_BOMBCASE)) && (weapon->timer >= 0))
        {
            weapon->timer -= g_ClockTimer;

            if (weapon->timer < 0)
            {
#if defined(VERSION_US)
                propExplode(prop, (obj->flags2 & PROPFLAG2_DOOR_ALTCOORDSYSTEM) ? EXPLOSION_DEF_MASSIVE : EXPLOSION_DEF_STANDARD);
#else
                exp_result = propExplode(prop, (obj->flags2 & PROPFLAG2_DOOR_ALTCOORDSYSTEM) ? EXPLOSION_DEF_MASSIVE : EXPLOSION_DEF_STANDARD);
                if (exp_result == 0)
                {
                    return;
                }
#endif
                weapon->timer = -1;
                obj->runtime_bitflags |= RUNTIMEBITFLAG_REMOVE;

            }
        }
        else if (weapon->weaponnum == ITEM_REMOTEMINE)
        {
            if (g_RemoteMineOwnerTriggerFlag)
            {
                owner_player_number = (obj->runtime_bitflags & RUNTIMEBITFLAG_OWNER) >> RUNTIMEBITSHIFT_OWNER;
                owner_player_as_bitflag = (1 << owner_player_number);
                if (g_RemoteMineOwnerTriggerFlag & owner_player_as_bitflag)
                {
                    weapon->timer = 0;
                }
            }

            if (weapon->timer > 1)
            {
                weapon->timer -= g_ClockTimer;

                if (weapon->timer < 2)
                {
                    weapon->timer = 1;
                }
            }
            else if (weapon->timer == 0)
            {
#if defined(VERSION_US)
                if (obj->flags2 & PROPFLAG2_DOOR_ALTCOORDSYSTEM)
                {
                    propExplode(prop, EXPLOSION_DEF_MASSIVE);
                }
                else if (bossGetStageNum() == LEVELID_FACILITY)
                {
                    propExplode(prop, EXPLOSION_DEF_FACILITY_REMOTE);
                }
                else
                {
                    propExplode(prop, EXPLOSION_DEF_STANDARD);
                }
#else
                if (obj->flags2 & PROPFLAG2_DOOR_ALTCOORDSYSTEM)
                {
                    p1 = EXPLOSION_DEF_MASSIVE;
                }
                else
                {
                    p1 = EXPLOSION_DEF_STANDARD;

                    if (bossGetStageNum() == LEVELID_FACILITY)
                    {
                        p1 = EXPLOSION_DEF_FACILITY_REMOTE;
                    }
                }

                exp_result = propExplode(prop, p1);
                if (exp_result == 0)
                {
                    return;
                }
#endif
                weapon->timer = -1;
                obj->runtime_bitflags |= RUNTIMEBITFLAG_REMOVE;
            }

        }
        else if (weapon->weaponnum == ITEM_PROXIMITYMINE)
        {
            if (weapon->timer > 1)
            {
                weapon->timer -= g_ClockTimer;

                if (weapon->timer < 2)
                {
                    weapon->timer = 1;
                    add_obj_to_temp_proxmine_table(weapon);
                }
            }
            else if (weapon->timer == 1)
            {
                player_prop = get_curplayer_positiondata();

                diff_x = player_prop->pos.f[0] - prop->pos.f[0];
                diff_y = player_prop->pos.f[1] - prop->pos.f[1];
                diff_z = player_prop->pos.f[2] - prop->pos.f[2];

                if ((diff_x * diff_x) + (diff_y * diff_y) + (diff_z * diff_z) < PROXIMITY_MINE_TRIGGER_DISTANCE)
                {
                    weapon->timer = 0;
                }
            }

            if (weapon->timer == 0)
            {
#if defined(VERSION_US)
                propExplode(prop, (obj->flags2 & PROPFLAG2_DOOR_ALTCOORDSYSTEM) ? EXPLOSION_DEF_MASSIVE : EXPLOSION_DEF_STANDARD);
#else
                exp_result = propExplode(prop, (obj->flags2 & PROPFLAG2_DOOR_ALTCOORDSYSTEM) ? EXPLOSION_DEF_MASSIVE : EXPLOSION_DEF_STANDARD);
                if (exp_result == 0)
                {
                    return;
                }
#endif

                weapon->timer = -1;
                obj->runtime_bitflags |= RUNTIMEBITFLAG_REMOVE;
                remove_obj_from_temp_proxmine_table(weapon);
            }
        }
    }
}



void objDropRecursively(PropRecord *prop)
{
	PropRecord *child = prop->child;

	while (child)
    {
		PropRecord *prev = child->prev;
		objDropRecursively(child);
		objDrop(child);
		child = prev;
	}
}


void sub_GAME_7F04424C(PropRecord* prop)
{
    ObjectRecord* obj;
    PropRecord* next;
    PropRecord* child;

    obj = prop->obj;
    if (obj->runtime_bitflags & RUNTIMEBITFLAG_REMOVE)
    {
        objFree(obj, 1, obj->state & PROPSTATE_RESPAWN);
        return;
    }

    prop->flags &= ~(PROPFLAG_ONSCREEN);
    chrobjWeaponTick(prop);

    child = prop->child;
    while (child != NULL)
    {
        next = child->prev;
        sub_GAME_7F04424C(child);
        child = next;
    }
}


void sub_GAME_7F0442DC(PropRecord* prop)
{
    ObjectRecord* obj;
    Model* model;
    PropRecord* child;
    PropRecord* prev;
    Mtxf* mtx;

    obj = prop->obj;
    model = obj->model;

    if (obj->runtime_bitflags & RUNTIMEBITFLAG_REMOVE)
    {
        objFree(obj, 1, (obj->state & PROPSTATE_RESPAWN));
        return;
    }

    if ((model->attachedto_objinst != NULL) && (obj->runtime_bitflags & RUNTIMEBITFLAG_EMBEDDED))
    {
        mtx = modelFindNodeMtx(model->attachedto, model->attachedto_objinst, 0);
        prop->flags |= PROPFLAG_ONSCREEN;
        model->render_pos = (RenderPosView*)dynAllocate(model->obj->numMatrices << 6);

        matrix_4x4_multiply_homogeneous(mtx, &obj->embedment->matrix, (Mtxf*)model->render_pos);
        modelUpdateRelationsQuick(model, model->obj->RootNode);
        chrobjWeaponTick(prop);

        child = prop->child;
        while (child != NULL)
        {
            prev = child->prev;
            sub_GAME_7F0442DC(child);
            child = prev;
        }
    }
    else
    {
        prop->flags &= ~(PROPFLAG_ONSCREEN);
        chrobjWeaponTick(prop);

        child = prop->child;
        while (child != NULL)
        {
            prev = child->prev;
            sub_GAME_7F04424C(child);
            child = prev;
        }
    }
}


/**
 * Address: 7F044414
 * Description: Separating Axis Theorem 
 * 
 * Return true if both blocks are not intersecting on the X/Z plane.
 * PD: cdBlockExcludesBlockLaterally
 */
bool chrobjSeparatingAxisTheorem(rect4f* rect1, s32 numvertices0, rect4f* rect2, s32 numvertices1)
{
    f64 diff2;
    f64 diff1;
    s32 j;
    s32 k;
    s32 next;
    s32 i;
    f64 sum3;
    f64 sum2;
    f64 sum1;
    coord3d tmp;

    for (i = 0; i < numvertices0; i++)
    {
        next = (i + 1) % numvertices0;
        diff1 = rect1->points[next].y - (f64)rect1->points[i].y;
        diff2 = rect1->points[i].x - (f64)rect1->points[next].x;

        if (diff1 == 0.0f && diff2 == 0.0f)
        {
            tmp.x = rect1->points[i].x;
            tmp.y = 0.0f;
            tmp.z = rect1->points[i].y;
            if (chrpropTestPointInPolygon(&tmp, rect2, numvertices1))
            {
                return FALSE;
            }
        }
        else
        {
            sum1 = rect1->points[i].x * diff1 + rect1->points[i].y * diff2;
            j = (next + 1) % numvertices0;

            while (j != i)
            {
                sum2 = rect1->points[j].x * diff1 + rect1->points[j].y * diff2;

                if (sum2 != sum1) { break; }

                j = (j + 1) % numvertices0;
            }

            for (k = 0; k < numvertices1; k++)
            {
                sum3 = rect2->points[k].x * diff1 + rect2->points[k].y * diff2;

                if (sum2 == sum1)
                {
                    sum2 = sum1 - sum3 + sum1;
                }

                if (sum3 < sum1 && sum2 < sum1) { break; }
                if (sum3 > sum1 && sum2 > sum1) { break; }
            }

            if (k == numvertices1)
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}


/**
 * Address 0x7F0446B8 (NTSC)
 * Address 0x7F0449A0 (NTSC-J)
 * 
 * Description: Does a 2D collision check between two (convex?) polygons.
 * 
 * Note: The NTSC version is 7 to 8 times faster than the others.
 *       Was this an attempt at optimization or to fix a bug?
 * 
 * Deepseek says JP/EU's new code will detect edges cases such as a polygon
 * fully contained into another. NTSC's only check is SAT, which misses when
 * the polygons have edges that don’t overlap. NTSC's code handles 95% of
 * collisions so it should be called first.
 * 
 * So they fixed a bug, but didn't do it the right way so it wouldn't affect performance.
*/
s32 chrobjTestPolygonsTouchingOrOverlap2D(struct rect4f *arg0, s32 arg1, struct rect4f *arg2, s32 arg3)
{
#if defined(VERSION_JP) || defined(VERSION_EU)
    s32 i;
    struct coord3d sp48;

    for (i=0; i<arg1; i++)
    {
        sp48.f[0] = arg0->points[i].f[0];
        sp48.f[1] = 0.0f;
        sp48.f[2] = arg0->points[i].f[1];

        if (chrpropTestPointInPolygon(&sp48, arg2, arg3) != 0)
        {
            return 1;
        }
    }

    for (i=0; i<arg3; i++)
    {
        sp48.f[0] = arg2->points[i].f[0];
        sp48.f[1] = 0.0f;
        sp48.f[2] = arg2->points[i].f[1];

        if (chrpropTestPointInPolygon(&sp48, arg0, arg1) != 0)
        {
            return 1;
        }
    }
#endif

    if (chrobjSeparatingAxisTheorem(arg0, arg1, arg2, arg3))
    {
        return 0;
    }

    if (chrobjSeparatingAxisTheorem(arg2, arg3, arg0, arg1))
    {
        return 0;
    }

    return 1;
}






/**
 * Checks whether a point collision with a convex polygon is within the specified radius.
 * @param point: 3d point to test collision with polygon. Only (x,z) are used.
 * @param collision_radius: Collision radius of point to test.
 * @param polygon: Convex polygon.
 * @param edges: Number of edges to test in polygon.
 *
 * Address 0x7F044718.
*/
s32 chrobjTestPointPolygonCollision(struct coord3d *point, f32 collision_radius, struct rect4f *polygon, s32 edges)
{
    f32 temp_f0;
    f32 temp_f26;
    f32 px;
    f32 pz;
    f32 temp_f30;
    s32 i;
    struct coord2d *temp_s0;

    px = point->f[0];
    pz = point->f[2];

    for (i=0; i<edges; i++)
    {
        temp_s0 = &polygon->points[(i+1) % edges];

        temp_f0 = sub_GAME_7F0B16C4(polygon->points[i].f[0], polygon->points[i].f[1], temp_s0->f[0], temp_s0->f[1], px, pz);

        if (temp_f0 < 0.0f)
        {
            temp_f0 = -temp_f0;
        }

        temp_f26 = distBetweenPoints2d(polygon->points[i].f[0], polygon->points[i].f[1], px, pz);
        temp_f30 = distBetweenPoints2d(temp_s0->f[0], temp_s0->f[1], px, pz);

        if ((temp_f0 < collision_radius)
            && ((temp_f26 < collision_radius)
                || (temp_f30 < collision_radius)
                || sub_GAME_7F0B17E4(polygon->points[i].f[0], polygon->points[i].f[1], temp_s0->f[0], temp_s0->f[1], px, pz)
            )
        )
        {
            return 1;
        }
    }

    return 0;
}


/**
 * NTSC address 0x7F0448A8.
*/
s32 sub_GAME_7F0448A8(struct PropRecord *arg0)
{
    s32 var_s0;
    struct rect4f *sp98;
    s32 sp94;
    f32 sp90;
    f32 sp8C;
    s32 roomids[8];
    s16 *temp_s0;
    f32 sp64;
    f32 sp60;
    f32 sp5C;
    f32 temp_f0;
    PropRecord *propss;
    ObjectRecord *temp_v0_2;
    struct rect4f *sp4C;
    s32 sp48;
    f32 sp44;
    f32 sp40;

    chraiGetCollisionBounds(arg0, &sp98, &sp94, &sp90, &sp8C);

    if (sp94 <= 0)
    {
        return 1;
    }

    chraiGetPropRoomIds(arg0, (s32*)&roomids);
    roomGetProps((s32*)&roomids);

    propss = (PropRecord *)&pos_data_entry;

    for (temp_s0 = ptr_list_object_lookup_indices; *temp_s0 >= 0; temp_s0++)
    {
        PropRecord *prop = &propss[*temp_s0];

        if (prop != arg0)
        {
            if ((prop->type == PROP_TYPE_VIEWER) || (prop->type == PROP_TYPE_CHR))
            {
                temp_v0_2 = prop->obj;
                if ((temp_v0_2 == NULL) || !((s32) temp_v0_2->model & 0x400))
                {
                    chrpropGetCollisionBounds(prop, &sp5C, &sp64, &sp60);

                    temp_f0 = sub_GAME_7F03CFE8(prop);
                    sp60 += temp_f0;
                    sp64 += temp_f0;

                    if (sp60 <= sp90)
                    {
                        var_s0 = 1;

                        if (sp8C <= sp64)
                        {
                            if (chrpropTestPointInPolygon(&prop->pos, sp98, sp94) != 0)
                            {
                                var_s0 = 0;
                            }

                            if ((var_s0 != 0) && (chrobjTestPointPolygonCollision(&prop->pos, sp5C, sp98, sp94) != 0))
                            {
                                var_s0 = 0;
                            }

                            if (var_s0 == 0)
                            {
                                if ((prop->type == PROP_TYPE_CHR) && (arg0->type == PROP_TYPE_DOOR))
                                {
                                    prop->chr->hidden |= CHRHIDDEN_OFFSCREEN_PATROL;
                                }

                                return 0;
                            }
                        }
                    }
                }
            }
            else if (
                ((prop->type == PROP_TYPE_OBJ) || (prop->type == PROP_TYPE_WEAPON) || (prop->type == PROP_TYPE_DOOR))
                && (
                    (arg0->type != PROP_TYPE_DOOR)
                    || ((prop->type != PROP_TYPE_DOOR) && ((prop->obj->type != PROPDEF_SAFE)) && (prop->obj->type != PROPDEF_AIRCRAFT))))
            {
                chraiGetCollisionBounds(prop, &sp4C, &sp48, &sp44, &sp40);

                if ((sp48 > 0)
                    && (sp40 <= sp90)
                    && (sp8C <= sp44)
                    && (chrobjTestPolygonsTouchingOrOverlap2D(sp4C, sp48, sp98, sp94) != 0))
                {
                    return 0;
                }
            }
        }
    }

    return 1;
}





#ifdef NONMATCHING
void sub_GAME_7F044B38(void) {

}
#else
GLOBAL_ASM(
.text
glabel sub_GAME_7F044B38
/* 079668 7F044B38 27BDFEA8 */  addiu $sp, $sp, -0x158
/* 07966C 7F044B3C AFBF0024 */  sw    $ra, 0x24($sp)
/* 079670 7F044B40 AFB10020 */  sw    $s1, 0x20($sp)
/* 079674 7F044B44 AFB0001C */  sw    $s0, 0x1c($sp)
/* 079678 7F044B48 8C830014 */  lw    $v1, 0x14($a0)
/* 07967C 7F044B4C 8C910010 */  lw    $s1, 0x10($a0)
/* 079680 7F044B50 24180001 */  li    $t8, 1
/* 079684 7F044B54 8C790008 */  lw    $t9, 8($v1)
/* 079688 7F044B58 00808025 */  move  $s0, $a0
/* 07968C 7F044B5C 27A500C8 */  addiu $a1, $sp, 0xc8
/* 079690 7F044B60 8F220008 */  lw    $v0, 8($t9)
/* 079694 7F044B64 8C480004 */  lw    $t0, 4($v0)
/* 079698 7F044B68 8D090004 */  lw    $t1, 4($t0)
/* 07969C 7F044B6C AFA90148 */  sw    $t1, 0x148($sp)
/* 0796A0 7F044B70 8C4A0008 */  lw    $t2, 8($v0)
/* 0796A4 7F044B74 8D4B0004 */  lw    $t3, 4($t2)
/* 0796A8 7F044B78 AFAB0144 */  sw    $t3, 0x144($sp)
/* 0796AC 7F044B7C 8C4C000C */  lw    $t4, 0xc($v0)
/* 0796B0 7F044B80 8D8D0004 */  lw    $t5, 4($t4)
/* 0796B4 7F044B84 AFAD0140 */  sw    $t5, 0x140($sp)
/* 0796B8 7F044B88 8C4E0010 */  lw    $t6, 0x10($v0)
/* 0796BC 7F044B8C 8DCF0004 */  lw    $t7, 4($t6)
/* 0796C0 7F044B90 AFB80108 */  sw    $t8, 0x108($sp)
/* 0796C4 7F044B94 AFAF013C */  sw    $t7, 0x13c($sp)
/* 0796C8 7F044B98 8C590018 */  lw    $t9, 0x18($v0)
/* 0796CC 7F044B9C 8F280004 */  lw    $t0, 4($t9)
/* 0796D0 7F044BA0 AFA800C0 */  sw    $t0, 0xc0($sp)
/* 0796D4 7F044BA4 C48C00A0 */  lwc1  $f12, 0xa0($a0)
/* 0796D8 7F044BA8 0FC1617F */  jal   matrix_4x4_set_rotation_around_y
/* 0796DC 7F044BAC AFA30154 */   sw    $v1, 0x154($sp)
/* 0796E0 7F044BB0 8FA90154 */  lw    $t1, 0x154($sp)
/* 0796E4 7F044BB4 27A500C8 */  addiu $a1, $sp, 0xc8
/* 0796E8 7F044BB8 0FC1629F */  jal   matrix_scalar_multiply
/* 0796EC 7F044BBC C52C0014 */   lwc1  $f12, 0x14($t1)
/* 0796F0 7F044BC0 26040058 */  addiu $a0, $s0, 0x58
/* 0796F4 7F044BC4 0FC16266 */  jal   matrix_4x4_set_position
/* 0796F8 7F044BC8 27A500C8 */   addiu $a1, $sp, 0xc8
/* 0796FC 7F044BCC 27A400C8 */  addiu $a0, $sp, 0xc8
/* 079700 7F044BD0 8FA50148 */  lw    $a1, 0x148($sp)
/* 079704 7F044BD4 0FC16106 */  jal   matrix_4x4_transform_vector
/* 079708 7F044BD8 27A60130 */   addiu $a2, $sp, 0x130
/* 07970C 7F044BDC 27A400C8 */  addiu $a0, $sp, 0xc8
/* 079710 7F044BE0 8FA50144 */  lw    $a1, 0x144($sp)
/* 079714 7F044BE4 0FC16106 */  jal   matrix_4x4_transform_vector
/* 079718 7F044BE8 27A60124 */   addiu $a2, $sp, 0x124
/* 07971C 7F044BEC 27A400C8 */  addiu $a0, $sp, 0xc8
/* 079720 7F044BF0 8FA50140 */  lw    $a1, 0x140($sp)
/* 079724 7F044BF4 0FC16106 */  jal   matrix_4x4_transform_vector
/* 079728 7F044BF8 27A60118 */   addiu $a2, $sp, 0x118
/* 07972C 7F044BFC 27A400C8 */  addiu $a0, $sp, 0xc8
/* 079730 7F044C00 8FA5013C */  lw    $a1, 0x13c($sp)
/* 079734 7F044C04 0FC16106 */  jal   matrix_4x4_transform_vector
/* 079738 7F044C08 27A6010C */   addiu $a2, $sp, 0x10c
/* 07973C 7F044C0C 8E2A0014 */  lw    $t2, 0x14($s1)
/* 079740 7F044C10 C7A80138 */  lwc1  $f8, 0x138($sp)
/* 079744 7F044C14 27A400C4 */  addiu $a0, $sp, 0xc4
/* 079748 7F044C18 AFAA00C4 */  sw    $t2, 0xc4($sp)
/* 07974C 7F044C1C 8E260010 */  lw    $a2, 0x10($s1)
/* 079750 7F044C20 8E250008 */  lw    $a1, 8($s1)
/* 079754 7F044C24 8FA70130 */  lw    $a3, 0x130($sp)
/* 079758 7F044C28 0FC2C2F9 */  jal   walkTilesBetweenPoints_NoCallback
/* 07975C 7F044C2C E7A80010 */   swc1  $f8, 0x10($sp)
/* 079760 7F044C30 10400006 */  beqz  $v0, .L7F044C4C
/* 079764 7F044C34 8FA400C4 */   lw    $a0, 0xc4($sp)
/* 079768 7F044C38 8FA50130 */  lw    $a1, 0x130($sp)
/* 07976C 7F044C3C 0FC2CA5C */  jal   stanGetPositionYValue
/* 079770 7F044C40 8FA60138 */   lw    $a2, 0x138($sp)
/* 079774 7F044C44 10000002 */  b     .L7F044C50
/* 079778 7F044C48 E7A00134 */   swc1  $f0, 0x134($sp)
.L7F044C4C:
/* 07977C 7F044C4C AFA00108 */  sw    $zero, 0x108($sp)
.L7F044C50:
/* 079780 7F044C50 8E2B0014 */  lw    $t3, 0x14($s1)
/* 079784 7F044C54 C7A6012C */  lwc1  $f6, 0x12c($sp)
/* 079788 7F044C58 27A400C4 */  addiu $a0, $sp, 0xc4
/* 07978C 7F044C5C AFAB00C4 */  sw    $t3, 0xc4($sp)
/* 079790 7F044C60 8E260010 */  lw    $a2, 0x10($s1)
/* 079794 7F044C64 8E250008 */  lw    $a1, 8($s1)
/* 079798 7F044C68 8FA70124 */  lw    $a3, 0x124($sp)
/* 07979C 7F044C6C 0FC2C2F9 */  jal   walkTilesBetweenPoints_NoCallback
/* 0797A0 7F044C70 E7A60010 */   swc1  $f6, 0x10($sp)
/* 0797A4 7F044C74 10400006 */  beqz  $v0, .L7F044C90
/* 0797A8 7F044C78 8FA400C4 */   lw    $a0, 0xc4($sp)
/* 0797AC 7F044C7C 8FA50124 */  lw    $a1, 0x124($sp)
/* 0797B0 7F044C80 0FC2CA5C */  jal   stanGetPositionYValue
/* 0797B4 7F044C84 8FA6012C */   lw    $a2, 0x12c($sp)
/* 0797B8 7F044C88 10000002 */  b     .L7F044C94
/* 0797BC 7F044C8C E7A00128 */   swc1  $f0, 0x128($sp)
.L7F044C90:
/* 0797C0 7F044C90 AFA00108 */  sw    $zero, 0x108($sp)
.L7F044C94:
/* 0797C4 7F044C94 8E2C0014 */  lw    $t4, 0x14($s1)
/* 0797C8 7F044C98 C7A40120 */  lwc1  $f4, 0x120($sp)
/* 0797CC 7F044C9C 27A400C4 */  addiu $a0, $sp, 0xc4
/* 0797D0 7F044CA0 AFAC00C4 */  sw    $t4, 0xc4($sp)
/* 0797D4 7F044CA4 8E260010 */  lw    $a2, 0x10($s1)
/* 0797D8 7F044CA8 8E250008 */  lw    $a1, 8($s1)
/* 0797DC 7F044CAC 8FA70118 */  lw    $a3, 0x118($sp)
/* 0797E0 7F044CB0 0FC2C2F9 */  jal   walkTilesBetweenPoints_NoCallback
/* 0797E4 7F044CB4 E7A40010 */   swc1  $f4, 0x10($sp)
/* 0797E8 7F044CB8 10400006 */  beqz  $v0, .L7F044CD4
/* 0797EC 7F044CBC 8FA400C4 */   lw    $a0, 0xc4($sp)
/* 0797F0 7F044CC0 8FA50118 */  lw    $a1, 0x118($sp)
/* 0797F4 7F044CC4 0FC2CA5C */  jal   stanGetPositionYValue
/* 0797F8 7F044CC8 8FA60120 */   lw    $a2, 0x120($sp)
/* 0797FC 7F044CCC 10000002 */  b     .L7F044CD8
/* 079800 7F044CD0 E7A0011C */   swc1  $f0, 0x11c($sp)
.L7F044CD4:
/* 079804 7F044CD4 AFA00108 */  sw    $zero, 0x108($sp)
.L7F044CD8:
/* 079808 7F044CD8 8E2D0014 */  lw    $t5, 0x14($s1)
/* 07980C 7F044CDC C7AA0114 */  lwc1  $f10, 0x114($sp)
/* 079810 7F044CE0 27A400C4 */  addiu $a0, $sp, 0xc4
/* 079814 7F044CE4 AFAD00C4 */  sw    $t5, 0xc4($sp)
/* 079818 7F044CE8 8E260010 */  lw    $a2, 0x10($s1)
/* 07981C 7F044CEC 8E250008 */  lw    $a1, 8($s1)
/* 079820 7F044CF0 8FA7010C */  lw    $a3, 0x10c($sp)
/* 079824 7F044CF4 0FC2C2F9 */  jal   walkTilesBetweenPoints_NoCallback
/* 079828 7F044CF8 E7AA0010 */   swc1  $f10, 0x10($sp)
/* 07982C 7F044CFC 10400006 */  beqz  $v0, .L7F044D18
/* 079830 7F044D00 8FA400C4 */   lw    $a0, 0xc4($sp)
/* 079834 7F044D04 8FA5010C */  lw    $a1, 0x10c($sp)
/* 079838 7F044D08 0FC2CA5C */  jal   stanGetPositionYValue
/* 07983C 7F044D0C 8FA60114 */   lw    $a2, 0x114($sp)
/* 079840 7F044D10 10000002 */  b     .L7F044D1C
/* 079844 7F044D14 E7A00110 */   swc1  $f0, 0x110($sp)
.L7F044D18:
/* 079848 7F044D18 AFA00108 */  sw    $zero, 0x108($sp)
.L7F044D1C:
/* 07984C 7F044D1C 8FAE0108 */  lw    $t6, 0x108($sp)
/* 079850 7F044D20 26040018 */  addiu $a0, $s0, 0x18
/* 079854 7F044D24 27A600BC */  addiu $a2, $sp, 0xbc
/* 079858 7F044D28 11C0010E */  beqz  $t6, .L7F045164
/* 07985C 7F044D2C 00802825 */   move  $a1, $a0
/* 079860 7F044D30 C7B20124 */  lwc1  $f18, 0x124($sp)
/* 079864 7F044D34 C7B00130 */  lwc1  $f16, 0x130($sp)
/* 079868 7F044D38 C7AE0128 */  lwc1  $f14, 0x128($sp)
/* 07986C 7F044D3C C7AC0134 */  lwc1  $f12, 0x134($sp)
/* 079870 7F044D40 46109401 */  sub.s $f16, $f18, $f16
/* 079874 7F044D44 C7B2012C */  lwc1  $f18, 0x12c($sp)
/* 079878 7F044D48 C7AA0138 */  lwc1  $f10, 0x138($sp)
/* 07987C 7F044D4C 460C7301 */  sub.s $f12, $f14, $f12
/* 079880 7F044D50 E7B000B4 */  swc1  $f16, 0xb4($sp)
/* 079884 7F044D54 27A400B4 */  addiu $a0, $sp, 0xb4
/* 079888 7F044D58 460A9281 */  sub.s $f10, $f18, $f10
/* 07988C 7F044D5C E7AC00B8 */  swc1  $f12, 0xb8($sp)
/* 079890 7F044D60 27A500B8 */  addiu $a1, $sp, 0xb8
/* 079894 7F044D64 0C007DD4 */  jal   guNormalize
/* 079898 7F044D68 E7AA00BC */   swc1  $f10, 0xbc($sp)
/* 07989C 7F044D6C C7B2010C */  lwc1  $f18, 0x10c($sp)
/* 0798A0 7F044D70 C7B00124 */  lwc1  $f16, 0x124($sp)
/* 0798A4 7F044D74 C7AE0110 */  lwc1  $f14, 0x110($sp)
/* 0798A8 7F044D78 C7AC0128 */  lwc1  $f12, 0x128($sp)
/* 0798AC 7F044D7C 46109401 */  sub.s $f16, $f18, $f16
/* 0798B0 7F044D80 C7B20114 */  lwc1  $f18, 0x114($sp)
/* 0798B4 7F044D84 C7AA012C */  lwc1  $f10, 0x12c($sp)
/* 0798B8 7F044D88 460C7301 */  sub.s $f12, $f14, $f12
/* 0798BC 7F044D8C E7B000A8 */  swc1  $f16, 0xa8($sp)
/* 0798C0 7F044D90 27A400A8 */  addiu $a0, $sp, 0xa8
/* 0798C4 7F044D94 460A9281 */  sub.s $f10, $f18, $f10
/* 0798C8 7F044D98 E7AC00AC */  swc1  $f12, 0xac($sp)
/* 0798CC 7F044D9C 27A500AC */  addiu $a1, $sp, 0xac
/* 0798D0 7F044DA0 27A600B0 */  addiu $a2, $sp, 0xb0
/* 0798D4 7F044DA4 0C007DD4 */  jal   guNormalize
/* 0798D8 7F044DA8 E7AA00B0 */   swc1  $f10, 0xb0($sp)
/* 0798DC 7F044DAC C7B20118 */  lwc1  $f18, 0x118($sp)
/* 0798E0 7F044DB0 C7B0010C */  lwc1  $f16, 0x10c($sp)
/* 0798E4 7F044DB4 C7AE011C */  lwc1  $f14, 0x11c($sp)
/* 0798E8 7F044DB8 C7AC0110 */  lwc1  $f12, 0x110($sp)
/* 0798EC 7F044DBC 46109401 */  sub.s $f16, $f18, $f16
/* 0798F0 7F044DC0 C7B20120 */  lwc1  $f18, 0x120($sp)
/* 0798F4 7F044DC4 C7AA0114 */  lwc1  $f10, 0x114($sp)
/* 0798F8 7F044DC8 460C7301 */  sub.s $f12, $f14, $f12
/* 0798FC 7F044DCC E7B0009C */  swc1  $f16, 0x9c($sp)
/* 079900 7F044DD0 27A4009C */  addiu $a0, $sp, 0x9c
/* 079904 7F044DD4 460A9281 */  sub.s $f10, $f18, $f10
/* 079908 7F044DD8 E7AC00A0 */  swc1  $f12, 0xa0($sp)
/* 07990C 7F044DDC 27A500A0 */  addiu $a1, $sp, 0xa0
/* 079910 7F044DE0 27A600A4 */  addiu $a2, $sp, 0xa4
/* 079914 7F044DE4 0C007DD4 */  jal   guNormalize
/* 079918 7F044DE8 E7AA00A4 */   swc1  $f10, 0xa4($sp)
/* 07991C 7F044DEC C7B20130 */  lwc1  $f18, 0x130($sp)
/* 079920 7F044DF0 C7B00118 */  lwc1  $f16, 0x118($sp)
/* 079924 7F044DF4 C7AE0134 */  lwc1  $f14, 0x134($sp)
/* 079928 7F044DF8 C7AC011C */  lwc1  $f12, 0x11c($sp)
/* 07992C 7F044DFC 46109401 */  sub.s $f16, $f18, $f16
/* 079930 7F044E00 C7B20138 */  lwc1  $f18, 0x138($sp)
/* 079934 7F044E04 C7AA0120 */  lwc1  $f10, 0x120($sp)
/* 079938 7F044E08 460C7301 */  sub.s $f12, $f14, $f12
/* 07993C 7F044E0C E7B00090 */  swc1  $f16, 0x90($sp)
/* 079940 7F044E10 27A40090 */  addiu $a0, $sp, 0x90
/* 079944 7F044E14 460A9281 */  sub.s $f10, $f18, $f10
/* 079948 7F044E18 E7AC0094 */  swc1  $f12, 0x94($sp)
/* 07994C 7F044E1C 27A50094 */  addiu $a1, $sp, 0x94
/* 079950 7F044E20 27A60098 */  addiu $a2, $sp, 0x98
/* 079954 7F044E24 0C007DD4 */  jal   guNormalize
/* 079958 7F044E28 E7AA0098 */   swc1  $f10, 0x98($sp)
/* 07995C 7F044E2C C7A800BC */  lwc1  $f8, 0xbc($sp)
/* 079960 7F044E30 C7A600A8 */  lwc1  $f6, 0xa8($sp)
/* 079964 7F044E34 C7AA00B0 */  lwc1  $f10, 0xb0($sp)
/* 079968 7F044E38 E7A80028 */  swc1  $f8, 0x28($sp)
/* 07996C 7F044E3C 46064102 */  mul.s $f4, $f8, $f6
/* 079970 7F044E40 C7A800B4 */  lwc1  $f8, 0xb4($sp)
/* 079974 7F044E44 E7A6002C */  swc1  $f6, 0x2c($sp)
/* 079978 7F044E48 E7AA0034 */  swc1  $f10, 0x34($sp)
/* 07997C 7F044E4C 46085182 */  mul.s $f6, $f10, $f8
/* 079980 7F044E50 E7A80030 */  swc1  $f8, 0x30($sp)
/* 079984 7F044E54 C7A800A4 */  lwc1  $f8, 0xa4($sp)
/* 079988 7F044E58 46062001 */  sub.s $f0, $f4, $f6
/* 07998C 7F044E5C C7A4009C */  lwc1  $f4, 0x9c($sp)
/* 079990 7F044E60 46045182 */  mul.s $f6, $f10, $f4
/* 079994 7F044E64 C7AA002C */  lwc1  $f10, 0x2c($sp)
/* 079998 7F044E68 E7A4002C */  swc1  $f4, 0x2c($sp)
/* 07999C 7F044E6C 460A4102 */  mul.s $f4, $f8, $f10
/* 0799A0 7F044E70 46043381 */  sub.s $f14, $f6, $f4
/* 0799A4 7F044E74 C7A60090 */  lwc1  $f6, 0x90($sp)
/* 0799A8 7F044E78 4600703E */  c.le.s $f14, $f0
/* 0799AC 7F044E7C 00000000 */  nop
/* 0799B0 7F044E80 45020023 */  bc1fl .L7F044F10
/* 0799B4 7F044E84 C7A600A4 */   lwc1  $f6, 0xa4($sp)
/* 0799B8 7F044E88 46064102 */  mul.s $f4, $f8, $f6
/* 0799BC 7F044E8C E7AA0038 */  swc1  $f10, 0x38($sp)
/* 0799C0 7F044E90 C7AA002C */  lwc1  $f10, 0x2c($sp)
/* 0799C4 7F044E94 C7A80098 */  lwc1  $f8, 0x98($sp)
/* 0799C8 7F044E98 460A4282 */  mul.s $f10, $f8, $f10
/* 0799CC 7F044E9C 460A2101 */  sub.s $f4, $f4, $f10
/* 0799D0 7F044EA0 C7AA0030 */  lwc1  $f10, 0x30($sp)
/* 0799D4 7F044EA4 4600203E */  c.le.s $f4, $f0
/* 0799D8 7F044EA8 00000000 */  nop
/* 0799DC 7F044EAC 45020018 */  bc1fl .L7F044F10
/* 0799E0 7F044EB0 C7A600A4 */   lwc1  $f6, 0xa4($sp)
/* 0799E4 7F044EB4 460A4102 */  mul.s $f4, $f8, $f10
/* 0799E8 7F044EB8 C7A80028 */  lwc1  $f8, 0x28($sp)
/* 0799EC 7F044EBC C7AC00B8 */  lwc1  $f12, 0xb8($sp)
/* 0799F0 7F044EC0 46064182 */  mul.s $f6, $f8, $f6
/* 0799F4 7F044EC4 46062101 */  sub.s $f4, $f4, $f6
/* 0799F8 7F044EC8 C7A60034 */  lwc1  $f6, 0x34($sp)
/* 0799FC 7F044ECC 4600203E */  c.le.s $f4, $f0
/* 079A00 7F044ED0 00000000 */  nop
/* 079A04 7F044ED4 4502000E */  bc1fl .L7F044F10
/* 079A08 7F044ED8 C7A600A4 */   lwc1  $f6, 0xa4($sp)
/* 079A0C 7F044EDC 46066102 */  mul.s $f4, $f12, $f6
/* 079A10 7F044EE0 C7A200AC */  lwc1  $f2, 0xac($sp)
/* 079A14 7F044EE4 E7A00078 */  swc1  $f0, 0x78($sp)
/* 079A18 7F044EE8 46081182 */  mul.s $f6, $f2, $f8
/* 079A1C 7F044EEC 46062201 */  sub.s $f8, $f4, $f6
/* 079A20 7F044EF0 46025102 */  mul.s $f4, $f10, $f2
/* 079A24 7F044EF4 C7A60038 */  lwc1  $f6, 0x38($sp)
/* 079A28 7F044EF8 E7A80074 */  swc1  $f8, 0x74($sp)
/* 079A2C 7F044EFC 460C3202 */  mul.s $f8, $f6, $f12
/* 079A30 7F044F00 46082281 */  sub.s $f10, $f4, $f8
/* 079A34 7F044F04 10000045 */  b     .L7F04501C
/* 079A38 7F044F08 E7AA007C */   swc1  $f10, 0x7c($sp)
/* 079A3C 7F044F0C C7A600A4 */  lwc1  $f6, 0xa4($sp)
.L7F044F10:
/* 079A40 7F044F10 C7A40090 */  lwc1  $f4, 0x90($sp)
/* 079A44 7F044F14 C7AA0098 */  lwc1  $f10, 0x98($sp)
/* 079A48 7F044F18 E7A60038 */  swc1  $f6, 0x38($sp)
/* 079A4C 7F044F1C 46043202 */  mul.s $f8, $f6, $f4
/* 079A50 7F044F20 C7A6009C */  lwc1  $f6, 0x9c($sp)
/* 079A54 7F044F24 E7A40034 */  swc1  $f4, 0x34($sp)
/* 079A58 7F044F28 46065102 */  mul.s $f4, $f10, $f6
/* 079A5C 7F044F2C 46044001 */  sub.s $f0, $f8, $f4
/* 079A60 7F044F30 C7A800B4 */  lwc1  $f8, 0xb4($sp)
/* 079A64 7F044F34 46085102 */  mul.s $f4, $f10, $f8
/* 079A68 7F044F38 C7A80034 */  lwc1  $f8, 0x34($sp)
/* 079A6C 7F044F3C C7AA00BC */  lwc1  $f10, 0xbc($sp)
/* 079A70 7F044F40 460E003E */  c.le.s $f0, $f14
/* 079A74 7F044F44 46085282 */  mul.s $f10, $f10, $f8
/* 079A78 7F044F48 45000013 */  bc1f  .L7F044F98
/* 079A7C 7F044F4C 460A2081 */   sub.s $f2, $f4, $f10
/* 079A80 7F044F50 460E103E */  c.le.s $f2, $f14
/* 079A84 7F044F54 C7A80038 */  lwc1  $f8, 0x38($sp)
/* 079A88 7F044F58 45020010 */  bc1fl .L7F044F9C
/* 079A8C 7F044F5C 4600103E */   c.le.s $f2, $f0
/* 079A90 7F044F60 C7A200AC */  lwc1  $f2, 0xac($sp)
/* 079A94 7F044F64 C7AC00A0 */  lwc1  $f12, 0xa0($sp)
/* 079A98 7F044F68 C7AA00B0 */  lwc1  $f10, 0xb0($sp)
/* 079A9C 7F044F6C 46081102 */  mul.s $f4, $f2, $f8
/* 079AA0 7F044F70 E7AE0078 */  swc1  $f14, 0x78($sp)
/* 079AA4 7F044F74 460A6202 */  mul.s $f8, $f12, $f10
/* 079AA8 7F044F78 46082281 */  sub.s $f10, $f4, $f8
/* 079AAC 7F044F7C C7A400A8 */  lwc1  $f4, 0xa8($sp)
/* 079AB0 7F044F80 460C2202 */  mul.s $f8, $f4, $f12
/* 079AB4 7F044F84 E7AA0074 */  swc1  $f10, 0x74($sp)
/* 079AB8 7F044F88 46023282 */  mul.s $f10, $f6, $f2
/* 079ABC 7F044F8C 460A4101 */  sub.s $f4, $f8, $f10
/* 079AC0 7F044F90 10000022 */  b     .L7F04501C
/* 079AC4 7F044F94 E7A4007C */   swc1  $f4, 0x7c($sp)
.L7F044F98:
/* 079AC8 7F044F98 4600103E */  c.le.s $f2, $f0
.L7F044F9C:
/* 079ACC 7F044F9C C7AC00A0 */  lwc1  $f12, 0xa0($sp)
/* 079AD0 7F044FA0 C7AE0094 */  lwc1  $f14, 0x94($sp)
/* 079AD4 7F044FA4 C7A400BC */  lwc1  $f4, 0xbc($sp)
/* 079AD8 7F044FA8 45000010 */  bc1f  .L7F044FEC
/* 079ADC 7F044FAC C7A60098 */   lwc1  $f6, 0x98($sp)
/* 079AE0 7F044FB0 C7A60098 */  lwc1  $f6, 0x98($sp)
/* 079AE4 7F044FB4 C7AE0094 */  lwc1  $f14, 0x94($sp)
/* 079AE8 7F044FB8 C7AA00A4 */  lwc1  $f10, 0xa4($sp)
/* 079AEC 7F044FBC 46066202 */  mul.s $f8, $f12, $f6
/* 079AF0 7F044FC0 E7A00078 */  swc1  $f0, 0x78($sp)
/* 079AF4 7F044FC4 460A7102 */  mul.s $f4, $f14, $f10
/* 079AF8 7F044FC8 C7AA009C */  lwc1  $f10, 0x9c($sp)
/* 079AFC 7F044FCC 46044181 */  sub.s $f6, $f8, $f4
/* 079B00 7F044FD0 460E5202 */  mul.s $f8, $f10, $f14
/* 079B04 7F044FD4 C7A40090 */  lwc1  $f4, 0x90($sp)
/* 079B08 7F044FD8 E7A60074 */  swc1  $f6, 0x74($sp)
/* 079B0C 7F044FDC 460C2182 */  mul.s $f6, $f4, $f12
/* 079B10 7F044FE0 46064281 */  sub.s $f10, $f8, $f6
/* 079B14 7F044FE4 1000000D */  b     .L7F04501C
/* 079B18 7F044FE8 E7AA007C */   swc1  $f10, 0x7c($sp)
.L7F044FEC:
/* 079B1C 7F044FEC 46047202 */  mul.s $f8, $f14, $f4
/* 079B20 7F044FF0 C7AC00B8 */  lwc1  $f12, 0xb8($sp)
/* 079B24 7F044FF4 E7A20078 */  swc1  $f2, 0x78($sp)
/* 079B28 7F044FF8 46066282 */  mul.s $f10, $f12, $f6
/* 079B2C 7F044FFC C7A60090 */  lwc1  $f6, 0x90($sp)
/* 079B30 7F045000 460A4101 */  sub.s $f4, $f8, $f10
/* 079B34 7F045004 460C3202 */  mul.s $f8, $f6, $f12
/* 079B38 7F045008 C7AA00B4 */  lwc1  $f10, 0xb4($sp)
/* 079B3C 7F04500C E7A40074 */  swc1  $f4, 0x74($sp)
/* 079B40 7F045010 460E5102 */  mul.s $f4, $f10, $f14
/* 079B44 7F045014 46044181 */  sub.s $f6, $f8, $f4
/* 079B48 7F045018 E7A6007C */  swc1  $f6, 0x7c($sp)
.L7F04501C:
/* 079B4C 7F04501C 0FC15FAB */  jal   sinf
/* 079B50 7F045020 C60C00A0 */   lwc1  $f12, 0xa0($s0)
/* 079B54 7F045024 44805000 */  mtc1  $zero, $f10
/* 079B58 7F045028 E7A00068 */  swc1  $f0, 0x68($sp)
/* 079B5C 7F04502C E7AA006C */  swc1  $f10, 0x6c($sp)
/* 079B60 7F045030 0FC15FA8 */  jal   cosf
/* 079B64 7F045034 C60C00A0 */   lwc1  $f12, 0xa0($s0)
/* 079B68 7F045038 26040018 */  addiu $a0, $s0, 0x18
/* 079B6C 7F04503C E7A00070 */  swc1  $f0, 0x70($sp)
/* 079B70 7F045040 0FC15FF4 */  jal   matrix_4x4_set_identity
/* 079B74 7F045044 AFA40058 */   sw    $a0, 0x58($sp)
/* 079B78 7F045048 C7A80074 */  lwc1  $f8, 0x74($sp)
/* 079B7C 7F04504C 8FA50058 */  lw    $a1, 0x58($sp)
/* 079B80 7F045050 E6080028 */  swc1  $f8, 0x28($s0)
/* 079B84 7F045054 C7A40078 */  lwc1  $f4, 0x78($sp)
/* 079B88 7F045058 C6100028 */  lwc1  $f16, 0x28($s0)
/* 079B8C 7F04505C E604002C */  swc1  $f4, 0x2c($s0)
/* 079B90 7F045060 C7A6007C */  lwc1  $f6, 0x7c($sp)
/* 079B94 7F045064 C60E002C */  lwc1  $f14, 0x2c($s0)
/* 079B98 7F045068 E6060030 */  swc1  $f6, 0x30($s0)
/* 079B9C 7F04506C C7AA0070 */  lwc1  $f10, 0x70($sp)
/* 079BA0 7F045070 C7A4006C */  lwc1  $f4, 0x6c($sp)
/* 079BA4 7F045074 C6000030 */  lwc1  $f0, 0x30($s0)
/* 079BA8 7F045078 460A7202 */  mul.s $f8, $f14, $f10
/* 079BAC 7F04507C 00000000 */  nop
/* 079BB0 7F045080 46002182 */  mul.s $f6, $f4, $f0
/* 079BB4 7F045084 46064281 */  sub.s $f10, $f8, $f6
/* 079BB8 7F045088 E60A0018 */  swc1  $f10, 0x18($s0)
/* 079BBC 7F04508C C7A40068 */  lwc1  $f4, 0x68($sp)
/* 079BC0 7F045090 C7A60070 */  lwc1  $f6, 0x70($sp)
/* 079BC4 7F045094 46040202 */  mul.s $f8, $f0, $f4
/* 079BC8 7F045098 00000000 */  nop
/* 079BCC 7F04509C 46103282 */  mul.s $f10, $f6, $f16
/* 079BD0 7F0450A0 460A4101 */  sub.s $f4, $f8, $f10
/* 079BD4 7F0450A4 E604001C */  swc1  $f4, 0x1c($s0)
/* 079BD8 7F0450A8 C7A6006C */  lwc1  $f6, 0x6c($sp)
/* 079BDC 7F0450AC C7AA0068 */  lwc1  $f10, 0x68($sp)
/* 079BE0 7F0450B0 46068202 */  mul.s $f8, $f16, $f6
/* 079BE4 7F0450B4 00000000 */  nop
/* 079BE8 7F0450B8 460E5102 */  mul.s $f4, $f10, $f14
/* 079BEC 7F0450BC C60A001C */  lwc1  $f10, 0x1c($s0)
/* 079BF0 7F0450C0 46044181 */  sub.s $f6, $f8, $f4
/* 079BF4 7F0450C4 E6060020 */  swc1  $f6, 0x20($s0)
/* 079BF8 7F0450C8 E7AA0048 */  swc1  $f10, 0x48($sp)
/* 079BFC 7F0450CC C7A80048 */  lwc1  $f8, 0x48($sp)
/* 079C00 7F0450D0 C6020020 */  lwc1  $f2, 0x20($s0)
/* 079C04 7F0450D4 C6120018 */  lwc1  $f18, 0x18($s0)
/* 079C08 7F0450D8 46004102 */  mul.s $f4, $f8, $f0
/* 079C0C 7F0450DC 00000000 */  nop
/* 079C10 7F0450E0 46027182 */  mul.s $f6, $f14, $f2
/* 079C14 7F0450E4 46062281 */  sub.s $f10, $f4, $f6
/* 079C18 7F0450E8 46101202 */  mul.s $f8, $f2, $f16
/* 079C1C 7F0450EC 00000000 */  nop
/* 079C20 7F0450F0 46120102 */  mul.s $f4, $f0, $f18
/* 079C24 7F0450F4 E60A0038 */  swc1  $f10, 0x38($s0)
/* 079C28 7F0450F8 460E9282 */  mul.s $f10, $f18, $f14
/* 079C2C 7F0450FC 46044181 */  sub.s $f6, $f8, $f4
/* 079C30 7F045100 E606003C */  swc1  $f6, 0x3c($s0)
/* 079C34 7F045104 C7A80048 */  lwc1  $f8, 0x48($sp)
/* 079C38 7F045108 46088102 */  mul.s $f4, $f16, $f8
/* 079C3C 7F04510C 46045181 */  sub.s $f6, $f10, $f4
/* 079C40 7F045110 E6060040 */  swc1  $f6, 0x40($s0)
/* 079C44 7F045114 8FAF0154 */  lw    $t7, 0x154($sp)
/* 079C48 7F045118 C5EC0014 */  lwc1  $f12, 0x14($t7)
/* 079C4C 7F04511C 0FC1629F */  jal   matrix_scalar_multiply
/* 079C50 7F045120 AFA50058 */   sw    $a1, 0x58($sp)
/* 079C54 7F045124 8FA40058 */  lw    $a0, 0x58($sp)
/* 079C58 7F045128 8FA50148 */  lw    $a1, 0x148($sp)
/* 079C5C 7F04512C 0FC16106 */  jal   matrix_4x4_transform_vector
/* 079C60 7F045130 27A60124 */   addiu $a2, $sp, 0x124
/* 079C64 7F045134 0FC0FA19 */  jal   chrpropBBOXGetYmin
/* 079C68 7F045138 8FA400C0 */   lw    $a0, 0xc0($sp)
/* 079C6C 7F04513C 8FB80154 */  lw    $t8, 0x154($sp)
/* 079C70 7F045140 C7A80134 */  lwc1  $f8, 0x134($sp)
/* 079C74 7F045144 C70A0014 */  lwc1  $f10, 0x14($t8)
/* 079C78 7F045148 460A0102 */  mul.s $f4, $f0, $f10
/* 079C7C 7F04514C C7AA0128 */  lwc1  $f10, 0x128($sp)
/* 079C80 7F045150 46044181 */  sub.s $f6, $f8, $f4
/* 079C84 7F045154 460A3081 */  sub.s $f2, $f6, $f10
/* 079C88 7F045158 E622000C */  swc1  $f2, 0xc($s1)
/* 079C8C 7F04515C 10000019 */  b     .L7F0451C4
/* 079C90 7F045160 E602005C */   swc1  $f2, 0x5c($s0)
.L7F045164:
/* 079C94 7F045164 C60C00A0 */  lwc1  $f12, 0xa0($s0)
/* 079C98 7F045168 0FC1617F */  jal   matrix_4x4_set_rotation_around_y
/* 079C9C 7F04516C AFA40058 */   sw    $a0, 0x58($sp)
/* 079CA0 7F045170 8FB90154 */  lw    $t9, 0x154($sp)
/* 079CA4 7F045174 8FA50058 */  lw    $a1, 0x58($sp)
/* 079CA8 7F045178 0FC1629F */  jal   matrix_scalar_multiply
/* 079CAC 7F04517C C72C0014 */   lwc1  $f12, 0x14($t9)
/* 079CB0 7F045180 8E240014 */  lw    $a0, 0x14($s1)
/* 079CB4 7F045184 8E250008 */  lw    $a1, 8($s1)
/* 079CB8 7F045188 0FC2CA5C */  jal   stanGetPositionYValue
/* 079CBC 7F04518C 8E260010 */   lw    $a2, 0x10($s1)
/* 079CC0 7F045190 E7A0005C */  swc1  $f0, 0x5c($sp)
/* 079CC4 7F045194 0FC0FA19 */  jal   chrpropBBOXGetYmin
/* 079CC8 7F045198 8FA400C0 */   lw    $a0, 0xc0($sp)
/* 079CCC 7F04519C 8FA80148 */  lw    $t0, 0x148($sp)
/* 079CD0 7F0451A0 8FA90154 */  lw    $t1, 0x154($sp)
/* 079CD4 7F0451A4 C5080004 */  lwc1  $f8, 4($t0)
/* 079CD8 7F0451A8 C5260014 */  lwc1  $f6, 0x14($t1)
/* 079CDC 7F0451AC 46004100 */  add.s $f4, $f8, $f0
/* 079CE0 7F0451B0 C7A8005C */  lwc1  $f8, 0x5c($sp)
/* 079CE4 7F0451B4 46062282 */  mul.s $f10, $f4, $f6
/* 079CE8 7F0451B8 460A4081 */  sub.s $f2, $f8, $f10
/* 079CEC 7F0451BC E622000C */  swc1  $f2, 0xc($s1)
/* 079CF0 7F0451C0 E602005C */  swc1  $f2, 0x5c($s0)
.L7F0451C4:
/* 079CF4 7F0451C4 8FBF0024 */  lw    $ra, 0x24($sp)
/* 079CF8 7F0451C8 8FA20108 */  lw    $v0, 0x108($sp)
/* 079CFC 7F0451CC 8FB0001C */  lw    $s0, 0x1c($sp)
/* 079D00 7F0451D0 8FB10020 */  lw    $s1, 0x20($sp)
/* 079D04 7F0451D4 03E00008 */  jr    $ra
/* 079D08 7F0451D8 27BD0158 */   addiu $sp, $sp, 0x158
)
#endif


s32 glassCalculateOpacity(coord3d *pos, f32 xludist, f32 opadist, f32 arg3)
{
    coord3d *campos = &get_curplayer_positiondata()->pos;
    s32 opacity;
    f32 xdiff = pos->x - campos->x;
    f32 ydiff = pos->y - campos->y;
    f32 zdiff = pos->z - campos->z;

    f32 distance = sqrtf(xdiff * xdiff + ydiff * ydiff + zdiff * zdiff);

    if (distance > opadist)
    {
        opacity = 255;
    } else if (distance < xludist)
    {
        opacity = arg3 * 255;
    }
    else
    {
        opacity = (((distance - xludist) * (1.0f - arg3)) / (opadist - xludist) + arg3) * 255;
    }

    return opacity;
}


#define PROJECTILE_GRAVITY_MODIFIER 0.27777779f

#if defined(VERSION_EU)
#define AUTOGUN_P1_ACCEL 0.0000139626345f
#define AUTOGUN_P1_MAX_SPEED 0.0008377581f
#define AUTOGUN_P2_ACCEL 0.0000069813173f
#define AUTOGUN_P2_MAX_SPEED 0.00041887906f
#define AUTOGUN_TRACK_ACCEL 0.0010471976f
#define VEHICLE_TURN_ACCEL 0.0001308997f
#define VEHICLE_TURN_DECEL 0.0002617994f
#define VEHICLE_TURN_MAX_SPEED 0.007853982f
#else
#define AUTOGUN_P1_ACCEL 0.000011635529f
#define AUTOGUN_P1_MAX_SPEED 0.00069813174f
#define AUTOGUN_P2_ACCEL 0.0000058177643f
#define AUTOGUN_P2_MAX_SPEED 0.00034906587f
#define AUTOGUN_TRACK_ACCEL 0.0008726647f
#define VEHICLE_TURN_ACCEL 0.000109083085f
#define VEHICLE_TURN_DECEL 0.00021816617f
#define VEHICLE_TURN_MAX_SPEED 0.006544985f
#endif

#if defined(VERSION_JP)
#define AUTOGUN_DAMAGE_TIMER_DELTA g_JP_GlobalTimerDelta
#else
#define AUTOGUN_DAMAGE_TIMER_DELTA g_GlobalTimerDelta
#endif

/**
 * Updates an object's per-frame movement, collision, animation, sound, and damage state.
 */
s32 object_interaction(struct PropRecord *arg0) {
  Mtxf *mtxs;
  f32 temp_f14_3;
  struct coord3d sp694;
  s32 sp690;
  f32 temp_f20;
  f32 temp_f14_4;
  struct PropRecord *sp684;
  s32 sp680;
  f32 sp67C;
  s32 sp678;
  s32 sp674;
  s32 sp670;
  struct ALSoundState *sfx_state;
  s32 var_s2_3;
  s32 sp664;
  struct coord3d sp658;
  struct coord3d sp64C;
  struct WeaponObjRecord *sp648;
  f32 temp_f12_5;
  struct ModelRoData_BoundingBoxRecord *sp640;
  f32 sp638;
  struct coord3d sp62C;
  struct coord3d sp620;
  struct coord3d sp614;
  s32 sp610;
  s32 sp60C;
  s32 var_v1_3;
  s32 sp604;
  f32 temp_f0_13;
  s32 var_s2_4;
  f32 var_f2;
  f32 temp_f0_14;
  f32 var_f12;
  struct coord3d sp5CC;
  Mtxf sp58C;
  Mtxf *temp_s0_10;
  f32 var_f2_3;
  struct PropRecord *temp_v0_19;
  f32 var_f12_3;
  s32 var_v0_3;
  f32 var_f0_2;
  f32 var_f12_5;
  struct coord3d sp564;
  f32 temp_f0_31;
  f32 sp550[4];
  struct ModelRoData_BoundingBoxRecord *sp54C;
  s32 sp548;
  struct coord3d sp53C;
  struct coord3d sp530;
  s32 temp_v0_32;
  Mtxf *temp_a1_6;
  f32 var_f12_7;
  struct coord3d *temp_s0_13;
  struct coord3d *temp_s0_14;
  f32 sp518;
  struct PropRecord *sp514;
  f32 xdiff;
  f32 ydiff;
  struct ModelNode **temp_v1_7;
  f32 var_f0_3;
  f32 *temp_v0_40;
  ModelNode **var_v1_7;
  Mtxf *temp_s0_21;
  ModelNode **temp_v1_8;
  struct StandTile *sp4F0;
  f32 var_f12_9;
  s32 var_a0_6;
  ModelNode **temp_v1_10;
  Mtxf *temp_s2_7;
  struct StandTile *sp490;
  f32 sp4D8;
  f32 sp4D4;
  struct coord3d *temp_v1_11;
  f32 temp_f20_4;
  f32 temp_f0_35;
  struct coord3d sp4BC;
  f32 sp4B8;
  f32 sp4B4;
  f32 sp4B0;
  s32 sp4AC;
  s32 sp4A8;
  s32 sp4A4;
  f32 sp4A0;
  f32 sp49C;
  f32 sp498;
  f32 sp494;
  f32 zdiff;
  f32 var_f12_6;
  f32 var_f2_7;
  f32 temp_f2_23;
  struct ObjectRecord *obj;
  f32 sp47C;
  struct coord3d *sp478;
  struct Model *model;
  f32 temp_f12;
  s32 var_s2_2;
  struct StandTile *sp468;
  s32 var_a0;
  f32 sp460;
  struct StandTile *temp_s2;
  struct coord3d sp450;
  struct StandTile *sp44C;
  f32 temp_f14_2;
  struct coord3d sp43C;
  f32 sp438;
  f32 sp434;
  f32 sp430;
  struct coord3d vec424;
  struct coord3d vec418;
  struct coord3d vec40C;
  struct coord3d vec400;
  struct ModelFileHeader *temp_v0_31;
  Mtxf *sp70;
  Mtxf *sp6C;
  Mtxf sp3A4;
  Mtxf sp320;
  struct coord3d sp360;
  struct DoorRecord *sp39C;
  f32 sp394;
  Mtxf *sp390;
  s32 sp38C;
  f32 sp384;
  f32 sp380;
  struct CCTVRecord *sp370;
  struct AutogunRecord *sp318;
  f32 sp304;
  f32 sp300;
  Mtxf *sp2FC;
  s32 sp2F8;
  struct coord3d sp308;
  Mtxf sp2AC;
  Mtxf sp26C;
  Mtxf sp200;
  struct coord3d *sp1FC;
  f32 var_f2_6;
  struct coord3d *sp260;
  struct coord3d *sp25C;
  struct coord3d *sp258;
  struct coord3d *sp254;
  f32 sp250;
  f32 sp24C;
  f32 sp248;
  s32 var_v1_5;
  s32 var_s0;
  s32 var_s0_2;
  struct PadRecord *var_v1_4;
  struct ModelFileHeader *temp_v0_29;
  s32 var_s2_6;
  Mtxf *eyelid_mtx;
  ModelRenderData sp1B0;
  Mtxf spB8;
  Mtxf spE8;
  struct coord3d *sp164;
  struct coord3d *sp160;
  struct coord3d *sp15C;
  struct ModelRoData_BoundingBoxRecord *sp158;
  f32 sp154;
  struct Model *temp_s0_6;
  f32 *temp_v0_25;
  struct coord3d *sp168;
  f32 *temp_s0_5;
  s32 var_s2_5;
  s32 sp13C;
  s32 sp138;
  struct coord3d sp12C;
  struct coord3d sp120;
  s32 sp11C;
  struct coord3d sp110;
  struct StandTile *sp10C;
  struct StandTile *sp108;
  s32 sp104;
  struct PropRecord *sp100;
  struct beam *beam;
  obj = arg0->obj;
  model = obj->model;
  sp690 = 0;
  sp680 = 0;
  sp67C = 0.0f;
  sp674 = getPlayerCount();
  sp670 = 1;
  if (obj->runtime_bitflags & 0x00000004)
  {
    objFree(obj, 0, obj->state & 0x00000004);
    return 1;
  }
  else
    if (obj->runtime_bitflags & 0x00000008)
  {
    obj->runtime_bitflags &= ~0x00000008;
  }
  else
    if (obj->runtime_bitflags & 0x00000080)
  {
    arg0->flags &= 0xFFFD;
    obj->runtime_bitflags |= 0x00000008;
    return 3;
  }
  if (sp674 == 1)
  {
    sp678 = 1;
  }
  else
  {
    sp678 = sub_GAME_7F09B4D8(get_cur_playernum()) == 0;
    if (obj->runtime_bitflags & 0x00000080)
    {
      if (obj->projectile->ownerprop != ((void *) 0))
      {
        sp678 = g_CurrentPlayer->prop == obj->projectile->ownerprop;
      }
    }
  }
  if (sp678)
  {
    if (obj->runtime_bitflags & 0x00000080)
    {
      struct Projectile *temp_s0 = obj->projectile;
      temp_s0->unkE0 += g_ClockTimer;
      if (((s32) temp_s0->unkE0) > 2400)
      {
        obj->runtime_bitflags |= 0x00000004;
      }
      if (temp_s0->flags & 0x100)
      {
        s32 var_s2 = 1;
        if (obj->type == 8)
        {
          sp648 = (struct WeaponObjRecord *) obj;
          if (sp648->weaponnum == 86)
          {
            var_s2 = 0;
          }
        }
        if (temp_s0->ownerprop != ((void *) 0))
        {
          sub_GAME_7F03D058(temp_s0->ownerprop, 0);
        }
        sp664 = sub_GAME_7F042EB4(obj, &temp_s0->unkd4, &sp64C, &sp658, 0, var_s2);
        if (temp_s0->ownerprop != ((void *) 0))
        {
          sub_GAME_7F03D058(temp_s0->ownerprop, 1);
        }
        if (((sp664 != 1) && (sp648 != ((void *) 0))) && (sp648->weaponnum == 86))
        {
          sp648->timer = 0;
        }
        temp_s0->flags &= ~0x100;
      }
      sp694.f[0] = obj->runtime_pos.f[0];
      sp694.f[1] = obj->runtime_pos.f[1];
      sp694.f[2] = obj->runtime_pos.f[2];
      if (temp_s0->refreshrate > 0)
      {
        temp_s0->refreshrate -= g_ClockTimer;
      }
      if (obj->projectile->flags & 1)
      {
        struct WeaponObjRecord *weapon_obj = (struct WeaponObjRecord *) obj;
        sp640 = chrobjGetBboxFromObjectRecord(obj);
        sp610 = 0;
        sp60C = 0;
        sp604 = 0;
        sp638 = 1.0f;
        temp_f20 = obj->runtime_pos.f[1];
        var_s2_2 = 0;
        temp_s0->unkA8 += g_ClockTimer;
        sp62C.f[0] = obj->mtx.m[0][0];
        sp62C.f[1] = obj->mtx.m[0][1];
        sp62C.f[2] = obj->mtx.m[0][2];
        if (temp_s0->flags & 0x20)
        {
          sp518 = PROJECTILE_GRAVITY_MODIFIER;
          if (temp_s0->unk1C < sp518)
          {
            temp_s0->unkB4 += temp_s0->unk10.f[1] * g_GlobalTimerDelta;
            temp_s0->unkB0 += temp_s0->unkB4 * g_GlobalTimerDelta;
            temp_s0->unk1C += (1.0f / 90.0f) * g_GlobalTimerDelta;
            if (temp_s0->unk1C > sp518)
            {
              temp_s0->unk1C = sp518;
            }
          }
          else
            if (sp694.f[1] < temp_s0->unkB0)
          {
            temp_s0->unkB4 += temp_s0->unk10.f[1] * g_GlobalTimerDelta;
            temp_s0->unkB0 += temp_s0->unkB4 * g_GlobalTimerDelta;
            sp694.f[1] += (0.07f * (temp_s0->unkB0 - sp694.f[1])) * g_GlobalTimerDelta;
          }
          else
          {
            sp694.f[1] = temp_s0->unkB0;
            temp_s0->flags &= ~0x20;
            temp_s0->unk1C = 0.0f;
            temp_s0->flags |= 0x10;
            temp_s0->speed.f[1] = temp_s0->unkB4;
          }
        }
        temp_f14_3 = PROJECTILE_GRAVITY_MODIFIER;
        if (!(temp_s0->flags & 0x10))
        {
          temp_s0->speed.f[1] += (temp_s0->unk10.f[1] + temp_s0->unk1C) * g_GlobalTimerDelta;
          temp_f12 = temp_s0->speed.f[1];
          temp_f14_4 = temp_f12 - (temp_f14_3 * g_GlobalTimerDelta);
          sp694.f[1] += (g_GlobalTimerDelta * (temp_f12 + temp_f14_4)) * 0.5f;
          temp_s0->speed.f[1] = temp_f14_4;
        }
        else
        {
          temp_s0->speed.f[1] += (temp_s0->unk10.f[1] + temp_s0->unk1C) * g_GlobalTimerDelta;
          sp694.f[1] += temp_s0->speed.f[1] * g_GlobalTimerDelta;
        }
        sp70 = &obj->mtx;
        sp6C = &temp_s0->mtx;
        temp_s0->speed.f[0] += temp_s0->unk10.f[0] * g_GlobalTimerDelta;
        temp_s0->speed.f[2] += temp_s0->unk10.f[2] * g_GlobalTimerDelta;
        sp694.f[0] += temp_s0->speed.f[0] * g_GlobalTimerDelta;
        sp694.f[2] += temp_s0->speed.f[2] * g_GlobalTimerDelta;
        sub_GAME_7F057DF8(sp70, sp6C, g_ClockTimer);
        if ((obj->type == 8) && (((((((weapon_obj->weaponnum == 0x1D) || (weapon_obj->weaponnum == 0x1B)) || (weapon_obj->weaponnum == 0x1C)) || (weapon_obj->weaponnum == 0x21)) || (weapon_obj->weaponnum == 0x2F)) || (weapon_obj->weaponnum == 0x30)) || (weapon_obj->weaponnum == 0x22)))
        {
          var_s2_2 = 1;
        }
        if (temp_s0->ownerprop != ((void *) 0))
        {
          sub_GAME_7F03D058(temp_s0->ownerprop, 0);
        }
        sp664 = sub_GAME_7F042EB4(obj, &sp694.f[0], &sp614, &sp620, var_s2_2, 0);
        if (temp_s0->ownerprop != ((void *) 0))
        {
          sub_GAME_7F03D058(temp_s0->ownerprop, 1);
        }
        sp690 = 1;
        if ((sp664 == 2) && ((D_80030B0C == ((void *) 0)) || ((D_80030B0C->type != 3) && (D_80030B0C->type != 6))))
        {
          sp548 = 0;
          if ((D_80030B0C != ((void *) 0)) && (D_80030B0C->obj->runtime_bitflags & 0x80))
          {
            sp548 = 1;
          }
          if (sp548 == 0)
          {
            projectileFree(temp_s0);
            obj->projectile = (void *) 0;
            obj->runtime_bitflags &= ~0x80;
            if (arg0->flags & 8)
            {
              arg0->flags |= 0x10;
            }
            chrobjSndCreatePostEventDefault(sndPlaySfx((struct ALBankAlt_s *) g_musicSfxBufferPtr, 241, (void *) 0), &arg0->pos);
            objectivestatusCheckDeposit(((struct WeaponObjRecord *) obj)->weaponnum, arg0->stan->room);
            sub_GAME_7F0439B8(obj, &sp614, arg0->stan, &sp620);
            if (D_80030B0C != ((void *) 0))
            {
              temp_s2 = arg0->stan;
              if (objEmbed(arg0, D_80030B0C, g_CurrentProjectileModel, dword_CODE_bss_80075B74) != 0)
              {
                arg0->stan = temp_s2;
                sp680 = 5;
                sp604 = 1;
              }
            }
          }
        }
        if (sp604 == 0)
        {
          temp_v0_19 = D_80030B0C;
          if ((temp_v0_19 != ((void *) 0)) && (obj->type == 8))
          {
            struct WeaponObjRecord *temp_weap = (struct WeaponObjRecord *) obj;
            if (temp_weap->weaponnum == 3)
            {
              if ((temp_v0_19->type == 3) || (((temp_v0_19->type == 6) && (temp_v0_19->obj != ((void *) 0))) && (getPlayerPointerIndex(temp_v0_19) != get_cur_playernum())))
              {
                struct Projectile *temp_v0_8;
                temp_v0_19 = D_80030B0C;
                temp_v0_8 = obj->projectile;
                temp_s0_13 = (struct coord3d *) temp_v0_19->chr;
                if ((((temp_v0_8->flags & 1) && (((s32) temp_v0_8->unk90) <= 0)) && (obj->runtime_bitflags & 0x20)) && (handles_shot_actors((struct ChrRecord *) temp_s0_13, bodypartshot, &flt_CODE_bss_80075B78, ((struct WeaponObjRecord *) obj)->weaponnum, 1) != 0))
                {
                  sp604 = 1;
                  if (temp_s0->unk8C > 0.0f)
                  {
                    temp_f14_3 = ((temp_s0->speed.f[0] * sp620.f[0]) + (temp_s0->speed.f[1] * sp620.f[1])) + (temp_s0->speed.f[2] * sp620.f[2]);
                    temp_f14_3 *= -(temp_s0->unk8C + 1.0f);
                    temp_s0->speed.f[0] += temp_f14_3 * sp620.f[0];
                    temp_s0->speed.f[1] += temp_f14_3 * sp620.f[1];
                    temp_s0->speed.f[2] += temp_f14_3 * sp620.f[2];
                  }
                  if (!(temp_s0->flags & 0x200))
                  {
                    mtxLoadRandomRotation(sp6C);
                  }
                  temp_s0->unk90 += 1;
                  recall_joy2_hits_edit_detail_edit_flag(((struct WeaponObjRecord *) obj)->weaponnum, D_80030B0C, -1);
                  if (((D_80030B0C->flags & 2) && (bodypartshot != 0x64)) && (bodypartshot != 0x6E))
                  {
                    temp_v0_19 = (struct PropRecord *) modelFindNodeMtx(g_CurrentProjectileModel, dword_CODE_bss_80075B74, 0);
                    sp5CC.f[0] = sp614.f[0];
                    sp5CC.f[1] = sp614.f[1];
                    sp5CC.f[2] = sp614.f[2];
                    mtx4TransformVecInPlace(camGetWorldToScreenMtxf(), &sp5CC);
                    sp5CC.f[0] += (sp5CC.f[0] - ((Mtxf *) temp_v0_19)->m[3][0]) * 0.5f;
                    sp5CC.f[1] += (sp5CC.f[1] - ((Mtxf *) temp_v0_19)->m[3][1]) * 0.5f;
                    sp5CC.f[2] += (sp5CC.f[2] - ((Mtxf *) temp_v0_19)->m[3][2]) * 0.5f;
                    sp5CC.f[0] -= (getjointsize(g_CurrentProjectileModel, dword_CODE_bss_80075B74) * 0.5f) * flt_CODE_bss_80075B88.f[0];
                    sp5CC.f[1] -= (getjointsize(g_CurrentProjectileModel, dword_CODE_bss_80075B74) * 0.5f) * flt_CODE_bss_80075B78.f[5];
                    sp5CC.f[2] -= (getjointsize(g_CurrentProjectileModel, dword_CODE_bss_80075B74) * 0.5f) * flt_CODE_bss_80075B88.f[2];
                    matrix_4x4_7F059E64((Mtxf *) temp_v0_19, &sp58C);
                    mtx4TransformVecInPlace(&sp58C, &sp5CC);
                    sub_GAME_7F0221DC(g_CurrentProjectileModel, bodypartshot, dword_CODE_bss_80075B74, &sp5CC);
                  }
                }
              }
            }
            else
              if (temp_weap->weaponnum == 86)
            {
              var_v0_3 = temp_v0_19->type;
              sp604 = 1;
              if (var_v0_3 == 3)
              {
                chrlvExplosionDamage((ChrRecord *) temp_v0_19->chr, &obj->runtime_pos, 2.0f, 1);
              }
              else
                if ((var_v0_3 == 1) || (var_v0_3 == 4))
              {
                maybe_detonate_object(temp_v0_19->obj, 100.0f, &obj->runtime_pos, 86, (s32) (((u32) (obj->runtime_bitflags & 0x00060000)) >> 0x11));
              }
              ((struct WeaponObjRecord *) obj)->timer = 0;
            }
          }
        }
        if (sp604 == 0)
        {
          if (sp664 == 0)
          {
            if (temp_s0->unk8C > 0.0f)
            {
              temp_f12_5 = temp_s0->speed.f[1];
              temp_f14_3 = ((temp_s0->speed.f[0] * sp620.f[0]) + (temp_f12_5 * sp620.f[1])) + (temp_s0->speed.f[2] * sp620.f[2]);
              temp_f14_3 *= -(temp_s0->unk8C + 1.0f);
              temp_s0->speed.f[0] += temp_f14_3 * sp620.f[0];
              temp_s0->speed.f[1] = temp_f12_5 + (temp_f14_3 * sp620.f[1]);
              temp_s0->speed.f[2] += temp_f14_3 * sp620.f[2];
              if ((temp_f12_5 <= 0.0f) && ((temp_s0->speed.f[1] >= 0.0f) || (temp_f20 <= obj->runtime_pos.f[1])))
              {
                sp60C = 1;
              }
            }
          }
          if (!(temp_s0->flags & 8))
          {
            temp_f20 = stanGetPositionYValue(arg0->stan, arg0->pos.f[0], arg0->pos.f[2]);
            sp638 = chrpropSumMatrixPosY(sp640, &sp70[0]);
            sp610 = arg0->pos.f[1] < (temp_f20 - sp638);
          }
          if ((sp610 != 0) || (sp664 == 0))
          {
            if (!(temp_s0->flags & 0x200))
            {
              mtxLoadRandomRotation(sp6C);
            }
            temp_s0->unk90 += 1;
          }
          if ((sp610 != 0) || (sp60C != 0))
          {
            if (sp610 != 0)
            {
              obj->runtime_pos.f[1] = (arg0->pos.f[1] = (temp_f20 - sp638) + 4.0f);
            }
            else
            {
              var_f2 = (sp614.f[1] - sp638) + 4.0f;
              obj->runtime_pos.f[1] = (arg0->pos.f[1] = var_f2);
            }
            if (!(obj->runtime_bitflags & 0x10000))
            {
              obj->runtime_bitflags |= 0x100;
            }
            if (temp_s0->unk8C > 0.0f)
            {
              temp_s0->speed.f[1] *= -temp_s0->unk8C;
              if (temp_s0->speed.f[1] < 2.2222223f)
              {
                if ((temp_s0->flags & 2) && (temp_s0->unk90 == 1))
                {
                  temp_s0->speed.f[1] = 2.2222223f;
                }
                else
                {
                  sub_GAME_7F0431E4(obj, &sp62C);
                }
              }
            }
            else
            {
              sub_GAME_7F0431E4(obj, &sp62C);
            }
          }
          if (obj->type == 8)
          {
            if (weapon_obj->weaponnum == 3)
            {
              sub_GAME_7F043650(weapon_obj);
            }
            else
              if (weapon_obj->weaponnum == 0x56)
            {
              if ((sp664 == 0) || (sp610 != 0))
              {
                weapon_obj->timer = 0;
              }
              else
              {
                temp_f14_4 = (temp_s0->speed.f[2] * temp_s0->speed.f[2]) + ((temp_s0->speed.f[1] * temp_s0->speed.f[1]) + (temp_s0->speed.f[0] * temp_s0->speed.f[0]));
                if (temp_f14_4 > 27777.773f)
                {
                  temp_s0->unk10.f[0] = 0.0f;
                  temp_s0->unk10.f[1] = 0.0f;
                  temp_s0->unk10.f[2] = 0.0f;
                }
                if (((s32) temp_s0->unkA8) >= 0x12D)
                {
                  temp_s0->unk1C = 0.0f;
                  temp_s0->flags &= ~0x30;
                }
                else
                {
                  explosionCreateSmoke(&weapon_obj->runtime_pos, arg0->stan, 8, arg0->rooms, (arg0->flags & 8) != 0);
                }
              }
            }
            else
              if (weapon_obj->weaponnum == 0x57)
            {
              if ((sp610 != 0) || (sp60C != 0))
              {
                weapon_obj->timer = 0;
              }
              else
              {
                explosionCreateSmoke(&obj->runtime_pos, arg0->stan, 9, arg0->rooms, (arg0->flags & 8) != 0);
              }
            }
            if ((sp664 == 0) || (sp610 != 0))
            {
              if (((s32) temp_s0->unkAC) < (((s32) D_80048380) - 2))
              {
                if ((weapon_obj->weaponnum == 3) || (weapon_obj->weaponnum == 2))
                {
                  sfx_state = sndPlaySfx((struct ALBankAlt_s *) g_musicSfxBufferPtr, 3, (void *) 0);
                }
                else
                {
                  sfx_state = sndPlaySfx((struct ALBankAlt_s *) g_musicSfxBufferPtr, 45, (void *) 0);
                }
                chrobjSndCreatePostEventDefault(sfx_state, &arg0->pos);
              }
              temp_s0->unkAC = D_80048380;
            }
          }
        }
        if (((weapon_obj->runtime_bitflags & 0x80) && (temp_s0->flags & 0x400)) && (!(D_80048380 & 7)))
        {
          sp564.f[0] = weapon_obj->runtime_pos.f[0] + 400.0f;
          sp564.f[1] = weapon_obj->runtime_pos.f[1] - 1800.0f;
          sp564.f[2] = weapon_obj->runtime_pos.f[2];
          if (!(D_80048380 & 0xF))
          {
            sp564.f[2] += 400.0f;
          }
          else
          {
            sp564.f[2] -= 400.0f;
          }
          explosionCreate((void *) 0, &sp564, weapon_obj->prop->stan, 0x14, 0, 0, weapon_obj->prop->rooms, 0);
          if ((((s32) D_80048380) % 40) == 0)
          {
            explosionCreateSmoke(&sp564, weapon_obj->prop->stan, 0xA, weapon_obj->prop->rooms, 1);
          }
        }
      }
      else
      {
        var_s2_3 = 1;
        if (temp_s0->unk60 < 1.0f)
        {
          temp_s0->unk60 += temp_s0->unk64 * g_GlobalTimerDelta;
          if (g_ClockTimer > 0)
          {
            temp_s0->unk64 *= 1.1f;
          }
          if ((temp_s0->unk60 > 1.0f) || (temp_s0->flags & 8))
          {
            temp_s0->unk60 = 1.0f;
          }
          quaternion_slerp((f32 *) (&temp_s0->unk68), (f32 *) (&temp_s0->unk78), temp_s0->unk60, (f32 *) (&sp550));
          sp70 = &obj->mtx;
          quaternion_to_matrix((f32 *) (&sp550), (f32 *) (&obj->mtx));
          matrix_column_1_scalar_multiply(temp_s0->unkC0, (f32 *) sp70);
          matrix_column_2_scalar_multiply(temp_s0->unkC4, (f32 *) sp70);
          matrix_column_3_scalar_multiply_2(temp_s0->unkC8, (f32 *) sp70);
          var_s2_3 = 0;
        }
        if ((((temp_s0->speed.f[0] != 0.0f) || (temp_s0->speed.f[2] != 0.0f)) || (temp_s0->unk60 < 1.0f)) && (!(temp_s0->flags & 8)))
        {
          sp70 = &obj->mtx;
          sp54C = chrobjGetBboxFromObjectRecord(obj);
          var_s2_3 = 0;
          for (sp548 = 0; sp548 < g_ClockTimer; sp548++)
          {
            sp694.f[0] += temp_s0->speed.f[0];
            sp694.f[2] += temp_s0->speed.f[2];
            if (temp_s0->unk60 >= 1.0f)
            {
              if (temp_s0->unk94 > 0.0f)
              {
                temp_f12_5 = (g_GlobalTimerDelta * temp_s0->unk94) / sqrtf((temp_s0->speed.f[0] * temp_s0->speed.f[0]) + (temp_s0->speed.f[2] * temp_s0->speed.f[2]));
                if (temp_f12_5 >= 1.0f)
                {
                  temp_s0->speed.f[0] = 0.0f;
                  temp_s0->speed.f[2] = 0.0f;
                }
                else
                {
                  temp_s0->speed.f[0] -= temp_s0->speed.f[0] * temp_f12_5;
                  temp_s0->speed.f[2] -= temp_s0->speed.f[2] * temp_f12_5;
                }
              }
              else
              {
                temp_s0->speed.f[0] *= 0.9f;
                temp_s0->speed.f[2] *= 0.9f;
              }
            }
          }

          sub_GAME_7F042EB4(obj, &sp694.f[0], &sp530, &sp53C, 0, 0);
          sp690 = 1;
          temp_f20 = stanGetPositionYValue(arg0->stan, arg0->pos.f[0], arg0->pos.f[2]);
          var_f12 = (temp_f20 - chrpropSumMatrixPosY(sp54C, sp70)) + 4.0f;
          arg0->pos.f[1] = var_f12;
          obj->runtime_pos.f[1] = var_f12;
          if ((temp_s0->speed.f[0] < 0.1f) && (temp_s0->speed.f[0] > (-0.1f)))
          {
            if ((temp_s0->speed.f[2] < 0.1f) && (temp_s0->speed.f[2] > (-0.1f)))
            {
              temp_s0->speed.f[2] = 0.0f;
              temp_s0->speed.f[0] = 0.0f;
            }
          }
        }
        if ((var_s2_3 != 0) || (temp_s0->flags & 8))
        {
          projectileFree(temp_s0);
          obj->projectile = (void *) 0;
          obj->runtime_bitflags &= ~0x80;
          if (arg0->flags & 8)
          {
            arg0->flags |= 0x10;
          }
        }
      }
    }
    if (sp690 != 0)
    {
      sp70 = (Mtxf *) (&obj->runtime_pos);
      chrobjCollisionRelated(obj);
      setupUpdateObjectRoomPosition(obj);
      sub_GAME_7F0402B4(obj->prop, &obj->nextcol);
      detonate_proxmine_In_range((struct coord3d *) sp70);
    }
    if (obj->type == 1)
    {
      struct DoorRecord *door = (struct DoorRecord *) arg0->obj;
      sp67C = door->openPosition;
      if ((((((s32) door->openedTime) > 0) && (((s32) door->openedTime) < (((s32) g_GlobalTimer) - ((s32) door->autoCloseFrames)))) && (door->openstate == 0)) && (!(door->flags & 0x80000000)))
      {
        doorActivate(door, 2);
      }
      if (door->openstate == 3)
      {
        struct DoorRecord *linked_door = door->linkedDoor;
        var_v1_5 = 1;
        while ((linked_door != ((void *) 0)) && (linked_door != door))
        {
          if ((linked_door->openstate != 0) || (linked_door->openPosition > 0.0f))
          {
            var_v1_5 = 0;
          }
          linked_door = linked_door->linkedDoor;
        }

        if (var_v1_5 != 0)
        {
          doorSetOpenState(door, 1);
        }
      }
      if (((door->doorType == 8) && (doorIsClosed(door) != 0)) && (doorIsPadlockFree(door) != 0))
      {
        doorActivateWrapper(arg0);
      }
      if ((door->lastcalc60i < g_GlobalTimer) || (g_ClockTimer == 0))
      {
        door7F054FB4(door);
      }
    }
    else
      if ((obj->type == 6) && (!(obj->flags & 0x10000000)))
    {
      struct CCTVRecord *camera = (struct CCTVRecord *) arg0->obj;
      if (camera->unkD4 != 0)
      {
        sp518 = camera->unkCC;
      }
      else
      {
        sp518 = camera->unkD0;
      }
      sp514 = get_curplayer_positiondata();
      xdiff = sp514->pos.f[0] - obj->runtime_pos.f[0];
      temp_f0_13 = camera->unkE8;
      ydiff = sp514->pos.f[1] - obj->runtime_pos.f[1];
      temp_f14_4 = sp514->pos.f[2] - obj->runtime_pos.f[2];
      var_s2_4 = 1;
      if ((temp_f0_13 > 0.0f) && ((temp_f0_13 * temp_f0_13) < (((xdiff * xdiff) + (ydiff * ydiff)) + (temp_f14_4 * temp_f14_4))))
      {
        var_s2_4 = 0;
      }
      if (obj->flags & 0x20000000)
      {
        var_s2_4 = 0;
      }
      if (var_s2_4 != 0)
      {
        temp_f0_14 = atan2f(xdiff, temp_f14_4);
        var_f2 = camera->unkC8;
        if (var_f2 < 0.0f)
        {
          var_f2 += 6.2831855f;
        }
        else
          if (var_f2 >= 6.2831855f)
        {
          var_f2 -= 6.2831855f;
        }
        var_f2 += camera->unkC4;
        if (var_f2 >= 6.2831855f)
        {
          var_f2 -= 6.2831855f;
        }
        var_f12 = temp_f0_14 - var_f2;
        if (temp_f0_14 < var_f2)
        {
          var_f12 += 6.2831855f;
        }
        var_f12 -= 3.1415927f;
        if (var_f12 < 0.0f)
        {
          var_f12 += 6.2831855f;
        }
        if (var_f12 > 3.1415927f)
        {
          var_f12 -= 6.2831855f;
        }
        if ((var_f12 > 0.7853982f) || (var_f12 < (-0.7853982f)))
        {
          var_s2_4 = 0;
        }
      }
      if (var_s2_4 != 0)
      {
        sp4F0 = arg0->stan;
        bondviewUpdateGuardTankFlagsRelated(sp514, 0);
        if (stanTestLineUnobstructed(&sp4F0, arg0->pos.f[0], arg0->pos.f[2], sp514->pos.f[0], sp514->pos.f[2], 0x1B, 100.0f, 100.0f, 0.0f, 1.0f) != 0)
        {
          camera->timer += g_ClockTimer;
          if (camera->timer >= ((s32) (300.0f * F_80030B14)))
          {
            alarmActivate();
            camera->timer = 0;
          }
        }
        bondviewUpdateGuardTankFlagsRelated(sp514, 1);
      }
      if (camera->unkC8 < sp518)
      {
        if ((sp518 - (((camera->unkD8 * camera->unkD8) * 0.5f) / 0.00065449846f)) <= camera->unkC8)
        {
          camera->unkD8 = (f32) (camera->unkD8 - (0.00065449846f * g_GlobalTimerDelta));
          if (camera->unkD8 < 0.00065449846f)
          {
            camera->unkD8 = 0.00065449846f;
          }
        }
        else
          if (camera->unkD8 < camera->unkDC)
        {
          var_f2_6 = camera->unkD8 + (0.00065449846f * g_GlobalTimerDelta);
          if (camera->unkDC < var_f2_6)
          {
            var_f2_6 = camera->unkDC;
          }
          if (camera->unkC8 < (sp518 - (((var_f2_6 * var_f2_6) * 0.5f) / 0.00065449846f)))
          {
            camera->unkD8 = var_f2_6;
          }
        }
        camera->unkC8 += camera->unkD8 * g_GlobalTimerDelta;
        if (sp518 <= camera->unkC8)
        {
          camera->unkC8 = sp518;
          camera->unkD8 = 0.0f;
          camera->unkD4 = 0;
        }
      }
      else
      {
        if (camera->unkC8 <= (sp518 + (((camera->unkD8 * camera->unkD8) * 0.5f) / 0.00065449846f)))
        {
          camera->unkD8 = (f32) (camera->unkD8 - (0.00065449846f * g_GlobalTimerDelta));
          if (camera->unkD8 < 0.00065449846f)
          {
            camera->unkD8 = 0.00065449846f;
          }
        }
        else
          if (camera->unkD8 < camera->unkDC)
        {
          var_f2_6 = camera->unkD8 + (0.00065449846f * g_GlobalTimerDelta);
          if (camera->unkDC < var_f2_6)
          {
            var_f2_6 = camera->unkDC;
          }
          if (camera->unkC8 > (sp518 + ((0.5f * (var_f2_6 * var_f2_6)) / 0.00065449846f)))
          {
            camera->unkD8 = var_f2_6;
          }
        }
        camera->unkC8 -= camera->unkD8 * g_GlobalTimerDelta;
        if (camera->unkC8 <= sp518)
        {
          camera->unkC8 = sp518;
          camera->unkD8 = 0.0f;
          camera->unkD4 = 1;
        }
      }
    }
    else
      if ((obj->type == 13) && (!(obj->flags & 0x10000000)))
    {
      struct AutogunRecord *autogun = (struct AutogunRecord *) arg0->obj;
      temp_v0_19 = get_curplayer_positiondata();
      sp4AC = 0;
      sp4A8 = 0;
      sp4A4 = 0;
      if (obj->flags2 & 0x80000000)
      {
        if (obj->flags2 & 0x40000000)
        {
          autogun->rot_related = autogun->unk90;
          autogun->unk98 = autogun->unk9C;
        }
        else
          if ((autogun->unk90 == autogun->rot_related) && (autogun->unk9C == autogun->unk98))
        {
          autogun->unk98 = ((((((f32) ((u32) randomGetNext())) * (1.0f / 4294967295)) * 39.0f) + 1.0f) * 6.2831855f) / 360.0f;
          autogun->rot_related = (((f32) ((u32) randomGetNext())) * (1.0f / 4294967295)) * 6.2831855f;
        }
        chrobjCallsApplySpeed(&autogun->unk90, autogun->rot_related, &autogun->unk94, AUTOGUN_P1_ACCEL, AUTOGUN_P1_ACCEL, AUTOGUN_P1_MAX_SPEED);
        chrobjCallsApplySpeed(&autogun->unk9C, autogun->unk98, &autogun->unkA0, AUTOGUN_P2_ACCEL, AUTOGUN_P2_ACCEL, AUTOGUN_P2_MAX_SPEED);
      }
      else
      {
        var_f0_2 = temp_v0_19->pos.f[0] - obj->runtime_pos.f[0];
        sp4BC.f[1] = (temp_v0_19->pos.f[1] - obj->runtime_pos.f[1]) - 20.0f;
        temp_f2_23 = temp_v0_19->pos.f[2] - obj->runtime_pos.f[2];
        sp4B8 = (var_f0_2 * var_f0_2) + (temp_f2_23 * temp_f2_23);
        sp4BC.f[2] = var_f0_2;
        sp4BC.f[0] = temp_f2_23;
        sp4B0 = sqrtf(sp4B8);
        sp4B4 = sp4B0;
        if (obj->flags & 0x08000000)
        {
          sp4B8 += sp4BC.f[1] * sp4BC.f[1];
          sp4B4 = sqrtf(sp4B8);
        }
        sp4A0 = chrlvGetAimLimitAngle(sp4B8);
        sp4D8 = autogun->rot_related;
        sp4D4 = autogun->unk98;
        if (sp4B4 <= autogun->aimdist)
        {
          sp49C = atan2f(sp4BC.f[2], sp4BC.f[0]);
          sp498 = atan2f(sp4BC.f[1], sp4B0);
          if ((obj->flags & 0x40000000) || (obj->flags & 0x20000000))
          {
            sp4AC = 1;
          }
          else
          {
            var_f12_3 = sp49C - autogun->unk90;
            if (var_f12_3 < 0.0f)
            {
              var_f12_3 += 6.2831855f;
            }
            if (var_f12_3 > 3.1415927f)
            {
              var_f12_3 -= 6.2831855f;
            }
            var_f2_6 = sp498 - autogun->unk9C;
            if (var_f2_6 < 0.0f)
            {
              if (sp4B0)
              {
                sp4B0 = (sp4B0) ? (sp4B0) : (sp4B0);
              }
            }
            if ((var_f12_3 < 1.2217306f) && (var_f12_3 > (-1.2217306f)))
            {
              sp4AC = 1;
            }
          }
          if (sp4AC != 0)
          {
            sp494 = sp49C - autogun->rot_related;
            sp490 = arg0->stan;
            if (sp494 < (-3.1415927f))
            {
              sp494 += 6.2831855f;
            }
            else
              if (sp494 >= 3.1415927f)
            {
              sp494 -= 6.2831855f;
            }
            bondviewUpdateGuardTankFlagsRelated(temp_v0_19, 0);
            if ((((sp494 <= autogun->unk88) && (autogun->unk8C <= sp494)) && (stanTestLineUnobstructed(&sp490, arg0->pos.f[0], arg0->pos.f[2], temp_v0_19->pos.f[0], temp_v0_19->pos.f[2], 0x1B, arg0->pos.f[1], arg0->pos.f[1], temp_v0_19->pos.f[1], temp_v0_19->pos.f[1]) != 0)) && ((sp490) == temp_v0_19->stan))
            {
              obj->flags |= 0x20000000;
              sp4A4 = 1;
              sp4D8 = sp49C;
              sp4D4 = sp498;
            }
            else
              if ((autogun->unkB8 >= 0) && ((g_GlobalTimer - 0x78) < autogun->unkB8))
            {
              sp4D8 = autogun->unk90;
              sp4D4 = autogun->unk9C;
            }
            else
            {
              sp4AC = 0;
            }
            bondviewUpdateGuardTankFlagsRelated(temp_v0_19, 1);
          }
        }
        if (sp4AC != 0)
        {
          sp4A0 = chrlvGetAimLimitAngle(sp4B8);
        }
        if (autogun->is_active != 0)
        {
          sp4D8 += (sp4A0 * 0.8f) * sinf((((f32) (((s32) g_GlobalTimer) % 120)) * 6.2831855f) / 120.0f);
          if (sp4D8 < 0.0f)
          {
            sp4D8 += 6.2831855f;
          }
          if (sp4D8 >= 6.2831855f)
          {
            sp4D8 -= 6.2831855f;
          }
        }
        var_f0_2 = sp4D8 - autogun->rot_related;
        if (var_f0_2 < (-3.1415927f))
        {
          var_f0_2 += 6.2831855f;
        }
        else
          if (var_f0_2 >= 3.1415927f)
        {
          var_f0_2 -= 6.2831855f;
        }
        if (autogun->unk88 < var_f0_2)
        {
          sp4D8 = autogun->rot_related + autogun->unk88;
        }
        else
          if (var_f0_2 < autogun->unk8C)
        {
          sp4D8 = autogun->rot_related + autogun->unk8C;
        }
        if (sp4D8 < 0.0f)
        {
          sp4D8 += 6.2831855f;
        }
        if (sp4D8 >= 6.2831855f)
        {
          sp4D8 -= 6.2831855f;
        }
        chrobjCallsApplySpeed(&autogun->unk90, sp4D8, &autogun->unk94, AUTOGUN_TRACK_ACCEL, AUTOGUN_TRACK_ACCEL, autogun->speed);
        chrobjCallsApplySpeed(&autogun->unk9C, sp4D4, &autogun->unkA0, AUTOGUN_TRACK_ACCEL, AUTOGUN_TRACK_ACCEL, autogun->speed);
        sp300 = sp4D8 - autogun->unk90;
        if (sp300 < 0.0f)
        {
          sp300 += 6.2831855f;
        }
        if (sp300 > 3.1415927f)
        {
          sp300 -= 6.2831855f;
        }
        sp394 = sp4D4 - autogun->unk9C;
        if (sp394 < 0.0f)
        {
          sp394 += 6.2831855f;
        }
        if (sp394 > 3.1415927f)
        {
          sp394 -= 6.2831855f;
        }
        autogun->is_active = 0;
        if (sp4AC != 0)
        {
          if ((((sp394 < sp4A0) && ((-sp4A0) < sp394)) && (sp300 < sp4A0)) && ((-sp4A0) < sp300))
          {
            autogun->is_active = 1;
            sp4A8 = 1;
            if (sp4A4 != 0)
            {
              autogun->unkB8 = (s32) g_GlobalTimer;
              autogun->unkBC = (s32) g_GlobalTimer;
            }
          }
          else
          {
            f32 temp_f0_22 = 2.0f * sp4A0;
            if ((((sp394 < temp_f0_22) && ((-temp_f0_22) < sp394)) && (sp300 < temp_f0_22)) && ((-temp_f0_22) < sp300))
            {
              autogun->is_active = 1;
              sp4A8 = 1;
              if (sp4A4 != 0)
              {
                autogun->unkB8 = (s32) g_GlobalTimer;
              }
            }
            else
              if ((autogun->unkB8 >= 0) && ((g_GlobalTimer - 0x78) < autogun->unkB8))
            {
              autogun->is_active = 1;
              sp4A8 = 1;
            }
          }
        }
        if (sp4A8 != 0)
        {
          autogun->unkB0 += 0.009973311f * g_GlobalTimerDelta;
          if (autogun->unkB0 > 0.5983986f)
          {
            autogun->unkB0 = 0.5983986f;
          }
        }
        else
          if (autogun->unkB0 > 0.0f)
        {
          for (var_v0_3 = 0; var_v0_3 < g_ClockTimer; var_v0_3++)
          {
            autogun->unkB0 *= 0.99f;
          }

          if (autogun->unkB0 <= 0.0001f)
          {
            autogun->unkB0 = 0.0f;
          }
        }
        if (autogun->unkB0 > 0.0f)
        {
          autogun->unkB4 += autogun->unkB0 * g_GlobalTimerDelta;
          while (autogun->unkB4 >= 6.2831855f)
          {
            autogun->unkB4 -= 6.2831855f;
          }

        }
      }
    }
    else
      if (obj->type == 39)
    {
      struct VehichleRecord *vehicle_tick = (struct VehichleRecord *) obj;
      sp47C = vehicle_tick->roty;
      sp478 = (void *) 0;
      ai((PropDefHeaderRecord *) vehicle_tick, 1);
      if (vehicle_tick->speedtime60 >= 0.0f)
      {
        if (vehicle_tick->speedtime60 <= g_GlobalTimerDelta)
        {
          vehicle_tick->speed = vehicle_tick->speedaim;
        }
        else
        {
          vehicle_tick->speed += ((vehicle_tick->speedaim - vehicle_tick->speed) * g_GlobalTimerDelta) / vehicle_tick->speedtime60;
        }
        vehicle_tick->speedtime60 -= g_GlobalTimerDelta;
      }
      var_s0 = 0;
      if (((!(obj->flags2 & 0x80000)) && (objIsHealthy(obj) != 0)) && ((vehicle_tick->speed > 0.0f) || (vehicle_tick->speedaim > 0.0f)))
      {
        var_s0 = sub_GAME_7F053894(&vehicle_tick->runtime_pos, 2000.0f, 3000.0f);
      }
      if (var_s0 > 0)
      {
        if (((vehicle_tick->Sound == ((void *) 0)) || (sndGetPlayingState(vehicle_tick->Sound) == 0)) && (lvlGetControlsLockedFlag() == 0))
        {
          sndPlaySfx((struct ALBankAlt_s *) g_musicSfxBufferPtr, 65, &vehicle_tick->Sound);
        }
        if (vehicle_tick->Sound != ((void *) 0))
        {
          sndCreatePostEvent(vehicle_tick->Sound, 8, var_s0);
        }
      }
      else
        if ((vehicle_tick->Sound != ((void *) 0)) && (((((sndGetPlayingState(vehicle_tick->Sound) != 0) & 0xFFu) & 0xFFu) & 0xFFu) & 0xFFu))
      {
        sndDeactivate(vehicle_tick->Sound);
      }
      if (vehicle_tick->path != ((void *) 0))
      {
        sp478 = &g_CurrentSetup.pads[g_CurrentSetup.pathwaypoints[vehicle_tick->path->waypoints[vehicle_tick->nextstep]].padID].pos;
        sp47C = atan2f(sp478->f[0] - vehicle_tick->runtime_pos.f[0], sp478->f[2] - vehicle_tick->runtime_pos.f[2]);
        if (vehicle_tick->flags & 0x20000000)
        {
          vehicle_tick->roty = sp47C;
          obj->flags &= 0xDFFFFFFF;
          sub_GAME_7F044B38(vehicle_tick);
        }
      }
      else
        if (vehicle_tick->flags & 0x20000000)
      {
        vehicle_tick->roty = atan2f(vehicle_tick->mtx.m[2][0], vehicle_tick->mtx.m[2][2]);
        vehicle_tick->flags &= 0xDFFFFFFF;
        sub_GAME_7F044B38(vehicle_tick);
      }
      if (vehicle_tick->speed > 0.0f)
      {
        sp430 = 0.0f;
        sp468 = arg0->stan;
        sp438 = vehicle_tick->roty;
        sp434 = vehicle_tick->turnrot60;
        if (sp478 != ((void *) 0))
        {
          sp430 = 0.0f;
          sp43C.f[0] = sinf(vehicle_tick->roty);
          sp43C.f[1] = 0.0f;
          sp43C.f[2] = cosf(vehicle_tick->roty);
          if (chrlvGeometryRelated7F02FC34(&vehicle_tick->runtime_pos, &sp43C, sp478, 10.0f) != 0)
          {
            sp47C = vehicle_tick->roty;
          }
        }
        chrobjCallsApplySpeed(&vehicle_tick->roty, sp47C, &vehicle_tick->turnrot60, VEHICLE_TURN_ACCEL, VEHICLE_TURN_DECEL, VEHICLE_TURN_MAX_SPEED);
        while (vehicle_tick->roty >= 6.2831855f)
        {
          vehicle_tick->roty -= 6.2831855f;
        }

        while (vehicle_tick->roty < 0.0f)
        {
          vehicle_tick->roty += 6.2831855f;
        }

        if (sp47C == vehicle_tick->roty)
        {
          if ((vehicle_tick->turnrot60 <= VEHICLE_TURN_DECEL) && (vehicle_tick->turnrot60 >= (-VEHICLE_TURN_DECEL)))
          {
            vehicle_tick->turnrot60 = 0.0f;
          }
        }
        temp_s0_5 = (f32 *) model->obj->Switches[3]->Data;
        if (g_GlobalTimerDelta > 0.0f)
        {
          sp430 = (vehicle_tick->roty - sp438) / g_GlobalTimerDelta;
        }
        if (sp430 < 0.0f)
        {
          sp430 += 6.2831855f;
        }
        sp460 = ((temp_s0_5[2] * model->scale) * sinf(sp430)) * g_GlobalTimerDelta;
        sp43C.f[0] = sinf(vehicle_tick->roty);
        sp43C.f[1] = 0.0f;
        sp43C.f[2] = cosf(vehicle_tick->roty);
        sp694.f[0] = (vehicle_tick->runtime_pos.f[0] + ((vehicle_tick->speed * g_GlobalTimerDelta) * sp43C.f[0])) - (sp43C.f[2] * sp460);
        sp694.f[1] = vehicle_tick->runtime_pos.f[1];
        sp694.f[2] = (sp43C.f[0] * sp460) + (((vehicle_tick->speed * g_GlobalTimerDelta) * sp43C.f[2]) + vehicle_tick->runtime_pos.f[2]);
        if ((stanTestLineUnobstructed(&sp468, arg0->pos.f[0], arg0->pos.f[2], sp694.f[0], sp694.f[2], 0x1F, 0.0f, 1.0f, 0.0f, 1.0f) != 0) && (stanTestVolume(&sp468, sp694.f[0], sp694.f[2], 10.0f, 0x1F, 0.0f, 1.0f) < 0))
        {
          sp44C = arg0->stan;
          sp450.f[0] = arg0->pos.f[0];
          sp450.f[1] = arg0->pos.f[1];
          sp450.f[2] = arg0->pos.f[2];
          arg0->stan = sp468;
          vehicle_tick->runtime_pos.f[0] = (arg0->pos.f[0] = sp694.f[0]);
          vehicle_tick->runtime_pos.f[2] = (arg0->pos.f[2] = sp694.f[2]);
          chrobjCollisionRelated(obj);
          setupUpdateObjectRoomPosition(obj);
          var_s2_5 = sub_GAME_7F0448A8(arg0);
          if (var_s2_5 != 0)
          {
            f32 collision_arg0;
            f32 collision_arg1;
            temp_v0_25 = (f32 *) model->obj->Switches[10]->Data;
            vec424.f[0] = vehicle_tick->mtx.m[0][0] * temp_v0_25[1];
            vec424.f[2] = temp_v0_25[1] * vehicle_tick->mtx.m[0][2];
            vec418.f[0] = vehicle_tick->mtx.m[0][0] * temp_v0_25[2];
            vec418.f[2] = temp_v0_25[2] * vehicle_tick->mtx.m[0][2];
            vec40C.f[0] = vehicle_tick->mtx.m[2][0] * temp_v0_25[5];
            vec40C.f[2] = temp_v0_25[5] * vehicle_tick->mtx.m[2][2];
            vec400.f[0] = vehicle_tick->mtx.m[2][0] * temp_v0_25[6];
            vec400.f[2] = temp_v0_25[6] * vehicle_tick->mtx.m[2][2];
            sp468 = arg0->stan;
            if (((((walkTilesBetweenPoints_NoCallback(&sp468, arg0->pos.f[0], arg0->pos.f[2], (arg0->pos.f[0] + vec424.f[0]) + vec40C.f[0], (arg0->pos.f[2] + vec424.f[2]) + vec40C.f[2]) == 0) || (walkTilesBetweenPoints_NoCallback(&sp468, vec40C.f[0] + (vec424.f[0] + arg0->pos.f[0]), vec40C.f[2] + (arg0->pos.f[2] + vec424.f[2]), vec40C.f[0] + (vec418.f[0] + arg0->pos.f[0]), vec40C.f[2] + (vec418.f[2] + arg0->pos.f[2])) == 0)) || (walkTilesBetweenPoints_NoCallback(&sp468, (vec418.f[0] + arg0->pos.f[0]) + vec40C.f[0], (arg0->pos.f[2] + vec418.f[2]) + vec40C.f[2], (arg0->pos.f[0] + vec418.f[0]) + vec400.f[0], (arg0->pos.f[2] + vec418.f[2]) + vec400.f[2]) == 0)) || ((collision_arg0 = vec400.f[0] + (vec418.f[0] + arg0->pos.f[0]), collision_arg1 = vec400.f[2] + (arg0->pos.f[2] + vec418.f[2]), walkTilesBetweenPoints_NoCallback(&sp468, collision_arg0, collision_arg1, vec400.f[0] + (vec424.f[0] + arg0->pos.f[0]), vec400.f[2] + (vec424.f[2] + arg0->pos.f[2])) == 0))) || (walkTilesBetweenPoints_NoCallback(&sp468, (vec424.f[0] + arg0->pos.f[0]) + vec400.f[0], (arg0->pos.f[2] + vec424.f[2]) + vec400.f[2], (arg0->pos.f[0] + vec424.f[0]) + vec40C.f[0], (arg0->pos.f[2] + vec424.f[2]) + vec40C.f[2]) == 0))
            {
              var_s2_5 = 0;
            }
          }
          if (var_s2_5 != 0)
          {
            sub_GAME_7F044B38(vehicle_tick);
            sub_GAME_7F0402B4(arg0, &vehicle_tick->nextcol);
            detonate_proxmine_In_range(&vehicle_tick->runtime_pos);
            if ((sp478 != ((void *) 0)) && (chrlvIsArrivingLaterallyAtPos(&sp450, &sp694, sp478, 100.0f) != 0))
            {
              vehicle_tick->nextstep++;
              if (vehicle_tick->path->waypoints[vehicle_tick->nextstep] < 0)
              {
                vehicle_tick->path = (void *) 0;
                vehicle_tick->speedaim = 0.0f;
                vehicle_tick->speedtime60 = 60.0f;
              }
            }
          }
          else
          {
            if (vehicle_tick->speedtime60 < 0.0f)
            {
              vehicle_tick->speedaim = (f32) vehicle_tick->speed;
              vehicle_tick->speedtime60 = 60.0f;
            }
            vehicle_tick->speed = 0.0f;
            vehicle_tick->roty = sp438;
            vehicle_tick->turnrot60 = sp434;
            arg0->stan = sp44C;
            obj->runtime_pos.f[0] = (arg0->pos.f[0] = sp450.f[0]);
            obj->runtime_pos.f[1] = (arg0->pos.f[1] = sp450.f[1]);
            obj->runtime_pos.f[2] = (arg0->pos.f[2] = sp450.f[2]);
            chrobjCollisionRelated(obj);
            setupUpdateObjectRoomPosition(obj);
          }
        }
        else
        {
          if (vehicle_tick->speedtime60 < 0.0f)
          {
            vehicle_tick->speedaim = (f32) vehicle_tick->speed;
            vehicle_tick->speedtime60 = 60.0f;
          }
          vehicle_tick->speed = 0.0f;
          vehicle_tick->roty = sp438;
          vehicle_tick->turnrot60 = sp434;
        }
      }
      else
        if (vehicle_tick->flags & 0x20000000)
      {
        vehicle_tick->roty = atan2f(vehicle_tick->mtx.m[2][0], vehicle_tick->mtx.m[2][2]);
        vehicle_tick->flags &= 0xDFFFFFFF;
        sub_GAME_7F044B38(vehicle_tick);
      }
    }
    else
      if (obj->type == 40)
    {
      struct AircraftRecord *aircraft_tick = (struct AircraftRecord *) obj;
      ai((PropDefHeaderRecord *) aircraft_tick, 1);
      temp_s0_6 = aircraft_tick->model;
      if (temp_s0_6->anim != ((void *) 0))
      {
        setsuboffset(temp_s0_6, &aircraft_tick->runtime_pos);
        temp_s0_6 = aircraft_tick->model;
        if (temp_s0_6->anim == animation_table_ptrs2[1])
        {
          sub_GAME_7F06CE84(temp_s0_6, 10.438f);
          setsubroty(aircraft_tick->model, 3.1415927f);
        }
        else
          if (bossGetStageNum() == 22)
        {
          sub_GAME_7F06CE84(aircraft_tick->model, 1.0438f);
          setsubroty(aircraft_tick->model, 2.3561945f);
        }
        else
          if (bossGetStageNum() == 26)
        {
          sub_GAME_7F06CE84(aircraft_tick->model, 1.0438f);
          setsubroty(aircraft_tick->model, 3.9269907f);
        }
        else
        {
          sub_GAME_7F06CE84(aircraft_tick->model, 1.0438f);
          setsubroty(aircraft_tick->model, 0.0f);
        }
        modelTickAnimQuarterSpeed(aircraft_tick->model, g_ClockTimer, 1);
        subcalcpos(aircraft_tick->model);
        getsuboffset(aircraft_tick->model, &aircraft_tick->runtime_pos);
        arg0->pos.f[0] = aircraft_tick->runtime_pos.f[0];
        arg0->pos.f[2] = aircraft_tick->runtime_pos.f[2];
        if (aircraft_tick->pad < 10000)
        {
          var_v1_4 = &g_CurrentSetup.pads[aircraft_tick->pad];
        }
        else
        {
          var_v1_4 = (PadRecord *) (&g_CurrentSetup.boundpads[aircraft_tick->pad - 10000]);
        }
        arg0->pos.f[1] = var_v1_4->pos.f[1] + aircraft_tick->runtime_pos.f[1];
        aircraft_tick->runtime_pos.f[1] = arg0->pos.f[1];
        setsuboffset(aircraft_tick->model, &aircraft_tick->runtime_pos);
      }
      var_f12 = aircraft_tick->speedtime60;
      if (var_f12 >= 0.0f)
      {
        if (var_f12 <= g_GlobalTimerDelta)
        {
          aircraft_tick->speed = (f32) aircraft_tick->speedaim;
          var_f12 = *((volatile f32 *) (&aircraft_tick->speedtime60));
        }
        else
        {
          aircraft_tick->speed += ((aircraft_tick->speedaim - aircraft_tick->speed) * g_GlobalTimerDelta) / var_f12;
        }
        aircraft_tick->speedtime60 = (f32) (var_f12 - g_GlobalTimerDelta);
      }
      if (aircraft_tick->rotaryspeedtime >= 0.0f)
      {
        if (aircraft_tick->rotaryspeedtime <= g_GlobalTimerDelta)
        {
          aircraft_tick->rotaryspeed = (f32) aircraft_tick->rotaryspeedaim;
        }
        else
        {
          var_f12 = aircraft_tick->rotaryspeed;
          aircraft_tick->rotaryspeed += ((aircraft_tick->rotaryspeedaim - aircraft_tick->rotaryspeed) * g_GlobalTimerDelta) / aircraft_tick->rotaryspeedtime;
        }
        aircraft_tick->rotaryspeedtime -= g_GlobalTimerDelta;
      }
      var_s0_2 = 0;
      if ((((!(aircraft_tick->flags2 & 0x80000)) && (objIsHealthy(obj) != 0)) && (aircraft_tick->rotaryspeed != 0.0f)) && (!(aircraft_tick->flags & 0x20000000)))
      {
        var_s0_2 = sub_GAME_7F053894(&aircraft_tick->runtime_pos, 5000.0f, 6000.0f);
      }
      if (var_s0_2 > 0)
      {
        if (((aircraft_tick->Sound == ((void *) 0)) || (sndGetPlayingState(aircraft_tick->Sound) == 0)) && (lvlGetControlsLockedFlag() == 0))
        {
          sndPlaySfx((struct ALBankAlt_s *) g_musicSfxBufferPtr, 58, &aircraft_tick->Sound);
        }
        if (aircraft_tick->Sound != ((void *) 0))
        {
          sndCreatePostEvent(aircraft_tick->Sound, 8, var_s0_2);
        }
      }
      else
        if ((aircraft_tick->Sound != ((void *) 0)) && (sndGetPlayingState(aircraft_tick->Sound) != 0))
      {
        sndDeactivate(aircraft_tick->Sound);
      }
    }
  }
  if (obj->type == 47)
  {
    struct TintedGlassRecord *tinted_glass = (struct TintedGlassRecord *) arg0->obj;
    tinted_glass->calculatedopacity = glassCalculateOpacity(&obj->runtime_pos, tinted_glass->TintDist, tinted_glass->CullDist, tinted_glass->unk90);
    if ((tinted_glass->portalnum >= 0) && (sp674 == 1))
    {
      if (tinted_glass->calculatedopacity == 0xFF)
      {
        bgToggleDataPortalsContrlBytes1Bit1(tinted_glass->portalnum, 0);
      }
      else
      {
        bgToggleDataPortalsContrlBytes1Bit1(tinted_glass->portalnum, 1);
      }
    }
    sp670 = 0;
  }
  else
    if ((obj->type == 1) && (((struct DoorRecord *) obj)->doorFlags & 0x2))
  {
    struct DoorRecord *door = (struct DoorRecord *) arg0->obj;
    var_s2_6 = 1;
    door->calculatedopacity = glassCalculateOpacity(&obj->runtime_pos, door->TintDist, *((s32 *) (((u8 *) door) + 0xC4)), 0.0f);
    if (sp674 == 1)
    {
      if ((door->calculatedopacity != 0xFF) || (door->openPosition > 0.0f))
      {
        var_s2_6 = 0;
      }
      temp_v0_29 = model->obj;
      if ((temp_v0_29->Skeleton == (&skeleton_door)) && (modelGetNodeRwData(model, temp_v0_29->Switches[1])->Switch.visible == 0))
      {
        var_s2_6 = 0;
      }
      if (var_s2_6 != 0)
      {
        doorDeactivatePortal(door);
      }
      else
      {
        doorActivatePortal(door);
      }
    }
  }
  if ((obj->type == 45) && (get_ptr_for_players_tank() == arg0))
  {
    var_v1_5 = 1;
  }
  else
    if (obj->flags2 & 0x04000000)
  {
    var_v1_5 = 1;
  }
  else
  {
    var_v1_5 = ((!(obj->runtime_bitflags & 0x800)) && (!(obj->flags2 & 0x80000))) ? (sub_GAME_7F054D6C(arg0, &obj->runtime_pos, getinstsize(model), sp670)) : (0);
  }
  if (var_v1_5 != 0)
  {
    if (sp678 != 0)
    {
      update_color_shading(&obj->shadecol, &obj->nextcol);
    }
    arg0->flags |= 2;
    mtxs = dynAllocate(model->obj->numMatrices << 6);
    model->render_pos = (RenderPosView *) mtxs;
    if (obj->type == 1)
    {
      sp39C = (struct DoorRecord *) arg0->obj;
      door7F0526EC(sp39C, mtxs);
      matrix_4x4_multiply_homogeneous_in_place(camGetWorldToScreenMtxf(), mtxs);
      if (model->obj->Skeleton == (&skeleton_eyelid_door))
      {
        Mtxf *eyelid_mtx;
        sp394 = 6.2831855f - ((sp39C->openPosition * 6.2831855f) / 360.0f);
        eyelid_mtx = &mtxs[1];
        temp_s0_10 = (Mtxf *) model->obj->Switches[1]->Data;
        matrix_4x4_set_rotation_around_x(sp394, eyelid_mtx);
        matrix_4x4_set_position((struct coord3d *) temp_s0_10, eyelid_mtx);
        matrix_4x4_multiply_in_place(mtxs, eyelid_mtx);
        eyelid_mtx = &mtxs[2];
        temp_s0_10 = (Mtxf *) model->obj->Switches[2]->Data;
        matrix_4x4_set_rotation_around_x(6.2831855f - sp394, eyelid_mtx);
        matrix_4x4_set_position((struct coord3d *) temp_s0_10, eyelid_mtx);
        matrix_4x4_multiply_in_place(mtxs, eyelid_mtx);
      }
      else
        if (model->obj->Skeleton == (&skeleton_iris_door))
      {
        sp380 = 0.0f;
        sp384 = (sp39C->openPosition * 6.2831855f) / 360.0f;
        temp_f0_31 = sp39C->maxFrac * 0.3f;
        if (temp_f0_31 < sp39C->openPosition)
        {
          sp380 = (((sp39C->maxFrac * (sp39C->openPosition - temp_f0_31)) / (sp39C->maxFrac - temp_f0_31)) * 6.2831855f) / 360.0f;
          if (sp678 != 0)
          {
            if (sp67C <= temp_f0_31)
            {
              chrobjSndCreatePostEventDefault(sndPlaySfx((struct ALBankAlt_s *) g_musicSfxBufferPtr, 202, (void *) 0), &arg0->pos);
            }
          }
        }
        else
          if (sp678 != 0)
        {
          if (temp_f0_31 < sp67C)
          {
            chrobjSndCreatePostEventDefault(sndPlaySfx((struct ALBankAlt_s *) g_musicSfxBufferPtr, 203, (void *) 0), &arg0->pos);
          }
        }
        sp38C = 0;
        do
        {
          temp_v0_32 = sp38C << 1;
          var_v1_5 = temp_v0_32 + 2;
          sp390 = (Mtxf *) model->obj->Switches[temp_v0_32 + 1]->Data;
          matrix_4x4_set_rotation_around_z(sp380, (Mtxf *) ((((u8 *) mtxs) + (temp_v0_32 * (sizeof(Mtxf)))) + (sizeof(Mtxf))));
          matrix_4x4_set_position((struct coord3d *) sp390, (Mtxf *) ((((u8 *) mtxs) + (temp_v0_32 * (sizeof(Mtxf)))) + (sizeof(Mtxf))));
          matrix_4x4_multiply_in_place(mtxs, (Mtxf *) ((((u8 *) mtxs) + (temp_v0_32 * (sizeof(Mtxf)))) + (sizeof(Mtxf))));
          sp390 = (Mtxf *) model->obj->Switches[var_v1_5]->Data;
          matrix_4x4_set_rotation_around_z(sp384, (Mtxf *) (((u8 *) mtxs) + (var_v1_5 << 6)));
          matrix_4x4_set_position((struct coord3d *) sp390, &mtxs[var_v1_5]);
          matrix_4x4_multiply_in_place((Mtxf *) ((((u8 *) mtxs) + (temp_v0_32 * (sizeof(Mtxf)))) + (sizeof(Mtxf))), &mtxs[var_v1_5]);
          sp38C++;
        }
        while (sp38C != 6);
      }
    }
    else
    {
      matrix_4x4_copy(&obj->mtx, &sp3A4);
      matrix_4x4_set_position(&obj->runtime_pos, &sp3A4);
      matrix_4x4_multiply_homogeneous(camGetWorldToScreenMtxf(), &sp3A4, mtxs);
      if (obj->type == 6)
      {
        sp370 = (struct CCTVRecord *) arg0->obj;
        var_f12_7 = sp370->unkC8;
        temp_s0_13 = (struct coord3d *) model->obj->Switches[0]->Data;
        if (var_f12_7 < 0.0f)
        {
          var_f12_7 += 6.2831855f;
        }
        else
          if (var_f12_7 >= 6.2831855f)
        {
          var_f12_7 -= 6.2831855f;
        }
        matrix_4x4_set_rotation_around_y(var_f12_7, &sp320);
        matrix_4x4_multiply(&sp320, &sp370->unk84, &mtxs[1]);
        (&sp360)->f[0] = temp_s0_13->f[0];
        (&sp360)->f[1] = temp_s0_13->f[1];
        (&sp360)->f[2] = temp_s0_13->f[2];
        mtx4TransformVecInPlace(&sp3A4, &sp360);
        matrix_4x4_set_position(&sp360, &mtxs[1]);
        matrix_4x4_multiply_homogeneous_in_place(camGetWorldToScreenMtxf(), &mtxs[1]);
      }
      else
        if (obj->type == 13)
      {
        sp318 = (struct AutogunRecord *) arg0->obj;
        sp304 = sp318->unk90 + 1.5707964f;
        sp300 = -sp318->unk9C;
        if (sp304 >= 6.2831855f)
        {
          sp304 -= 6.2831855f;
        }
        temp_s0_14 = model->obj->Switches[1]->Data;
        (&sp308)->f[0] = temp_s0_14->f[0];
        (&sp308)->f[1] = temp_s0_14->f[1];
        (&sp308)->f[2] = temp_s0_14->f[2];
        mtx4TransformVecInPlace(&sp3A4, &sp308);
        matrix_4x4_set_rotation_around_y(sp304, &mtxs[1]);
        matrix_4x4_set_position(&sp308, &mtxs[1]);
        matrix_scalar_multiply(sp318->model->scale, mtxs[1].m[0]);
        matrix_4x4_multiply_homogeneous_in_place(camGetWorldToScreenMtxf(), &mtxs[1]);
        temp_s0_13 = (struct coord3d *) model->obj->Switches[2]->Data;
        matrix_4x4_set_rotation_around_z(sp300, &mtxs[2]);
        matrix_4x4_set_position(temp_s0_13, &mtxs[2]);
        matrix_4x4_multiply_homogeneous_in_place(&mtxs[1], &mtxs[2]);
        if (model->obj->Switches[3] != ((void *) 0))
        {
          sp2FC = modelFindNodeMtx(model, model->obj->Switches[3], 0);
          temp_s0_13 = (struct coord3d *) model->obj->Switches[3]->Data;
          matrix_4x4_set_rotation_around_x(sp318->unkB4, sp2FC);
          matrix_4x4_set_position(temp_s0_13, sp2FC);
          matrix_4x4_multiply_homogeneous_in_place(&mtxs[2], sp2FC);
        }
        if (model->obj->Switches[4] != ((void *) 0))
        {
          sp2FC = modelFindNodeMtx(model, model->obj->Switches[4], 0);
          temp_s0_14 = (struct coord3d *) model->obj->Switches[4]->Data;
          matrix_4x4_set_identity_and_position(temp_s0_14, sp2FC);
          matrix_4x4_multiply_homogeneous_in_place(&mtxs[2], sp2FC);
        }
        if (model->obj->Switches[6] != ((void *) 0))
        {
          sp2FC = modelFindNodeMtx(model, model->obj->Switches[6], 0);
          temp_s0_13 = (struct coord3d *) model->obj->Switches[6]->Data;
          matrix_4x4_set_rotation_around_x(sp318->unkB4, sp2FC);
          matrix_4x4_set_position(temp_s0_13, sp2FC);
          matrix_4x4_multiply_homogeneous_in_place(&mtxs[2], sp2FC);
        }
      }
      else
        if (obj->type == 8)
      {
        Mtxf *temp_a1_4;
        sp2F8 = 1;
        temp_a1_4 = &mtxs[1];
        if (sp2F8 < model->obj->numMatrices)
        {
          do
          {
            matrix_4x4_set_identity(temp_a1_4);
            sp2F8++;
            temp_a1_4++;
          }
          while (sp2F8 < model->obj->numMatrices);
        }
      }
      else
        if (obj->type == 12)
      {
        matrix_4x4_set_identity_and_position(model->obj->Switches[0]->Data, &mtxs[1]);
        matrix_4x4_multiply_homogeneous_in_place(&mtxs[0], &mtxs[1]);
        matrix_4x4_set_identity_and_position(model->obj->Switches[1]->Data, &mtxs[2]);
        matrix_4x4_multiply_homogeneous_in_place(&mtxs[0], &mtxs[2]);
        matrix_4x4_set_identity_and_position(model->obj->Switches[2]->Data, &mtxs[3]);
        matrix_4x4_multiply_homogeneous_in_place(&mtxs[0], &mtxs[3]);
        matrix_4x4_set_identity_and_position(model->obj->Switches[3]->Data, &mtxs[4]);
        matrix_4x4_multiply_homogeneous_in_place(&mtxs[0], &mtxs[4]);
      }
      else
        if (obj->type == 39)
      {
        struct VehichleRecord *vehicle_render = (struct VehichleRecord *) obj;
        var_f0_3 = 0.0f;
        temp_v1_7 = model->obj->Switches;
        sp260 = temp_v1_7[1]->Data;
        sp25C = temp_v1_7[2]->Data;
        sp258 = temp_v1_7[3]->Data;
        sp254 = temp_v1_7[4]->Data;
        temp_v0_40 = temp_v1_7[6]->Data;
        sp250 = (temp_v0_40[4] - temp_v0_40[3]) * model->scale;
        if (sp678 != 0)
        {
          var_f0_3 = ((vehicle_render->speed * g_GlobalTimerDelta) * 6.2831855f) / ((sp250 * 6.2831855f) * 0.5f);
          vehicle_render->wheelxrot += var_f0_3;
          while (vehicle_render->wheelxrot >= 6.2831855f)
          {
            vehicle_render->wheelxrot -= 6.2831855f;
          }

          while (vehicle_render->wheelxrot < 0.0f)
          {
            vehicle_render->wheelxrot += 6.2831855f;
          }

        }
        vehicle_render->wheelxrot += var_f0_3;
        while (vehicle_render->wheelxrot >= 6.2831855f)
        {
          vehicle_render->wheelxrot -= 6.2831855f;
        }

        while (vehicle_render->wheelxrot < 0.0f)
        {
          vehicle_render->wheelxrot += 6.2831855f;
        }

        matrix_4x4_set_rotation_around_x(vehicle_render->wheelxrot, &sp2AC);
        if (vehicle_render->speed > 0.0f)
        {
          sp24C = vehicle_render->turnrot60;
          sp250 = (sp258->f[2] - sp260->f[2]) * model->scale;
          if (sp24C < 0.0f)
          {
            sp24C = -sp24C;
          }
          sp248 = sinf(sp24C) * sp250;
          vehicle_render->wheelyrot = atan2f(sp248, (cosf(sp24C) * sp250) - (sp250 - vehicle_render->speed));
          if (vehicle_render->wheelyrot < sp24C)
          {
            vehicle_render->wheelyrot = sp24C;
          }
          if (vehicle_render->turnrot60 > 0.0f)
          {
            vehicle_render->wheelyrot = 6.2831855f - vehicle_render->wheelyrot;
          }
        }
        matrix_4x4_set_rotation_around_y(vehicle_render->wheelyrot, &sp26C);
        matrix_4x4_copy(&sp2AC, &mtxs[3]);
        matrix_4x4_set_position(sp258, &mtxs[3]);
        matrix_4x4_multiply_homogeneous_in_place(&mtxs[0], &mtxs[3]);
        matrix_4x4_copy(&sp2AC, &mtxs[4]);
        matrix_4x4_set_position(sp254, &mtxs[4]);
        matrix_4x4_multiply_homogeneous_in_place(&mtxs[0], &mtxs[4]);
        matrix_4x4_multiply_homogeneous_in_place(&sp26C, &sp2AC);
        matrix_4x4_copy(&sp2AC, &mtxs[1]);
        matrix_4x4_set_position(sp260, &mtxs[1]);
        matrix_4x4_multiply_homogeneous_in_place(&mtxs[0], &mtxs[1]);
        matrix_4x4_copy(&sp2AC, &mtxs[2]);
        matrix_4x4_set_position(sp25C, &mtxs[2]);
        matrix_4x4_multiply_homogeneous_in_place(&mtxs[0], &mtxs[2]);
      }
      else
        if (obj->type == 40)
      {
        struct AircraftRecord *aircraft_render = (struct AircraftRecord *) obj;
        sp1FC = model->obj->Switches[2]->Data;
        if ((g_ClockTimer > 0) && (sp678 != 0))
        {
          aircraft_render->rotoryrot += aircraft_render->rotaryspeed;
          while (aircraft_render->rotoryrot >= 6.2831855f)
          {
            aircraft_render->rotoryrot -= 6.2831855f;
          }

          while (aircraft_render->rotoryrot < 0.0f)
          {
            aircraft_render->rotoryrot += 6.2831855f;
          }

        }
        if (aircraft_render->model->anim != ((void *) 0))
        {
          sp1B0 = D_80030B34;
          sp1B0.unk_matrix = camGetWorldToScreenMtxf();
          sp1B0.mtxlist = &mtxs[0];
          subcalcmatrices(&sp1B0, aircraft_render->model);
        }
        else
        {
          matrix_4x4_copy(&mtxs[0], &mtxs[1]);
        }
        if (aircraft_render->flags & 0x20000000)
        {
          matrix_4x4_set_rotation_around_z(aircraft_render->rotoryrot, &sp200);
        }
        else
        {
          matrix_4x4_set_rotation_around_y(aircraft_render->rotoryrot, &sp200);
        }
        matrix_4x4_copy(&sp200, &mtxs[2]);
        matrix_4x4_set_position(sp1FC, &mtxs[2]);
        matrix_4x4_multiply_homogeneous_in_place(&mtxs[1], &mtxs[2]);
        if (model->obj->Switches[3] != ((void *) 0))
        {
          temp_s0_14 = (struct coord3d *) model->obj->Switches[3]->Data;
          temp_s0_21 = modelFindNodeMtx(model, model->obj->Switches[3], 0);
          matrix_4x4_set_rotation_around_x(aircraft_render->rotoryrot, &sp200);
          matrix_4x4_copy(&sp200, temp_s0_21);
          matrix_4x4_set_position(temp_s0_14, temp_s0_21);
          matrix_4x4_multiply_homogeneous_in_place(&mtxs[1], temp_s0_21);
        }
        if (model->obj->Switches[4] != ((void *) 0))
        {
          temp_s0_14 = (struct coord3d *) model->obj->Switches[4]->Data;
          temp_s0_21 = modelFindNodeMtx(model, model->obj->Switches[4], 0);
          matrix_4x4_set_identity_and_position(temp_s0_14, temp_s0_21);
          matrix_4x4_multiply_homogeneous_in_place(&mtxs[1], temp_s0_21);
        }
      }
      else
        if (obj->type == 45)
      {
        struct TankRecord *tank_render = (struct TankRecord *) obj;
        temp_v1_8 = model->obj->Switches;
        sp168 = temp_v1_8[1]->Data;
        sp164 = temp_v1_8[3]->Data;
        sp160 = temp_v1_8[4]->Data;
        sp15C = temp_v1_8[2]->Data;
        sp158 = temp_v1_8[6]->Data;
        sp154 = -tank_render->turret_vertical_angle;
        if (sp154 < 0.0f)
        {
          sp154 += 6.2831855f;
        }
        var_f12_9 = -tank_render->turret_orientation_angle;
        if (var_f12_9 < 0.0f)
        {
          var_f12_9 += 6.2831855f;
        }
        matrix_4x4_set_rotation_around_y(var_f12_9, &mtxs[1]);
        matrix_4x4_set_position(sp168, &mtxs[1]);
        matrix_4x4_multiply_homogeneous_in_place(&mtxs[0], &mtxs[1]);
        matrix_4x4_set_rotation_around_x(sp154, &mtxs[3]);
        matrix_4x4_set_position(sp164, &mtxs[3]);
        matrix_4x4_multiply_homogeneous_in_place(&mtxs[1], &mtxs[3]);
        matrix_4x4_set_rotation_around_y(1.5707964f, &mtxs[4]);
        matrix_4x4_set_position(sp160, &mtxs[4]);
        matrix_4x4_multiply_homogeneous_in_place(&mtxs[3], &mtxs[4]);
        matrix_4x4_set_identity_and_position(sp15C, &mtxs[2]);
        matrix_4x4_multiply_homogeneous_in_place(&mtxs[1], &mtxs[2]);
        matrix_4x4_multiply_homogeneous(currentPlayerGetMatrix10D4(), &mtxs[1], &spE8);
        sub_GAME_7F03F540(sp158, &spE8, &tank_render->rect,
                          (struct collision_data *)&tank_render->unk80);
        if (model->obj->Switches[7] != ((void *) 0))
        {
          modelGetNodeRwData(model, model->obj->Switches[7])->Gunfire.visible = (s16) tank_render->is_firing_tank;
        }
        if (model->obj->Switches[8] != ((void *) 0))
        {
          modelGetNodeRwData(model, model->obj->Switches[8])->Switch.visible = 0;
        }
      }
    }
    modelUpdateRelationsQuick(model, model->obj->RootNode);
    arg0->zDepth = -((Mtxf *) model->render_pos)[0].m[3][2];
    chrobjWeaponTick(arg0);
    {
      struct PropRecord *current = arg0->child;
      while (current != ((void *) 0))
      {
        sp684 = current->prev;
        sub_GAME_7F0442DC(current);
        current = sp684;
      }

    }
  }
  else
  {
    arg0->flags &= 0xFFFD;
    chrobjWeaponTick(arg0);
    {
      struct PropRecord *current = arg0->child;
      while (current != ((void *) 0))
      {
        sp684 = current->prev;
        sub_GAME_7F04424C(current);
        current = sp684;
      }

    }
  }
  if (obj->runtime_bitflags & 0x100)
  {
    obj->runtime_bitflags &= ~0x100;
    maybe_detonate_object(obj, ((((f32) ((u32) randomGetNext())) * (1.0f / 4294967295)) * 4.0f) + 2.0f, &arg0->pos, 0, (s32) (((u32) (obj->runtime_bitflags & 0x00060000)) >> 0x11));
  }
  if (sp678 != 0)
  {
    if (obj->type == 1)
    {
      sub_GAME_7F053A3C((struct DoorRecord *) arg0->obj);
    }
    else
      if ((obj->type == 13) && (lvlGetControlsLockedFlag() == 0))
    {
      struct AutogunRecord *autogun = (struct AutogunRecord *) arg0->obj;
      sp13C = 0;
      sp138 = 0;
      if ((autogun->is_active != 0) && (!(obj->flags & 0x10000000)))
      {
        autogun->unkAC++;
        sp13C = (autogun->unkAC & 1) == 0;
        if (model->obj->Switches[5] != 0)
        {
          sp138 = (autogun->unkAC & 1) == 1;
        }
        if (autogun->unkC0 < g_GlobalTimer)
        {
          if ((autogun->unkC4 != ((void *) 0)) && (sndGetPlayingState(autogun->unkC4) != 0))
          {
            sndDeactivate(autogun->unkC4);
          }
          if ((autogun->unkC8 != ((void *) 0)) && (sndGetPlayingState(autogun->unkC8) != 0))
          {
            sndDeactivate(autogun->unkC8);
          }
          if (autogun->unkC4 == ((void *) 0))
          {
            sndPlaySfx((struct ALBankAlt_s *) g_musicSfxBufferPtr, 253, &autogun->unkC4);
            chrobjSndCreatePostEventDefault(autogun->unkC4, &arg0->pos);
          }
          else
            if (autogun->unkC8 == ((void *) 0))
          {
            sndPlaySfx((struct ALBankAlt_s *) g_musicSfxBufferPtr, 253, &autogun->unkC8);
            chrobjSndCreatePostEventDefault(autogun->unkC8, &arg0->pos);
          }
          autogun->unkC0 = (s32) (g_GlobalTimer + 2);
        }
        if ((sp13C != 0) || (sp138 != 0))
        {
          sp11C = 1;
          sp10C = (void *) 0;
          sp108 = arg0->stan;
          sp104 = (autogun->unkAC & 3) == 0;
          sp100 = get_curplayer_positiondata();
          var_a0_6 = 5;
          temp_v1_10 = model->obj->Switches;
          if ((model->obj->Switches[7] != 0) && (!(autogun->unkAC & 7)))
          {
            var_a0_6 = 7;
          }
          if ((arg0->flags & 2) && (temp_v1_10[var_a0_6] != ((void *) 0)))
          {
            temp_s2_7 = modelFindNodeMtx(model, temp_v1_10[var_a0_6], 0);
            temp_v1_11 = model->obj->Switches[var_a0_6]->Data;
            sp12C.f[0] = temp_v1_11->f[0];
            sp12C.f[1] = temp_v1_11->f[1];
            sp12C.f[2] = temp_v1_11->f[2];
            matrix_4x4_multiply_homogeneous(currentPlayerGetMatrix10D4(), temp_s2_7, &spB8);
            mtx4TransformVecInPlace(&spB8, &sp12C);
            if (walkTilesBetweenPoints_NoCallback(&sp108, arg0->pos.f[0], arg0->pos.f[2], sp12C.f[0], sp12C.f[2]) == 0)
            {
              sp12C.f[0] = arg0->pos.f[0];
              sp12C.f[1] = arg0->pos.f[1];
              sp12C.f[2] = arg0->pos.f[2];
            }
          }
          else
          {
            sp12C.f[0] = arg0->pos.f[0];
            sp12C.f[1] = arg0->pos.f[1];
            sp12C.f[2] = arg0->pos.f[2];
          }
          sp120.f[0] = cosf(autogun->unk9C) * sinf(autogun->unk90);
          sp120.f[1] = sinf(autogun->unk9C);
          sp120.f[2] = cosf(autogun->unk9C) * cosf(autogun->unk90);
          sp110.f[0] = sp12C.f[0] + (sp120.f[0] * 65536.0f);
          sp110.f[1] = sp12C.f[1] + (sp120.f[1] * 65536.0f);
          sp110.f[2] = sp12C.f[2] + (sp120.f[2] * 65536.0f);
          sub_GAME_7F0B1CC4();
          if (stanTestLineUnobstructed(&sp108, sp12C.f[0], sp12C.f[2], sp110.f[0], sp110.f[2], 2, 100.0f, 100.0f, 0.0f, 1.0f) == 0)
          {
            chrlvStanLineDirIntersection(&sp12C, &sp120, &sp110);
            sp10C = sp108;
            sp110.f[0] -= 26.0f * sp120.f[0];
            sp110.f[1] -= 26.0f * sp120.f[1];
            sp110.f[2] -= 26.0f * sp120.f[2];
          }
          if (g_GlobalTimer == ((s32) autogun->unkBC))
          {
            f32 beam_xdiff;
            f32 beam_ydiff;
            f32 beam_zdiff;
            beam_xdiff = sp100->pos.f[0] - sp12C.f[0];
            beam_ydiff = sp100->pos.f[1] - sp12C.f[1];
            beam_zdiff = sp100->pos.f[2] - sp12C.f[2];
            temp_f20_4 = ((beam_xdiff * beam_xdiff) + (beam_ydiff * beam_ydiff)) + (beam_zdiff * beam_zdiff);
            beam_xdiff = sp110.f[0] - sp12C.f[0];
            beam_ydiff = sp110.f[1] - sp12C.f[1];
            beam_zdiff = sp110.f[2] - sp12C.f[2];
            if ((temp_f20_4 <= (((beam_xdiff * beam_xdiff) + (beam_ydiff * beam_ydiff)) + (beam_zdiff * beam_zdiff))) && (bondviewGetIfCurrentPlayerDamageShowTime() == 0))
            {
              temp_f0_35 = sqrtf(temp_f20_4);
              var_f2_7 = (0.16f * AUTOGUN_DAMAGE_TIMER_DELTA) * g_AutogunPendingDamageTick;
              if (temp_f0_35 > 200.0f)
              {
                var_f2_7 *= 200.0f / temp_f0_35;
              }
              autogun->unkD4 += var_f2_7;
              if (autogun->unkD4 >= 1.0f)
              {
                bondviewCallRecordDamageKills((bondwalkItemGetDestructionAmount(14) * 0.125f) * g_AutogunDamageScalar, autogun->unk90, -1, 1);
                autogun->unkD4 = 0.0f;
                if (bondviewGetIfCurrentPlayerDamageShowTime() != 0)
                {
                  sp11C = 0;
                }
              }
            }
          }
          if (sp11C != 0)
          {
            if (sp10C != ((void *) 0))
            {
              sub_GAME_7F0A3E1C(&sp110, 1, 26.0f, (s16) sp10C->room);
            }
            recall_joy2_hits_edit_flag(14, &sp110, -1);
          }
          else
          {
            sp110.f[0] = sp100->pos.f[0];
            sp110.f[1] = sp100->pos.f[1];
            sp110.f[2] = sp100->pos.f[2];
            recall_joy2_hits_edit_detail_edit_flag(14, sp100, -1);
          }
          if (sp104 != 0)
          {
            struct beam *beam_local;
            f32 dist_local;
            u32 beam_pad[2];
            beam_local = autogun->beam;
            beam_local->from.f[0] = sp12C.f[0];
            beam_local->from.f[1] = sp12C.f[1];
            beam_local->from.f[2] = sp12C.f[2];
            beam_local->dir.f[0] = sp110.f[0] - beam_local->from.f[0];
            beam_local->dir.f[1] = sp110.f[1] - beam_local->from.f[1];
            beam_local->dir.f[2] = sp110.f[2] - beam_local->from.f[2];
            dist_local = sqrtf(((beam_local->dir.f[0] * beam_local->dir.f[0]) + (beam_local->dir.f[1] * beam_local->dir.f[1])) + (beam_local->dir.f[2] * beam_local->dir.f[2]));
            temp_f2_23 = 1.0f / dist_local;
            beam_local->dir.f[0] = (f32) (beam_local->dir.f[0] * temp_f2_23);
            beam_local->dir.f[1] = (f32) (beam_local->dir.f[1] * temp_f2_23);
            beam_local->dir.f[2] = (f32) (beam_local->dir.f[2] * temp_f2_23);
            if (dist_local > 10000.0f)
            {
              dist_local = 10000.0f;
            }
            beam_local->age = 0;
            beam_local->weaponnum = 14;
            beam_local->maxdist = dist_local;
            if (dist_local < 500.0f)
            {
              dist_local = 500.0f;
            }
            if (beam_local->weaponnum == 22)
            {
              beam_local->speed = 0.25f * dist_local;
              beam_local->mindist = 0.6f * dist_local;
              if (beam_local->mindist > 3000.0f)
              {
                beam_local->mindist = 3000.0f;
              }
              beam_local->dist = ((-0.1f) - ((((f32) ((u32) randomGetNext())) * (1.0f / 4294967295)) * 0.3f)) * dist_local;
            }
            else
            {
              beam_local->speed = 0.2f * dist_local;
              beam_local->mindist = 0.2f * dist_local;
              if (beam_local->mindist > 3000.0f)
              {
                beam_local->mindist = 3000.0f;
              }
              beam_local->dist = ((2.0f * (((f32) ((u32) randomGetNext())) * (1.0f / 4294967295))) - 1.0f) * beam_local->speed;
            }
          }
        }
      }
      if (model->obj->Switches[5] != ((void *) 0))
      {
        modelGetNodeRwData(model, model->obj->Switches[5])->Gunfire.visible = (s16) sp13C;
      }
      if (model->obj->Switches[7] != ((void *) 0))
      {
        modelGetNodeRwData(model, model->obj->Switches[7])->Gunfire.visible = (s16) sp138;
      }
    }
    objDropRecursively(arg0);
  }
  if (sp680 == 5)
  {
    arg0->stan = (void *) 0;
  }
  return sp680;
}

#undef PROJECTILE_GRAVITY_MODIFIER
#undef AUTOGUN_P1_ACCEL
#undef AUTOGUN_P1_MAX_SPEED
#undef AUTOGUN_P2_ACCEL
#undef AUTOGUN_P2_MAX_SPEED
#undef AUTOGUN_TRACK_ACCEL
#undef VEHICLE_TURN_ACCEL
#undef VEHICLE_TURN_DECEL
#undef VEHICLE_TURN_MAX_SPEED
#undef AUTOGUN_DAMAGE_TIMER_DELTA


#ifdef NONMATCHING
Gfx * sub_GAME_7F049B58(Gfx *arg0) {

}
#else
GLOBAL_ASM(
.text
glabel sub_GAME_7F049B58
/* 07E688 7F049B58 27BDFFC8 */  addiu $sp, $sp, -0x38
/* 07E68C 7F049B5C AFBF0034 */  sw    $ra, 0x34($sp)
/* 07E690 7F049B60 AFB20020 */  sw    $s2, 0x20($sp)
/* 07E694 7F049B64 00809025 */  move  $s2, $a0
/* 07E698 7F049B68 AFB60030 */  sw    $s6, 0x30($sp)
/* 07E69C 7F049B6C AFB5002C */  sw    $s5, 0x2c($sp)
/* 07E6A0 7F049B70 AFB40028 */  sw    $s4, 0x28($sp)
/* 07E6A4 7F049B74 AFB30024 */  sw    $s3, 0x24($sp)
/* 07E6A8 7F049B78 AFB1001C */  sw    $s1, 0x1c($sp)
/* 07E6AC 7F049B7C 0FC0E909 */  jal   get_ptr_obj_pos_list_current_entry
/* 07E6B0 7F049B80 AFB00018 */   sw    $s0, 0x18($sp)
/* 07E6B4 7F049B84 10400036 */  beqz  $v0, .L7F049C60
/* 07E6B8 7F049B88 00408025 */   move  $s0, $v0
/* 07E6BC 7F049B8C 24160006 */  li    $s6, 6
/* 07E6C0 7F049B90 2415000D */  li    $s5, 13
/* 07E6C4 7F049B94 24140001 */  li    $s4, 1
/* 07E6C8 7F049B98 24130003 */  li    $s3, 3
/* 07E6CC 7F049B9C 92020000 */  lbu   $v0, ($s0)
.L7F049BA0:
/* 07E6D0 7F049BA0 02402025 */  move  $a0, $s2
/* 07E6D4 7F049BA4 1662000B */  bne   $s3, $v0, .L7F049BD4
/* 07E6D8 7F049BA8 00000000 */   nop
/* 07E6DC 7F049BAC 8E110004 */  lw    $s1, 4($s0)
/* 07E6E0 7F049BB0 02803025 */  move  $a2, $s4
/* 07E6E4 7F049BB4 0FC18786 */  jal   sub_GAME_7F061E18
/* 07E6E8 7F049BB8 26250180 */   addiu $a1, $s1, 0x180
/* 07E6EC 7F049BBC 00402025 */  move  $a0, $v0
/* 07E6F0 7F049BC0 262501AC */  addiu $a1, $s1, 0x1ac
/* 07E6F4 7F049BC4 0FC18786 */  jal   sub_GAME_7F061E18
/* 07E6F8 7F049BC8 02803025 */   move  $a2, $s4
/* 07E6FC 7F049BCC 10000021 */  b     .L7F049C54
/* 07E700 7F049BD0 00409025 */   move  $s2, $v0
.L7F049BD4:
/* 07E704 7F049BD4 1682000B */  bne   $s4, $v0, .L7F049C04
/* 07E708 7F049BD8 00000000 */   nop
/* 07E70C 7F049BDC 8E030004 */  lw    $v1, 4($s0)
/* 07E710 7F049BE0 02402025 */  move  $a0, $s2
/* 07E714 7F049BE4 02803025 */  move  $a2, $s4
/* 07E718 7F049BE8 906E0003 */  lbu   $t6, 3($v1)
/* 07E71C 7F049BEC 56AE001A */  bnel  $s5, $t6, .L7F049C58
/* 07E720 7F049BF0 8E100024 */   lw    $s0, 0x24($s0)
/* 07E724 7F049BF4 0FC18786 */  jal   sub_GAME_7F061E18
/* 07E728 7F049BF8 8C6500CC */   lw    $a1, 0xcc($v1)
/* 07E72C 7F049BFC 10000015 */  b     .L7F049C54
/* 07E730 7F049C00 00409025 */   move  $s2, $v0
.L7F049C04:
/* 07E734 7F049C04 56C20014 */  bnel  $s6, $v0, .L7F049C58
/* 07E738 7F049C08 8E100024 */   lw    $s0, 0x24($s0)
/* 07E73C 7F049C0C 8E0F0004 */  lw    $t7, 4($s0)
/* 07E740 7F049C10 51E00011 */  beql  $t7, $zero, .L7F049C58
/* 07E744 7F049C14 8E100024 */   lw    $s0, 0x24($s0)
/* 07E748 7F049C18 0FC26C57 */  jal   getPlayerPointerIndex
/* 07E74C 7F049C1C 02002025 */   move  $a0, $s0
/* 07E750 7F049C20 0FC26C54 */  jal   get_cur_playernum
/* 07E754 7F049C24 00408825 */   move  $s1, $v0
/* 07E758 7F049C28 1051000A */  beq   $v0, $s1, .L7F049C54
/* 07E75C 7F049C2C 02402025 */   move  $a0, $s2
/* 07E760 7F049C30 8E110004 */  lw    $s1, 4($s0)
/* 07E764 7F049C34 02803025 */  move  $a2, $s4
/* 07E768 7F049C38 0FC18786 */  jal   sub_GAME_7F061E18
/* 07E76C 7F049C3C 26250180 */   addiu $a1, $s1, 0x180
/* 07E770 7F049C40 00402025 */  move  $a0, $v0
/* 07E774 7F049C44 262501AC */  addiu $a1, $s1, 0x1ac
/* 07E778 7F049C48 0FC18786 */  jal   sub_GAME_7F061E18
/* 07E77C 7F049C4C 02803025 */   move  $a2, $s4
/* 07E780 7F049C50 00409025 */  move  $s2, $v0
.L7F049C54:
/* 07E784 7F049C54 8E100024 */  lw    $s0, 0x24($s0)
.L7F049C58:
/* 07E788 7F049C58 5600FFD1 */  bnezl $s0, .L7F049BA0
/* 07E78C 7F049C5C 92020000 */   lbu   $v0, ($s0)
.L7F049C60:
/* 07E790 7F049C60 8FBF0034 */  lw    $ra, 0x34($sp)
/* 07E794 7F049C64 02401025 */  move  $v0, $s2
/* 07E798 7F049C68 8FB20020 */  lw    $s2, 0x20($sp)
/* 07E79C 7F049C6C 8FB00018 */  lw    $s0, 0x18($sp)
/* 07E7A0 7F049C70 8FB1001C */  lw    $s1, 0x1c($sp)
/* 07E7A4 7F049C74 8FB30024 */  lw    $s3, 0x24($sp)
/* 07E7A8 7F049C78 8FB40028 */  lw    $s4, 0x28($sp)
/* 07E7AC 7F049C7C 8FB5002C */  lw    $s5, 0x2c($sp)
/* 07E7B0 7F049C80 8FB60030 */  lw    $s6, 0x30($sp)
/* 07E7B4 7F049C84 03E00008 */  jr    $ra
/* 07E7B8 7F049C88 27BD0038 */   addiu $sp, $sp, 0x38
)
#endif





void save_ptr_monitor_ani_code_to_obj_ani_slot(MonitorRecord *mon, void *image)
{
    mon->cmdlist  = image;
    mon->offset = 0;
}


void monitorSetImageByNum(MonitorRecord *mon, s32 monAnimID)
{
    s32 *image = &monAnim00Bond;
    switch (monAnimID)
    {
         default:
         case 0:
            break;
         case 1:
            image = &monAnim01DesktopsSatellite;
            break;
        case 2:
            image = &monAnim02Astrological;
            break;
        case 3:
            image = &monAnim03ThreeWavePattern;
            break;
        case 4:
            image = &monAnim04WavePattern;
            break;
        case 5:
            image = &monAnim05GreenTextUp;
            break;
        case 6:
            image = &monAnim06RedTextDown;
            break;
        case 7:
            image = &monAnim07GreenTextDown;
            break;
        case 8:
            image = &monAnim08RedBarGraph;
            break;
        case 9:
            image = &monAnim09BlueBarGraph;
            break;
        case 10:
            image = &monAnim0AGreenBarGraph;
            break;
        case 11:
            image = &monAnim0BRadar;
            break;
        case 12:
            image = &monAnim0CSpinningCube;
            break;
        case 13:
            image = &monAnim0DLocWeapArmed;
            break;
        case 14:
            image = &monAnim0ERedTarget;
            break;
        case 15:
            image = &monAnim0FSatelliteTargeting;
            break;
        case 16:
            image = &monAnim10GlobalMap;
            break;
        case 17:
            image = &monAnim11KarlYelling;
            break;
        case 18:
            image = &monAnim12Skateboard;
            break;
        case 19:
            image = &monAnim13PoliceGuy;
            break;
        case 20:
            image = &monAnim14Off;
            break;
        case 21:
            image = &monAnim15RandomSeven;
            break;
        case 22:
            image = &monAnim16RandomFour;
            break;
        case 23:
            image = &monAnim17RandImageEffect;
            break;
        case 24:
            image = &monRandEffectChanceSHUTTLE1;
            break;
        case 25:
            image = &monRandEffectChanceSHUTTLE2;
            break;
        case 26:
            image = &monRandEffectChanceEARTHFULL1;
            break;
        case 27:
            image = &monRandEffectChanceEARTHFULL2;
            break;
        case 28:
            image = &monRandEffectChanceBLUESTARS;
            break;
        case 29:
            image = &monRandEffectChanceGALAXY1;
            break;
        case 30:
            image = &monRandEffectChanceGALAXY2;
            break;
        case 31:
            image = &monRandEffectChanceEARTHTEXT;
            break;
        case 32:
            image = &monRandEffectChanceTARGETEARTH;
            break;
        case 33:
            image = &monRandEffectChanceGALAXY3;
            break;
        case 34:
            image = &monRandChanceScrollOrZoomRandRGBN;
            break;
        case 35:
            image = &monRandChanceScrollOrZoomRed;
            break;
        case 36:
            image = &monRandChanceScrollOrZoomGreen;
            break;
        case 37:
            image = &monRandChanceScrollOrZoomBlue;
            break;
        case 38:
            image = &monRandChanceScrollOrZoom;
            break;
        case 39:
            image = &monAnim27RandomEffectScrollRight;
            break;
        case 40:
            image = &monAnim28RandomEffectScrollUpFast;
            break;
        case 41:
            image = &monAnim29RandomEffectScrollUp;
            break;
        case 42:
            image = &monAnim2ARandEffectScrollZoom1;
            break;
        case 43:
            image = &monAnim2ARandEffectScrollZoom2;
            break;
        case 44:
            image = &monAnim2CRandEffectWaitRoute;
            break;
        case 45:
            image = &monAnim2DRandEffectFlash;
            break;
        case 46:
            image = &monAnim2ERedBrightening;
            break;
        case 47:
            image = &monAnim2FGreenBrightening;
            break;
        case 48:
            image = &monAnim30GreySolid;
            break;
        case 49:
            image = &monAnim31RedSolid;
            break;
        case 50:
            image = &monAnim32GreenSolid;
            break;
        case 51:
            image = &monAnim33BlackSolid;
            break;
    }
    save_ptr_monitor_ani_code_to_obj_ani_slot(mon,  image);
}





void save_img_index_to_obj_ani_slot(MonitorRecord *mon, void *unk88)
{
    mon->tconfig = unk88;
}



#ifdef NONMATCHING

struct tvcmd {
    u32 type;
    s32 arg1;
    u32 arg2;
};

// Almost matching - only regalloc issues left (for VERSION_US)
// Called tvscreenRender in PD

Gfx *process_monitor_animation_microcode(Model *model, ModelNode *node, MonitorRecord *screen, Gfx *gdl, s32 arg4, s32 arg5)
{
    if (node && (node->Opcode & 0xff) == MODELNODE_OPCODE_DISPLAYLIST_COLLISIONRECORD) {
        Vertex *vertices = dynAllocate7F0BD6C4(4);
        Gfx *savedgdl = gdl++;
        union ModelRoData *rodata = node->Data;
        union ModelRwData *rwdata = modelGetNodeRwData(model, node);
        sImageTableEntry *tconfig;
        bool yielding = FALSE;

        while (!yielding) {
            struct tvcmd *cmd = (struct tvcmd *) &screen->cmdlist[screen->offset];

            switch (cmd->type) {
            case TVCMD_STOPSCROLL:
                screen->xmidinc = 0.0f;
                screen->ymidinc = 0.0f;
                screen->offset++;
                break;
            case TVCMD_SCROLLRELX:
                screen->xmidfrac = 0.0f;
                screen->xmidinc = 1.0f / cmd->arg2;
                screen->xmidold = screen->xmid;
                screen->xmidnew = screen->xmid + cmd->arg1 * (1.0f / 1024.0f);
                screen->offset += 3;
                break;
            case TVCMD_SCROLLRELY:
                screen->ymidfrac = 0.0f;
                screen->ymidinc = 1.0f / cmd->arg2;
                screen->ymidold = screen->ymid;
                screen->ymidnew = screen->ymid + cmd->arg1 * (1.0f / 1024.0f);
                screen->offset += 3;
                break;
            case TVCMD_SCROLLABSX:
                screen->xmidfrac = 0.0f;
                screen->xmidinc = 1.0f / cmd->arg2;
                screen->xmidold = screen->xmid;
                screen->xmidnew = cmd->arg1 * (1.0f / 1024.0f);
                screen->offset += 3;
                break;
            case TVCMD_SCROLLABSY:
                screen->ymidfrac = 0.0f;
                screen->ymidinc = 1.0f / cmd->arg2;
                screen->ymidold = screen->ymid;
                screen->ymidnew = cmd->arg1 * (1.0f / 1024.0f);
                screen->offset += 3;
                break;
            case TVCMD_SCALEABSX:
                screen->xscalefrac = 0.0f;
                screen->xscaleinc = 1.0f / cmd->arg2;
                screen->xscaleold = screen->xscale;
                screen->xscalenew = cmd->arg1 * (1.0f / 1024.0f);
                screen->offset += 3;
                break;
            case TVCMD_SCALEABSY:
                screen->yscalefrac = 0.0f;
                screen->yscaleinc = 1.0f / cmd->arg2;
                screen->yscaleold = screen->yscale;
                screen->yscalenew = cmd->arg1 * (1.0f / 1024.0f);
                screen->offset += 3;
                break;
            case TVCMD_SETTEXTURE:
                save_img_index_to_obj_ani_slot(screen, cmd->arg1);
                screen->offset += 2;
                break;
            case TVCMD_PAUSE:
                if (screen->pause60 >= 0) {
                    screen->pause60 -= g_ClockTimer;

                    if (screen->pause60 >= 0) {
                        yielding = TRUE;
                    } else {
                        screen->offset += 2;
                    }
                } else {
                    yielding = TRUE;
                    screen->pause60 = cmd->arg1;
                }
                break;
            case TVCMD_SETCMDLIST:
                save_ptr_monitor_ani_code_to_obj_ani_slot(screen, (u32 *) cmd->arg1);
                break;
            case TVCMD_RANDSETCMDLIST:
                if ((randomGetNext() >> 16) < cmd->arg2) {
                    save_ptr_monitor_ani_code_to_obj_ani_slot(screen, (u32 *) cmd->arg1);
                } else {
                    screen->offset += 3;
                }
                break;
            case TVCMD_RESTART:
                screen->offset = 0;
                break;
            case TVCMD_YIELD:
                yielding = TRUE;
                break;
            case TVCMD_SETCOLOUR:
                screen->colfrac = 0.0f;
                screen->colinc = 1.0f / cmd->arg2;

                screen->redold = screen->red;
                screen->rednew = ((u32)cmd->arg1 >> 24) & 0xff;

                screen->greenold = screen->green;
                screen->greennew = ((u32)cmd->arg1 >> 16) & 0xff;

                screen->blueold = screen->blue;
                screen->bluenew = ((u32)cmd->arg1 >> 8) & 0xff;

                screen->alphaold = screen->alpha;
                screen->alphanew = cmd->arg1 & 0xff;

                screen->offset += 3;
                break;
            case TVCMD_ROTATEABS:
                screen->rot = cmd->arg1 * M_TAU_F / M_U16_MAX_VALUE_F;
                screen->offset += 2;
                break;
            case TVCMD_ROTATEREL:
                screen->rot += g_GlobalTimerDelta * cmd->arg1 * M_TAU_F / M_U16_MAX_VALUE_F;

                if (screen->rot >= M_TAU_F) {
                    screen->rot -= M_TAU_F;
                }

                if (screen->rot < 0.0f) {
                    screen->rot += M_TAU_F;
                }

                screen->offset += 2;
                break;
            }
        }

        // Increment X scale
        if (screen->xscaleinc > 0.0f) {
            screen->xscalefrac += screen->xscaleinc * g_GlobalTimerDelta;

            if (screen->xscalefrac < 1.0f) {
                screen->xscale = screen->xscaleold + (screen->xscalenew - screen->xscaleold) * screen->xscalefrac;
            } else {
                screen->xscalefrac = 1.0f;
                screen->xscaleinc = 0.0f;
                screen->xscale = screen->xscalenew;
            }
        }

        // Increment Y scale
        if (screen->yscaleinc > 0.0f) {
            screen->yscalefrac += screen->yscaleinc * g_GlobalTimerDelta;

            if (screen->yscalefrac < 1.0f) {
                screen->yscale = screen->yscaleold + (screen->yscalenew - screen->yscaleold) * screen->yscalefrac;
            } else {
                screen->yscalefrac = 1.0f;
                screen->yscaleinc = 0.0f;
                screen->yscale = screen->yscalenew;
            }
        }

        // Increment X scroll
        if (screen->xmidinc > 0.0f) {
            screen->xmidfrac += screen->xmidinc * g_GlobalTimerDelta;

            if (screen->xmidfrac < 1.0f) {
                screen->xmid = screen->xmidold + (screen->xmidnew - screen->xmidold) * screen->xmidfrac;
            } else {
                screen->xmidfrac = 1.0f;
                screen->xmidinc = 0.0f;
                screen->xmid = screen->xmidnew;
            }
        }

        // Increment Y scroll
        if (screen->ymidinc > 0.0f) {
            screen->ymidfrac += screen->ymidinc * g_GlobalTimerDelta;

            if (screen->ymidfrac < 1.0f) {
                screen->ymid = screen->ymidold + (screen->ymidnew - screen->ymidold) * screen->ymidfrac;
            } else {
                screen->ymidfrac = 1.0f;
                screen->ymidinc = 0.0f;
                screen->ymid = screen->ymidnew;
            }
        }

        // Increment colour change
        if (screen->colinc > 0.0f) {
            screen->colfrac += screen->colinc * g_GlobalTimerDelta;

            if (screen->colfrac < 1.0f) {
                screen->red = screen->redold + (s32) ((screen->rednew - screen->redold) * screen->colfrac);
                screen->green = screen->greenold + (s32) ((screen->greennew - screen->greenold) * screen->colfrac);
                screen->blue = screen->blueold + (s32) ((screen->bluenew - screen->blueold) * screen->colfrac);
                screen->alpha = screen->alphaold + (s32) ((screen->alphanew - screen->alphaold) * screen->colfrac);
            } else {
                screen->colfrac = 1.0f;
                screen->colinc = 0.0f;
                screen->red = screen->rednew;
                screen->green = screen->greennew;
                screen->blue = screen->bluenew;
                screen->alpha = screen->alphanew;
            }
        }

        // Set up everything for rendering
        rwdata->DisplayListCollisions.gdl = gdl;
        rwdata->DisplayListCollisions.Vertices = vertices;

        vertices[0] = rodata->DisplayListCollisions.Vertices[0];
        vertices[1] = rodata->DisplayListCollisions.Vertices[1];
        vertices[2] = rodata->DisplayListCollisions.Vertices[2];
        vertices[3] = rodata->DisplayListCollisions.Vertices[3];

        if ((u32)screen->tconfig < 100) {
            tconfig = &monitorimages[(s32)screen->tconfig];
        } else {
            tconfig = screen->tconfig;
        }

        if (tconfig != NULL) {
            u32 stack[13];
            f32 f22; // sp54
            f32 f24; // sp50
            f32 f14;
            f32 f16;

            f22 = screen->xscale / 2.0f;
            f24 = screen->yscale / 2.0f;
            f14 = f22;
            f16 = f24;

            if (1);
            if (1);
            if (1);
            if (1);
            if (1);

            if (screen->rot != 0.0f) {
                f32 f20;
                f32 f2_6;

                f20 = cosf(screen->rot) * 1.4142f;
                f2_6 = sinf(screen->rot) * 1.4142f;

                f22 *= f20;
                f24 *= f2_6;
                f14 *= f2_6;
                f16 *= f20;
            }

            vertices[0].s = tconfig->width  * (screen->xmid + f22) * 32.0f;
            vertices[0].t = tconfig->height * (screen->ymid + f24) * 32.0f;

            vertices[1].s = tconfig->width  * (screen->xmid - f14) * 32.0f;
            vertices[1].t = tconfig->height * (screen->ymid + f16) * 32.0f;

            vertices[2].s = tconfig->width  * (screen->xmid - f22) * 32.0f;
            vertices[2].t = tconfig->height * (screen->ymid - f24) * 32.0f;

            vertices[3].s = tconfig->width  * (screen->xmid + f14) * 32.0f;
            vertices[3].t = tconfig->height * (screen->ymid - f16) * 32.0f;
        }

        if (1) {
            u8 tmpc;
            u8 tmpc2;
            tmpc = screen->red;
            vertices[3].r = tmpc;
            vertices[2].r = tmpc;
            vertices[1].r = tmpc;
            vertices[0].r = tmpc;

            tmpc = screen->green;
            vertices[3].g = tmpc;
            vertices[2].g = tmpc;
            vertices[1].g = tmpc;
            vertices[0].g = tmpc;

            tmpc2 = screen->blue;
            vertices[3].b = tmpc2;
            vertices[2].b = tmpc2;
            vertices[1].b = tmpc2;
            vertices[0].b = tmpc2;

            tmpc = screen->alpha;
            vertices[3].a = tmpc;
            vertices[2].a = tmpc;
            vertices[1].a = tmpc;
            vertices[0].a = tmpc;
        }

        if (screen->alpha < 255) {
            arg5 = 2;
        }

        // Render the image
        gSPSetGeometryMode(gdl++, G_CULL_BACK);

        texSelect(&gdl, tconfig, arg5, arg4, 2);

        gSPMatrix(gdl++, osVirtualToPhysical(model->render_pos), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(gdl++, SPSEGMENT_MODEL_VTX, osVirtualToPhysical(vertices));
        gSPVertex(gdl++, 0x04000000, 4, 0);
        gDPTri2(gdl++, 0, 1, 2, 0, 2, 3);
        gSPEndDisplayList(gdl++);

        gSPBranchList(savedgdl++, gdl);
    }

    return gdl;
}

// keeping old decompilation around for comparison's sake until a match is found:
#if 0
void *process_monitor_animation_microcode(Model *arg0, ModelNode *arg1, MonitorRecord *arg2, void *arg3, s32 arg4, s32 arg5)
{
    void           *spA8;
    void           *spA4;
    modeldata_root *spA0;
    void           *sp94;
    f32             sp54;
    f32             sp50;
    Mtxf           *temp_s2;
    f32             temp_f0;
    f32             temp_f0_10;
    f32             temp_f0_11;
    f32             temp_f0_2;
    f32             temp_f0_3;
    f32             temp_f0_4;
    f32             temp_f0_5;
    f32             temp_f0_6;
    f32             temp_f0_7;
    f32             temp_f0_8;
    f32             temp_f0_9;
    f32             temp_f10;
    f32             temp_f10_2;
    f32             temp_f12;
    f32             temp_f12_2;
    f32             temp_f18;
    f32             temp_f18_2;
    f32             temp_f18_3;
    f32             temp_f18_4;
    f32             temp_f20;
    f32             temp_f22;
    f32             temp_f24;
    f32             temp_f2;
    f32             temp_f2_2;
    f32             temp_f2_3;
    f32             temp_f2_4;
    f32             temp_f2_5;
    f32             temp_f2_6;
    f32             temp_f4;
    f32             temp_f4_2;
    f32             temp_f6;
    f32             temp_f6_2;
    f32             temp_f6_3;
    f32             temp_f6_4;
    f32             temp_f6_5;
    f32             temp_f6_6;
    f32             temp_f8;
    f32             temp_v0_4;
    s16             temp_v0_2;
    s32             temp_t0;
    s32             temp_t1;
    s32             temp_t4;
    s32             temp_t5;
    s32             temp_t5_2;
    s32             temp_t7;
    s32             temp_t8_2;
    u16             temp_v0;
    u32             temp_t3;
    u8              temp_a0;
    u8              temp_a1;
    u8              temp_t0_2;
    u8              temp_t2_2;
    u8              temp_t4_2;
    u8              temp_t5_3;
    u8              temp_t6_2;
    u8              temp_t7_2;
    u8              temp_t8_3;
    u8              temp_t9_2;
    u8              temp_v0_3;
    u8              temp_v0_5;
    u8              temp_v0_6;
    u8              temp_v0_7;
    u8              temp_v1_2;
    u8              temp_v1_3;
    void           *temp_s0;
    void           *temp_s1;
    void           *temp_t0_3;
    void           *temp_t2;
    void           *temp_t2_3;
    void           *temp_t2_4;
    void           *temp_t3_2;
    void           *temp_t5_4;
    void           *temp_t6;
    void           *temp_t8;
    void           *temp_t9;
    void           *temp_v1;
    void          **temp_a0_2;
    f32             phi_f6;
    f32             phi_f18;
    f32             phi_f6_2;
    f32             phi_f18_2;
    f32             phi_f6_3;
    f32             phi_f18_3;
    f32             phi_f6_4;
    f32             phi_f2;
    s32             phi_s1;
    void           *phi_s1_2;
    f32             phi_f6_5;
    f32             phi_f22;
    f32             phi_f4;
    f32             phi_f24;
    f32             phi_f10;
    f32             phi_f14;
    f32             phi_f18_4;
    f32             phi_f16;
    f32             phi_f8;
    f32             phi_f6_6;
    f32             phi_f4_2;
    f32             phi_f10_2;

    if ((arg1 != 0) && ((arg1->Opcode & 0xFF) == MODELNODE_OPCODE_DLCOLLISION))
    {
        temp_t8 = arg3;
        temp_s2 = dynAllocate7F0BD6C4(4);
        arg3    = temp_t8 + 8;
        spA8    = temp_t8;
        spA4    = arg1->Data;
        spA0    = modelGetNodeRwData(arg0, arg1);
        phi_f2  = M_U16_MAX_VALUE_F;
        phi_f2  = M_U16_MAX_VALUE_F;
        phi_s1  = 0;
        do
        {
            temp_v0 = arg2->offset;
            temp_v1 = arg2->image + (temp_v0 * 4);
            temp_t3 = temp_v1->unk0;
            switch (temp_t3)
            {
                case 0:
                    arg2->xmidinc = 0.0f;
                    arg2->ymidinc = 0.0f;
                    arg2->offset  = temp_v0 + 1;
                    break;
                case 1:
                    arg2->xmidfrac = 0.0f;
                    temp_t5        = temp_v1->unk8;
                    temp_f6        = temp_t5;
                    phi_f6         = temp_f6;
                    if (temp_t5 < 0)
                    {
                        phi_f6 = temp_f6 + 4294967296.0f;
                    }
                    temp_f0       = arg2->xmid;
                    arg2->xmidold = temp_f0;
                    arg2->xmidinc = 1.0f / phi_f6;
                    arg2->offset  = temp_v0 + 3;
                    arg2->xmidnew = temp_f0 + (temp_v1->unk4 * 0.0009765625f);
                    break;
                case 2:
                    arg2->ymidfrac = 0.0f;
                    temp_t8_2      = temp_v1->unk8;
                    temp_f18       = temp_t8_2;
                    phi_f18        = temp_f18;
                    if (temp_t8_2 < 0)
                    {
                        phi_f18 = temp_f18 + 4294967296.0f;
                    }
                    temp_f0_2     = arg2->ymid;
                    arg2->ymidold = temp_f0_2;
                    arg2->ymidinc = 1.0f / phi_f18;
                    arg2->offset  = temp_v0 + 3;
                    arg2->ymidnew = temp_f0_2 + (temp_v1->unk4 * 0.0009765625f);
                    break;
                case 3:
                    arg2->xmidfrac = 0.0f;
                    temp_t1        = temp_v1->unk8;
                    temp_f6_2      = temp_t1;
                    phi_f6_2       = temp_f6_2;
                    if (temp_t1 < 0)
                    {
                        phi_f6_2 = temp_f6_2 + 4294967296.0f;
                    }
                    arg2->xmidold = arg2->xmid;
                    arg2->xmidinc = 1.0f / phi_f6_2;
                    arg2->offset  = temp_v0 + 3;
                    arg2->xmidnew = temp_v1->unk4 * 0.0009765625f;
                    break;
                case 4:
                    arg2->ymidfrac = 0.0f;
                    temp_t4        = temp_v1->unk8;
                    temp_f18_2     = temp_t4;
                    phi_f18_2      = temp_f18_2;
                    if (temp_t4 < 0)
                    {
                        phi_f18_2 = temp_f18_2 + 4294967296.0f;
                    }
                    arg2->ymidold = arg2->ymid;
                    arg2->ymidinc = 1.0f / phi_f18_2;
                    arg2->offset  = temp_v0 + 3;
                    arg2->ymidnew = temp_v1->unk4 * 0.0009765625f;
                    break;
                case 5:
                    arg2->xscalefrac = 0.0f;
                    temp_t7          = temp_v1->unk8;
                    temp_f6_3        = temp_t7;
                    phi_f6_3         = temp_f6_3;
                    if (temp_t7 < 0)
                    {
                        phi_f6_3 = temp_f6_3 + 4294967296.0f;
                    }
                    arg2->xscaleold = arg2->xscale;
                    arg2->xscaleinc = 1.0f / phi_f6_3;
                    arg2->offset    = temp_v0 + 3;
                    arg2->xscalenew = temp_v1->unk4 * 0.0009765625f;
                    break;
                case 6:
                    arg2->yscalefrac = 0.0f;
                    temp_t0          = temp_v1->unk8;
                    temp_f18_3       = temp_t0;
                    phi_f18_3        = temp_f18_3;
                    if (temp_t0 < 0)
                    {
                        phi_f18_3 = temp_f18_3 + 4294967296.0f;
                    }
                    arg2->yscaleold = arg2->yscale;
                    arg2->yscaleinc = 1.0f / phi_f18_3;
                    arg2->offset    = temp_v0 + 3;
                    arg2->yscalenew = temp_v1->unk4 * 0.0009765625f;
                    break;
                case 7:
                    save_img_index_to_obj_ani_slot(arg2, temp_v1->unk4);
                    arg2->offset += 2;
                    break;
                case 8:
                    temp_v0_2 = arg2->pause60;
                    if (temp_v0_2 >= 0)
                    {
                        arg2->pause60 = temp_v0_2 - clock_timer;
                        if (arg2->pause60 >= 0)
                        {
                            phi_s1 = 1;
                        }
                        else
                        {
                            arg2->offset += 2;
                        }
                    }
                    else
                    {
                        arg2->pause60 = temp_v1->unk4;
                        phi_s1        = 1;
                    }
                    break;
                case 9:
                    save_ptr_monitor_ani_code_to_obj_ani_slot(arg2, temp_v1->unk4);
                    break;
                case 10:
                    sp94 = temp_v1;
                    if ((randomGetNext() >> 0x10) < temp_v1->unk8)
                    {
                        save_ptr_monitor_ani_code_to_obj_ani_slot(arg2, temp_v1->unk4);
                    }
                    else
                    {
                        arg2->offset += 3;
                    }
                    break;
                case 11:
                    arg2->offset = 0;
                    break;
                case 12:
                    phi_s1 = 1;
                    break;
                case 13:
                    arg2->colfrac = 0.0f;
                    temp_t5_2     = temp_v1->unk8;
                    temp_f6_4     = temp_t5_2;
                    phi_f6_4      = temp_f6_4;
                    if (temp_t5_2 < 0)
                    {
                        phi_f6_4 = temp_f6_4 + 4294967296.0f;
                    }
                    arg2->redold   = arg2->red;
                    arg2->colinc   = 1.0f / phi_f6_4;
                    arg2->greenold = arg2->green;
                    arg2->rednew   = temp_v1->unk4 >> 0x18;
                    arg2->blueold  = arg2->blue;
                    arg2->greennew = temp_v1->unk4 >> 0x10;
                    arg2->alphaold = arg2->alpha;
                    arg2->bluenew  = temp_v1->unk4 >> 8;
                    arg2->offset   = temp_v0 + 3;
                    arg2->alphanew = temp_v1->unk4;
                    break;
                case 14:
                    arg2->offset = temp_v0 + 2;
                    arg2->rot    = (temp_v1->unk4 * M_TAU_F) / phi_f2;
                    break;
                case 15:
                    arg2->rot += (g_GlobalTimerDelta * temp_v1->unk4 * M_TAU_F) / phi_f2;
                    temp_f12 = arg2->rot;
                    if (temp_f12 >= M_TAU_F)
                    {
                        arg2->rot = temp_f12 - M_TAU_F;
                    }
                    if (arg2->rot < 0.0f)
                    {
                        arg2->rot += M_TAU_F;
                    }
                    arg2->offset += 2;
                    break;
            }
        } while (phi_s1 == 0);
        temp_f0_3 = arg2->xscaleinc;
        if (temp_f0_3 > 0.0f)
        {
            arg2->xscalefrac += temp_f0_3 * g_GlobalTimerDelta;
            temp_f2 = arg2->xscalefrac;
            if (temp_f2 < 1.0f)
            {
                temp_f0_4    = arg2->xscaleold;
                arg2->xscale = temp_f0_4 + ((arg2->xscalenew - temp_f0_4) * temp_f2);
            }
            else
            {
                arg2->xscalefrac = 1.0f;
                arg2->xscaleinc  = 0.0f;
                arg2->xscale     = arg2->xscalenew;
            }
        }
        temp_f0_5 = arg2->yscaleinc;
        if (temp_f0_5 > 0.0f)
        {
            arg2->yscalefrac += temp_f0_5 * g_GlobalTimerDelta;
            temp_f2_2 = arg2->yscalefrac;
            if (temp_f2_2 < 1.0f)
            {
                temp_f0_6    = arg2->yscaleold;
                arg2->yscale = temp_f0_6 + ((arg2->yscalenew - temp_f0_6) * temp_f2_2);
            }
            else
            {
                arg2->yscalefrac = 1.0f;
                arg2->yscaleinc  = 0.0f;
                arg2->yscale     = arg2->yscalenew;
            }
        }
        temp_f0_7 = arg2->xmidinc;
        if (temp_f0_7 > 0.0f)
        {
            arg2->xmidfrac += temp_f0_7 * g_GlobalTimerDelta;
            temp_f2_3 = arg2->xmidfrac;
            if (temp_f2_3 < 1.0f)
            {
                temp_f0_8  = arg2->xmidold;
                arg2->xmid = temp_f0_8 + ((arg2->xmidnew - temp_f0_8) * temp_f2_3);
            }
            else
            {
                arg2->xmidfrac = 1.0f;
                arg2->xmidinc  = 0.0f;
                arg2->xmid     = arg2->xmidnew;
            }
        }
        temp_f0_9 = arg2->ymidinc;
        if (temp_f0_9 > 0.0f)
        {
            arg2->ymidfrac += temp_f0_9 * g_GlobalTimerDelta;
            temp_f2_4 = arg2->ymidfrac;
            if (temp_f2_4 < 1.0f)
            {
                temp_f0_10 = arg2->ymidold;
                arg2->ymid = temp_f0_10 + ((arg2->ymidnew - temp_f0_10) * temp_f2_4);
            }
            else
            {
                arg2->ymidfrac = 1.0f;
                arg2->ymidinc  = 0.0f;
                arg2->ymid     = arg2->ymidnew;
            }
        }
        temp_f2_5 = arg2->colinc;
        if (temp_f2_5 > 0.0f)
        {
            arg2->colfrac += temp_f2_5 * g_GlobalTimerDelta;
            temp_f0_11 = arg2->colfrac;
            if (temp_f0_11 < 1.0f)
            {
                temp_v0_3   = arg2->redold;
                temp_v1_2   = arg2->greenold;
                temp_a0     = arg2->blueold;
                temp_a1     = arg2->alphaold;
                arg2->red   = temp_v0_3 + ((arg2->rednew - temp_v0_3) * temp_f0_11);
                arg2->green = temp_v1_2 + ((arg2->greennew - temp_v1_2) * temp_f0_11);
                arg2->blue  = temp_a0 + ((arg2->bluenew - temp_a0) * temp_f0_11);
                arg2->alpha = temp_a1 + ((arg2->alphanew - temp_a1) * temp_f0_11);
            }
            else
            {
                arg2->colfrac = 1.0f;
                arg2->colinc  = 0.0f;
                arg2->red     = arg2->rednew;
                arg2->green   = arg2->greennew;
                arg2->blue    = arg2->bluenew;
                arg2->alpha   = arg2->alphanew;
            }
        }
        spA0->unk0       = temp_s2;
        spA0->ground     = (bitwise f32)arg3;
        temp_t3_2        = spA4->unk8;
        temp_s2->m[0][0] = temp_t3_2->unk0;
        temp_s2->m[0][1] = temp_t3_2->unk4;
        temp_s2->m[0][2] = temp_t3_2->unk8;
        temp_s2->m[0][3] = temp_t3_2->unkC;
        temp_t6          = spA4->unk8;
        temp_s2->m[1][0] = temp_t6->unk10;
        temp_s2->m[1][1] = temp_t6->unk14;
        temp_s2->m[1][2] = temp_t6->unk18;
        temp_s2->m[1][3] = temp_t6->unk1C;
        temp_t9          = spA4->unk8;
        temp_s2->m[2][0] = temp_t9->unk20;
        temp_s2->m[2][1] = temp_t9->unk24;
        temp_s2->m[2][2] = temp_t9->unk28;
        temp_s2->m[2][3] = temp_t9->unk2C;
        temp_t2          = spA4->unk8;
        temp_s2->m[3][0] = temp_t2->unk30;
        temp_s2->m[3][1] = temp_t2->unk34;
        temp_s2->m[3][2] = temp_t2->unk38;
        temp_s2->m[3][3] = temp_t2->unk3C;
        temp_v0_4        = arg2->unk88;
        phi_s1_2         = (bitwise void *)temp_v0_4;
        if ((bitwise u32)temp_v0_4 < 0x64)
        {
            phi_s1_2 = ((((bitwise s32)temp_v0_4 * 4) - (bitwise s32)temp_v0_4) * 4) + monitorimages;
        }
        if (phi_s1_2 != 0)
        {
            temp_f12_2 = arg2->rot;
            temp_f22   = arg2->xscale * 0.5f;
            temp_f24   = arg2->yscale * 0.5f;
            phi_f22    = temp_f22;
            phi_f24    = temp_f24;
            phi_f14    = temp_f22;
            phi_f16    = temp_f24;
            if (temp_f12_2 != 0.0f)
            {
                sp54      = temp_f22;
                sp50      = temp_f24;
                temp_f20  = cosf(temp_f12_2, temp_f22) * 1.4142f;
                temp_f2_6 = sinf(arg2->rot) * 1.4142f;
                phi_f22   = temp_f22 * temp_f20;
                phi_f24   = temp_f24 * temp_f2_6;
                phi_f14   = temp_f22 * temp_f2_6;
                phi_f16   = temp_f24 * temp_f20;
            }
            temp_t6_2 = phi_s1_2->unk4;
            temp_f6_5 = temp_t6_2;
            phi_f6_5  = temp_f6_5;
            if (temp_t6_2 < 0)
            {
                phi_f6_5 = temp_f6_5 + 4294967296.0f;
            }
            temp_s2->unk8 = phi_f6_5 * (arg2->xmid + phi_f22) * 32.0f;
            temp_t9_2     = phi_s1_2->unk5;
            temp_f4       = temp_t9_2;
            phi_f4        = temp_f4;
            if (temp_t9_2 < 0)
            {
                phi_f4 = temp_f4 + 4294967296.0f;
            }
            temp_s2->unkA = phi_f4 * (arg2->ymid + phi_f24) * 32.0f;
            temp_t2_2     = phi_s1_2->unk4;
            temp_f10      = temp_t2_2;
            phi_f10       = temp_f10;
            if (temp_t2_2 < 0)
            {
                phi_f10 = temp_f10 + 4294967296.0f;
            }
            temp_s2->unk18 = phi_f10 * (arg2->xmid - phi_f14) * 32.0f;
            temp_t7_2      = phi_s1_2->unk5;
            temp_f18_4     = temp_t7_2;
            phi_f18_4      = temp_f18_4;
            if (temp_t7_2 < 0)
            {
                phi_f18_4 = temp_f18_4 + 4294967296.0f;
            }
            temp_s2->unk1A = phi_f18_4 * (arg2->ymid + phi_f16) * 32.0f;
            temp_t0_2      = phi_s1_2->unk4;
            temp_f8        = temp_t0_2;
            phi_f8         = temp_f8;
            if (temp_t0_2 < 0)
            {
                phi_f8 = temp_f8 + 4294967296.0f;
            }
            temp_s2->unk28 = phi_f8 * (arg2->xmid - phi_f22) * 32.0f;
            temp_t4_2      = phi_s1_2->unk5;
            temp_f6_6      = temp_t4_2;
            phi_f6_6       = temp_f6_6;
            if (temp_t4_2 < 0)
            {
                phi_f6_6 = temp_f6_6 + 4294967296.0f;
            }
            temp_s2->unk2A = phi_f6_6 * (arg2->ymid - phi_f24) * 32.0f;
            temp_t5_3      = phi_s1_2->unk4;
            temp_f4_2      = temp_t5_3;
            phi_f4_2       = temp_f4_2;
            if (temp_t5_3 < 0)
            {
                phi_f4_2 = temp_f4_2 + 4294967296.0f;
            }
            temp_s2->unk38 = phi_f4_2 * (arg2->xmid + phi_f14) * 32.0f;
            temp_t8_3      = phi_s1_2->unk5;
            temp_f10_2     = temp_t8_3;
            phi_f10_2      = temp_f10_2;
            if (temp_t8_3 < 0)
            {
                phi_f10_2 = temp_f10_2 + 4294967296.0f;
            }
            temp_s2->unk3A = phi_f10_2 * (arg2->ymid - phi_f16) * 32.0f;
        }
        temp_v0_5      = arg2->red;
        temp_s2->unk3C = temp_v0_5;
        temp_s2->unk2C = temp_v0_5;
        temp_s2->unk1C = temp_v0_5;
        temp_s2->unkC  = temp_v0_5;
        temp_v0_6      = arg2->green;
        temp_a0_2      = &arg3;
        temp_s2->unk3D = temp_v0_6;
        temp_s2->unk2D = temp_v0_6;
        temp_s2->unk1D = temp_v0_6;
        temp_s2->unkD  = temp_v0_6;
        temp_v1_3      = arg2->blue;
        temp_s2->unk3E = temp_v1_3;
        temp_s2->unk2E = temp_v1_3;
        temp_s2->unk1E = temp_v1_3;
        temp_s2->unkE  = temp_v1_3;
        temp_v0_7      = arg2->alpha;
        temp_s2->unk3F = temp_v0_7;
        temp_s2->unk2F = temp_v0_7;
        temp_s2->unk1F = temp_v0_7;
        temp_s2->unkF  = temp_v0_7;
        if (arg2->alpha < 0xFF)
        {
            arg5 = 2;
        }
        temp_t2_3       = arg3;
        arg3            = temp_t2_3 + 8;
        temp_t2_3->unk4 = 0x2000;
        temp_t2_3->unk0 = 0xB7000000;
        texSelect(temp_a0_2, phi_s1_2, arg5, arg4, 2);
        temp_s0         = arg3;
        arg3            = temp_s0 + 8;
        temp_s0->unk0   = 0x1020040;
        temp_s0->unk4   = osVirtualToPhysical(arg0->unk0c);
        temp_s1         = arg3;
        arg3            = temp_s1 + 8;
        temp_s1->unk0   = 0xBC001006;
        temp_s1->unk4   = osVirtualToPhysical(temp_s2);
        temp_t5_4       = arg3;
        arg3            = temp_t5_4 + 8;
        temp_t5_4->unk4 = 0x4000000;
        temp_t5_4->unk0 = 0x4300040;
        temp_t0_3       = arg3;
        arg3            = temp_t0_3 + 8;
        temp_t0_3->unk4 = 0x2010;
        temp_t0_3->unk0 = 0xB1000032;
        temp_t2_4       = arg3;
        arg3            = temp_t2_4 + 8;
        temp_t2_4->unk4 = 0;
        temp_t2_4->unk0 = 0xB8000000;
        spA8->unk0      = 0x6010000;
        spA8->unk4      = arg3;
    }
    return arg3;
}
#endif

#else
#ifdef VERSION_US
GLOBAL_ASM(
.late_rodata
glabel D_80052C68
.word 0x40c90fdb /*6.2831855*/
/*D:80052C6C*/
glabel jpt_80052C6C
.word command00_reset_scroll_shift
.word command01_horizontal_scroll
.word command02_vertical_scroll
.word command03_horizontal_pos
.word command04_vertical_pos
.word command05_zoomx
.word command06_zoomy
.word command07_use_image_from_global_monitor_table
.word command08_halt_processing_for_time
.word command09_jump
.word command0A_jump_conditional
.word command0B_restart
.word command0C_break
.word command0D_colour_transition
.word command0E_set_rotation
.word command0F_rotate

glabel D_80052CAC
.word 0x3fc90fdb /*1.5707964*/
glabel D_80052CB0
.word 0x3fb50481 /*1.4141999*/
glabel D_80052CB4
.word 0x3f59999a /*0.85000002*/
glabel D_80052CB8
.word 0x3e19999a /*0.15000001*/
glabel D_80052CBC
.word 0x3e19999a /*0.15000001*/
glabel D_80052CC0
.word 0x3f666666 /*0.89999998*/
.text
glabel process_monitor_animation_microcode
/* 07EA78 7F049F48 27BDFF50 */  addiu $sp, $sp, -0xb0
/* 07EA7C 7F049F4C AFB10044 */  sw    $s1, 0x44($sp)
/* 07EA80 7F049F50 AFB00040 */  sw    $s0, 0x40($sp)
/* 07EA84 7F049F54 00C08025 */  move  $s0, $a2
/* 07EA88 7F049F58 00A08825 */  move  $s1, $a1
/* 07EA8C 7F049F5C AFBF004C */  sw    $ra, 0x4c($sp)
/* 07EA90 7F049F60 AFB20048 */  sw    $s2, 0x48($sp)
/* 07EA94 7F049F64 F7BA0038 */  sdc1  $f26, 0x38($sp)
/* 07EA98 7F049F68 F7B80030 */  sdc1  $f24, 0x30($sp)
/* 07EA9C 7F049F6C F7B60028 */  sdc1  $f22, 0x28($sp)
/* 07EAA0 7F049F70 F7B40020 */  sdc1  $f20, 0x20($sp)
/* 07EAA4 7F049F74 AFA400B0 */  sw    $a0, 0xb0($sp)
/* 07EAA8 7F049F78 10A0031E */  beqz  $a1, .L7F04ABF4
/* 07EAAC 7F049F7C AFA700BC */   sw    $a3, 0xbc($sp)
/* 07EAB0 7F049F80 94AE0000 */  lhu   $t6, ($a1)
/* 07EAB4 7F049F84 24010018 */  li    $at, 24
/* 07EAB8 7F049F88 31CF00FF */  andi  $t7, $t6, 0xff
/* 07EABC 7F049F8C 55E1031A */  bnel  $t7, $at, .L7F04ABF8
/* 07EAC0 7F049F90 8FBF004C */   lw    $ra, 0x4c($sp)
/* 07EAC4 7F049F94 0FC2F5B1 */  jal   dynAllocate7F0BD6C4
/* 07EAC8 7F049F98 24040004 */   li    $a0, 4
/* 07EACC 7F049F9C 8FB800BC */  lw    $t8, 0xbc($sp)
/* 07EAD0 7F049FA0 00409025 */  move  $s2, $v0
/* 07EAD4 7F049FA4 8FA400B0 */  lw    $a0, 0xb0($sp)
/* 07EAD8 7F049FA8 27190008 */  addiu $t9, $t8, 8
/* 07EADC 7F049FAC AFB900BC */  sw    $t9, 0xbc($sp)
/* 07EAE0 7F049FB0 AFB800A8 */  sw    $t8, 0xa8($sp)
/* 07EAE4 7F049FB4 8E280004 */  lw    $t0, 4($s1)
/* 07EAE8 7F049FB8 02202825 */  move  $a1, $s1
/* 07EAEC 7F049FBC 0FC1B1E7 */  jal   modelGetNodeRwData
/* 07EAF0 7F049FC0 AFA800A4 */   sw    $t0, 0xa4($sp)
/* 07EAF4 7F049FC4 3C018005 */  lui   $at, %hi(D_80052C68)
/* 07EAF8 7F049FC8 C43A2C68 */  lwc1  $f26, %lo(D_80052C68)($at)
/* 07EAFC 7F049FCC 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 07EB00 7F049FD0 4481C000 */  mtc1  $at, $f24
/* 07EB04 7F049FD4 3C013A80 */  li    $at, 0x3A800000 # 0.000977
/* 07EB08 7F049FD8 4481B000 */  mtc1  $at, $f22
/* 07EB0C 7F049FDC 3C014780 */  li    $at, 0x47800000 # 65536.000000
/* 07EB10 7F049FE0 44811000 */  mtc1  $at, $f2
/* 07EB14 7F049FE4 4480A000 */  mtc1  $zero, $f20
/* 07EB18 7F049FE8 AFA200A0 */  sw    $v0, 0xa0($sp)
/* 07EB1C 7F049FEC 00008825 */  move  $s1, $zero
/* 07EB20 7F049FF0 96020004 */  lhu   $v0, 4($s0)
.L7F049FF4:
/* 07EB24 7F049FF4 8E090000 */  lw    $t1, ($s0)
/* 07EB28 7F049FF8 00025080 */  sll   $t2, $v0, 2
/* 07EB2C 7F049FFC 012A1821 */  addu  $v1, $t1, $t2
/* 07EB30 7F04A000 8C6B0000 */  lw    $t3, ($v1)
/* 07EB34 7F04A004 2D610010 */  sltiu $at, $t3, 0x10
/* 07EB38 7F04A008 10200117 */  beqz  $at, .L7F04A468
/* 07EB3C 7F04A00C 000B5880 */   sll   $t3, $t3, 2
/* 07EB40 7F04A010 3C018005 */  lui   $at, %hi(jpt_80052C6C)
/* 07EB44 7F04A014 002B0821 */  addu  $at, $at, $t3
/* 07EB48 7F04A018 8C2B2C6C */  lw    $t3, %lo(jpt_80052C6C)($at)
/* 07EB4C 7F04A01C 01600008 */  jr    $t3
/* 07EB50 7F04A020 00000000 */   nop
command00_reset_scroll_shift:
/* 07EB54 7F04A024 244C0001 */  addiu $t4, $v0, 1
/* 07EB58 7F04A028 E6140040 */  swc1  $f20, 0x40($s0)
/* 07EB5C 7F04A02C E6140054 */  swc1  $f20, 0x54($s0)
/* 07EB60 7F04A030 1000010D */  b     .L7F04A468
/* 07EB64 7F04A034 A60C0004 */   sh    $t4, 4($s0)
command01_horizontal_scroll:
/* 07EB68 7F04A038 E614003C */  swc1  $f20, 0x3c($s0)
/* 07EB6C 7F04A03C 8C6D0008 */  lw    $t5, 8($v1)
/* 07EB70 7F04A040 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07EB74 7F04A044 448D2000 */  mtc1  $t5, $f4
/* 07EB78 7F04A048 05A10004 */  bgez  $t5, .L7F04A05C
/* 07EB7C 7F04A04C 468021A0 */   cvt.s.w $f6, $f4
/* 07EB80 7F04A050 44814000 */  mtc1  $at, $f8
/* 07EB84 7F04A054 00000000 */  nop
/* 07EB88 7F04A058 46083180 */  add.s $f6, $f6, $f8
.L7F04A05C:
/* 07EB8C 7F04A05C 4606C283 */  div.s $f10, $f24, $f6
/* 07EB90 7F04A060 C6000038 */  lwc1  $f0, 0x38($s0)
/* 07EB94 7F04A064 244F0003 */  addiu $t7, $v0, 3
/* 07EB98 7F04A068 E6000044 */  swc1  $f0, 0x44($s0)
/* 07EB9C 7F04A06C E60A0040 */  swc1  $f10, 0x40($s0)
/* 07EBA0 7F04A070 8C6E0004 */  lw    $t6, 4($v1)
/* 07EBA4 7F04A074 A60F0004 */  sh    $t7, 4($s0)
/* 07EBA8 7F04A078 448E9000 */  mtc1  $t6, $f18
/* 07EBAC 7F04A07C 00000000 */  nop
/* 07EBB0 7F04A080 46809120 */  cvt.s.w $f4, $f18
/* 07EBB4 7F04A084 46162202 */  mul.s $f8, $f4, $f22
/* 07EBB8 7F04A088 46080180 */  add.s $f6, $f0, $f8
/* 07EBBC 7F04A08C 100000F6 */  b     .L7F04A468
/* 07EBC0 7F04A090 E6060048 */   swc1  $f6, 0x48($s0)
command02_vertical_scroll:
/* 07EBC4 7F04A094 E6140050 */  swc1  $f20, 0x50($s0)
/* 07EBC8 7F04A098 8C780008 */  lw    $t8, 8($v1)
/* 07EBCC 7F04A09C 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07EBD0 7F04A0A0 44985000 */  mtc1  $t8, $f10
/* 07EBD4 7F04A0A4 07010004 */  bgez  $t8, .L7F04A0B8
/* 07EBD8 7F04A0A8 468054A0 */   cvt.s.w $f18, $f10
/* 07EBDC 7F04A0AC 44812000 */  mtc1  $at, $f4
/* 07EBE0 7F04A0B0 00000000 */  nop
/* 07EBE4 7F04A0B4 46049480 */  add.s $f18, $f18, $f4
.L7F04A0B8:
/* 07EBE8 7F04A0B8 4612C203 */  div.s $f8, $f24, $f18
/* 07EBEC 7F04A0BC C600004C */  lwc1  $f0, 0x4c($s0)
/* 07EBF0 7F04A0C0 24480003 */  addiu $t0, $v0, 3
/* 07EBF4 7F04A0C4 E6000058 */  swc1  $f0, 0x58($s0)
/* 07EBF8 7F04A0C8 E6080054 */  swc1  $f8, 0x54($s0)
/* 07EBFC 7F04A0CC 8C790004 */  lw    $t9, 4($v1)
/* 07EC00 7F04A0D0 A6080004 */  sh    $t0, 4($s0)
/* 07EC04 7F04A0D4 44993000 */  mtc1  $t9, $f6
/* 07EC08 7F04A0D8 00000000 */  nop
/* 07EC0C 7F04A0DC 468032A0 */  cvt.s.w $f10, $f6
/* 07EC10 7F04A0E0 46165102 */  mul.s $f4, $f10, $f22
/* 07EC14 7F04A0E4 46040480 */  add.s $f18, $f0, $f4
/* 07EC18 7F04A0E8 100000DF */  b     .L7F04A468
/* 07EC1C 7F04A0EC E612005C */   swc1  $f18, 0x5c($s0)
command03_horizontal_pos:
/* 07EC20 7F04A0F0 E614003C */  swc1  $f20, 0x3c($s0)
/* 07EC24 7F04A0F4 8C690008 */  lw    $t1, 8($v1)
/* 07EC28 7F04A0F8 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07EC2C 7F04A0FC 44894000 */  mtc1  $t1, $f8
/* 07EC30 7F04A100 05210004 */  bgez  $t1, .L7F04A114
/* 07EC34 7F04A104 468041A0 */   cvt.s.w $f6, $f8
/* 07EC38 7F04A108 44815000 */  mtc1  $at, $f10
/* 07EC3C 7F04A10C 00000000 */  nop
/* 07EC40 7F04A110 460A3180 */  add.s $f6, $f6, $f10
.L7F04A114:
/* 07EC44 7F04A114 4606C103 */  div.s $f4, $f24, $f6
/* 07EC48 7F04A118 C6120038 */  lwc1  $f18, 0x38($s0)
/* 07EC4C 7F04A11C 244B0003 */  addiu $t3, $v0, 3
/* 07EC50 7F04A120 E6120044 */  swc1  $f18, 0x44($s0)
/* 07EC54 7F04A124 E6040040 */  swc1  $f4, 0x40($s0)
/* 07EC58 7F04A128 8C6A0004 */  lw    $t2, 4($v1)
/* 07EC5C 7F04A12C A60B0004 */  sh    $t3, 4($s0)
/* 07EC60 7F04A130 448A4000 */  mtc1  $t2, $f8
/* 07EC64 7F04A134 00000000 */  nop
/* 07EC68 7F04A138 468042A0 */  cvt.s.w $f10, $f8
/* 07EC6C 7F04A13C 46165182 */  mul.s $f6, $f10, $f22
/* 07EC70 7F04A140 100000C9 */  b     .L7F04A468
/* 07EC74 7F04A144 E6060048 */   swc1  $f6, 0x48($s0)
command04_vertical_pos:
/* 07EC78 7F04A148 E6140050 */  swc1  $f20, 0x50($s0)
/* 07EC7C 7F04A14C 8C6C0008 */  lw    $t4, 8($v1)
/* 07EC80 7F04A150 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07EC84 7F04A154 448C2000 */  mtc1  $t4, $f4
/* 07EC88 7F04A158 05810004 */  bgez  $t4, .L7F04A16C
/* 07EC8C 7F04A15C 468024A0 */   cvt.s.w $f18, $f4
/* 07EC90 7F04A160 44814000 */  mtc1  $at, $f8
/* 07EC94 7F04A164 00000000 */  nop
/* 07EC98 7F04A168 46089480 */  add.s $f18, $f18, $f8
.L7F04A16C:
/* 07EC9C 7F04A16C 4612C283 */  div.s $f10, $f24, $f18
/* 07ECA0 7F04A170 C606004C */  lwc1  $f6, 0x4c($s0)
/* 07ECA4 7F04A174 244E0003 */  addiu $t6, $v0, 3
/* 07ECA8 7F04A178 E6060058 */  swc1  $f6, 0x58($s0)
/* 07ECAC 7F04A17C E60A0054 */  swc1  $f10, 0x54($s0)
/* 07ECB0 7F04A180 8C6D0004 */  lw    $t5, 4($v1)
/* 07ECB4 7F04A184 A60E0004 */  sh    $t6, 4($s0)
/* 07ECB8 7F04A188 448D2000 */  mtc1  $t5, $f4
/* 07ECBC 7F04A18C 00000000 */  nop
/* 07ECC0 7F04A190 46802220 */  cvt.s.w $f8, $f4
/* 07ECC4 7F04A194 46164482 */  mul.s $f18, $f8, $f22
/* 07ECC8 7F04A198 100000B3 */  b     .L7F04A468
/* 07ECCC 7F04A19C E612005C */   swc1  $f18, 0x5c($s0)
command05_zoomx:
/* 07ECD0 7F04A1A0 E6140014 */  swc1  $f20, 0x14($s0)
/* 07ECD4 7F04A1A4 8C6F0008 */  lw    $t7, 8($v1)
/* 07ECD8 7F04A1A8 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07ECDC 7F04A1AC 448F5000 */  mtc1  $t7, $f10
/* 07ECE0 7F04A1B0 05E10004 */  bgez  $t7, .L7F04A1C4
/* 07ECE4 7F04A1B4 468051A0 */   cvt.s.w $f6, $f10
/* 07ECE8 7F04A1B8 44812000 */  mtc1  $at, $f4
/* 07ECEC 7F04A1BC 00000000 */  nop
/* 07ECF0 7F04A1C0 46043180 */  add.s $f6, $f6, $f4
.L7F04A1C4:
/* 07ECF4 7F04A1C4 4606C203 */  div.s $f8, $f24, $f6
/* 07ECF8 7F04A1C8 C6120010 */  lwc1  $f18, 0x10($s0)
/* 07ECFC 7F04A1CC 24590003 */  addiu $t9, $v0, 3
/* 07ED00 7F04A1D0 E612001C */  swc1  $f18, 0x1c($s0)
/* 07ED04 7F04A1D4 E6080018 */  swc1  $f8, 0x18($s0)
/* 07ED08 7F04A1D8 8C780004 */  lw    $t8, 4($v1)
/* 07ED0C 7F04A1DC A6190004 */  sh    $t9, 4($s0)
/* 07ED10 7F04A1E0 44985000 */  mtc1  $t8, $f10
/* 07ED14 7F04A1E4 00000000 */  nop
/* 07ED18 7F04A1E8 46805120 */  cvt.s.w $f4, $f10
/* 07ED1C 7F04A1EC 46162182 */  mul.s $f6, $f4, $f22
/* 07ED20 7F04A1F0 1000009D */  b     .L7F04A468
/* 07ED24 7F04A1F4 E6060020 */   swc1  $f6, 0x20($s0)
command06_zoomy:
/* 07ED28 7F04A1F8 E6140028 */  swc1  $f20, 0x28($s0)
/* 07ED2C 7F04A1FC 8C680008 */  lw    $t0, 8($v1)
/* 07ED30 7F04A200 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07ED34 7F04A204 44884000 */  mtc1  $t0, $f8
/* 07ED38 7F04A208 05010004 */  bgez  $t0, .L7F04A21C
/* 07ED3C 7F04A20C 468044A0 */   cvt.s.w $f18, $f8
/* 07ED40 7F04A210 44815000 */  mtc1  $at, $f10
/* 07ED44 7F04A214 00000000 */  nop
/* 07ED48 7F04A218 460A9480 */  add.s $f18, $f18, $f10
.L7F04A21C:
/* 07ED4C 7F04A21C 4612C103 */  div.s $f4, $f24, $f18
/* 07ED50 7F04A220 C6060024 */  lwc1  $f6, 0x24($s0)
/* 07ED54 7F04A224 244A0003 */  addiu $t2, $v0, 3
/* 07ED58 7F04A228 E6060030 */  swc1  $f6, 0x30($s0)
/* 07ED5C 7F04A22C E604002C */  swc1  $f4, 0x2c($s0)
/* 07ED60 7F04A230 8C690004 */  lw    $t1, 4($v1)
/* 07ED64 7F04A234 A60A0004 */  sh    $t2, 4($s0)
/* 07ED68 7F04A238 44894000 */  mtc1  $t1, $f8
/* 07ED6C 7F04A23C 00000000 */  nop
/* 07ED70 7F04A240 468042A0 */  cvt.s.w $f10, $f8
/* 07ED74 7F04A244 46165482 */  mul.s $f18, $f10, $f22
/* 07ED78 7F04A248 10000087 */  b     .L7F04A468
/* 07ED7C 7F04A24C E6120034 */   swc1  $f18, 0x34($s0)
command07_use_image_from_global_monitor_table:
/* 07ED80 7F04A250 02002025 */  move  $a0, $s0
/* 07ED84 7F04A254 0FC127D0 */  jal   save_img_index_to_obj_ani_slot
/* 07ED88 7F04A258 8C650004 */   lw    $a1, 4($v1)
/* 07ED8C 7F04A25C 960B0004 */  lhu   $t3, 4($s0)
/* 07ED90 7F04A260 3C014780 */  li    $at, 0x47800000 # 65536.000000
/* 07ED94 7F04A264 44811000 */  mtc1  $at, $f2
/* 07ED98 7F04A268 256C0002 */  addiu $t4, $t3, 2
/* 07ED9C 7F04A26C 1000007E */  b     .L7F04A468
/* 07EDA0 7F04A270 A60C0004 */   sh    $t4, 4($s0)
command08_halt_processing_for_time:
/* 07EDA4 7F04A274 86020006 */  lh    $v0, 6($s0)
/* 07EDA8 7F04A278 3C0D8005 */  lui   $t5, %hi(g_ClockTimer)
/* 07EDAC 7F04A27C 0442000E */  bltzl $v0, .L7F04A2B8
/* 07EDB0 7F04A280 8C680004 */   lw    $t0, 4($v1)
/* 07EDB4 7F04A284 8DAD8374 */  lw    $t5, %lo(g_ClockTimer)($t5)
/* 07EDB8 7F04A288 004D7023 */  subu  $t6, $v0, $t5
/* 07EDBC 7F04A28C A60E0006 */  sh    $t6, 6($s0)
/* 07EDC0 7F04A290 860F0006 */  lh    $t7, 6($s0)
/* 07EDC4 7F04A294 05E20004 */  bltzl $t7, .L7F04A2A8
/* 07EDC8 7F04A298 96180004 */   lhu   $t8, 4($s0)
/* 07EDCC 7F04A29C 10000072 */  b     .L7F04A468
/* 07EDD0 7F04A2A0 24110001 */   li    $s1, 1
/* 07EDD4 7F04A2A4 96180004 */  lhu   $t8, 4($s0)
.L7F04A2A8:
/* 07EDD8 7F04A2A8 27190002 */  addiu $t9, $t8, 2
/* 07EDDC 7F04A2AC 1000006E */  b     .L7F04A468
/* 07EDE0 7F04A2B0 A6190004 */   sh    $t9, 4($s0)
/* 07EDE4 7F04A2B4 8C680004 */  lw    $t0, 4($v1)
.L7F04A2B8:
/* 07EDE8 7F04A2B8 24110001 */  li    $s1, 1
/* 07EDEC 7F04A2BC 1000006A */  b     .L7F04A468
/* 07EDF0 7F04A2C0 A6080006 */   sh    $t0, 6($s0)
command09_jump:
/* 07EDF4 7F04A2C4 02002025 */  move  $a0, $s0
/* 07EDF8 7F04A2C8 0FC12723 */  jal   save_ptr_monitor_ani_code_to_obj_ani_slot
/* 07EDFC 7F04A2CC 8C650004 */   lw    $a1, 4($v1)
/* 07EE00 7F04A2D0 3C014780 */  li    $at, 0x47800000 # 65536.000000
/* 07EE04 7F04A2D4 44811000 */  mtc1  $at, $f2
/* 07EE08 7F04A2D8 10000063 */  b     .L7F04A468
/* 07EE0C 7F04A2DC 00000000 */   nop
command0A_jump_conditional:
/* 07EE10 7F04A2E0 0C002914 */  jal   randomGetNext
/* 07EE14 7F04A2E4 AFA30094 */   sw    $v1, 0x94($sp)
/* 07EE18 7F04A2E8 8FA30094 */  lw    $v1, 0x94($sp)
/* 07EE1C 7F04A2EC 00024C02 */  srl   $t1, $v0, 0x10
/* 07EE20 7F04A2F0 02002025 */  move  $a0, $s0
/* 07EE24 7F04A2F4 8C6A0008 */  lw    $t2, 8($v1)
/* 07EE28 7F04A2F8 012A082B */  sltu  $at, $t1, $t2
/* 07EE2C 7F04A2FC 50200008 */  beql  $at, $zero, .L7F04A320
/* 07EE30 7F04A300 960B0004 */   lhu   $t3, 4($s0)
/* 07EE34 7F04A304 0FC12723 */  jal   save_ptr_monitor_ani_code_to_obj_ani_slot
/* 07EE38 7F04A308 8C650004 */   lw    $a1, 4($v1)
/* 07EE3C 7F04A30C 3C014780 */  li    $at, 0x47800000 # 65536.000000
/* 07EE40 7F04A310 44811000 */  mtc1  $at, $f2
/* 07EE44 7F04A314 10000054 */  b     .L7F04A468
/* 07EE48 7F04A318 00000000 */   nop
/* 07EE4C 7F04A31C 960B0004 */  lhu   $t3, 4($s0)
.L7F04A320:
/* 07EE50 7F04A320 3C014780 */  li    $at, 0x47800000 # 65536.000000
/* 07EE54 7F04A324 44811000 */  mtc1  $at, $f2
/* 07EE58 7F04A328 256C0003 */  addiu $t4, $t3, 3
/* 07EE5C 7F04A32C A60C0004 */  sh    $t4, 4($s0)
/* 07EE60 7F04A330 1000004D */  b     .L7F04A468
/* 07EE64 7F04A334 00000000 */   nop
command0B_restart:
/* 07EE68 7F04A338 1000004B */  b     .L7F04A468
/* 07EE6C 7F04A33C A6000004 */   sh    $zero, 4($s0)
command0C_break:
/* 07EE70 7F04A340 10000049 */  b     .L7F04A468
/* 07EE74 7F04A344 24110001 */   li    $s1, 1
command0D_colour_transition:
/* 07EE78 7F04A348 E614006C */  swc1  $f20, 0x6c($s0)
/* 07EE7C 7F04A34C 8C6D0008 */  lw    $t5, 8($v1)
/* 07EE80 7F04A350 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07EE84 7F04A354 448D2000 */  mtc1  $t5, $f4
/* 07EE88 7F04A358 05A10004 */  bgez  $t5, .L7F04A36C
/* 07EE8C 7F04A35C 468021A0 */   cvt.s.w $f6, $f4
/* 07EE90 7F04A360 44814000 */  mtc1  $at, $f8
/* 07EE94 7F04A364 00000000 */  nop
/* 07EE98 7F04A368 46083180 */  add.s $f6, $f6, $f8
.L7F04A36C:
/* 07EE9C 7F04A36C 4606C283 */  div.s $f10, $f24, $f6
/* 07EEA0 7F04A370 920E0060 */  lbu   $t6, 0x60($s0)
/* 07EEA4 7F04A374 92080063 */  lbu   $t0, 0x63($s0)
/* 07EEA8 7F04A378 920C0066 */  lbu   $t4, 0x66($s0)
/* 07EEAC 7F04A37C A20E0061 */  sb    $t6, 0x61($s0)
/* 07EEB0 7F04A380 92180069 */  lbu   $t8, 0x69($s0)
/* 07EEB4 7F04A384 E60A0070 */  swc1  $f10, 0x70($s0)
/* 07EEB8 7F04A388 8C6F0004 */  lw    $t7, 4($v1)
/* 07EEBC 7F04A38C A2080064 */  sb    $t0, 0x64($s0)
/* 07EEC0 7F04A390 000FCE02 */  srl   $t9, $t7, 0x18
/* 07EEC4 7F04A394 A2190062 */  sb    $t9, 0x62($s0)
/* 07EEC8 7F04A398 8C690004 */  lw    $t1, 4($v1)
/* 07EECC 7F04A39C A20C0067 */  sb    $t4, 0x67($s0)
/* 07EED0 7F04A3A0 00095C02 */  srl   $t3, $t1, 0x10
/* 07EED4 7F04A3A4 A20B0065 */  sb    $t3, 0x65($s0)
/* 07EED8 7F04A3A8 8C6D0004 */  lw    $t5, 4($v1)
/* 07EEDC 7F04A3AC A218006A */  sb    $t8, 0x6a($s0)
/* 07EEE0 7F04A3B0 24490003 */  addiu $t1, $v0, 3
/* 07EEE4 7F04A3B4 000D7A02 */  srl   $t7, $t5, 8
/* 07EEE8 7F04A3B8 A20F0068 */  sb    $t7, 0x68($s0)
/* 07EEEC 7F04A3BC 8C680004 */  lw    $t0, 4($v1)
/* 07EEF0 7F04A3C0 A6090004 */  sh    $t1, 4($s0)
/* 07EEF4 7F04A3C4 10000028 */  b     .L7F04A468
/* 07EEF8 7F04A3C8 A208006B */   sb    $t0, 0x6b($s0)
command0E_set_rotation:
/* 07EEFC 7F04A3CC 8C6A0004 */  lw    $t2, 4($v1)
/* 07EF00 7F04A3D0 244B0002 */  addiu $t3, $v0, 2
/* 07EF04 7F04A3D4 A60B0004 */  sh    $t3, 4($s0)
/* 07EF08 7F04A3D8 448A9000 */  mtc1  $t2, $f18
/* 07EF0C 7F04A3DC 00000000 */  nop
/* 07EF10 7F04A3E0 46809120 */  cvt.s.w $f4, $f18
/* 07EF14 7F04A3E4 461A2202 */  mul.s $f8, $f4, $f26
/* 07EF18 7F04A3E8 46024183 */  div.s $f6, $f8, $f2
/* 07EF1C 7F04A3EC 1000001E */  b     .L7F04A468
/* 07EF20 7F04A3F0 E606000C */   swc1  $f6, 0xc($s0)
command0F_rotate:
/* 07EF24 7F04A3F4 8C6C0004 */  lw    $t4, 4($v1)
/* 07EF28 7F04A3F8 3C018005 */  lui   $at, %hi(g_GlobalTimerDelta)
/* 07EF2C 7F04A3FC C42A8378 */  lwc1  $f10, %lo(g_GlobalTimerDelta)($at)
/* 07EF30 7F04A400 448C9000 */  mtc1  $t4, $f18
/* 07EF34 7F04A404 00000000 */  nop
/* 07EF38 7F04A408 46809120 */  cvt.s.w $f4, $f18
/* 07EF3C 7F04A40C 46045202 */  mul.s $f8, $f10, $f4
/* 07EF40 7F04A410 C60A000C */  lwc1  $f10, 0xc($s0)
/* 07EF44 7F04A414 461A4182 */  mul.s $f6, $f8, $f26
/* 07EF48 7F04A418 46023483 */  div.s $f18, $f6, $f2
/* 07EF4C 7F04A41C 46125100 */  add.s $f4, $f10, $f18
/* 07EF50 7F04A420 E604000C */  swc1  $f4, 0xc($s0)
/* 07EF54 7F04A424 C60C000C */  lwc1  $f12, 0xc($s0)
/* 07EF58 7F04A428 460CD03E */  c.le.s $f26, $f12
/* 07EF5C 7F04A42C 00000000 */  nop
/* 07EF60 7F04A430 45020005 */  bc1fl .L7F04A448
/* 07EF64 7F04A434 4614603C */   c.lt.s $f12, $f20
/* 07EF68 7F04A438 461A6201 */  sub.s $f8, $f12, $f26
/* 07EF6C 7F04A43C E608000C */  swc1  $f8, 0xc($s0)
/* 07EF70 7F04A440 C60C000C */  lwc1  $f12, 0xc($s0)
/* 07EF74 7F04A444 4614603C */  c.lt.s $f12, $f20
.L7F04A448:
/* 07EF78 7F04A448 00000000 */  nop
/* 07EF7C 7F04A44C 45020004 */  bc1fl .L7F04A460
/* 07EF80 7F04A450 960D0004 */   lhu   $t5, 4($s0)
/* 07EF84 7F04A454 461A6180 */  add.s $f6, $f12, $f26
/* 07EF88 7F04A458 E606000C */  swc1  $f6, 0xc($s0)
/* 07EF8C 7F04A45C 960D0004 */  lhu   $t5, 4($s0)
.L7F04A460:
/* 07EF90 7F04A460 25AE0002 */  addiu $t6, $t5, 2
/* 07EF94 7F04A464 A60E0004 */  sh    $t6, 4($s0)
def_7F04A01C:
.L7F04A468:
/* 07EF98 7F04A468 5220FEE2 */  beql  $s1, $zero, .L7F049FF4
/* 07EF9C 7F04A46C 96020004 */   lhu   $v0, 4($s0)
/* 07EFA0 7F04A470 C6000018 */  lwc1  $f0, 0x18($s0)
/* 07EFA4 7F04A474 4600A03C */  c.lt.s $f20, $f0
/* 07EFA8 7F04A478 00000000 */  nop
/* 07EFAC 7F04A47C 45000017 */  bc1f  .L7F04A4DC
/* 07EFB0 7F04A480 3C028005 */   lui   $v0, %hi(g_GlobalTimerDelta)
/* 07EFB4 7F04A484 24428378 */  addiu $v0, %lo(g_GlobalTimerDelta) # addiu $v0, $v0, -0x7c88
/* 07EFB8 7F04A488 C4520000 */  lwc1  $f18, ($v0)
/* 07EFBC 7F04A48C C60A0014 */  lwc1  $f10, 0x14($s0)
/* 07EFC0 7F04A490 46120102 */  mul.s $f4, $f0, $f18
/* 07EFC4 7F04A494 46045200 */  add.s $f8, $f10, $f4
/* 07EFC8 7F04A498 E6080014 */  swc1  $f8, 0x14($s0)
/* 07EFCC 7F04A49C C6020014 */  lwc1  $f2, 0x14($s0)
/* 07EFD0 7F04A4A0 4618103C */  c.lt.s $f2, $f24
/* 07EFD4 7F04A4A4 00000000 */  nop
/* 07EFD8 7F04A4A8 45020009 */  bc1fl .L7F04A4D0
/* 07EFDC 7F04A4AC C6080020 */   lwc1  $f8, 0x20($s0)
/* 07EFE0 7F04A4B0 C600001C */  lwc1  $f0, 0x1c($s0)
/* 07EFE4 7F04A4B4 C6060020 */  lwc1  $f6, 0x20($s0)
/* 07EFE8 7F04A4B8 46003481 */  sub.s $f18, $f6, $f0
/* 07EFEC 7F04A4BC 46029282 */  mul.s $f10, $f18, $f2
/* 07EFF0 7F04A4C0 460A0100 */  add.s $f4, $f0, $f10
/* 07EFF4 7F04A4C4 10000005 */  b     .L7F04A4DC
/* 07EFF8 7F04A4C8 E6040010 */   swc1  $f4, 0x10($s0)
/* 07EFFC 7F04A4CC C6080020 */  lwc1  $f8, 0x20($s0)
.L7F04A4D0:
/* 07F000 7F04A4D0 E6180014 */  swc1  $f24, 0x14($s0)
/* 07F004 7F04A4D4 E6140018 */  swc1  $f20, 0x18($s0)
/* 07F008 7F04A4D8 E6080010 */  swc1  $f8, 0x10($s0)
.L7F04A4DC:
/* 07F00C 7F04A4DC C600002C */  lwc1  $f0, 0x2c($s0)
/* 07F010 7F04A4E0 3C028005 */  lui   $v0, %hi(g_GlobalTimerDelta)
/* 07F014 7F04A4E4 24428378 */  addiu $v0, %lo(g_GlobalTimerDelta) # addiu $v0, $v0, -0x7c88
/* 07F018 7F04A4E8 4600A03C */  c.lt.s $f20, $f0
/* 07F01C 7F04A4EC 00000000 */  nop
/* 07F020 7F04A4F0 45020017 */  bc1fl .L7F04A550
/* 07F024 7F04A4F4 C6000040 */   lwc1  $f0, 0x40($s0)
/* 07F028 7F04A4F8 C4520000 */  lwc1  $f18, ($v0)
/* 07F02C 7F04A4FC C6060028 */  lwc1  $f6, 0x28($s0)
/* 07F030 7F04A500 46120282 */  mul.s $f10, $f0, $f18
/* 07F034 7F04A504 460A3100 */  add.s $f4, $f6, $f10
/* 07F038 7F04A508 E6040028 */  swc1  $f4, 0x28($s0)
/* 07F03C 7F04A50C C6020028 */  lwc1  $f2, 0x28($s0)
/* 07F040 7F04A510 4618103C */  c.lt.s $f2, $f24
/* 07F044 7F04A514 00000000 */  nop
/* 07F048 7F04A518 45020009 */  bc1fl .L7F04A540
/* 07F04C 7F04A51C C6040034 */   lwc1  $f4, 0x34($s0)
/* 07F050 7F04A520 C6000030 */  lwc1  $f0, 0x30($s0)
/* 07F054 7F04A524 C6080034 */  lwc1  $f8, 0x34($s0)
/* 07F058 7F04A528 46004481 */  sub.s $f18, $f8, $f0
/* 07F05C 7F04A52C 46029182 */  mul.s $f6, $f18, $f2
/* 07F060 7F04A530 46060280 */  add.s $f10, $f0, $f6
/* 07F064 7F04A534 10000005 */  b     .L7F04A54C
/* 07F068 7F04A538 E60A0024 */   swc1  $f10, 0x24($s0)
/* 07F06C 7F04A53C C6040034 */  lwc1  $f4, 0x34($s0)
.L7F04A540:
/* 07F070 7F04A540 E6180028 */  swc1  $f24, 0x28($s0)
/* 07F074 7F04A544 E614002C */  swc1  $f20, 0x2c($s0)
/* 07F078 7F04A548 E6040024 */  swc1  $f4, 0x24($s0)
.L7F04A54C:
/* 07F07C 7F04A54C C6000040 */  lwc1  $f0, 0x40($s0)
.L7F04A550:
/* 07F080 7F04A550 4600A03C */  c.lt.s $f20, $f0
/* 07F084 7F04A554 00000000 */  nop
/* 07F088 7F04A558 45020017 */  bc1fl .L7F04A5B8
/* 07F08C 7F04A55C C6000054 */   lwc1  $f0, 0x54($s0)
/* 07F090 7F04A560 C4520000 */  lwc1  $f18, ($v0)
/* 07F094 7F04A564 C608003C */  lwc1  $f8, 0x3c($s0)
/* 07F098 7F04A568 46120182 */  mul.s $f6, $f0, $f18
/* 07F09C 7F04A56C 46064280 */  add.s $f10, $f8, $f6
/* 07F0A0 7F04A570 E60A003C */  swc1  $f10, 0x3c($s0)
/* 07F0A4 7F04A574 C602003C */  lwc1  $f2, 0x3c($s0)
/* 07F0A8 7F04A578 4618103C */  c.lt.s $f2, $f24
/* 07F0AC 7F04A57C 00000000 */  nop
/* 07F0B0 7F04A580 45020009 */  bc1fl .L7F04A5A8
/* 07F0B4 7F04A584 C60A0048 */   lwc1  $f10, 0x48($s0)
/* 07F0B8 7F04A588 C6000044 */  lwc1  $f0, 0x44($s0)
/* 07F0BC 7F04A58C C6040048 */  lwc1  $f4, 0x48($s0)
/* 07F0C0 7F04A590 46002481 */  sub.s $f18, $f4, $f0
/* 07F0C4 7F04A594 46029202 */  mul.s $f8, $f18, $f2
/* 07F0C8 7F04A598 46080180 */  add.s $f6, $f0, $f8
/* 07F0CC 7F04A59C 10000005 */  b     .L7F04A5B4
/* 07F0D0 7F04A5A0 E6060038 */   swc1  $f6, 0x38($s0)
/* 07F0D4 7F04A5A4 C60A0048 */  lwc1  $f10, 0x48($s0)
.L7F04A5A8:
/* 07F0D8 7F04A5A8 E618003C */  swc1  $f24, 0x3c($s0)
/* 07F0DC 7F04A5AC E6140040 */  swc1  $f20, 0x40($s0)
/* 07F0E0 7F04A5B0 E60A0038 */  swc1  $f10, 0x38($s0)
.L7F04A5B4:
/* 07F0E4 7F04A5B4 C6000054 */  lwc1  $f0, 0x54($s0)
.L7F04A5B8:
/* 07F0E8 7F04A5B8 4600A03C */  c.lt.s $f20, $f0
/* 07F0EC 7F04A5BC 00000000 */  nop
/* 07F0F0 7F04A5C0 45020017 */  bc1fl .L7F04A620
/* 07F0F4 7F04A5C4 C6020070 */   lwc1  $f2, 0x70($s0)
/* 07F0F8 7F04A5C8 C4520000 */  lwc1  $f18, ($v0)
/* 07F0FC 7F04A5CC C6040050 */  lwc1  $f4, 0x50($s0)
/* 07F100 7F04A5D0 46120202 */  mul.s $f8, $f0, $f18
/* 07F104 7F04A5D4 46082180 */  add.s $f6, $f4, $f8
/* 07F108 7F04A5D8 E6060050 */  swc1  $f6, 0x50($s0)
/* 07F10C 7F04A5DC C6020050 */  lwc1  $f2, 0x50($s0)
/* 07F110 7F04A5E0 4618103C */  c.lt.s $f2, $f24
/* 07F114 7F04A5E4 00000000 */  nop
/* 07F118 7F04A5E8 45020009 */  bc1fl .L7F04A610
/* 07F11C 7F04A5EC C606005C */   lwc1  $f6, 0x5c($s0)
/* 07F120 7F04A5F0 C6000058 */  lwc1  $f0, 0x58($s0)
/* 07F124 7F04A5F4 C60A005C */  lwc1  $f10, 0x5c($s0)
/* 07F128 7F04A5F8 46005481 */  sub.s $f18, $f10, $f0
/* 07F12C 7F04A5FC 46029102 */  mul.s $f4, $f18, $f2
/* 07F130 7F04A600 46040200 */  add.s $f8, $f0, $f4
/* 07F134 7F04A604 10000005 */  b     .L7F04A61C
/* 07F138 7F04A608 E608004C */   swc1  $f8, 0x4c($s0)
/* 07F13C 7F04A60C C606005C */  lwc1  $f6, 0x5c($s0)
.L7F04A610:
/* 07F140 7F04A610 E6180050 */  swc1  $f24, 0x50($s0)
/* 07F144 7F04A614 E6140054 */  swc1  $f20, 0x54($s0)
/* 07F148 7F04A618 E606004C */  swc1  $f6, 0x4c($s0)
.L7F04A61C:
/* 07F14C 7F04A61C C6020070 */  lwc1  $f2, 0x70($s0)
.L7F04A620:
/* 07F150 7F04A620 4602A03C */  c.lt.s $f20, $f2
/* 07F154 7F04A624 00000000 */  nop
/* 07F158 7F04A628 4502003F */  bc1fl .L7F04A728
/* 07F15C 7F04A62C 8FAA00A0 */   lw    $t2, 0xa0($sp)
/* 07F160 7F04A630 C4520000 */  lwc1  $f18, ($v0)
/* 07F164 7F04A634 C60A006C */  lwc1  $f10, 0x6c($s0)
/* 07F168 7F04A638 46121102 */  mul.s $f4, $f2, $f18
/* 07F16C 7F04A63C 46045200 */  add.s $f8, $f10, $f4
/* 07F170 7F04A640 E608006C */  swc1  $f8, 0x6c($s0)
/* 07F174 7F04A644 C600006C */  lwc1  $f0, 0x6c($s0)
/* 07F178 7F04A648 4618003C */  c.lt.s $f0, $f24
/* 07F17C 7F04A64C 00000000 */  nop
/* 07F180 7F04A650 4502002B */  bc1fl .L7F04A700
/* 07F184 7F04A654 920F0062 */   lbu   $t7, 0x62($s0)
/* 07F188 7F04A658 92020061 */  lbu   $v0, 0x61($s0)
/* 07F18C 7F04A65C 920F0062 */  lbu   $t7, 0x62($s0)
/* 07F190 7F04A660 92030064 */  lbu   $v1, 0x64($s0)
/* 07F194 7F04A664 920A0065 */  lbu   $t2, 0x65($s0)
/* 07F198 7F04A668 01E2C023 */  subu  $t8, $t7, $v0
/* 07F19C 7F04A66C 44983000 */  mtc1  $t8, $f6
/* 07F1A0 7F04A670 01435823 */  subu  $t3, $t2, $v1
/* 07F1A4 7F04A674 448B4000 */  mtc1  $t3, $f8
/* 07F1A8 7F04A678 468034A0 */  cvt.s.w $f18, $f6
/* 07F1AC 7F04A67C 920F0068 */  lbu   $t7, 0x68($s0)
/* 07F1B0 7F04A680 92040067 */  lbu   $a0, 0x67($s0)
/* 07F1B4 7F04A684 920A006B */  lbu   $t2, 0x6b($s0)
/* 07F1B8 7F04A688 9205006A */  lbu   $a1, 0x6a($s0)
/* 07F1BC 7F04A68C 468041A0 */  cvt.s.w $f6, $f8
/* 07F1C0 7F04A690 46009282 */  mul.s $f10, $f18, $f0
/* 07F1C4 7F04A694 01E4C023 */  subu  $t8, $t7, $a0
/* 07F1C8 7F04A698 01455823 */  subu  $t3, $t2, $a1
/* 07F1CC 7F04A69C 46003482 */  mul.s $f18, $f6, $f0
/* 07F1D0 7F04A6A0 4600510D */  trunc.w.s $f4, $f10
/* 07F1D4 7F04A6A4 4600928D */  trunc.w.s $f10, $f18
/* 07F1D8 7F04A6A8 44082000 */  mfc1  $t0, $f4
/* 07F1DC 7F04A6AC 44982000 */  mtc1  $t8, $f4
/* 07F1E0 7F04A6B0 440D5000 */  mfc1  $t5, $f10
/* 07F1E4 7F04A6B4 46802220 */  cvt.s.w $f8, $f4
/* 07F1E8 7F04A6B8 448B5000 */  mtc1  $t3, $f10
/* 07F1EC 7F04A6BC 00484821 */  addu  $t1, $v0, $t0
/* 07F1F0 7F04A6C0 006D7021 */  addu  $t6, $v1, $t5
/* 07F1F4 7F04A6C4 A2090060 */  sb    $t1, 0x60($s0)
/* 07F1F8 7F04A6C8 46805120 */  cvt.s.w $f4, $f10
/* 07F1FC 7F04A6CC 46004182 */  mul.s $f6, $f8, $f0
/* 07F200 7F04A6D0 A20E0063 */  sb    $t6, 0x63($s0)
/* 07F204 7F04A6D4 46002202 */  mul.s $f8, $f4, $f0
/* 07F208 7F04A6D8 4600348D */  trunc.w.s $f18, $f6
/* 07F20C 7F04A6DC 4600418D */  trunc.w.s $f6, $f8
/* 07F210 7F04A6E0 44089000 */  mfc1  $t0, $f18
/* 07F214 7F04A6E4 440D3000 */  mfc1  $t5, $f6
/* 07F218 7F04A6E8 00884821 */  addu  $t1, $a0, $t0
/* 07F21C 7F04A6EC A2090066 */  sb    $t1, 0x66($s0)
/* 07F220 7F04A6F0 00AD7021 */  addu  $t6, $a1, $t5
/* 07F224 7F04A6F4 1000000B */  b     .L7F04A724
/* 07F228 7F04A6F8 A20E0069 */   sb    $t6, 0x69($s0)
/* 07F22C 7F04A6FC 920F0062 */  lbu   $t7, 0x62($s0)
.L7F04A700:
/* 07F230 7F04A700 92180065 */  lbu   $t8, 0x65($s0)
/* 07F234 7F04A704 92190068 */  lbu   $t9, 0x68($s0)
/* 07F238 7F04A708 9208006B */  lbu   $t0, 0x6b($s0)
/* 07F23C 7F04A70C E618006C */  swc1  $f24, 0x6c($s0)
/* 07F240 7F04A710 E6140070 */  swc1  $f20, 0x70($s0)
/* 07F244 7F04A714 A20F0060 */  sb    $t7, 0x60($s0)
/* 07F248 7F04A718 A2180063 */  sb    $t8, 0x63($s0)
/* 07F24C 7F04A71C A2190066 */  sb    $t9, 0x66($s0)
/* 07F250 7F04A720 A2080069 */  sb    $t0, 0x69($s0)
.L7F04A724:
/* 07F254 7F04A724 8FAA00A0 */  lw    $t2, 0xa0($sp)
.L7F04A728:
/* 07F258 7F04A728 8FA300A4 */  lw    $v1, 0xa4($sp)
/* 07F25C 7F04A72C 8FA900BC */  lw    $t1, 0xbc($sp)
/* 07F260 7F04A730 AD520000 */  sw    $s2, ($t2)
/* 07F264 7F04A734 3C0F8009 */  lui   $t7, %hi(monitorimages)
/* 07F268 7F04A738 AD490004 */  sw    $t1, 4($t2)
/* 07F26C 7F04A73C 8C6B0008 */  lw    $t3, 8($v1)
/* 07F270 7F04A740 8D610000 */  lw    $at, ($t3)
/* 07F274 7F04A744 AE410000 */  sw    $at, ($s2)
/* 07F278 7F04A748 8D6D0004 */  lw    $t5, 4($t3)
/* 07F27C 7F04A74C AE4D0004 */  sw    $t5, 4($s2)
/* 07F280 7F04A750 8D610008 */  lw    $at, 8($t3)
/* 07F284 7F04A754 AE410008 */  sw    $at, 8($s2)
/* 07F288 7F04A758 8D6D000C */  lw    $t5, 0xc($t3)
/* 07F28C 7F04A75C AE4D000C */  sw    $t5, 0xc($s2)
/* 07F290 7F04A760 8C6E0008 */  lw    $t6, 8($v1)
/* 07F294 7F04A764 8DC10010 */  lw    $at, 0x10($t6)
/* 07F298 7F04A768 AE410010 */  sw    $at, 0x10($s2)
/* 07F29C 7F04A76C 8DD80014 */  lw    $t8, 0x14($t6)
/* 07F2A0 7F04A770 AE580014 */  sw    $t8, 0x14($s2)
/* 07F2A4 7F04A774 8DC10018 */  lw    $at, 0x18($t6)
/* 07F2A8 7F04A778 AE410018 */  sw    $at, 0x18($s2)
/* 07F2AC 7F04A77C 8DD8001C */  lw    $t8, 0x1c($t6)
/* 07F2B0 7F04A780 AE58001C */  sw    $t8, 0x1c($s2)
/* 07F2B4 7F04A784 8C790008 */  lw    $t9, 8($v1)
/* 07F2B8 7F04A788 8F210020 */  lw    $at, 0x20($t9)
/* 07F2BC 7F04A78C AE410020 */  sw    $at, 0x20($s2)
/* 07F2C0 7F04A790 8F290024 */  lw    $t1, 0x24($t9)
/* 07F2C4 7F04A794 AE490024 */  sw    $t1, 0x24($s2)
/* 07F2C8 7F04A798 8F210028 */  lw    $at, 0x28($t9)
/* 07F2CC 7F04A79C AE410028 */  sw    $at, 0x28($s2)
/* 07F2D0 7F04A7A0 8F29002C */  lw    $t1, 0x2c($t9)
/* 07F2D4 7F04A7A4 AE49002C */  sw    $t1, 0x2c($s2)
/* 07F2D8 7F04A7A8 8C6A0008 */  lw    $t2, 8($v1)
/* 07F2DC 7F04A7AC 8D410030 */  lw    $at, 0x30($t2)
/* 07F2E0 7F04A7B0 AE410030 */  sw    $at, 0x30($s2)
/* 07F2E4 7F04A7B4 8D4B0034 */  lw    $t3, 0x34($t2)
/* 07F2E8 7F04A7B8 AE4B0034 */  sw    $t3, 0x34($s2)
/* 07F2EC 7F04A7BC 8D410038 */  lw    $at, 0x38($t2)
/* 07F2F0 7F04A7C0 AE410038 */  sw    $at, 0x38($s2)
/* 07F2F4 7F04A7C4 8D4B003C */  lw    $t3, 0x3c($t2)
/* 07F2F8 7F04A7C8 AE4B003C */  sw    $t3, 0x3c($s2)
/* 07F2FC 7F04A7CC 8E020008 */  lw    $v0, 8($s0)
/* 07F300 7F04A7D0 2C410064 */  sltiu $at, $v0, 0x64
/* 07F304 7F04A7D4 10200007 */  beqz  $at, .L7F04A7F4
/* 07F308 7F04A7D8 00408825 */   move  $s1, $v0
/* 07F30C 7F04A7DC 00026880 */  sll   $t5, $v0, 2
/* 07F310 7F04A7E0 8DEFD120 */  lw    $t7, %lo(monitorimages)($t7)
/* 07F314 7F04A7E4 01A26823 */  subu  $t5, $t5, $v0
/* 07F318 7F04A7E8 000D6880 */  sll   $t5, $t5, 2
/* 07F31C 7F04A7EC 10000001 */  b     .L7F04A7F4
/* 07F320 7F04A7F0 01AF8821 */   addu  $s1, $t5, $t7
.L7F04A7F4:
/* 07F324 7F04A7F4 122000A9 */  beqz  $s1, .L7F04AA9C
/* 07F328 7F04A7F8 3C013F00 */   li    $at, 0x3F000000 # 0.500000
/* 07F32C 7F04A7FC 44810000 */  mtc1  $at, $f0
/* 07F330 7F04A800 C6120010 */  lwc1  $f18, 0x10($s0)
/* 07F334 7F04A804 C60A0024 */  lwc1  $f10, 0x24($s0)
/* 07F338 7F04A808 C60C000C */  lwc1  $f12, 0xc($s0)
/* 07F33C 7F04A80C 46009582 */  mul.s $f22, $f18, $f0
/* 07F340 7F04A810 460CA032 */  c.eq.s $f20, $f12
/* 07F344 7F04A814 46005602 */  mul.s $f24, $f10, $f0
/* 07F348 7F04A818 4600B386 */  mov.s $f14, $f22
/* 07F34C 7F04A81C 45010015 */  bc1t  .L7F04A874
/* 07F350 7F04A820 4600C406 */   mov.s $f16, $f24
/* 07F354 7F04A824 E7AE0054 */  swc1  $f14, 0x54($sp)
/* 07F358 7F04A828 0FC15FA8 */  jal   cosf
/* 07F35C 7F04A82C E7B00050 */   swc1  $f16, 0x50($sp)
/* 07F360 7F04A830 3C018005 */  lui   $at, %hi(D_80052CB0)
/* 07F364 7F04A834 C43A2CB0 */  lwc1  $f26, %lo(D_80052CB0)($at)
/* 07F368 7F04A838 C60C000C */  lwc1  $f12, 0xc($s0)
/* 07F36C 7F04A83C 461A0502 */  mul.s $f20, $f0, $f26
/* 07F370 7F04A840 0FC15FAB */  jal   sinf
/* 07F374 7F04A844 00000000 */   nop
/* 07F378 7F04A848 4614B582 */  mul.s $f22, $f22, $f20
/* 07F37C 7F04A84C C7AE0054 */  lwc1  $f14, 0x54($sp)
/* 07F380 7F04A850 C7B00050 */  lwc1  $f16, 0x50($sp)
/* 07F384 7F04A854 461A0082 */  mul.s $f2, $f0, $f26
/* 07F388 7F04A858 00000000 */  nop
/* 07F38C 7F04A85C 4602C602 */  mul.s $f24, $f24, $f2
/* 07F390 7F04A860 00000000 */  nop
/* 07F394 7F04A864 46027382 */  mul.s $f14, $f14, $f2
/* 07F398 7F04A868 00000000 */  nop
/* 07F39C 7F04A86C 46148402 */  mul.s $f16, $f16, $f20
/* 07F3A0 7F04A870 00000000 */  nop
.L7F04A874:
/* 07F3A4 7F04A874 922E0004 */  lbu   $t6, 4($s1)
/* 07F3A8 7F04A878 3C014200 */  li    $at, 0x42000000 # 32.000000
/* 07F3AC 7F04A87C 44810000 */  mtc1  $at, $f0
/* 07F3B0 7F04A880 448E4000 */  mtc1  $t6, $f8
/* 07F3B4 7F04A884 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F3B8 7F04A888 05C10004 */  bgez  $t6, .L7F04A89C
/* 07F3BC 7F04A88C 468041A0 */   cvt.s.w $f6, $f8
/* 07F3C0 7F04A890 44819000 */  mtc1  $at, $f18
/* 07F3C4 7F04A894 00000000 */  nop
/* 07F3C8 7F04A898 46123180 */  add.s $f6, $f6, $f18
.L7F04A89C:
/* 07F3CC 7F04A89C C60A0038 */  lwc1  $f10, 0x38($s0)
/* 07F3D0 7F04A8A0 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F3D4 7F04A8A4 46165100 */  add.s $f4, $f10, $f22
/* 07F3D8 7F04A8A8 46043202 */  mul.s $f8, $f6, $f4
/* 07F3DC 7F04A8AC 00000000 */  nop
/* 07F3E0 7F04A8B0 46004482 */  mul.s $f18, $f8, $f0
/* 07F3E4 7F04A8B4 4600928D */  trunc.w.s $f10, $f18
/* 07F3E8 7F04A8B8 44085000 */  mfc1  $t0, $f10
/* 07F3EC 7F04A8BC 00000000 */  nop
/* 07F3F0 7F04A8C0 A6480008 */  sh    $t0, 8($s2)
/* 07F3F4 7F04A8C4 92390005 */  lbu   $t9, 5($s1)
/* 07F3F8 7F04A8C8 44993000 */  mtc1  $t9, $f6
/* 07F3FC 7F04A8CC 07210004 */  bgez  $t9, .L7F04A8E0
/* 07F400 7F04A8D0 46803120 */   cvt.s.w $f4, $f6
/* 07F404 7F04A8D4 44814000 */  mtc1  $at, $f8
/* 07F408 7F04A8D8 00000000 */  nop
/* 07F40C 7F04A8DC 46082100 */  add.s $f4, $f4, $f8
.L7F04A8E0:
/* 07F410 7F04A8E0 C612004C */  lwc1  $f18, 0x4c($s0)
/* 07F414 7F04A8E4 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F418 7F04A8E8 46189280 */  add.s $f10, $f18, $f24
/* 07F41C 7F04A8EC 460A2182 */  mul.s $f6, $f4, $f10
/* 07F420 7F04A8F0 00000000 */  nop
/* 07F424 7F04A8F4 46003202 */  mul.s $f8, $f6, $f0
/* 07F428 7F04A8F8 4600448D */  trunc.w.s $f18, $f8
/* 07F42C 7F04A8FC 440C9000 */  mfc1  $t4, $f18
/* 07F430 7F04A900 00000000 */  nop
/* 07F434 7F04A904 A64C000A */  sh    $t4, 0xa($s2)
/* 07F438 7F04A908 922A0004 */  lbu   $t2, 4($s1)
/* 07F43C 7F04A90C 448A2000 */  mtc1  $t2, $f4
/* 07F440 7F04A910 05410004 */  bgez  $t2, .L7F04A924
/* 07F444 7F04A914 468022A0 */   cvt.s.w $f10, $f4
/* 07F448 7F04A918 44813000 */  mtc1  $at, $f6
/* 07F44C 7F04A91C 00000000 */  nop
/* 07F450 7F04A920 46065280 */  add.s $f10, $f10, $f6
.L7F04A924:
/* 07F454 7F04A924 C6080038 */  lwc1  $f8, 0x38($s0)
/* 07F458 7F04A928 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F45C 7F04A92C 460E4481 */  sub.s $f18, $f8, $f14
/* 07F460 7F04A930 46125102 */  mul.s $f4, $f10, $f18
/* 07F464 7F04A934 00000000 */  nop
/* 07F468 7F04A938 46002182 */  mul.s $f6, $f4, $f0
/* 07F46C 7F04A93C 4600320D */  trunc.w.s $f8, $f6
/* 07F470 7F04A940 440D4000 */  mfc1  $t5, $f8
/* 07F474 7F04A944 00000000 */  nop
/* 07F478 7F04A948 A64D0018 */  sh    $t5, 0x18($s2)
/* 07F47C 7F04A94C 922F0005 */  lbu   $t7, 5($s1)
/* 07F480 7F04A950 448F5000 */  mtc1  $t7, $f10
/* 07F484 7F04A954 05E10004 */  bgez  $t7, .L7F04A968
/* 07F488 7F04A958 468054A0 */   cvt.s.w $f18, $f10
/* 07F48C 7F04A95C 44812000 */  mtc1  $at, $f4
/* 07F490 7F04A960 00000000 */  nop
/* 07F494 7F04A964 46049480 */  add.s $f18, $f18, $f4
.L7F04A968:
/* 07F498 7F04A968 C606004C */  lwc1  $f6, 0x4c($s0)
/* 07F49C 7F04A96C 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F4A0 7F04A970 46103200 */  add.s $f8, $f6, $f16
/* 07F4A4 7F04A974 46089282 */  mul.s $f10, $f18, $f8
/* 07F4A8 7F04A978 00000000 */  nop
/* 07F4AC 7F04A97C 46005102 */  mul.s $f4, $f10, $f0
/* 07F4B0 7F04A980 4600218D */  trunc.w.s $f6, $f4
/* 07F4B4 7F04A984 44183000 */  mfc1  $t8, $f6
/* 07F4B8 7F04A988 00000000 */  nop
/* 07F4BC 7F04A98C A658001A */  sh    $t8, 0x1a($s2)
/* 07F4C0 7F04A990 92280004 */  lbu   $t0, 4($s1)
/* 07F4C4 7F04A994 44889000 */  mtc1  $t0, $f18
/* 07F4C8 7F04A998 05010004 */  bgez  $t0, .L7F04A9AC
/* 07F4CC 7F04A99C 46809220 */   cvt.s.w $f8, $f18
/* 07F4D0 7F04A9A0 44815000 */  mtc1  $at, $f10
/* 07F4D4 7F04A9A4 00000000 */  nop
/* 07F4D8 7F04A9A8 460A4200 */  add.s $f8, $f8, $f10
.L7F04A9AC:
/* 07F4DC 7F04A9AC C6040038 */  lwc1  $f4, 0x38($s0)
/* 07F4E0 7F04A9B0 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F4E4 7F04A9B4 46162181 */  sub.s $f6, $f4, $f22
/* 07F4E8 7F04A9B8 46064482 */  mul.s $f18, $f8, $f6
/* 07F4EC 7F04A9BC 00000000 */  nop
/* 07F4F0 7F04A9C0 46009282 */  mul.s $f10, $f18, $f0
/* 07F4F4 7F04A9C4 4600510D */  trunc.w.s $f4, $f10
/* 07F4F8 7F04A9C8 44092000 */  mfc1  $t1, $f4
/* 07F4FC 7F04A9CC 00000000 */  nop
/* 07F500 7F04A9D0 A6490028 */  sh    $t1, 0x28($s2)
/* 07F504 7F04A9D4 922C0005 */  lbu   $t4, 5($s1)
/* 07F508 7F04A9D8 448C4000 */  mtc1  $t4, $f8
/* 07F50C 7F04A9DC 05810004 */  bgez  $t4, .L7F04A9F0
/* 07F510 7F04A9E0 468041A0 */   cvt.s.w $f6, $f8
/* 07F514 7F04A9E4 44819000 */  mtc1  $at, $f18
/* 07F518 7F04A9E8 00000000 */  nop
/* 07F51C 7F04A9EC 46123180 */  add.s $f6, $f6, $f18
.L7F04A9F0:
/* 07F520 7F04A9F0 C60A004C */  lwc1  $f10, 0x4c($s0)
/* 07F524 7F04A9F4 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F528 7F04A9F8 46185101 */  sub.s $f4, $f10, $f24
/* 07F52C 7F04A9FC 46043202 */  mul.s $f8, $f6, $f4
/* 07F530 7F04AA00 00000000 */  nop
/* 07F534 7F04AA04 46004482 */  mul.s $f18, $f8, $f0
/* 07F538 7F04AA08 4600928D */  trunc.w.s $f10, $f18
/* 07F53C 7F04AA0C 440B5000 */  mfc1  $t3, $f10
/* 07F540 7F04AA10 00000000 */  nop
/* 07F544 7F04AA14 A64B002A */  sh    $t3, 0x2a($s2)
/* 07F548 7F04AA18 922D0004 */  lbu   $t5, 4($s1)
/* 07F54C 7F04AA1C 448D3000 */  mtc1  $t5, $f6
/* 07F550 7F04AA20 05A10004 */  bgez  $t5, .L7F04AA34
/* 07F554 7F04AA24 46803120 */   cvt.s.w $f4, $f6
/* 07F558 7F04AA28 44814000 */  mtc1  $at, $f8
/* 07F55C 7F04AA2C 00000000 */  nop
/* 07F560 7F04AA30 46082100 */  add.s $f4, $f4, $f8
.L7F04AA34:
/* 07F564 7F04AA34 C6120038 */  lwc1  $f18, 0x38($s0)
/* 07F568 7F04AA38 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F56C 7F04AA3C 460E9280 */  add.s $f10, $f18, $f14
/* 07F570 7F04AA40 460A2182 */  mul.s $f6, $f4, $f10
/* 07F574 7F04AA44 00000000 */  nop
/* 07F578 7F04AA48 46003202 */  mul.s $f8, $f6, $f0
/* 07F57C 7F04AA4C 4600448D */  trunc.w.s $f18, $f8
/* 07F580 7F04AA50 440E9000 */  mfc1  $t6, $f18
/* 07F584 7F04AA54 00000000 */  nop
/* 07F588 7F04AA58 A64E0038 */  sh    $t6, 0x38($s2)
/* 07F58C 7F04AA5C 92380005 */  lbu   $t8, 5($s1)
/* 07F590 7F04AA60 44982000 */  mtc1  $t8, $f4
/* 07F594 7F04AA64 07010004 */  bgez  $t8, .L7F04AA78
/* 07F598 7F04AA68 468022A0 */   cvt.s.w $f10, $f4
/* 07F59C 7F04AA6C 44813000 */  mtc1  $at, $f6
/* 07F5A0 7F04AA70 00000000 */  nop
/* 07F5A4 7F04AA74 46065280 */  add.s $f10, $f10, $f6
.L7F04AA78:
/* 07F5A8 7F04AA78 C608004C */  lwc1  $f8, 0x4c($s0)
/* 07F5AC 7F04AA7C 46104481 */  sub.s $f18, $f8, $f16
/* 07F5B0 7F04AA80 46125102 */  mul.s $f4, $f10, $f18
/* 07F5B4 7F04AA84 00000000 */  nop
/* 07F5B8 7F04AA88 46002182 */  mul.s $f6, $f4, $f0
/* 07F5BC 7F04AA8C 4600320D */  trunc.w.s $f8, $f6
/* 07F5C0 7F04AA90 44194000 */  mfc1  $t9, $f8
/* 07F5C4 7F04AA94 00000000 */  nop
/* 07F5C8 7F04AA98 A659003A */  sh    $t9, 0x3a($s2)
.L7F04AA9C:
/* 07F5CC 7F04AA9C 92020060 */  lbu   $v0, 0x60($s0)
/* 07F5D0 7F04AAA0 240C0002 */  li    $t4, 2
/* 07F5D4 7F04AAA4 3C0DB700 */  lui   $t5, 0xb700
/* 07F5D8 7F04AAA8 A242003C */  sb    $v0, 0x3c($s2)
/* 07F5DC 7F04AAAC A242002C */  sb    $v0, 0x2c($s2)
/* 07F5E0 7F04AAB0 A242001C */  sb    $v0, 0x1c($s2)
/* 07F5E4 7F04AAB4 A242000C */  sb    $v0, 0xc($s2)
/* 07F5E8 7F04AAB8 92020063 */  lbu   $v0, 0x63($s0)
/* 07F5EC 7F04AABC 240F2000 */  li    $t7, 8192
/* 07F5F0 7F04AAC0 27A400BC */  addiu $a0, $sp, 0xbc
/* 07F5F4 7F04AAC4 A242003D */  sb    $v0, 0x3d($s2)
/* 07F5F8 7F04AAC8 A242002D */  sb    $v0, 0x2d($s2)
/* 07F5FC 7F04AACC A242001D */  sb    $v0, 0x1d($s2)
/* 07F600 7F04AAD0 A242000D */  sb    $v0, 0xd($s2)
/* 07F604 7F04AAD4 92030066 */  lbu   $v1, 0x66($s0)
/* 07F608 7F04AAD8 A243003E */  sb    $v1, 0x3e($s2)
/* 07F60C 7F04AADC A243002E */  sb    $v1, 0x2e($s2)
/* 07F610 7F04AAE0 A243001E */  sb    $v1, 0x1e($s2)
/* 07F614 7F04AAE4 A243000E */  sb    $v1, 0xe($s2)
/* 07F618 7F04AAE8 92020069 */  lbu   $v0, 0x69($s0)
/* 07F61C 7F04AAEC A242003F */  sb    $v0, 0x3f($s2)
/* 07F620 7F04AAF0 A242002F */  sb    $v0, 0x2f($s2)
/* 07F624 7F04AAF4 A242001F */  sb    $v0, 0x1f($s2)
/* 07F628 7F04AAF8 A242000F */  sb    $v0, 0xf($s2)
/* 07F62C 7F04AAFC 92090069 */  lbu   $t1, 0x69($s0)
/* 07F630 7F04AB00 292100FF */  slti  $at, $t1, 0xff
/* 07F634 7F04AB04 50200003 */  beql  $at, $zero, .L7F04AB14
/* 07F638 7F04AB08 8FAA00BC */   lw    $t2, 0xbc($sp)
/* 07F63C 7F04AB0C AFAC00C4 */  sw    $t4, 0xc4($sp)
/* 07F640 7F04AB10 8FAA00BC */  lw    $t2, 0xbc($sp)
.L7F04AB14:
/* 07F644 7F04AB14 240E0002 */  li    $t6, 2
/* 07F648 7F04AB18 02202825 */  move  $a1, $s1
/* 07F64C 7F04AB1C 254B0008 */  addiu $t3, $t2, 8
/* 07F650 7F04AB20 AFAB00BC */  sw    $t3, 0xbc($sp)
/* 07F654 7F04AB24 AD4F0004 */  sw    $t7, 4($t2)
/* 07F658 7F04AB28 AD4D0000 */  sw    $t5, ($t2)
/* 07F65C 7F04AB2C AFAE0010 */  sw    $t6, 0x10($sp)
/* 07F660 7F04AB30 8FA700C0 */  lw    $a3, 0xc0($sp)
/* 07F664 7F04AB34 0FC1DB5A */  jal   texSelect
/* 07F668 7F04AB38 8FA600C4 */   lw    $a2, 0xc4($sp)
/* 07F66C 7F04AB3C 8FB000BC */  lw    $s0, 0xbc($sp)
/* 07F670 7F04AB40 3C190102 */  lui   $t9, (0x01020040 >> 16) # lui $t9, 0x102
/* 07F674 7F04AB44 37390040 */  ori   $t9, (0x01020040 & 0xFFFF) # ori $t9, $t9, 0x40
/* 07F678 7F04AB48 26080008 */  addiu $t0, $s0, 8
/* 07F67C 7F04AB4C AFA800BC */  sw    $t0, 0xbc($sp)
/* 07F680 7F04AB50 AE190000 */  sw    $t9, ($s0)
/* 07F684 7F04AB54 8FA900B0 */  lw    $t1, 0xb0($sp)
/* 07F688 7F04AB58 0C003A2C */  jal   osVirtualToPhysical
/* 07F68C 7F04AB5C 8D24000C */   lw    $a0, 0xc($t1)
/* 07F690 7F04AB60 AE020004 */  sw    $v0, 4($s0)
/* 07F694 7F04AB64 8FB100BC */  lw    $s1, 0xbc($sp)
/* 07F698 7F04AB68 3C0BBC00 */  lui   $t3, (0xBC001006 >> 16) # lui $t3, 0xbc00
/* 07F69C 7F04AB6C 356B1006 */  ori   $t3, (0xBC001006 & 0xFFFF) # ori $t3, $t3, 0x1006
/* 07F6A0 7F04AB70 262A0008 */  addiu $t2, $s1, 8
/* 07F6A4 7F04AB74 AFAA00BC */  sw    $t2, 0xbc($sp)
/* 07F6A8 7F04AB78 02402025 */  move  $a0, $s2
/* 07F6AC 7F04AB7C 0C003A2C */  jal   osVirtualToPhysical
/* 07F6B0 7F04AB80 AE2B0000 */   sw    $t3, ($s1)
/* 07F6B4 7F04AB84 AE220004 */  sw    $v0, 4($s1)
/* 07F6B8 7F04AB88 8FAD00BC */  lw    $t5, 0xbc($sp)
/* 07F6BC 7F04AB8C 3C0E0430 */  lui   $t6, (0x04300040 >> 16) # lui $t6, 0x430
/* 07F6C0 7F04AB90 35CE0040 */  ori   $t6, (0x04300040 & 0xFFFF) # ori $t6, $t6, 0x40
/* 07F6C4 7F04AB94 25AF0008 */  addiu $t7, $t5, 8
/* 07F6C8 7F04AB98 AFAF00BC */  sw    $t7, 0xbc($sp)
/* 07F6CC 7F04AB9C 3C180400 */  lui   $t8, 0x400
/* 07F6D0 7F04ABA0 ADB80004 */  sw    $t8, 4($t5)
/* 07F6D4 7F04ABA4 ADAE0000 */  sw    $t6, ($t5)
/* 07F6D8 7F04ABA8 8FA800BC */  lw    $t0, 0xbc($sp)
/* 07F6DC 7F04ABAC 3C09B100 */  lui   $t1, (0xB1000032 >> 16) # lui $t1, 0xb100
/* 07F6E0 7F04ABB0 35290032 */  ori   $t1, (0xB1000032 & 0xFFFF) # ori $t1, $t1, 0x32
/* 07F6E4 7F04ABB4 25190008 */  addiu $t9, $t0, 8
/* 07F6E8 7F04ABB8 AFB900BC */  sw    $t9, 0xbc($sp)
/* 07F6EC 7F04ABBC 240C2010 */  li    $t4, 8208
/* 07F6F0 7F04ABC0 AD0C0004 */  sw    $t4, 4($t0)
/* 07F6F4 7F04ABC4 AD090000 */  sw    $t1, ($t0)
/* 07F6F8 7F04ABC8 8FAA00BC */  lw    $t2, 0xbc($sp)
/* 07F6FC 7F04ABCC 3C0DB800 */  lui   $t5, 0xb800
/* 07F700 7F04ABD0 3C0F0601 */  lui   $t7, 0x601
/* 07F704 7F04ABD4 254B0008 */  addiu $t3, $t2, 8
/* 07F708 7F04ABD8 AFAB00BC */  sw    $t3, 0xbc($sp)
/* 07F70C 7F04ABDC AD400004 */  sw    $zero, 4($t2)
/* 07F710 7F04ABE0 AD4D0000 */  sw    $t5, ($t2)
/* 07F714 7F04ABE4 8FA600A8 */  lw    $a2, 0xa8($sp)
/* 07F718 7F04ABE8 ACCF0000 */  sw    $t7, ($a2)
/* 07F71C 7F04ABEC 8FAE00BC */  lw    $t6, 0xbc($sp)
/* 07F720 7F04ABF0 ACCE0004 */  sw    $t6, 4($a2)
.L7F04ABF4:
/* 07F724 7F04ABF4 8FBF004C */  lw    $ra, 0x4c($sp)
.L7F04ABF8:
/* 07F728 7F04ABF8 8FA200BC */  lw    $v0, 0xbc($sp)
/* 07F72C 7F04ABFC D7B40020 */  ldc1  $f20, 0x20($sp)
/* 07F730 7F04AC00 D7B60028 */  ldc1  $f22, 0x28($sp)
/* 07F734 7F04AC04 D7B80030 */  ldc1  $f24, 0x30($sp)
/* 07F738 7F04AC08 D7BA0038 */  ldc1  $f26, 0x38($sp)
/* 07F73C 7F04AC0C 8FB00040 */  lw    $s0, 0x40($sp)
/* 07F740 7F04AC10 8FB10044 */  lw    $s1, 0x44($sp)
/* 07F744 7F04AC14 8FB20048 */  lw    $s2, 0x48($sp)
/* 07F748 7F04AC18 03E00008 */  jr    $ra
/* 07F74C 7F04AC1C 27BD00B0 */   addiu $sp, $sp, 0xb0
)
#endif
#ifdef VERSION_JP
GLOBAL_ASM(
.late_rodata
glabel D_80052C68
.word 0x40c90fdb /*6.2831855*/
/*D:80052C6C*/
glabel jpt_80052C6C
.word command00_reset_scroll_shift
.word command01_horizontal_scroll
.word command02_vertical_scroll
.word command03_horizontal_pos
.word command04_vertical_pos
.word command05_zoomx
.word command06_zoomy
.word command07_use_image_from_global_monitor_table
.word command08_halt_processing_for_time
.word command09_jump
.word command0A_jump_conditional
.word command0B_restart
.word command0C_break
.word command0D_colour_transition
.word command0E_set_rotation
.word command0F_rotate

glabel D_80052CAC
.word 0x3fc90fdb /*1.5707964*/
glabel D_80052CB0
.word 0x3fb50481 /*1.4141999*/
glabel D_80052CB4
.word 0x3f59999a /*0.85000002*/
glabel D_80052CB8
.word 0x3e19999a /*0.15000001*/
glabel D_80052CBC
.word 0x3e19999a /*0.15000001*/
glabel D_80052CC0
.word 0x3f666666 /*0.89999998*/
.text
glabel process_monitor_animation_microcode
/* 07EA78 7F049F48 27BDFF50 */  addiu $sp, $sp, -0xb0
/* 07EA7C 7F049F4C AFB10044 */  sw    $s1, 0x44($sp)
/* 07EA80 7F049F50 AFB00040 */  sw    $s0, 0x40($sp)
/* 07EA84 7F049F54 00C08025 */  move  $s0, $a2
/* 07EA88 7F049F58 00A08825 */  move  $s1, $a1
/* 07EA8C 7F049F5C AFBF004C */  sw    $ra, 0x4c($sp)
/* 07EA90 7F049F60 AFB20048 */  sw    $s2, 0x48($sp)
/* 07EA94 7F049F64 F7BA0038 */  sdc1  $f26, 0x38($sp)
/* 07EA98 7F049F68 F7B80030 */  sdc1  $f24, 0x30($sp)
/* 07EA9C 7F049F6C F7B60028 */  sdc1  $f22, 0x28($sp)
/* 07EAA0 7F049F70 F7B40020 */  sdc1  $f20, 0x20($sp)
/* 07EAA4 7F049F74 AFA400B0 */  sw    $a0, 0xb0($sp)
/* 07EAA8 7F049F78 10A0031E */  beqz  $a1, .L7F04ABF4
/* 07EAAC 7F049F7C AFA700BC */   sw    $a3, 0xbc($sp)
/* 07EAB0 7F049F80 94AE0000 */  lhu   $t6, ($a1)
/* 07EAB4 7F049F84 24010018 */  li    $at, 24
/* 07EAB8 7F049F88 31CF00FF */  andi  $t7, $t6, 0xff
/* 07EABC 7F049F8C 55E1031A */  bnel  $t7, $at, .L7F04ABF8
/* 07EAC0 7F049F90 8FBF004C */   lw    $ra, 0x4c($sp)
/* 07EAC4 7F049F94 0FC2F5B1 */  jal   dynAllocate7F0BD6C4
/* 07EAC8 7F049F98 24040004 */   li    $a0, 4
/* 07EACC 7F049F9C 8FB800BC */  lw    $t8, 0xbc($sp)
/* 07EAD0 7F049FA0 00409025 */  move  $s2, $v0
/* 07EAD4 7F049FA4 8FA400B0 */  lw    $a0, 0xb0($sp)
/* 07EAD8 7F049FA8 27190008 */  addiu $t9, $t8, 8
/* 07EADC 7F049FAC AFB900BC */  sw    $t9, 0xbc($sp)
/* 07EAE0 7F049FB0 AFB800A8 */  sw    $t8, 0xa8($sp)
/* 07EAE4 7F049FB4 8E280004 */  lw    $t0, 4($s1)
/* 07EAE8 7F049FB8 02202825 */  move  $a1, $s1
/* 07EAEC 7F049FBC 0FC1B1E7 */  jal   modelGetNodeRwData
/* 07EAF0 7F049FC0 AFA800A4 */   sw    $t0, 0xa4($sp)
/* 07EAF4 7F049FC4 3C018005 */  lui   $at, %hi(D_80052C68)
/* 07EAF8 7F049FC8 C43A2C68 */  lwc1  $f26, %lo(D_80052C68)($at)
/* 07EAFC 7F049FCC 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 07EB00 7F049FD0 4481C000 */  mtc1  $at, $f24
/* 07EB04 7F049FD4 3C013A80 */  li    $at, 0x3A800000 # 0.000977
/* 07EB08 7F049FD8 4481B000 */  mtc1  $at, $f22
/* 07EB0C 7F049FDC 3C014780 */  li    $at, 0x47800000 # 65536.000000
/* 07EB10 7F049FE0 44811000 */  mtc1  $at, $f2
/* 07EB14 7F049FE4 4480A000 */  mtc1  $zero, $f20
/* 07EB18 7F049FE8 AFA200A0 */  sw    $v0, 0xa0($sp)
/* 07EB1C 7F049FEC 00008825 */  move  $s1, $zero
/* 07EB20 7F049FF0 96020004 */  lhu   $v0, 4($s0)
.L7F049FF4:
/* 07EB24 7F049FF4 8E090000 */  lw    $t1, ($s0)
/* 07EB28 7F049FF8 00025080 */  sll   $t2, $v0, 2
/* 07EB2C 7F049FFC 012A1821 */  addu  $v1, $t1, $t2
/* 07EB30 7F04A000 8C6B0000 */  lw    $t3, ($v1)
/* 07EB34 7F04A004 2D610010 */  sltiu $at, $t3, 0x10
/* 07EB38 7F04A008 10200117 */  beqz  $at, .L7F04A468
/* 07EB3C 7F04A00C 000B5880 */   sll   $t3, $t3, 2
/* 07EB40 7F04A010 3C018005 */  lui   $at, %hi(jpt_80052C6C)
/* 07EB44 7F04A014 002B0821 */  addu  $at, $at, $t3
/* 07EB48 7F04A018 8C2B2C6C */  lw    $t3, %lo(jpt_80052C6C)($at)
/* 07EB4C 7F04A01C 01600008 */  jr    $t3
/* 07EB50 7F04A020 00000000 */   nop
command00_reset_scroll_shift:
/* 07EB54 7F04A024 244C0001 */  addiu $t4, $v0, 1
/* 07EB58 7F04A028 E6140040 */  swc1  $f20, 0x40($s0)
/* 07EB5C 7F04A02C E6140054 */  swc1  $f20, 0x54($s0)
/* 07EB60 7F04A030 1000010D */  b     .L7F04A468
/* 07EB64 7F04A034 A60C0004 */   sh    $t4, 4($s0)
command01_horizontal_scroll:
/* 07EB68 7F04A038 E614003C */  swc1  $f20, 0x3c($s0)
/* 07EB6C 7F04A03C 8C6D0008 */  lw    $t5, 8($v1)
/* 07EB70 7F04A040 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07EB74 7F04A044 448D2000 */  mtc1  $t5, $f4
/* 07EB78 7F04A048 05A10004 */  bgez  $t5, .L7F04A05C
/* 07EB7C 7F04A04C 468021A0 */   cvt.s.w $f6, $f4
/* 07EB80 7F04A050 44814000 */  mtc1  $at, $f8
/* 07EB84 7F04A054 00000000 */  nop
/* 07EB88 7F04A058 46083180 */  add.s $f6, $f6, $f8
.L7F04A05C:
/* 07EB8C 7F04A05C 4606C283 */  div.s $f10, $f24, $f6
/* 07EB90 7F04A060 C6000038 */  lwc1  $f0, 0x38($s0)
/* 07EB94 7F04A064 244F0003 */  addiu $t7, $v0, 3
/* 07EB98 7F04A068 E6000044 */  swc1  $f0, 0x44($s0)
/* 07EB9C 7F04A06C E60A0040 */  swc1  $f10, 0x40($s0)
/* 07EBA0 7F04A070 8C6E0004 */  lw    $t6, 4($v1)
/* 07EBA4 7F04A074 A60F0004 */  sh    $t7, 4($s0)
/* 07EBA8 7F04A078 448E9000 */  mtc1  $t6, $f18
/* 07EBAC 7F04A07C 00000000 */  nop
/* 07EBB0 7F04A080 46809120 */  cvt.s.w $f4, $f18
/* 07EBB4 7F04A084 46162202 */  mul.s $f8, $f4, $f22
/* 07EBB8 7F04A088 46080180 */  add.s $f6, $f0, $f8
/* 07EBBC 7F04A08C 100000F6 */  b     .L7F04A468
/* 07EBC0 7F04A090 E6060048 */   swc1  $f6, 0x48($s0)
command02_vertical_scroll:
/* 07EBC4 7F04A094 E6140050 */  swc1  $f20, 0x50($s0)
/* 07EBC8 7F04A098 8C780008 */  lw    $t8, 8($v1)
/* 07EBCC 7F04A09C 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07EBD0 7F04A0A0 44985000 */  mtc1  $t8, $f10
/* 07EBD4 7F04A0A4 07010004 */  bgez  $t8, .L7F04A0B8
/* 07EBD8 7F04A0A8 468054A0 */   cvt.s.w $f18, $f10
/* 07EBDC 7F04A0AC 44812000 */  mtc1  $at, $f4
/* 07EBE0 7F04A0B0 00000000 */  nop
/* 07EBE4 7F04A0B4 46049480 */  add.s $f18, $f18, $f4
.L7F04A0B8:
/* 07EBE8 7F04A0B8 4612C203 */  div.s $f8, $f24, $f18
/* 07EBEC 7F04A0BC C600004C */  lwc1  $f0, 0x4c($s0)
/* 07EBF0 7F04A0C0 24480003 */  addiu $t0, $v0, 3
/* 07EBF4 7F04A0C4 E6000058 */  swc1  $f0, 0x58($s0)
/* 07EBF8 7F04A0C8 E6080054 */  swc1  $f8, 0x54($s0)
/* 07EBFC 7F04A0CC 8C790004 */  lw    $t9, 4($v1)
/* 07EC00 7F04A0D0 A6080004 */  sh    $t0, 4($s0)
/* 07EC04 7F04A0D4 44993000 */  mtc1  $t9, $f6
/* 07EC08 7F04A0D8 00000000 */  nop
/* 07EC0C 7F04A0DC 468032A0 */  cvt.s.w $f10, $f6
/* 07EC10 7F04A0E0 46165102 */  mul.s $f4, $f10, $f22
/* 07EC14 7F04A0E4 46040480 */  add.s $f18, $f0, $f4
/* 07EC18 7F04A0E8 100000DF */  b     .L7F04A468
/* 07EC1C 7F04A0EC E612005C */   swc1  $f18, 0x5c($s0)
command03_horizontal_pos:
/* 07EC20 7F04A0F0 E614003C */  swc1  $f20, 0x3c($s0)
/* 07EC24 7F04A0F4 8C690008 */  lw    $t1, 8($v1)
/* 07EC28 7F04A0F8 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07EC2C 7F04A0FC 44894000 */  mtc1  $t1, $f8
/* 07EC30 7F04A100 05210004 */  bgez  $t1, .L7F04A114
/* 07EC34 7F04A104 468041A0 */   cvt.s.w $f6, $f8
/* 07EC38 7F04A108 44815000 */  mtc1  $at, $f10
/* 07EC3C 7F04A10C 00000000 */  nop
/* 07EC40 7F04A110 460A3180 */  add.s $f6, $f6, $f10
.L7F04A114:
/* 07EC44 7F04A114 4606C103 */  div.s $f4, $f24, $f6
/* 07EC48 7F04A118 C6120038 */  lwc1  $f18, 0x38($s0)
/* 07EC4C 7F04A11C 244B0003 */  addiu $t3, $v0, 3
/* 07EC50 7F04A120 E6120044 */  swc1  $f18, 0x44($s0)
/* 07EC54 7F04A124 E6040040 */  swc1  $f4, 0x40($s0)
/* 07EC58 7F04A128 8C6A0004 */  lw    $t2, 4($v1)
/* 07EC5C 7F04A12C A60B0004 */  sh    $t3, 4($s0)
/* 07EC60 7F04A130 448A4000 */  mtc1  $t2, $f8
/* 07EC64 7F04A134 00000000 */  nop
/* 07EC68 7F04A138 468042A0 */  cvt.s.w $f10, $f8
/* 07EC6C 7F04A13C 46165182 */  mul.s $f6, $f10, $f22
/* 07EC70 7F04A140 100000C9 */  b     .L7F04A468
/* 07EC74 7F04A144 E6060048 */   swc1  $f6, 0x48($s0)
command04_vertical_pos:
/* 07EC78 7F04A148 E6140050 */  swc1  $f20, 0x50($s0)
/* 07EC7C 7F04A14C 8C6C0008 */  lw    $t4, 8($v1)
/* 07EC80 7F04A150 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07EC84 7F04A154 448C2000 */  mtc1  $t4, $f4
/* 07EC88 7F04A158 05810004 */  bgez  $t4, .L7F04A16C
/* 07EC8C 7F04A15C 468024A0 */   cvt.s.w $f18, $f4
/* 07EC90 7F04A160 44814000 */  mtc1  $at, $f8
/* 07EC94 7F04A164 00000000 */  nop
/* 07EC98 7F04A168 46089480 */  add.s $f18, $f18, $f8
.L7F04A16C:
/* 07EC9C 7F04A16C 4612C283 */  div.s $f10, $f24, $f18
/* 07ECA0 7F04A170 C606004C */  lwc1  $f6, 0x4c($s0)
/* 07ECA4 7F04A174 244E0003 */  addiu $t6, $v0, 3
/* 07ECA8 7F04A178 E6060058 */  swc1  $f6, 0x58($s0)
/* 07ECAC 7F04A17C E60A0054 */  swc1  $f10, 0x54($s0)
/* 07ECB0 7F04A180 8C6D0004 */  lw    $t5, 4($v1)
/* 07ECB4 7F04A184 A60E0004 */  sh    $t6, 4($s0)
/* 07ECB8 7F04A188 448D2000 */  mtc1  $t5, $f4
/* 07ECBC 7F04A18C 00000000 */  nop
/* 07ECC0 7F04A190 46802220 */  cvt.s.w $f8, $f4
/* 07ECC4 7F04A194 46164482 */  mul.s $f18, $f8, $f22
/* 07ECC8 7F04A198 100000B3 */  b     .L7F04A468
/* 07ECCC 7F04A19C E612005C */   swc1  $f18, 0x5c($s0)
command05_zoomx:
/* 07ECD0 7F04A1A0 E6140014 */  swc1  $f20, 0x14($s0)
/* 07ECD4 7F04A1A4 8C6F0008 */  lw    $t7, 8($v1)
/* 07ECD8 7F04A1A8 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07ECDC 7F04A1AC 448F5000 */  mtc1  $t7, $f10
/* 07ECE0 7F04A1B0 05E10004 */  bgez  $t7, .L7F04A1C4
/* 07ECE4 7F04A1B4 468051A0 */   cvt.s.w $f6, $f10
/* 07ECE8 7F04A1B8 44812000 */  mtc1  $at, $f4
/* 07ECEC 7F04A1BC 00000000 */  nop
/* 07ECF0 7F04A1C0 46043180 */  add.s $f6, $f6, $f4
.L7F04A1C4:
/* 07ECF4 7F04A1C4 4606C203 */  div.s $f8, $f24, $f6
/* 07ECF8 7F04A1C8 C6120010 */  lwc1  $f18, 0x10($s0)
/* 07ECFC 7F04A1CC 24590003 */  addiu $t9, $v0, 3
/* 07ED00 7F04A1D0 E612001C */  swc1  $f18, 0x1c($s0)
/* 07ED04 7F04A1D4 E6080018 */  swc1  $f8, 0x18($s0)
/* 07ED08 7F04A1D8 8C780004 */  lw    $t8, 4($v1)
/* 07ED0C 7F04A1DC A6190004 */  sh    $t9, 4($s0)
/* 07ED10 7F04A1E0 44985000 */  mtc1  $t8, $f10
/* 07ED14 7F04A1E4 00000000 */  nop
/* 07ED18 7F04A1E8 46805120 */  cvt.s.w $f4, $f10
/* 07ED1C 7F04A1EC 46162182 */  mul.s $f6, $f4, $f22
/* 07ED20 7F04A1F0 1000009D */  b     .L7F04A468
/* 07ED24 7F04A1F4 E6060020 */   swc1  $f6, 0x20($s0)
command06_zoomy:
/* 07ED28 7F04A1F8 E6140028 */  swc1  $f20, 0x28($s0)
/* 07ED2C 7F04A1FC 8C680008 */  lw    $t0, 8($v1)
/* 07ED30 7F04A200 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07ED34 7F04A204 44884000 */  mtc1  $t0, $f8
/* 07ED38 7F04A208 05010004 */  bgez  $t0, .L7F04A21C
/* 07ED3C 7F04A20C 468044A0 */   cvt.s.w $f18, $f8
/* 07ED40 7F04A210 44815000 */  mtc1  $at, $f10
/* 07ED44 7F04A214 00000000 */  nop
/* 07ED48 7F04A218 460A9480 */  add.s $f18, $f18, $f10
.L7F04A21C:
/* 07ED4C 7F04A21C 4612C103 */  div.s $f4, $f24, $f18
/* 07ED50 7F04A220 C6060024 */  lwc1  $f6, 0x24($s0)
/* 07ED54 7F04A224 244A0003 */  addiu $t2, $v0, 3
/* 07ED58 7F04A228 E6060030 */  swc1  $f6, 0x30($s0)
/* 07ED5C 7F04A22C E604002C */  swc1  $f4, 0x2c($s0)
/* 07ED60 7F04A230 8C690004 */  lw    $t1, 4($v1)
/* 07ED64 7F04A234 A60A0004 */  sh    $t2, 4($s0)
/* 07ED68 7F04A238 44894000 */  mtc1  $t1, $f8
/* 07ED6C 7F04A23C 00000000 */  nop
/* 07ED70 7F04A240 468042A0 */  cvt.s.w $f10, $f8
/* 07ED74 7F04A244 46165482 */  mul.s $f18, $f10, $f22
/* 07ED78 7F04A248 10000087 */  b     .L7F04A468
/* 07ED7C 7F04A24C E6120034 */   swc1  $f18, 0x34($s0)
command07_use_image_from_global_monitor_table:
/* 07ED80 7F04A250 02002025 */  move  $a0, $s0
/* 07ED84 7F04A254 0FC127D0 */  jal   save_img_index_to_obj_ani_slot
/* 07ED88 7F04A258 8C650004 */   lw    $a1, 4($v1)
/* 07ED8C 7F04A25C 960B0004 */  lhu   $t3, 4($s0)
/* 07ED90 7F04A260 3C014780 */  li    $at, 0x47800000 # 65536.000000
/* 07ED94 7F04A264 44811000 */  mtc1  $at, $f2
/* 07ED98 7F04A268 256C0002 */  addiu $t4, $t3, 2
/* 07ED9C 7F04A26C 1000007E */  b     .L7F04A468
/* 07EDA0 7F04A270 A60C0004 */   sh    $t4, 4($s0)
command08_halt_processing_for_time:
/* 07EDA4 7F04A274 86020006 */  lh    $v0, 6($s0)
/* 07EDA8 7F04A278 3C0D8005 */  lui   $t5, %hi(g_ClockTimer)
/* 07EDAC 7F04A27C 0442000E */  bltzl $v0, .L7F04A2B8
/* 07EDB0 7F04A280 8C680004 */   lw    $t0, 4($v1)
/* 07EDB4 7F04A284 8DAD8374 */  lw    $t5, %lo(g_ClockTimer)($t5)
/* 07EDB8 7F04A288 004D7023 */  subu  $t6, $v0, $t5
/* 07EDBC 7F04A28C A60E0006 */  sh    $t6, 6($s0)
/* 07EDC0 7F04A290 860F0006 */  lh    $t7, 6($s0)
/* 07EDC4 7F04A294 05E20004 */  bltzl $t7, .L7F04A2A8
/* 07EDC8 7F04A298 96180004 */   lhu   $t8, 4($s0)
/* 07EDCC 7F04A29C 10000072 */  b     .L7F04A468
/* 07EDD0 7F04A2A0 24110001 */   li    $s1, 1
/* 07EDD4 7F04A2A4 96180004 */  lhu   $t8, 4($s0)
.L7F04A2A8:
/* 07EDD8 7F04A2A8 27190002 */  addiu $t9, $t8, 2
/* 07EDDC 7F04A2AC 1000006E */  b     .L7F04A468
/* 07EDE0 7F04A2B0 A6190004 */   sh    $t9, 4($s0)
/* 07EDE4 7F04A2B4 8C680004 */  lw    $t0, 4($v1)
.L7F04A2B8:
/* 07EDE8 7F04A2B8 24110001 */  li    $s1, 1
/* 07EDEC 7F04A2BC 1000006A */  b     .L7F04A468
/* 07EDF0 7F04A2C0 A6080006 */   sh    $t0, 6($s0)
command09_jump:
/* 07EDF4 7F04A2C4 02002025 */  move  $a0, $s0
/* 07EDF8 7F04A2C8 0FC12723 */  jal   save_ptr_monitor_ani_code_to_obj_ani_slot
/* 07EDFC 7F04A2CC 8C650004 */   lw    $a1, 4($v1)
/* 07EE00 7F04A2D0 3C014780 */  li    $at, 0x47800000 # 65536.000000
/* 07EE04 7F04A2D4 44811000 */  mtc1  $at, $f2
/* 07EE08 7F04A2D8 10000063 */  b     .L7F04A468
/* 07EE0C 7F04A2DC 00000000 */   nop
command0A_jump_conditional:
/* 07EE10 7F04A2E0 0C002914 */  jal   randomGetNext
/* 07EE14 7F04A2E4 AFA30094 */   sw    $v1, 0x94($sp)
/* 07EE18 7F04A2E8 8FA30094 */  lw    $v1, 0x94($sp)
/* 07EE1C 7F04A2EC 00024C02 */  srl   $t1, $v0, 0x10
/* 07EE20 7F04A2F0 02002025 */  move  $a0, $s0
/* 07EE24 7F04A2F4 8C6A0008 */  lw    $t2, 8($v1)
/* 07EE28 7F04A2F8 012A082B */  sltu  $at, $t1, $t2
/* 07EE2C 7F04A2FC 50200008 */  beql  $at, $zero, .L7F04A320
/* 07EE30 7F04A300 960B0004 */   lhu   $t3, 4($s0)
/* 07EE34 7F04A304 0FC12723 */  jal   save_ptr_monitor_ani_code_to_obj_ani_slot
/* 07EE38 7F04A308 8C650004 */   lw    $a1, 4($v1)
/* 07EE3C 7F04A30C 3C014780 */  li    $at, 0x47800000 # 65536.000000
/* 07EE40 7F04A310 44811000 */  mtc1  $at, $f2
/* 07EE44 7F04A314 10000054 */  b     .L7F04A468
/* 07EE48 7F04A318 00000000 */   nop
/* 07EE4C 7F04A31C 960B0004 */  lhu   $t3, 4($s0)
.L7F04A320:
/* 07EE50 7F04A320 3C014780 */  li    $at, 0x47800000 # 65536.000000
/* 07EE54 7F04A324 44811000 */  mtc1  $at, $f2
/* 07EE58 7F04A328 256C0003 */  addiu $t4, $t3, 3
/* 07EE5C 7F04A32C A60C0004 */  sh    $t4, 4($s0)
/* 07EE60 7F04A330 1000004D */  b     .L7F04A468
/* 07EE64 7F04A334 00000000 */   nop
command0B_restart:
/* 07EE68 7F04A338 1000004B */  b     .L7F04A468
/* 07EE6C 7F04A33C A6000004 */   sh    $zero, 4($s0)
command0C_break:
/* 07EE70 7F04A340 10000049 */  b     .L7F04A468
/* 07EE74 7F04A344 24110001 */   li    $s1, 1
command0D_colour_transition:
/* 07EE78 7F04A348 E614006C */  swc1  $f20, 0x6c($s0)
/* 07EE7C 7F04A34C 8C6D0008 */  lw    $t5, 8($v1)
/* 07EE80 7F04A350 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07EE84 7F04A354 448D2000 */  mtc1  $t5, $f4
/* 07EE88 7F04A358 05A10004 */  bgez  $t5, .L7F04A36C
/* 07EE8C 7F04A35C 468021A0 */   cvt.s.w $f6, $f4
/* 07EE90 7F04A360 44814000 */  mtc1  $at, $f8
/* 07EE94 7F04A364 00000000 */  nop
/* 07EE98 7F04A368 46083180 */  add.s $f6, $f6, $f8
.L7F04A36C:
/* 07EE9C 7F04A36C 4606C283 */  div.s $f10, $f24, $f6
/* 07EEA0 7F04A370 920E0060 */  lbu   $t6, 0x60($s0)
/* 07EEA4 7F04A374 92080063 */  lbu   $t0, 0x63($s0)
/* 07EEA8 7F04A378 920C0066 */  lbu   $t4, 0x66($s0)
/* 07EEAC 7F04A37C A20E0061 */  sb    $t6, 0x61($s0)
/* 07EEB0 7F04A380 92180069 */  lbu   $t8, 0x69($s0)
/* 07EEB4 7F04A384 E60A0070 */  swc1  $f10, 0x70($s0)
/* 07EEB8 7F04A388 8C6F0004 */  lw    $t7, 4($v1)
/* 07EEBC 7F04A38C A2080064 */  sb    $t0, 0x64($s0)
/* 07EEC0 7F04A390 000FCE02 */  srl   $t9, $t7, 0x18
/* 07EEC4 7F04A394 A2190062 */  sb    $t9, 0x62($s0)
/* 07EEC8 7F04A398 8C690004 */  lw    $t1, 4($v1)
/* 07EECC 7F04A39C A20C0067 */  sb    $t4, 0x67($s0)
/* 07EED0 7F04A3A0 00095C02 */  srl   $t3, $t1, 0x10
/* 07EED4 7F04A3A4 A20B0065 */  sb    $t3, 0x65($s0)
/* 07EED8 7F04A3A8 8C6D0004 */  lw    $t5, 4($v1)
/* 07EEDC 7F04A3AC A218006A */  sb    $t8, 0x6a($s0)
/* 07EEE0 7F04A3B0 24490003 */  addiu $t1, $v0, 3
/* 07EEE4 7F04A3B4 000D7A02 */  srl   $t7, $t5, 8
/* 07EEE8 7F04A3B8 A20F0068 */  sb    $t7, 0x68($s0)
/* 07EEEC 7F04A3BC 8C680004 */  lw    $t0, 4($v1)
/* 07EEF0 7F04A3C0 A6090004 */  sh    $t1, 4($s0)
/* 07EEF4 7F04A3C4 10000028 */  b     .L7F04A468
/* 07EEF8 7F04A3C8 A208006B */   sb    $t0, 0x6b($s0)
command0E_set_rotation:
/* 07EEFC 7F04A3CC 8C6A0004 */  lw    $t2, 4($v1)
/* 07EF00 7F04A3D0 244B0002 */  addiu $t3, $v0, 2
/* 07EF04 7F04A3D4 A60B0004 */  sh    $t3, 4($s0)
/* 07EF08 7F04A3D8 448A9000 */  mtc1  $t2, $f18
/* 07EF0C 7F04A3DC 00000000 */  nop
/* 07EF10 7F04A3E0 46809120 */  cvt.s.w $f4, $f18
/* 07EF14 7F04A3E4 461A2202 */  mul.s $f8, $f4, $f26
/* 07EF18 7F04A3E8 46024183 */  div.s $f6, $f8, $f2
/* 07EF1C 7F04A3EC 1000001E */  b     .L7F04A468
/* 07EF20 7F04A3F0 E606000C */   swc1  $f6, 0xc($s0)
command0F_rotate:
/* 07EF24 7F04A3F4 8C6C0004 */  lw    $t4, 4($v1)
/* 07EF28 7F04A3F8 3C018005 */  lui   $at, %hi(g_JP_GlobalTimerDelta)
/* 07EF2C 7F04A3FC C42A8378 */  lwc1  $f10, %lo(g_JP_GlobalTimerDelta)($at)
/* 07EF30 7F04A400 448C9000 */  mtc1  $t4, $f18
/* 07EF34 7F04A404 00000000 */  nop
/* 07EF38 7F04A408 46809120 */  cvt.s.w $f4, $f18
/* 07EF3C 7F04A40C 46045202 */  mul.s $f8, $f10, $f4
/* 07EF40 7F04A410 C60A000C */  lwc1  $f10, 0xc($s0)
/* 07EF44 7F04A414 461A4182 */  mul.s $f6, $f8, $f26
/* 07EF48 7F04A418 46023483 */  div.s $f18, $f6, $f2
/* 07EF4C 7F04A41C 46125100 */  add.s $f4, $f10, $f18
/* 07EF50 7F04A420 E604000C */  swc1  $f4, 0xc($s0)
/* 07EF54 7F04A424 C60C000C */  lwc1  $f12, 0xc($s0)
/* 07EF58 7F04A428 460CD03E */  c.le.s $f26, $f12
/* 07EF5C 7F04A42C 00000000 */  nop
/* 07EF60 7F04A430 45020005 */  bc1fl .L7F04A448
/* 07EF64 7F04A434 4614603C */   c.lt.s $f12, $f20
/* 07EF68 7F04A438 461A6201 */  sub.s $f8, $f12, $f26
/* 07EF6C 7F04A43C E608000C */  swc1  $f8, 0xc($s0)
/* 07EF70 7F04A440 C60C000C */  lwc1  $f12, 0xc($s0)
/* 07EF74 7F04A444 4614603C */  c.lt.s $f12, $f20
.L7F04A448:
/* 07EF78 7F04A448 00000000 */  nop
/* 07EF7C 7F04A44C 45020004 */  bc1fl .L7F04A460
/* 07EF80 7F04A450 960D0004 */   lhu   $t5, 4($s0)
/* 07EF84 7F04A454 461A6180 */  add.s $f6, $f12, $f26
/* 07EF88 7F04A458 E606000C */  swc1  $f6, 0xc($s0)
/* 07EF8C 7F04A45C 960D0004 */  lhu   $t5, 4($s0)
.L7F04A460:
/* 07EF90 7F04A460 25AE0002 */  addiu $t6, $t5, 2
/* 07EF94 7F04A464 A60E0004 */  sh    $t6, 4($s0)
def_7F04A01C:
.L7F04A468:
/* 07EF98 7F04A468 5220FEE2 */  beql  $s1, $zero, .L7F049FF4
/* 07EF9C 7F04A46C 96020004 */   lhu   $v0, 4($s0)
/* 07EFA0 7F04A470 C6000018 */  lwc1  $f0, 0x18($s0)
/* 07EFA4 7F04A474 4600A03C */  c.lt.s $f20, $f0
/* 07EFA8 7F04A478 00000000 */  nop
/* 07EFAC 7F04A47C 45000017 */  bc1f  .L7F04A4DC
/* 07EFB0 7F04A480 3C028005 */   lui   $v0, %hi(g_JP_GlobalTimerDelta)
/* 07EFB4 7F04A484 24428378 */  addiu $v0, %lo(g_JP_GlobalTimerDelta) # addiu $v0, $v0, -0x7c88
/* 07EFB8 7F04A488 C4520000 */  lwc1  $f18, ($v0)
/* 07EFBC 7F04A48C C60A0014 */  lwc1  $f10, 0x14($s0)
/* 07EFC0 7F04A490 46120102 */  mul.s $f4, $f0, $f18
/* 07EFC4 7F04A494 46045200 */  add.s $f8, $f10, $f4
/* 07EFC8 7F04A498 E6080014 */  swc1  $f8, 0x14($s0)
/* 07EFCC 7F04A49C C6020014 */  lwc1  $f2, 0x14($s0)
/* 07EFD0 7F04A4A0 4618103C */  c.lt.s $f2, $f24
/* 07EFD4 7F04A4A4 00000000 */  nop
/* 07EFD8 7F04A4A8 45020009 */  bc1fl .L7F04A4D0
/* 07EFDC 7F04A4AC C6080020 */   lwc1  $f8, 0x20($s0)
/* 07EFE0 7F04A4B0 C600001C */  lwc1  $f0, 0x1c($s0)
/* 07EFE4 7F04A4B4 C6060020 */  lwc1  $f6, 0x20($s0)
/* 07EFE8 7F04A4B8 46003481 */  sub.s $f18, $f6, $f0
/* 07EFEC 7F04A4BC 46029282 */  mul.s $f10, $f18, $f2
/* 07EFF0 7F04A4C0 460A0100 */  add.s $f4, $f0, $f10
/* 07EFF4 7F04A4C4 10000005 */  b     .L7F04A4DC
/* 07EFF8 7F04A4C8 E6040010 */   swc1  $f4, 0x10($s0)
/* 07EFFC 7F04A4CC C6080020 */  lwc1  $f8, 0x20($s0)
.L7F04A4D0:
/* 07F000 7F04A4D0 E6180014 */  swc1  $f24, 0x14($s0)
/* 07F004 7F04A4D4 E6140018 */  swc1  $f20, 0x18($s0)
/* 07F008 7F04A4D8 E6080010 */  swc1  $f8, 0x10($s0)
.L7F04A4DC:
/* 07F00C 7F04A4DC C600002C */  lwc1  $f0, 0x2c($s0)
/* 07F010 7F04A4E0 3C028005 */  lui   $v0, %hi(g_JP_GlobalTimerDelta)
/* 07F014 7F04A4E4 24428378 */  addiu $v0, %lo(g_JP_GlobalTimerDelta) # addiu $v0, $v0, -0x7c88
/* 07F018 7F04A4E8 4600A03C */  c.lt.s $f20, $f0
/* 07F01C 7F04A4EC 00000000 */  nop
/* 07F020 7F04A4F0 45020017 */  bc1fl .L7F04A550
/* 07F024 7F04A4F4 C6000040 */   lwc1  $f0, 0x40($s0)
/* 07F028 7F04A4F8 C4520000 */  lwc1  $f18, ($v0)
/* 07F02C 7F04A4FC C6060028 */  lwc1  $f6, 0x28($s0)
/* 07F030 7F04A500 46120282 */  mul.s $f10, $f0, $f18
/* 07F034 7F04A504 460A3100 */  add.s $f4, $f6, $f10
/* 07F038 7F04A508 E6040028 */  swc1  $f4, 0x28($s0)
/* 07F03C 7F04A50C C6020028 */  lwc1  $f2, 0x28($s0)
/* 07F040 7F04A510 4618103C */  c.lt.s $f2, $f24
/* 07F044 7F04A514 00000000 */  nop
/* 07F048 7F04A518 45020009 */  bc1fl .L7F04A540
/* 07F04C 7F04A51C C6040034 */   lwc1  $f4, 0x34($s0)
/* 07F050 7F04A520 C6000030 */  lwc1  $f0, 0x30($s0)
/* 07F054 7F04A524 C6080034 */  lwc1  $f8, 0x34($s0)
/* 07F058 7F04A528 46004481 */  sub.s $f18, $f8, $f0
/* 07F05C 7F04A52C 46029182 */  mul.s $f6, $f18, $f2
/* 07F060 7F04A530 46060280 */  add.s $f10, $f0, $f6
/* 07F064 7F04A534 10000005 */  b     .L7F04A54C
/* 07F068 7F04A538 E60A0024 */   swc1  $f10, 0x24($s0)
/* 07F06C 7F04A53C C6040034 */  lwc1  $f4, 0x34($s0)
.L7F04A540:
/* 07F070 7F04A540 E6180028 */  swc1  $f24, 0x28($s0)
/* 07F074 7F04A544 E614002C */  swc1  $f20, 0x2c($s0)
/* 07F078 7F04A548 E6040024 */  swc1  $f4, 0x24($s0)
.L7F04A54C:
/* 07F07C 7F04A54C C6000040 */  lwc1  $f0, 0x40($s0)
.L7F04A550:
/* 07F080 7F04A550 4600A03C */  c.lt.s $f20, $f0
/* 07F084 7F04A554 00000000 */  nop
/* 07F088 7F04A558 45020017 */  bc1fl .L7F04A5B8
/* 07F08C 7F04A55C C6000054 */   lwc1  $f0, 0x54($s0)
/* 07F090 7F04A560 C4520000 */  lwc1  $f18, ($v0)
/* 07F094 7F04A564 C608003C */  lwc1  $f8, 0x3c($s0)
/* 07F098 7F04A568 46120182 */  mul.s $f6, $f0, $f18
/* 07F09C 7F04A56C 46064280 */  add.s $f10, $f8, $f6
/* 07F0A0 7F04A570 E60A003C */  swc1  $f10, 0x3c($s0)
/* 07F0A4 7F04A574 C602003C */  lwc1  $f2, 0x3c($s0)
/* 07F0A8 7F04A578 4618103C */  c.lt.s $f2, $f24
/* 07F0AC 7F04A57C 00000000 */  nop
/* 07F0B0 7F04A580 45020009 */  bc1fl .L7F04A5A8
/* 07F0B4 7F04A584 C60A0048 */   lwc1  $f10, 0x48($s0)
/* 07F0B8 7F04A588 C6000044 */  lwc1  $f0, 0x44($s0)
/* 07F0BC 7F04A58C C6040048 */  lwc1  $f4, 0x48($s0)
/* 07F0C0 7F04A590 46002481 */  sub.s $f18, $f4, $f0
/* 07F0C4 7F04A594 46029202 */  mul.s $f8, $f18, $f2
/* 07F0C8 7F04A598 46080180 */  add.s $f6, $f0, $f8
/* 07F0CC 7F04A59C 10000005 */  b     .L7F04A5B4
/* 07F0D0 7F04A5A0 E6060038 */   swc1  $f6, 0x38($s0)
/* 07F0D4 7F04A5A4 C60A0048 */  lwc1  $f10, 0x48($s0)
.L7F04A5A8:
/* 07F0D8 7F04A5A8 E618003C */  swc1  $f24, 0x3c($s0)
/* 07F0DC 7F04A5AC E6140040 */  swc1  $f20, 0x40($s0)
/* 07F0E0 7F04A5B0 E60A0038 */  swc1  $f10, 0x38($s0)
.L7F04A5B4:
/* 07F0E4 7F04A5B4 C6000054 */  lwc1  $f0, 0x54($s0)
.L7F04A5B8:
/* 07F0E8 7F04A5B8 4600A03C */  c.lt.s $f20, $f0
/* 07F0EC 7F04A5BC 00000000 */  nop
/* 07F0F0 7F04A5C0 45020017 */  bc1fl .L7F04A620
/* 07F0F4 7F04A5C4 C6020070 */   lwc1  $f2, 0x70($s0)
/* 07F0F8 7F04A5C8 C4520000 */  lwc1  $f18, ($v0)
/* 07F0FC 7F04A5CC C6040050 */  lwc1  $f4, 0x50($s0)
/* 07F100 7F04A5D0 46120202 */  mul.s $f8, $f0, $f18
/* 07F104 7F04A5D4 46082180 */  add.s $f6, $f4, $f8
/* 07F108 7F04A5D8 E6060050 */  swc1  $f6, 0x50($s0)
/* 07F10C 7F04A5DC C6020050 */  lwc1  $f2, 0x50($s0)
/* 07F110 7F04A5E0 4618103C */  c.lt.s $f2, $f24
/* 07F114 7F04A5E4 00000000 */  nop
/* 07F118 7F04A5E8 45020009 */  bc1fl .L7F04A610
/* 07F11C 7F04A5EC C606005C */   lwc1  $f6, 0x5c($s0)
/* 07F120 7F04A5F0 C6000058 */  lwc1  $f0, 0x58($s0)
/* 07F124 7F04A5F4 C60A005C */  lwc1  $f10, 0x5c($s0)
/* 07F128 7F04A5F8 46005481 */  sub.s $f18, $f10, $f0
/* 07F12C 7F04A5FC 46029102 */  mul.s $f4, $f18, $f2
/* 07F130 7F04A600 46040200 */  add.s $f8, $f0, $f4
/* 07F134 7F04A604 10000005 */  b     .L7F04A61C
/* 07F138 7F04A608 E608004C */   swc1  $f8, 0x4c($s0)
/* 07F13C 7F04A60C C606005C */  lwc1  $f6, 0x5c($s0)
.L7F04A610:
/* 07F140 7F04A610 E6180050 */  swc1  $f24, 0x50($s0)
/* 07F144 7F04A614 E6140054 */  swc1  $f20, 0x54($s0)
/* 07F148 7F04A618 E606004C */  swc1  $f6, 0x4c($s0)
.L7F04A61C:
/* 07F14C 7F04A61C C6020070 */  lwc1  $f2, 0x70($s0)
.L7F04A620:
/* 07F150 7F04A620 4602A03C */  c.lt.s $f20, $f2
/* 07F154 7F04A624 00000000 */  nop
/* 07F158 7F04A628 4502003F */  bc1fl .L7F04A728
/* 07F15C 7F04A62C 8FAA00A0 */   lw    $t2, 0xa0($sp)
/* 07F160 7F04A630 C4520000 */  lwc1  $f18, ($v0)
/* 07F164 7F04A634 C60A006C */  lwc1  $f10, 0x6c($s0)
/* 07F168 7F04A638 46121102 */  mul.s $f4, $f2, $f18
/* 07F16C 7F04A63C 46045200 */  add.s $f8, $f10, $f4
/* 07F170 7F04A640 E608006C */  swc1  $f8, 0x6c($s0)
/* 07F174 7F04A644 C600006C */  lwc1  $f0, 0x6c($s0)
/* 07F178 7F04A648 4618003C */  c.lt.s $f0, $f24
/* 07F17C 7F04A64C 00000000 */  nop
/* 07F180 7F04A650 4502002B */  bc1fl .L7F04A700
/* 07F184 7F04A654 920F0062 */   lbu   $t7, 0x62($s0)
/* 07F188 7F04A658 92020061 */  lbu   $v0, 0x61($s0)
/* 07F18C 7F04A65C 920F0062 */  lbu   $t7, 0x62($s0)
/* 07F190 7F04A660 92030064 */  lbu   $v1, 0x64($s0)
/* 07F194 7F04A664 920A0065 */  lbu   $t2, 0x65($s0)
/* 07F198 7F04A668 01E2C023 */  subu  $t8, $t7, $v0
/* 07F19C 7F04A66C 44983000 */  mtc1  $t8, $f6
/* 07F1A0 7F04A670 01435823 */  subu  $t3, $t2, $v1
/* 07F1A4 7F04A674 448B4000 */  mtc1  $t3, $f8
/* 07F1A8 7F04A678 468034A0 */  cvt.s.w $f18, $f6
/* 07F1AC 7F04A67C 920F0068 */  lbu   $t7, 0x68($s0)
/* 07F1B0 7F04A680 92040067 */  lbu   $a0, 0x67($s0)
/* 07F1B4 7F04A684 920A006B */  lbu   $t2, 0x6b($s0)
/* 07F1B8 7F04A688 9205006A */  lbu   $a1, 0x6a($s0)
/* 07F1BC 7F04A68C 468041A0 */  cvt.s.w $f6, $f8
/* 07F1C0 7F04A690 46009282 */  mul.s $f10, $f18, $f0
/* 07F1C4 7F04A694 01E4C023 */  subu  $t8, $t7, $a0
/* 07F1C8 7F04A698 01455823 */  subu  $t3, $t2, $a1
/* 07F1CC 7F04A69C 46003482 */  mul.s $f18, $f6, $f0
/* 07F1D0 7F04A6A0 4600510D */  trunc.w.s $f4, $f10
/* 07F1D4 7F04A6A4 4600928D */  trunc.w.s $f10, $f18
/* 07F1D8 7F04A6A8 44082000 */  mfc1  $t0, $f4
/* 07F1DC 7F04A6AC 44982000 */  mtc1  $t8, $f4
/* 07F1E0 7F04A6B0 440D5000 */  mfc1  $t5, $f10
/* 07F1E4 7F04A6B4 46802220 */  cvt.s.w $f8, $f4
/* 07F1E8 7F04A6B8 448B5000 */  mtc1  $t3, $f10
/* 07F1EC 7F04A6BC 00484821 */  addu  $t1, $v0, $t0
/* 07F1F0 7F04A6C0 006D7021 */  addu  $t6, $v1, $t5
/* 07F1F4 7F04A6C4 A2090060 */  sb    $t1, 0x60($s0)
/* 07F1F8 7F04A6C8 46805120 */  cvt.s.w $f4, $f10
/* 07F1FC 7F04A6CC 46004182 */  mul.s $f6, $f8, $f0
/* 07F200 7F04A6D0 A20E0063 */  sb    $t6, 0x63($s0)
/* 07F204 7F04A6D4 46002202 */  mul.s $f8, $f4, $f0
/* 07F208 7F04A6D8 4600348D */  trunc.w.s $f18, $f6
/* 07F20C 7F04A6DC 4600418D */  trunc.w.s $f6, $f8
/* 07F210 7F04A6E0 44089000 */  mfc1  $t0, $f18
/* 07F214 7F04A6E4 440D3000 */  mfc1  $t5, $f6
/* 07F218 7F04A6E8 00884821 */  addu  $t1, $a0, $t0
/* 07F21C 7F04A6EC A2090066 */  sb    $t1, 0x66($s0)
/* 07F220 7F04A6F0 00AD7021 */  addu  $t6, $a1, $t5
/* 07F224 7F04A6F4 1000000B */  b     .L7F04A724
/* 07F228 7F04A6F8 A20E0069 */   sb    $t6, 0x69($s0)
/* 07F22C 7F04A6FC 920F0062 */  lbu   $t7, 0x62($s0)
.L7F04A700:
/* 07F230 7F04A700 92180065 */  lbu   $t8, 0x65($s0)
/* 07F234 7F04A704 92190068 */  lbu   $t9, 0x68($s0)
/* 07F238 7F04A708 9208006B */  lbu   $t0, 0x6b($s0)
/* 07F23C 7F04A70C E618006C */  swc1  $f24, 0x6c($s0)
/* 07F240 7F04A710 E6140070 */  swc1  $f20, 0x70($s0)
/* 07F244 7F04A714 A20F0060 */  sb    $t7, 0x60($s0)
/* 07F248 7F04A718 A2180063 */  sb    $t8, 0x63($s0)
/* 07F24C 7F04A71C A2190066 */  sb    $t9, 0x66($s0)
/* 07F250 7F04A720 A2080069 */  sb    $t0, 0x69($s0)
.L7F04A724:
/* 07F254 7F04A724 8FAA00A0 */  lw    $t2, 0xa0($sp)
.L7F04A728:
/* 07F258 7F04A728 8FA300A4 */  lw    $v1, 0xa4($sp)
/* 07F25C 7F04A72C 8FA900BC */  lw    $t1, 0xbc($sp)
/* 07F260 7F04A730 AD520000 */  sw    $s2, ($t2)
/* 07F264 7F04A734 3C0F8009 */  lui   $t7, %hi(monitorimages)
/* 07F268 7F04A738 AD490004 */  sw    $t1, 4($t2)
/* 07F26C 7F04A73C 8C6B0008 */  lw    $t3, 8($v1)
/* 07F270 7F04A740 8D610000 */  lw    $at, ($t3)
/* 07F274 7F04A744 AE410000 */  sw    $at, ($s2)
/* 07F278 7F04A748 8D6D0004 */  lw    $t5, 4($t3)
/* 07F27C 7F04A74C AE4D0004 */  sw    $t5, 4($s2)
/* 07F280 7F04A750 8D610008 */  lw    $at, 8($t3)
/* 07F284 7F04A754 AE410008 */  sw    $at, 8($s2)
/* 07F288 7F04A758 8D6D000C */  lw    $t5, 0xc($t3)
/* 07F28C 7F04A75C AE4D000C */  sw    $t5, 0xc($s2)
/* 07F290 7F04A760 8C6E0008 */  lw    $t6, 8($v1)
/* 07F294 7F04A764 8DC10010 */  lw    $at, 0x10($t6)
/* 07F298 7F04A768 AE410010 */  sw    $at, 0x10($s2)
/* 07F29C 7F04A76C 8DD80014 */  lw    $t8, 0x14($t6)
/* 07F2A0 7F04A770 AE580014 */  sw    $t8, 0x14($s2)
/* 07F2A4 7F04A774 8DC10018 */  lw    $at, 0x18($t6)
/* 07F2A8 7F04A778 AE410018 */  sw    $at, 0x18($s2)
/* 07F2AC 7F04A77C 8DD8001C */  lw    $t8, 0x1c($t6)
/* 07F2B0 7F04A780 AE58001C */  sw    $t8, 0x1c($s2)
/* 07F2B4 7F04A784 8C790008 */  lw    $t9, 8($v1)
/* 07F2B8 7F04A788 8F210020 */  lw    $at, 0x20($t9)
/* 07F2BC 7F04A78C AE410020 */  sw    $at, 0x20($s2)
/* 07F2C0 7F04A790 8F290024 */  lw    $t1, 0x24($t9)
/* 07F2C4 7F04A794 AE490024 */  sw    $t1, 0x24($s2)
/* 07F2C8 7F04A798 8F210028 */  lw    $at, 0x28($t9)
/* 07F2CC 7F04A79C AE410028 */  sw    $at, 0x28($s2)
/* 07F2D0 7F04A7A0 8F29002C */  lw    $t1, 0x2c($t9)
/* 07F2D4 7F04A7A4 AE49002C */  sw    $t1, 0x2c($s2)
/* 07F2D8 7F04A7A8 8C6A0008 */  lw    $t2, 8($v1)
/* 07F2DC 7F04A7AC 8D410030 */  lw    $at, 0x30($t2)
/* 07F2E0 7F04A7B0 AE410030 */  sw    $at, 0x30($s2)
/* 07F2E4 7F04A7B4 8D4B0034 */  lw    $t3, 0x34($t2)
/* 07F2E8 7F04A7B8 AE4B0034 */  sw    $t3, 0x34($s2)
/* 07F2EC 7F04A7BC 8D410038 */  lw    $at, 0x38($t2)
/* 07F2F0 7F04A7C0 AE410038 */  sw    $at, 0x38($s2)
/* 07F2F4 7F04A7C4 8D4B003C */  lw    $t3, 0x3c($t2)
/* 07F2F8 7F04A7C8 AE4B003C */  sw    $t3, 0x3c($s2)
/* 07F2FC 7F04A7CC 8E020008 */  lw    $v0, 8($s0)
/* 07F300 7F04A7D0 2C410064 */  sltiu $at, $v0, 0x64
/* 07F304 7F04A7D4 10200007 */  beqz  $at, .L7F04A7F4
/* 07F308 7F04A7D8 00408825 */   move  $s1, $v0
/* 07F30C 7F04A7DC 00026880 */  sll   $t5, $v0, 2
/* 07F310 7F04A7E0 8DEFD120 */  lw    $t7, %lo(monitorimages)($t7)
/* 07F314 7F04A7E4 01A26823 */  subu  $t5, $t5, $v0
/* 07F318 7F04A7E8 000D6880 */  sll   $t5, $t5, 2
/* 07F31C 7F04A7EC 10000001 */  b     .L7F04A7F4
/* 07F320 7F04A7F0 01AF8821 */   addu  $s1, $t5, $t7
.L7F04A7F4:
/* 07F324 7F04A7F4 122000A9 */  beqz  $s1, .L7F04AA9C
/* 07F328 7F04A7F8 3C013F00 */   li    $at, 0x3F000000 # 0.500000
/* 07F32C 7F04A7FC 44810000 */  mtc1  $at, $f0
/* 07F330 7F04A800 C6120010 */  lwc1  $f18, 0x10($s0)
/* 07F334 7F04A804 C60A0024 */  lwc1  $f10, 0x24($s0)
/* 07F338 7F04A808 C60C000C */  lwc1  $f12, 0xc($s0)
/* 07F33C 7F04A80C 46009582 */  mul.s $f22, $f18, $f0
/* 07F340 7F04A810 460CA032 */  c.eq.s $f20, $f12
/* 07F344 7F04A814 46005602 */  mul.s $f24, $f10, $f0
/* 07F348 7F04A818 4600B386 */  mov.s $f14, $f22
/* 07F34C 7F04A81C 45010015 */  bc1t  .L7F04A874
/* 07F350 7F04A820 4600C406 */   mov.s $f16, $f24
/* 07F354 7F04A824 E7AE0054 */  swc1  $f14, 0x54($sp)
/* 07F358 7F04A828 0FC15FA8 */  jal   cosf
/* 07F35C 7F04A82C E7B00050 */   swc1  $f16, 0x50($sp)
/* 07F360 7F04A830 3C018005 */  lui   $at, %hi(D_80052CB0)
/* 07F364 7F04A834 C43A2CB0 */  lwc1  $f26, %lo(D_80052CB0)($at)
/* 07F368 7F04A838 C60C000C */  lwc1  $f12, 0xc($s0)
/* 07F36C 7F04A83C 461A0502 */  mul.s $f20, $f0, $f26
/* 07F370 7F04A840 0FC15FAB */  jal   sinf
/* 07F374 7F04A844 00000000 */   nop
/* 07F378 7F04A848 4614B582 */  mul.s $f22, $f22, $f20
/* 07F37C 7F04A84C C7AE0054 */  lwc1  $f14, 0x54($sp)
/* 07F380 7F04A850 C7B00050 */  lwc1  $f16, 0x50($sp)
/* 07F384 7F04A854 461A0082 */  mul.s $f2, $f0, $f26
/* 07F388 7F04A858 00000000 */  nop
/* 07F38C 7F04A85C 4602C602 */  mul.s $f24, $f24, $f2
/* 07F390 7F04A860 00000000 */  nop
/* 07F394 7F04A864 46027382 */  mul.s $f14, $f14, $f2
/* 07F398 7F04A868 00000000 */  nop
/* 07F39C 7F04A86C 46148402 */  mul.s $f16, $f16, $f20
/* 07F3A0 7F04A870 00000000 */  nop
.L7F04A874:
/* 07F3A4 7F04A874 922E0004 */  lbu   $t6, 4($s1)
/* 07F3A8 7F04A878 3C014200 */  li    $at, 0x42000000 # 32.000000
/* 07F3AC 7F04A87C 44810000 */  mtc1  $at, $f0
/* 07F3B0 7F04A880 448E4000 */  mtc1  $t6, $f8
/* 07F3B4 7F04A884 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F3B8 7F04A888 05C10004 */  bgez  $t6, .L7F04A89C
/* 07F3BC 7F04A88C 468041A0 */   cvt.s.w $f6, $f8
/* 07F3C0 7F04A890 44819000 */  mtc1  $at, $f18
/* 07F3C4 7F04A894 00000000 */  nop
/* 07F3C8 7F04A898 46123180 */  add.s $f6, $f6, $f18
.L7F04A89C:
/* 07F3CC 7F04A89C C60A0038 */  lwc1  $f10, 0x38($s0)
/* 07F3D0 7F04A8A0 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F3D4 7F04A8A4 46165100 */  add.s $f4, $f10, $f22
/* 07F3D8 7F04A8A8 46043202 */  mul.s $f8, $f6, $f4
/* 07F3DC 7F04A8AC 00000000 */  nop
/* 07F3E0 7F04A8B0 46004482 */  mul.s $f18, $f8, $f0
/* 07F3E4 7F04A8B4 4600928D */  trunc.w.s $f10, $f18
/* 07F3E8 7F04A8B8 44085000 */  mfc1  $t0, $f10
/* 07F3EC 7F04A8BC 00000000 */  nop
/* 07F3F0 7F04A8C0 A6480008 */  sh    $t0, 8($s2)
/* 07F3F4 7F04A8C4 92390005 */  lbu   $t9, 5($s1)
/* 07F3F8 7F04A8C8 44993000 */  mtc1  $t9, $f6
/* 07F3FC 7F04A8CC 07210004 */  bgez  $t9, .L7F04A8E0
/* 07F400 7F04A8D0 46803120 */   cvt.s.w $f4, $f6
/* 07F404 7F04A8D4 44814000 */  mtc1  $at, $f8
/* 07F408 7F04A8D8 00000000 */  nop
/* 07F40C 7F04A8DC 46082100 */  add.s $f4, $f4, $f8
.L7F04A8E0:
/* 07F410 7F04A8E0 C612004C */  lwc1  $f18, 0x4c($s0)
/* 07F414 7F04A8E4 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F418 7F04A8E8 46189280 */  add.s $f10, $f18, $f24
/* 07F41C 7F04A8EC 460A2182 */  mul.s $f6, $f4, $f10
/* 07F420 7F04A8F0 00000000 */  nop
/* 07F424 7F04A8F4 46003202 */  mul.s $f8, $f6, $f0
/* 07F428 7F04A8F8 4600448D */  trunc.w.s $f18, $f8
/* 07F42C 7F04A8FC 440C9000 */  mfc1  $t4, $f18
/* 07F430 7F04A900 00000000 */  nop
/* 07F434 7F04A904 A64C000A */  sh    $t4, 0xa($s2)
/* 07F438 7F04A908 922A0004 */  lbu   $t2, 4($s1)
/* 07F43C 7F04A90C 448A2000 */  mtc1  $t2, $f4
/* 07F440 7F04A910 05410004 */  bgez  $t2, .L7F04A924
/* 07F444 7F04A914 468022A0 */   cvt.s.w $f10, $f4
/* 07F448 7F04A918 44813000 */  mtc1  $at, $f6
/* 07F44C 7F04A91C 00000000 */  nop
/* 07F450 7F04A920 46065280 */  add.s $f10, $f10, $f6
.L7F04A924:
/* 07F454 7F04A924 C6080038 */  lwc1  $f8, 0x38($s0)
/* 07F458 7F04A928 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F45C 7F04A92C 460E4481 */  sub.s $f18, $f8, $f14
/* 07F460 7F04A930 46125102 */  mul.s $f4, $f10, $f18
/* 07F464 7F04A934 00000000 */  nop
/* 07F468 7F04A938 46002182 */  mul.s $f6, $f4, $f0
/* 07F46C 7F04A93C 4600320D */  trunc.w.s $f8, $f6
/* 07F470 7F04A940 440D4000 */  mfc1  $t5, $f8
/* 07F474 7F04A944 00000000 */  nop
/* 07F478 7F04A948 A64D0018 */  sh    $t5, 0x18($s2)
/* 07F47C 7F04A94C 922F0005 */  lbu   $t7, 5($s1)
/* 07F480 7F04A950 448F5000 */  mtc1  $t7, $f10
/* 07F484 7F04A954 05E10004 */  bgez  $t7, .L7F04A968
/* 07F488 7F04A958 468054A0 */   cvt.s.w $f18, $f10
/* 07F48C 7F04A95C 44812000 */  mtc1  $at, $f4
/* 07F490 7F04A960 00000000 */  nop
/* 07F494 7F04A964 46049480 */  add.s $f18, $f18, $f4
.L7F04A968:
/* 07F498 7F04A968 C606004C */  lwc1  $f6, 0x4c($s0)
/* 07F49C 7F04A96C 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F4A0 7F04A970 46103200 */  add.s $f8, $f6, $f16
/* 07F4A4 7F04A974 46089282 */  mul.s $f10, $f18, $f8
/* 07F4A8 7F04A978 00000000 */  nop
/* 07F4AC 7F04A97C 46005102 */  mul.s $f4, $f10, $f0
/* 07F4B0 7F04A980 4600218D */  trunc.w.s $f6, $f4
/* 07F4B4 7F04A984 44183000 */  mfc1  $t8, $f6
/* 07F4B8 7F04A988 00000000 */  nop
/* 07F4BC 7F04A98C A658001A */  sh    $t8, 0x1a($s2)
/* 07F4C0 7F04A990 92280004 */  lbu   $t0, 4($s1)
/* 07F4C4 7F04A994 44889000 */  mtc1  $t0, $f18
/* 07F4C8 7F04A998 05010004 */  bgez  $t0, .L7F04A9AC
/* 07F4CC 7F04A99C 46809220 */   cvt.s.w $f8, $f18
/* 07F4D0 7F04A9A0 44815000 */  mtc1  $at, $f10
/* 07F4D4 7F04A9A4 00000000 */  nop
/* 07F4D8 7F04A9A8 460A4200 */  add.s $f8, $f8, $f10
.L7F04A9AC:
/* 07F4DC 7F04A9AC C6040038 */  lwc1  $f4, 0x38($s0)
/* 07F4E0 7F04A9B0 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F4E4 7F04A9B4 46162181 */  sub.s $f6, $f4, $f22
/* 07F4E8 7F04A9B8 46064482 */  mul.s $f18, $f8, $f6
/* 07F4EC 7F04A9BC 00000000 */  nop
/* 07F4F0 7F04A9C0 46009282 */  mul.s $f10, $f18, $f0
/* 07F4F4 7F04A9C4 4600510D */  trunc.w.s $f4, $f10
/* 07F4F8 7F04A9C8 44092000 */  mfc1  $t1, $f4
/* 07F4FC 7F04A9CC 00000000 */  nop
/* 07F500 7F04A9D0 A6490028 */  sh    $t1, 0x28($s2)
/* 07F504 7F04A9D4 922C0005 */  lbu   $t4, 5($s1)
/* 07F508 7F04A9D8 448C4000 */  mtc1  $t4, $f8
/* 07F50C 7F04A9DC 05810004 */  bgez  $t4, .L7F04A9F0
/* 07F510 7F04A9E0 468041A0 */   cvt.s.w $f6, $f8
/* 07F514 7F04A9E4 44819000 */  mtc1  $at, $f18
/* 07F518 7F04A9E8 00000000 */  nop
/* 07F51C 7F04A9EC 46123180 */  add.s $f6, $f6, $f18
.L7F04A9F0:
/* 07F520 7F04A9F0 C60A004C */  lwc1  $f10, 0x4c($s0)
/* 07F524 7F04A9F4 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F528 7F04A9F8 46185101 */  sub.s $f4, $f10, $f24
/* 07F52C 7F04A9FC 46043202 */  mul.s $f8, $f6, $f4
/* 07F530 7F04AA00 00000000 */  nop
/* 07F534 7F04AA04 46004482 */  mul.s $f18, $f8, $f0
/* 07F538 7F04AA08 4600928D */  trunc.w.s $f10, $f18
/* 07F53C 7F04AA0C 440B5000 */  mfc1  $t3, $f10
/* 07F540 7F04AA10 00000000 */  nop
/* 07F544 7F04AA14 A64B002A */  sh    $t3, 0x2a($s2)
/* 07F548 7F04AA18 922D0004 */  lbu   $t5, 4($s1)
/* 07F54C 7F04AA1C 448D3000 */  mtc1  $t5, $f6
/* 07F550 7F04AA20 05A10004 */  bgez  $t5, .L7F04AA34
/* 07F554 7F04AA24 46803120 */   cvt.s.w $f4, $f6
/* 07F558 7F04AA28 44814000 */  mtc1  $at, $f8
/* 07F55C 7F04AA2C 00000000 */  nop
/* 07F560 7F04AA30 46082100 */  add.s $f4, $f4, $f8
.L7F04AA34:
/* 07F564 7F04AA34 C6120038 */  lwc1  $f18, 0x38($s0)
/* 07F568 7F04AA38 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F56C 7F04AA3C 460E9280 */  add.s $f10, $f18, $f14
/* 07F570 7F04AA40 460A2182 */  mul.s $f6, $f4, $f10
/* 07F574 7F04AA44 00000000 */  nop
/* 07F578 7F04AA48 46003202 */  mul.s $f8, $f6, $f0
/* 07F57C 7F04AA4C 4600448D */  trunc.w.s $f18, $f8
/* 07F580 7F04AA50 440E9000 */  mfc1  $t6, $f18
/* 07F584 7F04AA54 00000000 */  nop
/* 07F588 7F04AA58 A64E0038 */  sh    $t6, 0x38($s2)
/* 07F58C 7F04AA5C 92380005 */  lbu   $t8, 5($s1)
/* 07F590 7F04AA60 44982000 */  mtc1  $t8, $f4
/* 07F594 7F04AA64 07010004 */  bgez  $t8, .L7F04AA78
/* 07F598 7F04AA68 468022A0 */   cvt.s.w $f10, $f4
/* 07F59C 7F04AA6C 44813000 */  mtc1  $at, $f6
/* 07F5A0 7F04AA70 00000000 */  nop
/* 07F5A4 7F04AA74 46065280 */  add.s $f10, $f10, $f6
.L7F04AA78:
/* 07F5A8 7F04AA78 C608004C */  lwc1  $f8, 0x4c($s0)
/* 07F5AC 7F04AA7C 46104481 */  sub.s $f18, $f8, $f16
/* 07F5B0 7F04AA80 46125102 */  mul.s $f4, $f10, $f18
/* 07F5B4 7F04AA84 00000000 */  nop
/* 07F5B8 7F04AA88 46002182 */  mul.s $f6, $f4, $f0
/* 07F5BC 7F04AA8C 4600320D */  trunc.w.s $f8, $f6
/* 07F5C0 7F04AA90 44194000 */  mfc1  $t9, $f8
/* 07F5C4 7F04AA94 00000000 */  nop
/* 07F5C8 7F04AA98 A659003A */  sh    $t9, 0x3a($s2)
.L7F04AA9C:
/* 07F5CC 7F04AA9C 92020060 */  lbu   $v0, 0x60($s0)
/* 07F5D0 7F04AAA0 240C0002 */  li    $t4, 2
/* 07F5D4 7F04AAA4 3C0DB700 */  lui   $t5, 0xb700
/* 07F5D8 7F04AAA8 A242003C */  sb    $v0, 0x3c($s2)
/* 07F5DC 7F04AAAC A242002C */  sb    $v0, 0x2c($s2)
/* 07F5E0 7F04AAB0 A242001C */  sb    $v0, 0x1c($s2)
/* 07F5E4 7F04AAB4 A242000C */  sb    $v0, 0xc($s2)
/* 07F5E8 7F04AAB8 92020063 */  lbu   $v0, 0x63($s0)
/* 07F5EC 7F04AABC 240F2000 */  li    $t7, 8192
/* 07F5F0 7F04AAC0 27A400BC */  addiu $a0, $sp, 0xbc
/* 07F5F4 7F04AAC4 A242003D */  sb    $v0, 0x3d($s2)
/* 07F5F8 7F04AAC8 A242002D */  sb    $v0, 0x2d($s2)
/* 07F5FC 7F04AACC A242001D */  sb    $v0, 0x1d($s2)
/* 07F600 7F04AAD0 A242000D */  sb    $v0, 0xd($s2)
/* 07F604 7F04AAD4 92030066 */  lbu   $v1, 0x66($s0)
/* 07F608 7F04AAD8 A243003E */  sb    $v1, 0x3e($s2)
/* 07F60C 7F04AADC A243002E */  sb    $v1, 0x2e($s2)
/* 07F610 7F04AAE0 A243001E */  sb    $v1, 0x1e($s2)
/* 07F614 7F04AAE4 A243000E */  sb    $v1, 0xe($s2)
/* 07F618 7F04AAE8 92020069 */  lbu   $v0, 0x69($s0)
/* 07F61C 7F04AAEC A242003F */  sb    $v0, 0x3f($s2)
/* 07F620 7F04AAF0 A242002F */  sb    $v0, 0x2f($s2)
/* 07F624 7F04AAF4 A242001F */  sb    $v0, 0x1f($s2)
/* 07F628 7F04AAF8 A242000F */  sb    $v0, 0xf($s2)
/* 07F62C 7F04AAFC 92090069 */  lbu   $t1, 0x69($s0)
/* 07F630 7F04AB00 292100FF */  slti  $at, $t1, 0xff
/* 07F634 7F04AB04 50200003 */  beql  $at, $zero, .L7F04AB14
/* 07F638 7F04AB08 8FAA00BC */   lw    $t2, 0xbc($sp)
/* 07F63C 7F04AB0C AFAC00C4 */  sw    $t4, 0xc4($sp)
/* 07F640 7F04AB10 8FAA00BC */  lw    $t2, 0xbc($sp)
.L7F04AB14:
/* 07F644 7F04AB14 240E0002 */  li    $t6, 2
/* 07F648 7F04AB18 02202825 */  move  $a1, $s1
/* 07F64C 7F04AB1C 254B0008 */  addiu $t3, $t2, 8
/* 07F650 7F04AB20 AFAB00BC */  sw    $t3, 0xbc($sp)
/* 07F654 7F04AB24 AD4F0004 */  sw    $t7, 4($t2)
/* 07F658 7F04AB28 AD4D0000 */  sw    $t5, ($t2)
/* 07F65C 7F04AB2C AFAE0010 */  sw    $t6, 0x10($sp)
/* 07F660 7F04AB30 8FA700C0 */  lw    $a3, 0xc0($sp)
/* 07F664 7F04AB34 0FC1DB5A */  jal   texSelect
/* 07F668 7F04AB38 8FA600C4 */   lw    $a2, 0xc4($sp)
/* 07F66C 7F04AB3C 8FB000BC */  lw    $s0, 0xbc($sp)
/* 07F670 7F04AB40 3C190102 */  lui   $t9, (0x01020040 >> 16) # lui $t9, 0x102
/* 07F674 7F04AB44 37390040 */  ori   $t9, (0x01020040 & 0xFFFF) # ori $t9, $t9, 0x40
/* 07F678 7F04AB48 26080008 */  addiu $t0, $s0, 8
/* 07F67C 7F04AB4C AFA800BC */  sw    $t0, 0xbc($sp)
/* 07F680 7F04AB50 AE190000 */  sw    $t9, ($s0)
/* 07F684 7F04AB54 8FA900B0 */  lw    $t1, 0xb0($sp)
/* 07F688 7F04AB58 0C003A2C */  jal   osVirtualToPhysical
/* 07F68C 7F04AB5C 8D24000C */   lw    $a0, 0xc($t1)
/* 07F690 7F04AB60 AE020004 */  sw    $v0, 4($s0)
/* 07F694 7F04AB64 8FB100BC */  lw    $s1, 0xbc($sp)
/* 07F698 7F04AB68 3C0BBC00 */  lui   $t3, (0xBC001006 >> 16) # lui $t3, 0xbc00
/* 07F69C 7F04AB6C 356B1006 */  ori   $t3, (0xBC001006 & 0xFFFF) # ori $t3, $t3, 0x1006
/* 07F6A0 7F04AB70 262A0008 */  addiu $t2, $s1, 8
/* 07F6A4 7F04AB74 AFAA00BC */  sw    $t2, 0xbc($sp)
/* 07F6A8 7F04AB78 02402025 */  move  $a0, $s2
/* 07F6AC 7F04AB7C 0C003A2C */  jal   osVirtualToPhysical
/* 07F6B0 7F04AB80 AE2B0000 */   sw    $t3, ($s1)
/* 07F6B4 7F04AB84 AE220004 */  sw    $v0, 4($s1)
/* 07F6B8 7F04AB88 8FAD00BC */  lw    $t5, 0xbc($sp)
/* 07F6BC 7F04AB8C 3C0E0430 */  lui   $t6, (0x04300040 >> 16) # lui $t6, 0x430
/* 07F6C0 7F04AB90 35CE0040 */  ori   $t6, (0x04300040 & 0xFFFF) # ori $t6, $t6, 0x40
/* 07F6C4 7F04AB94 25AF0008 */  addiu $t7, $t5, 8
/* 07F6C8 7F04AB98 AFAF00BC */  sw    $t7, 0xbc($sp)
/* 07F6CC 7F04AB9C 3C180400 */  lui   $t8, 0x400
/* 07F6D0 7F04ABA0 ADB80004 */  sw    $t8, 4($t5)
/* 07F6D4 7F04ABA4 ADAE0000 */  sw    $t6, ($t5)
/* 07F6D8 7F04ABA8 8FA800BC */  lw    $t0, 0xbc($sp)
/* 07F6DC 7F04ABAC 3C09B100 */  lui   $t1, (0xB1000032 >> 16) # lui $t1, 0xb100
/* 07F6E0 7F04ABB0 35290032 */  ori   $t1, (0xB1000032 & 0xFFFF) # ori $t1, $t1, 0x32
/* 07F6E4 7F04ABB4 25190008 */  addiu $t9, $t0, 8
/* 07F6E8 7F04ABB8 AFB900BC */  sw    $t9, 0xbc($sp)
/* 07F6EC 7F04ABBC 240C2010 */  li    $t4, 8208
/* 07F6F0 7F04ABC0 AD0C0004 */  sw    $t4, 4($t0)
/* 07F6F4 7F04ABC4 AD090000 */  sw    $t1, ($t0)
/* 07F6F8 7F04ABC8 8FAA00BC */  lw    $t2, 0xbc($sp)
/* 07F6FC 7F04ABCC 3C0DB800 */  lui   $t5, 0xb800
/* 07F700 7F04ABD0 3C0F0601 */  lui   $t7, 0x601
/* 07F704 7F04ABD4 254B0008 */  addiu $t3, $t2, 8
/* 07F708 7F04ABD8 AFAB00BC */  sw    $t3, 0xbc($sp)
/* 07F70C 7F04ABDC AD400004 */  sw    $zero, 4($t2)
/* 07F710 7F04ABE0 AD4D0000 */  sw    $t5, ($t2)
/* 07F714 7F04ABE4 8FA600A8 */  lw    $a2, 0xa8($sp)
/* 07F718 7F04ABE8 ACCF0000 */  sw    $t7, ($a2)
/* 07F71C 7F04ABEC 8FAE00BC */  lw    $t6, 0xbc($sp)
/* 07F720 7F04ABF0 ACCE0004 */  sw    $t6, 4($a2)
.L7F04ABF4:
/* 07F724 7F04ABF4 8FBF004C */  lw    $ra, 0x4c($sp)
.L7F04ABF8:
/* 07F728 7F04ABF8 8FA200BC */  lw    $v0, 0xbc($sp)
/* 07F72C 7F04ABFC D7B40020 */  ldc1  $f20, 0x20($sp)
/* 07F730 7F04AC00 D7B60028 */  ldc1  $f22, 0x28($sp)
/* 07F734 7F04AC04 D7B80030 */  ldc1  $f24, 0x30($sp)
/* 07F738 7F04AC08 D7BA0038 */  ldc1  $f26, 0x38($sp)
/* 07F73C 7F04AC0C 8FB00040 */  lw    $s0, 0x40($sp)
/* 07F740 7F04AC10 8FB10044 */  lw    $s1, 0x44($sp)
/* 07F744 7F04AC14 8FB20048 */  lw    $s2, 0x48($sp)
/* 07F748 7F04AC18 03E00008 */  jr    $ra
/* 07F74C 7F04AC1C 27BD00B0 */   addiu $sp, $sp, 0xb0
)
#endif
#ifdef VERSION_EU
GLOBAL_ASM(
.late_rodata
glabel D_80052C68
.word 0x40c90fdb /*6.2831855*/
/*D:80052C6C*/
glabel jpt_80052C6C
.word command00_reset_scroll_shift
.word command01_horizontal_scroll
.word command02_vertical_scroll
.word command03_horizontal_pos
.word command04_vertical_pos
.word command05_zoomx
.word command06_zoomy
.word command07_use_image_from_global_monitor_table
.word command08_halt_processing_for_time
.word command09_jump
.word command0A_jump_conditional
.word command0B_restart
.word command0C_break
.word command0D_colour_transition
.word command0E_set_rotation
.word command0F_rotate

glabel D_80052CAC
.word 0x3fc90fdb /*1.5707964*/
glabel D_80052CB0
.word 0x3fb50481 /*1.4141999*/
glabel D_80052CB4
.word 0x3f59999a /*0.85000002*/
glabel D_80052CB8
.word 0x3e19999a /*0.15000001*/
glabel D_80052CBC
.word 0x3e19999a /*0.15000001*/
glabel D_80052CC0
.word 0x3f666666 /*0.89999998*/
.text
glabel process_monitor_animation_microcode
/* 07EA78 7F049F48 27BDFF50 */  addiu $sp, $sp, -0xb0
/* 07EA7C 7F049F4C AFB10044 */  sw    $s1, 0x44($sp)
/* 07EA80 7F049F50 AFB00040 */  sw    $s0, 0x40($sp)
/* 07EA84 7F049F54 00C08025 */  move  $s0, $a2
/* 07EA88 7F049F58 00A08825 */  move  $s1, $a1
/* 07EA8C 7F049F5C AFBF004C */  sw    $ra, 0x4c($sp)
/* 07EA90 7F049F60 AFB20048 */  sw    $s2, 0x48($sp)
/* 07EA94 7F049F64 F7BA0038 */  sdc1  $f26, 0x38($sp)
/* 07EA98 7F049F68 F7B80030 */  sdc1  $f24, 0x30($sp)
/* 07EA9C 7F049F6C F7B60028 */  sdc1  $f22, 0x28($sp)
/* 07EAA0 7F049F70 F7B40020 */  sdc1  $f20, 0x20($sp)
/* 07EAA4 7F049F74 AFA400B0 */  sw    $a0, 0xb0($sp)
/* 07EAA8 7F049F78 10A0031E */  beqz  $a1, .L7F04ABF4
/* 07EAAC 7F049F7C AFA700BC */   sw    $a3, 0xbc($sp)
/* 07EAB0 7F049F80 94AE0000 */  lhu   $t6, ($a1)
/* 07EAB4 7F049F84 24010018 */  li    $at, 24
/* 07EAB8 7F049F88 31CF00FF */  andi  $t7, $t6, 0xff
/* 07EABC 7F049F8C 55E1031A */  bnel  $t7, $at, .L7F04ABF8
/* 07EAC0 7F049F90 8FBF004C */   lw    $ra, 0x4c($sp)
/* 07EAC4 7F049F94 0FC2F5B1 */  jal   dynAllocate7F0BD6C4
/* 07EAC8 7F049F98 24040004 */   li    $a0, 4
/* 07EACC 7F049F9C 8FB800BC */  lw    $t8, 0xbc($sp)
/* 07EAD0 7F049FA0 00409025 */  move  $s2, $v0
/* 07EAD4 7F049FA4 8FA400B0 */  lw    $a0, 0xb0($sp)
/* 07EAD8 7F049FA8 27190008 */  addiu $t9, $t8, 8
/* 07EADC 7F049FAC AFB900BC */  sw    $t9, 0xbc($sp)
/* 07EAE0 7F049FB0 AFB800A8 */  sw    $t8, 0xa8($sp)
/* 07EAE4 7F049FB4 8E280004 */  lw    $t0, 4($s1)
/* 07EAE8 7F049FB8 02202825 */  move  $a1, $s1
/* 07EAEC 7F049FBC 0FC1B1E7 */  jal   modelGetNodeRwData
/* 07EAF0 7F049FC0 AFA800A4 */   sw    $t0, 0xa4($sp)
/* 07EAF4 7F049FC4 3C018005 */  lui   $at, %hi(D_80052C68)
/* 07EAF8 7F049FC8 C43A2C68 */  lwc1  $f26, %lo(D_80052C68)($at)
/* 07EAFC 7F049FCC 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 07EB00 7F049FD0 4481C000 */  mtc1  $at, $f24
/* 07EB04 7F049FD4 3C013A80 */  li    $at, 0x3A800000 # 0.000977
/* 07EB08 7F049FD8 4481B000 */  mtc1  $at, $f22
/* 07EB0C 7F049FDC 3C014780 */  li    $at, 0x47800000 # 65536.000000
/* 07EB10 7F049FE0 44811000 */  mtc1  $at, $f2
/* 07EB14 7F049FE4 4480A000 */  mtc1  $zero, $f20
/* 07EB18 7F049FE8 AFA200A0 */  sw    $v0, 0xa0($sp)
/* 07EB1C 7F049FEC 00008825 */  move  $s1, $zero
/* 07EB20 7F049FF0 96020004 */  lhu   $v0, 4($s0)
.L7F049FF4:
/* 07EB24 7F049FF4 8E090000 */  lw    $t1, ($s0)
/* 07EB28 7F049FF8 00025080 */  sll   $t2, $v0, 2
/* 07EB2C 7F049FFC 012A1821 */  addu  $v1, $t1, $t2
/* 07EB30 7F04A000 8C6B0000 */  lw    $t3, ($v1)
/* 07EB34 7F04A004 2D610010 */  sltiu $at, $t3, 0x10
/* 07EB38 7F04A008 10200117 */  beqz  $at, .L7F04A468
/* 07EB3C 7F04A00C 000B5880 */   sll   $t3, $t3, 2
/* 07EB40 7F04A010 3C018005 */  lui   $at, %hi(jpt_80052C6C)
/* 07EB44 7F04A014 002B0821 */  addu  $at, $at, $t3
/* 07EB48 7F04A018 8C2B2C6C */  lw    $t3, %lo(jpt_80052C6C)($at)
/* 07EB4C 7F04A01C 01600008 */  jr    $t3
/* 07EB50 7F04A020 00000000 */   nop
command00_reset_scroll_shift:
/* 07EB54 7F04A024 244C0001 */  addiu $t4, $v0, 1
/* 07EB58 7F04A028 E6140040 */  swc1  $f20, 0x40($s0)
/* 07EB5C 7F04A02C E6140054 */  swc1  $f20, 0x54($s0)
/* 07EB60 7F04A030 1000010D */  b     .L7F04A468
/* 07EB64 7F04A034 A60C0004 */   sh    $t4, 4($s0)
command01_horizontal_scroll:
/* 07EB68 7F04A038 E614003C */  swc1  $f20, 0x3c($s0)
/* 07EB6C 7F04A03C 8C6D0008 */  lw    $t5, 8($v1)
/* 07EB70 7F04A040 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07EB74 7F04A044 448D2000 */  mtc1  $t5, $f4
/* 07EB78 7F04A048 05A10004 */  bgez  $t5, .L7F04A05C
/* 07EB7C 7F04A04C 468021A0 */   cvt.s.w $f6, $f4
/* 07EB80 7F04A050 44814000 */  mtc1  $at, $f8
/* 07EB84 7F04A054 00000000 */  nop
/* 07EB88 7F04A058 46083180 */  add.s $f6, $f6, $f8
.L7F04A05C:
/* 07EB8C 7F04A05C 4606C283 */  div.s $f10, $f24, $f6
/* 07EB90 7F04A060 C6000038 */  lwc1  $f0, 0x38($s0)
/* 07EB94 7F04A064 244F0003 */  addiu $t7, $v0, 3
/* 07EB98 7F04A068 E6000044 */  swc1  $f0, 0x44($s0)
/* 07EB9C 7F04A06C E60A0040 */  swc1  $f10, 0x40($s0)
/* 07EBA0 7F04A070 8C6E0004 */  lw    $t6, 4($v1)
/* 07EBA4 7F04A074 A60F0004 */  sh    $t7, 4($s0)
/* 07EBA8 7F04A078 448E9000 */  mtc1  $t6, $f18
/* 07EBAC 7F04A07C 00000000 */  nop
/* 07EBB0 7F04A080 46809120 */  cvt.s.w $f4, $f18
/* 07EBB4 7F04A084 46162202 */  mul.s $f8, $f4, $f22
/* 07EBB8 7F04A088 46080180 */  add.s $f6, $f0, $f8
/* 07EBBC 7F04A08C 100000F6 */  b     .L7F04A468
/* 07EBC0 7F04A090 E6060048 */   swc1  $f6, 0x48($s0)
command02_vertical_scroll:
/* 07EBC4 7F04A094 E6140050 */  swc1  $f20, 0x50($s0)
/* 07EBC8 7F04A098 8C780008 */  lw    $t8, 8($v1)
/* 07EBCC 7F04A09C 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07EBD0 7F04A0A0 44985000 */  mtc1  $t8, $f10
/* 07EBD4 7F04A0A4 07010004 */  bgez  $t8, .L7F04A0B8
/* 07EBD8 7F04A0A8 468054A0 */   cvt.s.w $f18, $f10
/* 07EBDC 7F04A0AC 44812000 */  mtc1  $at, $f4
/* 07EBE0 7F04A0B0 00000000 */  nop
/* 07EBE4 7F04A0B4 46049480 */  add.s $f18, $f18, $f4
.L7F04A0B8:
/* 07EBE8 7F04A0B8 4612C203 */  div.s $f8, $f24, $f18
/* 07EBEC 7F04A0BC C600004C */  lwc1  $f0, 0x4c($s0)
/* 07EBF0 7F04A0C0 24480003 */  addiu $t0, $v0, 3
/* 07EBF4 7F04A0C4 E6000058 */  swc1  $f0, 0x58($s0)
/* 07EBF8 7F04A0C8 E6080054 */  swc1  $f8, 0x54($s0)
/* 07EBFC 7F04A0CC 8C790004 */  lw    $t9, 4($v1)
/* 07EC00 7F04A0D0 A6080004 */  sh    $t0, 4($s0)
/* 07EC04 7F04A0D4 44993000 */  mtc1  $t9, $f6
/* 07EC08 7F04A0D8 00000000 */  nop
/* 07EC0C 7F04A0DC 468032A0 */  cvt.s.w $f10, $f6
/* 07EC10 7F04A0E0 46165102 */  mul.s $f4, $f10, $f22
/* 07EC14 7F04A0E4 46040480 */  add.s $f18, $f0, $f4
/* 07EC18 7F04A0E8 100000DF */  b     .L7F04A468
/* 07EC1C 7F04A0EC E612005C */   swc1  $f18, 0x5c($s0)
command03_horizontal_pos:
/* 07EC20 7F04A0F0 E614003C */  swc1  $f20, 0x3c($s0)
/* 07EC24 7F04A0F4 8C690008 */  lw    $t1, 8($v1)
/* 07EC28 7F04A0F8 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07EC2C 7F04A0FC 44894000 */  mtc1  $t1, $f8
/* 07EC30 7F04A100 05210004 */  bgez  $t1, .L7F04A114
/* 07EC34 7F04A104 468041A0 */   cvt.s.w $f6, $f8
/* 07EC38 7F04A108 44815000 */  mtc1  $at, $f10
/* 07EC3C 7F04A10C 00000000 */  nop
/* 07EC40 7F04A110 460A3180 */  add.s $f6, $f6, $f10
.L7F04A114:
/* 07EC44 7F04A114 4606C103 */  div.s $f4, $f24, $f6
/* 07EC48 7F04A118 C6120038 */  lwc1  $f18, 0x38($s0)
/* 07EC4C 7F04A11C 244B0003 */  addiu $t3, $v0, 3
/* 07EC50 7F04A120 E6120044 */  swc1  $f18, 0x44($s0)
/* 07EC54 7F04A124 E6040040 */  swc1  $f4, 0x40($s0)
/* 07EC58 7F04A128 8C6A0004 */  lw    $t2, 4($v1)
/* 07EC5C 7F04A12C A60B0004 */  sh    $t3, 4($s0)
/* 07EC60 7F04A130 448A4000 */  mtc1  $t2, $f8
/* 07EC64 7F04A134 00000000 */  nop
/* 07EC68 7F04A138 468042A0 */  cvt.s.w $f10, $f8
/* 07EC6C 7F04A13C 46165182 */  mul.s $f6, $f10, $f22
/* 07EC70 7F04A140 100000C9 */  b     .L7F04A468
/* 07EC74 7F04A144 E6060048 */   swc1  $f6, 0x48($s0)
command04_vertical_pos:
/* 07EC78 7F04A148 E6140050 */  swc1  $f20, 0x50($s0)
/* 07EC7C 7F04A14C 8C6C0008 */  lw    $t4, 8($v1)
/* 07EC80 7F04A150 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07EC84 7F04A154 448C2000 */  mtc1  $t4, $f4
/* 07EC88 7F04A158 05810004 */  bgez  $t4, .L7F04A16C
/* 07EC8C 7F04A15C 468024A0 */   cvt.s.w $f18, $f4
/* 07EC90 7F04A160 44814000 */  mtc1  $at, $f8
/* 07EC94 7F04A164 00000000 */  nop
/* 07EC98 7F04A168 46089480 */  add.s $f18, $f18, $f8
.L7F04A16C:
/* 07EC9C 7F04A16C 4612C283 */  div.s $f10, $f24, $f18
/* 07ECA0 7F04A170 C606004C */  lwc1  $f6, 0x4c($s0)
/* 07ECA4 7F04A174 244E0003 */  addiu $t6, $v0, 3
/* 07ECA8 7F04A178 E6060058 */  swc1  $f6, 0x58($s0)
/* 07ECAC 7F04A17C E60A0054 */  swc1  $f10, 0x54($s0)
/* 07ECB0 7F04A180 8C6D0004 */  lw    $t5, 4($v1)
/* 07ECB4 7F04A184 A60E0004 */  sh    $t6, 4($s0)
/* 07ECB8 7F04A188 448D2000 */  mtc1  $t5, $f4
/* 07ECBC 7F04A18C 00000000 */  nop
/* 07ECC0 7F04A190 46802220 */  cvt.s.w $f8, $f4
/* 07ECC4 7F04A194 46164482 */  mul.s $f18, $f8, $f22
/* 07ECC8 7F04A198 100000B3 */  b     .L7F04A468
/* 07ECCC 7F04A19C E612005C */   swc1  $f18, 0x5c($s0)
command05_zoomx:
/* 07ECD0 7F04A1A0 E6140014 */  swc1  $f20, 0x14($s0)
/* 07ECD4 7F04A1A4 8C6F0008 */  lw    $t7, 8($v1)
/* 07ECD8 7F04A1A8 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07ECDC 7F04A1AC 448F5000 */  mtc1  $t7, $f10
/* 07ECE0 7F04A1B0 05E10004 */  bgez  $t7, .L7F04A1C4
/* 07ECE4 7F04A1B4 468051A0 */   cvt.s.w $f6, $f10
/* 07ECE8 7F04A1B8 44812000 */  mtc1  $at, $f4
/* 07ECEC 7F04A1BC 00000000 */  nop
/* 07ECF0 7F04A1C0 46043180 */  add.s $f6, $f6, $f4
.L7F04A1C4:
/* 07ECF4 7F04A1C4 4606C203 */  div.s $f8, $f24, $f6
/* 07ECF8 7F04A1C8 C6120010 */  lwc1  $f18, 0x10($s0)
/* 07ECFC 7F04A1CC 24590003 */  addiu $t9, $v0, 3
/* 07ED00 7F04A1D0 E612001C */  swc1  $f18, 0x1c($s0)
/* 07ED04 7F04A1D4 E6080018 */  swc1  $f8, 0x18($s0)
/* 07ED08 7F04A1D8 8C780004 */  lw    $t8, 4($v1)
/* 07ED0C 7F04A1DC A6190004 */  sh    $t9, 4($s0)
/* 07ED10 7F04A1E0 44985000 */  mtc1  $t8, $f10
/* 07ED14 7F04A1E4 00000000 */  nop
/* 07ED18 7F04A1E8 46805120 */  cvt.s.w $f4, $f10
/* 07ED1C 7F04A1EC 46162182 */  mul.s $f6, $f4, $f22
/* 07ED20 7F04A1F0 1000009D */  b     .L7F04A468
/* 07ED24 7F04A1F4 E6060020 */   swc1  $f6, 0x20($s0)
command06_zoomy:
/* 07ED28 7F04A1F8 E6140028 */  swc1  $f20, 0x28($s0)
/* 07ED2C 7F04A1FC 8C680008 */  lw    $t0, 8($v1)
/* 07ED30 7F04A200 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07ED34 7F04A204 44884000 */  mtc1  $t0, $f8
/* 07ED38 7F04A208 05010004 */  bgez  $t0, .L7F04A21C
/* 07ED3C 7F04A20C 468044A0 */   cvt.s.w $f18, $f8
/* 07ED40 7F04A210 44815000 */  mtc1  $at, $f10
/* 07ED44 7F04A214 00000000 */  nop
/* 07ED48 7F04A218 460A9480 */  add.s $f18, $f18, $f10
.L7F04A21C:
/* 07ED4C 7F04A21C 4612C103 */  div.s $f4, $f24, $f18
/* 07ED50 7F04A220 C6060024 */  lwc1  $f6, 0x24($s0)
/* 07ED54 7F04A224 244A0003 */  addiu $t2, $v0, 3
/* 07ED58 7F04A228 E6060030 */  swc1  $f6, 0x30($s0)
/* 07ED5C 7F04A22C E604002C */  swc1  $f4, 0x2c($s0)
/* 07ED60 7F04A230 8C690004 */  lw    $t1, 4($v1)
/* 07ED64 7F04A234 A60A0004 */  sh    $t2, 4($s0)
/* 07ED68 7F04A238 44894000 */  mtc1  $t1, $f8
/* 07ED6C 7F04A23C 00000000 */  nop
/* 07ED70 7F04A240 468042A0 */  cvt.s.w $f10, $f8
/* 07ED74 7F04A244 46165482 */  mul.s $f18, $f10, $f22
/* 07ED78 7F04A248 10000087 */  b     .L7F04A468
/* 07ED7C 7F04A24C E6120034 */   swc1  $f18, 0x34($s0)
command07_use_image_from_global_monitor_table:
/* 07ED80 7F04A250 02002025 */  move  $a0, $s0
/* 07ED84 7F04A254 0FC127D0 */  jal   save_img_index_to_obj_ani_slot
/* 07ED88 7F04A258 8C650004 */   lw    $a1, 4($v1)
/* 07ED8C 7F04A25C 960B0004 */  lhu   $t3, 4($s0)
/* 07ED90 7F04A260 3C014780 */  li    $at, 0x47800000 # 65536.000000
/* 07ED94 7F04A264 44811000 */  mtc1  $at, $f2
/* 07ED98 7F04A268 256C0002 */  addiu $t4, $t3, 2
/* 07ED9C 7F04A26C 1000007E */  b     .L7F04A468
/* 07EDA0 7F04A270 A60C0004 */   sh    $t4, 4($s0)
command08_halt_processing_for_time:
/* 07EDA4 7F04A274 86020006 */  lh    $v0, 6($s0)
/* 07EDA8 7F04A278 3C0D8005 */  lui   $t5, %hi(g_ClockTimer)
/* 07EDAC 7F04A27C 0442000E */  bltzl $v0, .L7F04A2B8
/* 07EDB0 7F04A280 8C680004 */   lw    $t0, 4($v1)
/* 07EDB4 7F04A284 8DAD8374 */  lw    $t5, %lo(g_ClockTimer)($t5)
/* 07EDB8 7F04A288 004D7023 */  subu  $t6, $v0, $t5
/* 07EDBC 7F04A28C A60E0006 */  sh    $t6, 6($s0)
/* 07EDC0 7F04A290 860F0006 */  lh    $t7, 6($s0)
/* 07EDC4 7F04A294 05E20004 */  bltzl $t7, .L7F04A2A8
/* 07EDC8 7F04A298 96180004 */   lhu   $t8, 4($s0)
/* 07EDCC 7F04A29C 10000072 */  b     .L7F04A468
/* 07EDD0 7F04A2A0 24110001 */   li    $s1, 1
/* 07EDD4 7F04A2A4 96180004 */  lhu   $t8, 4($s0)
.L7F04A2A8:
/* 07EDD8 7F04A2A8 27190002 */  addiu $t9, $t8, 2
/* 07EDDC 7F04A2AC 1000006E */  b     .L7F04A468
/* 07EDE0 7F04A2B0 A6190004 */   sh    $t9, 4($s0)
/* 07EDE4 7F04A2B4 8C680004 */  lw    $t0, 4($v1)
.L7F04A2B8:
/* 07EDE8 7F04A2B8 24110001 */  li    $s1, 1
/* 07EDEC 7F04A2BC 1000006A */  b     .L7F04A468
/* 07EDF0 7F04A2C0 A6080006 */   sh    $t0, 6($s0)
command09_jump:
/* 07EDF4 7F04A2C4 02002025 */  move  $a0, $s0
/* 07EDF8 7F04A2C8 0FC12723 */  jal   save_ptr_monitor_ani_code_to_obj_ani_slot
/* 07EDFC 7F04A2CC 8C650004 */   lw    $a1, 4($v1)
/* 07EE00 7F04A2D0 3C014780 */  li    $at, 0x47800000 # 65536.000000
/* 07EE04 7F04A2D4 44811000 */  mtc1  $at, $f2
/* 07EE08 7F04A2D8 10000063 */  b     .L7F04A468
/* 07EE0C 7F04A2DC 00000000 */   nop
command0A_jump_conditional:
/* 07EE10 7F04A2E0 0C002914 */  jal   randomGetNext
/* 07EE14 7F04A2E4 AFA30094 */   sw    $v1, 0x94($sp)
/* 07EE18 7F04A2E8 8FA30094 */  lw    $v1, 0x94($sp)
/* 07EE1C 7F04A2EC 00024C02 */  srl   $t1, $v0, 0x10
/* 07EE20 7F04A2F0 02002025 */  move  $a0, $s0
/* 07EE24 7F04A2F4 8C6A0008 */  lw    $t2, 8($v1)
/* 07EE28 7F04A2F8 012A082B */  sltu  $at, $t1, $t2
/* 07EE2C 7F04A2FC 50200008 */  beql  $at, $zero, .L7F04A320
/* 07EE30 7F04A300 960B0004 */   lhu   $t3, 4($s0)
/* 07EE34 7F04A304 0FC12723 */  jal   save_ptr_monitor_ani_code_to_obj_ani_slot
/* 07EE38 7F04A308 8C650004 */   lw    $a1, 4($v1)
/* 07EE3C 7F04A30C 3C014780 */  li    $at, 0x47800000 # 65536.000000
/* 07EE40 7F04A310 44811000 */  mtc1  $at, $f2
/* 07EE44 7F04A314 10000054 */  b     .L7F04A468
/* 07EE48 7F04A318 00000000 */   nop
/* 07EE4C 7F04A31C 960B0004 */  lhu   $t3, 4($s0)
.L7F04A320:
/* 07EE50 7F04A320 3C014780 */  li    $at, 0x47800000 # 65536.000000
/* 07EE54 7F04A324 44811000 */  mtc1  $at, $f2
/* 07EE58 7F04A328 256C0003 */  addiu $t4, $t3, 3
/* 07EE5C 7F04A32C A60C0004 */  sh    $t4, 4($s0)
/* 07EE60 7F04A330 1000004D */  b     .L7F04A468
/* 07EE64 7F04A334 00000000 */   nop
command0B_restart:
/* 07EE68 7F04A338 1000004B */  b     .L7F04A468
/* 07EE6C 7F04A33C A6000004 */   sh    $zero, 4($s0)
command0C_break:
/* 07EE70 7F04A340 10000049 */  b     .L7F04A468
/* 07EE74 7F04A344 24110001 */   li    $s1, 1
command0D_colour_transition:
/* 07EE78 7F04A348 E614006C */  swc1  $f20, 0x6c($s0)
/* 07EE7C 7F04A34C 8C6D0008 */  lw    $t5, 8($v1)
/* 07EE80 7F04A350 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07EE84 7F04A354 448D2000 */  mtc1  $t5, $f4
/* 07EE88 7F04A358 05A10004 */  bgez  $t5, .L7F04A36C
/* 07EE8C 7F04A35C 468021A0 */   cvt.s.w $f6, $f4
/* 07EE90 7F04A360 44814000 */  mtc1  $at, $f8
/* 07EE94 7F04A364 00000000 */  nop
/* 07EE98 7F04A368 46083180 */  add.s $f6, $f6, $f8
.L7F04A36C:
/* 07EE9C 7F04A36C 4606C283 */  div.s $f10, $f24, $f6
/* 07EEA0 7F04A370 920E0060 */  lbu   $t6, 0x60($s0)
/* 07EEA4 7F04A374 92080063 */  lbu   $t0, 0x63($s0)
/* 07EEA8 7F04A378 920C0066 */  lbu   $t4, 0x66($s0)
/* 07EEAC 7F04A37C A20E0061 */  sb    $t6, 0x61($s0)
/* 07EEB0 7F04A380 92180069 */  lbu   $t8, 0x69($s0)
/* 07EEB4 7F04A384 E60A0070 */  swc1  $f10, 0x70($s0)
/* 07EEB8 7F04A388 8C6F0004 */  lw    $t7, 4($v1)
/* 07EEBC 7F04A38C A2080064 */  sb    $t0, 0x64($s0)
/* 07EEC0 7F04A390 000FCE02 */  srl   $t9, $t7, 0x18
/* 07EEC4 7F04A394 A2190062 */  sb    $t9, 0x62($s0)
/* 07EEC8 7F04A398 8C690004 */  lw    $t1, 4($v1)
/* 07EECC 7F04A39C A20C0067 */  sb    $t4, 0x67($s0)
/* 07EED0 7F04A3A0 00095C02 */  srl   $t3, $t1, 0x10
/* 07EED4 7F04A3A4 A20B0065 */  sb    $t3, 0x65($s0)
/* 07EED8 7F04A3A8 8C6D0004 */  lw    $t5, 4($v1)
/* 07EEDC 7F04A3AC A218006A */  sb    $t8, 0x6a($s0)
/* 07EEE0 7F04A3B0 24490003 */  addiu $t1, $v0, 3
/* 07EEE4 7F04A3B4 000D7A02 */  srl   $t7, $t5, 8
/* 07EEE8 7F04A3B8 A20F0068 */  sb    $t7, 0x68($s0)
/* 07EEEC 7F04A3BC 8C680004 */  lw    $t0, 4($v1)
/* 07EEF0 7F04A3C0 A6090004 */  sh    $t1, 4($s0)
/* 07EEF4 7F04A3C4 10000028 */  b     .L7F04A468
/* 07EEF8 7F04A3C8 A208006B */   sb    $t0, 0x6b($s0)
command0E_set_rotation:
/* 07EEFC 7F04A3CC 8C6A0004 */  lw    $t2, 4($v1)
/* 07EF00 7F04A3D0 244B0002 */  addiu $t3, $v0, 2
/* 07EF04 7F04A3D4 A60B0004 */  sh    $t3, 4($s0)
/* 07EF08 7F04A3D8 448A9000 */  mtc1  $t2, $f18
/* 07EF0C 7F04A3DC 00000000 */  nop
/* 07EF10 7F04A3E0 46809120 */  cvt.s.w $f4, $f18
/* 07EF14 7F04A3E4 461A2202 */  mul.s $f8, $f4, $f26
/* 07EF18 7F04A3E8 46024183 */  div.s $f6, $f8, $f2
/* 07EF1C 7F04A3EC 1000001E */  b     .L7F04A468
/* 07EF20 7F04A3F0 E606000C */   swc1  $f6, 0xc($s0)
command0F_rotate:
/* 07EF24 7F04A3F4 8C6C0004 */  lw    $t4, 4($v1)
/* 07EF28 7F04A3F8 3C018005 */  lui   $at, %hi(g_JP_GlobalTimerDelta)
/* 07EF2C 7F04A3FC C42A8378 */  lwc1  $f10, %lo(g_JP_GlobalTimerDelta)($at)
/* 07EF30 7F04A400 448C9000 */  mtc1  $t4, $f18
/* 07EF34 7F04A404 00000000 */  nop
/* 07EF38 7F04A408 46809120 */  cvt.s.w $f4, $f18
/* 07EF3C 7F04A40C 46045202 */  mul.s $f8, $f10, $f4
/* 07EF40 7F04A410 C60A000C */  lwc1  $f10, 0xc($s0)
/* 07EF44 7F04A414 461A4182 */  mul.s $f6, $f8, $f26
/* 07EF48 7F04A418 46023483 */  div.s $f18, $f6, $f2
/* 07EF4C 7F04A41C 46125100 */  add.s $f4, $f10, $f18
/* 07EF50 7F04A420 E604000C */  swc1  $f4, 0xc($s0)
/* 07EF54 7F04A424 C60C000C */  lwc1  $f12, 0xc($s0)
/* 07EF58 7F04A428 460CD03E */  c.le.s $f26, $f12
/* 07EF5C 7F04A42C 00000000 */  nop
/* 07EF60 7F04A430 45020005 */  bc1fl .L7F04A448
/* 07EF64 7F04A434 4614603C */   c.lt.s $f12, $f20
/* 07EF68 7F04A438 461A6201 */  sub.s $f8, $f12, $f26
/* 07EF6C 7F04A43C E608000C */  swc1  $f8, 0xc($s0)
/* 07EF70 7F04A440 C60C000C */  lwc1  $f12, 0xc($s0)
/* 07EF74 7F04A444 4614603C */  c.lt.s $f12, $f20
.L7F04A448:
/* 07EF78 7F04A448 00000000 */  nop
/* 07EF7C 7F04A44C 45020004 */  bc1fl .L7F04A460
/* 07EF80 7F04A450 960D0004 */   lhu   $t5, 4($s0)
/* 07EF84 7F04A454 461A6180 */  add.s $f6, $f12, $f26
/* 07EF88 7F04A458 E606000C */  swc1  $f6, 0xc($s0)
/* 07EF8C 7F04A45C 960D0004 */  lhu   $t5, 4($s0)
.L7F04A460:
/* 07EF90 7F04A460 25AE0002 */  addiu $t6, $t5, 2
/* 07EF94 7F04A464 A60E0004 */  sh    $t6, 4($s0)
def_7F04A01C:
.L7F04A468:
/* 07EF98 7F04A468 5220FEE2 */  beql  $s1, $zero, .L7F049FF4
/* 07EF9C 7F04A46C 96020004 */   lhu   $v0, 4($s0)
/* 07EFA0 7F04A470 C6000018 */  lwc1  $f0, 0x18($s0)
/* 07EFA4 7F04A474 4600A03C */  c.lt.s $f20, $f0
/* 07EFA8 7F04A478 00000000 */  nop
/* 07EFAC 7F04A47C 45000017 */  bc1f  .L7F04A4DC
/* 07EFB0 7F04A480 3C028005 */   lui   $v0, %hi(g_JP_GlobalTimerDelta)
/* 07EFB4 7F04A484 24428378 */  addiu $v0, %lo(g_JP_GlobalTimerDelta) # addiu $v0, $v0, -0x7c88
/* 07EFB8 7F04A488 C4520000 */  lwc1  $f18, ($v0)
/* 07EFBC 7F04A48C C60A0014 */  lwc1  $f10, 0x14($s0)
/* 07EFC0 7F04A490 46120102 */  mul.s $f4, $f0, $f18
/* 07EFC4 7F04A494 46045200 */  add.s $f8, $f10, $f4
/* 07EFC8 7F04A498 E6080014 */  swc1  $f8, 0x14($s0)
/* 07EFCC 7F04A49C C6020014 */  lwc1  $f2, 0x14($s0)
/* 07EFD0 7F04A4A0 4618103C */  c.lt.s $f2, $f24
/* 07EFD4 7F04A4A4 00000000 */  nop
/* 07EFD8 7F04A4A8 45020009 */  bc1fl .L7F04A4D0
/* 07EFDC 7F04A4AC C6080020 */   lwc1  $f8, 0x20($s0)
/* 07EFE0 7F04A4B0 C600001C */  lwc1  $f0, 0x1c($s0)
/* 07EFE4 7F04A4B4 C6060020 */  lwc1  $f6, 0x20($s0)
/* 07EFE8 7F04A4B8 46003481 */  sub.s $f18, $f6, $f0
/* 07EFEC 7F04A4BC 46029282 */  mul.s $f10, $f18, $f2
/* 07EFF0 7F04A4C0 460A0100 */  add.s $f4, $f0, $f10
/* 07EFF4 7F04A4C4 10000005 */  b     .L7F04A4DC
/* 07EFF8 7F04A4C8 E6040010 */   swc1  $f4, 0x10($s0)
/* 07EFFC 7F04A4CC C6080020 */  lwc1  $f8, 0x20($s0)
.L7F04A4D0:
/* 07F000 7F04A4D0 E6180014 */  swc1  $f24, 0x14($s0)
/* 07F004 7F04A4D4 E6140018 */  swc1  $f20, 0x18($s0)
/* 07F008 7F04A4D8 E6080010 */  swc1  $f8, 0x10($s0)
.L7F04A4DC:
/* 07F00C 7F04A4DC C600002C */  lwc1  $f0, 0x2c($s0)
/* 07F010 7F04A4E0 3C028005 */  lui   $v0, %hi(g_JP_GlobalTimerDelta)
/* 07F014 7F04A4E4 24428378 */  addiu $v0, %lo(g_JP_GlobalTimerDelta) # addiu $v0, $v0, -0x7c88
/* 07F018 7F04A4E8 4600A03C */  c.lt.s $f20, $f0
/* 07F01C 7F04A4EC 00000000 */  nop
/* 07F020 7F04A4F0 45020017 */  bc1fl .L7F04A550
/* 07F024 7F04A4F4 C6000040 */   lwc1  $f0, 0x40($s0)
/* 07F028 7F04A4F8 C4520000 */  lwc1  $f18, ($v0)
/* 07F02C 7F04A4FC C6060028 */  lwc1  $f6, 0x28($s0)
/* 07F030 7F04A500 46120282 */  mul.s $f10, $f0, $f18
/* 07F034 7F04A504 460A3100 */  add.s $f4, $f6, $f10
/* 07F038 7F04A508 E6040028 */  swc1  $f4, 0x28($s0)
/* 07F03C 7F04A50C C6020028 */  lwc1  $f2, 0x28($s0)
/* 07F040 7F04A510 4618103C */  c.lt.s $f2, $f24
/* 07F044 7F04A514 00000000 */  nop
/* 07F048 7F04A518 45020009 */  bc1fl .L7F04A540
/* 07F04C 7F04A51C C6040034 */   lwc1  $f4, 0x34($s0)
/* 07F050 7F04A520 C6000030 */  lwc1  $f0, 0x30($s0)
/* 07F054 7F04A524 C6080034 */  lwc1  $f8, 0x34($s0)
/* 07F058 7F04A528 46004481 */  sub.s $f18, $f8, $f0
/* 07F05C 7F04A52C 46029182 */  mul.s $f6, $f18, $f2
/* 07F060 7F04A530 46060280 */  add.s $f10, $f0, $f6
/* 07F064 7F04A534 10000005 */  b     .L7F04A54C
/* 07F068 7F04A538 E60A0024 */   swc1  $f10, 0x24($s0)
/* 07F06C 7F04A53C C6040034 */  lwc1  $f4, 0x34($s0)
.L7F04A540:
/* 07F070 7F04A540 E6180028 */  swc1  $f24, 0x28($s0)
/* 07F074 7F04A544 E614002C */  swc1  $f20, 0x2c($s0)
/* 07F078 7F04A548 E6040024 */  swc1  $f4, 0x24($s0)
.L7F04A54C:
/* 07F07C 7F04A54C C6000040 */  lwc1  $f0, 0x40($s0)
.L7F04A550:
/* 07F080 7F04A550 4600A03C */  c.lt.s $f20, $f0
/* 07F084 7F04A554 00000000 */  nop
/* 07F088 7F04A558 45020017 */  bc1fl .L7F04A5B8
/* 07F08C 7F04A55C C6000054 */   lwc1  $f0, 0x54($s0)
/* 07F090 7F04A560 C4520000 */  lwc1  $f18, ($v0)
/* 07F094 7F04A564 C608003C */  lwc1  $f8, 0x3c($s0)
/* 07F098 7F04A568 46120182 */  mul.s $f6, $f0, $f18
/* 07F09C 7F04A56C 46064280 */  add.s $f10, $f8, $f6
/* 07F0A0 7F04A570 E60A003C */  swc1  $f10, 0x3c($s0)
/* 07F0A4 7F04A574 C602003C */  lwc1  $f2, 0x3c($s0)
/* 07F0A8 7F04A578 4618103C */  c.lt.s $f2, $f24
/* 07F0AC 7F04A57C 00000000 */  nop
/* 07F0B0 7F04A580 45020009 */  bc1fl .L7F04A5A8
/* 07F0B4 7F04A584 C60A0048 */   lwc1  $f10, 0x48($s0)
/* 07F0B8 7F04A588 C6000044 */  lwc1  $f0, 0x44($s0)
/* 07F0BC 7F04A58C C6040048 */  lwc1  $f4, 0x48($s0)
/* 07F0C0 7F04A590 46002481 */  sub.s $f18, $f4, $f0
/* 07F0C4 7F04A594 46029202 */  mul.s $f8, $f18, $f2
/* 07F0C8 7F04A598 46080180 */  add.s $f6, $f0, $f8
/* 07F0CC 7F04A59C 10000005 */  b     .L7F04A5B4
/* 07F0D0 7F04A5A0 E6060038 */   swc1  $f6, 0x38($s0)
/* 07F0D4 7F04A5A4 C60A0048 */  lwc1  $f10, 0x48($s0)
.L7F04A5A8:
/* 07F0D8 7F04A5A8 E618003C */  swc1  $f24, 0x3c($s0)
/* 07F0DC 7F04A5AC E6140040 */  swc1  $f20, 0x40($s0)
/* 07F0E0 7F04A5B0 E60A0038 */  swc1  $f10, 0x38($s0)
.L7F04A5B4:
/* 07F0E4 7F04A5B4 C6000054 */  lwc1  $f0, 0x54($s0)
.L7F04A5B8:
/* 07F0E8 7F04A5B8 4600A03C */  c.lt.s $f20, $f0
/* 07F0EC 7F04A5BC 00000000 */  nop
/* 07F0F0 7F04A5C0 45020017 */  bc1fl .L7F04A620
/* 07F0F4 7F04A5C4 C6020070 */   lwc1  $f2, 0x70($s0)
/* 07F0F8 7F04A5C8 C4520000 */  lwc1  $f18, ($v0)
/* 07F0FC 7F04A5CC C6040050 */  lwc1  $f4, 0x50($s0)
/* 07F100 7F04A5D0 46120202 */  mul.s $f8, $f0, $f18
/* 07F104 7F04A5D4 46082180 */  add.s $f6, $f4, $f8
/* 07F108 7F04A5D8 E6060050 */  swc1  $f6, 0x50($s0)
/* 07F10C 7F04A5DC C6020050 */  lwc1  $f2, 0x50($s0)
/* 07F110 7F04A5E0 4618103C */  c.lt.s $f2, $f24
/* 07F114 7F04A5E4 00000000 */  nop
/* 07F118 7F04A5E8 45020009 */  bc1fl .L7F04A610
/* 07F11C 7F04A5EC C606005C */   lwc1  $f6, 0x5c($s0)
/* 07F120 7F04A5F0 C6000058 */  lwc1  $f0, 0x58($s0)
/* 07F124 7F04A5F4 C60A005C */  lwc1  $f10, 0x5c($s0)
/* 07F128 7F04A5F8 46005481 */  sub.s $f18, $f10, $f0
/* 07F12C 7F04A5FC 46029102 */  mul.s $f4, $f18, $f2
/* 07F130 7F04A600 46040200 */  add.s $f8, $f0, $f4
/* 07F134 7F04A604 10000005 */  b     .L7F04A61C
/* 07F138 7F04A608 E608004C */   swc1  $f8, 0x4c($s0)
/* 07F13C 7F04A60C C606005C */  lwc1  $f6, 0x5c($s0)
.L7F04A610:
/* 07F140 7F04A610 E6180050 */  swc1  $f24, 0x50($s0)
/* 07F144 7F04A614 E6140054 */  swc1  $f20, 0x54($s0)
/* 07F148 7F04A618 E606004C */  swc1  $f6, 0x4c($s0)
.L7F04A61C:
/* 07F14C 7F04A61C C6020070 */  lwc1  $f2, 0x70($s0)
.L7F04A620:
/* 07F150 7F04A620 4602A03C */  c.lt.s $f20, $f2
/* 07F154 7F04A624 00000000 */  nop
/* 07F158 7F04A628 4502003F */  bc1fl .L7F04A728
/* 07F15C 7F04A62C 8FAA00A0 */   lw    $t2, 0xa0($sp)
/* 07F160 7F04A630 C4520000 */  lwc1  $f18, ($v0)
/* 07F164 7F04A634 C60A006C */  lwc1  $f10, 0x6c($s0)
/* 07F168 7F04A638 46121102 */  mul.s $f4, $f2, $f18
/* 07F16C 7F04A63C 46045200 */  add.s $f8, $f10, $f4
/* 07F170 7F04A640 E608006C */  swc1  $f8, 0x6c($s0)
/* 07F174 7F04A644 C600006C */  lwc1  $f0, 0x6c($s0)
/* 07F178 7F04A648 4618003C */  c.lt.s $f0, $f24
/* 07F17C 7F04A64C 00000000 */  nop
/* 07F180 7F04A650 4502002B */  bc1fl .L7F04A700
/* 07F184 7F04A654 920F0062 */   lbu   $t7, 0x62($s0)
/* 07F188 7F04A658 92020061 */  lbu   $v0, 0x61($s0)
/* 07F18C 7F04A65C 920F0062 */  lbu   $t7, 0x62($s0)
/* 07F190 7F04A660 92030064 */  lbu   $v1, 0x64($s0)
/* 07F194 7F04A664 920A0065 */  lbu   $t2, 0x65($s0)
/* 07F198 7F04A668 01E2C023 */  subu  $t8, $t7, $v0
/* 07F19C 7F04A66C 44983000 */  mtc1  $t8, $f6
/* 07F1A0 7F04A670 01435823 */  subu  $t3, $t2, $v1
/* 07F1A4 7F04A674 448B4000 */  mtc1  $t3, $f8
/* 07F1A8 7F04A678 468034A0 */  cvt.s.w $f18, $f6
/* 07F1AC 7F04A67C 920F0068 */  lbu   $t7, 0x68($s0)
/* 07F1B0 7F04A680 92040067 */  lbu   $a0, 0x67($s0)
/* 07F1B4 7F04A684 920A006B */  lbu   $t2, 0x6b($s0)
/* 07F1B8 7F04A688 9205006A */  lbu   $a1, 0x6a($s0)
/* 07F1BC 7F04A68C 468041A0 */  cvt.s.w $f6, $f8
/* 07F1C0 7F04A690 46009282 */  mul.s $f10, $f18, $f0
/* 07F1C4 7F04A694 01E4C023 */  subu  $t8, $t7, $a0
/* 07F1C8 7F04A698 01455823 */  subu  $t3, $t2, $a1
/* 07F1CC 7F04A69C 46003482 */  mul.s $f18, $f6, $f0
/* 07F1D0 7F04A6A0 4600510D */  trunc.w.s $f4, $f10
/* 07F1D4 7F04A6A4 4600928D */  trunc.w.s $f10, $f18
/* 07F1D8 7F04A6A8 44082000 */  mfc1  $t0, $f4
/* 07F1DC 7F04A6AC 44982000 */  mtc1  $t8, $f4
/* 07F1E0 7F04A6B0 440D5000 */  mfc1  $t5, $f10
/* 07F1E4 7F04A6B4 46802220 */  cvt.s.w $f8, $f4
/* 07F1E8 7F04A6B8 448B5000 */  mtc1  $t3, $f10
/* 07F1EC 7F04A6BC 00484821 */  addu  $t1, $v0, $t0
/* 07F1F0 7F04A6C0 006D7021 */  addu  $t6, $v1, $t5
/* 07F1F4 7F04A6C4 A2090060 */  sb    $t1, 0x60($s0)
/* 07F1F8 7F04A6C8 46805120 */  cvt.s.w $f4, $f10
/* 07F1FC 7F04A6CC 46004182 */  mul.s $f6, $f8, $f0
/* 07F200 7F04A6D0 A20E0063 */  sb    $t6, 0x63($s0)
/* 07F204 7F04A6D4 46002202 */  mul.s $f8, $f4, $f0
/* 07F208 7F04A6D8 4600348D */  trunc.w.s $f18, $f6
/* 07F20C 7F04A6DC 4600418D */  trunc.w.s $f6, $f8
/* 07F210 7F04A6E0 44089000 */  mfc1  $t0, $f18
/* 07F214 7F04A6E4 440D3000 */  mfc1  $t5, $f6
/* 07F218 7F04A6E8 00884821 */  addu  $t1, $a0, $t0
/* 07F21C 7F04A6EC A2090066 */  sb    $t1, 0x66($s0)
/* 07F220 7F04A6F0 00AD7021 */  addu  $t6, $a1, $t5
/* 07F224 7F04A6F4 1000000B */  b     .L7F04A724
/* 07F228 7F04A6F8 A20E0069 */   sb    $t6, 0x69($s0)
/* 07F22C 7F04A6FC 920F0062 */  lbu   $t7, 0x62($s0)
.L7F04A700:
/* 07F230 7F04A700 92180065 */  lbu   $t8, 0x65($s0)
/* 07F234 7F04A704 92190068 */  lbu   $t9, 0x68($s0)
/* 07F238 7F04A708 9208006B */  lbu   $t0, 0x6b($s0)
/* 07F23C 7F04A70C E618006C */  swc1  $f24, 0x6c($s0)
/* 07F240 7F04A710 E6140070 */  swc1  $f20, 0x70($s0)
/* 07F244 7F04A714 A20F0060 */  sb    $t7, 0x60($s0)
/* 07F248 7F04A718 A2180063 */  sb    $t8, 0x63($s0)
/* 07F24C 7F04A71C A2190066 */  sb    $t9, 0x66($s0)
/* 07F250 7F04A720 A2080069 */  sb    $t0, 0x69($s0)
.L7F04A724:
/* 07F254 7F04A724 8FAA00A0 */  lw    $t2, 0xa0($sp)
.L7F04A728:
/* 07F258 7F04A728 8FA300A4 */  lw    $v1, 0xa4($sp)
/* 07F25C 7F04A72C 8FA900BC */  lw    $t1, 0xbc($sp)
/* 07F260 7F04A730 AD520000 */  sw    $s2, ($t2)
/* 07F264 7F04A734 3C0F8009 */  lui   $t7, %hi(monitorimages)
/* 07F268 7F04A738 AD490004 */  sw    $t1, 4($t2)
/* 07F26C 7F04A73C 8C6B0008 */  lw    $t3, 8($v1)
/* 07F270 7F04A740 8D610000 */  lw    $at, ($t3)
/* 07F274 7F04A744 AE410000 */  sw    $at, ($s2)
/* 07F278 7F04A748 8D6D0004 */  lw    $t5, 4($t3)
/* 07F27C 7F04A74C AE4D0004 */  sw    $t5, 4($s2)
/* 07F280 7F04A750 8D610008 */  lw    $at, 8($t3)
/* 07F284 7F04A754 AE410008 */  sw    $at, 8($s2)
/* 07F288 7F04A758 8D6D000C */  lw    $t5, 0xc($t3)
/* 07F28C 7F04A75C AE4D000C */  sw    $t5, 0xc($s2)
/* 07F290 7F04A760 8C6E0008 */  lw    $t6, 8($v1)
/* 07F294 7F04A764 8DC10010 */  lw    $at, 0x10($t6)
/* 07F298 7F04A768 AE410010 */  sw    $at, 0x10($s2)
/* 07F29C 7F04A76C 8DD80014 */  lw    $t8, 0x14($t6)
/* 07F2A0 7F04A770 AE580014 */  sw    $t8, 0x14($s2)
/* 07F2A4 7F04A774 8DC10018 */  lw    $at, 0x18($t6)
/* 07F2A8 7F04A778 AE410018 */  sw    $at, 0x18($s2)
/* 07F2AC 7F04A77C 8DD8001C */  lw    $t8, 0x1c($t6)
/* 07F2B0 7F04A780 AE58001C */  sw    $t8, 0x1c($s2)
/* 07F2B4 7F04A784 8C790008 */  lw    $t9, 8($v1)
/* 07F2B8 7F04A788 8F210020 */  lw    $at, 0x20($t9)
/* 07F2BC 7F04A78C AE410020 */  sw    $at, 0x20($s2)
/* 07F2C0 7F04A790 8F290024 */  lw    $t1, 0x24($t9)
/* 07F2C4 7F04A794 AE490024 */  sw    $t1, 0x24($s2)
/* 07F2C8 7F04A798 8F210028 */  lw    $at, 0x28($t9)
/* 07F2CC 7F04A79C AE410028 */  sw    $at, 0x28($s2)
/* 07F2D0 7F04A7A0 8F29002C */  lw    $t1, 0x2c($t9)
/* 07F2D4 7F04A7A4 AE49002C */  sw    $t1, 0x2c($s2)
/* 07F2D8 7F04A7A8 8C6A0008 */  lw    $t2, 8($v1)
/* 07F2DC 7F04A7AC 8D410030 */  lw    $at, 0x30($t2)
/* 07F2E0 7F04A7B0 AE410030 */  sw    $at, 0x30($s2)
/* 07F2E4 7F04A7B4 8D4B0034 */  lw    $t3, 0x34($t2)
/* 07F2E8 7F04A7B8 AE4B0034 */  sw    $t3, 0x34($s2)
/* 07F2EC 7F04A7BC 8D410038 */  lw    $at, 0x38($t2)
/* 07F2F0 7F04A7C0 AE410038 */  sw    $at, 0x38($s2)
/* 07F2F4 7F04A7C4 8D4B003C */  lw    $t3, 0x3c($t2)
/* 07F2F8 7F04A7C8 AE4B003C */  sw    $t3, 0x3c($s2)
/* 07F2FC 7F04A7CC 8E020008 */  lw    $v0, 8($s0)
/* 07F300 7F04A7D0 2C410064 */  sltiu $at, $v0, 0x64
/* 07F304 7F04A7D4 10200007 */  beqz  $at, .L7F04A7F4
/* 07F308 7F04A7D8 00408825 */   move  $s1, $v0
/* 07F30C 7F04A7DC 00026880 */  sll   $t5, $v0, 2
/* 07F310 7F04A7E0 8DEFD120 */  lw    $t7, %lo(monitorimages)($t7)
/* 07F314 7F04A7E4 01A26823 */  subu  $t5, $t5, $v0
/* 07F318 7F04A7E8 000D6880 */  sll   $t5, $t5, 2
/* 07F31C 7F04A7EC 10000001 */  b     .L7F04A7F4
/* 07F320 7F04A7F0 01AF8821 */   addu  $s1, $t5, $t7
.L7F04A7F4:
/* 07F324 7F04A7F4 122000A9 */  beqz  $s1, .L7F04AA9C
/* 07F328 7F04A7F8 3C013F00 */   li    $at, 0x3F000000 # 0.500000
/* 07F32C 7F04A7FC 44810000 */  mtc1  $at, $f0
/* 07F330 7F04A800 C6120010 */  lwc1  $f18, 0x10($s0)
/* 07F334 7F04A804 C60A0024 */  lwc1  $f10, 0x24($s0)
/* 07F338 7F04A808 C60C000C */  lwc1  $f12, 0xc($s0)
/* 07F33C 7F04A80C 46009582 */  mul.s $f22, $f18, $f0
/* 07F340 7F04A810 460CA032 */  c.eq.s $f20, $f12
/* 07F344 7F04A814 46005602 */  mul.s $f24, $f10, $f0
/* 07F348 7F04A818 4600B386 */  mov.s $f14, $f22
/* 07F34C 7F04A81C 45010015 */  bc1t  .L7F04A874
/* 07F350 7F04A820 4600C406 */   mov.s $f16, $f24
/* 07F354 7F04A824 E7AE0054 */  swc1  $f14, 0x54($sp)
/* 07F358 7F04A828 0FC15FA8 */  jal   cosf
/* 07F35C 7F04A82C E7B00050 */   swc1  $f16, 0x50($sp)
/* 07F360 7F04A830 3C018005 */  lui   $at, %hi(D_80052CB0)
/* 07F364 7F04A834 C43A2CB0 */  lwc1  $f26, %lo(D_80052CB0)($at)
/* 07F368 7F04A838 C60C000C */  lwc1  $f12, 0xc($s0)
/* 07F36C 7F04A83C 461A0502 */  mul.s $f20, $f0, $f26
/* 07F370 7F04A840 0FC15FAB */  jal   sinf
/* 07F374 7F04A844 00000000 */   nop
/* 07F378 7F04A848 4614B582 */  mul.s $f22, $f22, $f20
/* 07F37C 7F04A84C C7AE0054 */  lwc1  $f14, 0x54($sp)
/* 07F380 7F04A850 C7B00050 */  lwc1  $f16, 0x50($sp)
/* 07F384 7F04A854 461A0082 */  mul.s $f2, $f0, $f26
/* 07F388 7F04A858 00000000 */  nop
/* 07F38C 7F04A85C 4602C602 */  mul.s $f24, $f24, $f2
/* 07F390 7F04A860 00000000 */  nop
/* 07F394 7F04A864 46027382 */  mul.s $f14, $f14, $f2
/* 07F398 7F04A868 00000000 */  nop
/* 07F39C 7F04A86C 46148402 */  mul.s $f16, $f16, $f20
/* 07F3A0 7F04A870 00000000 */  nop
.L7F04A874:
/* 07F3A4 7F04A874 922E0004 */  lbu   $t6, 4($s1)
/* 07F3A8 7F04A878 3C014200 */  li    $at, 0x42000000 # 32.000000
/* 07F3AC 7F04A87C 44810000 */  mtc1  $at, $f0
/* 07F3B0 7F04A880 448E4000 */  mtc1  $t6, $f8
/* 07F3B4 7F04A884 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F3B8 7F04A888 05C10004 */  bgez  $t6, .L7F04A89C
/* 07F3BC 7F04A88C 468041A0 */   cvt.s.w $f6, $f8
/* 07F3C0 7F04A890 44819000 */  mtc1  $at, $f18
/* 07F3C4 7F04A894 00000000 */  nop
/* 07F3C8 7F04A898 46123180 */  add.s $f6, $f6, $f18
.L7F04A89C:
/* 07F3CC 7F04A89C C60A0038 */  lwc1  $f10, 0x38($s0)
/* 07F3D0 7F04A8A0 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F3D4 7F04A8A4 46165100 */  add.s $f4, $f10, $f22
/* 07F3D8 7F04A8A8 46043202 */  mul.s $f8, $f6, $f4
/* 07F3DC 7F04A8AC 00000000 */  nop
/* 07F3E0 7F04A8B0 46004482 */  mul.s $f18, $f8, $f0
/* 07F3E4 7F04A8B4 4600928D */  trunc.w.s $f10, $f18
/* 07F3E8 7F04A8B8 44085000 */  mfc1  $t0, $f10
/* 07F3EC 7F04A8BC 00000000 */  nop
/* 07F3F0 7F04A8C0 A6480008 */  sh    $t0, 8($s2)
/* 07F3F4 7F04A8C4 92390005 */  lbu   $t9, 5($s1)
/* 07F3F8 7F04A8C8 44993000 */  mtc1  $t9, $f6
/* 07F3FC 7F04A8CC 07210004 */  bgez  $t9, .L7F04A8E0
/* 07F400 7F04A8D0 46803120 */   cvt.s.w $f4, $f6
/* 07F404 7F04A8D4 44814000 */  mtc1  $at, $f8
/* 07F408 7F04A8D8 00000000 */  nop
/* 07F40C 7F04A8DC 46082100 */  add.s $f4, $f4, $f8
.L7F04A8E0:
/* 07F410 7F04A8E0 C612004C */  lwc1  $f18, 0x4c($s0)
/* 07F414 7F04A8E4 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F418 7F04A8E8 46189280 */  add.s $f10, $f18, $f24
/* 07F41C 7F04A8EC 460A2182 */  mul.s $f6, $f4, $f10
/* 07F420 7F04A8F0 00000000 */  nop
/* 07F424 7F04A8F4 46003202 */  mul.s $f8, $f6, $f0
/* 07F428 7F04A8F8 4600448D */  trunc.w.s $f18, $f8
/* 07F42C 7F04A8FC 440C9000 */  mfc1  $t4, $f18
/* 07F430 7F04A900 00000000 */  nop
/* 07F434 7F04A904 A64C000A */  sh    $t4, 0xa($s2)
/* 07F438 7F04A908 922A0004 */  lbu   $t2, 4($s1)
/* 07F43C 7F04A90C 448A2000 */  mtc1  $t2, $f4
/* 07F440 7F04A910 05410004 */  bgez  $t2, .L7F04A924
/* 07F444 7F04A914 468022A0 */   cvt.s.w $f10, $f4
/* 07F448 7F04A918 44813000 */  mtc1  $at, $f6
/* 07F44C 7F04A91C 00000000 */  nop
/* 07F450 7F04A920 46065280 */  add.s $f10, $f10, $f6
.L7F04A924:
/* 07F454 7F04A924 C6080038 */  lwc1  $f8, 0x38($s0)
/* 07F458 7F04A928 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F45C 7F04A92C 460E4481 */  sub.s $f18, $f8, $f14
/* 07F460 7F04A930 46125102 */  mul.s $f4, $f10, $f18
/* 07F464 7F04A934 00000000 */  nop
/* 07F468 7F04A938 46002182 */  mul.s $f6, $f4, $f0
/* 07F46C 7F04A93C 4600320D */  trunc.w.s $f8, $f6
/* 07F470 7F04A940 440D4000 */  mfc1  $t5, $f8
/* 07F474 7F04A944 00000000 */  nop
/* 07F478 7F04A948 A64D0018 */  sh    $t5, 0x18($s2)
/* 07F47C 7F04A94C 922F0005 */  lbu   $t7, 5($s1)
/* 07F480 7F04A950 448F5000 */  mtc1  $t7, $f10
/* 07F484 7F04A954 05E10004 */  bgez  $t7, .L7F04A968
/* 07F488 7F04A958 468054A0 */   cvt.s.w $f18, $f10
/* 07F48C 7F04A95C 44812000 */  mtc1  $at, $f4
/* 07F490 7F04A960 00000000 */  nop
/* 07F494 7F04A964 46049480 */  add.s $f18, $f18, $f4
.L7F04A968:
/* 07F498 7F04A968 C606004C */  lwc1  $f6, 0x4c($s0)
/* 07F49C 7F04A96C 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F4A0 7F04A970 46103200 */  add.s $f8, $f6, $f16
/* 07F4A4 7F04A974 46089282 */  mul.s $f10, $f18, $f8
/* 07F4A8 7F04A978 00000000 */  nop
/* 07F4AC 7F04A97C 46005102 */  mul.s $f4, $f10, $f0
/* 07F4B0 7F04A980 4600218D */  trunc.w.s $f6, $f4
/* 07F4B4 7F04A984 44183000 */  mfc1  $t8, $f6
/* 07F4B8 7F04A988 00000000 */  nop
/* 07F4BC 7F04A98C A658001A */  sh    $t8, 0x1a($s2)
/* 07F4C0 7F04A990 92280004 */  lbu   $t0, 4($s1)
/* 07F4C4 7F04A994 44889000 */  mtc1  $t0, $f18
/* 07F4C8 7F04A998 05010004 */  bgez  $t0, .L7F04A9AC
/* 07F4CC 7F04A99C 46809220 */   cvt.s.w $f8, $f18
/* 07F4D0 7F04A9A0 44815000 */  mtc1  $at, $f10
/* 07F4D4 7F04A9A4 00000000 */  nop
/* 07F4D8 7F04A9A8 460A4200 */  add.s $f8, $f8, $f10
.L7F04A9AC:
/* 07F4DC 7F04A9AC C6040038 */  lwc1  $f4, 0x38($s0)
/* 07F4E0 7F04A9B0 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F4E4 7F04A9B4 46162181 */  sub.s $f6, $f4, $f22
/* 07F4E8 7F04A9B8 46064482 */  mul.s $f18, $f8, $f6
/* 07F4EC 7F04A9BC 00000000 */  nop
/* 07F4F0 7F04A9C0 46009282 */  mul.s $f10, $f18, $f0
/* 07F4F4 7F04A9C4 4600510D */  trunc.w.s $f4, $f10
/* 07F4F8 7F04A9C8 44092000 */  mfc1  $t1, $f4
/* 07F4FC 7F04A9CC 00000000 */  nop
/* 07F500 7F04A9D0 A6490028 */  sh    $t1, 0x28($s2)
/* 07F504 7F04A9D4 922C0005 */  lbu   $t4, 5($s1)
/* 07F508 7F04A9D8 448C4000 */  mtc1  $t4, $f8
/* 07F50C 7F04A9DC 05810004 */  bgez  $t4, .L7F04A9F0
/* 07F510 7F04A9E0 468041A0 */   cvt.s.w $f6, $f8
/* 07F514 7F04A9E4 44819000 */  mtc1  $at, $f18
/* 07F518 7F04A9E8 00000000 */  nop
/* 07F51C 7F04A9EC 46123180 */  add.s $f6, $f6, $f18
.L7F04A9F0:
/* 07F520 7F04A9F0 C60A004C */  lwc1  $f10, 0x4c($s0)
/* 07F524 7F04A9F4 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F528 7F04A9F8 46185101 */  sub.s $f4, $f10, $f24
/* 07F52C 7F04A9FC 46043202 */  mul.s $f8, $f6, $f4
/* 07F530 7F04AA00 00000000 */  nop
/* 07F534 7F04AA04 46004482 */  mul.s $f18, $f8, $f0
/* 07F538 7F04AA08 4600928D */  trunc.w.s $f10, $f18
/* 07F53C 7F04AA0C 440B5000 */  mfc1  $t3, $f10
/* 07F540 7F04AA10 00000000 */  nop
/* 07F544 7F04AA14 A64B002A */  sh    $t3, 0x2a($s2)
/* 07F548 7F04AA18 922D0004 */  lbu   $t5, 4($s1)
/* 07F54C 7F04AA1C 448D3000 */  mtc1  $t5, $f6
/* 07F550 7F04AA20 05A10004 */  bgez  $t5, .L7F04AA34
/* 07F554 7F04AA24 46803120 */   cvt.s.w $f4, $f6
/* 07F558 7F04AA28 44814000 */  mtc1  $at, $f8
/* 07F55C 7F04AA2C 00000000 */  nop
/* 07F560 7F04AA30 46082100 */  add.s $f4, $f4, $f8
.L7F04AA34:
/* 07F564 7F04AA34 C6120038 */  lwc1  $f18, 0x38($s0)
/* 07F568 7F04AA38 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 07F56C 7F04AA3C 460E9280 */  add.s $f10, $f18, $f14
/* 07F570 7F04AA40 460A2182 */  mul.s $f6, $f4, $f10
/* 07F574 7F04AA44 00000000 */  nop
/* 07F578 7F04AA48 46003202 */  mul.s $f8, $f6, $f0
/* 07F57C 7F04AA4C 4600448D */  trunc.w.s $f18, $f8
/* 07F580 7F04AA50 440E9000 */  mfc1  $t6, $f18
/* 07F584 7F04AA54 00000000 */  nop
/* 07F588 7F04AA58 A64E0038 */  sh    $t6, 0x38($s2)
/* 07F58C 7F04AA5C 92380005 */  lbu   $t8, 5($s1)
/* 07F590 7F04AA60 44982000 */  mtc1  $t8, $f4
/* 07F594 7F04AA64 07010004 */  bgez  $t8, .L7F04AA78
/* 07F598 7F04AA68 468022A0 */   cvt.s.w $f10, $f4
/* 07F59C 7F04AA6C 44813000 */  mtc1  $at, $f6
/* 07F5A0 7F04AA70 00000000 */  nop
/* 07F5A4 7F04AA74 46065280 */  add.s $f10, $f10, $f6
.L7F04AA78:
/* 07F5A8 7F04AA78 C608004C */  lwc1  $f8, 0x4c($s0)
/* 07F5AC 7F04AA7C 46104481 */  sub.s $f18, $f8, $f16
/* 07F5B0 7F04AA80 46125102 */  mul.s $f4, $f10, $f18
/* 07F5B4 7F04AA84 00000000 */  nop
/* 07F5B8 7F04AA88 46002182 */  mul.s $f6, $f4, $f0
/* 07F5BC 7F04AA8C 4600320D */  trunc.w.s $f8, $f6
/* 07F5C0 7F04AA90 44194000 */  mfc1  $t9, $f8
/* 07F5C4 7F04AA94 00000000 */  nop
/* 07F5C8 7F04AA98 A659003A */  sh    $t9, 0x3a($s2)
.L7F04AA9C:
/* 07F5CC 7F04AA9C 92020060 */  lbu   $v0, 0x60($s0)
/* 07F5D0 7F04AAA0 240C0002 */  li    $t4, 2
/* 07F5D4 7F04AAA4 3C0DB700 */  lui   $t5, 0xb700
/* 07F5D8 7F04AAA8 A242003C */  sb    $v0, 0x3c($s2)
/* 07F5DC 7F04AAAC A242002C */  sb    $v0, 0x2c($s2)
/* 07F5E0 7F04AAB0 A242001C */  sb    $v0, 0x1c($s2)
/* 07F5E4 7F04AAB4 A242000C */  sb    $v0, 0xc($s2)
/* 07F5E8 7F04AAB8 92020063 */  lbu   $v0, 0x63($s0)
/* 07F5EC 7F04AABC 240F2000 */  li    $t7, 8192
/* 07F5F0 7F04AAC0 27A400BC */  addiu $a0, $sp, 0xbc
/* 07F5F4 7F04AAC4 A242003D */  sb    $v0, 0x3d($s2)
/* 07F5F8 7F04AAC8 A242002D */  sb    $v0, 0x2d($s2)
/* 07F5FC 7F04AACC A242001D */  sb    $v0, 0x1d($s2)
/* 07F600 7F04AAD0 A242000D */  sb    $v0, 0xd($s2)
/* 07F604 7F04AAD4 92030066 */  lbu   $v1, 0x66($s0)
/* 07F608 7F04AAD8 A243003E */  sb    $v1, 0x3e($s2)
/* 07F60C 7F04AADC A243002E */  sb    $v1, 0x2e($s2)
/* 07F610 7F04AAE0 A243001E */  sb    $v1, 0x1e($s2)
/* 07F614 7F04AAE4 A243000E */  sb    $v1, 0xe($s2)
/* 07F618 7F04AAE8 92020069 */  lbu   $v0, 0x69($s0)
/* 07F61C 7F04AAEC A242003F */  sb    $v0, 0x3f($s2)
/* 07F620 7F04AAF0 A242002F */  sb    $v0, 0x2f($s2)
/* 07F624 7F04AAF4 A242001F */  sb    $v0, 0x1f($s2)
/* 07F628 7F04AAF8 A242000F */  sb    $v0, 0xf($s2)
/* 07F62C 7F04AAFC 92090069 */  lbu   $t1, 0x69($s0)
/* 07F630 7F04AB00 292100FF */  slti  $at, $t1, 0xff
/* 07F634 7F04AB04 50200003 */  beql  $at, $zero, .L7F04AB14
/* 07F638 7F04AB08 8FAA00BC */   lw    $t2, 0xbc($sp)
/* 07F63C 7F04AB0C AFAC00C4 */  sw    $t4, 0xc4($sp)
/* 07F640 7F04AB10 8FAA00BC */  lw    $t2, 0xbc($sp)
.L7F04AB14:
/* 07F644 7F04AB14 240E0002 */  li    $t6, 2
/* 07F648 7F04AB18 02202825 */  move  $a1, $s1
/* 07F64C 7F04AB1C 254B0008 */  addiu $t3, $t2, 8
/* 07F650 7F04AB20 AFAB00BC */  sw    $t3, 0xbc($sp)
/* 07F654 7F04AB24 AD4F0004 */  sw    $t7, 4($t2)
/* 07F658 7F04AB28 AD4D0000 */  sw    $t5, ($t2)
/* 07F65C 7F04AB2C AFAE0010 */  sw    $t6, 0x10($sp)
/* 07F660 7F04AB30 8FA700C0 */  lw    $a3, 0xc0($sp)
/* 07F664 7F04AB34 0FC1DB5A */  jal   texSelect
/* 07F668 7F04AB38 8FA600C4 */   lw    $a2, 0xc4($sp)
/* 07F66C 7F04AB3C 8FB000BC */  lw    $s0, 0xbc($sp)
/* 07F670 7F04AB40 3C190102 */  lui   $t9, (0x01020040 >> 16) # lui $t9, 0x102
/* 07F674 7F04AB44 37390040 */  ori   $t9, (0x01020040 & 0xFFFF) # ori $t9, $t9, 0x40
/* 07F678 7F04AB48 26080008 */  addiu $t0, $s0, 8
/* 07F67C 7F04AB4C AFA800BC */  sw    $t0, 0xbc($sp)
/* 07F680 7F04AB50 AE190000 */  sw    $t9, ($s0)
/* 07F684 7F04AB54 8FA900B0 */  lw    $t1, 0xb0($sp)
/* 07F688 7F04AB58 0C003A2C */  jal   osVirtualToPhysical
/* 07F68C 7F04AB5C 8D24000C */   lw    $a0, 0xc($t1)
/* 07F690 7F04AB60 AE020004 */  sw    $v0, 4($s0)
/* 07F694 7F04AB64 8FB100BC */  lw    $s1, 0xbc($sp)
/* 07F698 7F04AB68 3C0BBC00 */  lui   $t3, (0xBC001006 >> 16) # lui $t3, 0xbc00
/* 07F69C 7F04AB6C 356B1006 */  ori   $t3, (0xBC001006 & 0xFFFF) # ori $t3, $t3, 0x1006
/* 07F6A0 7F04AB70 262A0008 */  addiu $t2, $s1, 8
/* 07F6A4 7F04AB74 AFAA00BC */  sw    $t2, 0xbc($sp)
/* 07F6A8 7F04AB78 02402025 */  move  $a0, $s2
/* 07F6AC 7F04AB7C 0C003A2C */  jal   osVirtualToPhysical
/* 07F6B0 7F04AB80 AE2B0000 */   sw    $t3, ($s1)
/* 07F6B4 7F04AB84 AE220004 */  sw    $v0, 4($s1)
/* 07F6B8 7F04AB88 8FAD00BC */  lw    $t5, 0xbc($sp)
/* 07F6BC 7F04AB8C 3C0E0430 */  lui   $t6, (0x04300040 >> 16) # lui $t6, 0x430
/* 07F6C0 7F04AB90 35CE0040 */  ori   $t6, (0x04300040 & 0xFFFF) # ori $t6, $t6, 0x40
/* 07F6C4 7F04AB94 25AF0008 */  addiu $t7, $t5, 8
/* 07F6C8 7F04AB98 AFAF00BC */  sw    $t7, 0xbc($sp)
/* 07F6CC 7F04AB9C 3C180400 */  lui   $t8, 0x400
/* 07F6D0 7F04ABA0 ADB80004 */  sw    $t8, 4($t5)
/* 07F6D4 7F04ABA4 ADAE0000 */  sw    $t6, ($t5)
/* 07F6D8 7F04ABA8 8FA800BC */  lw    $t0, 0xbc($sp)
/* 07F6DC 7F04ABAC 3C09B100 */  lui   $t1, (0xB1000032 >> 16) # lui $t1, 0xb100
/* 07F6E0 7F04ABB0 35290032 */  ori   $t1, (0xB1000032 & 0xFFFF) # ori $t1, $t1, 0x32
/* 07F6E4 7F04ABB4 25190008 */  addiu $t9, $t0, 8
/* 07F6E8 7F04ABB8 AFB900BC */  sw    $t9, 0xbc($sp)
/* 07F6EC 7F04ABBC 240C2010 */  li    $t4, 8208
/* 07F6F0 7F04ABC0 AD0C0004 */  sw    $t4, 4($t0)
/* 07F6F4 7F04ABC4 AD090000 */  sw    $t1, ($t0)
/* 07F6F8 7F04ABC8 8FAA00BC */  lw    $t2, 0xbc($sp)
/* 07F6FC 7F04ABCC 3C0DB800 */  lui   $t5, 0xb800
/* 07F700 7F04ABD0 3C0F0601 */  lui   $t7, 0x601
/* 07F704 7F04ABD4 254B0008 */  addiu $t3, $t2, 8
/* 07F708 7F04ABD8 AFAB00BC */  sw    $t3, 0xbc($sp)
/* 07F70C 7F04ABDC AD400004 */  sw    $zero, 4($t2)
/* 07F710 7F04ABE0 AD4D0000 */  sw    $t5, ($t2)
/* 07F714 7F04ABE4 8FA600A8 */  lw    $a2, 0xa8($sp)
/* 07F718 7F04ABE8 ACCF0000 */  sw    $t7, ($a2)
/* 07F71C 7F04ABEC 8FAE00BC */  lw    $t6, 0xbc($sp)
/* 07F720 7F04ABF0 ACCE0004 */  sw    $t6, 4($a2)
.L7F04ABF4:
/* 07F724 7F04ABF4 8FBF004C */  lw    $ra, 0x4c($sp)
.L7F04ABF8:
/* 07F728 7F04ABF8 8FA200BC */  lw    $v0, 0xbc($sp)
/* 07F72C 7F04ABFC D7B40020 */  ldc1  $f20, 0x20($sp)
/* 07F730 7F04AC00 D7B60028 */  ldc1  $f22, 0x28($sp)
/* 07F734 7F04AC04 D7B80030 */  ldc1  $f24, 0x30($sp)
/* 07F738 7F04AC08 D7BA0038 */  ldc1  $f26, 0x38($sp)
/* 07F73C 7F04AC0C 8FB00040 */  lw    $s0, 0x40($sp)
/* 07F740 7F04AC10 8FB10044 */  lw    $s1, 0x44($sp)
/* 07F744 7F04AC14 8FB20048 */  lw    $s2, 0x48($sp)
/* 07F748 7F04AC18 03E00008 */  jr    $ra
/* 07F74C 7F04AC1C 27BD00B0 */   addiu $sp, $sp, 0xb0
)
#endif

#endif




#ifdef NONMATCHING
void sub_GAME_7F04AC20(void) {

}
#else
GLOBAL_ASM(
.text
glabel sub_GAME_7F04AC20
/* 07F750 7F04AC20 27BDFF88 */  addiu $sp, $sp, -0x78
/* 07F754 7F04AC24 AFBF0024 */  sw    $ra, 0x24($sp)
/* 07F758 7F04AC28 AFB00020 */  sw    $s0, 0x20($sp)
/* 07F75C 7F04AC2C AFA40078 */  sw    $a0, 0x78($sp)
/* 07F760 7F04AC30 AFA60080 */  sw    $a2, 0x80($sp)
/* 07F764 7F04AC34 908F0001 */  lbu   $t7, 1($a0)
/* 07F768 7F04AC38 00A08025 */  move  $s0, $a1
/* 07F76C 7F04AC3C 31F80002 */  andi  $t8, $t7, 2
/* 07F770 7F04AC40 5300013F */  beql  $t8, $zero, .L7F04B140
/* 07F774 7F04AC44 8FBF0024 */   lw    $ra, 0x24($sp)
/* 07F778 7F04AC48 8C990004 */  lw    $t9, 4($a0)
/* 07F77C 7F04AC4C AFB90074 */  sw    $t9, 0x74($sp)
/* 07F780 7F04AC50 8F280014 */  lw    $t0, 0x14($t9)
/* 07F784 7F04AC54 AFA80070 */  sw    $t0, 0x70($sp)
/* 07F788 7F04AC58 8F290008 */  lw    $t1, 8($t9)
/* 07F78C 7F04AC5C 312A0200 */  andi  $t2, $t1, 0x200
/* 07F790 7F04AC60 000A582B */  sltu  $t3, $zero, $t2
/* 07F794 7F04AC64 11600005 */  beqz  $t3, .L7F04AC7C
/* 07F798 7F04AC68 AFAB0028 */   sw    $t3, 0x28($sp)
/* 07F79C 7F04AC6C 0FC1E0E5 */  jal   get_BONDdata_field_10E0
/* 07F7A0 7F04AC70 00000000 */   nop
/* 07F7A4 7F04AC74 0002602B */  sltu  $t4, $zero, $v0
/* 07F7A8 7F04AC78 AFAC0028 */  sw    $t4, 0x28($sp)
.L7F04AC7C:
/* 07F7AC 7F04AC7C 8FAD0074 */  lw    $t5, 0x74($sp)
/* 07F7B0 7F04AC80 2401000A */  li    $at, 10
/* 07F7B4 7F04AC84 8E07000C */  lw    $a3, 0xc($s0)
/* 07F7B8 7F04AC88 91A30003 */  lbu   $v1, 3($t5)
/* 07F7BC 7F04AC8C 54610022 */  bnel  $v1, $at, .L7F04AD18
/* 07F7C0 7F04AC90 2401000B */   li    $at, 11
/* 07F7C4 7F04AC94 8E0F0008 */  lw    $t7, 8($s0)
/* 07F7C8 7F04AC98 8FA40070 */  lw    $a0, 0x70($sp)
/* 07F7CC 7F04AC9C 31F80001 */  andi  $t8, $t7, 1
/* 07F7D0 7F04ACA0 53000075 */  beql  $t8, $zero, .L7F04AE78
/* 07F7D4 7F04ACA4 24010001 */   li    $at, 1
/* 07F7D8 7F04ACA8 8DA8000C */  lw    $t0, 0xc($t5)
/* 07F7DC 7F04ACAC 8FAE0078 */  lw    $t6, 0x78($sp)
/* 07F7E0 7F04ACB0 8FA90074 */  lw    $t1, 0x74($sp)
/* 07F7E4 7F04ACB4 0008CBC0 */  sll   $t9, $t0, 0xf
/* 07F7E8 7F04ACB8 07210003 */  bgez  $t9, .L7F04ACC8
/* 07F7EC 7F04ACBC 8DC20004 */   lw    $v0, 4($t6)
/* 07F7F0 7F04ACC0 10000008 */  b     .L7F04ACE4
/* 07F7F4 7F04ACC4 00001825 */   move  $v1, $zero
.L7F04ACC8:
/* 07F7F8 7F04ACC8 8D2A0008 */  lw    $t2, 8($t1)
/* 07F7FC 7F04ACCC 24030001 */  li    $v1, 1
/* 07F800 7F04ACD0 000A58C0 */  sll   $t3, $t2, 3
/* 07F804 7F04ACD4 05610003 */  bgez  $t3, .L7F04ACE4
/* 07F808 7F04ACD8 00000000 */   nop
/* 07F80C 7F04ACDC 10000001 */  b     .L7F04ACE4
/* 07F810 7F04ACE0 24030008 */   li    $v1, 8
.L7F04ACE4:
/* 07F814 7F04ACE4 8C8F0008 */  lw    $t7, 8($a0)
/* 07F818 7F04ACE8 240E0001 */  li    $t6, 1
/* 07F81C 7F04ACEC 24460080 */  addiu $a2, $v0, 0x80
/* 07F820 7F04ACF0 8DF80008 */  lw    $t8, 8($t7)
/* 07F824 7F04ACF4 8F050000 */  lw    $a1, ($t8)
/* 07F828 7F04ACF8 AFAE0014 */  sw    $t6, 0x14($sp)
/* 07F82C 7F04ACFC 0FC127D2 */  jal   process_monitor_animation_microcode
/* 07F830 7F04AD00 AFA30010 */   sw    $v1, 0x10($sp)
/* 07F834 7F04AD04 8FAD0074 */  lw    $t5, 0x74($sp)
/* 07F838 7F04AD08 00403825 */  move  $a3, $v0
/* 07F83C 7F04AD0C 10000059 */  b     .L7F04AE74
/* 07F840 7F04AD10 91A30003 */   lbu   $v1, 3($t5)
/* 07F844 7F04AD14 2401000B */  li    $at, 11
.L7F04AD18:
/* 07F848 7F04AD18 54610057 */  bnel  $v1, $at, .L7F04AE78
/* 07F84C 7F04AD1C 24010001 */   li    $at, 1
/* 07F850 7F04AD20 8E080008 */  lw    $t0, 8($s0)
/* 07F854 7F04AD24 8FAA0074 */  lw    $t2, 0x74($sp)
/* 07F858 7F04AD28 8FA40070 */  lw    $a0, 0x70($sp)
/* 07F85C 7F04AD2C 31190001 */  andi  $t9, $t0, 1
/* 07F860 7F04AD30 53200051 */  beql  $t9, $zero, .L7F04AE78
/* 07F864 7F04AD34 24010001 */   li    $at, 1
/* 07F868 7F04AD38 8D4B000C */  lw    $t3, 0xc($t2)
/* 07F86C 7F04AD3C 8FA90078 */  lw    $t1, 0x78($sp)
/* 07F870 7F04AD40 8FAF0074 */  lw    $t7, 0x74($sp)
/* 07F874 7F04AD44 000B63C0 */  sll   $t4, $t3, 0xf
/* 07F878 7F04AD48 05810003 */  bgez  $t4, .L7F04AD58
/* 07F87C 7F04AD4C 8D220004 */   lw    $v0, 4($t1)
/* 07F880 7F04AD50 10000008 */  b     .L7F04AD74
/* 07F884 7F04AD54 00001825 */   move  $v1, $zero
.L7F04AD58:
/* 07F888 7F04AD58 8DF80008 */  lw    $t8, 8($t7)
/* 07F88C 7F04AD5C 24030001 */  li    $v1, 1
/* 07F890 7F04AD60 001870C0 */  sll   $t6, $t8, 3
/* 07F894 7F04AD64 05C10003 */  bgez  $t6, .L7F04AD74
/* 07F898 7F04AD68 00000000 */   nop
/* 07F89C 7F04AD6C 10000001 */  b     .L7F04AD74
/* 07F8A0 7F04AD70 24030008 */   li    $v1, 8
.L7F04AD74:
/* 07F8A4 7F04AD74 8C880008 */  lw    $t0, 8($a0)
/* 07F8A8 7F04AD78 24090001 */  li    $t1, 1
/* 07F8AC 7F04AD7C 24460080 */  addiu $a2, $v0, 0x80
/* 07F8B0 7F04AD80 8D190008 */  lw    $t9, 8($t0)
/* 07F8B4 7F04AD84 8F250000 */  lw    $a1, ($t9)
/* 07F8B8 7F04AD88 AFA20058 */  sw    $v0, 0x58($sp)
/* 07F8BC 7F04AD8C AFA90014 */  sw    $t1, 0x14($sp)
/* 07F8C0 7F04AD90 0FC127D2 */  jal   process_monitor_animation_microcode
/* 07F8C4 7F04AD94 AFA30010 */   sw    $v1, 0x10($sp)
/* 07F8C8 7F04AD98 8FAA0074 */  lw    $t2, 0x74($sp)
/* 07F8CC 7F04AD9C 00403825 */  move  $a3, $v0
/* 07F8D0 7F04ADA0 8FAF0074 */  lw    $t7, 0x74($sp)
/* 07F8D4 7F04ADA4 8D4B000C */  lw    $t3, 0xc($t2)
/* 07F8D8 7F04ADA8 8FA40070 */  lw    $a0, 0x70($sp)
/* 07F8DC 7F04ADAC 000B63C0 */  sll   $t4, $t3, 0xf
/* 07F8E0 7F04ADB0 05830004 */  bgezl $t4, .L7F04ADC4
/* 07F8E4 7F04ADB4 8DF80008 */   lw    $t8, 8($t7)
/* 07F8E8 7F04ADB8 10000009 */  b     .L7F04ADE0
/* 07F8EC 7F04ADBC 00001825 */   move  $v1, $zero
/* 07F8F0 7F04ADC0 8DF80008 */  lw    $t8, 8($t7)
.L7F04ADC4:
/* 07F8F4 7F04ADC4 3C013000 */  lui   $at, 0x3000
/* 07F8F8 7F04ADC8 24030001 */  li    $v1, 1
/* 07F8FC 7F04ADCC 03017024 */  and   $t6, $t8, $at
/* 07F900 7F04ADD0 11C00003 */  beqz  $t6, .L7F04ADE0
/* 07F904 7F04ADD4 00000000 */   nop
/* 07F908 7F04ADD8 10000001 */  b     .L7F04ADE0
/* 07F90C 7F04ADDC 24030008 */   li    $v1, 8
.L7F04ADE0:
/* 07F910 7F04ADE0 8C880008 */  lw    $t0, 8($a0)
/* 07F914 7F04ADE4 8FA60058 */  lw    $a2, 0x58($sp)
/* 07F918 7F04ADE8 24090001 */  li    $t1, 1
/* 07F91C 7F04ADEC 8D190008 */  lw    $t9, 8($t0)
/* 07F920 7F04ADF0 24C600F4 */  addiu $a2, $a2, 0xf4
/* 07F924 7F04ADF4 8F250004 */  lw    $a1, 4($t9)
/* 07F928 7F04ADF8 AFA30060 */  sw    $v1, 0x60($sp)
/* 07F92C 7F04ADFC AFA90014 */  sw    $t1, 0x14($sp)
/* 07F930 7F04AE00 0FC127D2 */  jal   process_monitor_animation_microcode
/* 07F934 7F04AE04 AFA30010 */   sw    $v1, 0x10($sp)
/* 07F938 7F04AE08 8FA40070 */  lw    $a0, 0x70($sp)
/* 07F93C 7F04AE0C 8FA30060 */  lw    $v1, 0x60($sp)
/* 07F940 7F04AE10 8FA60058 */  lw    $a2, 0x58($sp)
/* 07F944 7F04AE14 8C8B0008 */  lw    $t3, 8($a0)
/* 07F948 7F04AE18 240F0001 */  li    $t7, 1
/* 07F94C 7F04AE1C 00403825 */  move  $a3, $v0
/* 07F950 7F04AE20 8D6C0008 */  lw    $t4, 8($t3)
/* 07F954 7F04AE24 24C60168 */  addiu $a2, $a2, 0x168
/* 07F958 7F04AE28 8D850008 */  lw    $a1, 8($t4)
/* 07F95C 7F04AE2C AFAF0014 */  sw    $t7, 0x14($sp)
/* 07F960 7F04AE30 0FC127D2 */  jal   process_monitor_animation_microcode
/* 07F964 7F04AE34 AFA30010 */   sw    $v1, 0x10($sp)
/* 07F968 7F04AE38 8FA40070 */  lw    $a0, 0x70($sp)
/* 07F96C 7F04AE3C 8FA30060 */  lw    $v1, 0x60($sp)
/* 07F970 7F04AE40 8FA60058 */  lw    $a2, 0x58($sp)
/* 07F974 7F04AE44 8C8E0008 */  lw    $t6, 8($a0)
/* 07F978 7F04AE48 24080001 */  li    $t0, 1
/* 07F97C 7F04AE4C 00403825 */  move  $a3, $v0
/* 07F980 7F04AE50 8DCD0008 */  lw    $t5, 8($t6)
/* 07F984 7F04AE54 24C601DC */  addiu $a2, $a2, 0x1dc
/* 07F988 7F04AE58 8DA5000C */  lw    $a1, 0xc($t5)
/* 07F98C 7F04AE5C AFA80014 */  sw    $t0, 0x14($sp)
/* 07F990 7F04AE60 0FC127D2 */  jal   process_monitor_animation_microcode
/* 07F994 7F04AE64 AFA30010 */   sw    $v1, 0x10($sp)
/* 07F998 7F04AE68 8FB90074 */  lw    $t9, 0x74($sp)
/* 07F99C 7F04AE6C 00403825 */  move  $a3, $v0
/* 07F9A0 7F04AE70 93230003 */  lbu   $v1, 3($t9)
.L7F04AE74:
/* 07F9A4 7F04AE74 24010001 */  li    $at, 1
.L7F04AE78:
/* 07F9A8 7F04AE78 1461001A */  bne   $v1, $at, .L7F04AEE4
/* 07F9AC 7F04AE7C 8FA40074 */   lw    $a0, 0x74($sp)
/* 07F9B0 7F04AE80 8FA90078 */  lw    $t1, 0x78($sp)
/* 07F9B4 7F04AE84 00E01025 */  move  $v0, $a3
/* 07F9B8 7F04AE88 3C0AB600 */  lui   $t2, 0xb600
/* 07F9BC 7F04AE8C 8D230004 */  lw    $v1, 4($t1)
/* 07F9C0 7F04AE90 240B3000 */  li    $t3, 12288
/* 07F9C4 7F04AE94 AC4B0004 */  sw    $t3, 4($v0)
/* 07F9C8 7F04AE98 AC4A0000 */  sw    $t2, ($v0)
/* 07F9CC 7F04AE9C 946C0098 */  lhu   $t4, 0x98($v1)
/* 07F9D0 7F04AEA0 24E70008 */  addiu $a3, $a3, 8
/* 07F9D4 7F04AEA4 24180002 */  li    $t8, 2
/* 07F9D8 7F04AEA8 318F0008 */  andi  $t7, $t4, 8
/* 07F9DC 7F04AEAC 11E00003 */  beqz  $t7, .L7F04AEBC
/* 07F9E0 7F04AEB0 240E0003 */   li    $t6, 3
/* 07F9E4 7F04AEB4 10000002 */  b     .L7F04AEC0
/* 07F9E8 7F04AEB8 AE18003C */   sw    $t8, 0x3c($s0)
.L7F04AEBC:
/* 07F9EC 7F04AEBC AE0E003C */  sw    $t6, 0x3c($s0)
.L7F04AEC0:
/* 07F9F0 7F04AEC0 8E0D0030 */  lw    $t5, 0x30($s0)
/* 07F9F4 7F04AEC4 24010009 */  li    $at, 9
/* 07F9F8 7F04AEC8 55A1004B */  bnel  $t5, $at, .L7F04AFF8
/* 07F9FC 7F04AECC 8FB90028 */   lw    $t9, 0x28($sp)
/* 07FA00 7F04AED0 8E080034 */  lw    $t0, 0x34($s0)
/* 07FA04 7F04AED4 2401FF00 */  li    $at, -256
/* 07FA08 7F04AED8 0101C824 */  and   $t9, $t0, $at
/* 07FA0C 7F04AEDC 10000045 */  b     .L7F04AFF4
/* 07FA10 7F04AEE0 AE190034 */   sw    $t9, 0x34($s0)
.L7F04AEE4:
/* 07FA14 7F04AEE4 0FC12D1E */  jal   sub_GAME_7F04B478
/* 07FA18 7F04AEE8 AFA70064 */   sw    $a3, 0x64($sp)
/* 07FA1C 7F04AEEC 8FA70064 */  lw    $a3, 0x64($sp)
/* 07FA20 7F04AEF0 1040000F */  beqz  $v0, .L7F04AF30
/* 07FA24 7F04AEF4 00002825 */   move  $a1, $zero
/* 07FA28 7F04AEF8 8C430004 */  lw    $v1, 4($v0)
/* 07FA2C 7F04AEFC 8FA90074 */  lw    $t1, 0x74($sp)
/* 07FA30 7F04AF00 5060000C */  beql  $v1, $zero, .L7F04AF34
/* 07FA34 7F04AF04 8FA40074 */   lw    $a0, 0x74($sp)
/* 07FA38 7F04AF08 8D2A0014 */  lw    $t2, 0x14($t1)
/* 07FA3C 7F04AF0C 946C001A */  lhu   $t4, 0x1a($v1)
/* 07FA40 7F04AF10 8C780008 */  lw    $t8, 8($v1)
/* 07FA44 7F04AF14 8D4B0010 */  lw    $t3, 0x10($t2)
/* 07FA48 7F04AF18 000C7880 */  sll   $t7, $t4, 2
/* 07FA4C 7F04AF1C 016F1021 */  addu  $v0, $t3, $t7
/* 07FA50 7F04AF20 8C4E0000 */  lw    $t6, ($v0)
/* 07FA54 7F04AF24 530E0003 */  beql  $t8, $t6, .L7F04AF34
/* 07FA58 7F04AF28 8FA40074 */   lw    $a0, 0x74($sp)
/* 07FA5C 7F04AF2C 24050001 */  li    $a1, 1
.L7F04AF30:
/* 07FA60 7F04AF30 8FA40074 */  lw    $a0, 0x74($sp)
.L7F04AF34:
/* 07FA64 7F04AF34 AFA50040 */  sw    $a1, 0x40($sp)
/* 07FA68 7F04AF38 0FC0FFF0 */  jal   objGetDestroyedLevel
/* 07FA6C 7F04AF3C AFA70064 */   sw    $a3, 0x64($sp)
/* 07FA70 7F04AF40 8FA50040 */  lw    $a1, 0x40($sp)
/* 07FA74 7F04AF44 10400003 */  beqz  $v0, .L7F04AF54
/* 07FA78 7F04AF48 8FA70064 */   lw    $a3, 0x64($sp)
/* 07FA7C 7F04AF4C 14A0000B */  bnez  $a1, .L7F04AF7C
/* 07FA80 7F04AF50 8FA40074 */   lw    $a0, 0x74($sp)
.L7F04AF54:
/* 07FA84 7F04AF54 8E080030 */  lw    $t0, 0x30($s0)
/* 07FA88 7F04AF58 240D0003 */  li    $t5, 3
/* 07FA8C 7F04AF5C 24010009 */  li    $at, 9
/* 07FA90 7F04AF60 15010024 */  bne   $t0, $at, .L7F04AFF4
/* 07FA94 7F04AF64 AE0D003C */   sw    $t5, 0x3c($s0)
/* 07FA98 7F04AF68 8E190034 */  lw    $t9, 0x34($s0)
/* 07FA9C 7F04AF6C 2401FF00 */  li    $at, -256
/* 07FAA0 7F04AF70 03214824 */  and   $t1, $t9, $at
/* 07FAA4 7F04AF74 1000001F */  b     .L7F04AFF4
/* 07FAA8 7F04AF78 AE090034 */   sw    $t1, 0x34($s0)
.L7F04AF7C:
/* 07FAAC 7F04AF7C 0FC0FFF0 */  jal   objGetDestroyedLevel
/* 07FAB0 7F04AF80 AFA70064 */   sw    $a3, 0x64($sp)
/* 07FAB4 7F04AF84 8E0C0030 */  lw    $t4, 0x30($s0)
/* 07FAB8 7F04AF88 8FA70064 */  lw    $a3, 0x64($sp)
/* 07FABC 7F04AF8C 240A0001 */  li    $t2, 1
/* 07FAC0 7F04AF90 24010009 */  li    $at, 9
/* 07FAC4 7F04AF94 15810012 */  bne   $t4, $at, .L7F04AFE0
/* 07FAC8 7F04AF98 AE0A003C */   sw    $t2, 0x3c($s0)
/* 07FACC 7F04AF9C 00021880 */  sll   $v1, $v0, 2
/* 07FAD0 7F04AFA0 00621823 */  subu  $v1, $v1, $v0
/* 07FAD4 7F04AFA4 000318C0 */  sll   $v1, $v1, 3
/* 07FAD8 7F04AFA8 00621821 */  addu  $v1, $v1, $v0
/* 07FADC 7F04AFAC 00031840 */  sll   $v1, $v1, 1
/* 07FAE0 7F04AFB0 24640064 */  addiu $a0, $v1, 0x64
/* 07FAE4 7F04AFB4 28810100 */  slti  $at, $a0, 0x100
/* 07FAE8 7F04AFB8 54200003 */  bnezl $at, .L7F04AFC8
/* 07FAEC 7F04AFBC 8E0B0034 */   lw    $t3, 0x34($s0)
/* 07FAF0 7F04AFC0 240400FF */  li    $a0, 255
/* 07FAF4 7F04AFC4 8E0B0034 */  lw    $t3, 0x34($s0)
.L7F04AFC8:
/* 07FAF8 7F04AFC8 2401FF00 */  li    $at, -256
/* 07FAFC 7F04AFCC 01617824 */  and   $t7, $t3, $at
/* 07FB00 7F04AFD0 AE0F0034 */  sw    $t7, 0x34($s0)
/* 07FB04 7F04AFD4 01E47025 */  or    $t6, $t7, $a0
/* 07FB08 7F04AFD8 10000006 */  b     .L7F04AFF4
/* 07FB0C 7F04AFDC AE0E0034 */   sw    $t6, 0x34($s0)
.L7F04AFE0:
/* 07FB10 7F04AFE0 58400005 */  blezl $v0, .L7F04AFF8
/* 07FB14 7F04AFE4 8FB90028 */   lw    $t9, 0x28($sp)
/* 07FB18 7F04AFE8 8E0D0034 */  lw    $t5, 0x34($s0)
/* 07FB1C 7F04AFEC 35A8FF00 */  ori   $t0, $t5, 0xff00
/* 07FB20 7F04AFF0 AE080034 */  sw    $t0, 0x34($s0)
.L7F04AFF4:
/* 07FB24 7F04AFF4 8FB90028 */  lw    $t9, 0x28($sp)
.L7F04AFF8:
/* 07FB28 7F04AFF8 3C090103 */  lui   $t1, (0x01030040 >> 16) # lui $t1, 0x103
/* 07FB2C 7F04AFFC 35290040 */  ori   $t1, (0x01030040 & 0xFFFF) # ori $t1, $t1, 0x40
/* 07FB30 7F04B000 13200009 */  beqz  $t9, .L7F04B028
/* 07FB34 7F04B004 00E01825 */   move  $v1, $a3
/* 07FB38 7F04B008 24E70008 */  addiu $a3, $a3, 8
/* 07FB3C 7F04B00C AC690000 */  sw    $t1, ($v1)
/* 07FB40 7F04B010 AFA70064 */  sw    $a3, 0x64($sp)
/* 07FB44 7F04B014 0FC1E0E5 */  jal   get_BONDdata_field_10E0
/* 07FB48 7F04B018 AFA30034 */   sw    $v1, 0x34($sp)
/* 07FB4C 7F04B01C 8FA30034 */  lw    $v1, 0x34($sp)
/* 07FB50 7F04B020 8FA70064 */  lw    $a3, 0x64($sp)
/* 07FB54 7F04B024 AC620004 */  sw    $v0, 4($v1)
.L7F04B028:
/* 07FB58 7F04B028 AE07000C */  sw    $a3, 0xc($s0)
/* 07FB5C 7F04B02C 8FA50070 */  lw    $a1, 0x70($sp)
/* 07FB60 7F04B030 0FC1D1A1 */  jal   subdraw
/* 07FB64 7F04B034 02002025 */   move  $a0, $s0
/* 07FB68 7F04B038 8FA30074 */  lw    $v1, 0x74($sp)
/* 07FB6C 7F04B03C 8E07000C */  lw    $a3, 0xc($s0)
/* 07FB70 7F04B040 24010001 */  li    $at, 1
/* 07FB74 7F04B044 906A0003 */  lbu   $t2, 3($v1)
/* 07FB78 7F04B048 3C0CB600 */  lui   $t4, 0xb600
/* 07FB7C 7F04B04C 240B3000 */  li    $t3, 12288
/* 07FB80 7F04B050 15410004 */  bne   $t2, $at, .L7F04B064
/* 07FB84 7F04B054 00E01025 */   move  $v0, $a3
/* 07FB88 7F04B058 24E70008 */  addiu $a3, $a3, 8
/* 07FB8C 7F04B05C AC4C0000 */  sw    $t4, ($v0)
/* 07FB90 7F04B060 AC4B0004 */  sw    $t3, 4($v0)
.L7F04B064:
/* 07FB94 7F04B064 8FA60080 */  lw    $a2, 0x80($sp)
/* 07FB98 7F04B068 906F0002 */  lbu   $t7, 2($v1)
/* 07FB9C 7F04B06C 24180001 */  li    $t8, 1
/* 07FBA0 7F04B070 00D87004 */  sllv  $t6, $t8, $a2
/* 07FBA4 7F04B074 01EE6824 */  and   $t5, $t7, $t6
/* 07FBA8 7F04B078 11A00005 */  beqz  $t5, .L7F04B090
/* 07FBAC 7F04B07C 00E02025 */   move  $a0, $a3
/* 07FBB0 7F04B080 0FC286A5 */  jal   explosionRenderBulletImpactOnProp
/* 07FBB4 7F04B084 8FA50078 */   lw    $a1, 0x78($sp)
/* 07FBB8 7F04B088 00403825 */  move  $a3, $v0
/* 07FBBC 7F04B08C 8FA60080 */  lw    $a2, 0x80($sp)
.L7F04B090:
/* 07FBC0 7F04B090 8FA80028 */  lw    $t0, 0x28($sp)
/* 07FBC4 7F04B094 3C190103 */  lui   $t9, (0x01030040 >> 16) # lui $t9, 0x103
/* 07FBC8 7F04B098 37390040 */  ori   $t9, (0x01030040 & 0xFFFF) # ori $t9, $t9, 0x40
/* 07FBCC 7F04B09C 1100000A */  beqz  $t0, .L7F04B0C8
/* 07FBD0 7F04B0A0 00E01825 */   move  $v1, $a3
/* 07FBD4 7F04B0A4 24E70008 */  addiu $a3, $a3, 8
/* 07FBD8 7F04B0A8 AC790000 */  sw    $t9, ($v1)
/* 07FBDC 7F04B0AC AFA70064 */  sw    $a3, 0x64($sp)
/* 07FBE0 7F04B0B0 0FC1E0DD */  jal   currentPlayerGetProjectionMatrix
/* 07FBE4 7F04B0B4 AFA3002C */   sw    $v1, 0x2c($sp)
/* 07FBE8 7F04B0B8 8FA3002C */  lw    $v1, 0x2c($sp)
/* 07FBEC 7F04B0BC 8FA70064 */  lw    $a3, 0x64($sp)
/* 07FBF0 7F04B0C0 AC620004 */  sw    $v0, 4($v1)
/* 07FBF4 7F04B0C4 8FA60080 */  lw    $a2, 0x80($sp)
.L7F04B0C8:
/* 07FBF8 7F04B0C8 AE07000C */  sw    $a3, 0xc($s0)
/* 07FBFC 7F04B0CC 8FA90078 */  lw    $t1, 0x78($sp)
/* 07FC00 7F04B0D0 8D240020 */  lw    $a0, 0x20($t1)
/* 07FC04 7F04B0D4 1080000A */  beqz  $a0, .L7F04B100
/* 07FC08 7F04B0D8 00000000 */   nop
/* 07FC0C 7F04B0DC 8FA60080 */  lw    $a2, 0x80($sp)
.L7F04B0E0:
/* 07FC10 7F04B0E0 02002825 */  move  $a1, $s0
/* 07FC14 7F04B0E4 0FC12B08 */  jal   sub_GAME_7F04AC20
/* 07FC18 7F04B0E8 AFA40068 */   sw    $a0, 0x68($sp)
/* 07FC1C 7F04B0EC 8FA40068 */  lw    $a0, 0x68($sp)
/* 07FC20 7F04B0F0 8C840024 */  lw    $a0, 0x24($a0)
/* 07FC24 7F04B0F4 5480FFFA */  bnezl $a0, .L7F04B0E0
/* 07FC28 7F04B0F8 8FA60080 */   lw    $a2, 0x80($sp)
/* 07FC2C 7F04B0FC 8FA60080 */  lw    $a2, 0x80($sp)
.L7F04B100:
/* 07FC30 7F04B100 10C0000E */  beqz  $a2, .L7F04B13C
/* 07FC34 7F04B104 8FAA0028 */   lw    $t2, 0x28($sp)
/* 07FC38 7F04B108 11400008 */  beqz  $t2, .L7F04B12C
/* 07FC3C 7F04B10C 8FA20070 */   lw    $v0, 0x70($sp)
/* 07FC40 7F04B110 8FA20070 */  lw    $v0, 0x70($sp)
/* 07FC44 7F04B114 8C4C0008 */  lw    $t4, 8($v0)
/* 07FC48 7F04B118 8C44000C */  lw    $a0, 0xc($v0)
/* 07FC4C 7F04B11C 0FC22FBB */  jal   sub_GAME_7F08BEEC
/* 07FC50 7F04B120 8585000E */   lh    $a1, 0xe($t4)
/* 07FC54 7F04B124 10000006 */  b     .L7F04B140
/* 07FC58 7F04B128 8FBF0024 */   lw    $ra, 0x24($sp)
.L7F04B12C:
/* 07FC5C 7F04B12C 8C4B0008 */  lw    $t3, 8($v0)
/* 07FC60 7F04B130 8C44000C */  lw    $a0, 0xc($v0)
/* 07FC64 7F04B134 0FC22F52 */  jal   bondviewTransformManyPosToViewMatrix
/* 07FC68 7F04B138 8565000E */   lh    $a1, 0xe($t3)
.L7F04B13C:
/* 07FC6C 7F04B13C 8FBF0024 */  lw    $ra, 0x24($sp)
.L7F04B140:
/* 07FC70 7F04B140 8FB00020 */  lw    $s0, 0x20($sp)
/* 07FC74 7F04B144 27BD0078 */  addiu $sp, $sp, 0x78
/* 07FC78 7F04B148 03E00008 */  jr    $ra
/* 07FC7C 7F04B14C 00000000 */   nop
)
#endif





/**
 * Address 0x7F04B150.
*/

Gfx *chrobjRenderProp(PropRecord *prop, Gfx *gdl, s32 arg2)
{
    struct rgba_f32 spB0;
    s32 spAC;
    s32 spA8;
    ModelRenderData mrData;
    struct view4f sp58;
    struct rgba_s32 sp48;
    s32 sp44;
    ObjectRecord *obj;
    s32 objAlpha;
    f32 temp_f0;
    s32 temp_v0_4;
    s32 phi_a0;

    obj = prop->obj;

    mrData = D_80031FD0;

    objAlpha = 0xFF;
    spAC = fogGetPropDistColor(prop, &spB0);

    if (spAC == 0)
    {
        return gdl;
    }

    if ((u8) obj->type != PROPDEF_TINTED_GLASS)
    {
        temp_f0 = chrobjFogVisRangeRelated(prop, getinstsize(obj->model));

        if (((s32) prop->timetoregen > 0) && ((s32) prop->timetoregen < CHROBJ_TIMETOREGEN))
        {
            temp_f0 *= ((CHROBJ_TIMETOREGEN_F - (f32) prop->timetoregen) / CHROBJ_TIMETOREGEN_F);
        }

        objAlpha = (s32) (temp_f0 * 255.0f);

        if (objAlpha <= 0)
        {
            return gdl;
        }
    }

    if ((objAlpha < 0xFF) || (obj->flags2 & 0x10000))
    {
        if (arg2 == 0)
        {
            return gdl;
        }

        sp44 = 3;
    }
    else
    {

        sp44 = (arg2 == 0) ? 1 : 2;
    }

    if ((sub_GAME_7F054A64(prop, &sp58) > 0) && (((s32)obj->flags2 << 5) >= 0))
    {
        gdl = bgScissorCurrentPlayerViewF(gdl, sp58.left, sp58.top, sp58.width, sp58.height);
    }
    else
    {
        gdl = bgScissorCurrentPlayerViewDefault(gdl);
    }

    mrData.flags = sp44;
    mrData.zbufferenabled = (obj->flags2 & 0x10000) == 0;

    mrData.gdl = gdl;

    if (objAlpha < 0xFF)
    {
        mrData.PropType = 5;
        mrData.envcolour.word = objAlpha;
    }
    else
    {
        mrData.PropType = 9;

        if (obj->type == PROPDEF_TINTED_GLASS)
        {
            mrData.envcolour.word = ((struct TintedGlassRecord*)obj)->calculatedopacity << 8;
        }
        else if ((obj->type == PROPDEF_DOOR) && ((((struct DoorRecord*)obj)->doorFlags & 2) != 0))
        {
            mrData.envcolour.word = ((struct DoorRecord*)obj)->calculatedopacity << 8;
        }
        else
        {
            mrData.envcolour.word = 0;
        }
    }

    temp_v0_4 = objGetShotsTaken(obj);
    phi_a0 = 0xFF - (temp_v0_4 * 0x15);

    if (phi_a0 < 0)
    {
        phi_a0 = 0;
    }

    sp48.r = (s32) (obj->shadecol.rgba[0] * phi_a0) >> 8;
    sp48.g = (s32) (obj->shadecol.rgba[1] * phi_a0) >> 8;
    sp48.b = (s32) (obj->shadecol.rgba[2] * phi_a0) >> 8;
    sp48.a = obj->shadecol.rgba[3] + temp_v0_4 * 0xF;

    if (sp48.a >= 0x100)
    {
        sp48.a = 0xFF;
    }

    sub_GAME_7F040384(&sp48, spAC, &spB0);

    mrData.fogcolour.word = (sp48.rgba[0] << 0x18) | (sp48.rgba[1] << 0x10) | (sp48.rgba[2] << 0x08) | (sp48.rgba[3] << 0x00);

    sub_GAME_7F04AC20(prop, &mrData, arg2);

    return mrData.gdl;
}


ModelNode* sub_GAME_7F04B478(ObjectRecord* obj)
{
    ModelFileHeader* header = obj->model->obj;
    ModelNode *node = header->RootNode;

    while (node)
    {
        u32 type = node->Opcode & 0xff;

        switch (type)
        {
            case MODELNODE_OPCODE_DLCOLLISION:
                return node;
            case MODELNODE_OPCODE_LOD:
                modelApplyDistanceRelations(obj->model, node);
                break;
            case MODELNODE_OPCODE_SWITCH:
                modelApplyToggleRelations(obj->model, node);
                break;
            case MODELNODE_OPCODE_HEAD:
                modelApplyHeadRelations(obj->model, node);
                break;
        }

        if (node->Child)
        {
            node = node->Child;
        }
        else
        {
            while (node)
            {
                if (node->Next)
                {
                    node = node->Next;
                    break;
                }

                node = node->Parent;
            }
        }
    }

    return NULL;
}


bool sub_GAME_7F04B590(ModelFileHeader* arg0, ModelNode* arg1)
{
    ModelSkeleton* onescreen = &skeleton_console_one_screen;
    ModelSkeleton* fourscreen = &skeleton_console_four_screen;

    if ((onescreen == arg0->Skeleton))
    {
        if (arg1 == arg0->Switches[0])
        {
            return FALSE;
        }
    }

    if (fourscreen == arg0->Skeleton)
    {
        if ((arg1 == arg0->Switches[0]) || (arg1 == arg0->Switches[1]) || (arg1 == arg0->Switches[2]) || (arg1 == arg0->Switches[3]))
        {
            return FALSE;
        }
    }

    return TRUE;
}


#ifdef NONMATCHING
/*
*   objDeform - Deform an object due to it being destroyed.
*   PD has a very similar function of the same name
*   Address: 7F04B610
*/
void objDeform(ObjectRecord *obj, s32 arg1) {
    s32 sp7C;
    s32 sp78;
    s32 sp70;
    f32 sp68;
    f32 sp5C;
    f32 sp44;
    s32 sp40;
    ModelNode *temp_v0;
    Vertex **temp_s3;
    Vertex *temp_a0_2;
    Vertex *temp_v0_3;
    Vertex *var_s0;
    Vertex *var_v1;
    Vertex *var_v1_2;
    f32 temp_s1;
    f32 temp_v0_8;
    f32 var_f6;
    s16 *temp_s0_2;
    s16 temp_a0;
    s16 temp_v0_4;
    s16 temp_v0_5;
    s16 var_s4;
    s16 var_s6;
    s32 *temp_t4;
    s32 *temp_t7;
    s32 temp_at;
    s32 temp_at_2;
    s32 temp_t1;
    s32 temp_t3;
    s32 temp_v0_2;
    s32 temp_v0_6;
    s32 temp_v0_9;
    s32 var_s0_2;
    s32 var_s1;
    s32 var_s1_2;
    s32 var_s1_3;
    s32 var_s1_4;
    s32 var_s2;
    s32 var_s7;
    s32 var_v1_3;
    void *temp_s0;
    void *temp_s0_3;
    void *temp_s0_4;
    void *temp_s0_5;
    void *temp_s5;
    void *temp_v0_10;
    void *temp_v0_7;

    temp_s1 = obj->mtx.m[0][0];
    var_s6 = 0x1869F;
    var_s4 = 0xFFFE7961;
    temp_v0 = sub_GAME_7F04B478(obj);
    if ((temp_v0 != NULL) && (temp_s5 = temp_v0->Data, (temp_s5 != NULL)) && (sub_GAME_7F04B590(obj->mtx.m[0][0]->unk8, temp_v0) != 0)) {
        temp_s3 = temp_s1->unk10 + (temp_s5->unk1A * 4);
        temp_t1 = arg1 * 2;
        if (randomGetNext() & 1) {
            sp40 = temp_t1;
            sp78 = (s32) *(object_explosion_details->Seed + ((obj->unk4 * 0xE) + temp_t1));
        } else {
            temp_v0_2 = arg1 * 2;
            sp40 = temp_v0_2;
            sp78 = (s32) *(&object_explosion_details->Seed[3] + ((obj->unk4 * 0xE) + temp_v0_2));
        }
        if ((get_debug_explosioninfo_flag() != 0) || (sp78 == 0)) {
            get_debug_explosioninfo_flag();
            sp78 = randomGetNext();
            if (get_debug_explosioninfo_flag() != 0) {
                sp78 &= 0xFFFF;
            }
        }
        explosionClearBulletImpactRoom((PropRecord *) obj->model);
        var_s7 = 1;
        if (obj->unk4 == 0x4C) {
            var_s7 = 0;
        }
        temp_v0_3 = sub_GAME_7F09BE4C(temp_s5->unkC, 0xB0B, temp_s1->unk8, objGetDestroyedLevel(obj));
        if (temp_v0_3 != NULL) {
            if (temp_s5->unk8 != *temp_s3) {
                var_s2 = 0;
                var_s1 = 0;
                var_v1 = temp_v0_3;
                if (temp_s5->unkC > 0) {
                    do {
                        var_s2 += 1;
                        var_v1 += 0x10;
                        temp_t7 = *temp_s3 + var_s1;
                        temp_at = temp_t7->unk0;
                        var_s1 += 0x10;
                        var_v1->unk-10 = temp_at;
                        var_v1->unk-C = (s32) temp_t7->unk4;
                        var_v1->unk-8 = (s32) temp_t7->unk8;
                        var_v1->unk-4 = (s32) temp_t7->unkC;
                    } while (var_s2 < temp_s5->unkC);
                    var_s2 = 0;
                }
                sub_GAME_7F09C044(*temp_s3);
            } else {
                var_s2 = 0;
                var_s1_2 = 0;
                var_v1_2 = temp_v0_3;
                if (temp_s5->unkC > 0) {
                    do {
                        var_s2 += 1;
                        var_v1_2 += 0x10;
                        temp_t4 = temp_s5->unk8 + var_s1_2;
                        temp_at_2 = temp_t4->unk0;
                        var_s1_2 += 0x10;
                        var_v1_2->unk-10 = temp_at_2;
                        var_v1_2->unk-C = (s32) temp_t4->unk4;
                        var_v1_2->unk-8 = (s32) temp_t4->unk8;
                        var_v1_2->unk-4 = (s32) temp_t4->unkC;
                    } while (var_s2 < temp_s5->unkC);
                    var_s2 = 0;
                }
            }
            *temp_s3 = temp_v0_3;
            temp_v0_4 = obj->unk4;
            var_s1_3 = 0;
            if ((temp_v0_4 == 0x26) || (temp_v0_4 == 0x27) || (temp_v0_4 == 0x55)) {
                sp70 = 0;
            } else {
                sp70 = 1;
            }
            temp_a0 = temp_s5->unkC;
            if (temp_a0 > 0) {
                var_s0 = *temp_s3;
                do {
                    temp_v0_5 = var_s0->coord.AsArray[1];
                    var_s1_3 += 0x10;
                    if (temp_v0_5 < var_s6) {
                        var_s6 = temp_v0_5;
                    }
                    if (var_s4 < temp_v0_5) {
                        var_s4 = temp_v0_5;
                    }
                    var_s0 += 0x10;
                } while (var_s1_3 < (temp_a0 * 0x10));
                var_s2 = 0;
            }
            temp_t3 = (s32) (var_s6 + var_s4) >> 1;
            var_v1_3 = temp_t3;
            if ((sp40 == 6) && ((temp_t3 - var_s6) >= 0x29)) {
                var_v1_3 = var_s6 + 0x28;
            }
            temp_v0_6 = var_s4 - var_s6;
            if (temp_v0_6 >= 0x3D) {
                if (sp40 < 6) {
                    var_f6 = D_80052CC0;
                    goto block_47;
                }
                sp68 = (f32) (temp_v0_6 - 0x3C) / (f32) temp_v0_6;
            } else {
                var_f6 = 1.0f;
block_47:
                sp68 = var_f6;
            }
            if (temp_a0 > 0) {
                var_s1_4 = 0;
                sp7C = var_v1_3;
                sp44 = (f32) var_s6;
                do {
                    temp_v0_7 = temp_s5->unk8 + var_s1_4;
                    chrObjRandomSetSeed(temp_v0_7->unk4 + temp_v0_7->unk0 + temp_v0_7->unk2 + sp78);
                    var_s0_2 = 0;
                    if (sp70 != 0) {
                        if (obj->mtx.m[1][2] >= 0.0f) {
                            if ((*temp_s3 + var_s1_4)->unk2 >= sp7C) {
                                if (var_s7 != 0) {
                                    var_s0_2 = 0x5A;
                                } else {
                                    var_s0_2 = 0x14;
                                }
                            } else if (var_s7 != 0) {
                                var_s0_2 = 0x14;
                            } else {
                                var_s0_2 = 0x5A;
                            }
                        } else if (sp7C >= (*temp_s3 + var_s1_4)->unk2) {
                            if (var_s7 != 0) {
                                var_s0_2 = 0x5A;
                            } else {
                                var_s0_2 = 0x14;
                            }
                        } else if (var_s7 != 0) {
                            var_s0_2 = 0x14;
                        } else {
                            var_s0_2 = 0x5A;
                        }
                    }
                    if ((s32) (chrObjRandomGetNext() % 100U) < var_s0_2) {
                        (*temp_s3 + var_s1_4)->unkC = 0;
                        (*temp_s3 + var_s1_4)->unkD = 0;
                        (*temp_s3 + var_s1_4)->unkE = 0;
                        (*temp_s3 + var_s1_4)->unkF = 0xFF;
                    } else if (sp40 == 2) {
                        (*temp_s3 + var_s1_4)->unkF = 0;
                    }
                    temp_s0 = *temp_s3 + var_s1_4;
                    temp_s0->unk2 = (s16) (s32) (((f32) (temp_s0->unk2 - var_s6) * sp68) + sp44);
                    temp_s0_2 = *temp_s3 + var_s1_4;
                    *temp_s0_2 = (*temp_s0_2 + (chrObjRandomGetNext() % 80U)) - 0x28;
                    temp_s0_3 = *temp_s3 + var_s1_4;
                    temp_s0_3->unk2 = (s16) ((temp_s0_3->unk2 + (chrObjRandomGetNext() % 80U)) - 0x28);
                    temp_s0_4 = *temp_s3 + var_s1_4;
                    temp_s0_4->unk4 = (s16) ((temp_s0_4->unk4 + (chrObjRandomGetNext() % 80U)) - 0x28);
                    temp_s0_5 = *temp_s3 + var_s1_4;
                    if (temp_s0_5->unk2 < var_s6) {
                        temp_s0_5->unk2 = var_s6;
                    }
                    var_s2 += 1;
                    var_s1_4 += 0x10;
                } while (var_s2 < temp_s5->unkC);
            }
        } else {
            temp_a0_2 = *temp_s3;
            if (temp_s5->unk8 != temp_a0_2) {
                sub_GAME_7F09C044(temp_a0_2);
                *temp_s3 = temp_s5->unk8;
                obj->ptr_allocated_collisiondata_block = (struct collision_data *) ((s32) obj->ptr_allocated_collisiondata_block | 4);
            }
            obj->mtx.m[1][1] *= D_80052CB4;
            obj->mtx.m[1][2] *= D_80052CB4;
            obj->mtx.m[1][3] *= D_80052CB4;
            if (var_s7 != 0) {
                temp_v0_8 = obj->mtx.m[0][0];
                sp5C = temp_v0_8->unk14;
                temp_v0_9 = chrobjGetBboxFromObjFile(temp_v0_8->unk8);
                temp_v0_10 = obj->model;
                temp_v0_10->unkC = (f32) (temp_v0_10->unkC + (sp5C * chrpropBBOXGetYmin(temp_v0_9) * D_80052CB8));
                obj->runtime_pos.f[2] += sp5C * chrpropBBOXGetYmin(temp_v0_9) * D_80052CBC;
            }
        }
    }
}
#else
#ifndef VERSION_EU
GLOBAL_ASM(
.text
glabel objDeform
/* 080140 7F04B610 27BDFF68 */  addiu $sp, $sp, -0x98
/* 080144 7F04B614 AFB60030 */  sw    $s6, 0x30($sp)
/* 080148 7F04B618 AFB40028 */  sw    $s4, 0x28($sp)
/* 08014C 7F04B61C AFBF003C */  sw    $ra, 0x3c($sp)
/* 080150 7F04B620 AFBE0038 */  sw    $fp, 0x38($sp)
/* 080154 7F04B624 AFB70034 */  sw    $s7, 0x34($sp)
/* 080158 7F04B628 AFB5002C */  sw    $s5, 0x2c($sp)
/* 08015C 7F04B62C AFB30024 */  sw    $s3, 0x24($sp)
/* 080160 7F04B630 AFB20020 */  sw    $s2, 0x20($sp)
/* 080164 7F04B634 AFB1001C */  sw    $s1, 0x1c($sp)
/* 080168 7F04B638 AFB00018 */  sw    $s0, 0x18($sp)
/* 08016C 7F04B63C 3C14FFFE */  lui   $s4, (0xFFFE7961 >> 16) # lui $s4, 0xfffe
/* 080170 7F04B640 3C160001 */  li    $s6,0x00010000 # 0.000000
/* 080174 7F04B644 00A08025 */  move  $s0, $a1
/* 080178 7F04B648 0080F025 */  move  $fp, $a0
/* 08017C 7F04B64C 8C910014 */  lw    $s1, 0x14($a0)
/* 080180 7F04B650 36D6869F */  ori   $s6, (0x0001869F & 0xFFFF) # ori $s6, $s6, 0x869f
/* 080184 7F04B654 0FC12D1E */  jal   sub_GAME_7F04B478
/* 080188 7F04B658 36947961 */   ori   $s4, (0xFFFE7961 & 0xFFFF) # ori $s4, $s4, 0x7961
/* 08018C 7F04B65C 10400193 */  beqz  $v0, .L7F04BCAC
/* 080190 7F04B660 00402825 */   move  $a1, $v0
/* 080194 7F04B664 8C550004 */  lw    $s5, 4($v0)
/* 080198 7F04B668 52A00191 */  beql  $s5, $zero, .L7F04BCB0
/* 08019C 7F04B66C 8FBF003C */   lw    $ra, 0x3c($sp)
/* 0801A0 7F04B670 8FCE0014 */  lw    $t6, 0x14($fp)
/* 0801A4 7F04B674 0FC12D64 */  jal   sub_GAME_7F04B590
/* 0801A8 7F04B678 8DC40008 */   lw    $a0, 8($t6)
/* 0801AC 7F04B67C 5040018C */  beql  $v0, $zero, .L7F04BCB0
/* 0801B0 7F04B680 8FBF003C */   lw    $ra, 0x3c($sp)
/* 0801B4 7F04B684 96B8001A */  lhu   $t8, 0x1a($s5)
/* 0801B8 7F04B688 8E2F0010 */  lw    $t7, 0x10($s1)
/* 0801BC 7F04B68C 0018C880 */  sll   $t9, $t8, 2
/* 0801C0 7F04B690 0C002914 */  jal   randomGetNext
/* 0801C4 7F04B694 01F99821 */   addu  $s3, $t7, $t9
/* 0801C8 7F04B698 30480001 */  andi  $t0, $v0, 1
/* 0801CC 7F04B69C 1100000C */  beqz  $t0, .L7F04B6D0
/* 0801D0 7F04B6A0 00104840 */   sll   $t1, $s0, 1
/* 0801D4 7F04B6A4 AFA90040 */  sw    $t1, 0x40($sp)
/* 0801D8 7F04B6A8 87CA0004 */  lh    $t2, 4($fp)
/* 0801DC 7F04B6AC 3C0E8004 */  lui   $t6, %hi(object_explosion_details+2)
/* 0801E0 7F04B6B0 000A58C0 */  sll   $t3, $t2, 3
/* 0801E4 7F04B6B4 016A5823 */  subu  $t3, $t3, $t2
/* 0801E8 7F04B6B8 000B5840 */  sll   $t3, $t3, 1
/* 0801EC 7F04B6BC 01696821 */  addu  $t5, $t3, $t1
/* 0801F0 7F04B6C0 01CD7021 */  addu  $t6, $t6, $t5
/* 0801F4 7F04B6C4 95CEB226 */  lhu   $t6, %lo(object_explosion_details+2)($t6)
/* 0801F8 7F04B6C8 1000000C */  b     .L7F04B6FC
/* 0801FC 7F04B6CC AFAE0078 */   sw    $t6, 0x78($sp)
.L7F04B6D0:
/* 080200 7F04B6D0 87D80004 */  lh    $t8, 4($fp)
/* 080204 7F04B6D4 00101040 */  sll   $v0, $s0, 1
/* 080208 7F04B6D8 3C088004 */  lui   $t0, %hi(object_explosion_details+8)
/* 08020C 7F04B6DC 001878C0 */  sll   $t7, $t8, 3
/* 080210 7F04B6E0 01F87823 */  subu  $t7, $t7, $t8
/* 080214 7F04B6E4 000F7840 */  sll   $t7, $t7, 1
/* 080218 7F04B6E8 01E2C821 */  addu  $t9, $t7, $v0
/* 08021C 7F04B6EC 01194021 */  addu  $t0, $t0, $t9
/* 080220 7F04B6F0 9508B22C */  lhu   $t0, %lo(object_explosion_details+8)($t0)
/* 080224 7F04B6F4 AFA20040 */  sw    $v0, 0x40($sp)
/* 080228 7F04B6F8 AFA80078 */  sw    $t0, 0x78($sp)
.L7F04B6FC:
/* 08022C 7F04B6FC 0FC243F1 */  jal   get_debug_explosioninfo_flag
/* 080230 7F04B700 00000000 */   nop
/* 080234 7F04B704 14400003 */  bnez  $v0, .L7F04B714
/* 080238 7F04B708 8FA90078 */   lw    $t1, 0x78($sp)
/* 08023C 7F04B70C 1520000B */  bnez  $t1, .L7F04B73C
/* 080240 7F04B710 00000000 */   nop
.L7F04B714:
/* 080244 7F04B714 0FC243F1 */  jal   get_debug_explosioninfo_flag
/* 080248 7F04B718 00000000 */   nop
/* 08024C 7F04B71C 0C002914 */  jal   randomGetNext
/* 080250 7F04B720 00000000 */   nop
/* 080254 7F04B724 0FC243F1 */  jal   get_debug_explosioninfo_flag
/* 080258 7F04B728 AFA20078 */   sw    $v0, 0x78($sp)
/* 08025C 7F04B72C 10400003 */  beqz  $v0, .L7F04B73C
/* 080260 7F04B730 8FAA0078 */   lw    $t2, 0x78($sp)
/* 080264 7F04B734 314BFFFF */  andi  $t3, $t2, 0xffff
/* 080268 7F04B738 AFAB0078 */  sw    $t3, 0x78($sp)
.L7F04B73C:
/* 08026C 7F04B73C 0FC28364 */  jal   explosionClearBulletImpactRoom
/* 080270 7F04B740 8FC40010 */   lw    $a0, 0x10($fp)
/* 080274 7F04B744 87CC0004 */  lh    $t4, 4($fp)
/* 080278 7F04B748 2401004C */  li    $at, 76
/* 08027C 7F04B74C 24170001 */  li    $s7, 1
/* 080280 7F04B750 15810003 */  bne   $t4, $at, .L7F04B760
/* 080284 7F04B754 00000000 */   nop
/* 080288 7F04B758 10000001 */  b     .L7F04B760
/* 08028C 7F04B75C 0000B825 */   move  $s7, $zero
.L7F04B760:
/* 080290 7F04B760 0FC0FFF0 */  jal   objGetDestroyedLevel
/* 080294 7F04B764 03C02025 */   move  $a0, $fp
/* 080298 7F04B768 86A4000C */  lh    $a0, 0xc($s5)
/* 08029C 7F04B76C 24050B0B */  li    $a1, 2827
/* 0802A0 7F04B770 8E260008 */  lw    $a2, 8($s1)
/* 0802A4 7F04B774 0FC26F93 */  jal   sub_GAME_7F09BE4C
/* 0802A8 7F04B778 00403825 */   move  $a3, $v0
/* 0802AC 7F04B77C 1040003A */  beqz  $v0, .L7F04B868
/* 0802B0 7F04B780 00408025 */   move  $s0, $v0
/* 0802B4 7F04B784 8E640000 */  lw    $a0, ($s3)
/* 0802B8 7F04B788 8EAD0008 */  lw    $t5, 8($s5)
/* 0802BC 7F04B78C 51A4001E */  beql  $t5, $a0, .L7F04B808
/* 0802C0 7F04B790 86AA000C */   lh    $t2, 0xc($s5)
/* 0802C4 7F04B794 86AE000C */  lh    $t6, 0xc($s5)
/* 0802C8 7F04B798 00009025 */  move  $s2, $zero
/* 0802CC 7F04B79C 00008825 */  move  $s1, $zero
/* 0802D0 7F04B7A0 19C00014 */  blez  $t6, .L7F04B7F4
/* 0802D4 7F04B7A4 00401825 */   move  $v1, $v0
/* 0802D8 7F04B7A8 8E780000 */  lw    $t8, ($s3)
.L7F04B7AC:
/* 0802DC 7F04B7AC 26520001 */  addiu $s2, $s2, 1
/* 0802E0 7F04B7B0 24630010 */  addiu $v1, $v1, 0x10
/* 0802E4 7F04B7B4 03117821 */  addu  $t7, $t8, $s1
/* 0802E8 7F04B7B8 8DE10000 */  lw    $at, ($t7)
/* 0802EC 7F04B7BC 26310010 */  addiu $s1, $s1, 0x10
/* 0802F0 7F04B7C0 AC61FFF0 */  sw    $at, -0x10($v1)
/* 0802F4 7F04B7C4 8DE80004 */  lw    $t0, 4($t7)
/* 0802F8 7F04B7C8 AC68FFF4 */  sw    $t0, -0xc($v1)
/* 0802FC 7F04B7CC 8DE10008 */  lw    $at, 8($t7)
/* 080300 7F04B7D0 AC61FFF8 */  sw    $at, -8($v1)
/* 080304 7F04B7D4 8DE8000C */  lw    $t0, 0xc($t7)
/* 080308 7F04B7D8 AC68FFFC */  sw    $t0, -4($v1)
/* 08030C 7F04B7DC 86A9000C */  lh    $t1, 0xc($s5)
/* 080310 7F04B7E0 0249082A */  slt   $at, $s2, $t1
/* 080314 7F04B7E4 5420FFF1 */  bnezl $at, .L7F04B7AC
/* 080318 7F04B7E8 8E780000 */   lw    $t8, ($s3)
/* 08031C 7F04B7EC 8E640000 */  lw    $a0, ($s3)
/* 080320 7F04B7F0 00009025 */  move  $s2, $zero
.L7F04B7F4:
/* 080324 7F04B7F4 0FC27011 */  jal   sub_GAME_7F09C044
/* 080328 7F04B7F8 00000000 */   nop
/* 08032C 7F04B7FC 10000018 */  b     .L7F04B860
/* 080330 7F04B800 00000000 */   nop
/* 080334 7F04B804 86AA000C */  lh    $t2, 0xc($s5)
.L7F04B808:
/* 080338 7F04B808 00009025 */  move  $s2, $zero
/* 08033C 7F04B80C 00008825 */  move  $s1, $zero
/* 080340 7F04B810 19400013 */  blez  $t2, .L7F04B860
/* 080344 7F04B814 00401825 */   move  $v1, $v0
/* 080348 7F04B818 8EAB0008 */  lw    $t3, 8($s5)
.L7F04B81C:
/* 08034C 7F04B81C 26520001 */  addiu $s2, $s2, 1
/* 080350 7F04B820 24630010 */  addiu $v1, $v1, 0x10
/* 080354 7F04B824 01716021 */  addu  $t4, $t3, $s1
/* 080358 7F04B828 8D810000 */  lw    $at, ($t4)
/* 08035C 7F04B82C 26310010 */  addiu $s1, $s1, 0x10
/* 080360 7F04B830 AC61FFF0 */  sw    $at, -0x10($v1)
/* 080364 7F04B834 8D8E0004 */  lw    $t6, 4($t4)
/* 080368 7F04B838 AC6EFFF4 */  sw    $t6, -0xc($v1)
/* 08036C 7F04B83C 8D810008 */  lw    $at, 8($t4)
/* 080370 7F04B840 AC61FFF8 */  sw    $at, -8($v1)
/* 080374 7F04B844 8D8E000C */  lw    $t6, 0xc($t4)
/* 080378 7F04B848 AC6EFFFC */  sw    $t6, -4($v1)
/* 08037C 7F04B84C 86B8000C */  lh    $t8, 0xc($s5)
/* 080380 7F04B850 0258082A */  slt   $at, $s2, $t8
/* 080384 7F04B854 5420FFF1 */  bnezl $at, .L7F04B81C
/* 080388 7F04B858 8EAB0008 */   lw    $t3, 8($s5)
/* 08038C 7F04B85C 00009025 */  move  $s2, $zero
.L7F04B860:
/* 080390 7F04B860 10000036 */  b     .L7F04B93C
/* 080394 7F04B864 AE700000 */   sw    $s0, ($s3)
.L7F04B868:
/* 080398 7F04B868 8E640000 */  lw    $a0, ($s3)
/* 08039C 7F04B86C 8EB90008 */  lw    $t9, 8($s5)
/* 0803A0 7F04B870 13240008 */  beq   $t9, $a0, .L7F04B894
/* 0803A4 7F04B874 00000000 */   nop
/* 0803A8 7F04B878 0FC27011 */  jal   sub_GAME_7F09C044
/* 0803AC 7F04B87C 00000000 */   nop
/* 0803B0 7F04B880 8EAF0008 */  lw    $t7, 8($s5)
/* 0803B4 7F04B884 AE6F0000 */  sw    $t7, ($s3)
/* 0803B8 7F04B888 8FC80064 */  lw    $t0, 0x64($fp)
/* 0803BC 7F04B88C 35090004 */  ori   $t1, $t0, 4
/* 0803C0 7F04B890 AFC90064 */  sw    $t1, 0x64($fp)
.L7F04B894:
/* 0803C4 7F04B894 3C018005 */  lui   $at, %hi(D_80052CB4)
/* 0803C8 7F04B898 C4202CB4 */  lwc1  $f0, %lo(D_80052CB4)($at)
/* 0803CC 7F04B89C C7C40028 */  lwc1  $f4, 0x28($fp)
/* 0803D0 7F04B8A0 C7C8002C */  lwc1  $f8, 0x2c($fp)
/* 0803D4 7F04B8A4 C7D00030 */  lwc1  $f16, 0x30($fp)
/* 0803D8 7F04B8A8 46002182 */  mul.s $f6, $f4, $f0
/* 0803DC 7F04B8AC 00000000 */  nop
/* 0803E0 7F04B8B0 46004282 */  mul.s $f10, $f8, $f0
/* 0803E4 7F04B8B4 00000000 */  nop
/* 0803E8 7F04B8B8 46008482 */  mul.s $f18, $f16, $f0
/* 0803EC 7F04B8BC E7C60028 */  swc1  $f6, 0x28($fp)
/* 0803F0 7F04B8C0 E7CA002C */  swc1  $f10, 0x2c($fp)
/* 0803F4 7F04B8C4 12E000F9 */  beqz  $s7, .L7F04BCAC
/* 0803F8 7F04B8C8 E7D20030 */   swc1  $f18, 0x30($fp)
/* 0803FC 7F04B8CC 8FC20014 */  lw    $v0, 0x14($fp)
/* 080400 7F04B8D0 C4440014 */  lwc1  $f4, 0x14($v0)
/* 080404 7F04B8D4 8C440008 */  lw    $a0, 8($v0)
/* 080408 7F04B8D8 0FC0FFFE */  jal   chrobjGetBboxFromObjFile
/* 08040C 7F04B8DC E7A4005C */   swc1  $f4, 0x5c($sp)
/* 080410 7F04B8E0 00408025 */  move  $s0, $v0
/* 080414 7F04B8E4 0FC0FA19 */  jal   chrpropBBOXGetYmin
/* 080418 7F04B8E8 00402025 */   move  $a0, $v0
/* 08041C 7F04B8EC C7A6005C */  lwc1  $f6, 0x5c($sp)
/* 080420 7F04B8F0 3C018005 */  lui   $at, %hi(D_80052CB8)
/* 080424 7F04B8F4 C42A2CB8 */  lwc1  $f10, %lo(D_80052CB8)($at)
/* 080428 7F04B8F8 46003202 */  mul.s $f8, $f6, $f0
/* 08042C 7F04B8FC 8FC20010 */  lw    $v0, 0x10($fp)
/* 080430 7F04B900 02002025 */  move  $a0, $s0
/* 080434 7F04B904 C452000C */  lwc1  $f18, 0xc($v0)
/* 080438 7F04B908 460A4402 */  mul.s $f16, $f8, $f10
/* 08043C 7F04B90C 46109100 */  add.s $f4, $f18, $f16
/* 080440 7F04B910 0FC0FA19 */  jal   chrpropBBOXGetYmin
/* 080444 7F04B914 E444000C */   swc1  $f4, 0xc($v0)
/* 080448 7F04B918 C7A6005C */  lwc1  $f6, 0x5c($sp)
/* 08044C 7F04B91C 3C018005 */  lui   $at, %hi(D_80052CBC)
/* 080450 7F04B920 C42A2CBC */  lwc1  $f10, %lo(D_80052CBC)($at)
/* 080454 7F04B924 46003202 */  mul.s $f8, $f6, $f0
/* 080458 7F04B928 C7D0005C */  lwc1  $f16, 0x5c($fp)
/* 08045C 7F04B92C 460A4482 */  mul.s $f18, $f8, $f10
/* 080460 7F04B930 46128100 */  add.s $f4, $f16, $f18
/* 080464 7F04B934 100000DD */  b     .L7F04BCAC
/* 080468 7F04B938 E7C4005C */   swc1  $f4, 0x5c($fp)
.L7F04B93C:
/* 08046C 7F04B93C 87C20004 */  lh    $v0, 4($fp)
/* 080470 7F04B940 24010026 */  li    $at, 38
/* 080474 7F04B944 00008825 */  move  $s1, $zero
/* 080478 7F04B948 10410005 */  beq   $v0, $at, .L7F04B960
/* 08047C 7F04B94C 24010027 */   li    $at, 39
/* 080480 7F04B950 10410003 */  beq   $v0, $at, .L7F04B960
/* 080484 7F04B954 24010055 */   li    $at, 85
/* 080488 7F04B958 14410003 */  bne   $v0, $at, .L7F04B968
/* 08048C 7F04B95C 240A0001 */   li    $t2, 1
.L7F04B960:
/* 080490 7F04B960 10000002 */  b     .L7F04B96C
/* 080494 7F04B964 AFA00070 */   sw    $zero, 0x70($sp)
.L7F04B968:
/* 080498 7F04B968 AFAA0070 */  sw    $t2, 0x70($sp)
.L7F04B96C:
/* 08049C 7F04B96C 86A4000C */  lh    $a0, 0xc($s5)
/* 0804A0 7F04B970 8FA50040 */  lw    $a1, 0x40($sp)
/* 0804A4 7F04B974 18800010 */  blez  $a0, .L7F04B9B8
/* 0804A8 7F04B978 00041900 */   sll   $v1, $a0, 4
/* 0804AC 7F04B97C 8E700000 */  lw    $s0, ($s3)
.L7F04B980:
/* 0804B0 7F04B980 86020002 */  lh    $v0, 2($s0)
/* 0804B4 7F04B984 26310010 */  addiu $s1, $s1, 0x10
/* 0804B8 7F04B988 0056082A */  slt   $at, $v0, $s6
/* 0804BC 7F04B98C 50200003 */  beql  $at, $zero, .L7F04B99C
/* 0804C0 7F04B990 0282082A */   slt   $at, $s4, $v0
/* 0804C4 7F04B994 0040B025 */  move  $s6, $v0
/* 0804C8 7F04B998 0282082A */  slt   $at, $s4, $v0
.L7F04B99C:
/* 0804CC 7F04B99C 50200003 */  beql  $at, $zero, .L7F04B9AC
/* 0804D0 7F04B9A0 0223082A */   slt   $at, $s1, $v1
/* 0804D4 7F04B9A4 0040A025 */  move  $s4, $v0
/* 0804D8 7F04B9A8 0223082A */  slt   $at, $s1, $v1
.L7F04B9AC:
/* 0804DC 7F04B9AC 1420FFF4 */  bnez  $at, .L7F04B980
/* 0804E0 7F04B9B0 26100010 */   addiu $s0, $s0, 0x10
/* 0804E4 7F04B9B4 00009025 */  move  $s2, $zero
.L7F04B9B8:
/* 0804E8 7F04B9B8 02D41021 */  addu  $v0, $s6, $s4
/* 0804EC 7F04B9BC 00025843 */  sra   $t3, $v0, 1
/* 0804F0 7F04B9C0 24010006 */  li    $at, 6
/* 0804F4 7F04B9C4 14A10006 */  bne   $a1, $at, .L7F04B9E0
/* 0804F8 7F04B9C8 01601825 */   move  $v1, $t3
/* 0804FC 7F04B9CC 01766823 */  subu  $t5, $t3, $s6
/* 080500 7F04B9D0 29A10029 */  slti  $at, $t5, 0x29
/* 080504 7F04B9D4 54200003 */  bnezl $at, .L7F04B9E4
/* 080508 7F04B9D8 02961023 */   subu  $v0, $s4, $s6
/* 08050C 7F04B9DC 26C30028 */  addiu $v1, $s6, 0x28
.L7F04B9E0:
/* 080510 7F04B9E0 02961023 */  subu  $v0, $s4, $s6
.L7F04B9E4:
/* 080514 7F04B9E4 2841003D */  slti  $at, $v0, 0x3d
/* 080518 7F04B9E8 1420000E */  bnez  $at, .L7F04BA24
/* 08051C 7F04B9EC 28A10006 */   slti  $at, $a1, 6
/* 080520 7F04B9F0 10200005 */  beqz  $at, .L7F04BA08
/* 080524 7F04B9F4 244CFFC4 */   addiu $t4, $v0, -0x3c
/* 080528 7F04B9F8 3C018005 */  lui   $at, %hi(D_80052CC0)
/* 08052C 7F04B9FC C4262CC0 */  lwc1  $f6, %lo(D_80052CC0)($at)
/* 080530 7F04BA00 1000000C */  b     .L7F04BA34
/* 080534 7F04BA04 E7A60068 */   swc1  $f6, 0x68($sp)
.L7F04BA08:
/* 080538 7F04BA08 448C4000 */  mtc1  $t4, $f8
/* 08053C 7F04BA0C 44828000 */  mtc1  $v0, $f16
/* 080540 7F04BA10 468042A0 */  cvt.s.w $f10, $f8
/* 080544 7F04BA14 468084A0 */  cvt.s.w $f18, $f16
/* 080548 7F04BA18 46125103 */  div.s $f4, $f10, $f18
/* 08054C 7F04BA1C 10000005 */  b     .L7F04BA34
/* 080550 7F04BA20 E7A40068 */   swc1  $f4, 0x68($sp)
.L7F04BA24:
/* 080554 7F04BA24 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 080558 7F04BA28 44813000 */  mtc1  $at, $f6
/* 08055C 7F04BA2C 00000000 */  nop
/* 080560 7F04BA30 E7A60068 */  swc1  $f6, 0x68($sp)
.L7F04BA34:
/* 080564 7F04BA34 5880009E */  blezl $a0, .L7F04BCB0
/* 080568 7F04BA38 8FBF003C */   lw    $ra, 0x3c($sp)
/* 08056C 7F04BA3C 44964000 */  mtc1  $s6, $f8
/* 080570 7F04BA40 00008825 */  move  $s1, $zero
/* 080574 7F04BA44 AFA3007C */  sw    $v1, 0x7c($sp)
/* 080578 7F04BA48 46804420 */  cvt.s.w $f16, $f8
/* 08057C 7F04BA4C 24140050 */  li    $s4, 80
/* 080580 7F04BA50 E7B00044 */  swc1  $f16, 0x44($sp)
/* 080584 7F04BA54 8EAE0008 */  lw    $t6, 8($s5)
.L7F04BA58:
/* 080588 7F04BA58 8FAA0078 */  lw    $t2, 0x78($sp)
/* 08058C 7F04BA5C 01D11021 */  addu  $v0, $t6, $s1
/* 080590 7F04BA60 84580004 */  lh    $t8, 4($v0)
/* 080594 7F04BA64 84590000 */  lh    $t9, ($v0)
/* 080598 7F04BA68 84480002 */  lh    $t0, 2($v0)
/* 08059C 7F04BA6C 03197821 */  addu  $t7, $t8, $t9
/* 0805A0 7F04BA70 01E84821 */  addu  $t1, $t7, $t0
/* 0805A4 7F04BA74 0FC26DE2 */  jal   chrObjRandomSetSeed
/* 0805A8 7F04BA78 012A2021 */   addu  $a0, $t1, $t2
/* 0805AC 7F04BA7C 8FAB0070 */  lw    $t3, 0x70($sp)
/* 0805B0 7F04BA80 00008025 */  move  $s0, $zero
/* 0805B4 7F04BA84 1160002E */  beqz  $t3, .L7F04BB40
/* 0805B8 7F04BA88 00000000 */   nop
/* 0805BC 7F04BA8C C7CA002C */  lwc1  $f10, 0x2c($fp)
/* 0805C0 7F04BA90 44809000 */  mtc1  $zero, $f18
/* 0805C4 7F04BA94 00000000 */  nop
/* 0805C8 7F04BA98 460A903E */  c.le.s $f18, $f10
/* 0805CC 7F04BA9C 00000000 */  nop
/* 0805D0 7F04BAA0 45020015 */  bc1fl .L7F04BAF8
/* 0805D4 7F04BAA4 8E6F0000 */   lw    $t7, ($s3)
/* 0805D8 7F04BAA8 8E6D0000 */  lw    $t5, ($s3)
/* 0805DC 7F04BAAC 8FB8007C */  lw    $t8, 0x7c($sp)
/* 0805E0 7F04BAB0 01B16021 */  addu  $t4, $t5, $s1
/* 0805E4 7F04BAB4 858E0002 */  lh    $t6, 2($t4)
/* 0805E8 7F04BAB8 01D8082A */  slt   $at, $t6, $t8
/* 0805EC 7F04BABC 14200007 */  bnez  $at, .L7F04BADC
/* 0805F0 7F04BAC0 00000000 */   nop
/* 0805F4 7F04BAC4 12E00003 */  beqz  $s7, .L7F04BAD4
/* 0805F8 7F04BAC8 00000000 */   nop
/* 0805FC 7F04BACC 1000001C */  b     .L7F04BB40
/* 080600 7F04BAD0 2410005A */   li    $s0, 90
.L7F04BAD4:
/* 080604 7F04BAD4 1000001A */  b     .L7F04BB40
/* 080608 7F04BAD8 24100014 */   li    $s0, 20
.L7F04BADC:
/* 08060C 7F04BADC 12E00003 */  beqz  $s7, .L7F04BAEC
/* 080610 7F04BAE0 00000000 */   nop
/* 080614 7F04BAE4 10000016 */  b     .L7F04BB40
/* 080618 7F04BAE8 24100014 */   li    $s0, 20
.L7F04BAEC:
/* 08061C 7F04BAEC 10000014 */  b     .L7F04BB40
/* 080620 7F04BAF0 2410005A */   li    $s0, 90
/* 080624 7F04BAF4 8E6F0000 */  lw    $t7, ($s3)
.L7F04BAF8:
/* 080628 7F04BAF8 8FB9007C */  lw    $t9, 0x7c($sp)
/* 08062C 7F04BAFC 01F14021 */  addu  $t0, $t7, $s1
/* 080630 7F04BB00 85090002 */  lh    $t1, 2($t0)
/* 080634 7F04BB04 0329082A */  slt   $at, $t9, $t1
/* 080638 7F04BB08 14200007 */  bnez  $at, .L7F04BB28
/* 08063C 7F04BB0C 00000000 */   nop
/* 080640 7F04BB10 12E00003 */  beqz  $s7, .L7F04BB20
/* 080644 7F04BB14 00000000 */   nop
/* 080648 7F04BB18 10000009 */  b     .L7F04BB40
/* 08064C 7F04BB1C 2410005A */   li    $s0, 90
.L7F04BB20:
/* 080650 7F04BB20 10000007 */  b     .L7F04BB40
/* 080654 7F04BB24 24100014 */   li    $s0, 20
.L7F04BB28:
/* 080658 7F04BB28 12E00003 */  beqz  $s7, .L7F04BB38
/* 08065C 7F04BB2C 00000000 */   nop
/* 080660 7F04BB30 10000003 */  b     .L7F04BB40
/* 080664 7F04BB34 24100014 */   li    $s0, 20
.L7F04BB38:
/* 080668 7F04BB38 10000001 */  b     .L7F04BB40
/* 08066C 7F04BB3C 2410005A */   li    $s0, 90
.L7F04BB40:
/* 080670 7F04BB40 0FC26DD0 */  jal   chrObjRandomGetNext
/* 080674 7F04BB44 00000000 */   nop
/* 080678 7F04BB48 24010064 */  li    $at, 100
/* 08067C 7F04BB4C 0041001B */  divu  $zero, $v0, $at
/* 080680 7F04BB50 00005010 */  mfhi  $t2
/* 080684 7F04BB54 0150082A */  slt   $at, $t2, $s0
/* 080688 7F04BB58 1020000F */  beqz  $at, .L7F04BB98
/* 08068C 7F04BB5C 8FAA0040 */   lw    $t2, 0x40($sp)
/* 080690 7F04BB60 8E6B0000 */  lw    $t3, ($s3)
/* 080694 7F04BB64 240800FF */  li    $t0, 255
/* 080698 7F04BB68 01716821 */  addu  $t5, $t3, $s1
/* 08069C 7F04BB6C A1A0000C */  sb    $zero, 0xc($t5)
/* 0806A0 7F04BB70 8E6C0000 */  lw    $t4, ($s3)
/* 0806A4 7F04BB74 01917021 */  addu  $t6, $t4, $s1
/* 0806A8 7F04BB78 A1C0000D */  sb    $zero, 0xd($t6)
/* 0806AC 7F04BB7C 8E780000 */  lw    $t8, ($s3)
/* 0806B0 7F04BB80 03117821 */  addu  $t7, $t8, $s1
/* 0806B4 7F04BB84 A1E0000E */  sb    $zero, 0xe($t7)
/* 0806B8 7F04BB88 8E790000 */  lw    $t9, ($s3)
/* 0806BC 7F04BB8C 03314821 */  addu  $t1, $t9, $s1
/* 0806C0 7F04BB90 10000007 */  b     .L7F04BBB0
/* 0806C4 7F04BB94 A128000F */   sb    $t0, 0xf($t1)
.L7F04BB98:
/* 0806C8 7F04BB98 24010002 */  li    $at, 2
/* 0806CC 7F04BB9C 55410005 */  bnel  $t2, $at, .L7F04BBB4
/* 0806D0 7F04BBA0 8E6C0000 */   lw    $t4, ($s3)
/* 0806D4 7F04BBA4 8E6B0000 */  lw    $t3, ($s3)
/* 0806D8 7F04BBA8 01716821 */  addu  $t5, $t3, $s1
/* 0806DC 7F04BBAC A1A0000F */  sb    $zero, 0xf($t5)
.L7F04BBB0:
/* 0806E0 7F04BBB0 8E6C0000 */  lw    $t4, ($s3)
.L7F04BBB4:
/* 0806E4 7F04BBB4 C7A80068 */  lwc1  $f8, 0x68($sp)
/* 0806E8 7F04BBB8 C7AA0044 */  lwc1  $f10, 0x44($sp)
/* 0806EC 7F04BBBC 01918021 */  addu  $s0, $t4, $s1
/* 0806F0 7F04BBC0 860E0002 */  lh    $t6, 2($s0)
/* 0806F4 7F04BBC4 01D6C023 */  subu  $t8, $t6, $s6
/* 0806F8 7F04BBC8 44982000 */  mtc1  $t8, $f4
/* 0806FC 7F04BBCC 00000000 */  nop
/* 080700 7F04BBD0 468021A0 */  cvt.s.w $f6, $f4
/* 080704 7F04BBD4 46083402 */  mul.s $f16, $f6, $f8
/* 080708 7F04BBD8 460A8480 */  add.s $f18, $f16, $f10
/* 08070C 7F04BBDC 4600910D */  trunc.w.s $f4, $f18
/* 080710 7F04BBE0 44192000 */  mfc1  $t9, $f4
/* 080714 7F04BBE4 0FC26DD0 */  jal   chrObjRandomGetNext
/* 080718 7F04BBE8 A6190002 */   sh    $t9, 2($s0)
/* 08071C 7F04BBEC 0054001B */  divu  $zero, $v0, $s4
/* 080720 7F04BBF0 8E680000 */  lw    $t0, ($s3)
/* 080724 7F04BBF4 00005010 */  mfhi  $t2
/* 080728 7F04BBF8 01118021 */  addu  $s0, $t0, $s1
/* 08072C 7F04BBFC 86090000 */  lh    $t1, ($s0)
/* 080730 7F04BC00 16800002 */  bnez  $s4, .L7F04BC0C
/* 080734 7F04BC04 00000000 */   nop
/* 080738 7F04BC08 0007000D */  break 7
.L7F04BC0C:
/* 08073C 7F04BC0C 012A5821 */  addu  $t3, $t1, $t2
/* 080740 7F04BC10 256DFFD8 */  addiu $t5, $t3, -0x28
/* 080744 7F04BC14 0FC26DD0 */  jal   chrObjRandomGetNext
/* 080748 7F04BC18 A60D0000 */   sh    $t5, ($s0)
/* 08074C 7F04BC1C 0054001B */  divu  $zero, $v0, $s4
/* 080750 7F04BC20 8E6C0000 */  lw    $t4, ($s3)
/* 080754 7F04BC24 0000C010 */  mfhi  $t8
/* 080758 7F04BC28 01918021 */  addu  $s0, $t4, $s1
/* 08075C 7F04BC2C 860E0002 */  lh    $t6, 2($s0)
/* 080760 7F04BC30 16800002 */  bnez  $s4, .L7F04BC3C
/* 080764 7F04BC34 00000000 */   nop
/* 080768 7F04BC38 0007000D */  break 7
.L7F04BC3C:
/* 08076C 7F04BC3C 01D87821 */  addu  $t7, $t6, $t8
/* 080770 7F04BC40 25F9FFD8 */  addiu $t9, $t7, -0x28
/* 080774 7F04BC44 0FC26DD0 */  jal   chrObjRandomGetNext
/* 080778 7F04BC48 A6190002 */   sh    $t9, 2($s0)
/* 08077C 7F04BC4C 0054001B */  divu  $zero, $v0, $s4
/* 080780 7F04BC50 8E680000 */  lw    $t0, ($s3)
/* 080784 7F04BC54 00005010 */  mfhi  $t2
/* 080788 7F04BC58 01118021 */  addu  $s0, $t0, $s1
/* 08078C 7F04BC5C 86090004 */  lh    $t1, 4($s0)
/* 080790 7F04BC60 16800002 */  bnez  $s4, .L7F04BC6C
/* 080794 7F04BC64 00000000 */   nop
/* 080798 7F04BC68 0007000D */  break 7
.L7F04BC6C:
/* 08079C 7F04BC6C 012A5821 */  addu  $t3, $t1, $t2
/* 0807A0 7F04BC70 256DFFD8 */  addiu $t5, $t3, -0x28
/* 0807A4 7F04BC74 A60D0004 */  sh    $t5, 4($s0)
/* 0807A8 7F04BC78 8E6C0000 */  lw    $t4, ($s3)
/* 0807AC 7F04BC7C 01918021 */  addu  $s0, $t4, $s1
/* 0807B0 7F04BC80 860E0002 */  lh    $t6, 2($s0)
/* 0807B4 7F04BC84 01D6082A */  slt   $at, $t6, $s6
/* 0807B8 7F04BC88 50200003 */  beql  $at, $zero, .L7F04BC98
/* 0807BC 7F04BC8C 86B8000C */   lh    $t8, 0xc($s5)
/* 0807C0 7F04BC90 A6160002 */  sh    $s6, 2($s0)
/* 0807C4 7F04BC94 86B8000C */  lh    $t8, 0xc($s5)
.L7F04BC98:
/* 0807C8 7F04BC98 26520001 */  addiu $s2, $s2, 1
/* 0807CC 7F04BC9C 26310010 */  addiu $s1, $s1, 0x10
/* 0807D0 7F04BCA0 0258082A */  slt   $at, $s2, $t8
/* 0807D4 7F04BCA4 5420FF6C */  bnezl $at, .L7F04BA58
/* 0807D8 7F04BCA8 8EAE0008 */   lw    $t6, 8($s5)
.L7F04BCAC:
/* 0807DC 7F04BCAC 8FBF003C */  lw    $ra, 0x3c($sp)
.L7F04BCB0:
/* 0807E0 7F04BCB0 8FB00018 */  lw    $s0, 0x18($sp)
/* 0807E4 7F04BCB4 8FB1001C */  lw    $s1, 0x1c($sp)
/* 0807E8 7F04BCB8 8FB20020 */  lw    $s2, 0x20($sp)
/* 0807EC 7F04BCBC 8FB30024 */  lw    $s3, 0x24($sp)
/* 0807F0 7F04BCC0 8FB40028 */  lw    $s4, 0x28($sp)
/* 0807F4 7F04BCC4 8FB5002C */  lw    $s5, 0x2c($sp)
/* 0807F8 7F04BCC8 8FB60030 */  lw    $s6, 0x30($sp)
/* 0807FC 7F04BCCC 8FB70034 */  lw    $s7, 0x34($sp)
/* 080800 7F04BCD0 8FBE0038 */  lw    $fp, 0x38($sp)
/* 080804 7F04BCD4 03E00008 */  jr    $ra
/* 080808 7F04BCD8 27BD0098 */   addiu $sp, $sp, 0x98
)
#endif

#ifdef VERSION_EU
GLOBAL_ASM(
.text
glabel objDeform
/* 07E1DC 7F04B7EC 27BDFF68 */  addiu $sp, $sp, -0x98
/* 07E1E0 7F04B7F0 AFB60030 */  sw    $s6, 0x30($sp)
/* 07E1E4 7F04B7F4 AFB40028 */  sw    $s4, 0x28($sp)
/* 07E1E8 7F04B7F8 AFBF003C */  sw    $ra, 0x3c($sp)
/* 07E1EC 7F04B7FC AFBE0038 */  sw    $fp, 0x38($sp)
/* 07E1F0 7F04B800 AFB70034 */  sw    $s7, 0x34($sp)
/* 07E1F4 7F04B804 AFB5002C */  sw    $s5, 0x2c($sp)
/* 07E1F8 7F04B808 AFB30024 */  sw    $s3, 0x24($sp)
/* 07E1FC 7F04B80C AFB20020 */  sw    $s2, 0x20($sp)
/* 07E200 7F04B810 AFB1001C */  sw    $s1, 0x1c($sp)
/* 07E204 7F04B814 AFB00018 */  sw    $s0, 0x18($sp)
/* 07E208 7F04B818 AFA5009C */  sw    $a1, 0x9c($sp)
/* 07E20C 7F04B81C 3C14FFFE */  lui   $s4, (0xFFFE7961 >> 16) # lui $s4, 0xfffe
/* 07E210 7F04B820 3C160001 */  li    $s6, 0x00010000 # 0.000000
/* 07E214 7F04B824 0080F025 */  move  $fp, $a0
/* 07E218 7F04B828 8C900014 */  lw    $s0, 0x14($a0)
/* 07E21C 7F04B82C 36D6869F */  ori   $s6, (0x0001869F & 0xFFFF) # ori $s6, $s6, 0x869f
/* 07E220 7F04B830 0FC12D95 */  jal   sub_GAME_7F04B478
/* 07E224 7F04B834 36947961 */   ori   $s4, (0xFFFE7961 & 0xFFFF) # ori $s4, $s4, 0x7961
/* 07E228 7F04B838 10400199 */  beqz  $v0, .L7F04BEA0
/* 07E22C 7F04B83C 00402825 */   move  $a1, $v0
/* 07E230 7F04B840 8C550004 */  lw    $s5, 4($v0)
/* 07E234 7F04B844 52A00197 */  beql  $s5, $zero, .L7F04BEA4
/* 07E238 7F04B848 8FBF003C */   lw    $ra, 0x3c($sp)
/* 07E23C 7F04B84C 8FCE0014 */  lw    $t6, 0x14($fp)
/* 07E240 7F04B850 0FC12DDB */  jal   sub_GAME_7F04B590
/* 07E244 7F04B854 8DC40008 */   lw    $a0, 8($t6)
/* 07E248 7F04B858 50400192 */  beql  $v0, $zero, .L7F04BEA4
/* 07E24C 7F04B85C 8FBF003C */   lw    $ra, 0x3c($sp)
/* 07E250 7F04B860 96B8001A */  lhu   $t8, 0x1a($s5)
/* 07E254 7F04B864 87C80004 */  lh    $t0, 4($fp)
/* 07E258 7F04B868 8E0F0010 */  lw    $t7, 0x10($s0)
/* 07E25C 7F04B86C 0018C880 */  sll   $t9, $t8, 2
/* 07E260 7F04B870 29010068 */  slti  $at, $t0, 0x68
/* 07E264 7F04B874 1020001D */  beqz  $at, .L7F04B8EC
/* 07E268 7F04B878 01F99821 */   addu  $s3, $t7, $t9
/* 07E26C 7F04B87C 0C00262C */  jal   randomGetNext
/* 07E270 7F04B880 00000000 */   nop
/* 07E274 7F04B884 30490001 */  andi  $t1, $v0, 1
/* 07E278 7F04B888 1120000D */  beqz  $t1, .L7F04B8C0
/* 07E27C 7F04B88C 8FA8009C */   lw    $t0, 0x9c($sp)
/* 07E280 7F04B890 87CA0004 */  lh    $t2, 4($fp)
/* 07E284 7F04B894 8FAC009C */  lw    $t4, 0x9c($sp)
/* 07E288 7F04B898 3C188003 */  lui   $t8, %hi(object_explosion_details+2)
/* 07E28C 7F04B89C 000A5880 */  sll   $t3, $t2, 2
/* 07E290 7F04B8A0 016A5823 */  subu  $t3, $t3, $t2
/* 07E294 7F04B8A4 000B5880 */  sll   $t3, $t3, 2
/* 07E298 7F04B8A8 000C6840 */  sll   $t5, $t4, 1
/* 07E29C 7F04B8AC 016D7021 */  addu  $t6, $t3, $t5
/* 07E2A0 7F04B8B0 030EC021 */  addu  $t8, $t8, $t6
/* 07E2A4 7F04B8B4 97185EDC */  lhu   $t8, %lo(object_explosion_details+344)($t8)
/* 07E2A8 7F04B8B8 1000000D */  b     .L7F04B8F0
/* 07E2AC 7F04B8BC AFB80078 */   sw    $t8, 0x78($sp)
.L7F04B8C0:
/* 07E2B0 7F04B8C0 87CF0004 */  lh    $t7, 4($fp)
/* 07E2B4 7F04B8C4 00084840 */  sll   $t1, $t0, 1
/* 07E2B8 7F04B8C8 3C0C8003 */  lui   $t4, %hi(object_explosion_details+8)
/* 07E2BC 7F04B8CC 000FC880 */  sll   $t9, $t7, 2
/* 07E2C0 7F04B8D0 032FC823 */  subu  $t9, $t9, $t7
/* 07E2C4 7F04B8D4 0019C880 */  sll   $t9, $t9, 2
/* 07E2C8 7F04B8D8 03295021 */  addu  $t2, $t9, $t1
/* 07E2CC 7F04B8DC 018A6021 */  addu  $t4, $t4, $t2
/* 07E2D0 7F04B8E0 958C5EE2 */  lhu   $t4, %lo(object_explosion_details+350)($t4)
/* 07E2D4 7F04B8E4 10000002 */  b     .L7F04B8F0
/* 07E2D8 7F04B8E8 AFAC0078 */   sw    $t4, 0x78($sp)
.L7F04B8EC:
/* 07E2DC 7F04B8EC AFA00078 */  sw    $zero, 0x78($sp)
.L7F04B8F0:
/* 07E2E0 7F04B8F0 0FC24118 */  jal   get_debug_explosioninfo_flag
/* 07E2E4 7F04B8F4 00000000 */   nop
/* 07E2E8 7F04B8F8 14400003 */  bnez  $v0, .L7F04B908
/* 07E2EC 7F04B8FC 8FAB0078 */   lw    $t3, 0x78($sp)
/* 07E2F0 7F04B900 1560000B */  bnez  $t3, .L7F04B930
/* 07E2F4 7F04B904 00000000 */   nop
.L7F04B908:
/* 07E2F8 7F04B908 0FC24118 */  jal   get_debug_explosioninfo_flag
/* 07E2FC 7F04B90C 00000000 */   nop
/* 07E300 7F04B910 0C00262C */  jal   randomGetNext
/* 07E304 7F04B914 00000000 */   nop
/* 07E308 7F04B918 0FC24118 */  jal   get_debug_explosioninfo_flag
/* 07E30C 7F04B91C AFA20078 */   sw    $v0, 0x78($sp)
/* 07E310 7F04B920 10400003 */  beqz  $v0, .L7F04B930
/* 07E314 7F04B924 8FAD0078 */   lw    $t5, 0x78($sp)
/* 07E318 7F04B928 31AEFFFF */  andi  $t6, $t5, 0xffff
/* 07E31C 7F04B92C AFAE0078 */  sw    $t6, 0x78($sp)
.L7F04B930:
/* 07E320 7F04B930 0FC280B5 */  jal   explosionClearBulletImpactRoom
/* 07E324 7F04B934 8FC40010 */   lw    $a0, 0x10($fp)
/* 07E328 7F04B938 87D80004 */  lh    $t8, 4($fp)
/* 07E32C 7F04B93C 2401004C */  li    $at, 76
/* 07E330 7F04B940 24170001 */  li    $s7, 1
/* 07E334 7F04B944 17010003 */  bne   $t8, $at, .L7F04B954
/* 07E338 7F04B948 00000000 */   nop
/* 07E33C 7F04B94C 10000001 */  b     .L7F04B954
/* 07E340 7F04B950 0000B825 */   move  $s7, $zero
.L7F04B954:
/* 07E344 7F04B954 0FC10020 */  jal   objGetDestroyedLevel
/* 07E348 7F04B958 03C02025 */   move  $a0, $fp
/* 07E34C 7F04B95C 86A4000C */  lh    $a0, 0xc($s5)
/* 07E350 7F04B960 24050B0B */  li    $a1, 2827
/* 07E354 7F04B964 8E060008 */  lw    $a2, 8($s0)
/* 07E358 7F04B968 0FC26CE3 */  jal   sub_GAME_7F09BE4C
/* 07E35C 7F04B96C 00403825 */   move  $a3, $v0
/* 07E360 7F04B970 1040003A */  beqz  $v0, .L7F04BA5C
/* 07E364 7F04B974 00408025 */   move  $s0, $v0
/* 07E368 7F04B978 8E640000 */  lw    $a0, ($s3)
/* 07E36C 7F04B97C 8EAF0008 */  lw    $t7, 8($s5)
/* 07E370 7F04B980 51E4001E */  beql  $t7, $a0, .L7F04B9FC
/* 07E374 7F04B984 86AD000C */   lh    $t5, 0xc($s5)
/* 07E378 7F04B988 86A8000C */  lh    $t0, 0xc($s5)
/* 07E37C 7F04B98C 00009025 */  move  $s2, $zero
/* 07E380 7F04B990 00008825 */  move  $s1, $zero
/* 07E384 7F04B994 19000014 */  blez  $t0, .L7F04B9E8
/* 07E388 7F04B998 00401825 */   move  $v1, $v0
/* 07E38C 7F04B99C 8E790000 */  lw    $t9, ($s3)
.L7F04B9A0:
/* 07E390 7F04B9A0 26520001 */  addiu $s2, $s2, 1
/* 07E394 7F04B9A4 24630010 */  addiu $v1, $v1, 0x10
/* 07E398 7F04B9A8 03314821 */  addu  $t1, $t9, $s1
/* 07E39C 7F04B9AC 8D210000 */  lw    $at, ($t1)
/* 07E3A0 7F04B9B0 26310010 */  addiu $s1, $s1, 0x10
/* 07E3A4 7F04B9B4 AC61FFF0 */  sw    $at, -0x10($v1)
/* 07E3A8 7F04B9B8 8D2C0004 */  lw    $t4, 4($t1)
/* 07E3AC 7F04B9BC AC6CFFF4 */  sw    $t4, -0xc($v1)
/* 07E3B0 7F04B9C0 8D210008 */  lw    $at, 8($t1)
/* 07E3B4 7F04B9C4 AC61FFF8 */  sw    $at, -8($v1)
/* 07E3B8 7F04B9C8 8D2C000C */  lw    $t4, 0xc($t1)
/* 07E3BC 7F04B9CC AC6CFFFC */  sw    $t4, -4($v1)
/* 07E3C0 7F04B9D0 86AB000C */  lh    $t3, 0xc($s5)
/* 07E3C4 7F04B9D4 024B082A */  slt   $at, $s2, $t3
/* 07E3C8 7F04B9D8 5420FFF1 */  bnezl $at, .L7F04B9A0
/* 07E3CC 7F04B9DC 8E790000 */   lw    $t9, ($s3)
/* 07E3D0 7F04B9E0 8E640000 */  lw    $a0, ($s3)
/* 07E3D4 7F04B9E4 00009025 */  move  $s2, $zero
.L7F04B9E8:
/* 07E3D8 7F04B9E8 0FC26D61 */  jal   sub_GAME_7F09C044
/* 07E3DC 7F04B9EC 00000000 */   nop
/* 07E3E0 7F04B9F0 10000018 */  b     .L7F04BA54
/* 07E3E4 7F04B9F4 00000000 */   nop
/* 07E3E8 7F04B9F8 86AD000C */  lh    $t5, 0xc($s5)
.L7F04B9FC:
/* 07E3EC 7F04B9FC 00009025 */  move  $s2, $zero
/* 07E3F0 7F04BA00 00008825 */  move  $s1, $zero
/* 07E3F4 7F04BA04 19A00013 */  blez  $t5, .L7F04BA54
/* 07E3F8 7F04BA08 00401825 */   move  $v1, $v0
/* 07E3FC 7F04BA0C 8EAE0008 */  lw    $t6, 8($s5)
.L7F04BA10:
/* 07E400 7F04BA10 26520001 */  addiu $s2, $s2, 1
/* 07E404 7F04BA14 24630010 */  addiu $v1, $v1, 0x10
/* 07E408 7F04BA18 01D1C021 */  addu  $t8, $t6, $s1
/* 07E40C 7F04BA1C 8F010000 */  lw    $at, ($t8)
/* 07E410 7F04BA20 26310010 */  addiu $s1, $s1, 0x10
/* 07E414 7F04BA24 AC61FFF0 */  sw    $at, -0x10($v1)
/* 07E418 7F04BA28 8F080004 */  lw    $t0, 4($t8)
/* 07E41C 7F04BA2C AC68FFF4 */  sw    $t0, -0xc($v1)
/* 07E420 7F04BA30 8F010008 */  lw    $at, 8($t8)
/* 07E424 7F04BA34 AC61FFF8 */  sw    $at, -8($v1)
/* 07E428 7F04BA38 8F08000C */  lw    $t0, 0xc($t8)
/* 07E42C 7F04BA3C AC68FFFC */  sw    $t0, -4($v1)
/* 07E430 7F04BA40 86B9000C */  lh    $t9, 0xc($s5)
/* 07E434 7F04BA44 0259082A */  slt   $at, $s2, $t9
/* 07E438 7F04BA48 5420FFF1 */  bnezl $at, .L7F04BA10
/* 07E43C 7F04BA4C 8EAE0008 */   lw    $t6, 8($s5)
/* 07E440 7F04BA50 00009025 */  move  $s2, $zero
.L7F04BA54:
/* 07E444 7F04BA54 10000036 */  b     .L7F04BB30
/* 07E448 7F04BA58 AE700000 */   sw    $s0, ($s3)
.L7F04BA5C:
/* 07E44C 7F04BA5C 8E640000 */  lw    $a0, ($s3)
/* 07E450 7F04BA60 8EAA0008 */  lw    $t2, 8($s5)
/* 07E454 7F04BA64 11440008 */  beq   $t2, $a0, .L7F04BA88
/* 07E458 7F04BA68 00000000 */   nop
/* 07E45C 7F04BA6C 0FC26D61 */  jal   sub_GAME_7F09C044
/* 07E460 7F04BA70 00000000 */   nop
/* 07E464 7F04BA74 8EA90008 */  lw    $t1, 8($s5)
/* 07E468 7F04BA78 AE690000 */  sw    $t1, ($s3)
/* 07E46C 7F04BA7C 8FCC0064 */  lw    $t4, 0x64($fp)
/* 07E470 7F04BA80 358B0004 */  ori   $t3, $t4, 4
/* 07E474 7F04BA84 AFCB0064 */  sw    $t3, 0x64($fp)
.L7F04BA88:
/* 07E478 7F04BA88 3C018005 */  lui   $at, %hi(D_80052CB4) # $at, 0x8005
/* 07E47C 7F04BA8C C4208DF4 */  lwc1  $f0, %lo(D_80052CB4)($at)
/* 07E480 7F04BA90 C7C40028 */  lwc1  $f4, 0x28($fp)
/* 07E484 7F04BA94 C7C8002C */  lwc1  $f8, 0x2c($fp)
/* 07E488 7F04BA98 C7D00030 */  lwc1  $f16, 0x30($fp)
/* 07E48C 7F04BA9C 46002182 */  mul.s $f6, $f4, $f0
/* 07E490 7F04BAA0 00000000 */  nop
/* 07E494 7F04BAA4 46004282 */  mul.s $f10, $f8, $f0
/* 07E498 7F04BAA8 00000000 */  nop
/* 07E49C 7F04BAAC 46008482 */  mul.s $f18, $f16, $f0
/* 07E4A0 7F04BAB0 E7C60028 */  swc1  $f6, 0x28($fp)
/* 07E4A4 7F04BAB4 E7CA002C */  swc1  $f10, 0x2c($fp)
/* 07E4A8 7F04BAB8 12E000F9 */  beqz  $s7, .L7F04BEA0
/* 07E4AC 7F04BABC E7D20030 */   swc1  $f18, 0x30($fp)
/* 07E4B0 7F04BAC0 8FC20014 */  lw    $v0, 0x14($fp)
/* 07E4B4 7F04BAC4 C4440014 */  lwc1  $f4, 0x14($v0)
/* 07E4B8 7F04BAC8 8C440008 */  lw    $a0, 8($v0)
/* 07E4BC 7F04BACC 0FC1002E */  jal   chrobjGetBboxFromObjFile
/* 07E4C0 7F04BAD0 E7A4005C */   swc1  $f4, 0x5c($sp)
/* 07E4C4 7F04BAD4 00408025 */  move  $s0, $v0
/* 07E4C8 7F04BAD8 0FC0FA49 */  jal   chrpropBBOXGetYmin
/* 07E4CC 7F04BADC 00402025 */   move  $a0, $v0
/* 07E4D0 7F04BAE0 C7A6005C */  lwc1  $f6, 0x5c($sp)
/* 07E4D4 7F04BAE4 3C018005 */  lui   $at, %hi(D_80052CB8) # $at, 0x8005
/* 07E4D8 7F04BAE8 C42A8DF8 */  lwc1  $f10, %lo(D_80052CB8)($at)
/* 07E4DC 7F04BAEC 46003202 */  mul.s $f8, $f6, $f0
/* 07E4E0 7F04BAF0 8FC20010 */  lw    $v0, 0x10($fp)
/* 07E4E4 7F04BAF4 02002025 */  move  $a0, $s0
/* 07E4E8 7F04BAF8 C452000C */  lwc1  $f18, 0xc($v0)
/* 07E4EC 7F04BAFC 460A4402 */  mul.s $f16, $f8, $f10
/* 07E4F0 7F04BB00 46109100 */  add.s $f4, $f18, $f16
/* 07E4F4 7F04BB04 0FC0FA49 */  jal   chrpropBBOXGetYmin
/* 07E4F8 7F04BB08 E444000C */   swc1  $f4, 0xc($v0)
/* 07E4FC 7F04BB0C C7A6005C */  lwc1  $f6, 0x5c($sp)
/* 07E500 7F04BB10 3C018005 */  lui   $at, %hi(D_80052CBC) # $at, 0x8005
/* 07E504 7F04BB14 C42A8DFC */  lwc1  $f10, %lo(D_80052CBC)($at)
/* 07E508 7F04BB18 46003202 */  mul.s $f8, $f6, $f0
/* 07E50C 7F04BB1C C7D0005C */  lwc1  $f16, 0x5c($fp)
/* 07E510 7F04BB20 460A4482 */  mul.s $f18, $f8, $f10
/* 07E514 7F04BB24 46128100 */  add.s $f4, $f16, $f18
/* 07E518 7F04BB28 100000DD */  b     .L7F04BEA0
/* 07E51C 7F04BB2C E7C4005C */   swc1  $f4, 0x5c($fp)
.L7F04BB30:
/* 07E520 7F04BB30 87C20004 */  lh    $v0, 4($fp)
/* 07E524 7F04BB34 24010026 */  li    $at, 38
/* 07E528 7F04BB38 00008825 */  move  $s1, $zero
/* 07E52C 7F04BB3C 10410005 */  beq   $v0, $at, .L7F04BB54
/* 07E530 7F04BB40 24010027 */   li    $at, 39
/* 07E534 7F04BB44 10410003 */  beq   $v0, $at, .L7F04BB54
/* 07E538 7F04BB48 24010055 */   li    $at, 85
/* 07E53C 7F04BB4C 14410003 */  bne   $v0, $at, .L7F04BB5C
/* 07E540 7F04BB50 240D0001 */   li    $t5, 1
.L7F04BB54:
/* 07E544 7F04BB54 10000002 */  b     .L7F04BB60
/* 07E548 7F04BB58 AFA00070 */   sw    $zero, 0x70($sp)
.L7F04BB5C:
/* 07E54C 7F04BB5C AFAD0070 */  sw    $t5, 0x70($sp)
.L7F04BB60:
/* 07E550 7F04BB60 86A4000C */  lh    $a0, 0xc($s5)
/* 07E554 7F04BB64 8FA5009C */  lw    $a1, 0x9c($sp)
/* 07E558 7F04BB68 18800010 */  blez  $a0, .L7F04BBAC
/* 07E55C 7F04BB6C 00041900 */   sll   $v1, $a0, 4
/* 07E560 7F04BB70 8E700000 */  lw    $s0, ($s3)
.L7F04BB74:
/* 07E564 7F04BB74 86020002 */  lh    $v0, 2($s0)
/* 07E568 7F04BB78 26310010 */  addiu $s1, $s1, 0x10
/* 07E56C 7F04BB7C 0056082A */  slt   $at, $v0, $s6
/* 07E570 7F04BB80 50200003 */  beql  $at, $zero, .L7F04BB90
/* 07E574 7F04BB84 0282082A */   slt   $at, $s4, $v0
/* 07E578 7F04BB88 0040B025 */  move  $s6, $v0
/* 07E57C 7F04BB8C 0282082A */  slt   $at, $s4, $v0
.L7F04BB90:
/* 07E580 7F04BB90 50200003 */  beql  $at, $zero, .L7F04BBA0
/* 07E584 7F04BB94 0223082A */   slt   $at, $s1, $v1
/* 07E588 7F04BB98 0040A025 */  move  $s4, $v0
/* 07E58C 7F04BB9C 0223082A */  slt   $at, $s1, $v1
.L7F04BBA0:
/* 07E590 7F04BBA0 1420FFF4 */  bnez  $at, .L7F04BB74
/* 07E594 7F04BBA4 26100010 */   addiu $s0, $s0, 0x10
/* 07E598 7F04BBA8 00009025 */  move  $s2, $zero
.L7F04BBAC:
/* 07E59C 7F04BBAC 02D41021 */  addu  $v0, $s6, $s4
/* 07E5A0 7F04BBB0 00027043 */  sra   $t6, $v0, 1
/* 07E5A4 7F04BBB4 24010003 */  li    $at, 3
/* 07E5A8 7F04BBB8 14A10006 */  bne   $a1, $at, .L7F04BBD4
/* 07E5AC 7F04BBBC 01C01825 */   move  $v1, $t6
/* 07E5B0 7F04BBC0 01D67823 */  subu  $t7, $t6, $s6
/* 07E5B4 7F04BBC4 29E10029 */  slti  $at, $t7, 0x29
/* 07E5B8 7F04BBC8 54200003 */  bnezl $at, .L7F04BBD8
/* 07E5BC 7F04BBCC 02961023 */   subu  $v0, $s4, $s6
/* 07E5C0 7F04BBD0 26C30028 */  addiu $v1, $s6, 0x28
.L7F04BBD4:
/* 07E5C4 7F04BBD4 02961023 */  subu  $v0, $s4, $s6
.L7F04BBD8:
/* 07E5C8 7F04BBD8 2841003D */  slti  $at, $v0, 0x3d
/* 07E5CC 7F04BBDC 1420000E */  bnez  $at, .L7F04BC18
/* 07E5D0 7F04BBE0 28A10003 */   slti  $at, $a1, 3
/* 07E5D4 7F04BBE4 10200005 */  beqz  $at, .L7F04BBFC
/* 07E5D8 7F04BBE8 2458FFC4 */   addiu $t8, $v0, -0x3c
/* 07E5DC 7F04BBEC 3C018005 */  lui   $at, %hi(D_80052CC0) # $at, 0x8005
/* 07E5E0 7F04BBF0 C4268E00 */  lwc1  $f6, %lo(D_80052CC0)($at)
/* 07E5E4 7F04BBF4 1000000C */  b     .L7F04BC28
/* 07E5E8 7F04BBF8 E7A60068 */   swc1  $f6, 0x68($sp)
.L7F04BBFC:
/* 07E5EC 7F04BBFC 44984000 */  mtc1  $t8, $f8
/* 07E5F0 7F04BC00 44828000 */  mtc1  $v0, $f16
/* 07E5F4 7F04BC04 468042A0 */  cvt.s.w $f10, $f8
/* 07E5F8 7F04BC08 468084A0 */  cvt.s.w $f18, $f16
/* 07E5FC 7F04BC0C 46125103 */  div.s $f4, $f10, $f18
/* 07E600 7F04BC10 10000005 */  b     .L7F04BC28
/* 07E604 7F04BC14 E7A40068 */   swc1  $f4, 0x68($sp)
.L7F04BC18:
/* 07E608 7F04BC18 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 07E60C 7F04BC1C 44813000 */  mtc1  $at, $f6
/* 07E610 7F04BC20 00000000 */  nop
/* 07E614 7F04BC24 E7A60068 */  swc1  $f6, 0x68($sp)
.L7F04BC28:
/* 07E618 7F04BC28 5880009E */  blezl $a0, .L7F04BEA4
/* 07E61C 7F04BC2C 8FBF003C */   lw    $ra, 0x3c($sp)
/* 07E620 7F04BC30 44964000 */  mtc1  $s6, $f8
/* 07E624 7F04BC34 00008825 */  move  $s1, $zero
/* 07E628 7F04BC38 AFA3007C */  sw    $v1, 0x7c($sp)
/* 07E62C 7F04BC3C 46804420 */  cvt.s.w $f16, $f8
/* 07E630 7F04BC40 24140050 */  li    $s4, 80
/* 07E634 7F04BC44 E7B00044 */  swc1  $f16, 0x44($sp)
/* 07E638 7F04BC48 8EA80008 */  lw    $t0, 8($s5)
.L7F04BC4C:
/* 07E63C 7F04BC4C 8FAD0078 */  lw    $t5, 0x78($sp)
/* 07E640 7F04BC50 01111021 */  addu  $v0, $t0, $s1
/* 07E644 7F04BC54 84590004 */  lh    $t9, 4($v0)
/* 07E648 7F04BC58 844A0000 */  lh    $t2, ($v0)
/* 07E64C 7F04BC5C 844C0002 */  lh    $t4, 2($v0)
/* 07E650 7F04BC60 032A4821 */  addu  $t1, $t9, $t2
/* 07E654 7F04BC64 012C5821 */  addu  $t3, $t1, $t4
/* 07E658 7F04BC68 0FC26B32 */  jal   chrObjRandomSetSeed
/* 07E65C 7F04BC6C 016D2021 */   addu  $a0, $t3, $t5
/* 07E660 7F04BC70 8FAE0070 */  lw    $t6, 0x70($sp)
/* 07E664 7F04BC74 00008025 */  move  $s0, $zero
/* 07E668 7F04BC78 11C0002E */  beqz  $t6, .L7F04BD34
/* 07E66C 7F04BC7C 00000000 */   nop
/* 07E670 7F04BC80 C7CA002C */  lwc1  $f10, 0x2c($fp)
/* 07E674 7F04BC84 44809000 */  mtc1  $zero, $f18
/* 07E678 7F04BC88 00000000 */  nop
/* 07E67C 7F04BC8C 460A903E */  c.le.s $f18, $f10
/* 07E680 7F04BC90 00000000 */  nop
/* 07E684 7F04BC94 45020015 */  bc1fl .L7F04BCEC
/* 07E688 7F04BC98 8E690000 */   lw    $t1, ($s3)
/* 07E68C 7F04BC9C 8E6F0000 */  lw    $t7, ($s3)
/* 07E690 7F04BCA0 8FB9007C */  lw    $t9, 0x7c($sp)
/* 07E694 7F04BCA4 01F1C021 */  addu  $t8, $t7, $s1
/* 07E698 7F04BCA8 87080002 */  lh    $t0, 2($t8)
/* 07E69C 7F04BCAC 0119082A */  slt   $at, $t0, $t9
/* 07E6A0 7F04BCB0 14200007 */  bnez  $at, .L7F04BCD0
/* 07E6A4 7F04BCB4 00000000 */   nop
/* 07E6A8 7F04BCB8 12E00003 */  beqz  $s7, .L7F04BCC8
/* 07E6AC 7F04BCBC 00000000 */   nop
/* 07E6B0 7F04BCC0 1000001C */  b     .L7F04BD34
/* 07E6B4 7F04BCC4 2410005A */   li    $s0, 90
.L7F04BCC8:
/* 07E6B8 7F04BCC8 1000001A */  b     .L7F04BD34
/* 07E6BC 7F04BCCC 24100014 */   li    $s0, 20
.L7F04BCD0:
/* 07E6C0 7F04BCD0 12E00003 */  beqz  $s7, .L7F04BCE0
/* 07E6C4 7F04BCD4 00000000 */   nop
/* 07E6C8 7F04BCD8 10000016 */  b     .L7F04BD34
/* 07E6CC 7F04BCDC 24100014 */   li    $s0, 20
.L7F04BCE0:
/* 07E6D0 7F04BCE0 10000014 */  b     .L7F04BD34
/* 07E6D4 7F04BCE4 2410005A */   li    $s0, 90
/* 07E6D8 7F04BCE8 8E690000 */  lw    $t1, ($s3)
.L7F04BCEC:
/* 07E6DC 7F04BCEC 8FAA007C */  lw    $t2, 0x7c($sp)
/* 07E6E0 7F04BCF0 01316021 */  addu  $t4, $t1, $s1
/* 07E6E4 7F04BCF4 858B0002 */  lh    $t3, 2($t4)
/* 07E6E8 7F04BCF8 014B082A */  slt   $at, $t2, $t3
/* 07E6EC 7F04BCFC 14200007 */  bnez  $at, .L7F04BD1C
/* 07E6F0 7F04BD00 00000000 */   nop
/* 07E6F4 7F04BD04 12E00003 */  beqz  $s7, .L7F04BD14
/* 07E6F8 7F04BD08 00000000 */   nop
/* 07E6FC 7F04BD0C 10000009 */  b     .L7F04BD34
/* 07E700 7F04BD10 2410005A */   li    $s0, 90
.L7F04BD14:
/* 07E704 7F04BD14 10000007 */  b     .L7F04BD34
/* 07E708 7F04BD18 24100014 */   li    $s0, 20
.L7F04BD1C:
/* 07E70C 7F04BD1C 12E00003 */  beqz  $s7, .L7F04BD2C
/* 07E710 7F04BD20 00000000 */   nop
/* 07E714 7F04BD24 10000003 */  b     .L7F04BD34
/* 07E718 7F04BD28 24100014 */   li    $s0, 20
.L7F04BD2C:
/* 07E71C 7F04BD2C 10000001 */  b     .L7F04BD34
/* 07E720 7F04BD30 2410005A */   li    $s0, 90
.L7F04BD34:
/* 07E724 7F04BD34 0FC26B20 */  jal   chrObjRandomGetNext
/* 07E728 7F04BD38 00000000 */   nop
/* 07E72C 7F04BD3C 24010064 */  li    $at, 100
/* 07E730 7F04BD40 0041001B */  divu  $zero, $v0, $at
/* 07E734 7F04BD44 00006810 */  mfhi  $t5
/* 07E738 7F04BD48 01B0082A */  slt   $at, $t5, $s0
/* 07E73C 7F04BD4C 1020000F */  beqz  $at, .L7F04BD8C
/* 07E740 7F04BD50 8FAD009C */   lw    $t5, 0x9c($sp)
/* 07E744 7F04BD54 8E6E0000 */  lw    $t6, ($s3)
/* 07E748 7F04BD58 240C00FF */  li    $t4, 255
/* 07E74C 7F04BD5C 01D17821 */  addu  $t7, $t6, $s1
/* 07E750 7F04BD60 A1E0000C */  sb    $zero, 0xc($t7)
/* 07E754 7F04BD64 8E780000 */  lw    $t8, ($s3)
/* 07E758 7F04BD68 03114021 */  addu  $t0, $t8, $s1
/* 07E75C 7F04BD6C A100000D */  sb    $zero, 0xd($t0)
/* 07E760 7F04BD70 8E790000 */  lw    $t9, ($s3)
/* 07E764 7F04BD74 03314821 */  addu  $t1, $t9, $s1
/* 07E768 7F04BD78 A120000E */  sb    $zero, 0xe($t1)
/* 07E76C 7F04BD7C 8E6A0000 */  lw    $t2, ($s3)
/* 07E770 7F04BD80 01515821 */  addu  $t3, $t2, $s1
/* 07E774 7F04BD84 10000007 */  b     .L7F04BDA4
/* 07E778 7F04BD88 A16C000F */   sb    $t4, 0xf($t3)
.L7F04BD8C:
/* 07E77C 7F04BD8C 24010001 */  li    $at, 1
/* 07E780 7F04BD90 55A10005 */  bnel  $t5, $at, .L7F04BDA8
/* 07E784 7F04BD94 8E780000 */   lw    $t8, ($s3)
/* 07E788 7F04BD98 8E6E0000 */  lw    $t6, ($s3)
/* 07E78C 7F04BD9C 01D17821 */  addu  $t7, $t6, $s1
/* 07E790 7F04BDA0 A1E0000F */  sb    $zero, 0xf($t7)
.L7F04BDA4:
/* 07E794 7F04BDA4 8E780000 */  lw    $t8, ($s3)
.L7F04BDA8:
/* 07E798 7F04BDA8 C7A80068 */  lwc1  $f8, 0x68($sp)
/* 07E79C 7F04BDAC C7AA0044 */  lwc1  $f10, 0x44($sp)
/* 07E7A0 7F04BDB0 03118021 */  addu  $s0, $t8, $s1
/* 07E7A4 7F04BDB4 86080002 */  lh    $t0, 2($s0)
/* 07E7A8 7F04BDB8 0116C823 */  subu  $t9, $t0, $s6
/* 07E7AC 7F04BDBC 44992000 */  mtc1  $t9, $f4
/* 07E7B0 7F04BDC0 00000000 */  nop
/* 07E7B4 7F04BDC4 468021A0 */  cvt.s.w $f6, $f4
/* 07E7B8 7F04BDC8 46083402 */  mul.s $f16, $f6, $f8
/* 07E7BC 7F04BDCC 460A8480 */  add.s $f18, $f16, $f10
/* 07E7C0 7F04BDD0 4600910D */  trunc.w.s $f4, $f18
/* 07E7C4 7F04BDD4 440A2000 */  mfc1  $t2, $f4
/* 07E7C8 7F04BDD8 0FC26B20 */  jal   chrObjRandomGetNext
/* 07E7CC 7F04BDDC A60A0002 */   sh    $t2, 2($s0)
/* 07E7D0 7F04BDE0 0054001B */  divu  $zero, $v0, $s4
/* 07E7D4 7F04BDE4 8E6C0000 */  lw    $t4, ($s3)
/* 07E7D8 7F04BDE8 00006810 */  mfhi  $t5
/* 07E7DC 7F04BDEC 01918021 */  addu  $s0, $t4, $s1
/* 07E7E0 7F04BDF0 860B0000 */  lh    $t3, ($s0)
/* 07E7E4 7F04BDF4 16800002 */  bnez  $s4, .L7F04BE00
/* 07E7E8 7F04BDF8 00000000 */   nop
/* 07E7EC 7F04BDFC 0007000D */  break 7
.L7F04BE00:
/* 07E7F0 7F04BE00 016D7021 */  addu  $t6, $t3, $t5
/* 07E7F4 7F04BE04 25CFFFD8 */  addiu $t7, $t6, -0x28
/* 07E7F8 7F04BE08 0FC26B20 */  jal   chrObjRandomGetNext
/* 07E7FC 7F04BE0C A60F0000 */   sh    $t7, ($s0)
/* 07E800 7F04BE10 0054001B */  divu  $zero, $v0, $s4
/* 07E804 7F04BE14 8E780000 */  lw    $t8, ($s3)
/* 07E808 7F04BE18 0000C810 */  mfhi  $t9
/* 07E80C 7F04BE1C 03118021 */  addu  $s0, $t8, $s1
/* 07E810 7F04BE20 86080002 */  lh    $t0, 2($s0)
/* 07E814 7F04BE24 16800002 */  bnez  $s4, .L7F04BE30
/* 07E818 7F04BE28 00000000 */   nop
/* 07E81C 7F04BE2C 0007000D */  break 7
.L7F04BE30:
/* 07E820 7F04BE30 01194821 */  addu  $t1, $t0, $t9
/* 07E824 7F04BE34 252AFFD8 */  addiu $t2, $t1, -0x28
/* 07E828 7F04BE38 0FC26B20 */  jal   chrObjRandomGetNext
/* 07E82C 7F04BE3C A60A0002 */   sh    $t2, 2($s0)
/* 07E830 7F04BE40 0054001B */  divu  $zero, $v0, $s4
/* 07E834 7F04BE44 8E6C0000 */  lw    $t4, ($s3)
/* 07E838 7F04BE48 00006810 */  mfhi  $t5
/* 07E83C 7F04BE4C 01918021 */  addu  $s0, $t4, $s1
/* 07E840 7F04BE50 860B0004 */  lh    $t3, 4($s0)
/* 07E844 7F04BE54 16800002 */  bnez  $s4, .L7F04BE60
/* 07E848 7F04BE58 00000000 */   nop
/* 07E84C 7F04BE5C 0007000D */  break 7
.L7F04BE60:
/* 07E850 7F04BE60 016D7021 */  addu  $t6, $t3, $t5
/* 07E854 7F04BE64 25CFFFD8 */  addiu $t7, $t6, -0x28
/* 07E858 7F04BE68 A60F0004 */  sh    $t7, 4($s0)
/* 07E85C 7F04BE6C 8E780000 */  lw    $t8, ($s3)
/* 07E860 7F04BE70 03118021 */  addu  $s0, $t8, $s1
/* 07E864 7F04BE74 86080002 */  lh    $t0, 2($s0)
/* 07E868 7F04BE78 0116082A */  slt   $at, $t0, $s6
/* 07E86C 7F04BE7C 50200003 */  beql  $at, $zero, .L7F04BE8C
/* 07E870 7F04BE80 86B9000C */   lh    $t9, 0xc($s5)
/* 07E874 7F04BE84 A6160002 */  sh    $s6, 2($s0)
/* 07E878 7F04BE88 86B9000C */  lh    $t9, 0xc($s5)
.L7F04BE8C:
/* 07E87C 7F04BE8C 26520001 */  addiu $s2, $s2, 1
/* 07E880 7F04BE90 26310010 */  addiu $s1, $s1, 0x10
/* 07E884 7F04BE94 0259082A */  slt   $at, $s2, $t9
/* 07E888 7F04BE98 5420FF6C */  bnezl $at, .L7F04BC4C
/* 07E88C 7F04BE9C 8EA80008 */   lw    $t0, 8($s5)
.L7F04BEA0:
/* 07E890 7F04BEA0 8FBF003C */  lw    $ra, 0x3c($sp)
.L7F04BEA4:
/* 07E894 7F04BEA4 8FB00018 */  lw    $s0, 0x18($sp)
/* 07E898 7F04BEA8 8FB1001C */  lw    $s1, 0x1c($sp)
/* 07E89C 7F04BEAC 8FB20020 */  lw    $s2, 0x20($sp)
/* 07E8A0 7F04BEB0 8FB30024 */  lw    $s3, 0x24($sp)
/* 07E8A4 7F04BEB4 8FB40028 */  lw    $s4, 0x28($sp)
/* 07E8A8 7F04BEB8 8FB5002C */  lw    $s5, 0x2c($sp)
/* 07E8AC 7F04BEBC 8FB60030 */  lw    $s6, 0x30($sp)
/* 07E8B0 7F04BEC0 8FB70034 */  lw    $s7, 0x34($sp)
/* 07E8B4 7F04BEC4 8FBE0038 */  lw    $fp, 0x38($sp)
/* 07E8B8 7F04BEC8 03E00008 */  jr    $ra
/* 07E8BC 7F04BECC 27BD0098 */   addiu $sp, $sp, 0x98
)
#endif

#endif


void objBounce(ObjectRecord *obj, coord3d *arg1)
{
    coord3d dir;
    coord3d rot = {0, 0, 0};
    Projectile *projectile = NULL;

    sub_GAME_7F03FDA8(obj->prop);

    if (obj->runtime_bitflags & RUNTIMEBITFLAG_EMBEDDED) {
        projectile = obj->embedment->projectile;
    } else if (obj->runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT) {
        projectile = obj->projectile;
    }

    if (projectile) {
        projectile->speed.x = (RANDOMFRAC() * 1.6666666f * 4.0f) - 3.3333333f;
        projectile->speed.y = (RANDOMFRAC() * 1.6666666f * 2.0f) + 3.3333333f;
        projectile->speed.z = (RANDOMFRAC() * 1.6666666f * 4.0f) - 3.3333333f;

#ifdef VERSION_EU
        rot.x = (RANDOMFRAC() * 7.53982257843f * 0.015625f) - 0.058904863894f;
        rot.y = (RANDOMFRAC() * 7.53982257843f * 0.015625f) - 0.058904863894f;
        rot.z = (RANDOMFRAC() * 7.53982257843f * 0.015625f) - 0.058904863894f;
#else
        rot.x = (RANDOMFRAC() * M_TAU_F * 0.015625f) - 0.049087387f;
        rot.y = (RANDOMFRAC() * M_TAU_F * 0.015625f) - 0.049087387f;
        rot.z = (RANDOMFRAC() * M_TAU_F * 0.015625f) - 0.049087387f;
#endif

        matrix_4x4_set_rotation_around_xyz((f32*)&rot, &projectile->mtx);

        projectile->flags |= PROJECTILEFLAG_AIRBORNE;

        dir.x = arg1->x;
        dir.y = arg1->y;
        dir.z = arg1->z;

        mtx4RotateVecInPlace(currentPlayerGetMatrix10D4(), (f32*)&dir);

        projectile->speed.x += 3.3333333f * dir.x;
        projectile->speed.z += 3.3333333f * dir.z;
        projectile->ownerprop = get_curplayer_positiondata();
        projectile->unk90 = 1;
    }
}


void propobjSetDropped(PropRecord *prop, DROPTYPE droptype)
{
    PropRecord *parent = prop->parent;

    if (parent)
    {
        ObjectRecord *obj = prop->obj;

        sub_GAME_7F03FDA8(prop);

        if ((obj->runtime_bitflags & RUNTIMEBITFLAG_EMBEDDED) && obj->embedment->projectile)
        {
            obj->embedment->projectile->droptype = droptype;
        }
        else if (obj->runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT)
        {
            obj->projectile->droptype = droptype;
        }
    }
}


void objDetach(PropRecord *prop)
{
    PropRecord *parent = prop->parent;

    if (parent)
    {
        ObjectRecord *obj = prop->obj;
        Model *model = obj->model;

        chrpropDetach(prop);

        model->attachedto_objinst = NULL;

        obj->runtime_bitflags &= ~RUNTIMEBITFLAG_HASOWNER;

        if (parent->type == PROP_TYPE_CHR || parent->type == PROP_TYPE_VIEWER)
        {
            ChrRecord *chr = parent->chr;

            if (chr)
            {
                if (prop == chr->handle_positiondata_hat)
                {
                    chr->handle_positiondata_hat = NULL;
                }
                else if (prop == chr->weapons_held[GUNRIGHT])
                {
                    chrSetFiring(chr, GUNRIGHT, FALSE);
                    chr->weapons_held[GUNRIGHT] = NULL;
                }
                else if (prop == chr->weapons_held[GUNLEFT])
                {
                    chrSetFiring(chr, GUNLEFT, FALSE);
                    chr->weapons_held[GUNLEFT] = NULL;
                }
            }
        }
    }
}


s32 objDrop(PropRecord *prop)
{
    PropRecord *parent = prop->parent;
    Projectile *projectile;
    ObjectRecord *obj = prop->obj;
    Model *model;
    Mtxf spB8;
    PropRecord *root;
    StandTile* rootstan;

    if ((obj->runtime_bitflags & RUNTIMEBITFLAG_EMBEDDED) && obj->embedment->projectile)
    {
        Projectile* projectile2 = obj->embedment->projectile;
        embedmentFree(obj->embedment);

        obj->projectile = projectile2;
        obj->runtime_bitflags &= ~RUNTIMEBITFLAG_EMBEDDED;
        obj->runtime_bitflags |= RUNTIMEBITFLAG_DEPOSIT;
    }

    if (parent && (obj->runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT))
    {
        model = obj->model;
        projectile = obj->projectile;
        root = parent;

        projectile->flags |= PROJECTILEFLAG_AIRBORNE;
        projectile->ownerprop = parent;

        if (projectile->droptype == DROPTYPE_SURRENDER && parent->type == PROP_TYPE_CHR)
        {
            ChrRecord* chr = parent->chr;
            Model *chrmodel = chr->model;
            coord3d rot = { 0.0f, 0.0f, 0.0f };
            f32 angle = getsubroty(chrmodel);

            projectile->speed.x = sinf(angle) * 1.6666666f;
            projectile->speed.y = -RANDOMFRAC() * 1.6666666f * 0.5f;
            projectile->speed.z = cosf(angle) *  1.6666666f;

#ifdef VERSION_EU
            rot.x = (RANDOMFRAC() * 7.53982257843f * 0.0078125f) - 0.029452431947f;
            rot.y = (RANDOMFRAC() * 7.53982257843f * 0.0078125f) - 0.029452431947f;
            rot.z = (RANDOMFRAC() * 7.53982257843f * 0.0078125f) - 0.029452431947f;
#else
            rot.x = (RANDOMFRAC() * M_TAU_F * 0.0078125f) - 0.024543693f;
            rot.y = (RANDOMFRAC() * M_TAU_F * 0.0078125f) - 0.024543693f;
            rot.z = (RANDOMFRAC() * M_TAU_F * 0.0078125f) - 0.024543693f;
#endif

            matrix_4x4_set_rotation_around_xyz(rot.f, &projectile->mtx);
        }
        else if (projectile->droptype == DROPTYPE_THROWGRENADE && parent->type == PROP_TYPE_CHR)
        {
            ChrRecord* chr = parent->chr;
            Model *chrmodel = chr->model;
            coord3d rot = { 0.0f, 0.0f, 0.0f };
            f32 angle = getsubroty(chrmodel);

            projectile->speed.x = sinf(angle) * 13.333333f;
            projectile->speed.y = 6.6666665f;
            projectile->speed.z = cosf(angle) * 13.333333f;

#ifdef VERSION_EU
            rot.x = (RANDOMFRAC() * 7.53982257843f * 0.0078125f) - 0.029452431947f;
            rot.y = (RANDOMFRAC() * 7.53982257843f * 0.0078125f) - 0.029452431947f;
            rot.z = (RANDOMFRAC() * 7.53982257843f * 0.0078125f) - 0.029452431947f;
#else
            rot.x = (RANDOMFRAC() * M_TAU_F * 0.0078125f) - 0.024543693f;
            rot.y = (RANDOMFRAC() * M_TAU_F * 0.0078125f) - 0.024543693f;
            rot.z = (RANDOMFRAC() * M_TAU_F * 0.0078125f) - 0.024543693f;
#endif

            matrix_4x4_set_rotation_around_xyz(rot.f, &projectile->mtx);
            projectile->flags |= 0x40;

        }
        else if (projectile->droptype == DROPTYPE_HAT)
        {
            coord3d rot = { 0.0f, 0.0f, 0.0f };
            PropRecord *playerprop = get_curplayer_positiondata();
            f32 x = parent->pos.x - playerprop->pos.x;
            f32 z = parent->pos.z - playerprop->pos.z;
            f32 angle = atan2f(x, z);

            projectile->speed.x = ((2.0f * (RANDOMFRAC() * 1.6666666f)) + 3.3333333f) * sinf(angle);
            projectile->speed.y = 2.0f * (RANDOMFRAC() * 1.6666666f);
            projectile->speed.z = ((2.0f * (RANDOMFRAC() * 1.6666666f)) + 3.3333333f) * cosf(angle);

#ifdef VERSION_EU
            rot.x = (RANDOMFRAC() * 7.53982257843f * 0.03125f) - 0.117809727788f;
            rot.y = (RANDOMFRAC() * 7.53982257843f * 0.03125f) - 0.117809727788f;
            rot.z = (RANDOMFRAC() * 7.53982257843f * 0.03125f) - 0.117809727788f;
#else
            rot.x = (RANDOMFRAC() * M_TAU_F * 0.03125f) - 0.09817477f;
            rot.y = (RANDOMFRAC() * M_TAU_F * 0.03125f) - 0.09817477f;
            rot.z = (RANDOMFRAC() * M_TAU_F * 0.03125f) - 0.09817477f;
#endif

            matrix_4x4_set_rotation_around_xyz(rot.f, &projectile->mtx);
        }
        else
        {
            // DROPTYPE_OWNERREAP ?
            sub_GAME_7F057C14(&projectile->speed, &projectile->mtx);
        }

        while (root->parent != NULL)
        {
            root = root->parent;
        }

        rootstan = root->stan;

        if (prop->flags & PROPFLAG_ONSCREEN)
        {
            // Do collision checks
            f32 objwidth = objGetWidth(obj);
            Mtxf *sp58 = getsubmatrix(model);
            s32 cdtypes = CDTYPE_OBJS | CDTYPE_DOORS | CDTYPE_PLAYERS | CDTYPE_CHRS | CDTYPE_PATHBLOCKER;

            matrix_4x4_multiply_homogeneous(currentPlayerGetMatrix10D4(), sp58, &spB8);

            if (projectile->flags & 0x40)
            {
                cdtypes = CDTYPE_OBJS | CDTYPE_PLAYERS | CDTYPE_CHRS | CDTYPE_PATHBLOCKER;
            }

            sub_GAME_7F03D058(root, FALSE);

            if ((stanTestLineUnobstructed(&rootstan, root->pos.f[0], root->pos.f[2], spB8.m[3][0], spB8.m[3][2], cdtypes, 0.0f, 1.0f, 0.0f, 1.0f) != 0)
                && (stanTestVolume(&rootstan, spB8.m[3][0], spB8.m[3][2], objwidth, cdtypes, 0.0f, 1.0f) < 0))
            {
                prop->stan = rootstan;

            }
            else
            {
                prop->stan = root->stan;
                spB8.m[3][0] = root->pos.x;
                spB8.m[3][2] = root->pos.z;
            }

            sub_GAME_7F03D058(root, TRUE);
            prop->zDepth = -sp58->m[3][2];

        }
        else
        {
            // No collision checks
            // Helpful for throwing mines through doors during speedruns
            prop->stan = root->stan;
            matrix_4x4_set_identity(&spB8);
            matrix_scalar_multiply(model->scale, spB8.m[0]);
            matrix_4x4_set_position(&root->pos, &spB8);
        }

        objDetach(prop);
        chrpropActivate(prop);
        chrpropEnable(prop);

        obj->runtime_pos.x = prop->pos.x = spB8.m[3][0];
        obj->runtime_pos.y = prop->pos.y = spB8.m[3][1];
        obj->runtime_pos.z = prop->pos.z = spB8.m[3][2];

        spB8.m[3][0] = 0.0f;
        spB8.m[3][1] = 0.0f;
        spB8.m[3][2] = 0.0f;

        matrix_4x4_copy(&spB8, &obj->mtx);
        sub_GAME_7F0402B4(obj->prop, &obj->nextcol);

        obj->shadecol.r = obj->nextcol.r;
        obj->shadecol.g = obj->nextcol.g;
        obj->shadecol.b = obj->nextcol.b;
        obj->shadecol.a = obj->nextcol.a;

        setupUpdateObjectRoomPosition(obj);

        return TRUE;
    }

    return FALSE;
}


/**
 * Make an object fall. Eg. due to it sitting on a table which is now destroyed,
 * or because it was a chopper that is now destroyed.
 */
void objFall(ObjectRecord *obj, s32 playernum)
{
    obj->runtime_bitflags &= ~(RUNTIMEBITFLAG_OWNER);
    obj->runtime_bitflags |= (playernum << RUNTIMEBITSHIFT_OWNER);

    if ((obj->flags2 & PROPFLAG2_NOFALL) == 0
            && (obj->flags & PROPFLAG_RENDERPOSTBG)
            && (obj->runtime_bitflags & (RUNTIMEBITFLAG_EMBEDDED | RUNTIMEBITFLAG_DEPOSIT)) == 0)
    {

        coord3d rot = {0, 0, 0};
        Projectile *projectile = NULL;
        s32 unused;

        sub_GAME_7F03FDA8(obj->prop);

        if (obj->runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT)
        {
            projectile = obj->projectile;
        }

        if (projectile)
        {
            projectile->speed.x = RANDOMFRAC() * 1.6666666f - 0.8333333f;
            projectile->speed.y = RANDOMFRAC() * 1.6666666f * 2.0f + 1.6666666f;
            projectile->speed.z = RANDOMFRAC() * 1.6666666f - 0.8333333f;

            if ((obj->flags2 & PROPFLAG2_FALLWITHOUTROTATION) == 0)
            {
#ifdef VERSION_EU
                rot.x = ((RANDOMFRAC() * 7.5398226f) / 320.0f) - 0.011780973f;
                rot.y = ((RANDOMFRAC() * 7.5398226f) / 320.0f) - 0.011780973f;
                rot.z = ((RANDOMFRAC() * 7.5398226f) / 320.0f) - 0.011780973f;
#else
                rot.x = ((RANDOMFRAC() * M_TAU_F) / 320.0f) - 0.009817477f;
                rot.y = ((RANDOMFRAC() * M_TAU_F) / 320.0f) - 0.009817477f;
                rot.z = ((RANDOMFRAC() * M_TAU_F) / 320.0f) - 0.009817477f;
#endif
            }

            matrix_4x4_set_rotation_around_xyz(rot.f, &projectile->mtx);

            projectile->flags |= PROJECTILEFLAG_AIRBORNE;

            obj->flags &= ~PROPFLAG_00000100;
            obj->runtime_bitflags &= ~RUNTIMEBITFLAG_00008000;
        }
    }
}


/**
 * Destroy the objects that the given prop is supporting.
 *
 * For example, destroying a table will also destroy all the props that are
 * sitting on that table.
 */
void objDestroySupportedObjects(PropRecord* tableprop, s32 playernum)
{
    ObjectRecord* obj;
    ObjectRecord* tableobj;
    PropRecord* prop;
    rect4f* rect;
    s32 sp44;
    u8 room;

    tableobj = tableprop->obj;
    room = tableprop->stan->room;

    chraiGetCollisionBoundsWithoutY(tableprop, &rect, &sp44);

    if (sp44 > 0)
    {
        prop = get_ptr_obj_pos_list_current_entry();
        while (prop)
        {
            if (((prop->type == PROP_TYPE_OBJ) || (prop->type == PROP_TYPE_WEAPON)) && (prop->stan->room == room))
            {
                obj = prop->obj;
                if ((tableobj->runtime_pos.y < obj->runtime_pos.y)
                        && ((s32) obj->runtime_bitflags & RUNTIMEBITFLAG_00008000)
                        && (chrpropTestPointInPolygon(&obj->runtime_pos, rect, sp44) != 0))
                {
                    objFall(obj, playernum);
                }
            }
            prop = prop->prev;
        }
    }
}


#ifdef NONMATCHING
void object_explosion_related(void) {

}
#else
#ifndef VERSION_EU
GLOBAL_ASM(
.text
glabel object_explosion_related
/* 08193C 7F04CE0C 27BDFFB8 */  addiu $sp, $sp, -0x48
/* 081940 7F04CE10 AFBF002C */  sw    $ra, 0x2c($sp)
/* 081944 7F04CE14 AFB10028 */  sw    $s1, 0x28($sp)
/* 081948 7F04CE18 AFB00024 */  sw    $s0, 0x24($sp)
/* 08194C 7F04CE1C AFA5004C */  sw    $a1, 0x4c($sp)
/* 081950 7F04CE20 AFA60050 */  sw    $a2, 0x50($sp)
/* 081954 7F04CE24 C4860070 */  lwc1  $f6, 0x70($a0)
/* 081958 7F04CE28 C4840074 */  lwc1  $f4, 0x74($a0)
/* 08195C 7F04CE2C 00808825 */  move  $s1, $a0
/* 081960 7F04CE30 4606203C */  c.lt.s $f4, $f6
/* 081964 7F04CE34 00000000 */  nop
/* 081968 7F04CE38 45030006 */  bc1tl .L7F04CE54
/* 08196C 7F04CE3C 862E0004 */   lh    $t6, 4($s1)
/* 081970 7F04CE40 0FC0FFF0 */  jal   objGetDestroyedLevel
/* 081974 7F04CE44 00000000 */   nop
/* 081978 7F04CE48 504000C4 */  beql  $v0, $zero, .L7F04D15C
/* 08197C 7F04CE4C 8FBF002C */   lw    $ra, 0x2c($sp)
/* 081980 7F04CE50 862E0004 */  lh    $t6, 4($s1)
.L7F04CE54:
/* 081984 7F04CE54 3C188004 */  lui   $t8, %hi(object_explosion_details)
/* 081988 7F04CE58 8E230010 */  lw    $v1, 0x10($s1)
/* 08198C 7F04CE5C 000E78C0 */  sll   $t7, $t6, 3
/* 081990 7F04CE60 01EE7823 */  subu  $t7, $t7, $t6
/* 081994 7F04CE64 000F7840 */  sll   $t7, $t7, 1
/* 081998 7F04CE68 030FC021 */  addu  $t8, $t8, $t7
/* 08199C 7F04CE6C 8718B224 */  lh    $t8, %lo(object_explosion_details)($t8)
/* 0819A0 7F04CE70 02202025 */  move  $a0, $s1
/* 0819A4 7F04CE74 00608025 */  move  $s0, $v1
/* 0819A8 7F04CE78 A7B8003E */  sh    $t8, 0x3e($sp)
/* 0819AC 7F04CE7C 8C79001C */  lw    $t9, 0x1c($v1)
/* 0819B0 7F04CE80 53200007 */  beql  $t9, $zero, .L7F04CEA0
/* 0819B4 7F04CE84 8E080014 */   lw    $t0, 0x14($s0)
/* 0819B8 7F04CE88 8E02001C */  lw    $v0, 0x1c($s0)
/* 0819BC 7F04CE8C 00408025 */  move  $s0, $v0
.L7F04CE90:
/* 0819C0 7F04CE90 8C42001C */  lw    $v0, 0x1c($v0)
/* 0819C4 7F04CE94 5440FFFE */  bnezl $v0, .L7F04CE90
/* 0819C8 7F04CE98 00408025 */   move  $s0, $v0
/* 0819CC 7F04CE9C 8E080014 */  lw    $t0, 0x14($s0)
.L7F04CEA0:
/* 0819D0 7F04CEA0 AFA30044 */  sw    $v1, 0x44($sp)
/* 0819D4 7F04CEA4 0FC0FFF0 */  jal   objGetDestroyedLevel
/* 0819D8 7F04CEA8 AFA80038 */   sw    $t0, 0x38($sp)
/* 0819DC 7F04CEAC 14400056 */  bnez  $v0, .L7F04D008
/* 0819E0 7F04CEB0 00000000 */   nop
/* 0819E4 7F04CEB4 92290002 */  lbu   $t1, 2($s1)
/* 0819E8 7F04CEB8 44804000 */  mtc1  $zero, $f8
/* 0819EC 7F04CEBC 352A0080 */  ori   $t2, $t1, 0x80
/* 0819F0 7F04CEC0 A22A0002 */  sb    $t2, 2($s1)
/* 0819F4 7F04CEC4 E6280070 */  swc1  $f8, 0x70($s1)
/* 0819F8 7F04CEC8 8FAB0038 */  lw    $t3, 0x38($sp)
/* 0819FC 7F04CECC 5160002B */  beql  $t3, $zero, .L7F04CF7C
/* 081A00 7F04CED0 8E2C000C */   lw    $t4, 0xc($s1)
/* 081A04 7F04CED4 920C0001 */  lbu   $t4, 1($s0)
/* 081A08 7F04CED8 8FA2004C */  lw    $v0, 0x4c($sp)
/* 081A0C 7F04CEDC 27A40038 */  addiu $a0, $sp, 0x38
/* 081A10 7F04CEE0 318D0008 */  andi  $t5, $t4, 8
/* 081A14 7F04CEE4 55A00019 */  bnezl $t5, .L7F04CF4C
/* 081A18 7F04CEE8 8E060014 */   lw    $a2, 0x14($s0)
/* 081A1C 7F04CEEC C44A0008 */  lwc1  $f10, 8($v0)
/* 081A20 7F04CEF0 8E050008 */  lw    $a1, 8($s0)
/* 081A24 7F04CEF4 8E060010 */  lw    $a2, 0x10($s0)
/* 081A28 7F04CEF8 8C470000 */  lw    $a3, ($v0)
/* 081A2C 7F04CEFC 0FC2C2F9 */  jal   walkTilesBetweenPoints_NoCallback
/* 081A30 7F04CF00 E7AA0010 */   swc1  $f10, 0x10($sp)
/* 081A34 7F04CF04 10400010 */  beqz  $v0, .L7F04CF48
/* 081A38 7F04CF08 8FA40044 */   lw    $a0, 0x44($sp)
/* 081A3C 7F04CF0C 8E2E0008 */  lw    $t6, 8($s1)
/* 081A40 7F04CF10 8FB90050 */  lw    $t9, 0x50($sp)
/* 081A44 7F04CF14 2608002C */  addiu $t0, $s0, 0x2c
/* 081A48 7F04CF18 31CF000E */  andi  $t7, $t6, 0xe
/* 081A4C 7F04CF1C 2DF80001 */  sltiu $t8, $t7, 1
/* 081A50 7F04CF20 AFB80010 */  sw    $t8, 0x10($sp)
/* 081A54 7F04CF24 AFA80018 */  sw    $t0, 0x18($sp)
/* 081A58 7F04CF28 AFA0001C */  sw    $zero, 0x1c($sp)
/* 081A5C 7F04CF2C 8FA5004C */  lw    $a1, 0x4c($sp)
/* 081A60 7F04CF30 8FA60038 */  lw    $a2, 0x38($sp)
/* 081A64 7F04CF34 87A7003E */  lh    $a3, 0x3e($sp)
/* 081A68 7F04CF38 0FC27094 */  jal   explosionCreate
/* 081A6C 7F04CF3C AFB90014 */   sw    $t9, 0x14($sp)
/* 081A70 7F04CF40 1000000E */  b     .L7F04CF7C
/* 081A74 7F04CF44 8E2C000C */   lw    $t4, 0xc($s1)
.L7F04CF48:
/* 081A78 7F04CF48 8E060014 */  lw    $a2, 0x14($s0)
.L7F04CF4C:
/* 081A7C 7F04CF4C 8FA90050 */  lw    $t1, 0x50($sp)
/* 081A80 7F04CF50 260A002C */  addiu $t2, $s0, 0x2c
/* 081A84 7F04CF54 240B0001 */  li    $t3, 1
/* 081A88 7F04CF58 AFAB001C */  sw    $t3, 0x1c($sp)
/* 081A8C 7F04CF5C AFAA0018 */  sw    $t2, 0x18($sp)
/* 081A90 7F04CF60 AFA00010 */  sw    $zero, 0x10($sp)
/* 081A94 7F04CF64 8FA40044 */  lw    $a0, 0x44($sp)
/* 081A98 7F04CF68 8FA5004C */  lw    $a1, 0x4c($sp)
/* 081A9C 7F04CF6C 87A7003E */  lh    $a3, 0x3e($sp)
/* 081AA0 7F04CF70 0FC27094 */  jal   explosionCreate
/* 081AA4 7F04CF74 AFA90014 */   sw    $t1, 0x14($sp)
/* 081AA8 7F04CF78 8E2C000C */  lw    $t4, 0xc($s1)
.L7F04CF7C:
/* 081AAC 7F04CF7C 02202025 */  move  $a0, $s1
/* 081AB0 7F04CF80 318D2000 */  andi  $t5, $t4, 0x2000
/* 081AB4 7F04CF84 11A00005 */  beqz  $t5, .L7F04CF9C
/* 081AB8 7F04CF88 00000000 */   nop
/* 081ABC 7F04CF8C 8E2E0064 */  lw    $t6, 0x64($s1)
/* 081AC0 7F04CF90 35CF0004 */  ori   $t7, $t6, 4
/* 081AC4 7F04CF94 10000070 */  b     .L7F04D158
/* 081AC8 7F04CF98 AE2F0064 */   sw    $t7, 0x64($s1)
.L7F04CF9C:
/* 081ACC 7F04CF9C 0FC12D84 */  jal   objDeform
/* 081AD0 7F04CFA0 24050001 */   li    $a1, 1
/* 081AD4 7F04CFA4 8FA40044 */  lw    $a0, 0x44($sp)
/* 081AD8 7F04CFA8 5604006C */  bnel  $s0, $a0, .L7F04D15C
/* 081ADC 7F04CFAC 8FBF002C */   lw    $ra, 0x2c($sp)
/* 081AE0 7F04CFB0 0FC13341 */  jal   objDestroySupportedObjects
/* 081AE4 7F04CFB4 8FA50050 */   lw    $a1, 0x50($sp)
/* 081AE8 7F04CFB8 8E220064 */  lw    $v0, 0x64($s1)
/* 081AEC 7F04CFBC 30588000 */  andi  $t8, $v0, 0x8000
/* 081AF0 7F04CFC0 5300000A */  beql  $t8, $zero, .L7F04CFEC
/* 081AF4 7F04CFC4 3C010001 */   lui   $at, 1
/* 081AF8 7F04CFC8 0C002914 */  jal   randomGetNext
/* 081AFC 7F04CFCC 00000000 */   nop
/* 081B00 7F04CFD0 24010003 */  li    $at, 3
/* 081B04 7F04CFD4 0041001B */  divu  $zero, $v0, $at
/* 081B08 7F04CFD8 0000C810 */  mfhi  $t9
/* 081B0C 7F04CFDC 5720005F */  bnezl $t9, .L7F04D15C
/* 081B10 7F04CFE0 8FBF002C */   lw    $ra, 0x2c($sp)
/* 081B14 7F04CFE4 8E220064 */  lw    $v0, 0x64($s1)
/* 081B18 7F04CFE8 3C010001 */  lui   $at, 1
.L7F04CFEC:
/* 081B1C 7F04CFEC 00414025 */  or    $t0, $v0, $at
/* 081B20 7F04CFF0 AE280064 */  sw    $t0, 0x64($s1)
/* 081B24 7F04CFF4 8FA50050 */  lw    $a1, 0x50($sp)
/* 081B28 7F04CFF8 0FC13291 */  jal   objFall
/* 081B2C 7F04CFFC 02202025 */   move  $a0, $s1
/* 081B30 7F04D000 10000056 */  b     .L7F04D15C
/* 081B34 7F04D004 8FBF002C */   lw    $ra, 0x2c($sp)
.L7F04D008:
/* 081B38 7F04D008 0FC0FFD8 */  jal   objGetShotsTaken
/* 081B3C 7F04D00C 02202025 */   move  $a0, $s1
/* 081B40 7F04D010 30490003 */  andi  $t1, $v0, 3
/* 081B44 7F04D014 15200031 */  bnez  $t1, .L7F04D0DC
/* 081B48 7F04D018 AFA20034 */   sw    $v0, 0x34($sp)
/* 081B4C 7F04D01C 00025083 */  sra   $t2, $v0, 2
/* 081B50 7F04D020 25450001 */  addiu $a1, $t2, 1
/* 081B54 7F04D024 0FC12D84 */  jal   objDeform
/* 081B58 7F04D028 02202025 */   move  $a0, $s1
/* 081B5C 7F04D02C 8FAB0038 */  lw    $t3, 0x38($sp)
/* 081B60 7F04D030 1160002A */  beqz  $t3, .L7F04D0DC
/* 081B64 7F04D034 00000000 */   nop
/* 081B68 7F04D038 920C0001 */  lbu   $t4, 1($s0)
/* 081B6C 7F04D03C 8FA2004C */  lw    $v0, 0x4c($sp)
/* 081B70 7F04D040 27A40038 */  addiu $a0, $sp, 0x38
/* 081B74 7F04D044 318D0008 */  andi  $t5, $t4, 8
/* 081B78 7F04D048 55A00019 */  bnezl $t5, .L7F04D0B0
/* 081B7C 7F04D04C 8E060014 */   lw    $a2, 0x14($s0)
/* 081B80 7F04D050 C4500008 */  lwc1  $f16, 8($v0)
/* 081B84 7F04D054 8E050008 */  lw    $a1, 8($s0)
/* 081B88 7F04D058 8E060010 */  lw    $a2, 0x10($s0)
/* 081B8C 7F04D05C 8C470000 */  lw    $a3, ($v0)
/* 081B90 7F04D060 0FC2C2F9 */  jal   walkTilesBetweenPoints_NoCallback
/* 081B94 7F04D064 E7B00010 */   swc1  $f16, 0x10($sp)
/* 081B98 7F04D068 10400010 */  beqz  $v0, .L7F04D0AC
/* 081B9C 7F04D06C 8FA40044 */   lw    $a0, 0x44($sp)
/* 081BA0 7F04D070 8E2E0008 */  lw    $t6, 8($s1)
/* 081BA4 7F04D074 8FB90050 */  lw    $t9, 0x50($sp)
/* 081BA8 7F04D078 2608002C */  addiu $t0, $s0, 0x2c
/* 081BAC 7F04D07C 31CF000E */  andi  $t7, $t6, 0xe
/* 081BB0 7F04D080 2DF80001 */  sltiu $t8, $t7, 1
/* 081BB4 7F04D084 AFB80010 */  sw    $t8, 0x10($sp)
/* 081BB8 7F04D088 AFA80018 */  sw    $t0, 0x18($sp)
/* 081BBC 7F04D08C AFA0001C */  sw    $zero, 0x1c($sp)
/* 081BC0 7F04D090 8FA5004C */  lw    $a1, 0x4c($sp)
/* 081BC4 7F04D094 8FA60038 */  lw    $a2, 0x38($sp)
/* 081BC8 7F04D098 24070010 */  li    $a3, 16
/* 081BCC 7F04D09C 0FC27094 */  jal   explosionCreate
/* 081BD0 7F04D0A0 AFB90014 */   sw    $t9, 0x14($sp)
/* 081BD4 7F04D0A4 1000000D */  b     .L7F04D0DC
/* 081BD8 7F04D0A8 00000000 */   nop
.L7F04D0AC:
/* 081BDC 7F04D0AC 8E060014 */  lw    $a2, 0x14($s0)
.L7F04D0B0:
/* 081BE0 7F04D0B0 8FA90050 */  lw    $t1, 0x50($sp)
/* 081BE4 7F04D0B4 260A002C */  addiu $t2, $s0, 0x2c
/* 081BE8 7F04D0B8 240B0001 */  li    $t3, 1
/* 081BEC 7F04D0BC AFAB001C */  sw    $t3, 0x1c($sp)
/* 081BF0 7F04D0C0 AFAA0018 */  sw    $t2, 0x18($sp)
/* 081BF4 7F04D0C4 AFA00010 */  sw    $zero, 0x10($sp)
/* 081BF8 7F04D0C8 8FA40044 */  lw    $a0, 0x44($sp)
/* 081BFC 7F04D0CC 8FA5004C */  lw    $a1, 0x4c($sp)
/* 081C00 7F04D0D0 24070010 */  li    $a3, 16
/* 081C04 7F04D0D4 0FC27094 */  jal   explosionCreate
/* 081C08 7F04D0D8 AFA90014 */   sw    $t1, 0x14($sp)
.L7F04D0DC:
/* 081C0C 7F04D0DC 0FC0FFF0 */  jal   objGetDestroyedLevel
/* 081C10 7F04D0E0 02202025 */   move  $a0, $s1
/* 081C14 7F04D0E4 58400012 */  blezl $v0, .L7F04D130
/* 081C18 7F04D0E8 8FA90034 */   lw    $t1, 0x34($sp)
/* 081C1C 7F04D0EC 92220002 */  lbu   $v0, 2($s1)
/* 081C20 7F04D0F0 304C0004 */  andi  $t4, $v0, 4
/* 081C24 7F04D0F4 5180000E */  beql  $t4, $zero, .L7F04D130
/* 081C28 7F04D0F8 8FA90034 */   lw    $t1, 0x34($sp)
/* 081C2C 7F04D0FC 8E2D0064 */  lw    $t5, 0x64($s1)
/* 081C30 7F04D100 344F0010 */  ori   $t7, $v0, 0x10
/* 081C34 7F04D104 3058FFEF */  andi  $t8, $v0, 0xffef
/* 081C38 7F04D108 31AE1000 */  andi  $t6, $t5, 0x1000
/* 081C3C 7F04D10C 51C00004 */  beql  $t6, $zero, .L7F04D120
/* 081C40 7F04D110 A2380002 */   sb    $t8, 2($s1)
/* 081C44 7F04D114 10000002 */  b     .L7F04D120
/* 081C48 7F04D118 A22F0002 */   sb    $t7, 2($s1)
/* 081C4C 7F04D11C A2380002 */  sb    $t8, 2($s1)
.L7F04D120:
/* 081C50 7F04D120 8FA80044 */  lw    $t0, 0x44($sp)
/* 081C54 7F04D124 241904B0 */  li    $t9, 1200
/* 081C58 7F04D128 A5190002 */  sh    $t9, 2($t0)
/* 081C5C 7F04D12C 8FA90034 */  lw    $t1, 0x34($sp)
.L7F04D130:
/* 081C60 7F04D130 2921000C */  slti  $at, $t1, 0xc
/* 081C64 7F04D134 54200009 */  bnezl $at, .L7F04D15C
/* 081C68 7F04D138 8FBF002C */   lw    $ra, 0x2c($sp)
/* 081C6C 7F04D13C 8E2A0064 */  lw    $t2, 0x64($s1)
/* 081C70 7F04D140 8E2C0008 */  lw    $t4, 8($s1)
/* 081C74 7F04D144 2401FEFF */  li    $at, -257
/* 081C78 7F04D148 354B1000 */  ori   $t3, $t2, 0x1000
/* 081C7C 7F04D14C 01816824 */  and   $t5, $t4, $at
/* 081C80 7F04D150 AE2B0064 */  sw    $t3, 0x64($s1)
/* 081C84 7F04D154 AE2D0008 */  sw    $t5, 8($s1)
.L7F04D158:
/* 081C88 7F04D158 8FBF002C */  lw    $ra, 0x2c($sp)
.L7F04D15C:
/* 081C8C 7F04D15C 8FB00024 */  lw    $s0, 0x24($sp)
/* 081C90 7F04D160 8FB10028 */  lw    $s1, 0x28($sp)
/* 081C94 7F04D164 03E00008 */  jr    $ra
/* 081C98 7F04D168 27BD0048 */   addiu $sp, $sp, 0x48
)
#endif

#ifdef VERSION_EU
GLOBAL_ASM(
.text
glabel object_explosion_related
/* 07F9F0 7F04D000 27BDFFB8 */  addiu $sp, $sp, -0x48
/* 07F9F4 7F04D004 AFBF002C */  sw    $ra, 0x2c($sp)
/* 07F9F8 7F04D008 AFB10028 */  sw    $s1, 0x28($sp)
/* 07F9FC 7F04D00C AFB00024 */  sw    $s0, 0x24($sp)
/* 07FA00 7F04D010 AFA5004C */  sw    $a1, 0x4c($sp)
/* 07FA04 7F04D014 AFA60050 */  sw    $a2, 0x50($sp)
/* 07FA08 7F04D018 C4860070 */  lwc1  $f6, 0x70($a0)
/* 07FA0C 7F04D01C C4840074 */  lwc1  $f4, 0x74($a0)
/* 07FA10 7F04D020 00808825 */  move  $s1, $a0
/* 07FA14 7F04D024 4606203C */  c.lt.s $f4, $f6
/* 07FA18 7F04D028 00000000 */  nop
/* 07FA1C 7F04D02C 45030006 */  bc1tl .L7F04D048
/* 07FA20 7F04D030 862E0004 */   lh    $t6, 4($s1)
/* 07FA24 7F04D034 0FC10020 */  jal   objGetDestroyedLevel
/* 07FA28 7F04D038 00000000 */   nop
/* 07FA2C 7F04D03C 504000C1 */  beql  $v0, $zero, .L7F04D344
/* 07FA30 7F04D040 8FBF002C */   lw    $ra, 0x2c($sp)
/* 07FA34 7F04D044 862E0004 */  lh    $t6, 4($s1)
.L7F04D048:
/* 07FA38 7F04D048 3C0F8003 */  lui   $t7, %hi(object_explosion_details)
/* 07FA3C 7F04D04C 8E230010 */  lw    $v1, 0x10($s1)
/* 07FA40 7F04D050 01EE7821 */  addu  $t7, $t7, $t6
/* 07FA44 7F04D054 81EF5D84 */  lb    $t7, %lo(object_explosion_details)($t7)
/* 07FA48 7F04D058 02202025 */  move  $a0, $s1
/* 07FA4C 7F04D05C 00608025 */  move  $s0, $v1
/* 07FA50 7F04D060 A7AF003E */  sh    $t7, 0x3e($sp)
/* 07FA54 7F04D064 8C78001C */  lw    $t8, 0x1c($v1)
/* 07FA58 7F04D068 53000007 */  beql  $t8, $zero, .L7F04D088
/* 07FA5C 7F04D06C 8E190014 */   lw    $t9, 0x14($s0)
/* 07FA60 7F04D070 8E02001C */  lw    $v0, 0x1c($s0)
/* 07FA64 7F04D074 00408025 */  move  $s0, $v0
.L7F04D078:
/* 07FA68 7F04D078 8C42001C */  lw    $v0, 0x1c($v0)
/* 07FA6C 7F04D07C 5440FFFE */  bnezl $v0, .L7F04D078
/* 07FA70 7F04D080 00408025 */   move  $s0, $v0
/* 07FA74 7F04D084 8E190014 */  lw    $t9, 0x14($s0)
.L7F04D088:
/* 07FA78 7F04D088 AFA30044 */  sw    $v1, 0x44($sp)
/* 07FA7C 7F04D08C 0FC10020 */  jal   objGetDestroyedLevel
/* 07FA80 7F04D090 AFB90038 */   sw    $t9, 0x38($sp)
/* 07FA84 7F04D094 14400056 */  bnez  $v0, .L7F04D1F0
/* 07FA88 7F04D098 00000000 */   nop
/* 07FA8C 7F04D09C 92280002 */  lbu   $t0, 2($s1)
/* 07FA90 7F04D0A0 44804000 */  mtc1  $zero, $f8
/* 07FA94 7F04D0A4 35090080 */  ori   $t1, $t0, 0x80
/* 07FA98 7F04D0A8 A2290002 */  sb    $t1, 2($s1)
/* 07FA9C 7F04D0AC E6280070 */  swc1  $f8, 0x70($s1)
/* 07FAA0 7F04D0B0 8FAA0038 */  lw    $t2, 0x38($sp)
/* 07FAA4 7F04D0B4 5140002B */  beql  $t2, $zero, .L7F04D164
/* 07FAA8 7F04D0B8 8E2B000C */   lw    $t3, 0xc($s1)
/* 07FAAC 7F04D0BC 920B0001 */  lbu   $t3, 1($s0)
/* 07FAB0 7F04D0C0 8FA2004C */  lw    $v0, 0x4c($sp)
/* 07FAB4 7F04D0C4 27A40038 */  addiu $a0, $sp, 0x38
/* 07FAB8 7F04D0C8 316C0008 */  andi  $t4, $t3, 8
/* 07FABC 7F04D0CC 55800019 */  bnezl $t4, .L7F04D134
/* 07FAC0 7F04D0D0 8E060014 */   lw    $a2, 0x14($s0)
/* 07FAC4 7F04D0D4 C44A0008 */  lwc1  $f10, 8($v0)
/* 07FAC8 7F04D0D8 8E050008 */  lw    $a1, 8($s0)
/* 07FACC 7F04D0DC 8E060010 */  lw    $a2, 0x10($s0)
/* 07FAD0 7F04D0E0 8C470000 */  lw    $a3, ($v0)
/* 07FAD4 7F04D0E4 0FC2BFA9 */  jal   walkTilesBetweenPoints_NoCallback
/* 07FAD8 7F04D0E8 E7AA0010 */   swc1  $f10, 0x10($sp)
/* 07FADC 7F04D0EC 10400010 */  beqz  $v0, .L7F04D130
/* 07FAE0 7F04D0F0 8FA40044 */   lw    $a0, 0x44($sp)
/* 07FAE4 7F04D0F4 8E2D0008 */  lw    $t5, 8($s1)
/* 07FAE8 7F04D0F8 8FB80050 */  lw    $t8, 0x50($sp)
/* 07FAEC 7F04D0FC 2619002C */  addiu $t9, $s0, 0x2c
/* 07FAF0 7F04D100 31AE000E */  andi  $t6, $t5, 0xe
/* 07FAF4 7F04D104 2DCF0001 */  sltiu $t7, $t6, 1
/* 07FAF8 7F04D108 AFAF0010 */  sw    $t7, 0x10($sp)
/* 07FAFC 7F04D10C AFB90018 */  sw    $t9, 0x18($sp)
/* 07FB00 7F04D110 AFA0001C */  sw    $zero, 0x1c($sp)
/* 07FB04 7F04D114 8FA5004C */  lw    $a1, 0x4c($sp)
/* 07FB08 7F04D118 8FA60038 */  lw    $a2, 0x38($sp)
/* 07FB0C 7F04D11C 87A7003E */  lh    $a3, 0x3e($sp)
/* 07FB10 7F04D120 0FC26DE4 */  jal   explosionCreate
/* 07FB14 7F04D124 AFB80014 */   sw    $t8, 0x14($sp)
/* 07FB18 7F04D128 1000000E */  b     .L7F04D164
/* 07FB1C 7F04D12C 8E2B000C */   lw    $t3, 0xc($s1)
.L7F04D130:
/* 07FB20 7F04D130 8E060014 */  lw    $a2, 0x14($s0)
.L7F04D134:
/* 07FB24 7F04D134 8FA80050 */  lw    $t0, 0x50($sp)
/* 07FB28 7F04D138 2609002C */  addiu $t1, $s0, 0x2c
/* 07FB2C 7F04D13C 240A0001 */  li    $t2, 1
/* 07FB30 7F04D140 AFAA001C */  sw    $t2, 0x1c($sp)
/* 07FB34 7F04D144 AFA90018 */  sw    $t1, 0x18($sp)
/* 07FB38 7F04D148 AFA00010 */  sw    $zero, 0x10($sp)
/* 07FB3C 7F04D14C 8FA40044 */  lw    $a0, 0x44($sp)
/* 07FB40 7F04D150 8FA5004C */  lw    $a1, 0x4c($sp)
/* 07FB44 7F04D154 87A7003E */  lh    $a3, 0x3e($sp)
/* 07FB48 7F04D158 0FC26DE4 */  jal   explosionCreate
/* 07FB4C 7F04D15C AFA80014 */   sw    $t0, 0x14($sp)
/* 07FB50 7F04D160 8E2B000C */  lw    $t3, 0xc($s1)
.L7F04D164:
/* 07FB54 7F04D164 02202025 */  move  $a0, $s1
/* 07FB58 7F04D168 316C2000 */  andi  $t4, $t3, 0x2000
/* 07FB5C 7F04D16C 11800005 */  beqz  $t4, .L7F04D184
/* 07FB60 7F04D170 00000000 */   nop
/* 07FB64 7F04D174 8E2D0064 */  lw    $t5, 0x64($s1)
/* 07FB68 7F04D178 35AE0004 */  ori   $t6, $t5, 4
/* 07FB6C 7F04D17C 10000070 */  b     .L7F04D340
/* 07FB70 7F04D180 AE2E0064 */   sw    $t6, 0x64($s1)
.L7F04D184:
/* 07FB74 7F04D184 0FC12DFB */  jal   objDeform
/* 07FB78 7F04D188 24050001 */   li    $a1, 1
/* 07FB7C 7F04D18C 8FA40044 */  lw    $a0, 0x44($sp)
/* 07FB80 7F04D190 5604006C */  bnel  $s0, $a0, .L7F04D344
/* 07FB84 7F04D194 8FBF002C */   lw    $ra, 0x2c($sp)
/* 07FB88 7F04D198 0FC133BE */  jal   objDestroySupportedObjects
/* 07FB8C 7F04D19C 8FA50050 */   lw    $a1, 0x50($sp)
/* 07FB90 7F04D1A0 8E220064 */  lw    $v0, 0x64($s1)
/* 07FB94 7F04D1A4 304F8000 */  andi  $t7, $v0, 0x8000
/* 07FB98 7F04D1A8 51E0000A */  beql  $t7, $zero, .L7F04D1D4
/* 07FB9C 7F04D1AC 3C010001 */   lui   $at, 1
/* 07FBA0 7F04D1B0 0C00262C */  jal   randomGetNext
/* 07FBA4 7F04D1B4 00000000 */   nop
/* 07FBA8 7F04D1B8 24010003 */  li    $at, 3
/* 07FBAC 7F04D1BC 0041001B */  divu  $zero, $v0, $at
/* 07FBB0 7F04D1C0 0000C010 */  mfhi  $t8
/* 07FBB4 7F04D1C4 5700005F */  bnezl $t8, .L7F04D344
/* 07FBB8 7F04D1C8 8FBF002C */   lw    $ra, 0x2c($sp)
/* 07FBBC 7F04D1CC 8E220064 */  lw    $v0, 0x64($s1)
/* 07FBC0 7F04D1D0 3C010001 */  lui   $at, 1
.L7F04D1D4:
/* 07FBC4 7F04D1D4 0041C825 */  or    $t9, $v0, $at
/* 07FBC8 7F04D1D8 AE390064 */  sw    $t9, 0x64($s1)
/* 07FBCC 7F04D1DC 8FA50050 */  lw    $a1, 0x50($sp)
/* 07FBD0 7F04D1E0 0FC1330E */  jal   objFall
/* 07FBD4 7F04D1E4 02202025 */   move  $a0, $s1
/* 07FBD8 7F04D1E8 10000056 */  b     .L7F04D344
/* 07FBDC 7F04D1EC 8FBF002C */   lw    $ra, 0x2c($sp)
.L7F04D1F0:
/* 07FBE0 7F04D1F0 0FC10008 */  jal   objGetShotsTaken
/* 07FBE4 7F04D1F4 02202025 */   move  $a0, $s1
/* 07FBE8 7F04D1F8 30480003 */  andi  $t0, $v0, 3
/* 07FBEC 7F04D1FC 15000031 */  bnez  $t0, .L7F04D2C4eu
/* 07FBF0 7F04D200 AFA20034 */   sw    $v0, 0x34($sp)
/* 07FBF4 7F04D204 00024883 */  sra   $t1, $v0, 2
/* 07FBF8 7F04D208 25250001 */  addiu $a1, $t1, 1
/* 07FBFC 7F04D20C 0FC12DFB */  jal   objDeform
/* 07FC00 7F04D210 02202025 */   move  $a0, $s1
/* 07FC04 7F04D214 8FAA0038 */  lw    $t2, 0x38($sp)
/* 07FC08 7F04D218 1140002A */  beqz  $t2, .L7F04D2C4eu
/* 07FC0C 7F04D21C 00000000 */   nop
/* 07FC10 7F04D220 920B0001 */  lbu   $t3, 1($s0)
/* 07FC14 7F04D224 8FA2004C */  lw    $v0, 0x4c($sp)
/* 07FC18 7F04D228 27A40038 */  addiu $a0, $sp, 0x38
/* 07FC1C 7F04D22C 316C0008 */  andi  $t4, $t3, 8
/* 07FC20 7F04D230 55800019 */  bnezl $t4, .L7F04D298
/* 07FC24 7F04D234 8E060014 */   lw    $a2, 0x14($s0)
/* 07FC28 7F04D238 C4500008 */  lwc1  $f16, 8($v0)
/* 07FC2C 7F04D23C 8E050008 */  lw    $a1, 8($s0)
/* 07FC30 7F04D240 8E060010 */  lw    $a2, 0x10($s0)
/* 07FC34 7F04D244 8C470000 */  lw    $a3, ($v0)
/* 07FC38 7F04D248 0FC2BFA9 */  jal   walkTilesBetweenPoints_NoCallback
/* 07FC3C 7F04D24C E7B00010 */   swc1  $f16, 0x10($sp)
/* 07FC40 7F04D250 10400010 */  beqz  $v0, .L7F04D294
/* 07FC44 7F04D254 8FA40044 */   lw    $a0, 0x44($sp)
/* 07FC48 7F04D258 8E2D0008 */  lw    $t5, 8($s1)
/* 07FC4C 7F04D25C 8FB80050 */  lw    $t8, 0x50($sp)
/* 07FC50 7F04D260 2619002C */  addiu $t9, $s0, 0x2c
/* 07FC54 7F04D264 31AE000E */  andi  $t6, $t5, 0xe
/* 07FC58 7F04D268 2DCF0001 */  sltiu $t7, $t6, 1
/* 07FC5C 7F04D26C AFAF0010 */  sw    $t7, 0x10($sp)
/* 07FC60 7F04D270 AFB90018 */  sw    $t9, 0x18($sp)
/* 07FC64 7F04D274 AFA0001C */  sw    $zero, 0x1c($sp)
/* 07FC68 7F04D278 8FA5004C */  lw    $a1, 0x4c($sp)
/* 07FC6C 7F04D27C 8FA60038 */  lw    $a2, 0x38($sp)
/* 07FC70 7F04D280 24070010 */  li    $a3, 16
/* 07FC74 7F04D284 0FC26DE4 */  jal   explosionCreate
/* 07FC78 7F04D288 AFB80014 */   sw    $t8, 0x14($sp)
/* 07FC7C 7F04D28C 1000000D */  b     .L7F04D2C4eu
/* 07FC80 7F04D290 00000000 */   nop
.L7F04D294:
/* 07FC84 7F04D294 8E060014 */  lw    $a2, 0x14($s0)
.L7F04D298:
/* 07FC88 7F04D298 8FA80050 */  lw    $t0, 0x50($sp)
/* 07FC8C 7F04D29C 2609002C */  addiu $t1, $s0, 0x2c
/* 07FC90 7F04D2A0 240A0001 */  li    $t2, 1
/* 07FC94 7F04D2A4 AFAA001C */  sw    $t2, 0x1c($sp)
/* 07FC98 7F04D2A8 AFA90018 */  sw    $t1, 0x18($sp)
/* 07FC9C 7F04D2AC AFA00010 */  sw    $zero, 0x10($sp)
/* 07FCA0 7F04D2B0 8FA40044 */  lw    $a0, 0x44($sp)
/* 07FCA4 7F04D2B4 8FA5004C */  lw    $a1, 0x4c($sp)
/* 07FCA8 7F04D2B8 24070010 */  li    $a3, 16
/* 07FCAC 7F04D2BC 0FC26DE4 */  jal   explosionCreate
/* 07FCB0 7F04D2C0 AFA80014 */   sw    $t0, 0x14($sp)
.L7F04D2C4eu:
/* 07FCB4 7F04D2C4 0FC10020 */  jal   objGetDestroyedLevel
/* 07FCB8 7F04D2C8 02202025 */   move  $a0, $s1
/* 07FCBC 7F04D2CC 58400012 */  blezl $v0, .L7F04D318
/* 07FCC0 7F04D2D0 8FA80034 */   lw    $t0, 0x34($sp)
/* 07FCC4 7F04D2D4 92220002 */  lbu   $v0, 2($s1)
/* 07FCC8 7F04D2D8 304B0004 */  andi  $t3, $v0, 4
/* 07FCCC 7F04D2DC 5160000E */  beql  $t3, $zero, .L7F04D318
/* 07FCD0 7F04D2E0 8FA80034 */   lw    $t0, 0x34($sp)
/* 07FCD4 7F04D2E4 8E2C0064 */  lw    $t4, 0x64($s1)
/* 07FCD8 7F04D2E8 344E0010 */  ori   $t6, $v0, 0x10
/* 07FCDC 7F04D2EC 304FFFEF */  andi  $t7, $v0, 0xffef
/* 07FCE0 7F04D2F0 318D1000 */  andi  $t5, $t4, 0x1000
/* 07FCE4 7F04D2F4 51A00004 */  beql  $t5, $zero, .L7F04D308
/* 07FCE8 7F04D2F8 A22F0002 */   sb    $t7, 2($s1)
/* 07FCEC 7F04D2FC 10000002 */  b     .L7F04D308
/* 07FCF0 7F04D300 A22E0002 */   sb    $t6, 2($s1)
/* 07FCF4 7F04D304 A22F0002 */  sb    $t7, 2($s1)
.L7F04D308:
/* 07FCF8 7F04D308 8FB90044 */  lw    $t9, 0x44($sp)
/* 07FCFC 7F04D30C 241803E8 */  li    $t8, 1000
/* 07FD00 7F04D310 A7380002 */  sh    $t8, 2($t9)
/* 07FD04 7F04D314 8FA80034 */  lw    $t0, 0x34($sp)
.L7F04D318:
/* 07FD08 7F04D318 2901000C */  slti  $at, $t0, 0xc
/* 07FD0C 7F04D31C 54200009 */  bnezl $at, .L7F04D344
/* 07FD10 7F04D320 8FBF002C */   lw    $ra, 0x2c($sp)
/* 07FD14 7F04D324 8E290064 */  lw    $t1, 0x64($s1)
/* 07FD18 7F04D328 8E2B0008 */  lw    $t3, 8($s1)
/* 07FD1C 7F04D32C 2401FEFF */  li    $at, -257
/* 07FD20 7F04D330 352A1000 */  ori   $t2, $t1, 0x1000
/* 07FD24 7F04D334 01616024 */  and   $t4, $t3, $at
/* 07FD28 7F04D338 AE2A0064 */  sw    $t2, 0x64($s1)
/* 07FD2C 7F04D33C AE2C0008 */  sw    $t4, 8($s1)
.L7F04D340:
/* 07FD30 7F04D340 8FBF002C */  lw    $ra, 0x2c($sp)
.L7F04D344:
/* 07FD34 7F04D344 8FB00024 */  lw    $s0, 0x24($sp)
/* 07FD38 7F04D348 8FB10028 */  lw    $s1, 0x28($sp)
/* 07FD3C 7F04D34C 03E00008 */  jr    $ra
/* 07FD40 7F04D350 27BD0048 */   addiu $sp, $sp, 0x48
)
#endif
#endif




#ifdef NONMATCHING
bool bgTestHitOnObj(coord3d *arg0, coord3d *arg1, coord3d *arg2, Gfx *gdl, Gfx *gdl2, Vertex *vertices, struct HitThing *hitthing) {

}
#else
bool bgTestHitOnObj(coord3d *arg0, coord3d *arg1, coord3d *arg2, Gfx *gdl, Gfx *gdl2, Vertex *vertices, struct HitThing *hitthing);
u32 D_8003204C = 0x7FFF;
u32 D_80032050 = 0x7FFF;
u32 D_80032054 = 0x7FFF;
u32 D_80032058 = 0xFFFF8000;
u32 D_8003205C = 0xFFFF8000;
u32 D_80032060 = 0xFFFF8000;
u32 D_80032064 = 0;
u32 D_80032068 = 0;
u32 D_8003206C = 0;
u32 D_80032070 = 0x7FFF;
u32 D_80032074 = 0x7FFF;
u32 D_80032078 = 0x7FFF;
u32 D_8003207C = 0xFFFF8000;
u32 D_80032080 = 0xFFFF8000;
u32 D_80032084 = 0xFFFF8000;
u32 D_80032088 = 0;
u32 D_8003208C = 0;
u32 D_80032090 = 0;
GLOBAL_ASM(
.text
glabel bgTestHitOnObj
/* 081C9C 7F04D16C 27BDFEB0 */  addiu $sp, $sp, -0x150
/* 081CA0 7F04D170 F7B40028 */  sdc1  $f20, 0x28($sp)
/* 081CA4 7F04D174 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 081CA8 7F04D178 AFBE0050 */  sw    $fp, 0x50($sp)
/* 081CAC 7F04D17C AFB7004C */  sw    $s7, 0x4c($sp)
/* 081CB0 7F04D180 AFB60048 */  sw    $s6, 0x48($sp)
/* 081CB4 7F04D184 AFB50044 */  sw    $s5, 0x44($sp)
/* 081CB8 7F04D188 AFB40040 */  sw    $s4, 0x40($sp)
/* 081CBC 7F04D18C AFB3003C */  sw    $s3, 0x3c($sp)
/* 081CC0 7F04D190 AFB10034 */  sw    $s1, 0x34($sp)
/* 081CC4 7F04D194 AFB00030 */  sw    $s0, 0x30($sp)
/* 081CC8 7F04D198 4481A000 */  mtc1  $at, $f20
/* 081CCC 7F04D19C 00E08825 */  move  $s1, $a3
/* 081CD0 7F04D1A0 0080B825 */  move  $s7, $a0
/* 081CD4 7F04D1A4 00C0F025 */  move  $fp, $a2
/* 081CD8 7F04D1A8 AFBF0054 */  sw    $ra, 0x54($sp)
/* 081CDC 7F04D1AC AFB20038 */  sw    $s2, 0x38($sp)
/* 081CE0 7F04D1B0 AFA50154 */  sw    $a1, 0x154($sp)
/* 081CE4 7F04D1B4 AFA00110 */  sw    $zero, 0x110($sp)
/* 081CE8 7F04D1B8 00E09825 */  move  $s3, $a3
/* 081CEC 7F04D1BC 8FB0014C */  lw    $s0, 0x14c($sp)
/* 081CF0 7F04D1C0 8FB40168 */  lw    $s4, 0x168($sp)
/* 081CF4 7F04D1C4 241500FD */  li    $s5, 253
/* 081CF8 7F04D1C8 27B600B0 */  addiu $s6, $sp, 0xb0
.L7F04D1CC:
/* 081CFC 7F04D1CC 82220000 */  lb    $v0, ($s1)
/* 081D00 7F04D1D0 2401FFB8 */  li    $at, -72
/* 081D04 7F04D1D4 8FAE0160 */  lw    $t6, 0x160($sp)
/* 081D08 7F04D1D8 54410007 */  bnel  $v0, $at, .L7F04D1F8
/* 081D0C 7F04D1DC 24010004 */   li    $at, 4
/* 081D10 7F04D1E0 11C001E5 */  beqz  $t6, .L7F04D978
/* 081D14 7F04D1E4 01C09825 */   move  $s3, $t6
/* 081D18 7F04D1E8 01C08825 */  move  $s1, $t6
/* 081D1C 7F04D1EC 1000FFF7 */  b     .L7F04D1CC
/* 081D20 7F04D1F0 AFA00160 */   sw    $zero, 0x160($sp)
/* 081D24 7F04D1F4 24010004 */  li    $at, 4
.L7F04D1F8:
/* 081D28 7F04D1F8 1441000C */  bne   $v0, $at, .L7F04D22C
/* 081D2C 7F04D1FC 8FB90164 */   lw    $t9, 0x164($sp)
/* 081D30 7F04D200 92220001 */  lbu   $v0, 1($s1)
/* 081D34 7F04D204 8E230004 */  lw    $v1, 4($s1)
/* 081D38 7F04D208 3C0100FF */  lui   $at, (0x00FFFFFF >> 16) # lui $at, 0xff
/* 081D3C 7F04D20C 3421FFFF */  ori   $at, (0x00FFFFFF & 0xFFFF) # ori $at, $at, 0xffff
/* 081D40 7F04D210 304F000F */  andi  $t7, $v0, 0xf
/* 081D44 7F04D214 0061C024 */  and   $t8, $v1, $at
/* 081D48 7F04D218 03384021 */  addu  $t0, $t9, $t8
/* 081D4C 7F04D21C 000F4900 */  sll   $t1, $t7, 4
/* 081D50 7F04D220 01098023 */  subu  $s0, $t0, $t1
/* 081D54 7F04D224 1000FFE9 */  b     .L7F04D1CC
/* 081D58 7F04D228 26310008 */   addiu $s1, $s1, 8
.L7F04D22C:
/* 081D5C 7F04D22C 2401FFBF */  li    $at, -65
/* 081D60 7F04D230 144100E2 */  bne   $v0, $at, .L7F04D5BC
/* 081D64 7F04D234 2403000A */   li    $v1, 10
/* 081D68 7F04D238 3C0B8003 */  lui   $t3, %hi(D_8003204C)
/* 081D6C 7F04D23C 256B204C */  addiu $t3, %lo(D_8003204C) # addiu $t3, $t3, 0x204c
/* 081D70 7F04D240 8D610000 */  lw    $at, ($t3)
/* 081D74 7F04D244 27AA00E8 */  addiu $t2, $sp, 0xe8
/* 081D78 7F04D248 8D6D0004 */  lw    $t5, 4($t3)
/* 081D7C 7F04D24C AD410000 */  sw    $at, ($t2)
/* 081D80 7F04D250 8D610008 */  lw    $at, 8($t3)
/* 081D84 7F04D254 3C0F8003 */  lui   $t7, %hi(D_80032058)
/* 081D88 7F04D258 25EF2058 */  addiu $t7, %lo(D_80032058) # addiu $t7, $t7, 0x2058
/* 081D8C 7F04D25C AD4D0004 */  sw    $t5, 4($t2)
/* 081D90 7F04D260 AD410008 */  sw    $at, 8($t2)
/* 081D94 7F04D264 8DE10000 */  lw    $at, ($t7)
/* 081D98 7F04D268 27AE00DC */  addiu $t6, $sp, 0xdc
/* 081D9C 7F04D26C 8DF90004 */  lw    $t9, 4($t7)
/* 081DA0 7F04D270 ADC10000 */  sw    $at, ($t6)
/* 081DA4 7F04D274 8DE10008 */  lw    $at, 8($t7)
/* 081DA8 7F04D278 ADD90004 */  sw    $t9, 4($t6)
/* 081DAC 7F04D27C 00001025 */  move  $v0, $zero
/* 081DB0 7F04D280 ADC10008 */  sw    $at, 8($t6)
/* 081DB4 7F04D284 92280005 */  lbu   $t0, 5($s1)
/* 081DB8 7F04D288 27A400F8 */  addiu $a0, $sp, 0xf8
/* 081DBC 7F04D28C 03C02825 */  move  $a1, $fp
/* 081DC0 7F04D290 0103001A */  div   $zero, $t0, $v1
/* 081DC4 7F04D294 00004812 */  mflo  $t1
/* 081DC8 7F04D298 AFA900F8 */  sw    $t1, 0xf8($sp)
/* 081DCC 7F04D29C 922C0006 */  lbu   $t4, 6($s1)
/* 081DD0 7F04D2A0 14600002 */  bnez  $v1, .L7F04D2AC
/* 081DD4 7F04D2A4 00000000 */   nop
/* 081DD8 7F04D2A8 0007000D */  break 7
.L7F04D2AC:
/* 081DDC 7F04D2AC 2401FFFF */  li    $at, -1
/* 081DE0 7F04D2B0 14610004 */  bne   $v1, $at, .L7F04D2C4
/* 081DE4 7F04D2B4 3C018000 */   lui   $at, 0x8000
/* 081DE8 7F04D2B8 15010002 */  bne   $t0, $at, .L7F04D2C4
/* 081DEC 7F04D2BC 00000000 */   nop
/* 081DF0 7F04D2C0 0006000D */  break 6
.L7F04D2C4:
/* 081DF4 7F04D2C4 0183001A */  div   $zero, $t4, $v1
/* 081DF8 7F04D2C8 00005012 */  mflo  $t2
/* 081DFC 7F04D2CC AFAA00FC */  sw    $t2, 0xfc($sp)
/* 081E00 7F04D2D0 922B0007 */  lbu   $t3, 7($s1)
/* 081E04 7F04D2D4 14600002 */  bnez  $v1, .L7F04D2E0
/* 081E08 7F04D2D8 00000000 */   nop
/* 081E0C 7F04D2DC 0007000D */  break 7
.L7F04D2E0:
/* 081E10 7F04D2E0 2401FFFF */  li    $at, -1
/* 081E14 7F04D2E4 14610004 */  bne   $v1, $at, .L7F04D2F8
/* 081E18 7F04D2E8 3C018000 */   lui   $at, 0x8000
/* 081E1C 7F04D2EC 15810002 */  bne   $t4, $at, .L7F04D2F8
/* 081E20 7F04D2F0 00000000 */   nop
/* 081E24 7F04D2F4 0006000D */  break 6
.L7F04D2F8:
/* 081E28 7F04D2F8 0163001A */  div   $zero, $t3, $v1
/* 081E2C 7F04D2FC 00006812 */  mflo  $t5
/* 081E30 7F04D300 AFAD0100 */  sw    $t5, 0x100($sp)
/* 081E34 7F04D304 14600002 */  bnez  $v1, .L7F04D310
/* 081E38 7F04D308 00000000 */   nop
/* 081E3C 7F04D30C 0007000D */  break 7
.L7F04D310:
/* 081E40 7F04D310 2401FFFF */  li    $at, -1
/* 081E44 7F04D314 14610004 */  bne   $v1, $at, .L7F04D328
/* 081E48 7F04D318 3C018000 */   lui   $at, 0x8000
/* 081E4C 7F04D31C 15610002 */  bne   $t3, $at, .L7F04D328
/* 081E50 7F04D320 00000000 */   nop
/* 081E54 7F04D324 0006000D */  break 6
.L7F04D328:
/* 081E58 7F04D328 8C980000 */  lw    $t8, ($a0)
.L7F04D32C:
/* 081E5C 7F04D32C 8FAF00E8 */  lw    $t7, 0xe8($sp)
/* 081E60 7F04D330 24840004 */  addiu $a0, $a0, 4
/* 081E64 7F04D334 00187100 */  sll   $t6, $t8, 4
/* 081E68 7F04D338 020E1821 */  addu  $v1, $s0, $t6
/* 081E6C 7F04D33C 84620000 */  lh    $v0, ($v1)
/* 081E70 7F04D340 27AB0104 */  addiu $t3, $sp, 0x104
/* 081E74 7F04D344 004F082A */  slt   $at, $v0, $t7
/* 081E78 7F04D348 50200004 */  beql  $at, $zero, .L7F04D35C
/* 081E7C 7F04D34C 8FB900DC */   lw    $t9, 0xdc($sp)
/* 081E80 7F04D350 AFA200E8 */  sw    $v0, 0xe8($sp)
/* 081E84 7F04D354 84620000 */  lh    $v0, ($v1)
/* 081E88 7F04D358 8FB900DC */  lw    $t9, 0xdc($sp)
.L7F04D35C:
/* 081E8C 7F04D35C 0322082A */  slt   $at, $t9, $v0
/* 081E90 7F04D360 50200003 */  beql  $at, $zero, .L7F04D370
/* 081E94 7F04D364 84620002 */   lh    $v0, 2($v1)
/* 081E98 7F04D368 AFA200DC */  sw    $v0, 0xdc($sp)
/* 081E9C 7F04D36C 84620002 */  lh    $v0, 2($v1)
.L7F04D370:
/* 081EA0 7F04D370 8FA800EC */  lw    $t0, 0xec($sp)
/* 081EA4 7F04D374 0048082A */  slt   $at, $v0, $t0
/* 081EA8 7F04D378 50200004 */  beql  $at, $zero, .L7F04D38C
/* 081EAC 7F04D37C 8FA900E0 */   lw    $t1, 0xe0($sp)
/* 081EB0 7F04D380 AFA200EC */  sw    $v0, 0xec($sp)
/* 081EB4 7F04D384 84620002 */  lh    $v0, 2($v1)
/* 081EB8 7F04D388 8FA900E0 */  lw    $t1, 0xe0($sp)
.L7F04D38C:
/* 081EBC 7F04D38C 0122082A */  slt   $at, $t1, $v0
/* 081EC0 7F04D390 50200003 */  beql  $at, $zero, .L7F04D3A0
/* 081EC4 7F04D394 84620004 */   lh    $v0, 4($v1)
/* 081EC8 7F04D398 AFA200E0 */  sw    $v0, 0xe0($sp)
/* 081ECC 7F04D39C 84620004 */  lh    $v0, 4($v1)
.L7F04D3A0:
/* 081ED0 7F04D3A0 8FAC00F0 */  lw    $t4, 0xf0($sp)
/* 081ED4 7F04D3A4 004C082A */  slt   $at, $v0, $t4
/* 081ED8 7F04D3A8 50200004 */  beql  $at, $zero, .L7F04D3BC
/* 081EDC 7F04D3AC 8FAA00E4 */   lw    $t2, 0xe4($sp)
/* 081EE0 7F04D3B0 AFA200F0 */  sw    $v0, 0xf0($sp)
/* 081EE4 7F04D3B4 84620004 */  lh    $v0, 4($v1)
/* 081EE8 7F04D3B8 8FAA00E4 */  lw    $t2, 0xe4($sp)
.L7F04D3BC:
/* 081EEC 7F04D3BC 0142082A */  slt   $at, $t2, $v0
/* 081EF0 7F04D3C0 10200002 */  beqz  $at, .L7F04D3CC
/* 081EF4 7F04D3C4 00000000 */   nop
/* 081EF8 7F04D3C8 AFA200E4 */  sw    $v0, 0xe4($sp)
.L7F04D3CC:
/* 081EFC 7F04D3CC 548BFFD7 */  bnel  $a0, $t3, .L7F04D32C
/* 081F00 7F04D3D0 8C980000 */   lw    $t8, ($a0)
/* 081F04 7F04D3D4 02E02025 */  move  $a0, $s7
/* 081F08 7F04D3D8 27A600E8 */  addiu $a2, $sp, 0xe8
/* 081F0C 7F04D3DC 0FC2DB3B */  jal   bgTestLineIntersectsBbox
/* 081F10 7F04D3E0 27A700DC */   addiu $a3, $sp, 0xdc
/* 081F14 7F04D3E4 10400162 */  beqz  $v0, .L7F04D970
/* 081F18 7F04D3E8 3C0D8003 */   lui   $t5, %hi(D_80032064)
/* 081F1C 7F04D3EC 25AD2064 */  addiu $t5, %lo(D_80032064) # addiu $t5, $t5, 0x2064
/* 081F20 7F04D3F0 8DA10000 */  lw    $at, ($t5)
/* 081F24 7F04D3F4 27A700C4 */  addiu $a3, $sp, 0xc4
/* 081F28 7F04D3F8 27B8011C */  addiu $t8, $sp, 0x11c
/* 081F2C 7F04D3FC ACE10000 */  sw    $at, ($a3)
/* 081F30 7F04D400 8DAE0004 */  lw    $t6, 4($t5)
/* 081F34 7F04D404 ACEE0004 */  sw    $t6, 4($a3)
/* 081F38 7F04D408 8DA10008 */  lw    $at, 8($t5)
/* 081F3C 7F04D40C ACE10008 */  sw    $at, 8($a3)
/* 081F40 7F04D410 8FAC0100 */  lw    $t4, 0x100($sp)
/* 081F44 7F04D414 8FA800FC */  lw    $t0, 0xfc($sp)
/* 081F48 7F04D418 8FAF00F8 */  lw    $t7, 0xf8($sp)
/* 081F4C 7F04D41C 8FAB0154 */  lw    $t3, 0x154($sp)
/* 081F50 7F04D420 000C5100 */  sll   $t2, $t4, 4
/* 081F54 7F04D424 00084900 */  sll   $t1, $t0, 4
/* 081F58 7F04D428 000FC900 */  sll   $t9, $t7, 4
/* 081F5C 7F04D42C 02192021 */  addu  $a0, $s0, $t9
/* 081F60 7F04D430 02092821 */  addu  $a1, $s0, $t1
/* 081F64 7F04D434 020A3021 */  addu  $a2, $s0, $t2
/* 081F68 7F04D438 AFB8001C */  sw    $t8, 0x1c($sp)
/* 081F6C 7F04D43C AFBE0018 */  sw    $fp, 0x18($sp)
/* 081F70 7F04D440 AFB70010 */  sw    $s7, 0x10($sp)
/* 081F74 7F04D444 0FC24A24 */  jal   intersectLineTriangle
/* 081F78 7F04D448 AFAB0014 */   sw    $t3, 0x14($sp)
/* 081F7C 7F04D44C 10400148 */  beqz  $v0, .L7F04D970
/* 081F80 7F04D450 00000000 */   nop
/* 081F84 7F04D454 C7A4011C */  lwc1  $f4, 0x11c($sp)
/* 081F88 7F04D458 C6E80000 */  lwc1  $f8, ($s7)
/* 081F8C 7F04D45C C7B20120 */  lwc1  $f18, 0x120($sp)
/* 081F90 7F04D460 4600218D */  trunc.w.s $f6, $f4
/* 081F94 7F04D464 92290000 */  lbu   $t1, ($s1)
/* 081F98 7F04D468 02201025 */  move  $v0, $s1
/* 081F9C 7F04D46C 4600428D */  trunc.w.s $f10, $f8
/* 081FA0 7F04D470 440E3000 */  mfc1  $t6, $f6
/* 081FA4 7F04D474 C6E60004 */  lwc1  $f6, 4($s7)
/* 081FA8 7F04D478 4600910D */  trunc.w.s $f4, $f18
/* 081FAC 7F04D47C 44195000 */  mfc1  $t9, $f10
/* 081FB0 7F04D480 0271082B */  sltu  $at, $s3, $s1
/* 081FB4 7F04D484 4600320D */  trunc.w.s $f8, $f6
/* 081FB8 7F04D488 01D94023 */  subu  $t0, $t6, $t9
/* 081FBC 7F04D48C 44888000 */  mtc1  $t0, $f16
/* 081FC0 7F04D490 440C2000 */  mfc1  $t4, $f4
/* 081FC4 7F04D494 440B4000 */  mfc1  $t3, $f8
/* 081FC8 7F04D498 468080A0 */  cvt.s.w $f2, $f16
/* 081FCC 7F04D49C C7B00124 */  lwc1  $f16, 0x124($sp)
/* 081FD0 7F04D4A0 C6E40008 */  lwc1  $f4, 8($s7)
/* 081FD4 7F04D4A4 018BC023 */  subu  $t8, $t4, $t3
/* 081FD8 7F04D4A8 44985000 */  mtc1  $t8, $f10
/* 081FDC 7F04D4AC 4600848D */  trunc.w.s $f18, $f16
/* 081FE0 7F04D4B0 4600218D */  trunc.w.s $f6, $f4
/* 081FE4 7F04D4B4 440F9000 */  mfc1  $t7, $f18
/* 081FE8 7F04D4B8 46805320 */  cvt.s.w $f12, $f10
/* 081FEC 7F04D4BC 44193000 */  mfc1  $t9, $f6
/* 081FF0 7F04D4C0 46021282 */  mul.s $f10, $f2, $f2
/* 081FF4 7F04D4C4 01F94023 */  subu  $t0, $t7, $t9
/* 081FF8 7F04D4C8 44884000 */  mtc1  $t0, $f8
/* 081FFC 7F04D4CC 460C6402 */  mul.s $f16, $f12, $f12
/* 082000 7F04D4D0 12A9000A */  beq   $s5, $t1, .L7F04D4FC
/* 082004 7F04D4D4 468043A0 */   cvt.s.w $f14, $f8
/* 082008 7F04D4D8 10200008 */  beqz  $at, .L7F04D4FC
/* 08200C 7F04D4DC 00000000 */   nop
/* 082010 7F04D4E0 904AFFF8 */  lbu   $t2, -8($v0)
.L7F04D4E4:
/* 082014 7F04D4E4 2442FFF8 */  addiu $v0, $v0, -8
/* 082018 7F04D4E8 0262082B */  sltu  $at, $s3, $v0
/* 08201C 7F04D4EC 12AA0003 */  beq   $s5, $t2, .L7F04D4FC
/* 082020 7F04D4F0 00000000 */   nop
/* 082024 7F04D4F4 5420FFFB */  bnezl $at, .L7F04D4E4
/* 082028 7F04D4F8 904AFFF8 */   lbu   $t2, -8($v0)
.L7F04D4FC:
/* 08202C 7F04D4FC 460E7102 */  mul.s $f4, $f14, $f14
/* 082030 7F04D500 54530004 */  bnel  $v0, $s3, .L7F04D514
/* 082034 7F04D504 8C430004 */   lw    $v1, 4($v0)
/* 082038 7F04D508 10000006 */  b     .L7F04D524
/* 08203C 7F04D50C 2404FFFF */   li    $a0, -1
/* 082040 7F04D510 8C430004 */  lw    $v1, 4($v0)
.L7F04D514:
/* 082044 7F04D514 3C018000 */  lui   $at, 0x8000
/* 082048 7F04D518 2463FFF8 */  addiu $v1, $v1, -8
/* 08204C 7F04D51C 00616025 */  or    $t4, $v1, $at
/* 082050 7F04D520 95840000 */  lhu   $a0, ($t4)
.L7F04D524:
/* 082054 7F04D524 46105480 */  add.s $f18, $f10, $f16
/* 082058 7F04D528 46049000 */  add.s $f0, $f18, $f4
/* 08205C 7F04D52C 4614003C */  c.lt.s $f0, $f20
/* 082060 7F04D530 00000000 */  nop
/* 082064 7F04D534 4500010E */  bc1f  .L7F04D970
/* 082068 7F04D538 00000000 */   nop
/* 08206C 7F04D53C C7A6011C */  lwc1  $f6, 0x11c($sp)
/* 082070 7F04D540 240C0001 */  li    $t4, 1
/* 082074 7F04D544 46000506 */  mov.s $f20, $f0
/* 082078 7F04D548 E6860000 */  swc1  $f6, ($s4)
/* 08207C 7F04D54C C7A80120 */  lwc1  $f8, 0x120($sp)
/* 082080 7F04D550 E6880004 */  swc1  $f8, 4($s4)
/* 082084 7F04D554 C7AA0124 */  lwc1  $f10, 0x124($sp)
/* 082088 7F04D558 E68A0008 */  swc1  $f10, 8($s4)
/* 08208C 7F04D55C C7B00128 */  lwc1  $f16, 0x128($sp)
/* 082090 7F04D560 E690000C */  swc1  $f16, 0xc($s4)
/* 082094 7F04D564 C7B2012C */  lwc1  $f18, 0x12c($sp)
/* 082098 7F04D568 E6920010 */  swc1  $f18, 0x10($s4)
/* 08209C 7F04D56C C7A40130 */  lwc1  $f4, 0x130($sp)
/* 0820A0 7F04D570 E6840014 */  swc1  $f4, 0x14($s4)
/* 0820A4 7F04D574 8FAB00F8 */  lw    $t3, 0xf8($sp)
/* 0820A8 7F04D578 000BC100 */  sll   $t8, $t3, 4
/* 0820AC 7F04D57C 03106821 */  addu  $t5, $t8, $s0
/* 0820B0 7F04D580 AE8D0018 */  sw    $t5, 0x18($s4)
/* 0820B4 7F04D584 8FAE00FC */  lw    $t6, 0xfc($sp)
/* 0820B8 7F04D588 000E7900 */  sll   $t7, $t6, 4
/* 0820BC 7F04D58C 01F0C821 */  addu  $t9, $t7, $s0
/* 0820C0 7F04D590 AE99001C */  sw    $t9, 0x1c($s4)
/* 0820C4 7F04D594 8FA80100 */  lw    $t0, 0x100($sp)
/* 0820C8 7F04D598 A684002A */  sh    $a0, 0x2a($s4)
/* 0820CC 7F04D59C AE910024 */  sw    $s1, 0x24($s4)
/* 0820D0 7F04D5A0 00084900 */  sll   $t1, $t0, 4
/* 0820D4 7F04D5A4 01305021 */  addu  $t2, $t1, $s0
/* 0820D8 7F04D5A8 AE8A0020 */  sw    $t2, 0x20($s4)
/* 0820DC 7F04D5AC A6800028 */  sh    $zero, 0x28($s4)
/* 0820E0 7F04D5B0 AFAC0110 */  sw    $t4, 0x110($sp)
/* 0820E4 7F04D5B4 1000FF05 */  b     .L7F04D1CC
/* 0820E8 7F04D5B8 26310008 */   addiu $s1, $s1, 8
.L7F04D5BC:
/* 0820EC 7F04D5BC 2401FFB1 */  li    $at, -79
/* 0820F0 7F04D5C0 144100EB */  bne   $v0, $at, .L7F04D970
/* 0820F4 7F04D5C4 00000000 */   nop
/* 0820F8 7F04D5C8 00009025 */  move  $s2, $zero
.L7F04D5CC:
/* 0820FC 7F04D5CC 3C188003 */  lui   $t8, %hi(D_80032070)
/* 082100 7F04D5D0 27182070 */  addiu $t8, %lo(D_80032070) # addiu $t8, $t8, 0x2070
/* 082104 7F04D5D4 8F010000 */  lw    $at, ($t8)
/* 082108 7F04D5D8 27AB0094 */  addiu $t3, $sp, 0x94
/* 08210C 7F04D5DC 8F0E0004 */  lw    $t6, 4($t8)
/* 082110 7F04D5E0 AD610000 */  sw    $at, ($t3)
/* 082114 7F04D5E4 8F010008 */  lw    $at, 8($t8)
/* 082118 7F04D5E8 3C198003 */  lui   $t9, %hi(D_8003207C)
/* 08211C 7F04D5EC 2739207C */  addiu $t9, %lo(D_8003207C) # addiu $t9, $t9, 0x207c
/* 082120 7F04D5F0 AD6E0004 */  sw    $t6, 4($t3)
/* 082124 7F04D5F4 AD610008 */  sw    $at, 8($t3)
/* 082128 7F04D5F8 8F210000 */  lw    $at, ($t9)
/* 08212C 7F04D5FC 27AF0088 */  addiu $t7, $sp, 0x88
/* 082130 7F04D600 8F290004 */  lw    $t1, 4($t9)
/* 082134 7F04D604 ADE10000 */  sw    $at, ($t7)
/* 082138 7F04D608 8F210008 */  lw    $at, 8($t9)
/* 08213C 7F04D60C ADE90004 */  sw    $t1, 4($t7)
/* 082140 7F04D610 1640000B */  bnez  $s2, .L7F04D640
/* 082144 7F04D614 ADE10008 */   sw    $at, 8($t7)
/* 082148 7F04D618 8E2A0004 */  lw    $t2, 4($s1)
/* 08214C 7F04D61C 314C000F */  andi  $t4, $t2, 0xf
/* 082150 7F04D620 AFAC00A4 */  sw    $t4, 0xa4($sp)
/* 082154 7F04D624 922D0007 */  lbu   $t5, 7($s1)
/* 082158 7F04D628 000D5902 */  srl   $t3, $t5, 4
/* 08215C 7F04D62C AFAB00A8 */  sw    $t3, 0xa8($sp)
/* 082160 7F04D630 8E380000 */  lw    $t8, ($s1)
/* 082164 7F04D634 330E000F */  andi  $t6, $t8, 0xf
/* 082168 7F04D638 10000024 */  b     .L7F04D6CC
/* 08216C 7F04D63C AFAE00AC */   sw    $t6, 0xac($sp)
.L7F04D640:
/* 082170 7F04D640 24010001 */  li    $at, 1
/* 082174 7F04D644 5641000C */  bnel  $s2, $at, .L7F04D678
/* 082178 7F04D648 24010002 */   li    $at, 2
/* 08217C 7F04D64C 92280006 */  lbu   $t0, 6($s1)
/* 082180 7F04D650 310F000F */  andi  $t7, $t0, 0xf
/* 082184 7F04D654 AFAF00A4 */  sw    $t7, 0xa4($sp)
/* 082188 7F04D658 96390006 */  lhu   $t9, 6($s1)
/* 08218C 7F04D65C 00194B02 */  srl   $t1, $t9, 0xc
/* 082190 7F04D660 AFA900A8 */  sw    $t1, 0xa8($sp)
/* 082194 7F04D664 922A0003 */  lbu   $t2, 3($s1)
/* 082198 7F04D668 000A6102 */  srl   $t4, $t2, 4
/* 08219C 7F04D66C 10000017 */  b     .L7F04D6CC
/* 0821A0 7F04D670 AFAC00AC */   sw    $t4, 0xac($sp)
/* 0821A4 7F04D674 24010002 */  li    $at, 2
.L7F04D678:
/* 0821A8 7F04D678 5641000C */  bnel  $s2, $at, .L7F04D6AC
/* 0821AC 7F04D67C 92390004 */   lbu   $t9, 4($s1)
/* 0821B0 7F04D680 962D0004 */  lhu   $t5, 4($s1)
/* 0821B4 7F04D684 31AB000F */  andi  $t3, $t5, 0xf
/* 0821B8 7F04D688 AFAB00A4 */  sw    $t3, 0xa4($sp)
/* 0821BC 7F04D68C 92380005 */  lbu   $t8, 5($s1)
/* 0821C0 7F04D690 00187102 */  srl   $t6, $t8, 4
/* 0821C4 7F04D694 AFAE00A8 */  sw    $t6, 0xa8($sp)
/* 0821C8 7F04D698 92280002 */  lbu   $t0, 2($s1)
/* 0821CC 7F04D69C 310F000F */  andi  $t7, $t0, 0xf
/* 0821D0 7F04D6A0 1000000A */  b     .L7F04D6CC
/* 0821D4 7F04D6A4 AFAF00AC */   sw    $t7, 0xac($sp)
/* 0821D8 7F04D6A8 92390004 */  lbu   $t9, 4($s1)
.L7F04D6AC:
/* 0821DC 7F04D6AC 3329000F */  andi  $t1, $t9, 0xf
/* 0821E0 7F04D6B0 AFA900A4 */  sw    $t1, 0xa4($sp)
/* 0821E4 7F04D6B4 8E2A0004 */  lw    $t2, 4($s1)
/* 0821E8 7F04D6B8 000A6702 */  srl   $t4, $t2, 0x1c
/* 0821EC 7F04D6BC AFAC00A8 */  sw    $t4, 0xa8($sp)
/* 0821F0 7F04D6C0 962D0002 */  lhu   $t5, 2($s1)
/* 0821F4 7F04D6C4 000D5B02 */  srl   $t3, $t5, 0xc
/* 0821F8 7F04D6C8 AFAB00AC */  sw    $t3, 0xac($sp)
.L7F04D6CC:
/* 0821FC 7F04D6CC 27A400A4 */  addiu $a0, $sp, 0xa4
/* 082200 7F04D6D0 8C980000 */  lw    $t8, ($a0)
.L7F04D6D4:
/* 082204 7F04D6D4 8FA80094 */  lw    $t0, 0x94($sp)
/* 082208 7F04D6D8 24840004 */  addiu $a0, $a0, 4
/* 08220C 7F04D6DC 00187100 */  sll   $t6, $t8, 4
/* 082210 7F04D6E0 020E1821 */  addu  $v1, $s0, $t6
/* 082214 7F04D6E4 84620000 */  lh    $v0, ($v1)
/* 082218 7F04D6E8 0048082A */  slt   $at, $v0, $t0
/* 08221C 7F04D6EC 50200004 */  beql  $at, $zero, .L7F04D700
/* 082220 7F04D6F0 8FAF0088 */   lw    $t7, 0x88($sp)
/* 082224 7F04D6F4 AFA20094 */  sw    $v0, 0x94($sp)
/* 082228 7F04D6F8 84620000 */  lh    $v0, ($v1)
/* 08222C 7F04D6FC 8FAF0088 */  lw    $t7, 0x88($sp)
.L7F04D700:
/* 082230 7F04D700 01E2082A */  slt   $at, $t7, $v0
/* 082234 7F04D704 50200003 */  beql  $at, $zero, .L7F04D714
/* 082238 7F04D708 84620002 */   lh    $v0, 2($v1)
/* 08223C 7F04D70C AFA20088 */  sw    $v0, 0x88($sp)
/* 082240 7F04D710 84620002 */  lh    $v0, 2($v1)
.L7F04D714:
/* 082244 7F04D714 8FB90098 */  lw    $t9, 0x98($sp)
/* 082248 7F04D718 0059082A */  slt   $at, $v0, $t9
/* 08224C 7F04D71C 50200004 */  beql  $at, $zero, .L7F04D730
/* 082250 7F04D720 8FA9008C */   lw    $t1, 0x8c($sp)
/* 082254 7F04D724 AFA20098 */  sw    $v0, 0x98($sp)
/* 082258 7F04D728 84620002 */  lh    $v0, 2($v1)
/* 08225C 7F04D72C 8FA9008C */  lw    $t1, 0x8c($sp)
.L7F04D730:
/* 082260 7F04D730 0122082A */  slt   $at, $t1, $v0
/* 082264 7F04D734 50200003 */  beql  $at, $zero, .L7F04D744
/* 082268 7F04D738 84620004 */   lh    $v0, 4($v1)
/* 08226C 7F04D73C AFA2008C */  sw    $v0, 0x8c($sp)
/* 082270 7F04D740 84620004 */  lh    $v0, 4($v1)
.L7F04D744:
/* 082274 7F04D744 8FAA009C */  lw    $t2, 0x9c($sp)
/* 082278 7F04D748 004A082A */  slt   $at, $v0, $t2
/* 08227C 7F04D74C 50200004 */  beql  $at, $zero, .L7F04D760
/* 082280 7F04D750 8FAC0090 */   lw    $t4, 0x90($sp)
/* 082284 7F04D754 AFA2009C */  sw    $v0, 0x9c($sp)
/* 082288 7F04D758 84620004 */  lh    $v0, 4($v1)
/* 08228C 7F04D75C 8FAC0090 */  lw    $t4, 0x90($sp)
.L7F04D760:
/* 082290 7F04D760 0182082A */  slt   $at, $t4, $v0
/* 082294 7F04D764 10200002 */  beqz  $at, .L7F04D770
/* 082298 7F04D768 00000000 */   nop
/* 08229C 7F04D76C AFA20090 */  sw    $v0, 0x90($sp)
.L7F04D770:
/* 0822A0 7F04D770 5496FFD8 */  bnel  $a0, $s6, .L7F04D6D4
/* 0822A4 7F04D774 8C980000 */   lw    $t8, ($a0)
/* 0822A8 7F04D778 02E02025 */  move  $a0, $s7
/* 0822AC 7F04D77C 03C02825 */  move  $a1, $fp
/* 0822B0 7F04D780 27A60094 */  addiu $a2, $sp, 0x94
/* 0822B4 7F04D784 0FC2DB3B */  jal   bgTestLineIntersectsBbox
/* 0822B8 7F04D788 27A70088 */   addiu $a3, $sp, 0x88
/* 0822BC 7F04D78C 10400074 */  beqz  $v0, .L7F04D960
/* 0822C0 7F04D790 3C0D8003 */   lui   $t5, %hi(D_80032088)
/* 0822C4 7F04D794 25AD2088 */  addiu $t5, %lo(D_80032088) # addiu $t5, $t5, 0x2088
/* 0822C8 7F04D798 8DA10000 */  lw    $at, ($t5)
/* 0822CC 7F04D79C 27A70070 */  addiu $a3, $sp, 0x70
/* 0822D0 7F04D7A0 27AB011C */  addiu $t3, $sp, 0x11c
/* 0822D4 7F04D7A4 ACE10000 */  sw    $at, ($a3)
/* 0822D8 7F04D7A8 8DB80004 */  lw    $t8, 4($t5)
/* 0822DC 7F04D7AC ACF80004 */  sw    $t8, 4($a3)
/* 0822E0 7F04D7B0 8DA10008 */  lw    $at, 8($t5)
/* 0822E4 7F04D7B4 ACE10008 */  sw    $at, 8($a3)
/* 0822E8 7F04D7B8 8FA900AC */  lw    $t1, 0xac($sp)
/* 0822EC 7F04D7BC 8FAF00A8 */  lw    $t7, 0xa8($sp)
/* 0822F0 7F04D7C0 8FAE00A4 */  lw    $t6, 0xa4($sp)
/* 0822F4 7F04D7C4 8FAC0154 */  lw    $t4, 0x154($sp)
/* 0822F8 7F04D7C8 00095100 */  sll   $t2, $t1, 4
/* 0822FC 7F04D7CC 000FC900 */  sll   $t9, $t7, 4
/* 082300 7F04D7D0 000E4100 */  sll   $t0, $t6, 4
/* 082304 7F04D7D4 02082021 */  addu  $a0, $s0, $t0
/* 082308 7F04D7D8 02192821 */  addu  $a1, $s0, $t9
/* 08230C 7F04D7DC 020A3021 */  addu  $a2, $s0, $t2
/* 082310 7F04D7E0 AFAB001C */  sw    $t3, 0x1c($sp)
/* 082314 7F04D7E4 AFBE0018 */  sw    $fp, 0x18($sp)
/* 082318 7F04D7E8 AFB70010 */  sw    $s7, 0x10($sp)
/* 08231C 7F04D7EC 0FC24A24 */  jal   intersectLineTriangle
/* 082320 7F04D7F0 AFAC0014 */   sw    $t4, 0x14($sp)
/* 082324 7F04D7F4 5040005B */  beql  $v0, $zero, .L7F04D964
/* 082328 7F04D7F8 26520001 */   addiu $s2, $s2, 1
/* 08232C 7F04D7FC C7A6011C */  lwc1  $f6, 0x11c($sp)
/* 082330 7F04D800 C6EA0000 */  lwc1  $f10, ($s7)
/* 082334 7F04D804 C7A40120 */  lwc1  $f4, 0x120($sp)
/* 082338 7F04D808 4600320D */  trunc.w.s $f8, $f6
/* 08233C 7F04D80C 92390000 */  lbu   $t9, ($s1)
/* 082340 7F04D810 02201025 */  move  $v0, $s1
/* 082344 7F04D814 4600540D */  trunc.w.s $f16, $f10
/* 082348 7F04D818 44184000 */  mfc1  $t8, $f8
/* 08234C 7F04D81C C6E80004 */  lwc1  $f8, 4($s7)
/* 082350 7F04D820 4600218D */  trunc.w.s $f6, $f4
/* 082354 7F04D824 44088000 */  mfc1  $t0, $f16
/* 082358 7F04D828 0271082B */  sltu  $at, $s3, $s1
/* 08235C 7F04D82C 4600428D */  trunc.w.s $f10, $f8
/* 082360 7F04D830 03087823 */  subu  $t7, $t8, $t0
/* 082364 7F04D834 448F9000 */  mtc1  $t7, $f18
/* 082368 7F04D838 44093000 */  mfc1  $t1, $f6
/* 08236C 7F04D83C 440C5000 */  mfc1  $t4, $f10
/* 082370 7F04D840 468090A0 */  cvt.s.w $f2, $f18
/* 082374 7F04D844 C7B20124 */  lwc1  $f18, 0x124($sp)
/* 082378 7F04D848 C6E60008 */  lwc1  $f6, 8($s7)
/* 08237C 7F04D84C 012C5823 */  subu  $t3, $t1, $t4
/* 082380 7F04D850 448B8000 */  mtc1  $t3, $f16
/* 082384 7F04D854 4600910D */  trunc.w.s $f4, $f18
/* 082388 7F04D858 4600320D */  trunc.w.s $f8, $f6
/* 08238C 7F04D85C 440E2000 */  mfc1  $t6, $f4
/* 082390 7F04D860 46808320 */  cvt.s.w $f12, $f16
/* 082394 7F04D864 44084000 */  mfc1  $t0, $f8
/* 082398 7F04D868 46021402 */  mul.s $f16, $f2, $f2
/* 08239C 7F04D86C 01C87823 */  subu  $t7, $t6, $t0
/* 0823A0 7F04D870 448F5000 */  mtc1  $t7, $f10
/* 0823A4 7F04D874 460C6482 */  mul.s $f18, $f12, $f12
/* 0823A8 7F04D878 12B9000A */  beq   $s5, $t9, .L7F04D8A4
/* 0823AC 7F04D87C 468053A0 */   cvt.s.w $f14, $f10
/* 0823B0 7F04D880 10200008 */  beqz  $at, .L7F04D8A4
/* 0823B4 7F04D884 00000000 */   nop
/* 0823B8 7F04D888 904AFFF8 */  lbu   $t2, -8($v0)
.L7F04D88C:
/* 0823BC 7F04D88C 2442FFF8 */  addiu $v0, $v0, -8
/* 0823C0 7F04D890 0262082B */  sltu  $at, $s3, $v0
/* 0823C4 7F04D894 12AA0003 */  beq   $s5, $t2, .L7F04D8A4
/* 0823C8 7F04D898 00000000 */   nop
/* 0823CC 7F04D89C 5420FFFB */  bnezl $at, .L7F04D88C
/* 0823D0 7F04D8A0 904AFFF8 */   lbu   $t2, -8($v0)
.L7F04D8A4:
/* 0823D4 7F04D8A4 460E7182 */  mul.s $f6, $f14, $f14
/* 0823D8 7F04D8A8 54530004 */  bnel  $v0, $s3, .L7F04D8BC
/* 0823DC 7F04D8AC 8C430004 */   lw    $v1, 4($v0)
/* 0823E0 7F04D8B0 10000006 */  b     .L7F04D8CC
/* 0823E4 7F04D8B4 2404FFFF */   li    $a0, -1
/* 0823E8 7F04D8B8 8C430004 */  lw    $v1, 4($v0)
.L7F04D8BC:
/* 0823EC 7F04D8BC 3C018000 */  lui   $at, 0x8000
/* 0823F0 7F04D8C0 2463FFF8 */  addiu $v1, $v1, -8
/* 0823F4 7F04D8C4 00614825 */  or    $t1, $v1, $at
/* 0823F8 7F04D8C8 95240000 */  lhu   $a0, ($t1)
.L7F04D8CC:
/* 0823FC 7F04D8CC 46128100 */  add.s $f4, $f16, $f18
/* 082400 7F04D8D0 46062000 */  add.s $f0, $f4, $f6
/* 082404 7F04D8D4 4614003C */  c.lt.s $f0, $f20
/* 082408 7F04D8D8 00000000 */  nop
/* 08240C 7F04D8DC 45020021 */  bc1fl .L7F04D964
/* 082410 7F04D8E0 26520001 */   addiu $s2, $s2, 1
/* 082414 7F04D8E4 C7A8011C */  lwc1  $f8, 0x11c($sp)
/* 082418 7F04D8E8 26490001 */  addiu $t1, $s2, 1
/* 08241C 7F04D8EC 46000506 */  mov.s $f20, $f0
/* 082420 7F04D8F0 E6880000 */  swc1  $f8, ($s4)
/* 082424 7F04D8F4 C7AA0120 */  lwc1  $f10, 0x120($sp)
/* 082428 7F04D8F8 E68A0004 */  swc1  $f10, 4($s4)
/* 08242C 7F04D8FC C7B00124 */  lwc1  $f16, 0x124($sp)
/* 082430 7F04D900 E6900008 */  swc1  $f16, 8($s4)
/* 082434 7F04D904 C7B20128 */  lwc1  $f18, 0x128($sp)
/* 082438 7F04D908 E692000C */  swc1  $f18, 0xc($s4)
/* 08243C 7F04D90C C7A4012C */  lwc1  $f4, 0x12c($sp)
/* 082440 7F04D910 E6840010 */  swc1  $f4, 0x10($s4)
/* 082444 7F04D914 C7A60130 */  lwc1  $f6, 0x130($sp)
/* 082448 7F04D918 E6860014 */  swc1  $f6, 0x14($s4)
/* 08244C 7F04D91C 8FAC00A4 */  lw    $t4, 0xa4($sp)
/* 082450 7F04D920 000C5900 */  sll   $t3, $t4, 4
/* 082454 7F04D924 01706821 */  addu  $t5, $t3, $s0
/* 082458 7F04D928 AE8D0018 */  sw    $t5, 0x18($s4)
/* 08245C 7F04D92C 8FB800A8 */  lw    $t8, 0xa8($sp)
/* 082460 7F04D930 240C0001 */  li    $t4, 1
/* 082464 7F04D934 00187100 */  sll   $t6, $t8, 4
/* 082468 7F04D938 01D04021 */  addu  $t0, $t6, $s0
/* 08246C 7F04D93C AE88001C */  sw    $t0, 0x1c($s4)
/* 082470 7F04D940 8FAF00AC */  lw    $t7, 0xac($sp)
/* 082474 7F04D944 A684002A */  sh    $a0, 0x2a($s4)
/* 082478 7F04D948 AE910024 */  sw    $s1, 0x24($s4)
/* 08247C 7F04D94C 000FC900 */  sll   $t9, $t7, 4
/* 082480 7F04D950 03305021 */  addu  $t2, $t9, $s0
/* 082484 7F04D954 AE8A0020 */  sw    $t2, 0x20($s4)
/* 082488 7F04D958 A6890028 */  sh    $t1, 0x28($s4)
/* 08248C 7F04D95C AFAC0110 */  sw    $t4, 0x110($sp)
.L7F04D960:
/* 082490 7F04D960 26520001 */  addiu $s2, $s2, 1
.L7F04D964:
/* 082494 7F04D964 24010004 */  li    $at, 4
/* 082498 7F04D968 1641FF18 */  bne   $s2, $at, .L7F04D5CC
/* 08249C 7F04D96C 00000000 */   nop
.L7F04D970:
/* 0824A0 7F04D970 1000FE16 */  b     .L7F04D1CC
/* 0824A4 7F04D974 26310008 */   addiu $s1, $s1, 8
.L7F04D978:
/* 0824A8 7F04D978 8FBF0054 */  lw    $ra, 0x54($sp)
/* 0824AC 7F04D97C 8FA20110 */  lw    $v0, 0x110($sp)
/* 0824B0 7F04D980 D7B40028 */  ldc1  $f20, 0x28($sp)
/* 0824B4 7F04D984 8FB00030 */  lw    $s0, 0x30($sp)
/* 0824B8 7F04D988 8FB10034 */  lw    $s1, 0x34($sp)
/* 0824BC 7F04D98C 8FB20038 */  lw    $s2, 0x38($sp)
/* 0824C0 7F04D990 8FB3003C */  lw    $s3, 0x3c($sp)
/* 0824C4 7F04D994 8FB40040 */  lw    $s4, 0x40($sp)
/* 0824C8 7F04D998 8FB50044 */  lw    $s5, 0x44($sp)
/* 0824CC 7F04D99C 8FB60048 */  lw    $s6, 0x48($sp)
/* 0824D0 7F04D9A0 8FB7004C */  lw    $s7, 0x4c($sp)
/* 0824D4 7F04D9A4 8FBE0050 */  lw    $fp, 0x50($sp)
/* 0824D8 7F04D9A8 03E00008 */  jr    $ra
/* 0824DC 7F04D9AC 27BD0150 */   addiu $sp, $sp, 0x150
)
#endif


bool sub_GAME_7F04D9B0(Model *model, ModelNode *nodearg, coord3d *arg2, coord3d *arg3, struct HitThing *hitthing, s32 *dstmtxindex, ModelNode **dstnode)
{
    coord3d spec;
    coord3d spe0;
    coord3d spd4;
    Mtxf *spd0;
    bool done;
    ModelNode *node;
    Vertex *vertices;

    spd0 = NULL;
    done = FALSE;
    node = nodearg;
    vertices = NULL;

    while (node && !done)
    {
        u32 type = node->Opcode & 0xff;
        Gfx *s3 = NULL;
        void *s5 = NULL;

        switch (type)
        {
            case MODELNODE_OPCODE_DLCOLLISION:
                {
                    ModelRoData_DisplayList_CollisionRecord *rodata = &node->Data->DisplayListCollisions;
                    ModelRwData_DisplayList_CollisionRecord *rwdata = modelGetNodeRwData(model, node);

                    if (rwdata->gdl != NULL)
                    {
                        if (rwdata->gdl == rodata->Primary)
                        {
                            s3 = (Gfx *)((uintptr_t)rodata->BaseAddr + ((u32)rodata->Primary & 0xffffff));
                        }
                        else
                        {
                            s3 = rwdata->gdl;
                        }

                        if (rodata->Secondary != NULL)
                        {
                            s5 = (void *)((uintptr_t)rodata->BaseAddr + ((u32)rodata->Secondary & 0xffffff));
                        }

                        vertices = rwdata->Vertices;
                    }
                }
                break;

            case MODELNODE_OPCODE_DL:
                {
                    ModelRoData_DisplayListRecord *rodata = &node->Data->DisplayList;

                    if (rodata->Primary != NULL)
                    {
                        s3 = (Gfx *)((uintptr_t)rodata->BaseAddr + ((u32)rodata->Primary & 0xffffff));

                        if (rodata->Secondary != NULL)
                        {
                            s5 = (Gfx *)((uintptr_t)rodata->BaseAddr + ((u32)rodata->Secondary & 0xffffff));
                        }

                        vertices = (void *)(uintptr_t)rodata->BaseAddr;
                    }
                }
                break;

            case MODELNODE_OPCODE_LOD:
                modelApplyDistanceRelations(model, node);
                break;

            case MODELNODE_OPCODE_SWITCH:
                modelApplyToggleRelations(model, node);
                break;

            case MODELNODE_OPCODE_HEAD:
                modelApplyHeadRelations(model, node);
                break;
        }

        if (s3 != NULL)
        {
            s32 mtxindex = modelFindNodeMtxIndex(node, 0);
            Mtxf *mtx = NULL;
            Mtxf sp64;

            if (mtxindex >= 0)
            {
                mtx = (Mtxf *)&model->render_pos[mtxindex]; // TODO: adjust
            }

            if (mtx && mtx != spd0)
            {
                spd0 = mtx;

                sub_GAME_7F059FB8(mtx->m, sp64.m);

                spec.x = arg2->x;
                spec.y = arg2->y;
                spec.z = arg2->z;

                mtx4TransformVecInPlace(&sp64, &spec);

                spd4.x = arg3->x;
                spd4.y = arg3->y;
                spd4.z = arg3->z;

                mtx4RotateVecInPlace(&sp64, &spd4);

                spe0.x = spd4.x * 32767.0f + spec.x;
                spe0.y = spd4.y * 32767.0f + spec.y;
                spe0.z = spd4.z * 32767.0f + spec.z;
            }

            if (bgTestHitOnObj(&spec, &spe0, &spd4, s3, s5, vertices, hitthing))
            {
                *dstmtxindex = mtxindex;
                *dstnode = node;
                done = TRUE;
            }
        }

        if (node->Child)
        {
            node = node->Child;
        } else {
            while (node)
            {
                if (node == nodearg)
                {
                    node = NULL;
                    break;
                }

                if (node->Next)
                {
                    node = node->Next;
                    break;
                }

                node = node->Parent;
            }
        }
    }

    return done;
}


void sub_GAME_7F04DCB4(ObjectRecord* obj)
{
    PropRecord* prop;
    struct ModelRoData_BoundingBoxRecord *bbox;

    prop = obj->prop;
    bbox = chrobjGetBboxFromObjectRecord(obj);
    explosionClearBulletImpactRoomByFlag(prop, FALSE);
    explosionClearBulletImpactRoomByFlag(prop, TRUE);

    sub_GAME_7F0A1DA0(&obj->runtime_pos.f[0],
        &obj->mtx.m[0][0], &obj->mtx.m[1][0], &obj->mtx.m[2][0],
        bbox->Bounds.xmin, bbox->Bounds.xmax,
        bbox->Bounds.ymin, bbox->Bounds.ymax,
        bbox->Bounds.zmin, bbox->Bounds.zmax);

    obj->runtime_bitflags |= RUNTIMEBITFLAG_REMOVE;
    obj->state |= PROPSTATE_DESTROYED;
    obj->maxdamage = 0.0f;
}


#ifdef NONMATCHING
void sub_GAME_7F04DD68(void) {

}
#else
GLOBAL_ASM(
.text
glabel sub_GAME_7F04DD68
/* 082898 7F04DD68 27BDFF80 */  addiu $sp, $sp, -0x80
/* 08289C 7F04DD6C AFBF002C */  sw    $ra, 0x2c($sp)
/* 0828A0 7F04DD70 8C8E0010 */  lw    $t6, 0x10($a0)
/* 0828A4 7F04DD74 27A50030 */  addiu $a1, $sp, 0x30
/* 0828A8 7F04DD78 AFAE007C */  sw    $t6, 0x7c($sp)
/* 0828AC 7F04DD7C 8C830014 */  lw    $v1, 0x14($a0)
/* 0828B0 7F04DD80 8C6F0008 */  lw    $t7, 8($v1)
/* 0828B4 7F04DD84 8DF80008 */  lw    $t8, 8($t7)
/* 0828B8 7F04DD88 8F190008 */  lw    $t9, 8($t8)
/* 0828BC 7F04DD8C 8F220004 */  lw    $v0, 4($t9)
/* 0828C0 7F04DD90 AFA30078 */  sw    $v1, 0x78($sp)
/* 0828C4 7F04DD94 0FC149BB */  jal   door7F0526EC
/* 0828C8 7F04DD98 AFA20074 */   sw    $v0, 0x74($sp)
/* 0828CC 7F04DD9C 8FA20074 */  lw    $v0, 0x74($sp)
/* 0828D0 7F04DDA0 27A40060 */  addiu $a0, $sp, 0x60
/* 0828D4 7F04DDA4 27A50030 */  addiu $a1, $sp, 0x30
/* 0828D8 7F04DDA8 C4440004 */  lwc1  $f4, 4($v0)
/* 0828DC 7F04DDAC 27A60040 */  addiu $a2, $sp, 0x40
/* 0828E0 7F04DDB0 27A70050 */  addiu $a3, $sp, 0x50
/* 0828E4 7F04DDB4 E7A40010 */  swc1  $f4, 0x10($sp)
/* 0828E8 7F04DDB8 C4460008 */  lwc1  $f6, 8($v0)
/* 0828EC 7F04DDBC E7A60014 */  swc1  $f6, 0x14($sp)
/* 0828F0 7F04DDC0 C448000C */  lwc1  $f8, 0xc($v0)
/* 0828F4 7F04DDC4 E7A80018 */  swc1  $f8, 0x18($sp)
/* 0828F8 7F04DDC8 C44A0010 */  lwc1  $f10, 0x10($v0)
/* 0828FC 7F04DDCC E7AA001C */  swc1  $f10, 0x1c($sp)
/* 082900 7F04DDD0 C4500014 */  lwc1  $f16, 0x14($v0)
/* 082904 7F04DDD4 E7B00020 */  swc1  $f16, 0x20($sp)
/* 082908 7F04DDD8 C4520018 */  lwc1  $f18, 0x18($v0)
/* 08290C 7F04DDDC 0FC28768 */  jal   sub_GAME_7F0A1DA0
/* 082910 7F04DDE0 E7B20024 */   swc1  $f18, 0x24($sp)
/* 082914 7F04DDE4 8FA4007C */  lw    $a0, 0x7c($sp)
/* 082918 7F04DDE8 0FC28333 */  jal   explosionClearBulletImpactRoomByFlag
/* 08291C 7F04DDEC 24050001 */   li    $a1, 1
/* 082920 7F04DDF0 8FA40078 */  lw    $a0, 0x78($sp)
/* 082924 7F04DDF4 8C880008 */  lw    $t0, 8($a0)
/* 082928 7F04DDF8 8D090008 */  lw    $t1, 8($t0)
/* 08292C 7F04DDFC 0FC1B1E7 */  jal   modelGetNodeRwData
/* 082930 7F04DE00 8D250004 */   lw    $a1, 4($t1)
/* 082934 7F04DE04 AC400000 */  sw    $zero, ($v0)
/* 082938 7F04DE08 8FBF002C */  lw    $ra, 0x2c($sp)
/* 08293C 7F04DE0C 27BD0080 */  addiu $sp, $sp, 0x80
/* 082940 7F04DE10 03E00008 */  jr    $ra
/* 082944 7F04DE14 00000000 */   nop
)
#endif





#ifdef NONMATCHING

#else
GLOBAL_ASM(

.text
glabel sub_GAME_7F04DE18
/* 082948 7F04DE18 27BDFF68 */  addiu $sp, $sp, -0x98
/* 08294C 7F04DE1C AFBF0034 */  sw    $ra, 0x34($sp)
/* 082950 7F04DE20 AFB00030 */  sw    $s0, 0x30($sp)
/* 082954 7F04DE24 8C8E0010 */  lw    $t6, 0x10($a0)
/* 082958 7F04DE28 AFAE0094 */  sw    $t6, 0x94($sp)
/* 08295C 7F04DE2C 91D80001 */  lbu   $t8, 1($t6)
/* 082960 7F04DE30 8C900014 */  lw    $s0, 0x14($a0)
/* 082964 7F04DE34 33190002 */  andi  $t9, $t8, 2
/* 082968 7F04DE38 53200023 */  beql  $t9, $zero, .L7F04DEC8
/* 08296C 7F04DE3C 8FA40094 */   lw    $a0, 0x94($sp)
/* 082970 7F04DE40 8E080008 */  lw    $t0, 8($s0)
/* 082974 7F04DE44 02002025 */  move  $a0, $s0
/* 082978 7F04DE48 00003025 */  move  $a2, $zero
/* 08297C 7F04DE4C 8D020008 */  lw    $v0, 8($t0)
/* 082980 7F04DE50 8C490008 */  lw    $t1, 8($v0)
/* 082984 7F04DE54 8C450004 */  lw    $a1, 4($v0)
/* 082988 7F04DE58 8D230004 */  lw    $v1, 4($t1)
/* 08298C 7F04DE5C 0FC1B198 */  jal   modelFindNodeMtx
/* 082990 7F04DE60 AFA30088 */   sw    $v1, 0x88($sp)
/* 082994 7F04DE64 0FC1E111 */  jal   currentPlayerGetMatrix10D4
/* 082998 7F04DE68 AFA20084 */   sw    $v0, 0x84($sp)
/* 08299C 7F04DE6C 00402025 */  move  $a0, $v0
/* 0829A0 7F04DE70 8FA50084 */  lw    $a1, 0x84($sp)
/* 0829A4 7F04DE74 0FC16063 */  jal   matrix_4x4_multiply_homogeneous
/* 0829A8 7F04DE78 27A60044 */   addiu $a2, $sp, 0x44
/* 0829AC 7F04DE7C 8FA30088 */  lw    $v1, 0x88($sp)
/* 0829B0 7F04DE80 27A40074 */  addiu $a0, $sp, 0x74
/* 0829B4 7F04DE84 27A50044 */  addiu $a1, $sp, 0x44
/* 0829B8 7F04DE88 C4640004 */  lwc1  $f4, 4($v1)
/* 0829BC 7F04DE8C 27A60054 */  addiu $a2, $sp, 0x54
/* 0829C0 7F04DE90 27A70064 */  addiu $a3, $sp, 0x64
/* 0829C4 7F04DE94 E7A40010 */  swc1  $f4, 0x10($sp)
/* 0829C8 7F04DE98 C4660008 */  lwc1  $f6, 8($v1)
/* 0829CC 7F04DE9C E7A60014 */  swc1  $f6, 0x14($sp)
/* 0829D0 7F04DEA0 C468000C */  lwc1  $f8, 0xc($v1)
/* 0829D4 7F04DEA4 E7A80018 */  swc1  $f8, 0x18($sp)
/* 0829D8 7F04DEA8 C46A0010 */  lwc1  $f10, 0x10($v1)
/* 0829DC 7F04DEAC E7AA001C */  swc1  $f10, 0x1c($sp)
/* 0829E0 7F04DEB0 C4700014 */  lwc1  $f16, 0x14($v1)
/* 0829E4 7F04DEB4 E7B00020 */  swc1  $f16, 0x20($sp)
/* 0829E8 7F04DEB8 C4720018 */  lwc1  $f18, 0x18($v1)
/* 0829EC 7F04DEBC 0FC28768 */  jal   sub_GAME_7F0A1DA0
/* 0829F0 7F04DEC0 E7B20024 */   swc1  $f18, 0x24($sp)
/* 0829F4 7F04DEC4 8FA40094 */  lw    $a0, 0x94($sp)
.L7F04DEC8:
/* 0829F8 7F04DEC8 0FC28333 */  jal   explosionClearBulletImpactRoomByFlag
/* 0829FC 7F04DECC 24050001 */   li    $a1, 1
/* 082A00 7F04DED0 8E0A0008 */  lw    $t2, 8($s0)
/* 082A04 7F04DED4 02002025 */  move  $a0, $s0
/* 082A08 7F04DED8 8D4B0008 */  lw    $t3, 8($t2)
/* 082A0C 7F04DEDC 0FC1B1E7 */  jal   modelGetNodeRwData
/* 082A10 7F04DEE0 8D65000C */   lw    $a1, 0xc($t3)
/* 082A14 7F04DEE4 AC400000 */  sw    $zero, ($v0)
/* 082A18 7F04DEE8 8FBF0034 */  lw    $ra, 0x34($sp)
/* 082A1C 7F04DEEC 8FB00030 */  lw    $s0, 0x30($sp)
/* 082A20 7F04DEF0 27BD0098 */  addiu $sp, $sp, 0x98
/* 082A24 7F04DEF4 03E00008 */  jr    $ra
/* 082A28 7F04DEF8 00000000 */   nop
)
#endif





/**
 * Address 0x7F04DEFC.
*/
void maybe_detonate_object_and_its_children(PropRecord *prop, f32 damage, struct coord3d *pos, s32 arg3, s32 owner)
{
    PropRecord *node;
    ObjectRecord *prop_obj;

    prop_obj = prop->obj;

    prop_obj->runtime_bitflags &= ~(RUNTIMEBITFLAG_OWNER);
    prop_obj->runtime_bitflags |= (owner << RUNTIMEBITSHIFT_OWNER);

    if ((s32)(prop_obj->runtime_bitflags << 0xc) >= 0)
    {
        node = prop->child;
        while (node != NULL)
        {
            PropRecord *iter_next = node->prev;
            // recursive call:
            maybe_detonate_object_and_its_children(node, damage, pos, arg3, owner);
            node = iter_next;
        }

        maybe_detonate_object(prop->obj, damage, pos, arg3, owner);
    }
}





bool check_if_destroyable_object_type(PropDefHeaderRecord *obj)//#MATCH
{
    switch (obj->type)
    {
        case PROPDEF_DOOR:
        case PROPDEF_PROP:
        case PROPDEF_ALARM:
        case PROPDEF_CCTV:
        case PROPDEF_MONITOR:
        case PROPDEF_MULTI_MONITOR:
        case PROPDEF_RACK:
        case PROPDEF_AUTOGUN:
        case PROPDEF_GAS_RELEASING:
        case PROPDEF_VEHICHLE:
        case PROPDEF_AIRCRAFT:
        case PROPDEF_UNK41:
        case PROPDEF_GLASS:
        case PROPDEF_SAFE:
        case PROPDEF_TANK:
        case PROPDEF_TINTED_GLASS:
            return TRUE;
        default:
            return FALSE;
    }
}


bool objIsCollectable(PropDefHeaderRecord *obj)
{
    switch (obj->type)
    {
        case PROPDEF_KEY:
        case PROPDEF_MAGAZINE:
        case PROPDEF_COLLECTABLE:
        case PROPDEF_HAT:
        case PROPDEF_AMMO:
        case PROPDEF_ARMOUR:
            return TRUE;
        default:
            return FALSE;
    }
}


bool objIsMortal(ObjectRecord* obj)
{
    if (obj->type == PROPDEF_DOOR)
    {
        return FALSE;
    }
    if ((objIsCollectable((PropDefHeaderRecord* ) obj) != 0) && (obj->type != PROPDEF_ARMOUR))
    {
        if (!(obj->flags & PROPFLAG_FORCEMORTAL))
        {
            return FALSE;
        }
    }
    else if (obj->flags & PROPFLAG_INVINCIBLE)
    {
        return FALSE;
    }
    return TRUE;
}


/**
 * Address 0x7F04E0CC.
*/
void chrobjMaybeDetonateObjectIfFlags(ObjectRecord *obj, f32 damage, coord3d *pos, ITEM_IDS item, s32 owner)
{
    if ((obj->flags2 & 0x4000) == 0)
    {
        maybe_detonate_object(obj, damage, pos, item, owner);
    }
}




#ifdef NONMATCHING
//https://decomp.me/scratch/UWQzo 44%
ObjectRecord blank_07_object = {
    0x0100, //extrascale
    0x0, //state
    0x07, //type
    0, //obj
    0xFFFF, //pad
    0x00000001, //flags
    0, //flags2
    NULL, //prop
    NULL, //model
    {
       1.0f, 0.0f, 0.0f, 0.0f,
       0.0f, 1.0f, 0.0f, 0.0f,
       0.0f, 0.0f, 1.0f, 0.0f,
       0.0f, 0.0f, 0.0f, 1.0f
    }, //mtx
    {0.0, 0.0, 0.0},//runtime_pos
    {0x00000000}, //runtime_bitflags
    NULL, //ptr_allocated_collisiondata_block
    NULL, //projectile/embedment
    0.0f, //maxdamage
    1000.0f, //damage
    {0xFF, 0xFF, 0xFF, 0x00}, //shadecol
    {0xFF, 0xFF, 0xFF, 0x00}, //nextcol
};
void maybe_detonate_object(ObjectRecord* self, f32 damage,  coord3d* pos, bool flag, bool flag2)
{
    u32 spC8;
    s32 randAmmoType;//c4
    int randAmmoType_2;

    s32 sp3C;
     PropRecord* sp28;
    u16 ammoAmmount;//24
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f12;
    s32 temp_a1;

    s32 temp_t7;
    s32 type;
    s32* temp_t3;
    s32* temp_t5;
    s32* temp_t9;
    s8 weaponnum;
     PropRecord* temp_a0_2;
    u16 temp_a3;
    u8 temp_v0;
    u8 temp_v0_4;
    u8 temp_v0_6;
    void* temp_a0;
    void* temp_a2;
    void* temp_t6;
    void* temp_v0_5;

    f32 phi_f12;
     PropRecord* phi_f12_2;
    s32* phi_t9;
    s32* phi_t3;
    s32* phi_t5;
    void* phi_t6;
    f32 phi_f0;
    struct {
        s32 rhgun:1;
        s32 noammo:1;
        s32 inmotion:1;
        s32 lhgun:1;
        s32 nocontrol:1;
        s32 unk80000:1;
        s32 jbush:1;
        s32 petegrenade:1;
        s32 embedded:1;
        s32 normal:1;
        s32 unk8000:1;
        s32 immobile:1;
        s32 dat:1;
        s32 uncollectable:1;
        s32 unk800:1;
        s32 allowpickup:1;
        s32 invincible:1;
        s32 embeddedin:1;
        s32 useguard:1;
        s32 forcecollection:1;
        s32 posabs:1;
        s32 freestand:1;
        s32 roadblock:1;
        s32 glassimage:1;
        s32 forcecollisions:1;
        s32 onground:1;
        s32 forcedonground:1;
        s32 rotate90:1;
        s32 upside_down:1;
        s32 rightside_up:1;
        s32 scalex:1;
        s32 scaley:1;
    } *flags = &self->flags;

    #define OBJECT_UNTOUCHED 1
    #define OBJECT_DESTROYED 0


    temp_t7 = self->runtime_bitflags & ~(RUNTIMEBITFLAG_OWNER);
    self->runtime_bitflags = temp_t7;
    self->runtime_bitflags = temp_t7 | (flag2 << RUNTIMEBITSHIFT_OWNER);


    if ((self->Head.type != PROPDEF_GAS_RELEASING) ||  (objGetDestroyedLevel(self) != OBJECT_UNTOUCHED))
    {
        if (!flag)
        {
            if (objIsCollectable(self))
            {
                if (self->flags << 8 >= 0)
                {
                    return;
                }
            }
            else if (flags->petegrenade) //looks like  bitfield (sll 7 + bgezl)
            {
                return;
            }
        }
        else
        {

            if (flags->unk800)
            {
                return;
            }

            if (self->Head.type == PROPDEF_COLLECTABLE || PROPDEF_MAGAZINE)
            //switch(self->Head.type)
            {
                if (self->Head.type == PROPDEF_COLLECTABLE  )
                {
                    weaponnum = ((WeaponObjRecord*)self)->weaponnum;
                    if ((weaponnum == ITEM_GRENADE) ||
                        (weaponnum == ITEM_TIMEDMINE) ||
                        (weaponnum == ITEM_REMOTEMINE) ||
                        (weaponnum == ITEM_PROXIMITYMINE) ||
                        (weaponnum == ITEM_56) ||
                        (weaponnum == ITEM_57) ||
                        (weaponnum == ITEM_BOMBCASE) ||
                        (weaponnum == ITEM_PLASTIQUE))
                    {
                        ((WeaponObjRecord*)self)->timer = 0;
                    }
                    return;
                }
                else if (self->Head.type == PROPDEF_MAGAZINE)
                {
                    type = ((AmmoCrateRecord*)self)->type;
                    if ((type == AMMO_GRENADE) ||
                        (type == AMMO_ROCKETS) ||
                        (type == AMMO_REMOTEMINE) ||
                        (type == AMMO_PROXMINE) ||
                        (type == AMMO_TIMEDMINE) ||
                        (type == AMMO_GRENADEROUND) ||
                        (type == AMMO_EXPLOSIVEPEN) ||
                        (type == AMMO_BOMBCASE) ||
                        (type == AMMO_DYNAMITE))
                    {
                        self->flags |= 0x10000000;
                    }
                    return;
                }
            }
            if (!objIsMortal(damage, self, 8) )
            {
                return;
            }

        } // if flag

        if (objGetDestroyedLevel(self) == OBJECT_DESTROYED)
        {
            self->maxdamage += damage * 250.0f;
        }
        else
        {
            temp_f0 = 4 - (objGetShotsTaken(damage, self) % 4);
            phi_f0 = temp_f0;
            if (temp_f0 < damage)
            {
                damage = phi_f0;
            }
            phi_f0 = 1.0f;
            if (damage < 1.0f)
            {
                damage = phi_f0;
            }
            self->maxdamage += damage;
            //phi_f12_2 = (bitwise struct PropRecord* ) damage;
        }

        if (( self->Head.type == PROPDEF_GLASS) || ( self->Head.type == PROPDEF_TINTED_GLASS))
        {
            if (self->damage <= self->maxdamage)
            {
                sub_GAME_7F04DCB4(phi_f12_2, self); //smash glass?
            }
        }
        else
        {
            propobjSetDropped(self->prop, 1);
            object_explosion_related(self, pos, flag2);
        }
        if ((self->Head.type == PROPDEF_AMMO) && (objGetDestroyedLevel(self) == OBJECT_UNTOUCHED))
        {
            randAmmoType = randomGetNext() % 0xD;
            do
            {
                MultiAmmoCrateRecord *ammo = self;
                ammoAmmount = ammo->quantities[randAmmoType];

                if (ammoAmmount > 0 && (ammoAmmount != -1) )
                {
                    temp_v0_5 = ammocrateAllocate(randAmmoType, randAmmoType);
                    if (temp_v0_5)
                    {
                        //spawn magazine
                        AmmoCrateRecord NewMag = blank_07_object; //New_AmmoCrateRecord()
                        AmmoCrateRecord temp_a2 = NewMag;

                        //temp_v0_5->obj = ammo->unk80;
                        //temp_v0_5->unk80 = randAmmoType + 1;
                        if (randAmmoType + 1 == 2)
                        {
                            //temp_v0_5->unk80 = 1;
                        }
                        if (objInitWithModelDef(&temp_a2, *(PitemZ_entries + (temp_a3 * 0xC)), &temp_a2, temp_a3) != 0)
                        {
                            //temp_a0 = temp_a2->unk14;

                            modelSetScale(temp_a0, 1);//tempa2
                            chrpropReparent(temp_a2.base.prop, self->prop);
                        }
                        break;
                    }
                }
                else
                {
                    randAmmoType_2 = (randAmmoType + 1) % 0xD;
                    randAmmoType = randAmmoType_2;
                    randAmmoType = randAmmoType;

                }
            }while (randAmmoType_2 != randAmmoType);
        }


        switch (self->Head.type)
        {
            case 13:
            {
                self->flags |= 0x40000000;
                if (objGetDestroyedLevel(self) == OBJECT_UNTOUCHED)
                {
                    self->flags |= 0x10000000;
                }
            }
            case 6:
            {
                if (objGetDestroyedLevel(self) == OBJECT_UNTOUCHED)
                {
                    self->flags |= 0x10000000;
                }
            }
            case 10:
            {
                if (objGetDestroyedLevel(self) == OBJECT_UNTOUCHED)
                {
                    //save_ptr_monitor_ani_code_to_obj_ani_slot(self + 0x80, &D_80031EE8);
                }
            }
            case 11:
            {
                if (objGetDestroyedLevel(self) == OBJECT_UNTOUCHED)
                {
                    //save_ptr_monitor_ani_code_to_obj_ani_slot(self + 0x80, &D_80031EE8);
                    // save_ptr_monitor_ani_code_to_obj_ani_slot(self + 0xF4, &D_80031EE8);
                    // save_ptr_monitor_ani_code_to_obj_ani_slot(self + 0x168, &D_80031EE8);
                    // save_ptr_monitor_ani_code_to_obj_ani_slot(self + 0x1DC, &D_80031EE8);
                }
            }
            case 36:
            {
                if (objGetDestroyedLevel(self) == OBJECT_UNTOUCHED)
                {
                    //init_trigger_toxic_gas_effect(&self->Pos);
                }
            }
            case 21:
            {
                if (objGetDestroyedLevel(self) == OBJECT_DESTROYED)
                {
                    temp_f0_2 = self->damage;
                    //self->unk84 = ((bitwise f32) self->unk80 * (temp_f0_2 - self->maxdamage)) / temp_f0_2;
                }
                else
                {
                    //self->unk84 = 0.0f;
                }
            }
        }
        if (objGetDestroyedLevel(self) == OBJECT_UNTOUCHED)
        {
            PropRecord *temp_a0_2 = self->prop->child;

            if (temp_a0_2 != 0)
            {
                do
                {
                    temp_a0_2 = temp_a0_2->prev;
                    propobjSetDropped(temp_a0_2, 1);

                } while (temp_a0_2 != 0);
            }
        }
    } // if PROPDEF_GAS_RELEASING or object not harmed

}

#else
ObjectRecord blank_07_object = {
    0x0100, //extrascale
    0x0, //state
    0x07, //type
    0, //obj
    0xFFFF, //pad
    0x00000001, //flags
    0, //flags2
    NULL, //prop
    NULL, //model
    {
       1.0f, 0.0f, 0.0f, 0.0f,
       0.0f, 1.0f, 0.0f, 0.0f,
       0.0f, 0.0f, 1.0f, 0.0f,
       0.0f, 0.0f, 0.0f, 1.0f
    }, //mtx
    {0.0, 0.0, 0.0},//runtime_pos
    {0x00000000}, //runtime_bitflags
    NULL, //ptr_allocated_collisiondata_block
    NULL, //projectile/embedment
    0.0f, //maxdamage
    1000.0f, //damage
    {0xFF, 0xFF, 0xFF, 0x00}, //shadecol
    {0xFF, 0xFF, 0xFF, 0x00}, //nextcol
};
GLOBAL_ASM(
.text
glabel maybe_detonate_object
/* 082C38 7F04E108 27BDFF20 */  addiu $sp, $sp, -0xe0
/* 082C3C 7F04E10C AFBF001C */  sw    $ra, 0x1c($sp)
/* 082C40 7F04E110 AFB00018 */  sw    $s0, 0x18($sp)
/* 082C44 7F04E114 AFA600E8 */  sw    $a2, 0xe8($sp)
/* 082C48 7F04E118 AFA700EC */  sw    $a3, 0xec($sp)
/* 082C4C 7F04E11C 8C8E0064 */  lw    $t6, 0x64($a0)
/* 082C50 7F04E120 3C01FFF9 */  lui   $at, (0xFFF9FFFF >> 16) # lui $at, 0xfff9
/* 082C54 7F04E124 3421FFFF */  ori   $at, (0xFFF9FFFF & 0xFFFF) # ori $at, $at, 0xffff
/* 082C58 7F04E128 01C17824 */  and   $t7, $t6, $at
/* 082C5C 7F04E12C AC8F0064 */  sw    $t7, 0x64($a0)
/* 082C60 7F04E130 8FB900F0 */  lw    $t9, 0xf0($sp)
/* 082C64 7F04E134 908B0003 */  lbu   $t3, 3($a0)
/* 082C68 7F04E138 44856000 */  mtc1  $a1, $f12
/* 082C6C 7F04E13C 00194C40 */  sll   $t1, $t9, 0x11
/* 082C70 7F04E140 01E95025 */  or    $t2, $t7, $t1
/* 082C74 7F04E144 24010024 */  li    $at, 36
/* 082C78 7F04E148 00808025 */  move  $s0, $a0
/* 082C7C 7F04E14C 15610006 */  bne   $t3, $at, .L7F04E168
/* 082C80 7F04E150 AC8A0064 */   sw    $t2, 0x64($a0)
/* 082C84 7F04E154 0FC0FFF0 */  jal   objGetDestroyedLevel
/* 082C88 7F04E158 E7AC00E4 */   swc1  $f12, 0xe4($sp)
/* 082C8C 7F04E15C 24010001 */  li    $at, 1
/* 082C90 7F04E160 1041016A */  beq   $v0, $at, .L7F04E70C
/* 082C94 7F04E164 C7AC00E4 */   lwc1  $f12, 0xe4($sp)
.L7F04E168:
/* 082C98 7F04E168 8FAC00EC */  lw    $t4, 0xec($sp)
/* 082C9C 7F04E16C 02002025 */  move  $a0, $s0
/* 082CA0 7F04E170 55800012 */  bnezl $t4, .L7F04E1BC
/* 082CA4 7F04E174 8E030008 */   lw    $v1, 8($s0)
/* 082CA8 7F04E178 0FC13803 */  jal   objIsCollectable
/* 082CAC 7F04E17C E7AC00E4 */   swc1  $f12, 0xe4($sp)
/* 082CB0 7F04E180 10400007 */  beqz  $v0, .L7F04E1A0
/* 082CB4 7F04E184 C7AC00E4 */   lwc1  $f12, 0xe4($sp)
/* 082CB8 7F04E188 8E0D0008 */  lw    $t5, 8($s0)
/* 082CBC 7F04E18C 000D7200 */  sll   $t6, $t5, 8
/* 082CC0 7F04E190 05C20045 */  bltzl $t6, .L7F04E2A8
/* 082CC4 7F04E194 02002025 */   move  $a0, $s0
/* 082CC8 7F04E198 1000015D */  b     .L7F04E710
/* 082CCC 7F04E19C 8FBF001C */   lw    $ra, 0x1c($sp)
.L7F04E1A0:
/* 082CD0 7F04E1A0 8E0F0008 */  lw    $t7, 8($s0)
/* 082CD4 7F04E1A4 000FC9C0 */  sll   $t9, $t7, 7
/* 082CD8 7F04E1A8 0723003F */  bgezl $t9, .L7F04E2A8
/* 082CDC 7F04E1AC 02002025 */   move  $a0, $s0
/* 082CE0 7F04E1B0 10000157 */  b     .L7F04E710
/* 082CE4 7F04E1B4 8FBF001C */   lw    $ra, 0x1c($sp)
/* 082CE8 7F04E1B8 8E030008 */  lw    $v1, 8($s0)
.L7F04E1BC:
/* 082CEC 7F04E1BC 0003C380 */  sll   $t8, $v1, 0xe
/* 082CF0 7F04E1C0 07020153 */  bltzl $t8, .L7F04E710
/* 082CF4 7F04E1C4 8FBF001C */   lw    $ra, 0x1c($sp)
/* 082CF8 7F04E1C8 92020003 */  lbu   $v0, 3($s0)
/* 082CFC 7F04E1CC 24050008 */  li    $a1, 8
/* 082D00 7F04E1D0 24040007 */  li    $a0, 7
/* 082D04 7F04E1D4 14A20015 */  bne   $a1, $v0, .L7F04E22C
/* 082D08 7F04E1D8 00000000 */   nop
/* 082D0C 7F04E1DC 82020080 */  lb    $v0, 0x80($s0)
/* 082D10 7F04E1E0 2401001A */  li    $at, 26
/* 082D14 7F04E1E4 1041000F */  beq   $v0, $at, .L7F04E224
/* 082D18 7F04E1E8 2401001B */   li    $at, 27
/* 082D1C 7F04E1EC 1041000D */  beq   $v0, $at, .L7F04E224
/* 082D20 7F04E1F0 2401001D */   li    $at, 29
/* 082D24 7F04E1F4 1041000B */  beq   $v0, $at, .L7F04E224
/* 082D28 7F04E1F8 2401001C */   li    $at, 28
/* 082D2C 7F04E1FC 10410009 */  beq   $v0, $at, .L7F04E224
/* 082D30 7F04E200 24010056 */   li    $at, 86
/* 082D34 7F04E204 10410007 */  beq   $v0, $at, .L7F04E224
/* 082D38 7F04E208 24010057 */   li    $at, 87
/* 082D3C 7F04E20C 10410005 */  beq   $v0, $at, .L7F04E224
/* 082D40 7F04E210 24010021 */   li    $at, 33
/* 082D44 7F04E214 10410003 */  beq   $v0, $at, .L7F04E224
/* 082D48 7F04E218 24010022 */   li    $at, 34
/* 082D4C 7F04E21C 5441013C */  bnel  $v0, $at, .L7F04E710
/* 082D50 7F04E220 8FBF001C */   lw    $ra, 0x1c($sp)
.L7F04E224:
/* 082D54 7F04E224 10000139 */  b     .L7F04E70C
/* 082D58 7F04E228 A6000082 */   sh    $zero, 0x82($s0)
.L7F04E22C:
/* 082D5C 7F04E22C 54820019 */  bnel  $a0, $v0, .L7F04E294
/* 082D60 7F04E230 02002025 */   move  $a0, $s0
/* 082D64 7F04E234 8E020080 */  lw    $v0, 0x80($s0)
/* 082D68 7F04E238 24010005 */  li    $at, 5
/* 082D6C 7F04E23C 10410010 */  beq   $v0, $at, .L7F04E280
/* 082D70 7F04E240 24010006 */   li    $at, 6
/* 082D74 7F04E244 5041000F */  beql  $v0, $at, .L7F04E284
/* 082D78 7F04E248 3C011000 */   lui   $at, 0x1000
/* 082D7C 7F04E24C 5082000D */  beql  $a0, $v0, .L7F04E284
/* 082D80 7F04E250 3C011000 */   lui   $at, 0x1000
/* 082D84 7F04E254 10A2000A */  beq   $a1, $v0, .L7F04E280
/* 082D88 7F04E258 24010009 */   li    $at, 9
/* 082D8C 7F04E25C 10410008 */  beq   $v0, $at, .L7F04E280
/* 082D90 7F04E260 2401000B */   li    $at, 11
/* 082D94 7F04E264 10410006 */  beq   $v0, $at, .L7F04E280
/* 082D98 7F04E268 2401000F */   li    $at, 15
/* 082D9C 7F04E26C 10410004 */  beq   $v0, $at, .L7F04E280
/* 082DA0 7F04E270 24010010 */   li    $at, 16
/* 082DA4 7F04E274 10410002 */  beq   $v0, $at, .L7F04E280
/* 082DA8 7F04E278 24010013 */   li    $at, 19
/* 082DAC 7F04E27C 14410123 */  bne   $v0, $at, .L7F04E70C
.L7F04E280:
/* 082DB0 7F04E280 3C011000 */   li    $at, 0x10000000 # 0.000000
.L7F04E284:
/* 082DB4 7F04E284 00614825 */  or    $t1, $v1, $at
/* 082DB8 7F04E288 10000120 */  b     .L7F04E70C
/* 082DBC 7F04E28C AE090008 */   sw    $t1, 8($s0)
/* 082DC0 7F04E290 02002025 */  move  $a0, $s0
.L7F04E294:
/* 082DC4 7F04E294 0FC13812 */  jal   objIsMortal
/* 082DC8 7F04E298 E7AC00E4 */   swc1  $f12, 0xe4($sp)
/* 082DCC 7F04E29C 1040011B */  beqz  $v0, .L7F04E70C
/* 082DD0 7F04E2A0 C7AC00E4 */   lwc1  $f12, 0xe4($sp)
/* 082DD4 7F04E2A4 02002025 */  move  $a0, $s0
.L7F04E2A8:
/* 082DD8 7F04E2A8 0FC0FFF0 */  jal   objGetDestroyedLevel
/* 082DDC 7F04E2AC E7AC00E4 */   swc1  $f12, 0xe4($sp)
/* 082DE0 7F04E2B0 14400008 */  bnez  $v0, .L7F04E2D4
/* 082DE4 7F04E2B4 C7AC00E4 */   lwc1  $f12, 0xe4($sp)
/* 082DE8 7F04E2B8 3C01437A */  li    $at, 0x437A0000 # 250.000000
/* 082DEC 7F04E2BC 44813000 */  mtc1  $at, $f6
/* 082DF0 7F04E2C0 C6040070 */  lwc1  $f4, 0x70($s0)
/* 082DF4 7F04E2C4 46066202 */  mul.s $f8, $f12, $f6
/* 082DF8 7F04E2C8 46082280 */  add.s $f10, $f4, $f8
/* 082DFC 7F04E2CC 1000001F */  b     .L7F04E34C
/* 082E00 7F04E2D0 E60A0070 */   swc1  $f10, 0x70($s0)
.L7F04E2D4:
/* 082E04 7F04E2D4 02002025 */  move  $a0, $s0
/* 082E08 7F04E2D8 0FC0FFD8 */  jal   objGetShotsTaken
/* 082E0C 7F04E2DC E7AC00E4 */   swc1  $f12, 0xe4($sp)
/* 082E10 7F04E2E0 240B0004 */  li    $t3, 4
/* 082E14 7F04E2E4 C7AC00E4 */  lwc1  $f12, 0xe4($sp)
/* 082E18 7F04E2E8 04410004 */  bgez  $v0, .L7F04E2FC
/* 082E1C 7F04E2EC 304A0003 */   andi  $t2, $v0, 3
/* 082E20 7F04E2F0 11400002 */  beqz  $t2, .L7F04E2FC
/* 082E24 7F04E2F4 00000000 */   nop
/* 082E28 7F04E2F8 254AFFFC */  addiu $t2, $t2, -4
.L7F04E2FC:
/* 082E2C 7F04E2FC 016A6023 */  subu  $t4, $t3, $t2
/* 082E30 7F04E300 448C8000 */  mtc1  $t4, $f16
/* 082E34 7F04E304 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 082E38 7F04E308 46808020 */  cvt.s.w $f0, $f16
/* 082E3C 7F04E30C 460C003C */  c.lt.s $f0, $f12
/* 082E40 7F04E310 00000000 */  nop
/* 082E44 7F04E314 45020004 */  bc1fl .L7F04E328
/* 082E48 7F04E318 44810000 */   mtc1  $at, $f0
/* 082E4C 7F04E31C 10000008 */  b     .L7F04E340
/* 082E50 7F04E320 46000306 */   mov.s $f12, $f0
/* 082E54 7F04E324 44810000 */  mtc1  $at, $f0
.L7F04E328:
/* 082E58 7F04E328 00000000 */  nop
/* 082E5C 7F04E32C 4600603C */  c.lt.s $f12, $f0
/* 082E60 7F04E330 00000000 */  nop
/* 082E64 7F04E334 45020003 */  bc1fl .L7F04E344
/* 082E68 7F04E338 C6120070 */   lwc1  $f18, 0x70($s0)
/* 082E6C 7F04E33C 46000306 */  mov.s $f12, $f0
.L7F04E340:
/* 082E70 7F04E340 C6120070 */  lwc1  $f18, 0x70($s0)
.L7F04E344:
/* 082E74 7F04E344 460C9180 */  add.s $f6, $f18, $f12
/* 082E78 7F04E348 E6060070 */  swc1  $f6, 0x70($s0)
.L7F04E34C:
/* 082E7C 7F04E34C 92020003 */  lbu   $v0, 3($s0)
/* 082E80 7F04E350 2401002A */  li    $at, 42
/* 082E84 7F04E354 10410003 */  beq   $v0, $at, .L7F04E364
/* 082E88 7F04E358 2401002F */   li    $at, 47
/* 082E8C 7F04E35C 1441000B */  bne   $v0, $at, .L7F04E38C
/* 082E90 7F04E360 24050001 */   li    $a1, 1
.L7F04E364:
/* 082E94 7F04E364 C6040070 */  lwc1  $f4, 0x70($s0)
/* 082E98 7F04E368 C6080074 */  lwc1  $f8, 0x74($s0)
/* 082E9C 7F04E36C 4604403E */  c.le.s $f8, $f4
/* 082EA0 7F04E370 00000000 */  nop
/* 082EA4 7F04E374 4502000D */  bc1fl .L7F04E3AC
/* 082EA8 7F04E378 24010014 */   li    $at, 20
/* 082EAC 7F04E37C 0FC1372D */  jal   sub_GAME_7F04DCB4
/* 082EB0 7F04E380 02002025 */   move  $a0, $s0
/* 082EB4 7F04E384 10000008 */  b     .L7F04E3A8
/* 082EB8 7F04E388 92020003 */   lbu   $v0, 3($s0)
.L7F04E38C:
/* 082EBC 7F04E38C 0FC12FF4 */  jal   propobjSetDropped
/* 082EC0 7F04E390 8E040010 */   lw    $a0, 0x10($s0)
/* 082EC4 7F04E394 02002025 */  move  $a0, $s0
/* 082EC8 7F04E398 8FA500E8 */  lw    $a1, 0xe8($sp)
/* 082ECC 7F04E39C 0FC13383 */  jal   object_explosion_related
/* 082ED0 7F04E3A0 8FA600F0 */   lw    $a2, 0xf0($sp)
/* 082ED4 7F04E3A4 92020003 */  lbu   $v0, 3($s0)
.L7F04E3A8:
/* 082ED8 7F04E3A8 24010014 */  li    $at, 20
.L7F04E3AC:
/* 082EDC 7F04E3AC 54410065 */  bnel  $v0, $at, .L7F04E544
/* 082EE0 7F04E3B0 92020003 */   lbu   $v0, 3($s0)
/* 082EE4 7F04E3B4 0FC0FFF0 */  jal   objGetDestroyedLevel
/* 082EE8 7F04E3B8 02002025 */   move  $a0, $s0
/* 082EEC 7F04E3BC 24010001 */  li    $at, 1
/* 082EF0 7F04E3C0 54410060 */  bnel  $v0, $at, .L7F04E544
/* 082EF4 7F04E3C4 92020003 */   lbu   $v0, 3($s0)
/* 082EF8 7F04E3C8 0C002914 */  jal   randomGetNext
/* 082EFC 7F04E3CC 00000000 */   nop
/* 082F00 7F04E3D0 2401000D */  li    $at, 13
/* 082F04 7F04E3D4 0041001B */  divu  $zero, $v0, $at
/* 082F08 7F04E3D8 00002010 */  mfhi  $a0
/* 082F0C 7F04E3DC 00802825 */  move  $a1, $a0
/* 082F10 7F04E3E0 00000000 */  nop
/* 082F14 7F04E3E4 00046880 */  sll   $t5, $a0, 2
.L7F04E3E8:
/* 082F18 7F04E3E8 020D1821 */  addu  $v1, $s0, $t5
/* 082F1C 7F04E3EC 946E0082 */  lhu   $t6, 0x82($v1)
/* 082F20 7F04E3F0 59C0004E */  blezl $t6, .L7F04E52C
/* 082F24 7F04E3F4 24840001 */   addiu $a0, $a0, 1
/* 082F28 7F04E3F8 946F0080 */  lhu   $t7, 0x80($v1)
/* 082F2C 7F04E3FC 3401FFFF */  li    $at, 65535
/* 082F30 7F04E400 51E1004A */  beql  $t7, $at, .L7F04E52C
/* 082F34 7F04E404 24840001 */   addiu $a0, $a0, 1
/* 082F38 7F04E408 AFA30024 */  sw    $v1, 0x24($sp)
/* 082F3C 7F04E40C AFA400C4 */  sw    $a0, 0xc4($sp)
/* 082F40 7F04E410 0FC14634 */  jal   ammocrateAllocate
/* 082F44 7F04E414 AFA500C8 */   sw    $a1, 0xc8($sp)
/* 082F48 7F04E418 8FA30024 */  lw    $v1, 0x24($sp)
/* 082F4C 7F04E41C 8FA400C4 */  lw    $a0, 0xc4($sp)
/* 082F50 7F04E420 8FA500C8 */  lw    $a1, 0xc8($sp)
/* 082F54 7F04E424 10400040 */  beqz  $v0, .L7F04E528
/* 082F58 7F04E428 00403025 */   move  $a2, $v0
/* 082F5C 7F04E42C 3C198003 */  lui   $t9, %hi(blank_07_object)
/* 082F60 7F04E430 27A8003C */  addiu $t0, $sp, 0x3c
/* 082F64 7F04E434 27392094 */  addiu $t9, %lo(blank_07_object) # addiu $t9, $t9, 0x2094
/* 082F68 7F04E438 94670080 */  lhu   $a3, 0x80($v1)
/* 082F6C 7F04E43C 27290078 */  addiu $t1, $t9, 0x78
/* 082F70 7F04E440 01005825 */  move  $t3, $t0
.L7F04E444:
/* 082F74 7F04E444 8F210000 */  lw    $at, ($t9)
/* 082F78 7F04E448 2739000C */  addiu $t9, $t9, 0xc
/* 082F7C 7F04E44C 256B000C */  addiu $t3, $t3, 0xc
/* 082F80 7F04E450 AD61FFF4 */  sw    $at, -0xc($t3)
/* 082F84 7F04E454 8F21FFF8 */  lw    $at, -8($t9)
/* 082F88 7F04E458 AD61FFF8 */  sw    $at, -8($t3)
/* 082F8C 7F04E45C 8F21FFFC */  lw    $at, -4($t9)
/* 082F90 7F04E460 1729FFF8 */  bne   $t9, $t1, .L7F04E444
/* 082F94 7F04E464 AD61FFFC */   sw    $at, -4($t3)
/* 082F98 7F04E468 8F210000 */  lw    $at, ($t9)
/* 082F9C 7F04E46C 01006825 */  move  $t5, $t0
/* 082FA0 7F04E470 00407025 */  move  $t6, $v0
/* 082FA4 7F04E474 AD610000 */  sw    $at, ($t3)
/* 082FA8 7F04E478 8F290004 */  lw    $t1, 4($t9)
/* 082FAC 7F04E47C 250C0078 */  addiu $t4, $t0, 0x78
/* 082FB0 7F04E480 0007C080 */  sll   $t8, $a3, 2
/* 082FB4 7F04E484 AD690004 */  sw    $t1, 4($t3)
.L7F04E488:
/* 082FB8 7F04E488 8DA10000 */  lw    $at, ($t5)
/* 082FBC 7F04E48C 25AD000C */  addiu $t5, $t5, 0xc
/* 082FC0 7F04E490 25CE000C */  addiu $t6, $t6, 0xc
/* 082FC4 7F04E494 ADC1FFF4 */  sw    $at, -0xc($t6)
/* 082FC8 7F04E498 8DA1FFF8 */  lw    $at, -8($t5)
/* 082FCC 7F04E49C ADC1FFF8 */  sw    $at, -8($t6)
/* 082FD0 7F04E4A0 8DA1FFFC */  lw    $at, -4($t5)
/* 082FD4 7F04E4A4 15ACFFF8 */  bne   $t5, $t4, .L7F04E488
/* 082FD8 7F04E4A8 ADC1FFFC */   sw    $at, -4($t6)
/* 082FDC 7F04E4AC 8DA10000 */  lw    $at, ($t5)
/* 082FE0 7F04E4B0 24850001 */  addiu $a1, $a0, 1
/* 082FE4 7F04E4B4 00C02025 */  move  $a0, $a2
/* 082FE8 7F04E4B8 ADC10000 */  sw    $at, ($t6)
/* 082FEC 7F04E4BC 8DAC0004 */  lw    $t4, 4($t5)
/* 082FF0 7F04E4C0 24010002 */  li    $at, 2
/* 082FF4 7F04E4C4 0307C023 */  subu  $t8, $t8, $a3
/* 082FF8 7F04E4C8 ADCC0004 */  sw    $t4, 4($t6)
/* 082FFC 7F04E4CC A4470004 */  sh    $a3, 4($v0)
/* 083000 7F04E4D0 14A10003 */  bne   $a1, $at, .L7F04E4E0
/* 083004 7F04E4D4 AC450080 */   sw    $a1, 0x80($v0)
/* 083008 7F04E4D8 240F0001 */  li    $t7, 1
/* 08300C 7F04E4DC AC4F0080 */  sw    $t7, 0x80($v0)
.L7F04E4E0:
/* 083010 7F04E4E0 0018C080 */  sll   $t8, $t8, 2
/* 083014 7F04E4E4 3C058004 */  lui   $a1, %hi(PitemZ_entries)
/* 083018 7F04E4E8 00B82821 */  addu  $a1, $a1, $t8
/* 08301C 7F04E4EC 8CA5A228 */  lw    $a1, %lo(PitemZ_entries)($a1)
/* 083020 7F04E4F0 0FC101BE */  jal   objInitWithModelDef
/* 083024 7F04E4F4 AFA600C0 */   sw    $a2, 0xc0($sp)
/* 083028 7F04E4F8 10400011 */  beqz  $v0, .L7F04E540
/* 08302C 7F04E4FC 8FA600C0 */   lw    $a2, 0xc0($sp)
/* 083030 7F04E500 8CC40014 */  lw    $a0, 0x14($a2)
/* 083034 7F04E504 8C850014 */  lw    $a1, 0x14($a0)
/* 083038 7F04E508 0FC1B39E */  jal   modelSetScale
/* 08303C 7F04E50C AFA600C0 */   sw    $a2, 0xc0($sp)
/* 083040 7F04E510 8FA600C0 */  lw    $a2, 0xc0($sp)
/* 083044 7F04E514 8E050010 */  lw    $a1, 0x10($s0)
/* 083048 7F04E518 0FC0E969 */  jal   chrpropReparent
/* 08304C 7F04E51C 8CC40010 */   lw    $a0, 0x10($a2)
/* 083050 7F04E520 10000008 */  b     .L7F04E544
/* 083054 7F04E524 92020003 */   lbu   $v0, 3($s0)
.L7F04E528:
/* 083058 7F04E528 24840001 */  addiu $a0, $a0, 1
.L7F04E52C:
/* 08305C 7F04E52C 2401000D */  li    $at, 13
/* 083060 7F04E530 0081001A */  div   $zero, $a0, $at
/* 083064 7F04E534 00002010 */  mfhi  $a0
/* 083068 7F04E538 5485FFAB */  bnel  $a0, $a1, .L7F04E3E8
/* 08306C 7F04E53C 00046880 */   sll   $t5, $a0, 2
.L7F04E540:
/* 083070 7F04E540 92020003 */  lbu   $v0, 3($s0)
.L7F04E544:
/* 083074 7F04E544 2401000D */  li    $at, 13
/* 083078 7F04E548 54410010 */  bnel  $v0, $at, .L7F04E58C
/* 08307C 7F04E54C 24010006 */   li    $at, 6
/* 083080 7F04E550 8E190008 */  lw    $t9, 8($s0)
/* 083084 7F04E554 3C014000 */  lui   $at, 0x4000
/* 083088 7F04E558 02002025 */  move  $a0, $s0
/* 08308C 7F04E55C 03215825 */  or    $t3, $t9, $at
/* 083090 7F04E560 0FC0FFF0 */  jal   objGetDestroyedLevel
/* 083094 7F04E564 AE0B0008 */   sw    $t3, 8($s0)
/* 083098 7F04E568 24010001 */  li    $at, 1
/* 08309C 7F04E56C 14410057 */  bne   $v0, $at, .L7F04E6CC
/* 0830A0 7F04E570 00000000 */   nop
/* 0830A4 7F04E574 8E0A0008 */  lw    $t2, 8($s0)
/* 0830A8 7F04E578 3C011000 */  lui   $at, 0x1000
/* 0830AC 7F04E57C 01416025 */  or    $t4, $t2, $at
/* 0830B0 7F04E580 10000052 */  b     .L7F04E6CC
/* 0830B4 7F04E584 AE0C0008 */   sw    $t4, 8($s0)
/* 0830B8 7F04E588 24010006 */  li    $at, 6
.L7F04E58C:
/* 0830BC 7F04E58C 5441000C */  bnel  $v0, $at, .L7F04E5C0
/* 0830C0 7F04E590 2401000A */   li    $at, 10
/* 0830C4 7F04E594 0FC0FFF0 */  jal   objGetDestroyedLevel
/* 0830C8 7F04E598 02002025 */   move  $a0, $s0
/* 0830CC 7F04E59C 24010001 */  li    $at, 1
/* 0830D0 7F04E5A0 1441004A */  bne   $v0, $at, .L7F04E6CC
/* 0830D4 7F04E5A4 00000000 */   nop
/* 0830D8 7F04E5A8 8E0D0008 */  lw    $t5, 8($s0)
/* 0830DC 7F04E5AC 3C011000 */  lui   $at, 0x1000
/* 0830E0 7F04E5B0 01A17025 */  or    $t6, $t5, $at
/* 0830E4 7F04E5B4 10000045 */  b     .L7F04E6CC
/* 0830E8 7F04E5B8 AE0E0008 */   sw    $t6, 8($s0)
/* 0830EC 7F04E5BC 2401000A */  li    $at, 10
.L7F04E5C0:
/* 0830F0 7F04E5C0 5441000C */  bnel  $v0, $at, .L7F04E5F4
/* 0830F4 7F04E5C4 2401000B */   li    $at, 11
/* 0830F8 7F04E5C8 0FC0FFF0 */  jal   objGetDestroyedLevel
/* 0830FC 7F04E5CC 02002025 */   move  $a0, $s0
/* 083100 7F04E5D0 24010001 */  li    $at, 1
/* 083104 7F04E5D4 1441003D */  bne   $v0, $at, .L7F04E6CC
/* 083108 7F04E5D8 26040080 */   addiu $a0, $s0, 0x80
/* 08310C 7F04E5DC 3C058003 */  lui   $a1, %hi(monAnim33BlackSolid)
/* 083110 7F04E5E0 0FC12723 */  jal   save_ptr_monitor_ani_code_to_obj_ani_slot
/* 083114 7F04E5E4 24A51EE8 */   addiu $a1, %lo(monAnim33BlackSolid) # addiu $a1, $a1, 0x1ee8
/* 083118 7F04E5E8 10000038 */  b     .L7F04E6CC
/* 08311C 7F04E5EC 00000000 */   nop
/* 083120 7F04E5F0 2401000B */  li    $at, 11
.L7F04E5F4:
/* 083124 7F04E5F4 54410018 */  bnel  $v0, $at, .L7F04E658
/* 083128 7F04E5F8 24010024 */   li    $at, 36
/* 08312C 7F04E5FC 0FC0FFF0 */  jal   objGetDestroyedLevel
/* 083130 7F04E600 02002025 */   move  $a0, $s0
/* 083134 7F04E604 24010001 */  li    $at, 1
/* 083138 7F04E608 14410030 */  bne   $v0, $at, .L7F04E6CC
/* 08313C 7F04E60C 26040080 */   addiu $a0, $s0, 0x80
/* 083140 7F04E610 3C058003 */  lui   $a1, %hi(monAnim33BlackSolid)
/* 083144 7F04E614 0FC12723 */  jal   save_ptr_monitor_ani_code_to_obj_ani_slot
/* 083148 7F04E618 24A51EE8 */   addiu $a1, %lo(monAnim33BlackSolid) # addiu $a1, $a1, 0x1ee8
/* 08314C 7F04E61C 3C058003 */  lui   $a1, %hi(monAnim33BlackSolid)
/* 083150 7F04E620 24A51EE8 */  addiu $a1, %lo(monAnim33BlackSolid) # addiu $a1, $a1, 0x1ee8
/* 083154 7F04E624 0FC12723 */  jal   save_ptr_monitor_ani_code_to_obj_ani_slot
/* 083158 7F04E628 260400F4 */   addiu $a0, $s0, 0xf4
/* 08315C 7F04E62C 3C058003 */  lui   $a1, %hi(monAnim33BlackSolid)
/* 083160 7F04E630 24A51EE8 */  addiu $a1, %lo(monAnim33BlackSolid) # addiu $a1, $a1, 0x1ee8
/* 083164 7F04E634 0FC12723 */  jal   save_ptr_monitor_ani_code_to_obj_ani_slot
/* 083168 7F04E638 26040168 */   addiu $a0, $s0, 0x168
/* 08316C 7F04E63C 3C058003 */  lui   $a1, %hi(monAnim33BlackSolid)
/* 083170 7F04E640 24A51EE8 */  addiu $a1, %lo(monAnim33BlackSolid) # addiu $a1, $a1, 0x1ee8
/* 083174 7F04E644 0FC12723 */  jal   save_ptr_monitor_ani_code_to_obj_ani_slot
/* 083178 7F04E648 260401DC */   addiu $a0, $s0, 0x1dc
/* 08317C 7F04E64C 1000001F */  b     .L7F04E6CC
/* 083180 7F04E650 00000000 */   nop
/* 083184 7F04E654 24010024 */  li    $at, 36
.L7F04E658:
/* 083188 7F04E658 5441000B */  bnel  $v0, $at, .L7F04E688
/* 08318C 7F04E65C 24010015 */   li    $at, 21
/* 083190 7F04E660 0FC0FFF0 */  jal   objGetDestroyedLevel
/* 083194 7F04E664 02002025 */   move  $a0, $s0
/* 083198 7F04E668 24010001 */  li    $at, 1
/* 08319C 7F04E66C 14410017 */  bne   $v0, $at, .L7F04E6CC
/* 0831A0 7F04E670 00000000 */   nop
/* 0831A4 7F04E674 0FC15799 */  jal   init_trigger_toxic_gas_effect
/* 0831A8 7F04E678 26040058 */   addiu $a0, $s0, 0x58
/* 0831AC 7F04E67C 10000013 */  b     .L7F04E6CC
/* 0831B0 7F04E680 00000000 */   nop
/* 0831B4 7F04E684 24010015 */  li    $at, 21
.L7F04E688:
/* 0831B8 7F04E688 14410010 */  bne   $v0, $at, .L7F04E6CC
/* 0831BC 7F04E68C 00000000 */   nop
/* 0831C0 7F04E690 0FC0FFF0 */  jal   objGetDestroyedLevel
/* 0831C4 7F04E694 02002025 */   move  $a0, $s0
/* 0831C8 7F04E698 5440000A */  bnezl $v0, .L7F04E6C4
/* 0831CC 7F04E69C 44804000 */   mtc1  $zero, $f8
/* 0831D0 7F04E6A0 C6000074 */  lwc1  $f0, 0x74($s0)
/* 0831D4 7F04E6A4 C6100070 */  lwc1  $f16, 0x70($s0)
/* 0831D8 7F04E6A8 C60A0080 */  lwc1  $f10, 0x80($s0)
/* 0831DC 7F04E6AC 46100481 */  sub.s $f18, $f0, $f16
/* 0831E0 7F04E6B0 46125182 */  mul.s $f6, $f10, $f18
/* 0831E4 7F04E6B4 46003103 */  div.s $f4, $f6, $f0
/* 0831E8 7F04E6B8 10000004 */  b     .L7F04E6CC
/* 0831EC 7F04E6BC E6040084 */   swc1  $f4, 0x84($s0)
/* 0831F0 7F04E6C0 44804000 */  mtc1  $zero, $f8
.L7F04E6C4:
/* 0831F4 7F04E6C4 00000000 */  nop
/* 0831F8 7F04E6C8 E6080084 */  swc1  $f8, 0x84($s0)
.L7F04E6CC:
/* 0831FC 7F04E6CC 0FC0FFF0 */  jal   objGetDestroyedLevel
/* 083200 7F04E6D0 02002025 */   move  $a0, $s0
/* 083204 7F04E6D4 24010001 */  li    $at, 1
/* 083208 7F04E6D8 5441000D */  bnel  $v0, $at, .L7F04E710
/* 08320C 7F04E6DC 8FBF001C */   lw    $ra, 0x1c($sp)
/* 083210 7F04E6E0 8E0F0010 */  lw    $t7, 0x10($s0)
/* 083214 7F04E6E4 8DE40020 */  lw    $a0, 0x20($t7)
/* 083218 7F04E6E8 50800009 */  beql  $a0, $zero, .L7F04E710
/* 08321C 7F04E6EC 8FBF001C */   lw    $ra, 0x1c($sp)
/* 083220 7F04E6F0 8C820024 */  lw    $v0, 0x24($a0)
.L7F04E6F4:
/* 083224 7F04E6F4 24050001 */  li    $a1, 1
/* 083228 7F04E6F8 0FC12FF4 */  jal   propobjSetDropped
/* 08322C 7F04E6FC AFA20028 */   sw    $v0, 0x28($sp)
/* 083230 7F04E700 8FA40028 */  lw    $a0, 0x28($sp)
/* 083234 7F04E704 5480FFFB */  bnezl $a0, .L7F04E6F4
/* 083238 7F04E708 8C820024 */   lw    $v0, 0x24($a0)
.L7F04E70C:
/* 08323C 7F04E70C 8FBF001C */  lw    $ra, 0x1c($sp)
.L7F04E710:
/* 083240 7F04E710 8FB00018 */  lw    $s0, 0x18($sp)
/* 083244 7F04E714 27BD00E0 */  addiu $sp, $sp, 0xe0
/* 083248 7F04E718 03E00008 */  jr    $ra
/* 08324C 7F04E71C 00000000 */   nop
)
#endif





#ifdef NONMATCHING
void sub_GAME_7F04E720(PropRecord* prop, struct ShotData* shotdata) {

}
#else
void sub_GAME_7F04E720(PropRecord*, struct ShotData*);
GLOBAL_ASM(
.text
glabel sub_GAME_7F04E720
/* 083250 7F04E720 27BDFF40 */  addiu $sp, $sp, -0xc0
/* 083254 7F04E724 AFBF004C */  sw    $ra, 0x4c($sp)
/* 083258 7F04E728 AFB50048 */  sw    $s5, 0x48($sp)
/* 08325C 7F04E72C AFB40044 */  sw    $s4, 0x44($sp)
/* 083260 7F04E730 AFB30040 */  sw    $s3, 0x40($sp)
/* 083264 7F04E734 AFB2003C */  sw    $s2, 0x3c($sp)
/* 083268 7F04E738 AFB10038 */  sw    $s1, 0x38($sp)
/* 08326C 7F04E73C AFB00034 */  sw    $s0, 0x34($sp)
/* 083270 7F04E740 AFA400C0 */  sw    $a0, 0xc0($sp)
/* 083274 7F04E744 8C950004 */  lw    $s5, 4($a0)
/* 083278 7F04E748 00A09825 */  move  $s3, $a1
/* 08327C 7F04E74C 8EAF0064 */  lw    $t7, 0x64($s5)
/* 083280 7F04E750 31F81000 */  andi  $t8, $t7, 0x1000
/* 083284 7F04E754 57000091 */  bnezl $t8, .L7F04E99C
/* 083288 7F04E758 8FBF004C */   lw    $ra, 0x4c($sp)
/* 08328C 7F04E75C 90880001 */  lbu   $t0, 1($a0)
/* 083290 7F04E760 31090002 */  andi  $t1, $t0, 2
/* 083294 7F04E764 5120008D */  beql  $t1, $zero, .L7F04E99C
/* 083298 7F04E768 8FBF004C */   lw    $ra, 0x4c($sp)
/* 08329C 7F04E76C 8C900020 */  lw    $s0, 0x20($a0)
/* 0832A0 7F04E770 52000008 */  beql  $s0, $zero, .L7F04E794
/* 0832A4 7F04E774 8EB40014 */   lw    $s4, 0x14($s5)
.L7F04E778:
/* 0832A8 7F04E778 8E110024 */  lw    $s1, 0x24($s0)
/* 0832AC 7F04E77C 02002025 */  move  $a0, $s0
/* 0832B0 7F04E780 0FC139C8 */  jal   sub_GAME_7F04E720
/* 0832B4 7F04E784 02602825 */   move  $a1, $s3
/* 0832B8 7F04E788 1620FFFB */  bnez  $s1, .L7F04E778
/* 0832BC 7F04E78C 02208025 */   move  $s0, $s1
/* 0832C0 7F04E790 8EB40014 */  lw    $s4, 0x14($s5)
.L7F04E794:
/* 0832C4 7F04E794 AFA000A8 */  sw    $zero, 0xa8($sp)
/* 0832C8 7F04E798 92AB0003 */  lbu   $t3, 3($s5)
/* 0832CC 7F04E79C 24010001 */  li    $at, 1
/* 0832D0 7F04E7A0 2670000C */  addiu $s0, $s3, 0xc
/* 0832D4 7F04E7A4 15610025 */  bne   $t3, $at, .L7F04E83C
/* 0832D8 7F04E7A8 27B100A8 */   addiu $s1, $sp, 0xa8
/* 0832DC 7F04E7AC 96AC0098 */  lhu   $t4, 0x98($s5)
/* 0832E0 7F04E7B0 26A400D0 */  addiu $a0, $s5, 0xd0
/* 0832E4 7F04E7B4 02603025 */  move  $a2, $s3
/* 0832E8 7F04E7B8 318D0004 */  andi  $t5, $t4, 4
/* 0832EC 7F04E7BC 55A00008 */  bnezl $t5, .L7F04E7E0
/* 0832F0 7F04E7C0 2670000C */   addiu $s0, $s3, 0xc
/* 0832F4 7F04E7C4 96A2009A */  lhu   $v0, 0x9a($s5)
/* 0832F8 7F04E7C8 24010006 */  li    $at, 6
/* 0832FC 7F04E7CC 10410003 */  beq   $v0, $at, .L7F04E7DC
/* 083300 7F04E7D0 24010007 */   li    $at, 7
/* 083304 7F04E7D4 14410019 */  bne   $v0, $at, .L7F04E83C
/* 083308 7F04E7D8 00000000 */   nop
.L7F04E7DC:
/* 08330C 7F04E7DC 2670000C */  addiu $s0, $s3, 0xc
.L7F04E7E0:
/* 083310 7F04E7E0 02003825 */  move  $a3, $s0
/* 083314 7F04E7E4 0FC1D1F4 */  jal   sub_GAME_7F0747D0
/* 083318 7F04E7E8 8E85000C */   lw    $a1, 0xc($s4)
/* 08331C 7F04E7EC 8E8E0008 */  lw    $t6, 8($s4)
/* 083320 7F04E7F0 00409025 */  move  $s2, $v0
/* 083324 7F04E7F4 02802025 */  move  $a0, $s4
/* 083328 7F04E7F8 8DCF0000 */  lw    $t7, ($t6)
/* 08332C 7F04E7FC 02603025 */  move  $a2, $s3
/* 083330 7F04E800 18400024 */  blez  $v0, .L7F04E894
/* 083334 7F04E804 AFAF00A8 */   sw    $t7, 0xa8($sp)
/* 083338 7F04E808 27B80078 */  addiu $t8, $sp, 0x78
/* 08333C 7F04E80C 27B90074 */  addiu $t9, $sp, 0x74
/* 083340 7F04E810 27A80060 */  addiu $t0, $sp, 0x60
/* 083344 7F04E814 AFA80018 */  sw    $t0, 0x18($sp)
/* 083348 7F04E818 AFB90014 */  sw    $t9, 0x14($sp)
/* 08334C 7F04E81C AFB80010 */  sw    $t8, 0x10($sp)
/* 083350 7F04E820 01E02825 */  move  $a1, $t7
/* 083354 7F04E824 0FC1366C */  jal   sub_GAME_7F04D9B0
/* 083358 7F04E828 02003825 */   move  $a3, $s0
/* 08335C 7F04E82C 14400019 */  bnez  $v0, .L7F04E894
/* 083360 7F04E830 00000000 */   nop
/* 083364 7F04E834 10000017 */  b     .L7F04E894
/* 083368 7F04E838 00009025 */   move  $s2, $zero
.L7F04E83C:
/* 08336C 7F04E83C 02802025 */  move  $a0, $s4
.L7F04E840:
/* 083370 7F04E840 02602825 */  move  $a1, $s3
/* 083374 7F04E844 02003025 */  move  $a2, $s0
/* 083378 7F04E848 0FC1D4BF */  jal   sub_GAME_7F0752FC
/* 08337C 7F04E84C 02203825 */   move  $a3, $s1
/* 083380 7F04E850 1840000E */  blez  $v0, .L7F04E88C
/* 083384 7F04E854 00409025 */   move  $s2, $v0
/* 083388 7F04E858 27A90078 */  addiu $t1, $sp, 0x78
/* 08338C 7F04E85C 27AA0074 */  addiu $t2, $sp, 0x74
/* 083390 7F04E860 27AB0060 */  addiu $t3, $sp, 0x60
/* 083394 7F04E864 AFAB0018 */  sw    $t3, 0x18($sp)
/* 083398 7F04E868 AFAA0014 */  sw    $t2, 0x14($sp)
/* 08339C 7F04E86C AFA90010 */  sw    $t1, 0x10($sp)
/* 0833A0 7F04E870 02802025 */  move  $a0, $s4
/* 0833A4 7F04E874 8FA500A8 */  lw    $a1, 0xa8($sp)
/* 0833A8 7F04E878 02603025 */  move  $a2, $s3
/* 0833AC 7F04E87C 0FC1366C */  jal   sub_GAME_7F04D9B0
/* 0833B0 7F04E880 02003825 */   move  $a3, $s0
/* 0833B4 7F04E884 14400003 */  bnez  $v0, .L7F04E894
/* 0833B8 7F04E888 00000000 */   nop
.L7F04E88C:
/* 0833BC 7F04E88C 5E40FFEC */  bgtzl $s2, .L7F04E840
/* 0833C0 7F04E890 02802025 */   move  $a0, $s4
.L7F04E894:
/* 0833C4 7F04E894 1A400040 */  blez  $s2, .L7F04E998
/* 0833C8 7F04E898 8FAD0074 */   lw    $t5, 0x74($sp)
/* 0833CC 7F04E89C 8E8C000C */  lw    $t4, 0xc($s4)
/* 0833D0 7F04E8A0 C7A40078 */  lwc1  $f4, 0x78($sp)
/* 0833D4 7F04E8A4 C7A6007C */  lwc1  $f6, 0x7c($sp)
/* 0833D8 7F04E8A8 C7A80080 */  lwc1  $f8, 0x80($sp)
/* 0833DC 7F04E8AC 000D7180 */  sll   $t6, $t5, 6
/* 0833E0 7F04E8B0 27A50068 */  addiu $a1, $sp, 0x68
/* 0833E4 7F04E8B4 018E2021 */  addu  $a0, $t4, $t6
/* 0833E8 7F04E8B8 E7A40068 */  swc1  $f4, 0x68($sp)
/* 0833EC 7F04E8BC E7A6006C */  swc1  $f6, 0x6c($sp)
/* 0833F0 7F04E8C0 0FC1611D */  jal   mtx4TransformVecInPlace
/* 0833F4 7F04E8C4 E7A80070 */   swc1  $f8, 0x70($sp)
/* 0833F8 7F04E8C8 C7A00070 */  lwc1  $f0, 0x70($sp)
/* 0833FC 7F04E8CC C66A0034 */  lwc1  $f10, 0x34($s3)
/* 083400 7F04E8D0 02602025 */  move  $a0, $s3
/* 083404 7F04E8D4 46000007 */  neg.s $f0, $f0
/* 083408 7F04E8D8 8FA500C0 */  lw    $a1, 0xc0($sp)
/* 08340C 7F04E8DC 460A003E */  c.le.s $f0, $f10
/* 083410 7F04E8E0 02403825 */  move  $a3, $s2
/* 083414 7F04E8E4 8FAC00A8 */  lw    $t4, 0xa8($sp)
/* 083418 7F04E8E8 27AE0078 */  addiu $t6, $sp, 0x78
/* 08341C 7F04E8EC 4502002B */  bc1fl .L7F04E99C
/* 083420 7F04E8F0 8FBF004C */   lw    $ra, 0x4c($sp)
/* 083424 7F04E8F4 8EAF0008 */  lw    $t7, 8($s5)
/* 083428 7F04E8F8 24030001 */  li    $v1, 1
/* 08342C 7F04E8FC 000FC380 */  sll   $t8, $t7, 0xe
/* 083430 7F04E900 07020017 */  bltzl $t8, .L7F04E960
/* 083434 7F04E904 8FAF0074 */   lw    $t7, 0x74($sp)
/* 083438 7F04E908 92A20003 */  lbu   $v0, 3($s5)
/* 08343C 7F04E90C 2401002A */  li    $at, 42
/* 083440 7F04E910 10410003 */  beq   $v0, $at, .L7F04E920
/* 083444 7F04E914 2401002F */   li    $at, 47
/* 083448 7F04E918 54410004 */  bnel  $v0, $at, .L7F04E92C
/* 08344C 7F04E91C 8EB90014 */   lw    $t9, 0x14($s5)
.L7F04E920:
/* 083450 7F04E920 1000000E */  b     .L7F04E95C
/* 083454 7F04E924 00001825 */   move  $v1, $zero
/* 083458 7F04E928 8EB90014 */  lw    $t9, 0x14($s5)
.L7F04E92C:
/* 08345C 7F04E92C 3C088004 */  lui   $t0, %hi(skeleton_door)
/* 083460 7F04E930 2508A1DC */  addiu $t0, %lo(skeleton_door) # addiu $t0, $t0, -0x5e24
/* 083464 7F04E934 8F220008 */  lw    $v0, 8($t9)
/* 083468 7F04E938 8C490004 */  lw    $t1, 4($v0)
/* 08346C 7F04E93C 55090008 */  bnel  $t0, $t1, .L7F04E960
/* 083470 7F04E940 8FAF0074 */   lw    $t7, 0x74($sp)
/* 083474 7F04E944 8C4B0008 */  lw    $t3, 8($v0)
/* 083478 7F04E948 8FAA0060 */  lw    $t2, 0x60($sp)
/* 08347C 7F04E94C 8D6D000C */  lw    $t5, 0xc($t3)
/* 083480 7F04E950 554D0003 */  bnel  $t2, $t5, .L7F04E960
/* 083484 7F04E954 8FAF0074 */   lw    $t7, 0x74($sp)
/* 083488 7F04E958 00001825 */  move  $v1, $zero
.L7F04E95C:
/* 08348C 7F04E95C 8FAF0074 */  lw    $t7, 0x74($sp)
.L7F04E960:
/* 083490 7F04E960 8FB80060 */  lw    $t8, 0x60($sp)
/* 083494 7F04E964 AFAC0010 */  sw    $t4, 0x10($sp)
/* 083498 7F04E968 AFAE0014 */  sw    $t6, 0x14($sp)
/* 08349C 7F04E96C AFB40020 */  sw    $s4, 0x20($sp)
/* 0834A0 7F04E970 AFA30024 */  sw    $v1, 0x24($sp)
/* 0834A4 7F04E974 AFAF0018 */  sw    $t7, 0x18($sp)
/* 0834A8 7F04E978 AFB8001C */  sw    $t8, 0x1c($sp)
/* 0834AC 7F04E97C 8EB9000C */  lw    $t9, 0xc($s5)
/* 0834B0 7F04E980 3C010010 */  lui   $at, 0x10
/* 0834B4 7F04E984 44060000 */  mfc1  $a2, $f0
/* 0834B8 7F04E988 03214024 */  and   $t0, $t9, $at
/* 0834BC 7F04E98C 0008482B */  sltu  $t1, $zero, $t0
/* 0834C0 7F04E990 0FC0EE70 */  jal   sub_GAME_7F03B9C0
/* 0834C4 7F04E994 AFA90028 */   sw    $t1, 0x28($sp)
.L7F04E998:
/* 0834C8 7F04E998 8FBF004C */  lw    $ra, 0x4c($sp)
.L7F04E99C:
/* 0834CC 7F04E99C 8FB00034 */  lw    $s0, 0x34($sp)
/* 0834D0 7F04E9A0 8FB10038 */  lw    $s1, 0x38($sp)
/* 0834D4 7F04E9A4 8FB2003C */  lw    $s2, 0x3c($sp)
/* 0834D8 7F04E9A8 8FB30040 */  lw    $s3, 0x40($sp)
/* 0834DC 7F04E9AC 8FB40044 */  lw    $s4, 0x44($sp)
/* 0834E0 7F04E9B0 8FB50048 */  lw    $s5, 0x48($sp)
/* 0834E4 7F04E9B4 03E00008 */  jr    $ra
/* 0834E8 7F04E9B8 27BD00C0 */   addiu $sp, $sp, 0xc0
)
#endif


void sub_GAME_7F04E9BC(PropRecord* prop, struct ShotData* shotdata)
{
    ObjectRecord *obj;
    f32 tmp;
    Model *model;
    struct ModelRoData_BoundingBoxRecord *bbox;

    obj = prop->obj;
    model = obj->model;
    bbox = chrobjGetBboxFromObjectRecord(obj);

    if ((prop->flags & PROPFLAG_ONSCREEN)
            && (obj->runtime_bitflags & RUNTIMEBITFLAG_00001000) == 0
            && (obj->flags2 & PROPFLAG2_SHOOTTHROUGH) == 0) {
        tmp = -(model->render_pos->pos.m[3][2] + chrpropSumMatrixNegZ(bbox, (Mtxf*)model->render_pos));

        if (tmp <= shotdata->unk34) {
            sub_GAME_7F04E720(prop, (void*)shotdata);
        }
    }
}


#ifdef NONMATCHING
void sub_GAME_7F04EA68(void) {

}
#else
GLOBAL_ASM(
.text
glabel sub_GAME_7F04EA68
/* 083598 7F04EA68 27BDFF90 */  addiu $sp, $sp, -0x70
/* 08359C 7F04EA6C AFBF002C */  sw    $ra, 0x2c($sp)
/* 0835A0 7F04EA70 AFB10028 */  sw    $s1, 0x28($sp)
/* 0835A4 7F04EA74 AFB00024 */  sw    $s0, 0x24($sp)
/* 0835A8 7F04EA78 8CA60004 */  lw    $a2, 4($a1)
/* 0835AC 7F04EA7C 00A08025 */  move  $s0, $a1
/* 0835B0 7F04EA80 8CC2001C */  lw    $v0, 0x1c($a2)
/* 0835B4 7F04EA84 00C01825 */  move  $v1, $a2
/* 0835B8 7F04EA88 50400006 */  beql  $v0, $zero, .L7F04EAA4
/* 0835BC 7F04EA8C C6040000 */   lwc1  $f4, ($s0)
/* 0835C0 7F04EA90 00401825 */  move  $v1, $v0
.L7F04EA94:
/* 0835C4 7F04EA94 8C42001C */  lw    $v0, 0x1c($v0)
/* 0835C8 7F04EA98 5440FFFE */  bnezl $v0, .L7F04EA94
/* 0835CC 7F04EA9C 00401825 */   move  $v1, $v0
/* 0835D0 7F04EAA0 C6040000 */  lwc1  $f4, ($s0)
.L7F04EAA4:
/* 0835D4 7F04EAA4 C486000C */  lwc1  $f6, 0xc($a0)
/* 0835D8 7F04EAA8 C48A0014 */  lwc1  $f10, 0x14($a0)
/* 0835DC 7F04EAAC C4920000 */  lwc1  $f18, ($a0)
/* 0835E0 7F04EAB0 46062202 */  mul.s $f8, $f4, $f6
/* 0835E4 7F04EAB4 8CD10004 */  lw    $s1, 4($a2)
/* 0835E8 7F04EAB8 3C0141D0 */  li    $at, 0x41D00000 # 26.000000
/* 0835EC 7F04EABC 44810000 */  mtc1  $at, $f0
/* 0835F0 7F04EAC0 460A4403 */  div.s $f16, $f8, $f10
/* 0835F4 7F04EAC4 46109101 */  sub.s $f4, $f18, $f16
/* 0835F8 7F04EAC8 E7A40060 */  swc1  $f4, 0x60($sp)
/* 0835FC 7F04EACC C4880010 */  lwc1  $f8, 0x10($a0)
/* 083600 7F04EAD0 C6060000 */  lwc1  $f6, ($s0)
/* 083604 7F04EAD4 C4920014 */  lwc1  $f18, 0x14($a0)
/* 083608 7F04EAD8 C4840004 */  lwc1  $f4, 4($a0)
/* 08360C 7F04EADC 46083282 */  mul.s $f10, $f6, $f8
/* 083610 7F04EAE0 46125403 */  div.s $f16, $f10, $f18
/* 083614 7F04EAE4 46102181 */  sub.s $f6, $f4, $f16
/* 083618 7F04EAE8 C7A40060 */  lwc1  $f4, 0x60($sp)
/* 08361C 7F04EAEC E7A60064 */  swc1  $f6, 0x64($sp)
/* 083620 7F04EAF0 C60A0000 */  lwc1  $f10, ($s0)
/* 083624 7F04EAF4 C4880008 */  lwc1  $f8, 8($a0)
/* 083628 7F04EAF8 460A4481 */  sub.s $f18, $f8, $f10
/* 08362C 7F04EAFC C7AA0064 */  lwc1  $f10, 0x64($sp)
/* 083630 7F04EB00 E7B20068 */  swc1  $f18, 0x68($sp)
/* 083634 7F04EB04 C490000C */  lwc1  $f16, 0xc($a0)
/* 083638 7F04EB08 46100182 */  mul.s $f6, $f0, $f16
/* 08363C 7F04EB0C 46062201 */  sub.s $f8, $f4, $f6
/* 083640 7F04EB10 C7A60068 */  lwc1  $f6, 0x68($sp)
/* 083644 7F04EB14 E7A80060 */  swc1  $f8, 0x60($sp)
/* 083648 7F04EB18 C4920010 */  lwc1  $f18, 0x10($a0)
/* 08364C 7F04EB1C 46120402 */  mul.s $f16, $f0, $f18
/* 083650 7F04EB20 46105101 */  sub.s $f4, $f10, $f16
/* 083654 7F04EB24 E7A40064 */  swc1  $f4, 0x64($sp)
/* 083658 7F04EB28 C4880014 */  lwc1  $f8, 0x14($a0)
/* 08365C 7F04EB2C AFA40070 */  sw    $a0, 0x70($sp)
/* 083660 7F04EB30 AFA3005C */  sw    $v1, 0x5c($sp)
/* 083664 7F04EB34 46080482 */  mul.s $f18, $f0, $f8
/* 083668 7F04EB38 46123281 */  sub.s $f10, $f6, $f18
/* 08366C 7F04EB3C 0FC1E111 */  jal   currentPlayerGetMatrix10D4
/* 083670 7F04EB40 E7AA0068 */   swc1  $f10, 0x68($sp)
/* 083674 7F04EB44 00402025 */  move  $a0, $v0
/* 083678 7F04EB48 0FC1611D */  jal   mtx4TransformVecInPlace
/* 08367C 7F04EB4C 27A50060 */   addiu $a1, $sp, 0x60
/* 083680 7F04EB50 8E0E004C */  lw    $t6, 0x4c($s0)
/* 083684 7F04EB54 8FA3005C */  lw    $v1, 0x5c($sp)
/* 083688 7F04EB58 11C00004 */  beqz  $t6, .L7F04EB6C
/* 08368C 7F04EB5C 27A40060 */   addiu $a0, $sp, 0x60
/* 083690 7F04EB60 0FC19ED3 */  jal   sub_GAME_7F067B4C
/* 083694 7F04EB64 AFA3005C */   sw    $v1, 0x5c($sp)
/* 083698 7F04EB68 8FA3005C */  lw    $v1, 0x5c($sp)
.L7F04EB6C:
/* 08369C 7F04EB6C 8C6F0014 */  lw    $t7, 0x14($v1)
/* 0836A0 7F04EB70 27A40060 */  addiu $a0, $sp, 0x60
/* 0836A4 7F04EB74 24050001 */  li    $a1, 1
/* 0836A8 7F04EB78 3C0641D0 */  lui   $a2, 0x41d0
/* 0836AC 7F04EB7C 0FC28F87 */  jal   sub_GAME_7F0A3E1C
/* 0836B0 7F04EB80 91E70003 */   lbu   $a3, 3($t7)
/* 0836B4 7F04EB84 0FC13BCD */  jal   objIsHealthy
/* 0836B8 7F04EB88 02202025 */   move  $a0, $s1
/* 0836BC 7F04EB8C 5040000D */  beql  $v0, $zero, .L7F04EBC4
/* 0836C0 7F04EB90 8E0B004C */   lw    $t3, 0x4c($s0)
/* 0836C4 7F04EB94 0FC13812 */  jal   objIsMortal
/* 0836C8 7F04EB98 02202025 */   move  $a0, $s1
/* 0836CC 7F04EB9C 50400009 */  beql  $v0, $zero, .L7F04EBC4
/* 0836D0 7F04EBA0 8E0B004C */   lw    $t3, 0x4c($s0)
/* 0836D4 7F04EBA4 8E18004C */  lw    $t8, 0x4c($s0)
/* 0836D8 7F04EBA8 8FB90070 */  lw    $t9, 0x70($sp)
/* 0836DC 7F04EBAC 24050006 */  li    $a1, 6
/* 0836E0 7F04EBB0 53000004 */  beql  $t8, $zero, .L7F04EBC4
/* 0836E4 7F04EBB4 8E0B004C */   lw    $t3, 0x4c($s0)
/* 0836E8 7F04EBB8 0FC1A9BE */  jal   inc_curplayer_hitcount_with_weapon
/* 0836EC 7F04EBBC 8F240018 */   lw    $a0, 0x18($t9)
/* 0836F0 7F04EBC0 8E0B004C */  lw    $t3, 0x4c($s0)
.L7F04EBC4:
/* 0836F4 7F04EBC4 55600007 */  bnezl $t3, .L7F04EBE4
/* 0836F8 7F04EBC8 8FAC0070 */   lw    $t4, 0x70($sp)
/* 0836FC 7F04EBCC 8E040004 */  lw    $a0, 4($s0)
/* 083700 7F04EBD0 0FC191C8 */  jal   sub_GAME_7F064720
/* 083704 7F04EBD4 24840008 */   addiu $a0, $a0, 8
/* 083708 7F04EBD8 10000007 */  b     .L7F04EBF8
/* 08370C 7F04EBDC 8FAD0070 */   lw    $t5, 0x70($sp)
/* 083710 7F04EBE0 8FAC0070 */  lw    $t4, 0x70($sp)
.L7F04EBE4:
/* 083714 7F04EBE4 8E050004 */  lw    $a1, 4($s0)
/* 083718 7F04EBE8 8606003A */  lh    $a2, 0x3a($s0)
/* 08371C 7F04EBEC 0FC19107 */  jal   recall_joy2_hits_edit_detail_edit_flag
/* 083720 7F04EBF0 8D840018 */   lw    $a0, 0x18($t4)
/* 083724 7F04EBF4 8FAD0070 */  lw    $t5, 0x70($sp)
.L7F04EBF8:
/* 083728 7F04EBF8 24010017 */  li    $at, 23
/* 08372C 7F04EBFC 8DA40018 */  lw    $a0, 0x18($t5)
/* 083730 7F04EC00 10810060 */  beq   $a0, $at, .L7F04ED84
/* 083734 7F04EC04 00000000 */   nop
/* 083738 7F04EC08 8E0E004C */  lw    $t6, 0x4c($s0)
/* 08373C 7F04EC0C 55C00022 */  bnezl $t6, .L7F04EC98
/* 083740 7F04EC10 8602003A */   lh    $v0, 0x3a($s0)
/* 083744 7F04EC14 8E380014 */  lw    $t8, 0x14($s1)
/* 083748 7F04EC18 3C0F8004 */  lui   $t7, %hi(skeleton_door)
/* 08374C 7F04EC1C 25EFA1DC */  addiu $t7, %lo(skeleton_door) # addiu $t7, $t7, -0x5e24
/* 083750 7F04EC20 8F190008 */  lw    $t9, 8($t8)
/* 083754 7F04EC24 8E080004 */  lw    $t0, 4($s0)
/* 083758 7F04EC28 00001825 */  move  $v1, $zero
/* 08375C 7F04EC2C 8F2B0004 */  lw    $t3, 4($t9)
/* 083760 7F04EC30 55EB0003 */  bnel  $t7, $t3, .L7F04EC40
/* 083764 7F04EC34 A3A30057 */   sb    $v1, 0x57($sp)
/* 083768 7F04EC38 24030001 */  li    $v1, 1
/* 08376C 7F04EC3C A3A30057 */  sb    $v1, 0x57($sp)
.L7F04EC40:
/* 083770 7F04EC40 0C002914 */  jal   randomGetNext
/* 083774 7F04EC44 AFA80058 */   sw    $t0, 0x58($sp)
/* 083778 7F04EC48 24010003 */  li    $at, 3
/* 08377C 7F04EC4C 0041001B */  divu  $zero, $v0, $at
/* 083780 7F04EC50 8FA80058 */  lw    $t0, 0x58($sp)
/* 083784 7F04EC54 00003010 */  mfhi  $a2
/* 083788 7F04EC58 83A30057 */  lb    $v1, 0x57($sp)
/* 08378C 7F04EC5C AFA80010 */  sw    $t0, 0x10($sp)
/* 083790 7F04EC60 8E0E0040 */  lw    $t6, 0x40($s0)
/* 083794 7F04EC64 24C60011 */  addiu $a2, $a2, 0x11
/* 083798 7F04EC68 00066400 */  sll   $t4, $a2, 0x10
/* 08379C 7F04EC6C 000C3403 */  sra   $a2, $t4, 0x10
/* 0837A0 7F04EC70 26040010 */  addiu $a0, $s0, 0x10
/* 0837A4 7F04EC74 2605001C */  addiu $a1, $s0, 0x1c
/* 0837A8 7F04EC78 24070001 */  li    $a3, 1
/* 0837AC 7F04EC7C AFA30018 */  sw    $v1, 0x18($sp)
/* 0837B0 7F04EC80 0FC28423 */  jal   explosionCreateBulletImpact
/* 0837B4 7F04EC84 AFAE0014 */   sw    $t6, 0x14($sp)
/* 0837B8 7F04EC88 8FB80070 */  lw    $t8, 0x70($sp)
/* 0837BC 7F04EC8C 1000003D */  b     .L7F04ED84
/* 0837C0 7F04EC90 8F040018 */   lw    $a0, 0x18($t8)
/* 0837C4 7F04EC94 8602003A */  lh    $v0, 0x3a($s0)
.L7F04EC98:
/* 0837C8 7F04EC98 00004825 */  move  $t1, $zero
/* 0837CC 7F04EC9C 3C0F8005 */  lui   $t7, %hi(g_Textures)
/* 0837D0 7F04ECA0 04410004 */  bgez  $v0, .L7F04ECB4
/* 0837D4 7F04ECA4 0002C8C0 */   sll   $t9, $v0, 3
/* 0837D8 7F04ECA8 3C088005 */  lui   $t0, %hi(D_8004E86C)
/* 0837DC 7F04ECAC 10000008 */  b     .L7F04ECD0
/* 0837E0 7F04ECB0 8D08E86C */   lw    $t0, %lo(D_8004E86C)($t0)
.L7F04ECB4:
/* 0837E4 7F04ECB4 01F97821 */  addu  $t7, $t7, $t9
/* 0837E8 7F04ECB8 91EF9300 */  lbu   $t7, %lo(g_Textures)($t7)
/* 0837EC 7F04ECBC 3C088005 */  lui   $t0, %hi(D_8004E86C)
/* 0837F0 7F04ECC0 31EB000F */  andi  $t3, $t7, 0xf
/* 0837F4 7F04ECC4 000B6080 */  sll   $t4, $t3, 2
/* 0837F8 7F04ECC8 010C4021 */  addu  $t0, $t0, $t4
/* 0837FC 7F04ECCC 8D08E86C */  lw    $t0, %lo(D_8004E86C)($t0)
.L7F04ECD0:
/* 083800 7F04ECD0 AFA80050 */  sw    $t0, 0x50($sp)
/* 083804 7F04ECD4 0C002914 */  jal   randomGetNext
/* 083808 7F04ECD8 A3A9004B */   sb    $t1, 0x4b($sp)
/* 08380C 7F04ECDC 8FA80050 */  lw    $t0, 0x50($sp)
/* 083810 7F04ECE0 8E2E0014 */  lw    $t6, 0x14($s1)
/* 083814 7F04ECE4 3C188004 */  lui   $t8, %hi(skeleton_door)
/* 083818 7F04ECE8 850D000A */  lh    $t5, 0xa($t0)
/* 08381C 7F04ECEC 8DC30008 */  lw    $v1, 8($t6)
/* 083820 7F04ECF0 2718A1DC */  addiu $t8, %lo(skeleton_door) # addiu $t8, $t8, -0x5e24
/* 083824 7F04ECF4 004D001B */  divu  $zero, $v0, $t5
/* 083828 7F04ECF8 8C640004 */  lw    $a0, 4($v1)
/* 08382C 7F04ECFC 00005010 */  mfhi  $t2
/* 083830 7F04ED00 83A9004B */  lb    $t1, 0x4b($sp)
/* 083834 7F04ED04 15A00002 */  bnez  $t5, .L7F04ED10
/* 083838 7F04ED08 00000000 */   nop
/* 08383C 7F04ED0C 0007000D */  break 7
.L7F04ED10:
/* 083840 7F04ED10 2605001C */  addiu $a1, $s0, 0x1c
/* 083844 7F04ED14 17040005 */  bne   $t8, $a0, .L7F04ED2C
/* 083848 7F04ED18 24070001 */   li    $a3, 1
/* 08384C 7F04ED1C 8C6F0008 */  lw    $t7, 8($v1)
/* 083850 7F04ED20 8E190044 */  lw    $t9, 0x44($s0)
/* 083854 7F04ED24 8DEB000C */  lw    $t3, 0xc($t7)
/* 083858 7F04ED28 132B0009 */  beq   $t9, $t3, .L7F04ED50
.L7F04ED2C:
/* 08385C 7F04ED2C 3C0C8004 */   lui   $t4, %hi(skeleton_cctv)
/* 083860 7F04ED30 258CA05C */  addiu $t4, %lo(skeleton_cctv) # addiu $t4, $t4, -0x5fa4
/* 083864 7F04ED34 55840008 */  bnel  $t4, $a0, .L7F04ED58
/* 083868 7F04ED38 8D0F0004 */   lw    $t7, 4($t0)
/* 08386C 7F04ED3C 8C6E0008 */  lw    $t6, 8($v1)
/* 083870 7F04ED40 8E0D0044 */  lw    $t5, 0x44($s0)
/* 083874 7F04ED44 8DD80004 */  lw    $t8, 4($t6)
/* 083878 7F04ED48 55B80003 */  bnel  $t5, $t8, .L7F04ED58
/* 08387C 7F04ED4C 8D0F0004 */   lw    $t7, 4($t0)
.L7F04ED50:
/* 083880 7F04ED50 24090001 */  li    $t1, 1
/* 083884 7F04ED54 8D0F0004 */  lw    $t7, 4($t0)
.L7F04ED58:
/* 083888 7F04ED58 8E0B0004 */  lw    $t3, 4($s0)
/* 08388C 7F04ED5C 26040010 */  addiu $a0, $s0, 0x10
/* 083890 7F04ED60 01EAC821 */  addu  $t9, $t7, $t2
/* 083894 7F04ED64 93260000 */  lbu   $a2, ($t9)
/* 083898 7F04ED68 AFAB0010 */  sw    $t3, 0x10($sp)
/* 08389C 7F04ED6C 8E0C0040 */  lw    $t4, 0x40($s0)
/* 0838A0 7F04ED70 AFA90018 */  sw    $t1, 0x18($sp)
/* 0838A4 7F04ED74 0FC28423 */  jal   explosionCreateBulletImpact
/* 0838A8 7F04ED78 AFAC0014 */   sw    $t4, 0x14($sp)
/* 0838AC 7F04ED7C 8FAE0070 */  lw    $t6, 0x70($sp)
/* 0838B0 7F04ED80 8DC40018 */  lw    $a0, 0x18($t6)
.L7F04ED84:
/* 0838B4 7F04ED84 0FC177E1 */  jal   bondwalkItemGetDestructionAmount
/* 0838B8 7F04ED88 00000000 */   nop
/* 0838BC 7F04ED8C 92220003 */  lbu   $v0, 3($s1)
/* 0838C0 7F04ED90 2401000D */  li    $at, 13
/* 0838C4 7F04ED94 46000086 */  mov.s $f2, $f0
/* 0838C8 7F04ED98 14410005 */  bne   $v0, $at, .L7F04EDB0
/* 0838CC 7F04ED9C 3C018003 */   lui   $at, %hi(F_80030B24)
/* 0838D0 7F04EDA0 C4300B24 */  lwc1  $f16, %lo(F_80030B24)($at)
/* 0838D4 7F04EDA4 46100082 */  mul.s $f2, $f0, $f16
/* 0838D8 7F04EDA8 1000001B */  b     .L7F04EE18
/* 0838DC 7F04EDAC 00000000 */   nop
.L7F04EDB0:
/* 0838E0 7F04EDB0 24010006 */  li    $at, 6
/* 0838E4 7F04EDB4 14410018 */  bne   $v0, $at, .L7F04EE18
/* 0838E8 7F04EDB8 00000000 */   nop
/* 0838EC 7F04EDBC 8E2D0014 */  lw    $t5, 0x14($s1)
/* 0838F0 7F04EDC0 3C188004 */  lui   $t8, %hi(skeleton_cctv)
/* 0838F4 7F04EDC4 2718A05C */  addiu $t8, %lo(skeleton_cctv) # addiu $t8, $t8, -0x5fa4
/* 0838F8 7F04EDC8 8DA30008 */  lw    $v1, 8($t5)
/* 0838FC 7F04EDCC 8C6F0004 */  lw    $t7, 4($v1)
/* 083900 7F04EDD0 170F000D */  bne   $t8, $t7, .L7F04EE08
/* 083904 7F04EDD4 00000000 */   nop
/* 083908 7F04EDD8 8C6B0008 */  lw    $t3, 8($v1)
/* 08390C 7F04EDDC 8E190044 */  lw    $t9, 0x44($s0)
/* 083910 7F04EDE0 3C0142C8 */  li    $at, 0x42C80000 # 100.000000
/* 083914 7F04EDE4 8D6C0004 */  lw    $t4, 4($t3)
/* 083918 7F04EDE8 172C0007 */  bne   $t9, $t4, .L7F04EE08
/* 08391C 7F04EDEC 00000000 */   nop
/* 083920 7F04EDF0 44812000 */  mtc1  $at, $f4
/* 083924 7F04EDF4 02202025 */  move  $a0, $s1
/* 083928 7F04EDF8 46041082 */  mul.s $f2, $f2, $f4
/* 08392C 7F04EDFC 0FC13786 */  jal   sub_GAME_7F04DE18
/* 083930 7F04EE00 E7A20044 */   swc1  $f2, 0x44($sp)
/* 083934 7F04EE04 C7A20044 */  lwc1  $f2, 0x44($sp)
.L7F04EE08:
/* 083938 7F04EE08 3C018003 */  lui   $at, %hi(F_80030B18)
/* 08393C 7F04EE0C C4280B18 */  lwc1  $f8, %lo(F_80030B18)($at)
/* 083940 7F04EE10 46081082 */  mul.s $f2, $f2, $f8
/* 083944 7F04EE14 00000000 */  nop
.L7F04EE18:
/* 083948 7F04EE18 0FC26C54 */  jal   get_cur_playernum
/* 08394C 7F04EE1C E7A20044 */   swc1  $f2, 0x44($sp)
/* 083950 7F04EE20 C7A20044 */  lwc1  $f2, 0x44($sp)
/* 083954 7F04EE24 8FAE0070 */  lw    $t6, 0x70($sp)
/* 083958 7F04EE28 02202025 */  move  $a0, $s1
/* 08395C 7F04EE2C 44051000 */  mfc1  $a1, $f2
/* 083960 7F04EE30 8DC70018 */  lw    $a3, 0x18($t6)
/* 083964 7F04EE34 AFA20010 */  sw    $v0, 0x10($sp)
/* 083968 7F04EE38 0FC13833 */  jal   chrobjMaybeDetonateObjectIfFlags
/* 08396C 7F04EE3C 27A60060 */   addiu $a2, $sp, 0x60
/* 083970 7F04EE40 8E380014 */  lw    $t8, 0x14($s1)
/* 083974 7F04EE44 3C0D8004 */  lui   $t5, %hi(skeleton_door)
/* 083978 7F04EE48 25ADA1DC */  addiu $t5, %lo(skeleton_door) # addiu $t5, $t5, -0x5e24
/* 08397C 7F04EE4C 8F0F0008 */  lw    $t7, 8($t8)
/* 083980 7F04EE50 8DEB0004 */  lw    $t3, 4($t7)
/* 083984 7F04EE54 15AB000D */  bne   $t5, $t3, .L7F04EE8C
/* 083988 7F04EE58 00000000 */   nop
/* 08398C 7F04EE5C 8E19004C */  lw    $t9, 0x4c($s0)
/* 083990 7F04EE60 1720000A */  bnez  $t9, .L7F04EE8C
/* 083994 7F04EE64 00000000 */   nop
/* 083998 7F04EE68 822C00BD */  lb    $t4, 0xbd($s1)
/* 08399C 7F04EE6C 258E0001 */  addiu $t6, $t4, 1
/* 0839A0 7F04EE70 A22E00BD */  sb    $t6, 0xbd($s1)
/* 0839A4 7F04EE74 823800BD */  lb    $t8, 0xbd($s1)
/* 0839A8 7F04EE78 2B010003 */  slti  $at, $t8, 3
/* 0839AC 7F04EE7C 14200003 */  bnez  $at, .L7F04EE8C
/* 0839B0 7F04EE80 00000000 */   nop
/* 0839B4 7F04EE84 0FC1375A */  jal   sub_GAME_7F04DD68
/* 0839B8 7F04EE88 02202025 */   move  $a0, $s1
.L7F04EE8C:
/* 0839BC 7F04EE8C 0FC11080 */  jal   objDropRecursively
/* 0839C0 7F04EE90 8E040004 */   lw    $a0, 4($s0)
/* 0839C4 7F04EE94 00008025 */  move  $s0, $zero
/* 0839C8 7F04EE98 0FC13803 */  jal   objIsCollectable
/* 0839CC 7F04EE9C 02202025 */   move  $a0, $s1
/* 0839D0 7F04EEA0 50400008 */  beql  $v0, $zero, .L7F04EEC4
/* 0839D4 7F04EEA4 8E2B0008 */   lw    $t3, 8($s1)
/* 0839D8 7F04EEA8 8E2F0008 */  lw    $t7, 8($s1)
/* 0839DC 7F04EEAC 000F6A40 */  sll   $t5, $t7, 9
/* 0839E0 7F04EEB0 05A20009 */  bltzl $t5, .L7F04EED8
/* 0839E4 7F04EEB4 8E2C000C */   lw    $t4, 0xc($s1)
/* 0839E8 7F04EEB8 10000006 */  b     .L7F04EED4
/* 0839EC 7F04EEBC 24100001 */   li    $s0, 1
/* 0839F0 7F04EEC0 8E2B0008 */  lw    $t3, 8($s1)
.L7F04EEC4:
/* 0839F4 7F04EEC4 000BCA80 */  sll   $t9, $t3, 0xa
/* 0839F8 7F04EEC8 07230003 */  bgezl $t9, .L7F04EED8
/* 0839FC 7F04EECC 8E2C000C */   lw    $t4, 0xc($s1)
/* 083A00 7F04EED0 24100001 */  li    $s0, 1
.L7F04EED4:
/* 083A04 7F04EED4 8E2C000C */  lw    $t4, 0xc($s1)
.L7F04EED8:
/* 083A08 7F04EED8 318E0002 */  andi  $t6, $t4, 2
/* 083A0C 7F04EEDC 51C00007 */  beql  $t6, $zero, .L7F04EEFC
/* 083A10 7F04EEE0 8E38000C */   lw    $t8, 0xc($s1)
/* 083A14 7F04EEE4 0FC13BCD */  jal   objIsHealthy
/* 083A18 7F04EEE8 02202025 */   move  $a0, $s1
/* 083A1C 7F04EEEC 54400003 */  bnezl $v0, .L7F04EEFC
/* 083A20 7F04EEF0 8E38000C */   lw    $t8, 0xc($s1)
/* 083A24 7F04EEF4 24100001 */  li    $s0, 1
/* 083A28 7F04EEF8 8E38000C */  lw    $t8, 0xc($s1)
.L7F04EEFC:
/* 083A2C 7F04EEFC 8FA50070 */  lw    $a1, 0x70($sp)
/* 083A30 7F04EF00 330F0400 */  andi  $t7, $t8, 0x400
/* 083A34 7F04EF04 11E00002 */  beqz  $t7, .L7F04EF10
/* 083A38 7F04EF08 24A5000C */   addiu $a1, $a1, 0xc
/* 083A3C 7F04EF0C 00008025 */  move  $s0, $zero
.L7F04EF10:
/* 083A40 7F04EF10 52000004 */  beql  $s0, $zero, .L7F04EF24
/* 083A44 7F04EF14 8FBF002C */   lw    $ra, 0x2c($sp)
/* 083A48 7F04EF18 0FC12F37 */  jal   objBounce
/* 083A4C 7F04EF1C 02202025 */   move  $a0, $s1
/* 083A50 7F04EF20 8FBF002C */  lw    $ra, 0x2c($sp)
.L7F04EF24:
/* 083A54 7F04EF24 8FB00024 */  lw    $s0, 0x24($sp)
/* 083A58 7F04EF28 8FB10028 */  lw    $s1, 0x28($sp)
/* 083A5C 7F04EF2C 03E00008 */  jr    $ra
/* 083A60 7F04EF30 27BD0070 */   addiu $sp, $sp, 0x70
)
#endif





bool objIsHealthy(ObjectRecord *self) //#MATCH
{
    return objGetDestroyedLevel(self) == 0;
}


bool objTestForInteract(PropRecord* prop)
{
    f32 xdiff;
    ObjectRecord *obj;
    PropRecord *player;
    f32 var_f2;
    f32 ydiff;
    f32 zdiff;
    f32 var_f0;
    f32 anglediff;
    f32 playerangle;
    f32 sp30;
    StandTile *stan;
    f32 xzdiff;
    f32 angle;

    obj = prop->obj;

    if (((obj->type == PROP_TYPE_PLAYER)
            || (obj->flags & PROPFLAG_00080000)
            || (obj->runtime_bitflags & (RUNTIMEBITFLAG_00000001 | RUNTIMEBITFLAG_00000002 | RUNTIMEBITFLAG_TAGGED))))
    {
        if ((prop->flags & PROPFLAG_ONSCREEN)
                && (objIsHealthy(obj) != 0)
                && !(obj->flags & PROPFLAG_CANNOT_ACTIVATE))
        {

            player = get_curplayer_positiondata();

            xdiff = obj->runtime_pos.x - player->pos.x;
            ydiff = obj->runtime_pos.y - player->pos.y;
            zdiff = obj->runtime_pos.z - player->pos.z;

            stan = player->stan;

            if ((obj->type == 0x28) && (obj->flags & PROPFLAG_DOOR_OPENTOFRONT))
            {
                var_f0 = 400.0f;
                var_f2 = 160000.0f;
                sp30 = 2.0943952f;
            }
            else
            {
                var_f0 = 200.0f;
                var_f2 = 40000.0f;
                sp30 = 0.3926991f;
            }

            xzdiff = ((xdiff * xdiff) + (zdiff * zdiff));

            if ((xzdiff < var_f2) && (ydiff < var_f0) && (-var_f0 < ydiff))
            {

                angle = atan2f(xdiff, zdiff);
                playerangle = get_curplay_horizontal_rotation_in_degrees();
                anglediff = angle - playerangle;

                if (angle < playerangle)
                {
                    anglediff += M_TAU_F;
                }

                if (anglediff > M_PI_F)
                {
                    anglediff = M_TAU_F - anglediff;
                }

                if (anglediff <= sp30)
                {
                    if (!(obj->flags2 & PROPFLAG2_INTERACTCHECKLOS) || (walkTilesBetweenPoints_NoCallback(&stan, player->pos.x, player->pos.z, prop->pos.x, prop->pos.z) != 0))
                    {
                        g_InteractProp = prop;
                    }
                }
            }
        }
    }
    return TRUE;
}


/*
 * Return TYPE if Collected or Interacted (except for Alarm which always returns False)
 */
INV_ITEM_TYPE propobjInteract(PropRecord *prop) //#MATCH
{
    ObjectRecord *obj        = prop->obj;
    INV_ITEM_TYPE colllected = INV_ITEM_NONE;

    if (obj->type == PROPDEF_ALARM)
    {
        sndPlaySfx(g_musicSfxBufferPtr, 0xBA, 0);
        if (alarmIsActive())
        {
            alarmDeactivate();
        }
        else
        {
            alarmActivate();
        }
    }
    if (obj->flags & PROPFLAG_00080000)
    {
        colllected = collect_or_interact_object(prop, TRUE);
    }
    obj->runtime_bitflags |= RUNTIMEBITFLAG_ACTIVATED;
    sub_GAME_7F03E6A0(prop);
    return colllected;
}


void sub_GAME_7F04F218(PropRecord* prop, s32 arg1) {
    ChrRecord* chr;
    chr = prop->chr;

    if (arg1 != 0)
    {
        chr->accuracyrating = (u8) chr->accuracyrating & 0xFFDF;
    }
    else
    {
        chr->accuracyrating = (u8) chr->accuracyrating | 0x20;
    }
}


void sub_GAME_7F04F244(PropRecord* prop, rect4f** arg1, s32* arg2, f32* arg3, f32* arg4)
{
    ObjectRecord* obj;
    obj = prop->obj;

    if ((obj->ptr_allocated_collisiondata_block != NULL) && (obj->flags & PROPFLAG_00000100) && !(obj->state & PROPSTATE_20))
    {
        *arg2 = obj->ptr_allocated_collisiondata_block->unk00;
        *arg1 = &obj->ptr_allocated_collisiondata_block->unk04;
        *arg4 = obj->ptr_allocated_collisiondata_block->unk48;
        *arg3 = obj->ptr_allocated_collisiondata_block->unk44;
        return;
    }

    *arg2 = 0;
}


#ifdef __GNUC__
void append_text_picked_up(u8 *buffer, AMMOTYPE param2, u32 param3)
#else
void append_text_picked_up(u8 *buffer,u8 * param2,u8 * param3)
#endif
{
  u8 *str;

  str = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_00_PICKEDUP)); //Picked up
  strcat(buffer,str);
  return;
}





void append_text_ammo_amount_word(u8 *buffer, AMMOTYPE ammotype,u32 amount)
{
    u8 *textfiletext;

    switch(ammotype) {
    case AMMO_9MM:
    case AMMO_9MM_2:
    case AMMO_RIFLE:
    case AMMO_PLASTIQUE:
        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_01_SOME)); //some
        strcat(buffer,textfiletext);
        break;
    case AMMO_SHOTGUN:
    case AMMO_GRENADE:
    case AMMO_ROCKETS:
    case AMMO_REMOTEMINE:
    case AMMO_PROXMINE:
    case AMMO_TIMEDMINE:
    case AMMO_KNIFE:
    case AMMO_GRENADEROUND:
    case AMMO_MAGNUM:
    case AMMO_GGUN:
    case AMMO_DARTS:
    case AMMO_FLARE:
    case AMMO_PITON:
    case AMMO_DYNAMITE:
    case AMMO_BUG:
    case AMMO_MICRO_CAMERA:
        if (amount == 1) {
            textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_02_A)); //a
            strcat(buffer,textfiletext);
        }
        else {
            textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_01_SOME)); //some
            strcat(buffer,textfiletext);
        }
        break;
    case AMMO_EXPLOSIVEPEN:
    case AMMO_BOMBCASE:
        if (amount == 1) {
            textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_03_AN)); //an
            strcat(buffer,textfiletext);
        }
        else {
            textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_01_SOME)); //some
            strcat(buffer,textfiletext);
        }
        break;
    case AMMO_GEKEY:
    case AMMO_TOKEN:
        if (amount == 1) {
            textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_04_THE)); //the
            strcat(buffer,textfiletext);
        }
        else {
            textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_01_SOME)); //some
            strcat(buffer,textfiletext);
        }
    }
    return;
}


void apped_text_ammotype(u8 *buffer, AMMOTYPE ammotype, s32 amount)
{
    u8 *textfiletext;
    if (((ammotype == AMMO_9MM) || (ammotype == AMMO_9MM_2)) || (ammotype == AMMO_RIFLE))
    {
        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_05_AMMO)); //ammo
        strcat(buffer,textfiletext);
    }
    else
    {
        if (ammotype == AMMO_KNIFE)
        {
            textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_0F_THROWING)); //throwing
            strcat(buffer,textfiletext);
            if (amount == 1)
            {
                textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_10_KNIFE)); //knife
                strcat(buffer,textfiletext);
            }
            else
            {
                textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_11_KNIVES)); //knives
                strcat(buffer,textfiletext);
            }
        }
        else
        {
            if (ammotype == AMMO_DYNAMITE)
            {
                if (amount == 1)
                {
                    textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_19_STICK)); //stick
                    strcat(buffer,textfiletext);
                }
                else
                {
                    textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_1A_STICKS)); //sticks
                    strcat(buffer,textfiletext);
                }
                textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_18_OFDYNAMITE)); //of dynamite
                strcat(buffer,textfiletext);
            }
            else
            {
                switch(ammotype)
                {
                    case AMMO_SHOTGUN:
                        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_06_SHOTGUNCARTRIDGE)); //shotgun cartridge
                        strcat(buffer,textfiletext);
                        break;
                    case AMMO_MAGNUM:
                        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_07_MAGNUMBULLET)); //magnum bullet
                        strcat(buffer,textfiletext);
                        break;
                    case AMMO_GGUN:
                        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_08_GOLDENBULLET)); //golden bullet
                        strcat(buffer,textfiletext);
                        break;
                    case AMMO_GRENADE:
                        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_09_HANDGRENADE)); //hand grenade
                        strcat(buffer,textfiletext);
                        break;
                    case AMMO_GRENADEROUND:
                        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_0A_GRENADEROUND)); //grenade round
                        strcat(buffer,textfiletext);
                        break;
                    case AMMO_ROCKETS:
                        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_0B_ROCKET)); //rocket
                        strcat(buffer,textfiletext);
                        break;
                    case AMMO_REMOTEMINE:
                        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_0C_REMOTEMINE)); //remote mine
                        strcat(buffer,textfiletext);
                        break;
                    case AMMO_PROXMINE:
                        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_0D_PROXIMITYMINE)); //proximity mine
                        strcat(buffer,textfiletext);
                        break;
                    case AMMO_TIMEDMINE:
                        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_0E_TIMEDMINE)); //timed mine
                        strcat(buffer,textfiletext);
                        break;
                    case AMMO_DARTS:
                        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_13_DART)); //dart
                        strcat(buffer,textfiletext);
                        break;
                    case AMMO_EXPLOSIVEPEN:
                        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_14_EXPLOSIVEPEN)); //explosive pen
                        strcat(buffer,textfiletext);
                        break;
                    case AMMO_BOMBCASE:
                        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_15_EXPLOSIVECASE)); //explosive case
                        strcat(buffer,textfiletext);
                        break;
                    case AMMO_FLARE:
                        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_16_FLARE)); //flare
                        strcat(buffer,textfiletext);
                        break;
                    case AMMO_PITON:
                        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_17_PITON)); //piton
                        strcat(buffer,textfiletext);
                        break;
                    case AMMO_BUG:
                        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_1B_BUG)); //bug
                        strcat(buffer,textfiletext);
                        break;
                    case AMMO_MICRO_CAMERA:
                        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_1C_MICROCAMERA)); //micro camera
                        strcat(buffer,textfiletext);
                        break;
                    case AMMO_GEKEY:
                        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_1D_GOLDENEYEKEY)); //GoldenEye key
                        strcat(buffer,textfiletext);
                        break;
                    case AMMO_TOKEN:
                        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_1E_TOKEN)); //token
                        strcat(buffer,textfiletext);
                        break;
                    case AMMO_PLASTIQUE:
                        textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_1F_PLASTIQUE)); //plastique
                        strcat(buffer,textfiletext);
                        break;
                }
                if (1 < amount)
                {
                    textfiletext = langGet(getStringID(LPROPOBJ,PROPOBJ_STR_12_S)); //s
                    strcat(buffer,textfiletext);
                }
            }
        }
    }
}


void set_sound_effect_for_ammo_collection(AMMOTYPE ammotype)
{
    switch(ammotype) {
        case AMMO_9MM:
        case AMMO_9MM_2:
        case AMMO_RIFLE:
        case AMMO_SHOTGUN:
        case AMMO_GRENADE:
        case AMMO_ROCKETS:
        case AMMO_GRENADEROUND:
        case AMMO_MAGNUM:
        case AMMO_GGUN:
        case AMMO_DARTS:
        case AMMO_EXPLOSIVEPEN:
        case AMMO_FLARE:
        case AMMO_PITON:
        case AMMO_DYNAMITE:
        case AMMO_GEKEY:
        case AMMO_TOKEN:
            sndPlaySfx(g_musicSfxBufferPtr,PICKUP_AMMO_SFX,0);
            break;
        case AMMO_REMOTEMINE:
        case AMMO_PROXMINE:
        case AMMO_TIMEDMINE:
        case AMMO_BOMBCASE:
        case AMMO_BUG:
        case AMMO_MICRO_CAMERA:
        case AMMO_PLASTIQUE:
            sndPlaySfx(g_musicSfxBufferPtr,PICKUP_MINE_SFX,0);
            break;
        case AMMO_KNIFE:
            sndPlaySfx(g_musicSfxBufferPtr,PICKUP_KNIFE_SFX,0);
    }
}


void set_sound_effect_for_weapontype_collection(ITEM_IDS weapontype)
{
    if ((weapontype == ITEM_KNIFE) || (weapontype == ITEM_THROWKNIFE))
    {
        sndPlaySfx(g_musicSfxBufferPtr,PICKUP_KNIFE_SFX,0);
    }
    else
    {
        if ((weapontype == ITEM_REMOTEMINE) || (weapontype == ITEM_PROXIMITYMINE) || (weapontype == ITEM_TIMEDMINE) ||
            (weapontype == ITEM_BOMBCASE) || (weapontype == ITEM_BUG) || (weapontype == ITEM_MICROCAMERA) ||
            (weapontype == ITEM_PLASTIQUE))
        {
            sndPlaySfx(g_musicSfxBufferPtr,PICKUP_MINE_SFX,0);
        }
        else
        {
            if ((weapontype == ITEM_GRENADE) || (weapontype == ITEM_NULL87) || (weapontype == ITEM_NULL86))
            {
                sndPlaySfx(g_musicSfxBufferPtr,PICKUP_AMMO_SFX,0);
            }
            else
            {
                if (weapontype == ITEM_LASER)
                {
                    sndPlaySfx(g_musicSfxBufferPtr,PICKUP_LASER_SFX,0);
                }
                else
                {
                    sndPlaySfx(g_musicSfxBufferPtr,PICKUP_GUN_SFX,0);
                }
            }
        }
    }
}


//!FIXME, i need to be properly split from chrai.c
void prepare_ammo_type_collection_text(u8 *finaltext, AMMOTYPE ammotype, u32 quantity)
{
    *finaltext = 0;
    if (j_text_trigger != 0)
    {
        apped_text_ammotype(finaltext,ammotype,quantity);
        if (getPlayerCount() < 3)
        {
            append_text_picked_up(finaltext, ammotype, quantity);
        }
        strcat(finaltext, "\n");
        return;
    }
    if (getPlayerCount() < 3)
    {
        append_text_picked_up(finaltext, ammotype, quantity);
        append_text_ammo_amount_word(finaltext, ammotype, quantity);
    }
    apped_text_ammotype(finaltext, ammotype, quantity);
    strcat(finaltext, ".\n");
}


void display_text_when_ammo_collected(s32 ammotype, s32 quantity)
{
    char buffer[100] = "";
    prepare_ammo_type_collection_text(buffer, ammotype, quantity);
#ifdef VERSION_US
    hudmsgBottomShow(buffer);
#else
    jp_hudmsgBottomShow(buffer);
#endif
}

void add_ammo_to_inventory(AMMOTYPE ammotype,int amount,int doplaysound,int dodisplaytext)
{
    int curammo;
    int maxammo;

    if (0 < amount)
    {
        curammo = check_cur_player_ammo_amount_in_inventory(ammotype);
        maxammo = get_max_ammo_for_type(ammotype);
        if (curammo < maxammo)
        {
            curammo = check_cur_player_ammo_amount_in_inventory(ammotype);
            give_cur_player_ammo(ammotype,curammo + amount);
#if defined(BUGFIX_R1)
        }
#endif
            if (dodisplaytext != 0)
            {
                display_text_when_ammo_collected(ammotype,amount);
            }

            if (doplaysound != 0)
            {
                set_sound_effect_for_ammo_collection(ammotype);
            }

            if (ammotype == AMMO_GRENADE)
            {
                bondinvAddInvItem(ITEM_GRENADE);
            }
            else if (ammotype == AMMO_REMOTEMINE)
            {
                bondinvAddInvItem(ITEM_REMOTEMINE);
                bondinvAddInvItem(ITEM_TRIGGER);
            }
            else if (ammotype == AMMO_PROXMINE)
            {
                bondinvAddInvItem(ITEM_PROXIMITYMINE);
            }
            else if (ammotype == AMMO_TIMEDMINE)
            {
                bondinvAddInvItem(ITEM_TIMEDMINE);
            }
            else if (ammotype == AMMO_KNIFE)
            {
                bondinvAddInvItem(ITEM_THROWKNIFE);
            }
            else if (ammotype == AMMO_BOMBCASE)
            {
                bondinvAddInvItem(ITEM_BOMBCASE);
            }
            else if (ammotype == AMMO_BUG)
            {
                bondinvAddInvItem(ITEM_BUG);
            }
            else if (ammotype == AMMO_MICRO_CAMERA)
            {
                bondinvAddInvItem(ITEM_MICROCAMERA);
            }
            else if (ammotype == AMMO_GEKEY)
            {
                bondinvAddInvItem(ITEM_GOLDENEYEKEY);
            }
            else if (ammotype == AMMO_TOKEN)
            {
                bondinvAddInvItem(ITEM_TOKEN);
            }
            else if (ammotype == AMMO_PLASTIQUE)
            {
                bondinvAddInvItem(ITEM_PLASTIQUE);
            }
#if !defined(BUGFIX_R1)
        }
#endif
    }
}


s32 get_ammo_in_magazine(AmmoCrateRecord *crate)
{
    s32 qty = 1;

    switch (crate->ammoType)
    {
        case AMMO_9MM:     qty = 10; break;
        case AMMO_9MM_2:   qty = 10; break;
        case AMMO_RIFLE:   qty = 10; break;
        case AMMO_SHOTGUN: qty =  5; break;
        case AMMO_MAGNUM:  qty =  5; break;
        case AMMO_GGUN:    qty =  3; break;
        case AMMO_DARTS:   qty =  4; break;
    }

    if (qty > 1 && getPlayerCount() == 1)
    {
        qty *= g_SoloAmmoMultiplier;
    }

    return qty;
}

s32 ammo_collected_from_weapon(WeaponObjRecord *weapon)
{
    s32 ammotype;
    s32 qty;

    ammotype = get_ammo_type_for_weapon(weapon->weaponnum);
    qty = 1;

    if (weapon->flags & PROPFLAG_NO_AMMO)
    {
        return 0;
    }

    switch (ammotype)
    {
        case AMMO_9MM:          qty = 10; break;
        case AMMO_9MM_2:        qty = 10; break;
        case AMMO_RIFLE:        qty = 10; break;
        case AMMO_SHOTGUN:      qty =  5; break;
        case AMMO_MAGNUM:       qty =  5; break;
        case AMMO_GGUN:         qty =  3; break;
        case AMMO_DARTS:        qty =  4; break;
        case AMMO_GRENADEROUND: qty =  3; break;
    }

    if (qty > 1 && getPlayerCount() == 1)
    {
        qty *= g_SoloAmmoMultiplier;
    }

    return qty;
}


void generate_language_specific_text_for_weapon(u8 *finalstring, ITEM_IDS itemtype)
{
    u32 morethan2players;

    morethan2players = FALSE;

    if (j_text_trigger != 0)
    {
          strcpy(finalstring,"");
          if (2 < getPlayerCount())
          {
              morethan2players = TRUE;
          }
    }
    else
    {
          if (getPlayerCount() < 3)
          {
             //Picked up
            strcpy(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_00_PICKEDUP)));
          }
    }

    switch(itemtype)
    {
        case ITEM_THROWKNIFE:
        case ITEM_GRENADE:
        case ITEM_TIMEDMINE:
        case ITEM_PROXIMITYMINE:
        case ITEM_REMOTEMINE:
        case ITEM_BOMBCASE:
        case ITEM_PLASTIQUE:
        case ITEM_BUG:
        case ITEM_MICROCAMERA:
        case ITEM_GOLDENEYEKEY:
        case ITEM_NULL86:
        case ITEM_NULL87:
        case ITEM_TOKEN:
            prepare_ammo_type_collection_text(finalstring,get_ammo_type_for_weapon(itemtype),1);
            return;
        case ITEM_KNIFE:
            //a hunting knife.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_20_AHUNTINGKNIFE)));
            break;
        case ITEM_WPPK:
            //a PP7.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_21_APPK)));
            break;
        case ITEM_WPPKSIL:
            //a silenced PP7.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_22_ASILENCEDPPK)));
            break;
        case ITEM_TT33:
            //a DD44 Dostovei.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_23_ATT33)));
            break;
        case ITEM_SKORPION:
            //a Klobb.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_24_ASPKORPION)));
            break;
        case ITEM_AK47:
            //a KF7 Soviet.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_25_ANAK47)));
            break;
        case ITEM_UZI:
            //a ZMG (9mm).
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_26_ANUZI)));
            break;
        case ITEM_MP5K:
            //a D5K Deutsche.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_27_ANMP5K)));
            break;
        case ITEM_MP5KSIL:
            //a silenced D5K.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_28_ASILENCEDMP5)));
            break;
        case ITEM_SPECTRE:
            //a Phantom.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_29_ASPECTRE)));
            break;
        case ITEM_M16:
            //an AR33 assault rifle.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_2A_ANM16)));
            break;
        case ITEM_FNP90:
            //an RC-P90.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_2B_ANFNP90)));
            break;
        case ITEM_SHOTGUN:
            //a shotgun.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_2C_ASHOTGUN)));
            break;
        case ITEM_AUTOSHOT:
            //an automatic shotgun.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_2D_ANAUTOSHOTGUN)));
            break;
        case ITEM_SNIPERRIFLE:
            //a sniper rifle.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_2E_ASNIPERRIFLE)));
            break;
        case ITEM_GRENADELAUNCH:
            //a grenade launcher.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_2F_AGRENADELAUNCHER)));
            break;
        case ITEM_ROCKETLAUNCH:
            //a rocket launcher.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_30_AROCKETLAUNCHER)));
            break;
        case ITEM_RUGER:
            //a Cougar Magnum.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_31_ARUGERMAGNUM)));
            break;
        case ITEM_GOLDENGUN:
            //the Golden Gun.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_32_THEGOLDENGUN)));
            break;
        case ITEM_LASER:
            //a Moonraker laser.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_33_AMOOKRAKERLASER)));
            break;
        case ITEM_FLAREPISTOL:
            //a flare pistol.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_34_AFLAREPISTOL)));
            break;
        case ITEM_PITONGUN:
            //a piton gun.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_35_APITONGUN)));
            break;
        case ITEM_SILVERWPPK:
            //a silver PP7.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_36_ASILVERPPK)));
            break;
        case ITEM_GOLDWPPK:
            //a gold PP7.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_37_AGOLDPPK)));
            break;
        case ITEM_KEYCARD:
            //a keycard.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_38_AKEYCARD)));
            break;
        case ITEM_KEYYALE:
            //a yale key.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_39_AYALEKEY)));
            break;
        case ITEM_KEYBOLT:
            //a bolt key.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_3A_ABOLTKEY)));
            break;
        default:
            //a new weapon.
            strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_3B_ANEWWEAPON)));
            break;
    }

    if ((j_text_trigger != 0) && (!morethan2players))
    {
        if (finalstring[strlen(finalstring) - 1] == '\n')
        {
            finalstring[strlen(finalstring) - 1] = '\0';
        }
        //Picked up
        strcat(finalstring, langGet(getStringID(LPROPOBJ,PROPOBJ_STR_00_PICKEDUP)));
        strcat(finalstring,"\n");
    }

}


void display_text_for_weapon_in_lower_left_corner(ITEM_IDS weaponid)
{
    char acStack100 [100];

    generate_language_specific_text_for_weapon(acStack100,weaponid);
    HUDMESSAGEBOTTOM(acStack100);
    return;
}






#ifdef NONMATCHING
// https://decomp.me/scratch/nJWA0
INV_ITEM_TYPE collect_or_interact_object(PropRecord *prop, bool showstring) //#50% - needs some love, but compiles and looks similar
{
    ObjectRecord *propobj;
    INV_ITEM_TYPE collectType;

    propobj = prop->obj;
    if ((pPlayer->bonddead) || (!gclock_timer))
    {
        return INV_ITEM_NONE;
    }

    switch (propobj->type)
    {
        case PROPDEF_KEY:
        {
            sndPlaySfx(g_musicSfxBufferPtr, 0xE5, 0);
            if (showstring)
            {
                char *text = bondinvGetActivatedTextObject(propobj);
                if (!text)
                {
                    text = langGet(0xA43C);
                }
                hudmsgBottomShow(text);
            }
            collectType = INV_ITEM_PICKUP;
            break;
        }
        case PROPDEF_MAGAZINE:
        {
            AmmoCrateRecord *mag = propobj;
            add_ammo_to_inventory(mag->type, get_ammo_in_magazine(mag), 1, showstring);
            collectType = INV_ITEM_WEAPON;
            break;
        }
        case PROPDEF_AMMO:
        {
            MultiAmmoCrateRecord *ammo = propobj;
            s32                   i, ammoquantity;
            for (i = 0; i < AMMOTYPE_GLOBAL_MAX; i++)
            {
                ammoquantity = ammo->quantities[i];

                if (getPlayerCount() == 1)
                {
                    ammoquantity = ammoquantity * solo_ammo_multiplier;
                }

                add_ammo_to_inventory(i, ammoquantity, 0, showstring);
            }
            sndPlaySfx(g_musicSfxBufferPtr, 0xEA, 0);
            collectType = INV_ITEM_WEAPON;
            break;
        }
        case PROPDEF_COLLECTABLE:
        {
            bool             pass = 0;
            WeaponObjRecord *wep  = propobj;
            char *           text, *text2;
            int              ammocollected;
            set_sound_effect_for_weapontype_collection(wep->weaponnum);

            if (wep->weaponnum == ITEM_REMOTEMINE)
            {
                bondinvAddInvItem(ITEM_TRIGGER);
            }
            else if (wep->weaponnum == ITEM_TOKEN)
            {
                currentPlayerEquipWeaponWrapper(GUNRIGHT, ITEM_TOKEN);
            }
            if ((wep->runtime_bitflags & RUNTIMEBITFLAG_DESTROYED))
            {
                if (wep->weaponnum < ITEM_BOMBCASE)
                {
                    sub_GAME_7F08C764(prop);
                }
                if (showstring)
                {
                    text = bondinvGetActivatedTextObject(propobj);
                    if (text)
                    {
                        hudmsgBottomShow(text);
                    }
                    else
                    {
                        display_text_for_weapon_in_lower_left_corner(wep->weaponnum);
                    }
                    pass = 1;
                }
                collectType = INV_ITEM_PICKUP;
            }
            else
            {
                if (sub_GAME_7F08C764(prop) != 0)
                {
                    pass = 1;
                }
                if (showstring)
                {
                    text2 = bondinvGetActivatedTextWeapon(wep->weaponnum);

                    if (text2 != 0)
                    {
                        pass = 1;
                        hudmsgBottomShow(text2);
                    }
                    else if (pass != 0)
                    {
                        display_text_for_weapon_in_lower_left_corner(wep->weaponnum);
                    }
                }
                collectType = INV_ITEM_WEAPON;
            }
            text = get_ammo_type_for_weapon(wep->weaponnum);
            if (text != 0)
            {
                ammocollected = ammo_collected_from_weapon(wep);

                if (ammocollected > 0)
                {
                    if (check_cur_player_ammo_amount_in_inventory(text) < get_max_ammo_for_type(text))
                    {
                        give_cur_player_ammo(text, check_cur_player_ammo_amount_in_inventory(text) + ammocollected);
                        if ((pass == 0) && showstring)
                        {
                            display_text_when_ammo_collected(text, ammocollected);
                        }
                    }
                }
            }
            break;
        }
        case PROPDEF_ARMOUR:
        {
            BodyArmourRecord *armour = propobj;
            char *            text;
            //add_BONDdata_watch_armor(armour->initialamount);
            sndPlaySfx(g_musicSfxBufferPtr, 0x51, 0);
            if (showstring)
            {
                text = bondinvGetActivatedTextObject(armour);

                if (text == 0)
                {
                    if (getPlayerCount() < 3)
                    {
                        text = langGet(0xA43D);
                    }
                    else
                    {
                        text = langGet(0xA43E);
                    }
                }
                hudmsgBottomShow(text);
            }
            collectType = INV_ITEM_WEAPON;
            break;
        }
        default:
        {
            char *text;
            sndPlaySfx(g_musicSfxBufferPtr, 0xE5, 0);
            if (showstring)
            {
                text = bondinvGetActivatedTextObject(propobj);
                if (text == 0)
                {
                    text = langGet(0xA43F);
                }
                hudmsgBottomShow(text);
            }
            collectType = INV_ITEM_PICKUP;
            break;
        }
    }
    if ((collectType == 1) && ((propobj->runtime_bitflags & 0x10) == 0))
    {
        objFree(propobj, 0, propobj->state & 4);
        return INV_ITEM_WEAPON;
    }
    if (collectType != 0)
    {
        bondinvAddPropToInv(prop);
        return INV_ITEM_PICKUP;
    }
    return INV_ITEM_NONE; //inventory(4) or ammo(1) or nothing(0)
}
#else
#ifdef VERSION_US
GLOBAL_ASM(
.late_rodata
/*D:8005322C*/
glabel object_interaction_table
.word interact_default_object
.word interact_key_object
.word interact_default_object
.word interact_default_object
.word interact_magazine_object
.word interact_weapon_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_ammobox_object
.word interact_bodyarmor_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object

.text
glabel collect_or_interact_object
/* 084D80 7F050250 27BDFF90 */  addiu $sp, $sp, -0x70
/* 084D84 7F050254 AFBF001C */  sw    $ra, 0x1c($sp)
/* 084D88 7F050258 AFB00018 */  sw    $s0, 0x18($sp)
/* 084D8C 7F05025C AFA40070 */  sw    $a0, 0x70($sp)
/* 084D90 7F050260 AFA50074 */  sw    $a1, 0x74($sp)
/* 084D94 7F050264 8C830004 */  lw    $v1, 4($a0)
/* 084D98 7F050268 3C0F8008 */  lui   $t7, %hi(g_CurrentPlayer)
/* 084D9C 7F05026C 8DEFA0B0 */  lw    $t7, %lo(g_CurrentPlayer)($t7)
/* 084DA0 7F050270 AFA3006C */  sw    $v1, 0x6c($sp)
/* 084DA4 7F050274 3C198005 */  lui   $t9, %hi(g_ClockTimer)
/* 084DA8 7F050278 8DF800D8 */  lw    $t8, 0xd8($t7)
/* 084DAC 7F05027C 17000005 */  bnez  $t8, .L7F050294
/* 084DB0 7F050280 00000000 */   nop
/* 084DB4 7F050284 8F398374 */  lw    $t9, %lo(g_ClockTimer)($t9)
/* 084DB8 7F050288 8FA8006C */  lw    $t0, 0x6c($sp)
/* 084DBC 7F05028C 57200004 */  bnezl $t9, .L7F0502A0
/* 084DC0 7F050290 91090003 */   lbu   $t1, 3($t0)
.L7F050294:
/* 084DC4 7F050294 1000010C */  b     .L7F0506C8
/* 084DC8 7F050298 00001025 */   move  $v0, $zero
/* 084DCC 7F05029C 91090003 */  lbu   $t1, 3($t0)
.L7F0502A0:
/* 084DD0 7F0502A0 252AFFFD */  addiu $t2, $t1, -3
/* 084DD4 7F0502A4 2D41002D */  sltiu $at, $t2, 0x2d
/* 084DD8 7F0502A8 102000E0 */  beqz  $at, .L7F05062C
/* 084DDC 7F0502AC 000A5080 */   sll   $t2, $t2, 2
/* 084DE0 7F0502B0 3C018005 */  lui   $at, %hi(object_interaction_table)
/* 084DE4 7F0502B4 002A0821 */  addu  $at, $at, $t2
/* 084DE8 7F0502B8 8C2A322C */  lw    $t2, %lo(object_interaction_table)($at)
/* 084DEC 7F0502BC 01400008 */  jr    $t2
/* 084DF0 7F0502C0 00000000 */   nop
interact_key_object:
/* 084DF4 7F0502C4 3C048006 */  lui   $a0, %hi(g_musicSfxBufferPtr)
/* 084DF8 7F0502C8 8C843720 */  lw    $a0, %lo(g_musicSfxBufferPtr)($a0)
/* 084DFC 7F0502CC 240500E5 */  li    $a1, 229
/* 084E00 7F0502D0 0C002382 */  jal   sndPlaySfx
/* 084E04 7F0502D4 00003025 */   move  $a2, $zero
/* 084E08 7F0502D8 8FAB0074 */  lw    $t3, 0x74($sp)
/* 084E0C 7F0502DC 1160000A */  beqz  $t3, .L7F050308
/* 084E10 7F0502E0 00000000 */   nop
/* 084E14 7F0502E4 0FC23657 */  jal   bondinvGetActivatedTextObject
/* 084E18 7F0502E8 8FA4006C */   lw    $a0, 0x6c($sp)
/* 084E1C 7F0502EC 14400004 */  bnez  $v0, .L7F050300
/* 084E20 7F0502F0 00402025 */   move  $a0, $v0
/* 084E24 7F0502F4 0FC30776 */  jal   langGet
/* 084E28 7F0502F8 3404A43C */   li    $a0, 42044
/* 084E2C 7F0502FC 00402025 */  move  $a0, $v0
.L7F050300:
/* 084E30 7F050300 0FC228F2 */  jal   hudmsgBottomShow
/* 084E34 7F050304 00000000 */   nop
.L7F050308:
/* 084E38 7F050308 100000DA */  b     .L7F050674
/* 084E3C 7F05030C 24030004 */   li    $v1, 4
interact_magazine_object:
/* 084E40 7F050310 00608025 */  move  $s0, $v1
/* 084E44 7F050314 0FC13F0F */  jal   get_ammo_in_magazine
/* 084E48 7F050318 00602025 */   move  $a0, $v1
/* 084E4C 7F05031C 8E040080 */  lw    $a0, 0x80($s0)
/* 084E50 7F050320 00402825 */  move  $a1, $v0
/* 084E54 7F050324 24060001 */  li    $a2, 1
/* 084E58 7F050328 0FC13E9A */  jal   add_ammo_to_inventory
/* 084E5C 7F05032C 8FA70074 */   lw    $a3, 0x74($sp)
/* 084E60 7F050330 100000D0 */  b     .L7F050674
/* 084E64 7F050334 24030001 */   li    $v1, 1
interact_ammobox_object:
/* 084E68 7F050338 00001025 */  move  $v0, $zero
/* 084E6C 7F05033C 00608025 */  move  $s0, $v1
/* 084E70 7F050340 24430001 */  addiu $v1, $v0, 1
.L7F050344:
/* 084E74 7F050344 24010001 */  li    $at, 1
/* 084E78 7F050348 14410002 */  bne   $v0, $at, .L7F050354
/* 084E7C 7F05034C 00602025 */   move  $a0, $v1
/* 084E80 7F050350 24040001 */  li    $a0, 1
.L7F050354:
/* 084E84 7F050354 96050082 */  lhu   $a1, 0x82($s0)
/* 084E88 7F050358 AFA4004C */  sw    $a0, 0x4c($sp)
/* 084E8C 7F05035C AFA30020 */  sw    $v1, 0x20($sp)
/* 084E90 7F050360 0FC26919 */  jal   getPlayerCount
/* 084E94 7F050364 AFA50050 */   sw    $a1, 0x50($sp)
/* 084E98 7F050368 24010001 */  li    $at, 1
/* 084E9C 7F05036C 8FA30020 */  lw    $v1, 0x20($sp)
/* 084EA0 7F050370 8FA4004C */  lw    $a0, 0x4c($sp)
/* 084EA4 7F050374 14410009 */  bne   $v0, $at, .L7F05039C
/* 084EA8 7F050378 8FA50050 */   lw    $a1, 0x50($sp)
/* 084EAC 7F05037C 44852000 */  mtc1  $a1, $f4
/* 084EB0 7F050380 3C018003 */  lui   $at, %hi(g_SoloAmmoMultiplier)
/* 084EB4 7F050384 C4280B28 */  lwc1  $f8, %lo(g_SoloAmmoMultiplier)($at)
/* 084EB8 7F050388 468021A0 */  cvt.s.w $f6, $f4
/* 084EBC 7F05038C 46083282 */  mul.s $f10, $f6, $f8
/* 084EC0 7F050390 4600540D */  trunc.w.s $f16, $f10
/* 084EC4 7F050394 44058000 */  mfc1  $a1, $f16
/* 084EC8 7F050398 00000000 */  nop
.L7F05039C:
/* 084ECC 7F05039C 00003025 */  move  $a2, $zero
/* 084ED0 7F0503A0 8FA70074 */  lw    $a3, 0x74($sp)
/* 084ED4 7F0503A4 0FC13E9A */  jal   add_ammo_to_inventory
/* 084ED8 7F0503A8 AFA30020 */   sw    $v1, 0x20($sp)
/* 084EDC 7F0503AC 8FA20020 */  lw    $v0, 0x20($sp)
/* 084EE0 7F0503B0 2401000D */  li    $at, 13
/* 084EE4 7F0503B4 26100004 */  addiu $s0, $s0, 4
/* 084EE8 7F0503B8 5441FFE2 */  bnel  $v0, $at, .L7F050344
/* 084EEC 7F0503BC 24430001 */   addiu $v1, $v0, 1
/* 084EF0 7F0503C0 3C048006 */  lui   $a0, %hi(g_musicSfxBufferPtr)
/* 084EF4 7F0503C4 8C843720 */  lw    $a0, %lo(g_musicSfxBufferPtr)($a0)
/* 084EF8 7F0503C8 240500EA */  li    $a1, 234
/* 084EFC 7F0503CC 0C002382 */  jal   sndPlaySfx
/* 084F00 7F0503D0 00003025 */   move  $a2, $zero
/* 084F04 7F0503D4 100000A7 */  b     .L7F050674
/* 084F08 7F0503D8 24030001 */   li    $v1, 1
interact_weapon_object:
/* 084F0C 7F0503DC AFA00044 */  sw    $zero, 0x44($sp)
/* 084F10 7F0503E0 80640080 */  lb    $a0, 0x80($v1)
/* 084F14 7F0503E4 0FC13E04 */  jal   set_sound_effect_for_weapontype_collection
/* 084F18 7F0503E8 AFA30048 */   sw    $v1, 0x48($sp)
/* 084F1C 7F0503EC 8FAD0048 */  lw    $t5, 0x48($sp)
/* 084F20 7F0503F0 2401001D */  li    $at, 29
/* 084F24 7F0503F4 81A40080 */  lb    $a0, 0x80($t5)
/* 084F28 7F0503F8 54810006 */  bnel  $a0, $at, .L7F050414
/* 084F2C 7F0503FC 24010058 */   li    $at, 88
/* 084F30 7F050400 0FC23122 */  jal   bondinvAddInvItem
/* 084F34 7F050404 2404001E */   li    $a0, 30
/* 084F38 7F050408 10000007 */  b     .L7F050428
/* 084F3C 7F05040C 8FAE006C */   lw    $t6, 0x6c($sp)
/* 084F40 7F050410 24010058 */  li    $at, 88
.L7F050414:
/* 084F44 7F050414 14810003 */  bne   $a0, $at, .L7F050424
/* 084F48 7F050418 24050058 */   li    $a1, 88
/* 084F4C 7F05041C 0FC17645 */  jal   currentPlayerEquipWeaponWrapper
/* 084F50 7F050420 00002025 */   move  $a0, $zero
.L7F050424:
/* 084F54 7F050424 8FAE006C */  lw    $t6, 0x6c($sp)
.L7F050428:
/* 084F58 7F050428 8FB90048 */  lw    $t9, 0x48($sp)
/* 084F5C 7F05042C 8DCF0064 */  lw    $t7, 0x64($t6)
/* 084F60 7F050430 31F80400 */  andi  $t8, $t7, 0x400
/* 084F64 7F050434 1300001B */  beqz  $t8, .L7F0504A4
/* 084F68 7F050438 00000000 */   nop
/* 084F6C 7F05043C 83280080 */  lb    $t0, 0x80($t9)
/* 084F70 7F050440 29010021 */  slti  $at, $t0, 0x21
/* 084F74 7F050444 50200004 */  beql  $at, $zero, .L7F050458
/* 084F78 7F050448 8FA90074 */   lw    $t1, 0x74($sp)
/* 084F7C 7F05044C 0FC231D9 */  jal   bondinvAddWeaponByProp
/* 084F80 7F050450 8FA40070 */   lw    $a0, 0x70($sp)
/* 084F84 7F050454 8FA90074 */  lw    $t1, 0x74($sp)
.L7F050458:
/* 084F88 7F050458 5120000F */  beql  $t1, $zero, .L7F050498
/* 084F8C 7F05045C 8FAC0048 */   lw    $t4, 0x48($sp)
/* 084F90 7F050460 0FC23657 */  jal   bondinvGetActivatedTextObject
/* 084F94 7F050464 8FA4006C */   lw    $a0, 0x6c($sp)
/* 084F98 7F050468 10400005 */  beqz  $v0, .L7F050480
/* 084F9C 7F05046C 00402025 */   move  $a0, $v0
/* 084FA0 7F050470 0FC228F2 */  jal   hudmsgBottomShow
/* 084FA4 7F050474 00000000 */   nop
/* 084FA8 7F050478 10000005 */  b     .L7F050490
/* 084FAC 7F05047C 240B0001 */   li    $t3, 1
.L7F050480:
/* 084FB0 7F050480 8FAA0048 */  lw    $t2, 0x48($sp)
/* 084FB4 7F050484 0FC14089 */  jal   display_text_for_weapon_in_lower_left_corner
/* 084FB8 7F050488 81440080 */   lb    $a0, 0x80($t2)
/* 084FBC 7F05048C 240B0001 */  li    $t3, 1
.L7F050490:
/* 084FC0 7F050490 AFAB0044 */  sw    $t3, 0x44($sp)
/* 084FC4 7F050494 8FAC0048 */  lw    $t4, 0x48($sp)
.L7F050498:
/* 084FC8 7F050498 24030004 */  li    $v1, 4
/* 084FCC 7F05049C 1000001C */  b     .L7F050510
/* 084FD0 7F0504A0 81840080 */   lb    $a0, 0x80($t4)
.L7F0504A4:
/* 084FD4 7F0504A4 0FC231D9 */  jal   bondinvAddWeaponByProp
/* 084FD8 7F0504A8 8FA40070 */   lw    $a0, 0x70($sp)
/* 084FDC 7F0504AC 10400002 */  beqz  $v0, .L7F0504B8
/* 084FE0 7F0504B0 240D0001 */   li    $t5, 1
/* 084FE4 7F0504B4 AFAD0044 */  sw    $t5, 0x44($sp)
.L7F0504B8:
/* 084FE8 7F0504B8 8FAE0074 */  lw    $t6, 0x74($sp)
/* 084FEC 7F0504BC 8FAF0048 */  lw    $t7, 0x48($sp)
/* 084FF0 7F0504C0 51C00011 */  beql  $t6, $zero, .L7F050508
/* 084FF4 7F0504C4 8FA90048 */   lw    $t1, 0x48($sp)
/* 084FF8 7F0504C8 0FC23669 */  jal   bondinvGetActivatedTextWeapon
/* 084FFC 7F0504CC 81E40080 */   lb    $a0, 0x80($t7)
/* 085000 7F0504D0 10400006 */  beqz  $v0, .L7F0504EC
/* 085004 7F0504D4 00402025 */   move  $a0, $v0
/* 085008 7F0504D8 24180001 */  li    $t8, 1
/* 08500C 7F0504DC 0FC228F2 */  jal   hudmsgBottomShow
/* 085010 7F0504E0 AFB80044 */   sw    $t8, 0x44($sp)
/* 085014 7F0504E4 10000008 */  b     .L7F050508
/* 085018 7F0504E8 8FA90048 */   lw    $t1, 0x48($sp)
.L7F0504EC:
/* 08501C 7F0504EC 8FB90044 */  lw    $t9, 0x44($sp)
/* 085020 7F0504F0 8FA80048 */  lw    $t0, 0x48($sp)
/* 085024 7F0504F4 53200004 */  beql  $t9, $zero, .L7F050508
/* 085028 7F0504F8 8FA90048 */   lw    $t1, 0x48($sp)
/* 08502C 7F0504FC 0FC14089 */  jal   display_text_for_weapon_in_lower_left_corner
/* 085030 7F050500 81040080 */   lb    $a0, 0x80($t0)
/* 085034 7F050504 8FA90048 */  lw    $t1, 0x48($sp)
.L7F050508:
/* 085038 7F050508 24030001 */  li    $v1, 1
/* 08503C 7F05050C 81240080 */  lb    $a0, 0x80($t1)
.L7F050510:
/* 085040 7F050510 0FC1A50B */  jal   get_ammo_type_for_weapon
/* 085044 7F050514 AFA30068 */   sw    $v1, 0x68($sp)
/* 085048 7F050518 8FA30068 */  lw    $v1, 0x68($sp)
/* 08504C 7F05051C 10400055 */  beqz  $v0, .L7F050674
/* 085050 7F050520 00408025 */   move  $s0, $v0
/* 085054 7F050524 8FA40048 */  lw    $a0, 0x48($sp)
/* 085058 7F050528 0FC13F3E */  jal   ammo_collected_from_weapon
/* 08505C 7F05052C AFA30068 */   sw    $v1, 0x68($sp)
/* 085060 7F050530 8FA30068 */  lw    $v1, 0x68($sp)
/* 085064 7F050534 1840004F */  blez  $v0, .L7F050674
/* 085068 7F050538 AFA20034 */   sw    $v0, 0x34($sp)
/* 08506C 7F05053C 02002025 */  move  $a0, $s0
/* 085070 7F050540 0FC1A490 */  jal   check_cur_player_ammo_amount_in_inventory
/* 085074 7F050544 AFA30068 */   sw    $v1, 0x68($sp)
/* 085078 7F050548 AFA20024 */  sw    $v0, 0x24($sp)
/* 08507C 7F05054C 0FC1A4B5 */  jal   get_max_ammo_for_type
/* 085080 7F050550 02002025 */   move  $a0, $s0
/* 085084 7F050554 8FAA0024 */  lw    $t2, 0x24($sp)
/* 085088 7F050558 8FA30068 */  lw    $v1, 0x68($sp)
/* 08508C 7F05055C 02002025 */  move  $a0, $s0
/* 085090 7F050560 0142082A */  slt   $at, $t2, $v0
/* 085094 7F050564 50200044 */  beql  $at, $zero, .L7F050678
/* 085098 7F050568 24010001 */   li    $at, 1
/* 08509C 7F05056C 0FC1A490 */  jal   check_cur_player_ammo_amount_in_inventory
/* 0850A0 7F050570 AFA30068 */   sw    $v1, 0x68($sp)
/* 0850A4 7F050574 8FAB0034 */  lw    $t3, 0x34($sp)
/* 0850A8 7F050578 02002025 */  move  $a0, $s0
/* 0850AC 7F05057C 0FC1A44C */  jal   give_cur_player_ammo
/* 0850B0 7F050580 004B2821 */   addu  $a1, $v0, $t3
/* 0850B4 7F050584 8FAC0044 */  lw    $t4, 0x44($sp)
/* 0850B8 7F050588 8FA30068 */  lw    $v1, 0x68($sp)
/* 0850BC 7F05058C 8FAD0074 */  lw    $t5, 0x74($sp)
/* 0850C0 7F050590 55800039 */  bnezl $t4, .L7F050678
/* 0850C4 7F050594 24010001 */   li    $at, 1
/* 0850C8 7F050598 11A00036 */  beqz  $t5, .L7F050674
/* 0850CC 7F05059C 02002025 */   move  $a0, $s0
/* 0850D0 7F0505A0 8FA50034 */  lw    $a1, 0x34($sp)
/* 0850D4 7F0505A4 0FC13E7E */  jal   display_text_when_ammo_collected
/* 0850D8 7F0505A8 AFA30068 */   sw    $v1, 0x68($sp)
/* 0850DC 7F0505AC 10000031 */  b     .L7F050674
/* 0850E0 7F0505B0 8FA30068 */   lw    $v1, 0x68($sp)
interact_bodyarmor_object:
/* 0850E4 7F0505B4 0FC228C3 */  jal   bondviewAddCurrentPlayerArmor
/* 0850E8 7F0505B8 C46C0084 */   lwc1  $f12, 0x84($v1)
/* 0850EC 7F0505BC 3C048006 */  lui   $a0, %hi(g_musicSfxBufferPtr)
/* 0850F0 7F0505C0 8C843720 */  lw    $a0, %lo(g_musicSfxBufferPtr)($a0)
/* 0850F4 7F0505C4 24050051 */  li    $a1, 81
/* 0850F8 7F0505C8 0C002382 */  jal   sndPlaySfx
/* 0850FC 7F0505CC 00003025 */   move  $a2, $zero
/* 085100 7F0505D0 8FAE0074 */  lw    $t6, 0x74($sp)
/* 085104 7F0505D4 11C00013 */  beqz  $t6, .L7F050624
/* 085108 7F0505D8 00000000 */   nop
/* 08510C 7F0505DC 0FC23657 */  jal   bondinvGetActivatedTextObject
/* 085110 7F0505E0 8FA4006C */   lw    $a0, 0x6c($sp)
/* 085114 7F0505E4 1440000D */  bnez  $v0, .L7F05061C
/* 085118 7F0505E8 00402025 */   move  $a0, $v0
/* 08511C 7F0505EC 0FC26919 */  jal   getPlayerCount
/* 085120 7F0505F0 00000000 */   nop
/* 085124 7F0505F4 28410003 */  slti  $at, $v0, 3
/* 085128 7F0505F8 10200005 */  beqz  $at, .L7F050610
/* 08512C 7F0505FC 00000000 */   nop
/* 085130 7F050600 0FC30776 */  jal   langGet
/* 085134 7F050604 3404A43D */   li    $a0, 42045
/* 085138 7F050608 10000004 */  b     .L7F05061C
/* 08513C 7F05060C 00402025 */   move  $a0, $v0
.L7F050610:
/* 085140 7F050610 0FC30776 */  jal   langGet
/* 085144 7F050614 3404A43E */   li    $a0, 42046
/* 085148 7F050618 00402025 */  move  $a0, $v0
.L7F05061C:
/* 08514C 7F05061C 0FC228F2 */  jal   hudmsgBottomShow
/* 085150 7F050620 00000000 */   nop
.L7F050624:
/* 085154 7F050624 10000013 */  b     .L7F050674
/* 085158 7F050628 24030001 */   li    $v1, 1
interact_default_object:
.L7F05062C:
/* 08515C 7F05062C 3C048006 */  lui   $a0, %hi(g_musicSfxBufferPtr)
/* 085160 7F050630 8C843720 */  lw    $a0, %lo(g_musicSfxBufferPtr)($a0)
/* 085164 7F050634 240500E5 */  li    $a1, 229
/* 085168 7F050638 0C002382 */  jal   sndPlaySfx
/* 08516C 7F05063C 00003025 */   move  $a2, $zero
/* 085170 7F050640 8FAF0074 */  lw    $t7, 0x74($sp)
/* 085174 7F050644 51E0000B */  beql  $t7, $zero, .L7F050674
/* 085178 7F050648 24030004 */   li    $v1, 4
/* 08517C 7F05064C 0FC23657 */  jal   bondinvGetActivatedTextObject
/* 085180 7F050650 8FA4006C */   lw    $a0, 0x6c($sp)
/* 085184 7F050654 14400004 */  bnez  $v0, .L7F050668
/* 085188 7F050658 00402025 */   move  $a0, $v0
/* 08518C 7F05065C 0FC30776 */  jal   langGet
/* 085190 7F050660 3404A43F */   li    $a0, 42047
/* 085194 7F050664 00402025 */  move  $a0, $v0
.L7F050668:
/* 085198 7F050668 0FC228F2 */  jal   hudmsgBottomShow
/* 08519C 7F05066C 00000000 */   nop
/* 0851A0 7F050670 24030004 */  li    $v1, 4
.L7F050674:
/* 0851A4 7F050674 24010001 */  li    $at, 1
.L7F050678:
/* 0851A8 7F050678 1461000D */  bne   $v1, $at, .L7F0506B0
/* 0851AC 7F05067C 8FB8006C */   lw    $t8, 0x6c($sp)
/* 0851B0 7F050680 8F190064 */  lw    $t9, 0x64($t8)
/* 0851B4 7F050684 33280010 */  andi  $t0, $t9, 0x10
/* 0851B8 7F050688 15000009 */  bnez  $t0, .L7F0506B0
/* 0851BC 7F05068C 00000000 */   nop
/* 0851C0 7F050690 93060002 */  lbu   $a2, 2($t8)
/* 0851C4 7F050694 03002025 */  move  $a0, $t8
/* 0851C8 7F050698 00002825 */  move  $a1, $zero
/* 0851CC 7F05069C 30C90004 */  andi  $t1, $a2, 4
/* 0851D0 7F0506A0 0FC10366 */  jal   objFree
/* 0851D4 7F0506A4 01203025 */   move  $a2, $t1
/* 0851D8 7F0506A8 10000007 */  b     .L7F0506C8
/* 0851DC 7F0506AC 24020001 */   li    $v0, 1
.L7F0506B0:
/* 0851E0 7F0506B0 10600005 */  beqz  $v1, .L7F0506C8
/* 0851E4 7F0506B4 00001025 */   move  $v0, $zero
/* 0851E8 7F0506B8 0FC231C9 */  jal   bondinvAddPropToInv
/* 0851EC 7F0506BC 8FA40070 */   lw    $a0, 0x70($sp)
/* 0851F0 7F0506C0 10000001 */  b     .L7F0506C8
/* 0851F4 7F0506C4 24020004 */   li    $v0, 4
.L7F0506C8:
/* 0851F8 7F0506C8 8FBF001C */  lw    $ra, 0x1c($sp)
/* 0851FC 7F0506CC 8FB00018 */  lw    $s0, 0x18($sp)
/* 085200 7F0506D0 27BD0070 */  addiu $sp, $sp, 0x70
/* 085204 7F0506D4 03E00008 */  jr    $ra
/* 085208 7F0506D8 00000000 */   nop
)
#endif
#ifdef VERSION_JP
GLOBAL_ASM(
.late_rodata
/*D:8005322C*/
glabel object_interaction_table
.word interact_default_object
.word interact_key_object
.word interact_default_object
.word interact_default_object
.word interact_magazine_object
.word interact_weapon_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_ammobox_object
.word interact_bodyarmor_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object

.text
glabel collect_or_interact_object
/* 084D80 7F050250 27BDFF90 */  addiu $sp, $sp, -0x70
/* 084D84 7F050254 AFBF001C */  sw    $ra, 0x1c($sp)
/* 084D88 7F050258 AFB00018 */  sw    $s0, 0x18($sp)
/* 084D8C 7F05025C AFA40070 */  sw    $a0, 0x70($sp)
/* 084D90 7F050260 AFA50074 */  sw    $a1, 0x74($sp)
/* 084D94 7F050264 8C830004 */  lw    $v1, 4($a0)
/* 084D98 7F050268 3C0F8008 */  lui   $t7, %hi(g_CurrentPlayer)
/* 084D9C 7F05026C 8DEFA0B0 */  lw    $t7, %lo(g_CurrentPlayer)($t7)
/* 084DA0 7F050270 AFA3006C */  sw    $v1, 0x6c($sp)
/* 084DA4 7F050274 3C198005 */  lui   $t9, %hi(g_ClockTimer)
/* 084DA8 7F050278 8DF800D8 */  lw    $t8, 0xd8($t7)
/* 084DAC 7F05027C 17000005 */  bnez  $t8, .L7F050294
/* 084DB0 7F050280 00000000 */   nop
/* 084DB4 7F050284 8F398374 */  lw    $t9, %lo(g_ClockTimer)($t9)
/* 084DB8 7F050288 8FA8006C */  lw    $t0, 0x6c($sp)
/* 084DBC 7F05028C 57200004 */  bnezl $t9, .L7F0502A0
/* 084DC0 7F050290 91090003 */   lbu   $t1, 3($t0)
.L7F050294:
/* 084DC4 7F050294 1000010C */  b     .L7F0506C8
/* 084DC8 7F050298 00001025 */   move  $v0, $zero
/* 084DCC 7F05029C 91090003 */  lbu   $t1, 3($t0)
.L7F0502A0:
/* 084DD0 7F0502A0 252AFFFD */  addiu $t2, $t1, -3
/* 084DD4 7F0502A4 2D41002D */  sltiu $at, $t2, 0x2d
/* 084DD8 7F0502A8 102000E0 */  beqz  $at, .L7F05062C
/* 084DDC 7F0502AC 000A5080 */   sll   $t2, $t2, 2
/* 084DE0 7F0502B0 3C018005 */  lui   $at, %hi(object_interaction_table)
/* 084DE4 7F0502B4 002A0821 */  addu  $at, $at, $t2
/* 084DE8 7F0502B8 8C2A322C */  lw    $t2, %lo(object_interaction_table)($at)
/* 084DEC 7F0502BC 01400008 */  jr    $t2
/* 084DF0 7F0502C0 00000000 */   nop
interact_key_object:
/* 084DF4 7F0502C4 3C048006 */  lui   $a0, %hi(g_musicSfxBufferPtr)
/* 084DF8 7F0502C8 8C843720 */  lw    $a0, %lo(g_musicSfxBufferPtr)($a0)
/* 084DFC 7F0502CC 240500E5 */  li    $a1, 229
/* 084E00 7F0502D0 0C002382 */  jal   sndPlaySfx
/* 084E04 7F0502D4 00003025 */   move  $a2, $zero
/* 084E08 7F0502D8 8FAB0074 */  lw    $t3, 0x74($sp)
/* 084E0C 7F0502DC 1160000A */  beqz  $t3, .L7F050308
/* 084E10 7F0502E0 00000000 */   nop
/* 084E14 7F0502E4 0FC23657 */  jal   bondinvGetActivatedTextObject
/* 084E18 7F0502E8 8FA4006C */   lw    $a0, 0x6c($sp)
/* 084E1C 7F0502EC 14400004 */  bnez  $v0, .L7F050300
/* 084E20 7F0502F0 00402025 */   move  $a0, $v0
/* 084E24 7F0502F4 0FC30776 */  jal   langGet
/* 084E28 7F0502F8 3404A43C */   li    $a0, 42044
/* 084E2C 7F0502FC 00402025 */  move  $a0, $v0
.L7F050300:
/* 084E30 7F050300 0FC228F2 */  jal   jp_hudmsgBottomShow
/* 084E34 7F050304 00000000 */   nop
.L7F050308:
/* 084E38 7F050308 100000DA */  b     .L7F050674
/* 084E3C 7F05030C 24030004 */   li    $v1, 4
interact_magazine_object:
/* 084E40 7F050310 00608025 */  move  $s0, $v1
/* 084E44 7F050314 0FC13F0F */  jal   get_ammo_in_magazine
/* 084E48 7F050318 00602025 */   move  $a0, $v1
/* 084E4C 7F05031C 8E040080 */  lw    $a0, 0x80($s0)
/* 084E50 7F050320 00402825 */  move  $a1, $v0
/* 084E54 7F050324 24060001 */  li    $a2, 1
/* 084E58 7F050328 0FC13E9A */  jal   add_ammo_to_inventory
/* 084E5C 7F05032C 8FA70074 */   lw    $a3, 0x74($sp)
/* 084E60 7F050330 100000D0 */  b     .L7F050674
/* 084E64 7F050334 24030001 */   li    $v1, 1
interact_ammobox_object:
/* 084E68 7F050338 00001025 */  move  $v0, $zero
/* 084E6C 7F05033C 00608025 */  move  $s0, $v1
/* 084E70 7F050340 24430001 */  addiu $v1, $v0, 1
.L7F050344:
/* 084E74 7F050344 24010001 */  li    $at, 1
/* 084E78 7F050348 14410002 */  bne   $v0, $at, .L7F050354
/* 084E7C 7F05034C 00602025 */   move  $a0, $v1
/* 084E80 7F050350 24040001 */  li    $a0, 1
.L7F050354:
/* 084E84 7F050354 96050082 */  lhu   $a1, 0x82($s0)
/* 084E88 7F050358 AFA4004C */  sw    $a0, 0x4c($sp)
/* 084E8C 7F05035C AFA30020 */  sw    $v1, 0x20($sp)
/* 084E90 7F050360 0FC26919 */  jal   getPlayerCount
/* 084E94 7F050364 AFA50050 */   sw    $a1, 0x50($sp)
/* 084E98 7F050368 24010001 */  li    $at, 1
/* 084E9C 7F05036C 8FA30020 */  lw    $v1, 0x20($sp)
/* 084EA0 7F050370 8FA4004C */  lw    $a0, 0x4c($sp)
/* 084EA4 7F050374 14410009 */  bne   $v0, $at, .L7F05039C
/* 084EA8 7F050378 8FA50050 */   lw    $a1, 0x50($sp)
/* 084EAC 7F05037C 44852000 */  mtc1  $a1, $f4
/* 084EB0 7F050380 3C018003 */  lui   $at, %hi(g_SoloAmmoMultiplier)
/* 084EB4 7F050384 C4280B28 */  lwc1  $f8, %lo(g_SoloAmmoMultiplier)($at)
/* 084EB8 7F050388 468021A0 */  cvt.s.w $f6, $f4
/* 084EBC 7F05038C 46083282 */  mul.s $f10, $f6, $f8
/* 084EC0 7F050390 4600540D */  trunc.w.s $f16, $f10
/* 084EC4 7F050394 44058000 */  mfc1  $a1, $f16
/* 084EC8 7F050398 00000000 */  nop
.L7F05039C:
/* 084ECC 7F05039C 00003025 */  move  $a2, $zero
/* 084ED0 7F0503A0 8FA70074 */  lw    $a3, 0x74($sp)
/* 084ED4 7F0503A4 0FC13E9A */  jal   add_ammo_to_inventory
/* 084ED8 7F0503A8 AFA30020 */   sw    $v1, 0x20($sp)
/* 084EDC 7F0503AC 8FA20020 */  lw    $v0, 0x20($sp)
/* 084EE0 7F0503B0 2401000D */  li    $at, 13
/* 084EE4 7F0503B4 26100004 */  addiu $s0, $s0, 4
/* 084EE8 7F0503B8 5441FFE2 */  bnel  $v0, $at, .L7F050344
/* 084EEC 7F0503BC 24430001 */   addiu $v1, $v0, 1
/* 084EF0 7F0503C0 3C048006 */  lui   $a0, %hi(g_musicSfxBufferPtr)
/* 084EF4 7F0503C4 8C843720 */  lw    $a0, %lo(g_musicSfxBufferPtr)($a0)
/* 084EF8 7F0503C8 240500EA */  li    $a1, 234
/* 084EFC 7F0503CC 0C002382 */  jal   sndPlaySfx
/* 084F00 7F0503D0 00003025 */   move  $a2, $zero
/* 084F04 7F0503D4 100000A7 */  b     .L7F050674
/* 084F08 7F0503D8 24030001 */   li    $v1, 1
interact_weapon_object:
/* 084F0C 7F0503DC AFA00044 */  sw    $zero, 0x44($sp)
/* 084F10 7F0503E0 80640080 */  lb    $a0, 0x80($v1)
/* 084F14 7F0503E4 0FC13E04 */  jal   set_sound_effect_for_weapontype_collection
/* 084F18 7F0503E8 AFA30048 */   sw    $v1, 0x48($sp)
/* 084F1C 7F0503EC 8FAD0048 */  lw    $t5, 0x48($sp)
/* 084F20 7F0503F0 2401001D */  li    $at, 29
/* 084F24 7F0503F4 81A40080 */  lb    $a0, 0x80($t5)
/* 084F28 7F0503F8 54810006 */  bnel  $a0, $at, .L7F050414
/* 084F2C 7F0503FC 24010058 */   li    $at, 88
/* 084F30 7F050400 0FC23122 */  jal   bondinvAddInvItem
/* 084F34 7F050404 2404001E */   li    $a0, 30
/* 084F38 7F050408 10000007 */  b     .L7F050428
/* 084F3C 7F05040C 8FAE006C */   lw    $t6, 0x6c($sp)
/* 084F40 7F050410 24010058 */  li    $at, 88
.L7F050414:
/* 084F44 7F050414 14810003 */  bne   $a0, $at, .L7F050424
/* 084F48 7F050418 24050058 */   li    $a1, 88
/* 084F4C 7F05041C 0FC17645 */  jal   currentPlayerEquipWeaponWrapper
/* 084F50 7F050420 00002025 */   move  $a0, $zero
.L7F050424:
/* 084F54 7F050424 8FAE006C */  lw    $t6, 0x6c($sp)
.L7F050428:
/* 084F58 7F050428 8FB90048 */  lw    $t9, 0x48($sp)
/* 084F5C 7F05042C 8DCF0064 */  lw    $t7, 0x64($t6)
/* 084F60 7F050430 31F80400 */  andi  $t8, $t7, 0x400
/* 084F64 7F050434 1300001B */  beqz  $t8, .L7F0504A4
/* 084F68 7F050438 00000000 */   nop
/* 084F6C 7F05043C 83280080 */  lb    $t0, 0x80($t9)
/* 084F70 7F050440 29010021 */  slti  $at, $t0, 0x21
/* 084F74 7F050444 50200004 */  beql  $at, $zero, .L7F050458
/* 084F78 7F050448 8FA90074 */   lw    $t1, 0x74($sp)
/* 084F7C 7F05044C 0FC231D9 */  jal   bondinvAddWeaponByProp
/* 084F80 7F050450 8FA40070 */   lw    $a0, 0x70($sp)
/* 084F84 7F050454 8FA90074 */  lw    $t1, 0x74($sp)
.L7F050458:
/* 084F88 7F050458 5120000F */  beql  $t1, $zero, .L7F050498
/* 084F8C 7F05045C 8FAC0048 */   lw    $t4, 0x48($sp)
/* 084F90 7F050460 0FC23657 */  jal   bondinvGetActivatedTextObject
/* 084F94 7F050464 8FA4006C */   lw    $a0, 0x6c($sp)
/* 084F98 7F050468 10400005 */  beqz  $v0, .L7F050480
/* 084F9C 7F05046C 00402025 */   move  $a0, $v0
/* 084FA0 7F050470 0FC228F2 */  jal   jp_hudmsgBottomShow
/* 084FA4 7F050474 00000000 */   nop
/* 084FA8 7F050478 10000005 */  b     .L7F050490
/* 084FAC 7F05047C 240B0001 */   li    $t3, 1
.L7F050480:
/* 084FB0 7F050480 8FAA0048 */  lw    $t2, 0x48($sp)
/* 084FB4 7F050484 0FC14089 */  jal   display_text_for_weapon_in_lower_left_corner
/* 084FB8 7F050488 81440080 */   lb    $a0, 0x80($t2)
/* 084FBC 7F05048C 240B0001 */  li    $t3, 1
.L7F050490:
/* 084FC0 7F050490 AFAB0044 */  sw    $t3, 0x44($sp)
/* 084FC4 7F050494 8FAC0048 */  lw    $t4, 0x48($sp)
.L7F050498:
/* 084FC8 7F050498 24030004 */  li    $v1, 4
/* 084FCC 7F05049C 1000001C */  b     .L7F050510
/* 084FD0 7F0504A0 81840080 */   lb    $a0, 0x80($t4)
.L7F0504A4:
/* 084FD4 7F0504A4 0FC231D9 */  jal   bondinvAddWeaponByProp
/* 084FD8 7F0504A8 8FA40070 */   lw    $a0, 0x70($sp)
/* 084FDC 7F0504AC 10400002 */  beqz  $v0, .L7F0504B8
/* 084FE0 7F0504B0 240D0001 */   li    $t5, 1
/* 084FE4 7F0504B4 AFAD0044 */  sw    $t5, 0x44($sp)
.L7F0504B8:
/* 084FE8 7F0504B8 8FAE0074 */  lw    $t6, 0x74($sp)
/* 084FEC 7F0504BC 8FAF0048 */  lw    $t7, 0x48($sp)
/* 084FF0 7F0504C0 51C00011 */  beql  $t6, $zero, .L7F050508
/* 084FF4 7F0504C4 8FA90048 */   lw    $t1, 0x48($sp)
/* 084FF8 7F0504C8 0FC23669 */  jal   bondinvGetActivatedTextWeapon
/* 084FFC 7F0504CC 81E40080 */   lb    $a0, 0x80($t7)
/* 085000 7F0504D0 10400006 */  beqz  $v0, .L7F0504EC
/* 085004 7F0504D4 00402025 */   move  $a0, $v0
/* 085008 7F0504D8 24180001 */  li    $t8, 1
/* 08500C 7F0504DC 0FC228F2 */  jal   jp_hudmsgBottomShow
/* 085010 7F0504E0 AFB80044 */   sw    $t8, 0x44($sp)
/* 085014 7F0504E4 10000008 */  b     .L7F050508
/* 085018 7F0504E8 8FA90048 */   lw    $t1, 0x48($sp)
.L7F0504EC:
/* 08501C 7F0504EC 8FB90044 */  lw    $t9, 0x44($sp)
/* 085020 7F0504F0 8FA80048 */  lw    $t0, 0x48($sp)
/* 085024 7F0504F4 53200004 */  beql  $t9, $zero, .L7F050508
/* 085028 7F0504F8 8FA90048 */   lw    $t1, 0x48($sp)
/* 08502C 7F0504FC 0FC14089 */  jal   display_text_for_weapon_in_lower_left_corner
/* 085030 7F050500 81040080 */   lb    $a0, 0x80($t0)
/* 085034 7F050504 8FA90048 */  lw    $t1, 0x48($sp)
.L7F050508:
/* 085038 7F050508 24030001 */  li    $v1, 1
/* 08503C 7F05050C 81240080 */  lb    $a0, 0x80($t1)
.L7F050510:
/* 085040 7F050510 0FC1A50B */  jal   get_ammo_type_for_weapon
/* 085044 7F050514 AFA30068 */   sw    $v1, 0x68($sp)
/* 085048 7F050518 8FA30068 */  lw    $v1, 0x68($sp)
/* 08504C 7F05051C 10400055 */  beqz  $v0, .L7F050674
/* 085050 7F050520 00408025 */   move  $s0, $v0
/* 085054 7F050524 8FA40048 */  lw    $a0, 0x48($sp)
/* 085058 7F050528 0FC13F3E */  jal   ammo_collected_from_weapon
/* 08505C 7F05052C AFA30068 */   sw    $v1, 0x68($sp)
/* 085060 7F050530 8FA30068 */  lw    $v1, 0x68($sp)
/* 085064 7F050534 1840004F */  blez  $v0, .L7F050674
/* 085068 7F050538 AFA20034 */   sw    $v0, 0x34($sp)
/* 08506C 7F05053C 02002025 */  move  $a0, $s0
/* 085070 7F050540 0FC1A490 */  jal   check_cur_player_ammo_amount_in_inventory
/* 085074 7F050544 AFA30068 */   sw    $v1, 0x68($sp)
/* 085078 7F050548 AFA20024 */  sw    $v0, 0x24($sp)
/* 08507C 7F05054C 0FC1A4B5 */  jal   get_max_ammo_for_type
/* 085080 7F050550 02002025 */   move  $a0, $s0
/* 085084 7F050554 8FAA0024 */  lw    $t2, 0x24($sp)
/* 085088 7F050558 8FA30068 */  lw    $v1, 0x68($sp)
/* 08508C 7F05055C 02002025 */  move  $a0, $s0
/* 085090 7F050560 0142082A */  slt   $at, $t2, $v0
/* 085094 7F050564 50200044 */  beql  $at, $zero, .L7F050678
/* 085098 7F050568 24010001 */   li    $at, 1
/* 08509C 7F05056C 0FC1A490 */  jal   check_cur_player_ammo_amount_in_inventory
/* 0850A0 7F050570 AFA30068 */   sw    $v1, 0x68($sp)
/* 0850A4 7F050574 8FAB0034 */  lw    $t3, 0x34($sp)
/* 0850A8 7F050578 02002025 */  move  $a0, $s0
/* 0850AC 7F05057C 0FC1A44C */  jal   give_cur_player_ammo
/* 0850B0 7F050580 004B2821 */   addu  $a1, $v0, $t3
/* 0850B4 7F050584 8FAC0044 */  lw    $t4, 0x44($sp)
/* 0850B8 7F050588 8FA30068 */  lw    $v1, 0x68($sp)
/* 0850BC 7F05058C 8FAD0074 */  lw    $t5, 0x74($sp)
/* 0850C0 7F050590 55800039 */  bnezl $t4, .L7F050678
/* 0850C4 7F050594 24010001 */   li    $at, 1
/* 0850C8 7F050598 11A00036 */  beqz  $t5, .L7F050674
/* 0850CC 7F05059C 02002025 */   move  $a0, $s0
/* 0850D0 7F0505A0 8FA50034 */  lw    $a1, 0x34($sp)
/* 0850D4 7F0505A4 0FC13E7E */  jal   display_text_when_ammo_collected
/* 0850D8 7F0505A8 AFA30068 */   sw    $v1, 0x68($sp)
/* 0850DC 7F0505AC 10000031 */  b     .L7F050674
/* 0850E0 7F0505B0 8FA30068 */   lw    $v1, 0x68($sp)
interact_bodyarmor_object:
/* 0850E4 7F0505B4 0FC228C3 */  jal   bondviewAddCurrentPlayerArmor
/* 0850E8 7F0505B8 C46C0084 */   lwc1  $f12, 0x84($v1)
/* 0850EC 7F0505BC 3C048006 */  lui   $a0, %hi(g_musicSfxBufferPtr)
/* 0850F0 7F0505C0 8C843720 */  lw    $a0, %lo(g_musicSfxBufferPtr)($a0)
/* 0850F4 7F0505C4 24050051 */  li    $a1, 81
/* 0850F8 7F0505C8 0C002382 */  jal   sndPlaySfx
/* 0850FC 7F0505CC 00003025 */   move  $a2, $zero
/* 085100 7F0505D0 8FAE0074 */  lw    $t6, 0x74($sp)
/* 085104 7F0505D4 11C00013 */  beqz  $t6, .L7F050624
/* 085108 7F0505D8 00000000 */   nop
/* 08510C 7F0505DC 0FC23657 */  jal   bondinvGetActivatedTextObject
/* 085110 7F0505E0 8FA4006C */   lw    $a0, 0x6c($sp)
/* 085114 7F0505E4 1440000D */  bnez  $v0, .L7F05061C
/* 085118 7F0505E8 00402025 */   move  $a0, $v0
/* 08511C 7F0505EC 0FC26919 */  jal   getPlayerCount
/* 085120 7F0505F0 00000000 */   nop
/* 085124 7F0505F4 28410003 */  slti  $at, $v0, 3
/* 085128 7F0505F8 10200005 */  beqz  $at, .L7F050610
/* 08512C 7F0505FC 00000000 */   nop
/* 085130 7F050600 0FC30776 */  jal   langGet
/* 085134 7F050604 3404A43D */   li    $a0, 42045
/* 085138 7F050608 10000004 */  b     .L7F05061C
/* 08513C 7F05060C 00402025 */   move  $a0, $v0
.L7F050610:
/* 085140 7F050610 0FC30776 */  jal   langGet
/* 085144 7F050614 3404A43E */   li    $a0, 42046
/* 085148 7F050618 00402025 */  move  $a0, $v0
.L7F05061C:
/* 08514C 7F05061C 0FC228F2 */  jal   jp_hudmsgBottomShow
/* 085150 7F050620 00000000 */   nop
.L7F050624:
/* 085154 7F050624 10000013 */  b     .L7F050674
/* 085158 7F050628 24030001 */   li    $v1, 1
interact_default_object:
.L7F05062C:
/* 08515C 7F05062C 3C048006 */  lui   $a0, %hi(g_musicSfxBufferPtr)
/* 085160 7F050630 8C843720 */  lw    $a0, %lo(g_musicSfxBufferPtr)($a0)
/* 085164 7F050634 240500E5 */  li    $a1, 229
/* 085168 7F050638 0C002382 */  jal   sndPlaySfx
/* 08516C 7F05063C 00003025 */   move  $a2, $zero
/* 085170 7F050640 8FAF0074 */  lw    $t7, 0x74($sp)
/* 085174 7F050644 51E0000B */  beql  $t7, $zero, .L7F050674
/* 085178 7F050648 24030004 */   li    $v1, 4
/* 08517C 7F05064C 0FC23657 */  jal   bondinvGetActivatedTextObject
/* 085180 7F050650 8FA4006C */   lw    $a0, 0x6c($sp)
/* 085184 7F050654 14400004 */  bnez  $v0, .L7F050668
/* 085188 7F050658 00402025 */   move  $a0, $v0
/* 08518C 7F05065C 0FC30776 */  jal   langGet
/* 085190 7F050660 3404A43F */   li    $a0, 42047
/* 085194 7F050664 00402025 */  move  $a0, $v0
.L7F050668:
/* 085198 7F050668 0FC228F2 */  jal   jp_hudmsgBottomShow
/* 08519C 7F05066C 00000000 */   nop
/* 0851A0 7F050670 24030004 */  li    $v1, 4
.L7F050674:
/* 0851A4 7F050674 24010001 */  li    $at, 1
.L7F050678:
/* 0851A8 7F050678 1461000D */  bne   $v1, $at, .L7F0506B0
/* 0851AC 7F05067C 8FB8006C */   lw    $t8, 0x6c($sp)
/* 0851B0 7F050680 8F190064 */  lw    $t9, 0x64($t8)
/* 0851B4 7F050684 33280010 */  andi  $t0, $t9, 0x10
/* 0851B8 7F050688 15000009 */  bnez  $t0, .L7F0506B0
/* 0851BC 7F05068C 00000000 */   nop
/* 0851C0 7F050690 93060002 */  lbu   $a2, 2($t8)
/* 0851C4 7F050694 03002025 */  move  $a0, $t8
/* 0851C8 7F050698 00002825 */  move  $a1, $zero
/* 0851CC 7F05069C 30C90004 */  andi  $t1, $a2, 4
/* 0851D0 7F0506A0 0FC10366 */  jal   objFree
/* 0851D4 7F0506A4 01203025 */   move  $a2, $t1
/* 0851D8 7F0506A8 10000007 */  b     .L7F0506C8
/* 0851DC 7F0506AC 24020001 */   li    $v0, 1
.L7F0506B0:
/* 0851E0 7F0506B0 10600005 */  beqz  $v1, .L7F0506C8
/* 0851E4 7F0506B4 00001025 */   move  $v0, $zero
/* 0851E8 7F0506B8 0FC231C9 */  jal   bondinvAddPropToInv
/* 0851EC 7F0506BC 8FA40070 */   lw    $a0, 0x70($sp)
/* 0851F0 7F0506C0 10000001 */  b     .L7F0506C8
/* 0851F4 7F0506C4 24020004 */   li    $v0, 4
.L7F0506C8:
/* 0851F8 7F0506C8 8FBF001C */  lw    $ra, 0x1c($sp)
/* 0851FC 7F0506CC 8FB00018 */  lw    $s0, 0x18($sp)
/* 085200 7F0506D0 27BD0070 */  addiu $sp, $sp, 0x70
/* 085204 7F0506D4 03E00008 */  jr    $ra
/* 085208 7F0506D8 00000000 */   nop
)
#endif

#ifdef VERSION_EU
GLOBAL_ASM(
.late_rodata
/*D:8005322C*/
glabel object_interaction_table
.word interact_default_object
.word interact_key_object
.word interact_default_object
.word interact_default_object
.word interact_magazine_object
.word interact_weapon_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_ammobox_object
.word interact_bodyarmor_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object
.word interact_default_object

.text
glabel collect_or_interact_object
/* 084D80 7F050250 27BDFF90 */  addiu $sp, $sp, -0x70
/* 084D84 7F050254 AFBF001C */  sw    $ra, 0x1c($sp)
/* 084D88 7F050258 AFB00018 */  sw    $s0, 0x18($sp)
/* 084D8C 7F05025C AFA40070 */  sw    $a0, 0x70($sp)
/* 084D90 7F050260 AFA50074 */  sw    $a1, 0x74($sp)
/* 084D94 7F050264 8C830004 */  lw    $v1, 4($a0)
/* 084D98 7F050268 3C0F8008 */  lui   $t7, %hi(g_CurrentPlayer)
/* 084D9C 7F05026C 8DEFA0B0 */  lw    $t7, %lo(g_CurrentPlayer)($t7)
/* 084DA0 7F050270 AFA3006C */  sw    $v1, 0x6c($sp)
/* 084DA4 7F050274 3C198005 */  lui   $t9, %hi(g_ClockTimer)
/* 084DA8 7F050278 8DF800D8 */  lw    $t8, 0xd8($t7)
/* 084DAC 7F05027C 17000005 */  bnez  $t8, .L7F050294
/* 084DB0 7F050280 00000000 */   nop
/* 084DB4 7F050284 8F398374 */  lw    $t9, %lo(g_ClockTimer)($t9)
/* 084DB8 7F050288 8FA8006C */  lw    $t0, 0x6c($sp)
/* 084DBC 7F05028C 57200004 */  bnezl $t9, .L7F0502A0
/* 084DC0 7F050290 91090003 */   lbu   $t1, 3($t0)
.L7F050294:
/* 084DC4 7F050294 1000010C */  b     .L7F0506C8
/* 084DC8 7F050298 00001025 */   move  $v0, $zero
/* 084DCC 7F05029C 91090003 */  lbu   $t1, 3($t0)
.L7F0502A0:
/* 084DD0 7F0502A0 252AFFFD */  addiu $t2, $t1, -3
/* 084DD4 7F0502A4 2D41002D */  sltiu $at, $t2, 0x2d
/* 084DD8 7F0502A8 102000E0 */  beqz  $at, .L7F05062C
/* 084DDC 7F0502AC 000A5080 */   sll   $t2, $t2, 2
/* 084DE0 7F0502B0 3C018005 */  lui   $at, %hi(object_interaction_table)
/* 084DE4 7F0502B4 002A0821 */  addu  $at, $at, $t2
/* 084DE8 7F0502B8 8C2A322C */  lw    $t2, %lo(object_interaction_table)($at)
/* 084DEC 7F0502BC 01400008 */  jr    $t2
/* 084DF0 7F0502C0 00000000 */   nop
interact_key_object:
/* 084DF4 7F0502C4 3C048006 */  lui   $a0, %hi(g_musicSfxBufferPtr)
/* 084DF8 7F0502C8 8C843720 */  lw    $a0, %lo(g_musicSfxBufferPtr)($a0)
/* 084DFC 7F0502CC 240500E5 */  li    $a1, 229
/* 084E00 7F0502D0 0C002382 */  jal   sndPlaySfx
/* 084E04 7F0502D4 00003025 */   move  $a2, $zero
/* 084E08 7F0502D8 8FAB0074 */  lw    $t3, 0x74($sp)
/* 084E0C 7F0502DC 1160000A */  beqz  $t3, .L7F050308
/* 084E10 7F0502E0 00000000 */   nop
/* 084E14 7F0502E4 0FC23657 */  jal   bondinvGetActivatedTextObject
/* 084E18 7F0502E8 8FA4006C */   lw    $a0, 0x6c($sp)
/* 084E1C 7F0502EC 14400004 */  bnez  $v0, .L7F050300
/* 084E20 7F0502F0 00402025 */   move  $a0, $v0
/* 084E24 7F0502F4 0FC30776 */  jal   langGet
/* 084E28 7F0502F8 3404A43C */   li    $a0, 42044
/* 084E2C 7F0502FC 00402025 */  move  $a0, $v0
.L7F050300:
/* 084E30 7F050300 0FC228F2 */  jal   jp_hudmsgBottomShow
/* 084E34 7F050304 00000000 */   nop
.L7F050308:
/* 084E38 7F050308 100000DA */  b     .L7F050674
/* 084E3C 7F05030C 24030004 */   li    $v1, 4
interact_magazine_object:
/* 084E40 7F050310 00608025 */  move  $s0, $v1
/* 084E44 7F050314 0FC13F0F */  jal   get_ammo_in_magazine
/* 084E48 7F050318 00602025 */   move  $a0, $v1
/* 084E4C 7F05031C 8E040080 */  lw    $a0, 0x80($s0)
/* 084E50 7F050320 00402825 */  move  $a1, $v0
/* 084E54 7F050324 24060001 */  li    $a2, 1
/* 084E58 7F050328 0FC13E9A */  jal   add_ammo_to_inventory
/* 084E5C 7F05032C 8FA70074 */   lw    $a3, 0x74($sp)
/* 084E60 7F050330 100000D0 */  b     .L7F050674
/* 084E64 7F050334 24030001 */   li    $v1, 1
interact_ammobox_object:
/* 084E68 7F050338 00001025 */  move  $v0, $zero
/* 084E6C 7F05033C 00608025 */  move  $s0, $v1
/* 084E70 7F050340 24430001 */  addiu $v1, $v0, 1
.L7F050344:
/* 084E74 7F050344 24010001 */  li    $at, 1
/* 084E78 7F050348 14410002 */  bne   $v0, $at, .L7F050354
/* 084E7C 7F05034C 00602025 */   move  $a0, $v1
/* 084E80 7F050350 24040001 */  li    $a0, 1
.L7F050354:
/* 084E84 7F050354 96050082 */  lhu   $a1, 0x82($s0)
/* 084E88 7F050358 AFA4004C */  sw    $a0, 0x4c($sp)
/* 084E8C 7F05035C AFA30020 */  sw    $v1, 0x20($sp)
/* 084E90 7F050360 0FC26919 */  jal   getPlayerCount
/* 084E94 7F050364 AFA50050 */   sw    $a1, 0x50($sp)
/* 084E98 7F050368 24010001 */  li    $at, 1
/* 084E9C 7F05036C 8FA30020 */  lw    $v1, 0x20($sp)
/* 084EA0 7F050370 8FA4004C */  lw    $a0, 0x4c($sp)
/* 084EA4 7F050374 14410009 */  bne   $v0, $at, .L7F05039C
/* 084EA8 7F050378 8FA50050 */   lw    $a1, 0x50($sp)
/* 084EAC 7F05037C 44852000 */  mtc1  $a1, $f4
/* 084EB0 7F050380 3C018003 */  lui   $at, %hi(g_SoloAmmoMultiplier)
/* 084EB4 7F050384 C4280B28 */  lwc1  $f8, %lo(g_SoloAmmoMultiplier)($at)
/* 084EB8 7F050388 468021A0 */  cvt.s.w $f6, $f4
/* 084EBC 7F05038C 46083282 */  mul.s $f10, $f6, $f8
/* 084EC0 7F050390 4600540D */  trunc.w.s $f16, $f10
/* 084EC4 7F050394 44058000 */  mfc1  $a1, $f16
/* 084EC8 7F050398 00000000 */  nop
.L7F05039C:
/* 084ECC 7F05039C 00003025 */  move  $a2, $zero
/* 084ED0 7F0503A0 8FA70074 */  lw    $a3, 0x74($sp)
/* 084ED4 7F0503A4 0FC13E9A */  jal   add_ammo_to_inventory
/* 084ED8 7F0503A8 AFA30020 */   sw    $v1, 0x20($sp)
/* 084EDC 7F0503AC 8FA20020 */  lw    $v0, 0x20($sp)
/* 084EE0 7F0503B0 2401000D */  li    $at, 13
/* 084EE4 7F0503B4 26100004 */  addiu $s0, $s0, 4
/* 084EE8 7F0503B8 5441FFE2 */  bnel  $v0, $at, .L7F050344
/* 084EEC 7F0503BC 24430001 */   addiu $v1, $v0, 1
/* 084EF0 7F0503C0 3C048006 */  lui   $a0, %hi(g_musicSfxBufferPtr)
/* 084EF4 7F0503C4 8C843720 */  lw    $a0, %lo(g_musicSfxBufferPtr)($a0)
/* 084EF8 7F0503C8 240500EA */  li    $a1, 234
/* 084EFC 7F0503CC 0C002382 */  jal   sndPlaySfx
/* 084F00 7F0503D0 00003025 */   move  $a2, $zero
/* 084F04 7F0503D4 100000A7 */  b     .L7F050674
/* 084F08 7F0503D8 24030001 */   li    $v1, 1
interact_weapon_object:
/* 084F0C 7F0503DC AFA00044 */  sw    $zero, 0x44($sp)
/* 084F10 7F0503E0 80640080 */  lb    $a0, 0x80($v1)
/* 084F14 7F0503E4 0FC13E04 */  jal   set_sound_effect_for_weapontype_collection
/* 084F18 7F0503E8 AFA30048 */   sw    $v1, 0x48($sp)
/* 084F1C 7F0503EC 8FAD0048 */  lw    $t5, 0x48($sp)
/* 084F20 7F0503F0 2401001D */  li    $at, 29
/* 084F24 7F0503F4 81A40080 */  lb    $a0, 0x80($t5)
/* 084F28 7F0503F8 54810006 */  bnel  $a0, $at, .L7F050414
/* 084F2C 7F0503FC 24010058 */   li    $at, 88
/* 084F30 7F050400 0FC23122 */  jal   bondinvAddInvItem
/* 084F34 7F050404 2404001E */   li    $a0, 30
/* 084F38 7F050408 10000007 */  b     .L7F050428
/* 084F3C 7F05040C 8FAE006C */   lw    $t6, 0x6c($sp)
/* 084F40 7F050410 24010058 */  li    $at, 88
.L7F050414:
/* 084F44 7F050414 14810003 */  bne   $a0, $at, .L7F050424
/* 084F48 7F050418 24050058 */   li    $a1, 88
/* 084F4C 7F05041C 0FC17645 */  jal   currentPlayerEquipWeaponWrapper
/* 084F50 7F050420 00002025 */   move  $a0, $zero
.L7F050424:
/* 084F54 7F050424 8FAE006C */  lw    $t6, 0x6c($sp)
.L7F050428:
/* 084F58 7F050428 8FB90048 */  lw    $t9, 0x48($sp)
/* 084F5C 7F05042C 8DCF0064 */  lw    $t7, 0x64($t6)
/* 084F60 7F050430 31F80400 */  andi  $t8, $t7, 0x400
/* 084F64 7F050434 1300001B */  beqz  $t8, .L7F0504A4
/* 084F68 7F050438 00000000 */   nop
/* 084F6C 7F05043C 83280080 */  lb    $t0, 0x80($t9)
/* 084F70 7F050440 29010021 */  slti  $at, $t0, 0x21
/* 084F74 7F050444 50200004 */  beql  $at, $zero, .L7F050458
/* 084F78 7F050448 8FA90074 */   lw    $t1, 0x74($sp)
/* 084F7C 7F05044C 0FC231D9 */  jal   bondinvAddWeaponByProp
/* 084F80 7F050450 8FA40070 */   lw    $a0, 0x70($sp)
/* 084F84 7F050454 8FA90074 */  lw    $t1, 0x74($sp)
.L7F050458:
/* 084F88 7F050458 5120000F */  beql  $t1, $zero, .L7F050498
/* 084F8C 7F05045C 8FAC0048 */   lw    $t4, 0x48($sp)
/* 084F90 7F050460 0FC23657 */  jal   bondinvGetActivatedTextObject
/* 084F94 7F050464 8FA4006C */   lw    $a0, 0x6c($sp)
/* 084F98 7F050468 10400005 */  beqz  $v0, .L7F050480
/* 084F9C 7F05046C 00402025 */   move  $a0, $v0
/* 084FA0 7F050470 0FC228F2 */  jal   jp_hudmsgBottomShow
/* 084FA4 7F050474 00000000 */   nop
/* 084FA8 7F050478 10000005 */  b     .L7F050490
/* 084FAC 7F05047C 240B0001 */   li    $t3, 1
.L7F050480:
/* 084FB0 7F050480 8FAA0048 */  lw    $t2, 0x48($sp)
/* 084FB4 7F050484 0FC14089 */  jal   display_text_for_weapon_in_lower_left_corner
/* 084FB8 7F050488 81440080 */   lb    $a0, 0x80($t2)
/* 084FBC 7F05048C 240B0001 */  li    $t3, 1
.L7F050490:
/* 084FC0 7F050490 AFAB0044 */  sw    $t3, 0x44($sp)
/* 084FC4 7F050494 8FAC0048 */  lw    $t4, 0x48($sp)
.L7F050498:
/* 084FC8 7F050498 24030004 */  li    $v1, 4
/* 084FCC 7F05049C 1000001C */  b     .L7F050510
/* 084FD0 7F0504A0 81840080 */   lb    $a0, 0x80($t4)
.L7F0504A4:
/* 084FD4 7F0504A4 0FC231D9 */  jal   bondinvAddWeaponByProp
/* 084FD8 7F0504A8 8FA40070 */   lw    $a0, 0x70($sp)
/* 084FDC 7F0504AC 10400002 */  beqz  $v0, .L7F0504B8
/* 084FE0 7F0504B0 240D0001 */   li    $t5, 1
/* 084FE4 7F0504B4 AFAD0044 */  sw    $t5, 0x44($sp)
.L7F0504B8:
/* 084FE8 7F0504B8 8FAE0074 */  lw    $t6, 0x74($sp)
/* 084FEC 7F0504BC 8FAF0048 */  lw    $t7, 0x48($sp)
/* 084FF0 7F0504C0 51C00011 */  beql  $t6, $zero, .L7F050508
/* 084FF4 7F0504C4 8FA90048 */   lw    $t1, 0x48($sp)
/* 084FF8 7F0504C8 0FC23669 */  jal   bondinvGetActivatedTextWeapon
/* 084FFC 7F0504CC 81E40080 */   lb    $a0, 0x80($t7)
/* 085000 7F0504D0 10400006 */  beqz  $v0, .L7F0504EC
/* 085004 7F0504D4 00402025 */   move  $a0, $v0
/* 085008 7F0504D8 24180001 */  li    $t8, 1
/* 08500C 7F0504DC 0FC228F2 */  jal   jp_hudmsgBottomShow
/* 085010 7F0504E0 AFB80044 */   sw    $t8, 0x44($sp)
/* 085014 7F0504E4 10000008 */  b     .L7F050508
/* 085018 7F0504E8 8FA90048 */   lw    $t1, 0x48($sp)
.L7F0504EC:
/* 08501C 7F0504EC 8FB90044 */  lw    $t9, 0x44($sp)
/* 085020 7F0504F0 8FA80048 */  lw    $t0, 0x48($sp)
/* 085024 7F0504F4 53200004 */  beql  $t9, $zero, .L7F050508
/* 085028 7F0504F8 8FA90048 */   lw    $t1, 0x48($sp)
/* 08502C 7F0504FC 0FC14089 */  jal   display_text_for_weapon_in_lower_left_corner
/* 085030 7F050500 81040080 */   lb    $a0, 0x80($t0)
/* 085034 7F050504 8FA90048 */  lw    $t1, 0x48($sp)
.L7F050508:
/* 085038 7F050508 24030001 */  li    $v1, 1
/* 08503C 7F05050C 81240080 */  lb    $a0, 0x80($t1)
.L7F050510:
/* 085040 7F050510 0FC1A50B */  jal   get_ammo_type_for_weapon
/* 085044 7F050514 AFA30068 */   sw    $v1, 0x68($sp)
/* 085048 7F050518 8FA30068 */  lw    $v1, 0x68($sp)
/* 08504C 7F05051C 10400055 */  beqz  $v0, .L7F050674
/* 085050 7F050520 00408025 */   move  $s0, $v0
/* 085054 7F050524 8FA40048 */  lw    $a0, 0x48($sp)
/* 085058 7F050528 0FC13F3E */  jal   ammo_collected_from_weapon
/* 08505C 7F05052C AFA30068 */   sw    $v1, 0x68($sp)
/* 085060 7F050530 8FA30068 */  lw    $v1, 0x68($sp)
/* 085064 7F050534 1840004F */  blez  $v0, .L7F050674
/* 085068 7F050538 AFA20034 */   sw    $v0, 0x34($sp)
/* 08506C 7F05053C 02002025 */  move  $a0, $s0
/* 085070 7F050540 0FC1A490 */  jal   check_cur_player_ammo_amount_in_inventory
/* 085074 7F050544 AFA30068 */   sw    $v1, 0x68($sp)
/* 085078 7F050548 AFA20024 */  sw    $v0, 0x24($sp)
/* 08507C 7F05054C 0FC1A4B5 */  jal   get_max_ammo_for_type
/* 085080 7F050550 02002025 */   move  $a0, $s0
/* 085084 7F050554 8FAA0024 */  lw    $t2, 0x24($sp)
/* 085088 7F050558 8FA30068 */  lw    $v1, 0x68($sp)
/* 08508C 7F05055C 02002025 */  move  $a0, $s0
/* 085090 7F050560 0142082A */  slt   $at, $t2, $v0
/* 085094 7F050564 50200044 */  beql  $at, $zero, .L7F050678
/* 085098 7F050568 24010001 */   li    $at, 1
/* 08509C 7F05056C 0FC1A490 */  jal   check_cur_player_ammo_amount_in_inventory
/* 0850A0 7F050570 AFA30068 */   sw    $v1, 0x68($sp)
/* 0850A4 7F050574 8FAB0034 */  lw    $t3, 0x34($sp)
/* 0850A8 7F050578 02002025 */  move  $a0, $s0
/* 0850AC 7F05057C 0FC1A44C */  jal   give_cur_player_ammo
/* 0850B0 7F050580 004B2821 */   addu  $a1, $v0, $t3
/* 0850B4 7F050584 8FAC0044 */  lw    $t4, 0x44($sp)
/* 0850B8 7F050588 8FA30068 */  lw    $v1, 0x68($sp)
/* 0850BC 7F05058C 8FAD0074 */  lw    $t5, 0x74($sp)
/* 0850C0 7F050590 55800039 */  bnezl $t4, .L7F050678
/* 0850C4 7F050594 24010001 */   li    $at, 1
/* 0850C8 7F050598 11A00036 */  beqz  $t5, .L7F050674
/* 0850CC 7F05059C 02002025 */   move  $a0, $s0
/* 0850D0 7F0505A0 8FA50034 */  lw    $a1, 0x34($sp)
/* 0850D4 7F0505A4 0FC13E7E */  jal   display_text_when_ammo_collected
/* 0850D8 7F0505A8 AFA30068 */   sw    $v1, 0x68($sp)
/* 0850DC 7F0505AC 10000031 */  b     .L7F050674
/* 0850E0 7F0505B0 8FA30068 */   lw    $v1, 0x68($sp)
interact_bodyarmor_object:
/* 0850E4 7F0505B4 0FC228C3 */  jal   bondviewAddCurrentPlayerArmor
/* 0850E8 7F0505B8 C46C0084 */   lwc1  $f12, 0x84($v1)
/* 0850EC 7F0505BC 3C048006 */  lui   $a0, %hi(g_musicSfxBufferPtr)
/* 0850F0 7F0505C0 8C843720 */  lw    $a0, %lo(g_musicSfxBufferPtr)($a0)
/* 0850F4 7F0505C4 24050051 */  li    $a1, 81
/* 0850F8 7F0505C8 0C002382 */  jal   sndPlaySfx
/* 0850FC 7F0505CC 00003025 */   move  $a2, $zero
/* 085100 7F0505D0 8FAE0074 */  lw    $t6, 0x74($sp)
/* 085104 7F0505D4 11C00013 */  beqz  $t6, .L7F050624
/* 085108 7F0505D8 00000000 */   nop
/* 08510C 7F0505DC 0FC23657 */  jal   bondinvGetActivatedTextObject
/* 085110 7F0505E0 8FA4006C */   lw    $a0, 0x6c($sp)
/* 085114 7F0505E4 1440000D */  bnez  $v0, .L7F05061C
/* 085118 7F0505E8 00402025 */   move  $a0, $v0
/* 08511C 7F0505EC 0FC26919 */  jal   getPlayerCount
/* 085120 7F0505F0 00000000 */   nop
/* 085124 7F0505F4 28410003 */  slti  $at, $v0, 3
/* 085128 7F0505F8 10200005 */  beqz  $at, .L7F050610
/* 08512C 7F0505FC 00000000 */   nop
/* 085130 7F050600 0FC30776 */  jal   langGet
/* 085134 7F050604 3404A43D */   li    $a0, 42045
/* 085138 7F050608 10000004 */  b     .L7F05061C
/* 08513C 7F05060C 00402025 */   move  $a0, $v0
.L7F050610:
/* 085140 7F050610 0FC30776 */  jal   langGet
/* 085144 7F050614 3404A43E */   li    $a0, 42046
/* 085148 7F050618 00402025 */  move  $a0, $v0
.L7F05061C:
/* 08514C 7F05061C 0FC228F2 */  jal   jp_hudmsgBottomShow
/* 085150 7F050620 00000000 */   nop
.L7F050624:
/* 085154 7F050624 10000013 */  b     .L7F050674
/* 085158 7F050628 24030001 */   li    $v1, 1
interact_default_object:
.L7F05062C:
/* 08515C 7F05062C 3C048006 */  lui   $a0, %hi(g_musicSfxBufferPtr)
/* 085160 7F050630 8C843720 */  lw    $a0, %lo(g_musicSfxBufferPtr)($a0)
/* 085164 7F050634 240500E5 */  li    $a1, 229
/* 085168 7F050638 0C002382 */  jal   sndPlaySfx
/* 08516C 7F05063C 00003025 */   move  $a2, $zero
/* 085170 7F050640 8FAF0074 */  lw    $t7, 0x74($sp)
/* 085174 7F050644 51E0000B */  beql  $t7, $zero, .L7F050674
/* 085178 7F050648 24030004 */   li    $v1, 4
/* 08517C 7F05064C 0FC23657 */  jal   bondinvGetActivatedTextObject
/* 085180 7F050650 8FA4006C */   lw    $a0, 0x6c($sp)
/* 085184 7F050654 14400004 */  bnez  $v0, .L7F050668
/* 085188 7F050658 00402025 */   move  $a0, $v0
/* 08518C 7F05065C 0FC30776 */  jal   langGet
/* 085190 7F050660 3404A43F */   li    $a0, 42047
/* 085194 7F050664 00402025 */  move  $a0, $v0
.L7F050668:
/* 085198 7F050668 0FC228F2 */  jal   jp_hudmsgBottomShow
/* 08519C 7F05066C 00000000 */   nop
/* 0851A0 7F050670 24030004 */  li    $v1, 4
.L7F050674:
/* 0851A4 7F050674 24010001 */  li    $at, 1
.L7F050678:
/* 0851A8 7F050678 1461000D */  bne   $v1, $at, .L7F0506B0
/* 0851AC 7F05067C 8FB8006C */   lw    $t8, 0x6c($sp)
/* 0851B0 7F050680 8F190064 */  lw    $t9, 0x64($t8)
/* 0851B4 7F050684 33280010 */  andi  $t0, $t9, 0x10
/* 0851B8 7F050688 15000009 */  bnez  $t0, .L7F0506B0
/* 0851BC 7F05068C 00000000 */   nop
/* 0851C0 7F050690 93060002 */  lbu   $a2, 2($t8)
/* 0851C4 7F050694 03002025 */  move  $a0, $t8
/* 0851C8 7F050698 00002825 */  move  $a1, $zero
/* 0851CC 7F05069C 30C90004 */  andi  $t1, $a2, 4
/* 0851D0 7F0506A0 0FC10366 */  jal   objFree
/* 0851D4 7F0506A4 01203025 */   move  $a2, $t1
/* 0851D8 7F0506A8 10000007 */  b     .L7F0506C8
/* 0851DC 7F0506AC 24020001 */   li    $v0, 1
.L7F0506B0:
/* 0851E0 7F0506B0 10600005 */  beqz  $v1, .L7F0506C8
/* 0851E4 7F0506B4 00001025 */   move  $v0, $zero
/* 0851E8 7F0506B8 0FC231C9 */  jal   bondinvAddPropToInv
/* 0851EC 7F0506BC 8FA40070 */   lw    $a0, 0x70($sp)
/* 0851F0 7F0506C0 10000001 */  b     .L7F0506C8
/* 0851F4 7F0506C4 24020004 */   li    $v0, 4
.L7F0506C8:
/* 0851F8 7F0506C8 8FBF001C */  lw    $ra, 0x1c($sp)
/* 0851FC 7F0506CC 8FB00018 */  lw    $s0, 0x18($sp)
/* 085200 7F0506D0 27BD0070 */  addiu $sp, $sp, 0x70
/* 085204 7F0506D4 03E00008 */  jr    $ra
/* 085208 7F0506D8 00000000 */   nop
)
#endif


#endif




#ifdef NONMATCHING

s32 object_collectability_routines(PropRecord *arg0)
{
    ObjectRecord *sp84;
    ObjectRecord *sp80;
    ObjectRecord *sp74;
    ObjectRecord *sp70;
    s32           sp6C;
    ObjectRecord *sp64;
    ObjectRecord *sp60;
    s32           sp5C;
    s32           sp58;
    s32           sp40;
    StandTile *   sp3C;
    s32           sp38;
    ObjectRecord *sp34;
    ObjectRecord *sp30;
    ObjectRecord *temp_a0;
    ObjectRecord *temp_a1;
    ObjectRecord *temp_v1;
    ObjectRecord *temp_v1_2;
    PropRecord *  temp_v0_7;
    StandTile **  temp_a0_3;
    f32           temp_f0;
    f32           temp_f12;
    f32           temp_f2;
    f32           temp_v0_5;
    s32           temp_v0_6;
    s8            temp_a0_2;
    u32           temp_v0;
    u32           temp_v0_2;
    u8            temp_v0_4;
    void *        temp_v0_3;
    u32           phi_v0;
    ObjectRecord *phi_a0;
    s8            phi_a0_2;
    s8            phi_a1;
    ObjectRecord *phi_v0_2;
    ObjectRecord *phi_v1;
    ObjectRecord *phi_a0_3;
    s32           phi_v1_2;
    s32           phi_v0_3;
    s32           phi_v0_4;
    s32           phi_v0_5;
    s32           phi_v0_6;
    s32           phi_a2;
    s32           phi_v1_3;
    s32           phi_v0_7;
    s8            phi_a1_2;

    temp_a0 = arg0->chr;
    sp84    = temp_a0;
    phi_a0  = temp_a0;
    if ((objIsCollectable(temp_a0) != 0) && (temp_a0->Head.type != 0x11))
    {
        temp_v0 = temp_a0->flags;
        phi_v0  = temp_v0;
        if (temp_v0 & 0x100000)
        {
            return 0;
        }
        goto block_6;
    }
    temp_v0_2 = sp84->flags;
    phi_v0    = temp_v0_2;
    phi_a0    = sp84;
    if ((temp_v0_2 << 0xD) >= 0)
    {
        return 0;
    }
block_6:
    if (phi_v0 & 0x80000)
    {
        return 0;
    }
    if ((phi_a0->mtx.m[0][3] & 0x80) != 0)
    {
        temp_v0_3 = phi_a0->mtx.m[1][1];
        if ((temp_v0_3->unkBC > 0) && (temp_v0_3->unk90 == 0))
        {
            return 0;
        }
    }
    if (objCanPickupFromSafe(phi_a0) == 0)
    {
        return 0;
    }
    temp_v0_4 = sp84->Head.type;
    if (temp_v0_4 == 8)
    {
        temp_v1   = arg0->chr;
        temp_a0_2 = temp_v1->unk80;
        if (((temp_a0_2 == 0x1A) || (temp_a0_2 == 0x57)) && ((temp_v1->unk82 >= 0) || (((bitwise s32)sp84->mtx.m[0][3] & 4) != 0)))
        {
            return 0;
        }
        if (((temp_a0_2 == 0x1D) || (temp_a0_2 == 0x1C) || (temp_a0_2 == 0x1B) || (temp_a0_2 == 0x21) || (temp_a0_2 == 0x2F) || (temp_a0_2 == 0x30) || (temp_a0_2 == 0x22)) && ((temp_v1->unk82 >= 0) || (((bitwise s32)sp84->mtx.m[0][3] & 4) != 0)))
        {
            return 0;
        }
        if ((temp_a0_2 == 0x56) && (((bitwise s32)sp84->mtx.m[0][3] & 0x80) != 0))
        {
            return 0;
        }
        sp80 = temp_v1;
        if (is_item_in_inventory(temp_a0_2, sp84) != 0)
        {
            sp80 = temp_v1;
            if (get_ammo_type_for_weapon(temp_v1->unk80) != 0)
            {
                sp80 = temp_v1;
                sp38 = get_ammo_count_for_weapon(temp_v1->unk80);
                if (sp38 >= get_max_ammo_for_weapon(temp_v1->unk80))
                {
                    temp_v0_5 = temp_v1->mtx.m[2][3];
                    if (((bitwise s32)temp_v0_5 != 0) || (temp_v1->unk81 >= 0))
                    {
                        if ((bitwise s32)temp_v0_5 != 0)
                        {
                            phi_a1_2 = temp_v0_5->unk80;
                        }
                        else
                        {
                            phi_a1_2 = temp_v1->unk81;
                        }
                        phi_a0_2 = phi_a1_2;
                        phi_a1   = phi_a1_2;
                        if ((temp_v1->flags * 8) < 0)
                        {
                            phi_a1 = temp_v1->unk80;
                        }
                        else
                        {
                            phi_a0_2 = temp_v1->unk80;
                        }
                        if (is_dual_weapon_in_inventory(phi_a0_2, phi_a1, sp38) != 0)
                        {
                            return 0;
                        }
                        goto block_74;
                    }
                    return 0;
                }
            }
        }
        goto block_74;
    }
    if (temp_v0_4 == 7)
    {
        temp_v1_2 = arg0->chr;
        sp74      = temp_v1_2;
        sp38      = check_cur_player_ammo_amount_in_inventory((bitwise ObjectRecord *)temp_v1_2->mtx.m[2][2], sp84);
        if (sp38 >= get_max_ammo_for_type((bitwise ObjectRecord *)temp_v1_2->mtx.m[2][2]))
        {
            return 0;
        }
        goto block_74;
    }
    if (temp_v0_4 == 0x14)
    {
        sp6C   = 1;
        sp70   = arg0->chr;
        phi_v1 = sp70;
        if (objGetDestroyedLevel(sp84) != 0)
        {
            return 0;
        }
        phi_v0_2 = NULL;
loop_54:
        temp_a1  = phi_v0_2 + 1;
        phi_a0_3 = temp_a1;
        if (phi_v0_2 == 1)
        {
            phi_a0_3 = 1;
        }
        if ((phi_v1->unk82 > 0) && (sp30 = phi_v1, sp64 = phi_a0_3, sp34 = temp_a1, sp38 = check_cur_player_ammo_amount_in_inventory(phi_a0_3, temp_a1), ((sp38 < get_max_ammo_for_type(phi_a0_3)) != 0)))
        {
            sp6C = 0;
        }
        else
        {
            phi_v0_2 += 1;
            phi_v1 += 4;
            if ((phi_v0_2 + 1) != 0xD)
            {
                goto loop_54;
            }
        }
        if (sp6C != 0)
        {
            return 0;
        }
        goto block_74;
    }
    if (temp_v0_4 == 0x15)
    {
        sp5C = 0;
        sp60 = arg0->chr;
        if (sp60->mtx.m[2][3] <= get_BONDdata_watch_armor())
        {
            sp5C = 1;
        }
        else if (getPlayerCount() >= 2)
        {
            temp_v0_6 = get_scenario();
            phi_v1_2  = temp_v0_6;
            if ((temp_v0_6 == 2) && (sp58 = temp_v0_6, phi_v1_2 = sp58, (bondinvIsAliveWithFlag() != 0)))
            {
                sp5C = 1;
            }
            else if ((phi_v1_2 == 3) && (checkforgoldengun() != 0))
            {
                sp5C = 1;
            }
        }
        if (sp5C != 0)
        {
            return 0;
        }
        goto block_74;
    }
block_74:
    if ((get_curplay_vertical_rotation_in_degrees() < -0.7853982f) && (g_CurrentPlayer->unk1270 < 0))
    {
        return 0;
    }
    temp_v0_7 = get_curplayer_positiondata();
    temp_f0   = sp84->mtx.m[0][0] - temp_v0_7->pos.AsArray[0];
    temp_f12  = sp84->mtx.m[0][1] - temp_v0_7->pos.AsArray[1];
    temp_f2   = sp84->mtx.m[0][2] - temp_v0_7->pos.AsArray[2];
    if (g_CurrentPlayer->unk1270 >= 0x3C)
    {
        phi_v0_3 = 0;
        phi_v0_4 = 0;
        if (((temp_f0 * temp_f0) + (temp_f2 * temp_f2)) <= 122500.0f)
        {
            phi_v0_3 = 1;
        }
        phi_a2 = phi_v0_3;
        if (phi_v0_3 != 0)
        {
            if (temp_f12 >= -500.0f)
            {
                phi_v0_4 = 1;
            }
            phi_a2 = phi_v0_4;
            if (phi_v0_4 != 0)
            {
                phi_a2 = 0;
                if (temp_f12 <= 500.0f)
                {
                    phi_a2 = 1;
                }
            }
        }
    }
    else
    {
        phi_v0_5 = 0;
        phi_v0_6 = 0;
        if (((temp_f0 * temp_f0) + (temp_f2 * temp_f2)) <= 10000.0f)
        {
            phi_v0_5 = 1;
        }
        phi_a2 = phi_v0_5;
        if (phi_v0_5 != 0)
        {
            if (temp_f12 >= -200.0f)
            {
                phi_v0_6 = 1;
            }
            phi_a2 = phi_v0_6;
            if (phi_v0_6 != 0)
            {
                phi_a2 = 0;
                if (temp_f12 <= 200.0f)
                {
                    phi_a2 = 1;
                }
            }
        }
    }
    phi_v1_3 = phi_a2;
    if (phi_a2 != 0)
    {
        temp_a0_3 = &sp3C;
        if (((sp84->flags2 & 0x1000) == 0) && ((sp3C = temp_v0_7->stan, sp40 = phi_a2, phi_v1_3 = phi_a2, (stanTestLineUnobstructed(temp_a0_3, temp_v0_7->pos.AsArray[0], temp_v0_7->pos.AsArray[2], arg0->pos.AsArray[0], arg0->pos.AsArray[2], 2, 30.0f, 30.0f, 0.0f, 1.0f) == 0)) || (sp3C != arg0->stan)))
        {
            phi_v1_3 = 0;
        }
    }
    phi_v0_7 = 0;
    if (phi_v1_3 != 0)
    {
        phi_v0_7 = collect_or_interact_object(arg0, 1);
    }
    return phi_v0_7;
}

#else

#ifdef VERSION_US
GLOBAL_ASM(
.late_rodata
glabel D_800532E0
.word 0xbf490fdb /*-0.78539819*/
glabel D_800532E4
.word 0x47ef4200 /*122500.0*/
glabel D_800532E8
.word 0x461c4000 /*10000.0*/
.text
glabel object_collectability_routines
/* 08520C 7F0506DC 27BDFF78 */  addiu $sp, $sp, -0x88
/* 085210 7F0506E0 AFBF002C */  sw    $ra, 0x2c($sp)
/* 085214 7F0506E4 AFA40088 */  sw    $a0, 0x88($sp)
/* 085218 7F0506E8 8C840004 */  lw    $a0, 4($a0)
/* 08521C 7F0506EC 0FC13803 */  jal   objIsCollectable
/* 085220 7F0506F0 AFA40084 */   sw    $a0, 0x84($sp)
/* 085224 7F0506F4 1040000B */  beqz  $v0, .L7F050724
/* 085228 7F0506F8 8FA40084 */   lw    $a0, 0x84($sp)
/* 08522C 7F0506FC 908F0003 */  lbu   $t7, 3($a0)
/* 085230 7F050700 24010011 */  li    $at, 17
/* 085234 7F050704 51E10008 */  beql  $t7, $at, .L7F050728
/* 085238 7F050708 8FA40084 */   lw    $a0, 0x84($sp)
/* 08523C 7F05070C 8C820008 */  lw    $v0, 8($a0)
/* 085240 7F050710 0002C2C0 */  sll   $t8, $v0, 0xb
/* 085244 7F050714 0703000B */  bgezl $t8, .L7F050744
/* 085248 7F050718 00024B00 */   sll   $t1, $v0, 0xc
/* 08524C 7F05071C 10000180 */  b     .L7F050D20
/* 085250 7F050720 00001025 */   move  $v0, $zero
.L7F050724:
/* 085254 7F050724 8FA40084 */  lw    $a0, 0x84($sp)
.L7F050728:
/* 085258 7F050728 8C820008 */  lw    $v0, 8($a0)
/* 08525C 7F05072C 0002CB40 */  sll   $t9, $v0, 0xd
/* 085260 7F050730 07220004 */  bltzl $t9, .L7F050744
/* 085264 7F050734 00024B00 */   sll   $t1, $v0, 0xc
/* 085268 7F050738 10000179 */  b     .L7F050D20
/* 08526C 7F05073C 00001025 */   move  $v0, $zero
/* 085270 7F050740 00024B00 */  sll   $t1, $v0, 0xc
.L7F050744:
/* 085274 7F050744 05230004 */  bgezl $t1, .L7F050758
/* 085278 7F050748 8C8A0064 */   lw    $t2, 0x64($a0)
/* 08527C 7F05074C 10000174 */  b     .L7F050D20
/* 085280 7F050750 00001025 */   move  $v0, $zero
/* 085284 7F050754 8C8A0064 */  lw    $t2, 0x64($a0)
.L7F050758:
/* 085288 7F050758 314B0080 */  andi  $t3, $t2, 0x80
/* 08528C 7F05075C 1160000A */  beqz  $t3, .L7F050788
/* 085290 7F050760 00000000 */   nop
/* 085294 7F050764 8C82006C */  lw    $v0, 0x6c($a0)
/* 085298 7F050768 8C4C00BC */  lw    $t4, 0xbc($v0)
/* 08529C 7F05076C 19800006 */  blez  $t4, .L7F050788
/* 0852A0 7F050770 00000000 */   nop
/* 0852A4 7F050774 8C4D0090 */  lw    $t5, 0x90($v0)
/* 0852A8 7F050778 15A00003 */  bnez  $t5, .L7F050788
/* 0852AC 7F05077C 00000000 */   nop
/* 0852B0 7F050780 10000167 */  b     .L7F050D20
/* 0852B4 7F050784 00001025 */   move  $v0, $zero
.L7F050788:
/* 0852B8 7F050788 0FC0F9EB */  jal   objCanPickupFromSafe
/* 0852BC 7F05078C 00000000 */   nop
/* 0852C0 7F050790 14400003 */  bnez  $v0, .L7F0507A0
/* 0852C4 7F050794 8FA50084 */   lw    $a1, 0x84($sp)
/* 0852C8 7F050798 10000161 */  b     .L7F050D20
/* 0852CC 7F05079C 00001025 */   move  $v0, $zero
.L7F0507A0:
/* 0852D0 7F0507A0 90A20003 */  lbu   $v0, 3($a1)
/* 0852D4 7F0507A4 24010008 */  li    $at, 8
/* 0852D8 7F0507A8 8FAE0088 */  lw    $t6, 0x88($sp)
/* 0852DC 7F0507AC 54410064 */  bnel  $v0, $at, .L7F050940
/* 0852E0 7F0507B0 24010007 */   li    $at, 7
/* 0852E4 7F0507B4 8DC30004 */  lw    $v1, 4($t6)
/* 0852E8 7F0507B8 2401001A */  li    $at, 26
/* 0852EC 7F0507BC 80640080 */  lb    $a0, 0x80($v1)
/* 0852F0 7F0507C0 10810003 */  beq   $a0, $at, .L7F0507D0
/* 0852F4 7F0507C4 24010057 */   li    $at, 87
/* 0852F8 7F0507C8 5481000B */  bnel  $a0, $at, .L7F0507F8
/* 0852FC 7F0507CC 2401001D */   li    $at, 29
.L7F0507D0:
/* 085300 7F0507D0 846F0082 */  lh    $t7, 0x82($v1)
/* 085304 7F0507D4 05E10005 */  bgez  $t7, .L7F0507EC
/* 085308 7F0507D8 00000000 */   nop
/* 08530C 7F0507DC 8CB80064 */  lw    $t8, 0x64($a1)
/* 085310 7F0507E0 33190004 */  andi  $t9, $t8, 4
/* 085314 7F0507E4 53200004 */  beql  $t9, $zero, .L7F0507F8
/* 085318 7F0507E8 2401001D */   li    $at, 29
.L7F0507EC:
/* 08531C 7F0507EC 1000014C */  b     .L7F050D20
/* 085320 7F0507F0 00001025 */   move  $v0, $zero
/* 085324 7F0507F4 2401001D */  li    $at, 29
.L7F0507F8:
/* 085328 7F0507F8 1081000D */  beq   $a0, $at, .L7F050830
/* 08532C 7F0507FC 2401001C */   li    $at, 28
/* 085330 7F050800 1081000B */  beq   $a0, $at, .L7F050830
/* 085334 7F050804 2401001B */   li    $at, 27
/* 085338 7F050808 10810009 */  beq   $a0, $at, .L7F050830
/* 08533C 7F05080C 24010021 */   li    $at, 33
/* 085340 7F050810 10810007 */  beq   $a0, $at, .L7F050830
/* 085344 7F050814 2401002F */   li    $at, 47
/* 085348 7F050818 10810005 */  beq   $a0, $at, .L7F050830
/* 08534C 7F05081C 24010030 */   li    $at, 48
/* 085350 7F050820 10810003 */  beq   $a0, $at, .L7F050830
/* 085354 7F050824 24010022 */   li    $at, 34
/* 085358 7F050828 5481000B */  bnel  $a0, $at, .L7F050858
/* 08535C 7F05082C 24010056 */   li    $at, 86
.L7F050830:
/* 085360 7F050830 84690082 */  lh    $t1, 0x82($v1)
/* 085364 7F050834 05210005 */  bgez  $t1, .L7F05084C
/* 085368 7F050838 00000000 */   nop
/* 08536C 7F05083C 8CAA0064 */  lw    $t2, 0x64($a1)
/* 085370 7F050840 314B0004 */  andi  $t3, $t2, 4
/* 085374 7F050844 51600004 */  beql  $t3, $zero, .L7F050858
/* 085378 7F050848 24010056 */   li    $at, 86
.L7F05084C:
/* 08537C 7F05084C 10000134 */  b     .L7F050D20
/* 085380 7F050850 00001025 */   move  $v0, $zero
/* 085384 7F050854 24010056 */  li    $at, 86
.L7F050858:
/* 085388 7F050858 14810007 */  bne   $a0, $at, .L7F050878
/* 08538C 7F05085C 00000000 */   nop
/* 085390 7F050860 8CAC0064 */  lw    $t4, 0x64($a1)
/* 085394 7F050864 318D0080 */  andi  $t5, $t4, 0x80
/* 085398 7F050868 11A00003 */  beqz  $t5, .L7F050878
/* 08539C 7F05086C 00000000 */   nop
/* 0853A0 7F050870 1000012B */  b     .L7F050D20
/* 0853A4 7F050874 00001025 */   move  $v0, $zero
.L7F050878:
/* 0853A8 7F050878 0FC230C5 */  jal   bondinvHasInvItem
/* 0853AC 7F05087C AFA30080 */   sw    $v1, 0x80($sp)
/* 0853B0 7F050880 1040009B */  beqz  $v0, .L7F050AF0
/* 0853B4 7F050884 8FA30080 */   lw    $v1, 0x80($sp)
/* 0853B8 7F050888 80640080 */  lb    $a0, 0x80($v1)
/* 0853BC 7F05088C 0FC1A50B */  jal   get_ammo_type_for_weapon
/* 0853C0 7F050890 AFA30080 */   sw    $v1, 0x80($sp)
/* 0853C4 7F050894 10400096 */  beqz  $v0, .L7F050AF0
/* 0853C8 7F050898 8FA30080 */   lw    $v1, 0x80($sp)
/* 0853CC 7F05089C 80640080 */  lb    $a0, 0x80($v1)
/* 0853D0 7F0508A0 0FC1A514 */  jal   get_ammo_count_for_weapon
/* 0853D4 7F0508A4 AFA30080 */   sw    $v1, 0x80($sp)
/* 0853D8 7F0508A8 8FA30080 */  lw    $v1, 0x80($sp)
/* 0853DC 7F0508AC 80640080 */  lb    $a0, 0x80($v1)
/* 0853E0 7F0508B0 0FC1A52C */  jal   get_max_ammo_for_weapon
/* 0853E4 7F0508B4 AFA20038 */   sw    $v0, 0x38($sp)
/* 0853E8 7F0508B8 8FA60038 */  lw    $a2, 0x38($sp)
/* 0853EC 7F0508BC 8FA30080 */  lw    $v1, 0x80($sp)
/* 0853F0 7F0508C0 00C2082A */  slt   $at, $a2, $v0
/* 0853F4 7F0508C4 1420008A */  bnez  $at, .L7F050AF0
/* 0853F8 7F0508C8 00000000 */   nop
/* 0853FC 7F0508CC 8C620084 */  lw    $v0, 0x84($v1)
/* 085400 7F0508D0 14400004 */  bnez  $v0, .L7F0508E4
/* 085404 7F0508D4 00000000 */   nop
/* 085408 7F0508D8 806E0081 */  lb    $t6, 0x81($v1)
/* 08540C 7F0508DC 05C00015 */  bltz  $t6, .L7F050934
/* 085410 7F0508E0 00000000 */   nop
.L7F0508E4:
/* 085414 7F0508E4 50400005 */  beql  $v0, $zero, .L7F0508FC
/* 085418 7F0508E8 80650081 */   lb    $a1, 0x81($v1)
/* 08541C 7F0508EC 80450080 */  lb    $a1, 0x80($v0)
/* 085420 7F0508F0 10000003 */  b     .L7F050900
/* 085424 7F0508F4 00A02025 */   move  $a0, $a1
/* 085428 7F0508F8 80650081 */  lb    $a1, 0x81($v1)
.L7F0508FC:
/* 08542C 7F0508FC 00A02025 */  move  $a0, $a1
.L7F050900:
/* 085430 7F050900 8C6F0008 */  lw    $t7, 8($v1)
/* 085434 7F050904 000FC0C0 */  sll   $t8, $t7, 3
/* 085438 7F050908 07030004 */  bgezl $t8, .L7F05091C
/* 08543C 7F05090C 80640080 */   lb    $a0, 0x80($v1)
/* 085440 7F050910 10000002 */  b     .L7F05091C
/* 085444 7F050914 80650080 */   lb    $a1, 0x80($v1)
/* 085448 7F050918 80640080 */  lb    $a0, 0x80($v1)
.L7F05091C:
/* 08544C 7F05091C 0FC230E7 */  jal   bondinvHasDualWeapon
/* 085450 7F050920 00000000 */   nop
/* 085454 7F050924 10400072 */  beqz  $v0, .L7F050AF0
/* 085458 7F050928 00000000 */   nop
/* 08545C 7F05092C 100000FC */  b     .L7F050D20
/* 085460 7F050930 00001025 */   move  $v0, $zero
.L7F050934:
/* 085464 7F050934 100000FA */  b     .L7F050D20
/* 085468 7F050938 00001025 */   move  $v0, $zero
/* 08546C 7F05093C 24010007 */  li    $at, 7
.L7F050940:
/* 085470 7F050940 1441000F */  bne   $v0, $at, .L7F050980
/* 085474 7F050944 8FB90088 */   lw    $t9, 0x88($sp)
/* 085478 7F050948 8F230004 */  lw    $v1, 4($t9)
/* 08547C 7F05094C 8C640080 */  lw    $a0, 0x80($v1)
/* 085480 7F050950 0FC1A490 */  jal   check_cur_player_ammo_amount_in_inventory
/* 085484 7F050954 AFA30074 */   sw    $v1, 0x74($sp)
/* 085488 7F050958 8FA30074 */  lw    $v1, 0x74($sp)
/* 08548C 7F05095C 8C640080 */  lw    $a0, 0x80($v1)
/* 085490 7F050960 0FC1A4B5 */  jal   get_max_ammo_for_type
/* 085494 7F050964 AFA20038 */   sw    $v0, 0x38($sp)
/* 085498 7F050968 8FA60038 */  lw    $a2, 0x38($sp)
/* 08549C 7F05096C 00C2082A */  slt   $at, $a2, $v0
/* 0854A0 7F050970 1420005F */  bnez  $at, .L7F050AF0
/* 0854A4 7F050974 00000000 */   nop
/* 0854A8 7F050978 100000E9 */  b     .L7F050D20
/* 0854AC 7F05097C 00001025 */   move  $v0, $zero
.L7F050980:
/* 0854B0 7F050980 24010014 */  li    $at, 20
/* 0854B4 7F050984 1441002C */  bne   $v0, $at, .L7F050A38
/* 0854B8 7F050988 8FA90088 */   lw    $t1, 0x88($sp)
/* 0854BC 7F05098C 8D2A0004 */  lw    $t2, 4($t1)
/* 0854C0 7F050990 240B0001 */  li    $t3, 1
/* 0854C4 7F050994 AFAB006C */  sw    $t3, 0x6c($sp)
/* 0854C8 7F050998 00A02025 */  move  $a0, $a1
/* 0854CC 7F05099C 0FC0FFF0 */  jal   objGetDestroyedLevel
/* 0854D0 7F0509A0 AFAA0070 */   sw    $t2, 0x70($sp)
/* 0854D4 7F0509A4 10400003 */  beqz  $v0, .L7F0509B4
/* 0854D8 7F0509A8 8FA30070 */   lw    $v1, 0x70($sp)
/* 0854DC 7F0509AC 100000DC */  b     .L7F050D20
/* 0854E0 7F0509B0 00001025 */   move  $v0, $zero
.L7F0509B4:
/* 0854E4 7F0509B4 00001025 */  move  $v0, $zero
.L7F0509B8:
/* 0854E8 7F0509B8 24450001 */  addiu $a1, $v0, 1
/* 0854EC 7F0509BC 24010001 */  li    $at, 1
/* 0854F0 7F0509C0 14410002 */  bne   $v0, $at, .L7F0509CC
/* 0854F4 7F0509C4 00A02025 */   move  $a0, $a1
/* 0854F8 7F0509C8 24040001 */  li    $a0, 1
.L7F0509CC:
/* 0854FC 7F0509CC 946C0082 */  lhu   $t4, 0x82($v1)
/* 085500 7F0509D0 59800011 */  blezl $t4, .L7F050A18
/* 085504 7F0509D4 2401000D */   li    $at, 13
/* 085508 7F0509D8 AFA30030 */  sw    $v1, 0x30($sp)
/* 08550C 7F0509DC AFA40064 */  sw    $a0, 0x64($sp)
/* 085510 7F0509E0 0FC1A490 */  jal   check_cur_player_ammo_amount_in_inventory
/* 085514 7F0509E4 AFA50034 */   sw    $a1, 0x34($sp)
/* 085518 7F0509E8 8FA40064 */  lw    $a0, 0x64($sp)
/* 08551C 7F0509EC 0FC1A4B5 */  jal   get_max_ammo_for_type
/* 085520 7F0509F0 AFA20038 */   sw    $v0, 0x38($sp)
/* 085524 7F0509F4 8FA60038 */  lw    $a2, 0x38($sp)
/* 085528 7F0509F8 8FA30030 */  lw    $v1, 0x30($sp)
/* 08552C 7F0509FC 8FA50034 */  lw    $a1, 0x34($sp)
/* 085530 7F050A00 00C2082A */  slt   $at, $a2, $v0
/* 085534 7F050A04 50200004 */  beql  $at, $zero, .L7F050A18
/* 085538 7F050A08 2401000D */   li    $at, 13
/* 08553C 7F050A0C 10000005 */  b     .L7F050A24
/* 085540 7F050A10 AFA0006C */   sw    $zero, 0x6c($sp)
/* 085544 7F050A14 2401000D */  li    $at, 13
.L7F050A18:
/* 085548 7F050A18 00A01025 */  move  $v0, $a1
/* 08554C 7F050A1C 14A1FFE6 */  bne   $a1, $at, .L7F0509B8
/* 085550 7F050A20 24630004 */   addiu $v1, $v1, 4
.L7F050A24:
/* 085554 7F050A24 8FAD006C */  lw    $t5, 0x6c($sp)
/* 085558 7F050A28 11A00031 */  beqz  $t5, .L7F050AF0
/* 08555C 7F050A2C 00000000 */   nop
/* 085560 7F050A30 100000BB */  b     .L7F050D20
/* 085564 7F050A34 00001025 */   move  $v0, $zero
.L7F050A38:
/* 085568 7F050A38 24010015 */  li    $at, 21
/* 08556C 7F050A3C 1441002C */  bne   $v0, $at, .L7F050AF0
/* 085570 7F050A40 8FAE0088 */   lw    $t6, 0x88($sp)
/* 085574 7F050A44 8DCF0004 */  lw    $t7, 4($t6)
/* 085578 7F050A48 AFA0005C */  sw    $zero, 0x5c($sp)
/* 08557C 7F050A4C 0FC228BF */  jal   get_BONDdata_watch_armor
/* 085580 7F050A50 AFAF0060 */   sw    $t7, 0x60($sp)
/* 085584 7F050A54 8FB80060 */  lw    $t8, 0x60($sp)
/* 085588 7F050A58 24190001 */  li    $t9, 1
/* 08558C 7F050A5C C7040084 */  lwc1  $f4, 0x84($t8)
/* 085590 7F050A60 4600203E */  c.le.s $f4, $f0
/* 085594 7F050A64 00000000 */  nop
/* 085598 7F050A68 45000003 */  bc1f  .L7F050A78
/* 08559C 7F050A6C 00000000 */   nop
/* 0855A0 7F050A70 1000001A */  b     .L7F050ADC
/* 0855A4 7F050A74 AFB9005C */   sw    $t9, 0x5c($sp)
.L7F050A78:
/* 0855A8 7F050A78 0FC26919 */  jal   getPlayerCount
/* 0855AC 7F050A7C 00000000 */   nop
/* 0855B0 7F050A80 28410002 */  slti  $at, $v0, 2
/* 0855B4 7F050A84 54200016 */  bnezl $at, .L7F050AE0
/* 0855B8 7F050A88 8FAB005C */   lw    $t3, 0x5c($sp)
/* 0855BC 7F050A8C 0FC051D6 */  jal   get_scenario
/* 0855C0 7F050A90 00000000 */   nop
/* 0855C4 7F050A94 24010002 */  li    $at, 2
/* 0855C8 7F050A98 14410008 */  bne   $v0, $at, .L7F050ABC
/* 0855CC 7F050A9C 00401825 */   move  $v1, $v0
/* 0855D0 7F050AA0 0FC233E0 */  jal   bondinvIsAliveWithFlag
/* 0855D4 7F050AA4 AFA20058 */   sw    $v0, 0x58($sp)
/* 0855D8 7F050AA8 10400004 */  beqz  $v0, .L7F050ABC
/* 0855DC 7F050AAC 8FA30058 */   lw    $v1, 0x58($sp)
/* 0855E0 7F050AB0 24090001 */  li    $t1, 1
/* 0855E4 7F050AB4 10000009 */  b     .L7F050ADC
/* 0855E8 7F050AB8 AFA9005C */   sw    $t1, 0x5c($sp)
.L7F050ABC:
/* 0855EC 7F050ABC 24010003 */  li    $at, 3
/* 0855F0 7F050AC0 54610007 */  bnel  $v1, $at, .L7F050AE0
/* 0855F4 7F050AC4 8FAB005C */   lw    $t3, 0x5c($sp)
/* 0855F8 7F050AC8 0FC233F0 */  jal   bondinvHasGoldenGun
/* 0855FC 7F050ACC 00000000 */   nop
/* 085600 7F050AD0 10400002 */  beqz  $v0, .L7F050ADC
/* 085604 7F050AD4 240A0001 */   li    $t2, 1
/* 085608 7F050AD8 AFAA005C */  sw    $t2, 0x5c($sp)
.L7F050ADC:
/* 08560C 7F050ADC 8FAB005C */  lw    $t3, 0x5c($sp)
.L7F050AE0:
/* 085610 7F050AE0 11600003 */  beqz  $t3, .L7F050AF0
/* 085614 7F050AE4 00000000 */   nop
/* 085618 7F050AE8 1000008D */  b     .L7F050D20
/* 08561C 7F050AEC 00001025 */   move  $v0, $zero
.L7F050AF0:
/* 085620 7F050AF0 0FC227C4 */  jal   get_curplay_vertical_rotation_in_degrees
/* 085624 7F050AF4 00000000 */   nop
/* 085628 7F050AF8 3C018005 */  lui   $at, %hi(D_800532E0)
/* 08562C 7F050AFC C42632E0 */  lwc1  $f6, %lo(D_800532E0)($at)
/* 085630 7F050B00 3C0C8008 */  lui   $t4, %hi(g_CurrentPlayer)
/* 085634 7F050B04 4606003C */  c.lt.s $f0, $f6
/* 085638 7F050B08 00000000 */  nop
/* 08563C 7F050B0C 45000007 */  bc1f  .L7F050B2C
/* 085640 7F050B10 00000000 */   nop
/* 085644 7F050B14 8D8CA0B0 */  lw    $t4, %lo(g_CurrentPlayer)($t4)
/* 085648 7F050B18 8D8D1270 */  lw    $t5, 0x1270($t4)
/* 08564C 7F050B1C 05A10003 */  bgez  $t5, .L7F050B2C
/* 085650 7F050B20 00000000 */   nop
/* 085654 7F050B24 1000007E */  b     .L7F050D20
/* 085658 7F050B28 00001025 */   move  $v0, $zero
.L7F050B2C:
/* 08565C 7F050B2C 0FC225E6 */  jal   get_curplayer_positiondata
/* 085660 7F050B30 00000000 */   nop
/* 085664 7F050B34 3C0F8008 */  lui   $t7, %hi(g_CurrentPlayer)
/* 085668 7F050B38 8FAE0084 */  lw    $t6, 0x84($sp)
/* 08566C 7F050B3C 8DEFA0B0 */  lw    $t7, %lo(g_CurrentPlayer)($t7)
/* 085670 7F050B40 C44A0008 */  lwc1  $f10, 8($v0)
/* 085674 7F050B44 C452000C */  lwc1  $f18, 0xc($v0)
/* 085678 7F050B48 8DF81270 */  lw    $t8, 0x1270($t7)
/* 08567C 7F050B4C C4460010 */  lwc1  $f6, 0x10($v0)
/* 085680 7F050B50 C5C80058 */  lwc1  $f8, 0x58($t6)
/* 085684 7F050B54 C5D0005C */  lwc1  $f16, 0x5c($t6)
/* 085688 7F050B58 C5C40060 */  lwc1  $f4, 0x60($t6)
/* 08568C 7F050B5C 2B01003C */  slti  $at, $t8, 0x3c
/* 085690 7F050B60 00404025 */  move  $t0, $v0
/* 085694 7F050B64 460A4001 */  sub.s $f0, $f8, $f10
/* 085698 7F050B68 46128301 */  sub.s $f12, $f16, $f18
/* 08569C 7F050B6C 14200022 */  bnez  $at, .L7F050BF8
/* 0856A0 7F050B70 46062081 */   sub.s $f2, $f4, $f6
/* 0856A4 7F050B74 46000202 */  mul.s $f8, $f0, $f0
/* 0856A8 7F050B78 3C018005 */  lui   $at, %hi(D_800532E4)
/* 0856AC 7F050B7C C43232E4 */  lwc1  $f18, %lo(D_800532E4)($at)
/* 0856B0 7F050B80 46021282 */  mul.s $f10, $f2, $f2
/* 0856B4 7F050B84 3C01C3FA */  li    $at, 0xC3FA0000 # -500.000000
/* 0856B8 7F050B88 00001025 */  move  $v0, $zero
/* 0856BC 7F050B8C 460A4400 */  add.s $f16, $f8, $f10
/* 0856C0 7F050B90 4612803E */  c.le.s $f16, $f18
/* 0856C4 7F050B94 00000000 */  nop
/* 0856C8 7F050B98 45000002 */  bc1f  .L7F050BA4
/* 0856CC 7F050B9C 00000000 */   nop
/* 0856D0 7F050BA0 24020001 */  li    $v0, 1
.L7F050BA4:
/* 0856D4 7F050BA4 10400012 */  beqz  $v0, .L7F050BF0
/* 0856D8 7F050BA8 00403025 */   move  $a2, $v0
/* 0856DC 7F050BAC 44812000 */  mtc1  $at, $f4
/* 0856E0 7F050BB0 3C0143FA */  li    $at, 0x43FA0000 # 500.000000
/* 0856E4 7F050BB4 00001025 */  move  $v0, $zero
/* 0856E8 7F050BB8 460C203E */  c.le.s $f4, $f12
/* 0856EC 7F050BBC 00000000 */  nop
/* 0856F0 7F050BC0 45000002 */  bc1f  .L7F050BCC
/* 0856F4 7F050BC4 00000000 */   nop
/* 0856F8 7F050BC8 24020001 */  li    $v0, 1
.L7F050BCC:
/* 0856FC 7F050BCC 10400008 */  beqz  $v0, .L7F050BF0
/* 085700 7F050BD0 00403025 */   move  $a2, $v0
/* 085704 7F050BD4 44813000 */  mtc1  $at, $f6
/* 085708 7F050BD8 00003025 */  move  $a2, $zero
/* 08570C 7F050BDC 4606603E */  c.le.s $f12, $f6
/* 085710 7F050BE0 00000000 */  nop
/* 085714 7F050BE4 45000002 */  bc1f  .L7F050BF0
/* 085718 7F050BE8 00000000 */   nop
/* 08571C 7F050BEC 24060001 */  li    $a2, 1
.L7F050BF0:
/* 085720 7F050BF0 10000021 */  b     .L7F050C78
/* 085724 7F050BF4 00C01825 */   move  $v1, $a2
.L7F050BF8:
/* 085728 7F050BF8 46000202 */  mul.s $f8, $f0, $f0
/* 08572C 7F050BFC 3C018005 */  lui   $at, %hi(D_800532E8)
/* 085730 7F050C00 C43032E8 */  lwc1  $f16, %lo(D_800532E8)($at)
/* 085734 7F050C04 46021282 */  mul.s $f10, $f2, $f2
/* 085738 7F050C08 3C01C348 */  li    $at, 0xC3480000 # -200.000000
/* 08573C 7F050C0C 00001025 */  move  $v0, $zero
/* 085740 7F050C10 460A4480 */  add.s $f18, $f8, $f10
/* 085744 7F050C14 4610903E */  c.le.s $f18, $f16
/* 085748 7F050C18 00000000 */  nop
/* 08574C 7F050C1C 45000002 */  bc1f  .L7F050C28
/* 085750 7F050C20 00000000 */   nop
/* 085754 7F050C24 24020001 */  li    $v0, 1
.L7F050C28:
/* 085758 7F050C28 10400012 */  beqz  $v0, .L7F050C74
/* 08575C 7F050C2C 00403025 */   move  $a2, $v0
/* 085760 7F050C30 44812000 */  mtc1  $at, $f4
/* 085764 7F050C34 3C014348 */  li    $at, 0x43480000 # 200.000000
/* 085768 7F050C38 00001025 */  move  $v0, $zero
/* 08576C 7F050C3C 460C203E */  c.le.s $f4, $f12
/* 085770 7F050C40 00000000 */  nop
/* 085774 7F050C44 45000002 */  bc1f  .L7F050C50
/* 085778 7F050C48 00000000 */   nop
/* 08577C 7F050C4C 24020001 */  li    $v0, 1
.L7F050C50:
/* 085780 7F050C50 10400008 */  beqz  $v0, .L7F050C74
/* 085784 7F050C54 00403025 */   move  $a2, $v0
/* 085788 7F050C58 44813000 */  mtc1  $at, $f6
/* 08578C 7F050C5C 00003025 */  move  $a2, $zero
/* 085790 7F050C60 4606603E */  c.le.s $f12, $f6
/* 085794 7F050C64 00000000 */  nop
/* 085798 7F050C68 45020003 */  bc1fl .L7F050C78
/* 08579C 7F050C6C 00C01825 */   move  $v1, $a2
/* 0857A0 7F050C70 24060001 */  li    $a2, 1
.L7F050C74:
/* 0857A4 7F050C74 00C01825 */  move  $v1, $a2
.L7F050C78:
/* 0857A8 7F050C78 10C00022 */  beqz  $a2, .L7F050D04
/* 0857AC 7F050C7C 8FB90084 */   lw    $t9, 0x84($sp)
/* 0857B0 7F050C80 8F29000C */  lw    $t1, 0xc($t9)
/* 0857B4 7F050C84 8FA20088 */  lw    $v0, 0x88($sp)
/* 0857B8 7F050C88 3C0141F0 */  li    $at, 0x41F00000 # 30.000000
/* 0857BC 7F050C8C 312A1000 */  andi  $t2, $t1, 0x1000
/* 0857C0 7F050C90 1540001C */  bnez  $t2, .L7F050D04
/* 0857C4 7F050C94 27A4003C */   addiu $a0, $sp, 0x3c
/* 0857C8 7F050C98 8D0B0014 */  lw    $t3, 0x14($t0)
/* 0857CC 7F050C9C 44810000 */  mtc1  $at, $f0
/* 0857D0 7F050CA0 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 0857D4 7F050CA4 AFAB003C */  sw    $t3, 0x3c($sp)
/* 0857D8 7F050CA8 C4480010 */  lwc1  $f8, 0x10($v0)
/* 0857DC 7F050CAC 8C470008 */  lw    $a3, 8($v0)
/* 0857E0 7F050CB0 8D060010 */  lw    $a2, 0x10($t0)
/* 0857E4 7F050CB4 8D050008 */  lw    $a1, 8($t0)
/* 0857E8 7F050CB8 44818000 */  mtc1  $at, $f16
/* 0857EC 7F050CBC 44805000 */  mtc1  $zero, $f10
/* 0857F0 7F050CC0 240C0002 */  li    $t4, 2
/* 0857F4 7F050CC4 AFAC0014 */  sw    $t4, 0x14($sp)
/* 0857F8 7F050CC8 AFA30040 */  sw    $v1, 0x40($sp)
/* 0857FC 7F050CCC E7A0001C */  swc1  $f0, 0x1c($sp)
/* 085800 7F050CD0 E7A00018 */  swc1  $f0, 0x18($sp)
/* 085804 7F050CD4 E7A80010 */  swc1  $f8, 0x10($sp)
/* 085808 7F050CD8 E7B00024 */  swc1  $f16, 0x24($sp)
/* 08580C 7F050CDC 0FC2C389 */  jal   stanTestLineUnobstructed
/* 085810 7F050CE0 E7AA0020 */   swc1  $f10, 0x20($sp)
/* 085814 7F050CE4 10400006 */  beqz  $v0, .L7F050D00
/* 085818 7F050CE8 8FA30040 */   lw    $v1, 0x40($sp)
/* 08581C 7F050CEC 8FAE0088 */  lw    $t6, 0x88($sp)
/* 085820 7F050CF0 8FAD003C */  lw    $t5, 0x3c($sp)
/* 085824 7F050CF4 8DCF0014 */  lw    $t7, 0x14($t6)
/* 085828 7F050CF8 11AF0002 */  beq   $t5, $t7, .L7F050D04
/* 08582C 7F050CFC 00000000 */   nop
.L7F050D00:
/* 085830 7F050D00 00001825 */  move  $v1, $zero
.L7F050D04:
/* 085834 7F050D04 10600006 */  beqz  $v1, .L7F050D20
/* 085838 7F050D08 00001025 */   move  $v0, $zero
/* 08583C 7F050D0C 8FA40088 */  lw    $a0, 0x88($sp)
/* 085840 7F050D10 0FC14094 */  jal   collect_or_interact_object
/* 085844 7F050D14 24050001 */   li    $a1, 1
/* 085848 7F050D18 10000002 */  b     .L7F050D24
/* 08584C 7F050D1C 8FBF002C */   lw    $ra, 0x2c($sp)
.L7F050D20:
/* 085850 7F050D20 8FBF002C */  lw    $ra, 0x2c($sp)
.L7F050D24:
/* 085854 7F050D24 27BD0088 */  addiu $sp, $sp, 0x88
/* 085858 7F050D28 03E00008 */  jr    $ra
/* 08585C 7F050D2C 00000000 */   nop
)
#endif

#ifdef VERSION_JP
GLOBAL_ASM(
.late_rodata
glabel D_800532E0
.word 0xbf490fdb /*-0.78539819*/
glabel D_800532E4
.word 0x47ef4200 /*122500.0*/
glabel D_800532E8
.word 0x461c4000 /*10000.0*/
.text
glabel object_collectability_routines
/* 085650 7F050AE0 27BDFF70 */  addiu $sp, $sp, -0x90
/* 085654 7F050AE4 AFBF0034 */  sw    $ra, 0x34($sp)
/* 085658 7F050AE8 AFB00030 */  sw    $s0, 0x30($sp)
/* 08565C 7F050AEC AFA40090 */  sw    $a0, 0x90($sp)
/* 085660 7F050AF0 8C840004 */  lw    $a0, 4($a0)
/* 085664 7F050AF4 0FC13904 */  jal   objIsCollectable
/* 085668 7F050AF8 AFA4008C */   sw    $a0, 0x8c($sp)
/* 08566C 7F050AFC 1040000B */  beqz  $v0, .Ljp7F050B2C
/* 085670 7F050B00 8FA4008C */   lw    $a0, 0x8c($sp)
/* 085674 7F050B04 908F0003 */  lbu   $t7, 3($a0)
/* 085678 7F050B08 24010011 */  li    $at, 17
/* 08567C 7F050B0C 51E10008 */  beql  $t7, $at, .Ljp7F050B30
/* 085680 7F050B10 8FA4008C */   lw    $a0, 0x8c($sp)
/* 085684 7F050B14 8C820008 */  lw    $v0, 8($a0)
/* 085688 7F050B18 0002C2C0 */  sll   $t8, $v0, 0xb
/* 08568C 7F050B1C 0703000B */  bgezl $t8, .Ljp7F050B4C
/* 085690 7F050B20 00024B00 */   sll   $t1, $v0, 0xc
/* 085694 7F050B24 100001BC */  b     .Ljp7F051218
/* 085698 7F050B28 00001025 */   move  $v0, $zero
.Ljp7F050B2C:
/* 08569C 7F050B2C 8FA4008C */  lw    $a0, 0x8c($sp)
.Ljp7F050B30:
/* 0856A0 7F050B30 8C820008 */  lw    $v0, 8($a0)
/* 0856A4 7F050B34 0002CB40 */  sll   $t9, $v0, 0xd
/* 0856A8 7F050B38 07220004 */  bltzl $t9, .Ljp7F050B4C
/* 0856AC 7F050B3C 00024B00 */   sll   $t1, $v0, 0xc
/* 0856B0 7F050B40 100001B5 */  b     .Ljp7F051218
/* 0856B4 7F050B44 00001025 */   move  $v0, $zero
/* 0856B8 7F050B48 00024B00 */  sll   $t1, $v0, 0xc
.Ljp7F050B4C:
/* 0856BC 7F050B4C 05230004 */  bgezl $t1, .Ljp7F050B60
/* 0856C0 7F050B50 8C8A0064 */   lw    $t2, 0x64($a0)
/* 0856C4 7F050B54 100001B0 */  b     .Ljp7F051218
/* 0856C8 7F050B58 00001025 */   move  $v0, $zero
/* 0856CC 7F050B5C 8C8A0064 */  lw    $t2, 0x64($a0)
.Ljp7F050B60:
/* 0856D0 7F050B60 314B0080 */  andi  $t3, $t2, 0x80
/* 0856D4 7F050B64 1160000A */  beqz  $t3, .Ljp7F050B90
/* 0856D8 7F050B68 00000000 */   nop
/* 0856DC 7F050B6C 8C82006C */  lw    $v0, 0x6c($a0)
/* 0856E0 7F050B70 8C4C00BC */  lw    $t4, 0xbc($v0)
/* 0856E4 7F050B74 19800006 */  blez  $t4, .Ljp7F050B90
/* 0856E8 7F050B78 00000000 */   nop
/* 0856EC 7F050B7C 8C4D0090 */  lw    $t5, 0x90($v0)
/* 0856F0 7F050B80 15A00003 */  bnez  $t5, .Ljp7F050B90
/* 0856F4 7F050B84 00000000 */   nop
/* 0856F8 7F050B88 100001A3 */  b     .Ljp7F051218
/* 0856FC 7F050B8C 00001025 */   move  $v0, $zero
.Ljp7F050B90:
/* 085700 7F050B90 0FC0FAAB */  jal   objCanPickupFromSafe
/* 085704 7F050B94 00000000 */   nop
/* 085708 7F050B98 14400003 */  bnez  $v0, .Ljp7F050BA8
/* 08570C 7F050B9C 8FA5008C */   lw    $a1, 0x8c($sp)
/* 085710 7F050BA0 1000019D */  b     .Ljp7F051218
/* 085714 7F050BA4 00001025 */   move  $v0, $zero
.Ljp7F050BA8:
/* 085718 7F050BA8 90A20003 */  lbu   $v0, 3($a1)
/* 08571C 7F050BAC 24010008 */  li    $at, 8
/* 085720 7F050BB0 8FAE0090 */  lw    $t6, 0x90($sp)
/* 085724 7F050BB4 54410060 */  bnel  $v0, $at, .Ljp7F050D38
/* 085728 7F050BB8 24010007 */   li    $at, 7
/* 08572C 7F050BBC 8DD00004 */  lw    $s0, 4($t6)
/* 085730 7F050BC0 2401001A */  li    $at, 26
/* 085734 7F050BC4 82040080 */  lb    $a0, 0x80($s0)
/* 085738 7F050BC8 10810003 */  beq   $a0, $at, .Ljp7F050BD8
/* 08573C 7F050BCC 24010057 */   li    $at, 87
/* 085740 7F050BD0 5481000B */  bnel  $a0, $at, .Ljp7F050C00
/* 085744 7F050BD4 2401001D */   li    $at, 29
.Ljp7F050BD8:
/* 085748 7F050BD8 860F0082 */  lh    $t7, 0x82($s0)
/* 08574C 7F050BDC 05E10005 */  bgez  $t7, .Ljp7F050BF4
/* 085750 7F050BE0 00000000 */   nop
/* 085754 7F050BE4 8CB80064 */  lw    $t8, 0x64($a1)
/* 085758 7F050BE8 33190004 */  andi  $t9, $t8, 4
/* 08575C 7F050BEC 53200004 */  beql  $t9, $zero, .Ljp7F050C00
/* 085760 7F050BF0 2401001D */   li    $at, 29
.Ljp7F050BF4:
/* 085764 7F050BF4 10000188 */  b     .Ljp7F051218
/* 085768 7F050BF8 00001025 */   move  $v0, $zero
/* 08576C 7F050BFC 2401001D */  li    $at, 29
.Ljp7F050C00:
/* 085770 7F050C00 1081000D */  beq   $a0, $at, .Ljp7F050C38
/* 085774 7F050C04 2401001C */   li    $at, 28
/* 085778 7F050C08 1081000B */  beq   $a0, $at, .Ljp7F050C38
/* 08577C 7F050C0C 2401001B */   li    $at, 27
/* 085780 7F050C10 10810009 */  beq   $a0, $at, .Ljp7F050C38
/* 085784 7F050C14 24010021 */   li    $at, 33
/* 085788 7F050C18 10810007 */  beq   $a0, $at, .Ljp7F050C38
/* 08578C 7F050C1C 2401002F */   li    $at, 47
/* 085790 7F050C20 10810005 */  beq   $a0, $at, .Ljp7F050C38
/* 085794 7F050C24 24010030 */   li    $at, 48
/* 085798 7F050C28 10810003 */  beq   $a0, $at, .Ljp7F050C38
/* 08579C 7F050C2C 24010022 */   li    $at, 34
/* 0857A0 7F050C30 5481000B */  bnel  $a0, $at, .Ljp7F050C60
/* 0857A4 7F050C34 24010056 */   li    $at, 86
.Ljp7F050C38:
/* 0857A8 7F050C38 86090082 */  lh    $t1, 0x82($s0)
/* 0857AC 7F050C3C 05210005 */  bgez  $t1, .Ljp7F050C54
/* 0857B0 7F050C40 00000000 */   nop
/* 0857B4 7F050C44 8CAA0064 */  lw    $t2, 0x64($a1)
/* 0857B8 7F050C48 314B0004 */  andi  $t3, $t2, 4
/* 0857BC 7F050C4C 51600004 */  beql  $t3, $zero, .Ljp7F050C60
/* 0857C0 7F050C50 24010056 */   li    $at, 86
.Ljp7F050C54:
/* 0857C4 7F050C54 10000170 */  b     .Ljp7F051218
/* 0857C8 7F050C58 00001025 */   move  $v0, $zero
/* 0857CC 7F050C5C 24010056 */  li    $at, 86
.Ljp7F050C60:
/* 0857D0 7F050C60 14810007 */  bne   $a0, $at, .Ljp7F050C80
/* 0857D4 7F050C64 00000000 */   nop
/* 0857D8 7F050C68 8CAC0064 */  lw    $t4, 0x64($a1)
/* 0857DC 7F050C6C 318D0080 */  andi  $t5, $t4, 0x80
/* 0857E0 7F050C70 11A00003 */  beqz  $t5, .Ljp7F050C80
/* 0857E4 7F050C74 00000000 */   nop
/* 0857E8 7F050C78 10000167 */  b     .Ljp7F051218
/* 0857EC 7F050C7C 00001025 */   move  $v0, $zero
.Ljp7F050C80:
/* 0857F0 7F050C80 0FC232E9 */  jal   bondinvHasInvItem
/* 0857F4 7F050C84 00000000 */   nop
/* 0857F8 7F050C88 104000D9 */  beqz  $v0, .Ljp7F050FF0
/* 0857FC 7F050C8C 00000000 */   nop
/* 085800 7F050C90 0FC1A686 */  jal   get_ammo_type_for_weapon
/* 085804 7F050C94 82040080 */   lb    $a0, 0x80($s0)
/* 085808 7F050C98 104000D5 */  beqz  $v0, .Ljp7F050FF0
/* 08580C 7F050C9C 00000000 */   nop
/* 085810 7F050CA0 0FC1A68F */  jal   get_ammo_count_for_weapon
/* 085814 7F050CA4 82040080 */   lb    $a0, 0x80($s0)
/* 085818 7F050CA8 82040080 */  lb    $a0, 0x80($s0)
/* 08581C 7F050CAC 0FC1A6A7 */  jal   get_max_ammo_for_weapon
/* 085820 7F050CB0 AFA20040 */   sw    $v0, 0x40($sp)
/* 085824 7F050CB4 8FA30040 */  lw    $v1, 0x40($sp)
/* 085828 7F050CB8 0062082A */  slt   $at, $v1, $v0
/* 08582C 7F050CBC 142000CC */  bnez  $at, .Ljp7F050FF0
/* 085830 7F050CC0 00000000 */   nop
/* 085834 7F050CC4 8E020084 */  lw    $v0, 0x84($s0)
/* 085838 7F050CC8 14400004 */  bnez  $v0, .Ljp7F050CDC
/* 08583C 7F050CCC 00000000 */   nop
/* 085840 7F050CD0 820E0081 */  lb    $t6, 0x81($s0)
/* 085844 7F050CD4 05C00015 */  bltz  $t6, .Ljp7F050D2C
/* 085848 7F050CD8 00000000 */   nop
.Ljp7F050CDC:
/* 08584C 7F050CDC 50400005 */  beql  $v0, $zero, .Ljp7F050CF4
/* 085850 7F050CE0 82050081 */   lb    $a1, 0x81($s0)
/* 085854 7F050CE4 80450080 */  lb    $a1, 0x80($v0)
/* 085858 7F050CE8 10000003 */  b     .Ljp7F050CF8
/* 08585C 7F050CEC 00A02025 */   move  $a0, $a1
/* 085860 7F050CF0 82050081 */  lb    $a1, 0x81($s0)
.Ljp7F050CF4:
/* 085864 7F050CF4 00A02025 */  move  $a0, $a1
.Ljp7F050CF8:
/* 085868 7F050CF8 8E0F0008 */  lw    $t7, 8($s0)
/* 08586C 7F050CFC 000FC0C0 */  sll   $t8, $t7, 3
/* 085870 7F050D00 07030004 */  bgezl $t8, .Ljp7F050D14
/* 085874 7F050D04 82040080 */   lb    $a0, 0x80($s0)
/* 085878 7F050D08 10000002 */  b     .Ljp7F050D14
/* 08587C 7F050D0C 82050080 */   lb    $a1, 0x80($s0)
/* 085880 7F050D10 82040080 */  lb    $a0, 0x80($s0)
.Ljp7F050D14:
/* 085884 7F050D14 0FC2330B */  jal   bondinvHasDualWeapon
/* 085888 7F050D18 00000000 */   nop
/* 08588C 7F050D1C 104000B4 */  beqz  $v0, .Ljp7F050FF0
/* 085890 7F050D20 00000000 */   nop
/* 085894 7F050D24 1000013C */  b     .Ljp7F051218
/* 085898 7F050D28 00001025 */   move  $v0, $zero
.Ljp7F050D2C:
/* 08589C 7F050D2C 1000013A */  b     .Ljp7F051218
/* 0858A0 7F050D30 00001025 */   move  $v0, $zero
/* 0858A4 7F050D34 24010007 */  li    $at, 7
.Ljp7F050D38:
/* 0858A8 7F050D38 14410035 */  bne   $v0, $at, .Ljp7F050E10
/* 0858AC 7F050D3C 8FB90090 */   lw    $t9, 0x90($sp)
/* 0858B0 7F050D40 8F300004 */  lw    $s0, 4($t9)
/* 0858B4 7F050D44 0FC1A60B */  jal   check_cur_player_ammo_amount_in_inventory
/* 0858B8 7F050D48 8E040080 */   lw    $a0, 0x80($s0)
/* 0858BC 7F050D4C 8E040080 */  lw    $a0, 0x80($s0)
/* 0858C0 7F050D50 0FC1A630 */  jal   get_max_ammo_for_type
/* 0858C4 7F050D54 AFA20040 */   sw    $v0, 0x40($sp)
/* 0858C8 7F050D58 8FA30040 */  lw    $v1, 0x40($sp)
/* 0858CC 7F050D5C 0062082A */  slt   $at, $v1, $v0
/* 0858D0 7F050D60 142000A3 */  bnez  $at, .Ljp7F050FF0
/* 0858D4 7F050D64 00000000 */   nop
/* 0858D8 7F050D68 8E020080 */  lw    $v0, 0x80($s0)
/* 0858DC 7F050D6C 24010005 */  li    $at, 5
/* 0858E0 7F050D70 54410007 */  bnel  $v0, $at, .Ljp7F050D90
/* 0858E4 7F050D74 24010007 */   li    $at, 7
/* 0858E8 7F050D78 0FC232E9 */  jal   bondinvHasInvItem
/* 0858EC 7F050D7C 2404001A */   li    $a0, 26
/* 0858F0 7F050D80 1040009B */  beqz  $v0, .Ljp7F050FF0
/* 0858F4 7F050D84 00000000 */   nop
/* 0858F8 7F050D88 8E020080 */  lw    $v0, 0x80($s0)
/* 0858FC 7F050D8C 24010007 */  li    $at, 7
.Ljp7F050D90:
/* 085900 7F050D90 54410007 */  bnel  $v0, $at, .Ljp7F050DB0
/* 085904 7F050D94 24010008 */   li    $at, 8
/* 085908 7F050D98 0FC232E9 */  jal   bondinvHasInvItem
/* 08590C 7F050D9C 2404001D */   li    $a0, 29
/* 085910 7F050DA0 10400093 */  beqz  $v0, .Ljp7F050FF0
/* 085914 7F050DA4 00000000 */   nop
/* 085918 7F050DA8 8E020080 */  lw    $v0, 0x80($s0)
/* 08591C 7F050DAC 24010008 */  li    $at, 8
.Ljp7F050DB0:
/* 085920 7F050DB0 54410007 */  bnel  $v0, $at, .Ljp7F050DD0
/* 085924 7F050DB4 24010009 */   li    $at, 9
/* 085928 7F050DB8 0FC232E9 */  jal   bondinvHasInvItem
/* 08592C 7F050DBC 2404001C */   li    $a0, 28
/* 085930 7F050DC0 1040008B */  beqz  $v0, .Ljp7F050FF0
/* 085934 7F050DC4 00000000 */   nop
/* 085938 7F050DC8 8E020080 */  lw    $v0, 0x80($s0)
/* 08593C 7F050DCC 24010009 */  li    $at, 9
.Ljp7F050DD0:
/* 085940 7F050DD0 54410007 */  bnel  $v0, $at, .Ljp7F050DF0
/* 085944 7F050DD4 2401000A */   li    $at, 10
/* 085948 7F050DD8 0FC232E9 */  jal   bondinvHasInvItem
/* 08594C 7F050DDC 2404001B */   li    $a0, 27
/* 085950 7F050DE0 10400083 */  beqz  $v0, .Ljp7F050FF0
/* 085954 7F050DE4 00000000 */   nop
/* 085958 7F050DE8 8E020080 */  lw    $v0, 0x80($s0)
/* 08595C 7F050DEC 2401000A */  li    $at, 10
.Ljp7F050DF0:
/* 085960 7F050DF0 14410005 */  bne   $v0, $at, .Ljp7F050E08
/* 085964 7F050DF4 00000000 */   nop
/* 085968 7F050DF8 0FC232E9 */  jal   bondinvHasInvItem
/* 08596C 7F050DFC 24040003 */   li    $a0, 3
/* 085970 7F050E00 1040007B */  beqz  $v0, .Ljp7F050FF0
/* 085974 7F050E04 00000000 */   nop
.Ljp7F050E08:
/* 085978 7F050E08 10000103 */  b     .Ljp7F051218
/* 08597C 7F050E0C 00001025 */   move  $v0, $zero
.Ljp7F050E10:
/* 085980 7F050E10 24010014 */  li    $at, 20
/* 085984 7F050E14 1441004B */  bne   $v0, $at, .Ljp7F050F44
/* 085988 7F050E18 8FA90090 */   lw    $t1, 0x90($sp)
/* 08598C 7F050E1C 8D300004 */  lw    $s0, 4($t1)
/* 085990 7F050E20 240A0001 */  li    $t2, 1
/* 085994 7F050E24 AFAA0074 */  sw    $t2, 0x74($sp)
/* 085998 7F050E28 0FC100B0 */  jal   objGetDestroyedLevel
/* 08599C 7F050E2C 00A02025 */   move  $a0, $a1
/* 0859A0 7F050E30 10400003 */  beqz  $v0, .Ljp7F050E40
/* 0859A4 7F050E34 02001825 */   move  $v1, $s0
/* 0859A8 7F050E38 100000F7 */  b     .Ljp7F051218
/* 0859AC 7F050E3C 00001025 */   move  $v0, $zero
.Ljp7F050E40:
/* 0859B0 7F050E40 00001025 */  move  $v0, $zero
.Ljp7F050E44:
/* 0859B4 7F050E44 24500001 */  addiu $s0, $v0, 1
/* 0859B8 7F050E48 24010001 */  li    $at, 1
/* 0859BC 7F050E4C 14410002 */  bne   $v0, $at, .Ljp7F050E58
/* 0859C0 7F050E50 AFB0003C */   sw    $s0, 0x3c($sp)
/* 0859C4 7F050E54 24100001 */  li    $s0, 1
.Ljp7F050E58:
/* 0859C8 7F050E58 AFA30038 */  sw    $v1, 0x38($sp)
/* 0859CC 7F050E5C 946C0082 */  lhu   $t4, 0x82($v1)
/* 0859D0 7F050E60 02002025 */  move  $a0, $s0
/* 0859D4 7F050E64 5980002E */  blezl $t4, .Ljp7F050F20
/* 0859D8 7F050E68 8FA2003C */   lw    $v0, 0x3c($sp)
/* 0859DC 7F050E6C 0FC1A60B */  jal   check_cur_player_ammo_amount_in_inventory
/* 0859E0 7F050E70 AFA30038 */   sw    $v1, 0x38($sp)
/* 0859E4 7F050E74 02002025 */  move  $a0, $s0
/* 0859E8 7F050E78 0FC1A630 */  jal   get_max_ammo_for_type
/* 0859EC 7F050E7C AFA20040 */   sw    $v0, 0x40($sp)
/* 0859F0 7F050E80 8FA30040 */  lw    $v1, 0x40($sp)
/* 0859F4 7F050E84 0062082A */  slt   $at, $v1, $v0
/* 0859F8 7F050E88 50200004 */  beql  $at, $zero, .Ljp7F050E9C
/* 0859FC 7F050E8C 24010005 */   li    $at, 5
/* 085A00 7F050E90 10000027 */  b     .Ljp7F050F30
/* 085A04 7F050E94 AFA00074 */   sw    $zero, 0x74($sp)
/* 085A08 7F050E98 24010005 */  li    $at, 5
.Ljp7F050E9C:
/* 085A0C 7F050E9C 56010005 */  bnel  $s0, $at, .Ljp7F050EB4
/* 085A10 7F050EA0 24010007 */   li    $at, 7
/* 085A14 7F050EA4 0FC232E9 */  jal   bondinvHasInvItem
/* 085A18 7F050EA8 2404001A */   li    $a0, 26
/* 085A1C 7F050EAC 10400019 */  beqz  $v0, .Ljp7F050F14
/* 085A20 7F050EB0 24010007 */   li    $at, 7
.Ljp7F050EB4:
/* 085A24 7F050EB4 56010005 */  bnel  $s0, $at, .Ljp7F050ECC
/* 085A28 7F050EB8 24010008 */   li    $at, 8
/* 085A2C 7F050EBC 0FC232E9 */  jal   bondinvHasInvItem
/* 085A30 7F050EC0 2404001D */   li    $a0, 29
/* 085A34 7F050EC4 10400013 */  beqz  $v0, .Ljp7F050F14
/* 085A38 7F050EC8 24010008 */   li    $at, 8
.Ljp7F050ECC:
/* 085A3C 7F050ECC 56010005 */  bnel  $s0, $at, .Ljp7F050EE4
/* 085A40 7F050ED0 24010009 */   li    $at, 9
/* 085A44 7F050ED4 0FC232E9 */  jal   bondinvHasInvItem
/* 085A48 7F050ED8 2404001C */   li    $a0, 28
/* 085A4C 7F050EDC 1040000D */  beqz  $v0, .Ljp7F050F14
/* 085A50 7F050EE0 24010009 */   li    $at, 9
.Ljp7F050EE4:
/* 085A54 7F050EE4 56010005 */  bnel  $s0, $at, .Ljp7F050EFC
/* 085A58 7F050EE8 2401000A */   li    $at, 10
/* 085A5C 7F050EEC 0FC232E9 */  jal   bondinvHasInvItem
/* 085A60 7F050EF0 2404001B */   li    $a0, 27
/* 085A64 7F050EF4 10400007 */  beqz  $v0, .Ljp7F050F14
/* 085A68 7F050EF8 2401000A */   li    $at, 10
.Ljp7F050EFC:
/* 085A6C 7F050EFC 56010008 */  bnel  $s0, $at, .Ljp7F050F20
/* 085A70 7F050F00 8FA2003C */   lw    $v0, 0x3c($sp)
/* 085A74 7F050F04 0FC232E9 */  jal   bondinvHasInvItem
/* 085A78 7F050F08 24040003 */   li    $a0, 3
/* 085A7C 7F050F0C 54400004 */  bnezl $v0, .Ljp7F050F20
/* 085A80 7F050F10 8FA2003C */   lw    $v0, 0x3c($sp)
.Ljp7F050F14:
/* 085A84 7F050F14 10000006 */  b     .Ljp7F050F30
/* 085A88 7F050F18 AFA00074 */   sw    $zero, 0x74($sp)
/* 085A8C 7F050F1C 8FA2003C */  lw    $v0, 0x3c($sp)
.Ljp7F050F20:
/* 085A90 7F050F20 8FA30038 */  lw    $v1, 0x38($sp)
/* 085A94 7F050F24 2401000D */  li    $at, 13
/* 085A98 7F050F28 1441FFC6 */  bne   $v0, $at, .Ljp7F050E44
/* 085A9C 7F050F2C 24630004 */   addiu $v1, $v1, 4
.Ljp7F050F30:
/* 085AA0 7F050F30 8FAD0074 */  lw    $t5, 0x74($sp)
/* 085AA4 7F050F34 11A0002E */  beqz  $t5, .Ljp7F050FF0
/* 085AA8 7F050F38 00000000 */   nop
/* 085AAC 7F050F3C 100000B6 */  b     .Ljp7F051218
/* 085AB0 7F050F40 00001025 */   move  $v0, $zero
.Ljp7F050F44:
/* 085AB4 7F050F44 24010015 */  li    $at, 21
/* 085AB8 7F050F48 14410029 */  bne   $v0, $at, .Ljp7F050FF0
/* 085ABC 7F050F4C 8FAE0090 */   lw    $t6, 0x90($sp)
/* 085AC0 7F050F50 8DD00004 */  lw    $s0, 4($t6)
/* 085AC4 7F050F54 0FC22A8D */  jal   get_BONDdata_watch_armor
/* 085AC8 7F050F58 AFA00064 */   sw    $zero, 0x64($sp)
/* 085ACC 7F050F5C C6040084 */  lwc1  $f4, 0x84($s0)
/* 085AD0 7F050F60 240F0001 */  li    $t7, 1
/* 085AD4 7F050F64 4600203E */  c.le.s $f4, $f0
/* 085AD8 7F050F68 00000000 */  nop
/* 085ADC 7F050F6C 45000003 */  bc1f  .Ljp7F050F7C
/* 085AE0 7F050F70 00000000 */   nop
/* 085AE4 7F050F74 10000019 */  b     .Ljp7F050FDC
/* 085AE8 7F050F78 AFAF0064 */   sw    $t7, 0x64($sp)
.Ljp7F050F7C:
/* 085AEC 7F050F7C 0FC26C01 */  jal   getPlayerCount
/* 085AF0 7F050F80 00000000 */   nop
/* 085AF4 7F050F84 28410002 */  slti  $at, $v0, 2
/* 085AF8 7F050F88 54200015 */  bnezl $at, .Ljp7F050FE0
/* 085AFC 7F050F8C 8FA90064 */   lw    $t1, 0x64($sp)
/* 085B00 7F050F90 0FC05202 */  jal   get_scenario
/* 085B04 7F050F94 00000000 */   nop
/* 085B08 7F050F98 24010002 */  li    $at, 2
/* 085B0C 7F050F9C 14410007 */  bne   $v0, $at, .Ljp7F050FBC
/* 085B10 7F050FA0 00408025 */   move  $s0, $v0
/* 085B14 7F050FA4 0FC23639 */  jal   bondinvIsAliveWithFlag
/* 085B18 7F050FA8 00000000 */   nop
/* 085B1C 7F050FAC 10400003 */  beqz  $v0, .Ljp7F050FBC
/* 085B20 7F050FB0 24180001 */   li    $t8, 1
/* 085B24 7F050FB4 10000009 */  b     .Ljp7F050FDC
/* 085B28 7F050FB8 AFB80064 */   sw    $t8, 0x64($sp)
.Ljp7F050FBC:
/* 085B2C 7F050FBC 24010003 */  li    $at, 3
/* 085B30 7F050FC0 56010007 */  bnel  $s0, $at, .Ljp7F050FE0
/* 085B34 7F050FC4 8FA90064 */   lw    $t1, 0x64($sp)
/* 085B38 7F050FC8 0FC23649 */  jal   bondinvHasGoldenGun
/* 085B3C 7F050FCC 00000000 */   nop
/* 085B40 7F050FD0 10400002 */  beqz  $v0, .Ljp7F050FDC
/* 085B44 7F050FD4 24190001 */   li    $t9, 1
/* 085B48 7F050FD8 AFB90064 */  sw    $t9, 0x64($sp)
.Ljp7F050FDC:
/* 085B4C 7F050FDC 8FA90064 */  lw    $t1, 0x64($sp)
.Ljp7F050FE0:
/* 085B50 7F050FE0 11200003 */  beqz  $t1, .Ljp7F050FF0
/* 085B54 7F050FE4 00000000 */   nop
/* 085B58 7F050FE8 1000008B */  b     .Ljp7F051218
/* 085B5C 7F050FEC 00001025 */   move  $v0, $zero
.Ljp7F050FF0:
/* 085B60 7F050FF0 0FC22992 */  jal   get_curplay_vertical_rotation_in_degrees
/* 085B64 7F050FF4 00000000 */   nop
/* 085B68 7F050FF8 3C018005 */  lui   $at, %hi(D_800532E0) # $at, 0x8005
/* 085B6C 7F050FFC C4263310 */  lwc1  $f6, %lo(D_800532E0)($at)
/* 085B70 7F051000 3C0A8008 */  lui   $t2, %hi(g_CurrentPlayer) # $t2, 0x8008
/* 085B74 7F051004 4606003C */  c.lt.s $f0, $f6
/* 085B78 7F051008 00000000 */  nop
/* 085B7C 7F05100C 45000007 */  bc1f  .Ljp7F05102C
/* 085B80 7F051010 00000000 */   nop
/* 085B84 7F051014 8D4AA120 */  lw    $t2, %lo(g_CurrentPlayer)($t2)
/* 085B88 7F051018 8D4B1270 */  lw    $t3, 0x1270($t2)
/* 085B8C 7F05101C 05610003 */  bgez  $t3, .Ljp7F05102C
/* 085B90 7F051020 00000000 */   nop
/* 085B94 7F051024 1000007C */  b     .Ljp7F051218
/* 085B98 7F051028 00001025 */   move  $v0, $zero
.Ljp7F05102C:
/* 085B9C 7F05102C 0FC2279B */  jal   get_curplayer_positiondata
/* 085BA0 7F051030 00000000 */   nop
/* 085BA4 7F051034 3C0D8008 */  lui   $t5, %hi(g_CurrentPlayer) # $t5, 0x8008
/* 085BA8 7F051038 8FAC008C */  lw    $t4, 0x8c($sp)
/* 085BAC 7F05103C 8DADA120 */  lw    $t5, %lo(g_CurrentPlayer)($t5)
/* 085BB0 7F051040 C44A0008 */  lwc1  $f10, 8($v0)
/* 085BB4 7F051044 C452000C */  lwc1  $f18, 0xc($v0)
/* 085BB8 7F051048 8DAE1270 */  lw    $t6, 0x1270($t5)
/* 085BBC 7F05104C C4460010 */  lwc1  $f6, 0x10($v0)
/* 085BC0 7F051050 C5880058 */  lwc1  $f8, 0x58($t4)
/* 085BC4 7F051054 C590005C */  lwc1  $f16, 0x5c($t4)
/* 085BC8 7F051058 C5840060 */  lwc1  $f4, 0x60($t4)
/* 085BCC 7F05105C 29C1003C */  slti  $at, $t6, 0x3c
/* 085BD0 7F051060 00404025 */  move  $t0, $v0
/* 085BD4 7F051064 460A4001 */  sub.s $f0, $f8, $f10
/* 085BD8 7F051068 46128301 */  sub.s $f12, $f16, $f18
/* 085BDC 7F05106C 14200022 */  bnez  $at, .Ljp7F0510F8
/* 085BE0 7F051070 46062081 */   sub.s $f2, $f4, $f6
/* 085BE4 7F051074 46000202 */  mul.s $f8, $f0, $f0
/* 085BE8 7F051078 3C018005 */  lui   $at, %hi(D_800532E4) # $at, 0x8005
/* 085BEC 7F05107C C4323314 */  lwc1  $f18, %lo(D_800532E4)($at)
/* 085BF0 7F051080 46021282 */  mul.s $f10, $f2, $f2
/* 085BF4 7F051084 3C01C3FA */  li    $at, 0xC3FA0000 # -500.000000
/* 085BF8 7F051088 00001025 */  move  $v0, $zero
/* 085BFC 7F05108C 460A4400 */  add.s $f16, $f8, $f10
/* 085C00 7F051090 4612803E */  c.le.s $f16, $f18
/* 085C04 7F051094 00000000 */  nop
/* 085C08 7F051098 45000002 */  bc1f  .Ljp7F0510A4
/* 085C0C 7F05109C 00000000 */   nop
/* 085C10 7F0510A0 24020001 */  li    $v0, 1
.Ljp7F0510A4:
/* 085C14 7F0510A4 10400012 */  beqz  $v0, .Ljp7F0510F0
/* 085C18 7F0510A8 00401825 */   move  $v1, $v0
/* 085C1C 7F0510AC 44812000 */  mtc1  $at, $f4
/* 085C20 7F0510B0 3C0143FA */  li    $at, 0x43FA0000 # 500.000000
/* 085C24 7F0510B4 00001025 */  move  $v0, $zero
/* 085C28 7F0510B8 460C203E */  c.le.s $f4, $f12
/* 085C2C 7F0510BC 00000000 */  nop
/* 085C30 7F0510C0 45000002 */  bc1f  .Ljp7F0510CC
/* 085C34 7F0510C4 00000000 */   nop
/* 085C38 7F0510C8 24020001 */  li    $v0, 1
.Ljp7F0510CC:
/* 085C3C 7F0510CC 10400008 */  beqz  $v0, .Ljp7F0510F0
/* 085C40 7F0510D0 00401825 */   move  $v1, $v0
/* 085C44 7F0510D4 44813000 */  mtc1  $at, $f6
/* 085C48 7F0510D8 00001825 */  move  $v1, $zero
/* 085C4C 7F0510DC 4606603E */  c.le.s $f12, $f6
/* 085C50 7F0510E0 00000000 */  nop
/* 085C54 7F0510E4 45000002 */  bc1f  .Ljp7F0510F0
/* 085C58 7F0510E8 00000000 */   nop
/* 085C5C 7F0510EC 24030001 */  li    $v1, 1
.Ljp7F0510F0:
/* 085C60 7F0510F0 10000021 */  b     .Ljp7F051178
/* 085C64 7F0510F4 00608025 */   move  $s0, $v1
.Ljp7F0510F8:
/* 085C68 7F0510F8 46000202 */  mul.s $f8, $f0, $f0
/* 085C6C 7F0510FC 3C018005 */  lui   $at, %hi(D_800532E8) # $at, 0x8005
/* 085C70 7F051100 C4303318 */  lwc1  $f16, %lo(D_800532E8)($at)
/* 085C74 7F051104 46021282 */  mul.s $f10, $f2, $f2
/* 085C78 7F051108 3C01C348 */  li    $at, 0xC3480000 # -200.000000
/* 085C7C 7F05110C 00001025 */  move  $v0, $zero
/* 085C80 7F051110 460A4480 */  add.s $f18, $f8, $f10
/* 085C84 7F051114 4610903E */  c.le.s $f18, $f16
/* 085C88 7F051118 00000000 */  nop
/* 085C8C 7F05111C 45000002 */  bc1f  .Ljp7F051128
/* 085C90 7F051120 00000000 */   nop
/* 085C94 7F051124 24020001 */  li    $v0, 1
.Ljp7F051128:
/* 085C98 7F051128 10400012 */  beqz  $v0, .Ljp7F051174
/* 085C9C 7F05112C 00401825 */   move  $v1, $v0
/* 085CA0 7F051130 44812000 */  mtc1  $at, $f4
/* 085CA4 7F051134 3C014348 */  li    $at, 0x43480000 # 200.000000
/* 085CA8 7F051138 00001025 */  move  $v0, $zero
/* 085CAC 7F05113C 460C203E */  c.le.s $f4, $f12
/* 085CB0 7F051140 00000000 */  nop
/* 085CB4 7F051144 45000002 */  bc1f  .Ljp7F051150
/* 085CB8 7F051148 00000000 */   nop
/* 085CBC 7F05114C 24020001 */  li    $v0, 1
.Ljp7F051150:
/* 085CC0 7F051150 10400008 */  beqz  $v0, .Ljp7F051174
/* 085CC4 7F051154 00401825 */   move  $v1, $v0
/* 085CC8 7F051158 44813000 */  mtc1  $at, $f6
/* 085CCC 7F05115C 00001825 */  move  $v1, $zero
/* 085CD0 7F051160 4606603E */  c.le.s $f12, $f6
/* 085CD4 7F051164 00000000 */  nop
/* 085CD8 7F051168 45020003 */  bc1fl .Ljp7F051178
/* 085CDC 7F05116C 00608025 */   move  $s0, $v1
/* 085CE0 7F051170 24030001 */  li    $v1, 1
.Ljp7F051174:
/* 085CE4 7F051174 00608025 */  move  $s0, $v1
.Ljp7F051178:
/* 085CE8 7F051178 10600020 */  beqz  $v1, .Ljp7F0511FC
/* 085CEC 7F05117C 8FAF008C */   lw    $t7, 0x8c($sp)
/* 085CF0 7F051180 8DF8000C */  lw    $t8, 0xc($t7)
/* 085CF4 7F051184 8FA20090 */  lw    $v0, 0x90($sp)
/* 085CF8 7F051188 3C0141F0 */  li    $at, 0x41F00000 # 30.000000
/* 085CFC 7F05118C 33191000 */  andi  $t9, $t8, 0x1000
/* 085D00 7F051190 1720001A */  bnez  $t9, .Ljp7F0511FC
/* 085D04 7F051194 27A40044 */   addiu $a0, $sp, 0x44
/* 085D08 7F051198 8D090014 */  lw    $t1, 0x14($t0)
/* 085D0C 7F05119C 44810000 */  mtc1  $at, $f0
/* 085D10 7F0511A0 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 085D14 7F0511A4 AFA90044 */  sw    $t1, 0x44($sp)
/* 085D18 7F0511A8 C4480010 */  lwc1  $f8, 0x10($v0)
/* 085D1C 7F0511AC 8C470008 */  lw    $a3, 8($v0)
/* 085D20 7F0511B0 8D060010 */  lw    $a2, 0x10($t0)
/* 085D24 7F0511B4 8D050008 */  lw    $a1, 8($t0)
/* 085D28 7F0511B8 44818000 */  mtc1  $at, $f16
/* 085D2C 7F0511BC 44805000 */  mtc1  $zero, $f10
/* 085D30 7F0511C0 240A0002 */  li    $t2, 2
/* 085D34 7F0511C4 AFAA0014 */  sw    $t2, 0x14($sp)
/* 085D38 7F0511C8 E7A0001C */  swc1  $f0, 0x1c($sp)
/* 085D3C 7F0511CC E7A00018 */  swc1  $f0, 0x18($sp)
/* 085D40 7F0511D0 E7A80010 */  swc1  $f8, 0x10($sp)
/* 085D44 7F0511D4 E7B00024 */  swc1  $f16, 0x24($sp)
/* 085D48 7F0511D8 0FC2C675 */  jal   stanTestLineUnobstructed
/* 085D4C 7F0511DC E7AA0020 */   swc1  $f10, 0x20($sp)
/* 085D50 7F0511E0 10400005 */  beqz  $v0, .Ljp7F0511F8
/* 085D54 7F0511E4 8FAC0090 */   lw    $t4, 0x90($sp)
/* 085D58 7F0511E8 8FAB0044 */  lw    $t3, 0x44($sp)
/* 085D5C 7F0511EC 8D8D0014 */  lw    $t5, 0x14($t4)
/* 085D60 7F0511F0 116D0002 */  beq   $t3, $t5, .Ljp7F0511FC
/* 085D64 7F0511F4 00000000 */   nop
.Ljp7F0511F8:
/* 085D68 7F0511F8 00008025 */  move  $s0, $zero
.Ljp7F0511FC:
/* 085D6C 7F0511FC 12000006 */  beqz  $s0, .Ljp7F051218
/* 085D70 7F051200 00001025 */   move  $v0, $zero
/* 085D74 7F051204 8FA40090 */  lw    $a0, 0x90($sp)
/* 085D78 7F051208 0FC14195 */  jal   collect_or_interact_object
/* 085D7C 7F05120C 24050001 */   li    $a1, 1
/* 085D80 7F051210 10000002 */  b     .Ljp7F05121C
/* 085D84 7F051214 8FBF0034 */   lw    $ra, 0x34($sp)
.Ljp7F051218:
/* 085D88 7F051218 8FBF0034 */  lw    $ra, 0x34($sp)
.Ljp7F05121C:
/* 085D8C 7F05121C 8FB00030 */  lw    $s0, 0x30($sp)
/* 085D90 7F051220 27BD0090 */  addiu $sp, $sp, 0x90
/* 085D94 7F051224 03E00008 */  jr    $ra
/* 085D98 7F051228 00000000 */   nop
)
#endif

#ifdef VERSION_EU
GLOBAL_ASM(
.late_rodata
glabel D_800532E0
.word 0xbf490fdb /*-0.78539819*/
glabel D_800532E4
.word 0x47ef4200 /*122500.0*/
glabel D_800532E8
.word 0x461c4000 /*10000.0*/
.text
glabel object_collectability_routines
/* 085650 7F050AE0 27BDFF70 */  addiu $sp, $sp, -0x90
/* 085654 7F050AE4 AFBF0034 */  sw    $ra, 0x34($sp)
/* 085658 7F050AE8 AFB00030 */  sw    $s0, 0x30($sp)
/* 08565C 7F050AEC AFA40090 */  sw    $a0, 0x90($sp)
/* 085660 7F050AF0 8C840004 */  lw    $a0, 4($a0)
/* 085664 7F050AF4 0FC13904 */  jal   objIsCollectable
/* 085668 7F050AF8 AFA4008C */   sw    $a0, 0x8c($sp)
/* 08566C 7F050AFC 1040000B */  beqz  $v0, .Ljp7F050B2C
/* 085670 7F050B00 8FA4008C */   lw    $a0, 0x8c($sp)
/* 085674 7F050B04 908F0003 */  lbu   $t7, 3($a0)
/* 085678 7F050B08 24010011 */  li    $at, 17
/* 08567C 7F050B0C 51E10008 */  beql  $t7, $at, .Ljp7F050B30
/* 085680 7F050B10 8FA4008C */   lw    $a0, 0x8c($sp)
/* 085684 7F050B14 8C820008 */  lw    $v0, 8($a0)
/* 085688 7F050B18 0002C2C0 */  sll   $t8, $v0, 0xb
/* 08568C 7F050B1C 0703000B */  bgezl $t8, .Ljp7F050B4C
/* 085690 7F050B20 00024B00 */   sll   $t1, $v0, 0xc
/* 085694 7F050B24 100001BC */  b     .Ljp7F051218
/* 085698 7F050B28 00001025 */   move  $v0, $zero
.Ljp7F050B2C:
/* 08569C 7F050B2C 8FA4008C */  lw    $a0, 0x8c($sp)
.Ljp7F050B30:
/* 0856A0 7F050B30 8C820008 */  lw    $v0, 8($a0)
/* 0856A4 7F050B34 0002CB40 */  sll   $t9, $v0, 0xd
/* 0856A8 7F050B38 07220004 */  bltzl $t9, .Ljp7F050B4C
/* 0856AC 7F050B3C 00024B00 */   sll   $t1, $v0, 0xc
/* 0856B0 7F050B40 100001B5 */  b     .Ljp7F051218
/* 0856B4 7F050B44 00001025 */   move  $v0, $zero
/* 0856B8 7F050B48 00024B00 */  sll   $t1, $v0, 0xc
.Ljp7F050B4C:
/* 0856BC 7F050B4C 05230004 */  bgezl $t1, .Ljp7F050B60
/* 0856C0 7F050B50 8C8A0064 */   lw    $t2, 0x64($a0)
/* 0856C4 7F050B54 100001B0 */  b     .Ljp7F051218
/* 0856C8 7F050B58 00001025 */   move  $v0, $zero
/* 0856CC 7F050B5C 8C8A0064 */  lw    $t2, 0x64($a0)
.Ljp7F050B60:
/* 0856D0 7F050B60 314B0080 */  andi  $t3, $t2, 0x80
/* 0856D4 7F050B64 1160000A */  beqz  $t3, .Ljp7F050B90
/* 0856D8 7F050B68 00000000 */   nop
/* 0856DC 7F050B6C 8C82006C */  lw    $v0, 0x6c($a0)
/* 0856E0 7F050B70 8C4C00BC */  lw    $t4, 0xbc($v0)
/* 0856E4 7F050B74 19800006 */  blez  $t4, .Ljp7F050B90
/* 0856E8 7F050B78 00000000 */   nop
/* 0856EC 7F050B7C 8C4D0090 */  lw    $t5, 0x90($v0)
/* 0856F0 7F050B80 15A00003 */  bnez  $t5, .Ljp7F050B90
/* 0856F4 7F050B84 00000000 */   nop
/* 0856F8 7F050B88 100001A3 */  b     .Ljp7F051218
/* 0856FC 7F050B8C 00001025 */   move  $v0, $zero
.Ljp7F050B90:
/* 085700 7F050B90 0FC0FAAB */  jal   objCanPickupFromSafe
/* 085704 7F050B94 00000000 */   nop
/* 085708 7F050B98 14400003 */  bnez  $v0, .Ljp7F050BA8
/* 08570C 7F050B9C 8FA5008C */   lw    $a1, 0x8c($sp)
/* 085710 7F050BA0 1000019D */  b     .Ljp7F051218
/* 085714 7F050BA4 00001025 */   move  $v0, $zero
.Ljp7F050BA8:
/* 085718 7F050BA8 90A20003 */  lbu   $v0, 3($a1)
/* 08571C 7F050BAC 24010008 */  li    $at, 8
/* 085720 7F050BB0 8FAE0090 */  lw    $t6, 0x90($sp)
/* 085724 7F050BB4 54410060 */  bnel  $v0, $at, .Ljp7F050D38
/* 085728 7F050BB8 24010007 */   li    $at, 7
/* 08572C 7F050BBC 8DD00004 */  lw    $s0, 4($t6)
/* 085730 7F050BC0 2401001A */  li    $at, 26
/* 085734 7F050BC4 82040080 */  lb    $a0, 0x80($s0)
/* 085738 7F050BC8 10810003 */  beq   $a0, $at, .Ljp7F050BD8
/* 08573C 7F050BCC 24010057 */   li    $at, 87
/* 085740 7F050BD0 5481000B */  bnel  $a0, $at, .Ljp7F050C00
/* 085744 7F050BD4 2401001D */   li    $at, 29
.Ljp7F050BD8:
/* 085748 7F050BD8 860F0082 */  lh    $t7, 0x82($s0)
/* 08574C 7F050BDC 05E10005 */  bgez  $t7, .Ljp7F050BF4
/* 085750 7F050BE0 00000000 */   nop
/* 085754 7F050BE4 8CB80064 */  lw    $t8, 0x64($a1)
/* 085758 7F050BE8 33190004 */  andi  $t9, $t8, 4
/* 08575C 7F050BEC 53200004 */  beql  $t9, $zero, .Ljp7F050C00
/* 085760 7F050BF0 2401001D */   li    $at, 29
.Ljp7F050BF4:
/* 085764 7F050BF4 10000188 */  b     .Ljp7F051218
/* 085768 7F050BF8 00001025 */   move  $v0, $zero
/* 08576C 7F050BFC 2401001D */  li    $at, 29
.Ljp7F050C00:
/* 085770 7F050C00 1081000D */  beq   $a0, $at, .Ljp7F050C38
/* 085774 7F050C04 2401001C */   li    $at, 28
/* 085778 7F050C08 1081000B */  beq   $a0, $at, .Ljp7F050C38
/* 08577C 7F050C0C 2401001B */   li    $at, 27
/* 085780 7F050C10 10810009 */  beq   $a0, $at, .Ljp7F050C38
/* 085784 7F050C14 24010021 */   li    $at, 33
/* 085788 7F050C18 10810007 */  beq   $a0, $at, .Ljp7F050C38
/* 08578C 7F050C1C 2401002F */   li    $at, 47
/* 085790 7F050C20 10810005 */  beq   $a0, $at, .Ljp7F050C38
/* 085794 7F050C24 24010030 */   li    $at, 48
/* 085798 7F050C28 10810003 */  beq   $a0, $at, .Ljp7F050C38
/* 08579C 7F050C2C 24010022 */   li    $at, 34
/* 0857A0 7F050C30 5481000B */  bnel  $a0, $at, .Ljp7F050C60
/* 0857A4 7F050C34 24010056 */   li    $at, 86
.Ljp7F050C38:
/* 0857A8 7F050C38 86090082 */  lh    $t1, 0x82($s0)
/* 0857AC 7F050C3C 05210005 */  bgez  $t1, .Ljp7F050C54
/* 0857B0 7F050C40 00000000 */   nop
/* 0857B4 7F050C44 8CAA0064 */  lw    $t2, 0x64($a1)
/* 0857B8 7F050C48 314B0004 */  andi  $t3, $t2, 4
/* 0857BC 7F050C4C 51600004 */  beql  $t3, $zero, .Ljp7F050C60
/* 0857C0 7F050C50 24010056 */   li    $at, 86
.Ljp7F050C54:
/* 0857C4 7F050C54 10000170 */  b     .Ljp7F051218
/* 0857C8 7F050C58 00001025 */   move  $v0, $zero
/* 0857CC 7F050C5C 24010056 */  li    $at, 86
.Ljp7F050C60:
/* 0857D0 7F050C60 14810007 */  bne   $a0, $at, .Ljp7F050C80
/* 0857D4 7F050C64 00000000 */   nop
/* 0857D8 7F050C68 8CAC0064 */  lw    $t4, 0x64($a1)
/* 0857DC 7F050C6C 318D0080 */  andi  $t5, $t4, 0x80
/* 0857E0 7F050C70 11A00003 */  beqz  $t5, .Ljp7F050C80
/* 0857E4 7F050C74 00000000 */   nop
/* 0857E8 7F050C78 10000167 */  b     .Ljp7F051218
/* 0857EC 7F050C7C 00001025 */   move  $v0, $zero
.Ljp7F050C80:
/* 0857F0 7F050C80 0FC232E9 */  jal   bondinvHasInvItem
/* 0857F4 7F050C84 00000000 */   nop
/* 0857F8 7F050C88 104000D9 */  beqz  $v0, .Ljp7F050FF0
/* 0857FC 7F050C8C 00000000 */   nop
/* 085800 7F050C90 0FC1A686 */  jal   get_ammo_type_for_weapon
/* 085804 7F050C94 82040080 */   lb    $a0, 0x80($s0)
/* 085808 7F050C98 104000D5 */  beqz  $v0, .Ljp7F050FF0
/* 08580C 7F050C9C 00000000 */   nop
/* 085810 7F050CA0 0FC1A68F */  jal   get_ammo_count_for_weapon
/* 085814 7F050CA4 82040080 */   lb    $a0, 0x80($s0)
/* 085818 7F050CA8 82040080 */  lb    $a0, 0x80($s0)
/* 08581C 7F050CAC 0FC1A6A7 */  jal   get_max_ammo_for_weapon
/* 085820 7F050CB0 AFA20040 */   sw    $v0, 0x40($sp)
/* 085824 7F050CB4 8FA30040 */  lw    $v1, 0x40($sp)
/* 085828 7F050CB8 0062082A */  slt   $at, $v1, $v0
/* 08582C 7F050CBC 142000CC */  bnez  $at, .Ljp7F050FF0
/* 085830 7F050CC0 00000000 */   nop
/* 085834 7F050CC4 8E020084 */  lw    $v0, 0x84($s0)
/* 085838 7F050CC8 14400004 */  bnez  $v0, .Ljp7F050CDC
/* 08583C 7F050CCC 00000000 */   nop
/* 085840 7F050CD0 820E0081 */  lb    $t6, 0x81($s0)
/* 085844 7F050CD4 05C00015 */  bltz  $t6, .Ljp7F050D2C
/* 085848 7F050CD8 00000000 */   nop
.Ljp7F050CDC:
/* 08584C 7F050CDC 50400005 */  beql  $v0, $zero, .Ljp7F050CF4
/* 085850 7F050CE0 82050081 */   lb    $a1, 0x81($s0)
/* 085854 7F050CE4 80450080 */  lb    $a1, 0x80($v0)
/* 085858 7F050CE8 10000003 */  b     .Ljp7F050CF8
/* 08585C 7F050CEC 00A02025 */   move  $a0, $a1
/* 085860 7F050CF0 82050081 */  lb    $a1, 0x81($s0)
.Ljp7F050CF4:
/* 085864 7F050CF4 00A02025 */  move  $a0, $a1
.Ljp7F050CF8:
/* 085868 7F050CF8 8E0F0008 */  lw    $t7, 8($s0)
/* 08586C 7F050CFC 000FC0C0 */  sll   $t8, $t7, 3
/* 085870 7F050D00 07030004 */  bgezl $t8, .Ljp7F050D14
/* 085874 7F050D04 82040080 */   lb    $a0, 0x80($s0)
/* 085878 7F050D08 10000002 */  b     .Ljp7F050D14
/* 08587C 7F050D0C 82050080 */   lb    $a1, 0x80($s0)
/* 085880 7F050D10 82040080 */  lb    $a0, 0x80($s0)
.Ljp7F050D14:
/* 085884 7F050D14 0FC2330B */  jal   bondinvHasDualWeapon
/* 085888 7F050D18 00000000 */   nop
/* 08588C 7F050D1C 104000B4 */  beqz  $v0, .Ljp7F050FF0
/* 085890 7F050D20 00000000 */   nop
/* 085894 7F050D24 1000013C */  b     .Ljp7F051218
/* 085898 7F050D28 00001025 */   move  $v0, $zero
.Ljp7F050D2C:
/* 08589C 7F050D2C 1000013A */  b     .Ljp7F051218
/* 0858A0 7F050D30 00001025 */   move  $v0, $zero
/* 0858A4 7F050D34 24010007 */  li    $at, 7
.Ljp7F050D38:
/* 0858A8 7F050D38 14410035 */  bne   $v0, $at, .Ljp7F050E10
/* 0858AC 7F050D3C 8FB90090 */   lw    $t9, 0x90($sp)
/* 0858B0 7F050D40 8F300004 */  lw    $s0, 4($t9)
/* 0858B4 7F050D44 0FC1A60B */  jal   check_cur_player_ammo_amount_in_inventory
/* 0858B8 7F050D48 8E040080 */   lw    $a0, 0x80($s0)
/* 0858BC 7F050D4C 8E040080 */  lw    $a0, 0x80($s0)
/* 0858C0 7F050D50 0FC1A630 */  jal   get_max_ammo_for_type
/* 0858C4 7F050D54 AFA20040 */   sw    $v0, 0x40($sp)
/* 0858C8 7F050D58 8FA30040 */  lw    $v1, 0x40($sp)
/* 0858CC 7F050D5C 0062082A */  slt   $at, $v1, $v0
/* 0858D0 7F050D60 142000A3 */  bnez  $at, .Ljp7F050FF0
/* 0858D4 7F050D64 00000000 */   nop
/* 0858D8 7F050D68 8E020080 */  lw    $v0, 0x80($s0)
/* 0858DC 7F050D6C 24010005 */  li    $at, 5
/* 0858E0 7F050D70 54410007 */  bnel  $v0, $at, .Ljp7F050D90
/* 0858E4 7F050D74 24010007 */   li    $at, 7
/* 0858E8 7F050D78 0FC232E9 */  jal   bondinvHasInvItem
/* 0858EC 7F050D7C 2404001A */   li    $a0, 26
/* 0858F0 7F050D80 1040009B */  beqz  $v0, .Ljp7F050FF0
/* 0858F4 7F050D84 00000000 */   nop
/* 0858F8 7F050D88 8E020080 */  lw    $v0, 0x80($s0)
/* 0858FC 7F050D8C 24010007 */  li    $at, 7
.Ljp7F050D90:
/* 085900 7F050D90 54410007 */  bnel  $v0, $at, .Ljp7F050DB0
/* 085904 7F050D94 24010008 */   li    $at, 8
/* 085908 7F050D98 0FC232E9 */  jal   bondinvHasInvItem
/* 08590C 7F050D9C 2404001D */   li    $a0, 29
/* 085910 7F050DA0 10400093 */  beqz  $v0, .Ljp7F050FF0
/* 085914 7F050DA4 00000000 */   nop
/* 085918 7F050DA8 8E020080 */  lw    $v0, 0x80($s0)
/* 08591C 7F050DAC 24010008 */  li    $at, 8
.Ljp7F050DB0:
/* 085920 7F050DB0 54410007 */  bnel  $v0, $at, .Ljp7F050DD0
/* 085924 7F050DB4 24010009 */   li    $at, 9
/* 085928 7F050DB8 0FC232E9 */  jal   bondinvHasInvItem
/* 08592C 7F050DBC 2404001C */   li    $a0, 28
/* 085930 7F050DC0 1040008B */  beqz  $v0, .Ljp7F050FF0
/* 085934 7F050DC4 00000000 */   nop
/* 085938 7F050DC8 8E020080 */  lw    $v0, 0x80($s0)
/* 08593C 7F050DCC 24010009 */  li    $at, 9
.Ljp7F050DD0:
/* 085940 7F050DD0 54410007 */  bnel  $v0, $at, .Ljp7F050DF0
/* 085944 7F050DD4 2401000A */   li    $at, 10
/* 085948 7F050DD8 0FC232E9 */  jal   bondinvHasInvItem
/* 08594C 7F050DDC 2404001B */   li    $a0, 27
/* 085950 7F050DE0 10400083 */  beqz  $v0, .Ljp7F050FF0
/* 085954 7F050DE4 00000000 */   nop
/* 085958 7F050DE8 8E020080 */  lw    $v0, 0x80($s0)
/* 08595C 7F050DEC 2401000A */  li    $at, 10
.Ljp7F050DF0:
/* 085960 7F050DF0 14410005 */  bne   $v0, $at, .Ljp7F050E08
/* 085964 7F050DF4 00000000 */   nop
/* 085968 7F050DF8 0FC232E9 */  jal   bondinvHasInvItem
/* 08596C 7F050DFC 24040003 */   li    $a0, 3
/* 085970 7F050E00 1040007B */  beqz  $v0, .Ljp7F050FF0
/* 085974 7F050E04 00000000 */   nop
.Ljp7F050E08:
/* 085978 7F050E08 10000103 */  b     .Ljp7F051218
/* 08597C 7F050E0C 00001025 */   move  $v0, $zero
.Ljp7F050E10:
/* 085980 7F050E10 24010014 */  li    $at, 20
/* 085984 7F050E14 1441004B */  bne   $v0, $at, .Ljp7F050F44
/* 085988 7F050E18 8FA90090 */   lw    $t1, 0x90($sp)
/* 08598C 7F050E1C 8D300004 */  lw    $s0, 4($t1)
/* 085990 7F050E20 240A0001 */  li    $t2, 1
/* 085994 7F050E24 AFAA0074 */  sw    $t2, 0x74($sp)
/* 085998 7F050E28 0FC100B0 */  jal   objGetDestroyedLevel
/* 08599C 7F050E2C 00A02025 */   move  $a0, $a1
/* 0859A0 7F050E30 10400003 */  beqz  $v0, .Ljp7F050E40
/* 0859A4 7F050E34 02001825 */   move  $v1, $s0
/* 0859A8 7F050E38 100000F7 */  b     .Ljp7F051218
/* 0859AC 7F050E3C 00001025 */   move  $v0, $zero
.Ljp7F050E40:
/* 0859B0 7F050E40 00001025 */  move  $v0, $zero
.Ljp7F050E44:
/* 0859B4 7F050E44 24500001 */  addiu $s0, $v0, 1
/* 0859B8 7F050E48 24010001 */  li    $at, 1
/* 0859BC 7F050E4C 14410002 */  bne   $v0, $at, .Ljp7F050E58
/* 0859C0 7F050E50 AFB0003C */   sw    $s0, 0x3c($sp)
/* 0859C4 7F050E54 24100001 */  li    $s0, 1
.Ljp7F050E58:
/* 0859C8 7F050E58 AFA30038 */  sw    $v1, 0x38($sp)
/* 0859CC 7F050E5C 946C0082 */  lhu   $t4, 0x82($v1)
/* 0859D0 7F050E60 02002025 */  move  $a0, $s0
/* 0859D4 7F050E64 5980002E */  blezl $t4, .Ljp7F050F20
/* 0859D8 7F050E68 8FA2003C */   lw    $v0, 0x3c($sp)
/* 0859DC 7F050E6C 0FC1A60B */  jal   check_cur_player_ammo_amount_in_inventory
/* 0859E0 7F050E70 AFA30038 */   sw    $v1, 0x38($sp)
/* 0859E4 7F050E74 02002025 */  move  $a0, $s0
/* 0859E8 7F050E78 0FC1A630 */  jal   get_max_ammo_for_type
/* 0859EC 7F050E7C AFA20040 */   sw    $v0, 0x40($sp)
/* 0859F0 7F050E80 8FA30040 */  lw    $v1, 0x40($sp)
/* 0859F4 7F050E84 0062082A */  slt   $at, $v1, $v0
/* 0859F8 7F050E88 50200004 */  beql  $at, $zero, .Ljp7F050E9C
/* 0859FC 7F050E8C 24010005 */   li    $at, 5
/* 085A00 7F050E90 10000027 */  b     .Ljp7F050F30
/* 085A04 7F050E94 AFA00074 */   sw    $zero, 0x74($sp)
/* 085A08 7F050E98 24010005 */  li    $at, 5
.Ljp7F050E9C:
/* 085A0C 7F050E9C 56010005 */  bnel  $s0, $at, .Ljp7F050EB4
/* 085A10 7F050EA0 24010007 */   li    $at, 7
/* 085A14 7F050EA4 0FC232E9 */  jal   bondinvHasInvItem
/* 085A18 7F050EA8 2404001A */   li    $a0, 26
/* 085A1C 7F050EAC 10400019 */  beqz  $v0, .Ljp7F050F14
/* 085A20 7F050EB0 24010007 */   li    $at, 7
.Ljp7F050EB4:
/* 085A24 7F050EB4 56010005 */  bnel  $s0, $at, .Ljp7F050ECC
/* 085A28 7F050EB8 24010008 */   li    $at, 8
/* 085A2C 7F050EBC 0FC232E9 */  jal   bondinvHasInvItem
/* 085A30 7F050EC0 2404001D */   li    $a0, 29
/* 085A34 7F050EC4 10400013 */  beqz  $v0, .Ljp7F050F14
/* 085A38 7F050EC8 24010008 */   li    $at, 8
.Ljp7F050ECC:
/* 085A3C 7F050ECC 56010005 */  bnel  $s0, $at, .Ljp7F050EE4
/* 085A40 7F050ED0 24010009 */   li    $at, 9
/* 085A44 7F050ED4 0FC232E9 */  jal   bondinvHasInvItem
/* 085A48 7F050ED8 2404001C */   li    $a0, 28
/* 085A4C 7F050EDC 1040000D */  beqz  $v0, .Ljp7F050F14
/* 085A50 7F050EE0 24010009 */   li    $at, 9
.Ljp7F050EE4:
/* 085A54 7F050EE4 56010005 */  bnel  $s0, $at, .Ljp7F050EFC
/* 085A58 7F050EE8 2401000A */   li    $at, 10
/* 085A5C 7F050EEC 0FC232E9 */  jal   bondinvHasInvItem
/* 085A60 7F050EF0 2404001B */   li    $a0, 27
/* 085A64 7F050EF4 10400007 */  beqz  $v0, .Ljp7F050F14
/* 085A68 7F050EF8 2401000A */   li    $at, 10
.Ljp7F050EFC:
/* 085A6C 7F050EFC 56010008 */  bnel  $s0, $at, .Ljp7F050F20
/* 085A70 7F050F00 8FA2003C */   lw    $v0, 0x3c($sp)
/* 085A74 7F050F04 0FC232E9 */  jal   bondinvHasInvItem
/* 085A78 7F050F08 24040003 */   li    $a0, 3
/* 085A7C 7F050F0C 54400004 */  bnezl $v0, .Ljp7F050F20
/* 085A80 7F050F10 8FA2003C */   lw    $v0, 0x3c($sp)
.Ljp7F050F14:
/* 085A84 7F050F14 10000006 */  b     .Ljp7F050F30
/* 085A88 7F050F18 AFA00074 */   sw    $zero, 0x74($sp)
/* 085A8C 7F050F1C 8FA2003C */  lw    $v0, 0x3c($sp)
.Ljp7F050F20:
/* 085A90 7F050F20 8FA30038 */  lw    $v1, 0x38($sp)
/* 085A94 7F050F24 2401000D */  li    $at, 13
/* 085A98 7F050F28 1441FFC6 */  bne   $v0, $at, .Ljp7F050E44
/* 085A9C 7F050F2C 24630004 */   addiu $v1, $v1, 4
.Ljp7F050F30:
/* 085AA0 7F050F30 8FAD0074 */  lw    $t5, 0x74($sp)
/* 085AA4 7F050F34 11A0002E */  beqz  $t5, .Ljp7F050FF0
/* 085AA8 7F050F38 00000000 */   nop
/* 085AAC 7F050F3C 100000B6 */  b     .Ljp7F051218
/* 085AB0 7F050F40 00001025 */   move  $v0, $zero
.Ljp7F050F44:
/* 085AB4 7F050F44 24010015 */  li    $at, 21
/* 085AB8 7F050F48 14410029 */  bne   $v0, $at, .Ljp7F050FF0
/* 085ABC 7F050F4C 8FAE0090 */   lw    $t6, 0x90($sp)
/* 085AC0 7F050F50 8DD00004 */  lw    $s0, 4($t6)
/* 085AC4 7F050F54 0FC22A8D */  jal   get_BONDdata_watch_armor
/* 085AC8 7F050F58 AFA00064 */   sw    $zero, 0x64($sp)
/* 085ACC 7F050F5C C6040084 */  lwc1  $f4, 0x84($s0)
/* 085AD0 7F050F60 240F0001 */  li    $t7, 1
/* 085AD4 7F050F64 4600203E */  c.le.s $f4, $f0
/* 085AD8 7F050F68 00000000 */  nop
/* 085ADC 7F050F6C 45000003 */  bc1f  .Ljp7F050F7C
/* 085AE0 7F050F70 00000000 */   nop
/* 085AE4 7F050F74 10000019 */  b     .Ljp7F050FDC
/* 085AE8 7F050F78 AFAF0064 */   sw    $t7, 0x64($sp)
.Ljp7F050F7C:
/* 085AEC 7F050F7C 0FC26C01 */  jal   getPlayerCount
/* 085AF0 7F050F80 00000000 */   nop
/* 085AF4 7F050F84 28410002 */  slti  $at, $v0, 2
/* 085AF8 7F050F88 54200015 */  bnezl $at, .Ljp7F050FE0
/* 085AFC 7F050F8C 8FA90064 */   lw    $t1, 0x64($sp)
/* 085B00 7F050F90 0FC05202 */  jal   get_scenario
/* 085B04 7F050F94 00000000 */   nop
/* 085B08 7F050F98 24010002 */  li    $at, 2
/* 085B0C 7F050F9C 14410007 */  bne   $v0, $at, .Ljp7F050FBC
/* 085B10 7F050FA0 00408025 */   move  $s0, $v0
/* 085B14 7F050FA4 0FC23639 */  jal   bondinvIsAliveWithFlag
/* 085B18 7F050FA8 00000000 */   nop
/* 085B1C 7F050FAC 10400003 */  beqz  $v0, .Ljp7F050FBC
/* 085B20 7F050FB0 24180001 */   li    $t8, 1
/* 085B24 7F050FB4 10000009 */  b     .Ljp7F050FDC
/* 085B28 7F050FB8 AFB80064 */   sw    $t8, 0x64($sp)
.Ljp7F050FBC:
/* 085B2C 7F050FBC 24010003 */  li    $at, 3
/* 085B30 7F050FC0 56010007 */  bnel  $s0, $at, .Ljp7F050FE0
/* 085B34 7F050FC4 8FA90064 */   lw    $t1, 0x64($sp)
/* 085B38 7F050FC8 0FC23649 */  jal   bondinvHasGoldenGun
/* 085B3C 7F050FCC 00000000 */   nop
/* 085B40 7F050FD0 10400002 */  beqz  $v0, .Ljp7F050FDC
/* 085B44 7F050FD4 24190001 */   li    $t9, 1
/* 085B48 7F050FD8 AFB90064 */  sw    $t9, 0x64($sp)
.Ljp7F050FDC:
/* 085B4C 7F050FDC 8FA90064 */  lw    $t1, 0x64($sp)
.Ljp7F050FE0:
/* 085B50 7F050FE0 11200003 */  beqz  $t1, .Ljp7F050FF0
/* 085B54 7F050FE4 00000000 */   nop
/* 085B58 7F050FE8 1000008B */  b     .Ljp7F051218
/* 085B5C 7F050FEC 00001025 */   move  $v0, $zero
.Ljp7F050FF0:
/* 085B60 7F050FF0 0FC22992 */  jal   get_curplay_vertical_rotation_in_degrees
/* 085B64 7F050FF4 00000000 */   nop
/* 085B68 7F050FF8 3C018005 */  lui   $at, %hi(D_800532E0) # $at, 0x8005
/* 085B6C 7F050FFC C4263310 */  lwc1  $f6, %lo(D_800532E0)($at)
/* 085B70 7F051000 3C0A8008 */  lui   $t2, %hi(g_CurrentPlayer) # $t2, 0x8008
/* 085B74 7F051004 4606003C */  c.lt.s $f0, $f6
/* 085B78 7F051008 00000000 */  nop
/* 085B7C 7F05100C 45000007 */  bc1f  .Ljp7F05102C
/* 085B80 7F051010 00000000 */   nop
/* 085B84 7F051014 8D4AA120 */  lw    $t2, %lo(g_CurrentPlayer)($t2)
/* 085B88 7F051018 8D4B1270 */  lw    $t3, 0x1268($t2)
/* 085B8C 7F05101C 05610003 */  bgez  $t3, .Ljp7F05102C
/* 085B90 7F051020 00000000 */   nop
/* 085B94 7F051024 1000007C */  b     .Ljp7F051218
/* 085B98 7F051028 00001025 */   move  $v0, $zero
.Ljp7F05102C:
/* 085B9C 7F05102C 0FC2279B */  jal   get_curplayer_positiondata
/* 085BA0 7F051030 00000000 */   nop
/* 085BA4 7F051034 3C0D8008 */  lui   $t5, %hi(g_CurrentPlayer) # $t5, 0x8008
/* 085BA8 7F051038 8FAC008C */  lw    $t4, 0x8c($sp)
/* 085BAC 7F05103C 8DADA120 */  lw    $t5, %lo(g_CurrentPlayer)($t5)
/* 085BB0 7F051040 C44A0008 */  lwc1  $f10, 8($v0)
/* 085BB4 7F051044 C452000C */  lwc1  $f18, 0xc($v0)
/* 085BB8 7F051048 8DAE1270 */  lw    $t6, 0x1268($t5)
/* 085BBC 7F05104C C4460010 */  lwc1  $f6, 0x10($v0)
/* 085BC0 7F051050 C5880058 */  lwc1  $f8, 0x58($t4)
/* 085BC4 7F051054 C590005C */  lwc1  $f16, 0x5c($t4)
/* 085BC8 7F051058 C5840060 */  lwc1  $f4, 0x60($t4)
/* 085BCC 7F05105C 29C1003C */  slti  $at, $t6, 0x3c
/* 085BD0 7F051060 00404025 */  move  $t0, $v0
/* 085BD4 7F051064 460A4001 */  sub.s $f0, $f8, $f10
/* 085BD8 7F051068 46128301 */  sub.s $f12, $f16, $f18
/* 085BDC 7F05106C 14200022 */  bnez  $at, .Ljp7F0510F8
/* 085BE0 7F051070 46062081 */   sub.s $f2, $f4, $f6
/* 085BE4 7F051074 46000202 */  mul.s $f8, $f0, $f0
/* 085BE8 7F051078 3C018005 */  lui   $at, %hi(D_800532E4) # $at, 0x8005
/* 085BEC 7F05107C C4323314 */  lwc1  $f18, %lo(D_800532E4)($at)
/* 085BF0 7F051080 46021282 */  mul.s $f10, $f2, $f2
/* 085BF4 7F051084 3C01C3FA */  li    $at, 0xC3FA0000 # -500.000000
/* 085BF8 7F051088 00001025 */  move  $v0, $zero
/* 085BFC 7F05108C 460A4400 */  add.s $f16, $f8, $f10
/* 085C00 7F051090 4612803E */  c.le.s $f16, $f18
/* 085C04 7F051094 00000000 */  nop
/* 085C08 7F051098 45000002 */  bc1f  .Ljp7F0510A4
/* 085C0C 7F05109C 00000000 */   nop
/* 085C10 7F0510A0 24020001 */  li    $v0, 1
.Ljp7F0510A4:
/* 085C14 7F0510A4 10400012 */  beqz  $v0, .Ljp7F0510F0
/* 085C18 7F0510A8 00401825 */   move  $v1, $v0
/* 085C1C 7F0510AC 44812000 */  mtc1  $at, $f4
/* 085C20 7F0510B0 3C0143FA */  li    $at, 0x43FA0000 # 500.000000
/* 085C24 7F0510B4 00001025 */  move  $v0, $zero
/* 085C28 7F0510B8 460C203E */  c.le.s $f4, $f12
/* 085C2C 7F0510BC 00000000 */  nop
/* 085C30 7F0510C0 45000002 */  bc1f  .Ljp7F0510CC
/* 085C34 7F0510C4 00000000 */   nop
/* 085C38 7F0510C8 24020001 */  li    $v0, 1
.Ljp7F0510CC:
/* 085C3C 7F0510CC 10400008 */  beqz  $v0, .Ljp7F0510F0
/* 085C40 7F0510D0 00401825 */   move  $v1, $v0
/* 085C44 7F0510D4 44813000 */  mtc1  $at, $f6
/* 085C48 7F0510D8 00001825 */  move  $v1, $zero
/* 085C4C 7F0510DC 4606603E */  c.le.s $f12, $f6
/* 085C50 7F0510E0 00000000 */  nop
/* 085C54 7F0510E4 45000002 */  bc1f  .Ljp7F0510F0
/* 085C58 7F0510E8 00000000 */   nop
/* 085C5C 7F0510EC 24030001 */  li    $v1, 1
.Ljp7F0510F0:
/* 085C60 7F0510F0 10000021 */  b     .Ljp7F051178
/* 085C64 7F0510F4 00608025 */   move  $s0, $v1
.Ljp7F0510F8:
/* 085C68 7F0510F8 46000202 */  mul.s $f8, $f0, $f0
/* 085C6C 7F0510FC 3C018005 */  lui   $at, %hi(D_800532E8) # $at, 0x8005
/* 085C70 7F051100 C4303318 */  lwc1  $f16, %lo(D_800532E8)($at)
/* 085C74 7F051104 46021282 */  mul.s $f10, $f2, $f2
/* 085C78 7F051108 3C01C348 */  li    $at, 0xC3480000 # -200.000000
/* 085C7C 7F05110C 00001025 */  move  $v0, $zero
/* 085C80 7F051110 460A4480 */  add.s $f18, $f8, $f10
/* 085C84 7F051114 4610903E */  c.le.s $f18, $f16
/* 085C88 7F051118 00000000 */  nop
/* 085C8C 7F05111C 45000002 */  bc1f  .Ljp7F051128
/* 085C90 7F051120 00000000 */   nop
/* 085C94 7F051124 24020001 */  li    $v0, 1
.Ljp7F051128:
/* 085C98 7F051128 10400012 */  beqz  $v0, .Ljp7F051174
/* 085C9C 7F05112C 00401825 */   move  $v1, $v0
/* 085CA0 7F051130 44812000 */  mtc1  $at, $f4
/* 085CA4 7F051134 3C014348 */  li    $at, 0x43480000 # 200.000000
/* 085CA8 7F051138 00001025 */  move  $v0, $zero
/* 085CAC 7F05113C 460C203E */  c.le.s $f4, $f12
/* 085CB0 7F051140 00000000 */  nop
/* 085CB4 7F051144 45000002 */  bc1f  .Ljp7F051150
/* 085CB8 7F051148 00000000 */   nop
/* 085CBC 7F05114C 24020001 */  li    $v0, 1
.Ljp7F051150:
/* 085CC0 7F051150 10400008 */  beqz  $v0, .Ljp7F051174
/* 085CC4 7F051154 00401825 */   move  $v1, $v0
/* 085CC8 7F051158 44813000 */  mtc1  $at, $f6
/* 085CCC 7F05115C 00001825 */  move  $v1, $zero
/* 085CD0 7F051160 4606603E */  c.le.s $f12, $f6
/* 085CD4 7F051164 00000000 */  nop
/* 085CD8 7F051168 45020003 */  bc1fl .Ljp7F051178
/* 085CDC 7F05116C 00608025 */   move  $s0, $v1
/* 085CE0 7F051170 24030001 */  li    $v1, 1
.Ljp7F051174:
/* 085CE4 7F051174 00608025 */  move  $s0, $v1
.Ljp7F051178:
/* 085CE8 7F051178 10600020 */  beqz  $v1, .Ljp7F0511FC
/* 085CEC 7F05117C 8FAF008C */   lw    $t7, 0x8c($sp)
/* 085CF0 7F051180 8DF8000C */  lw    $t8, 0xc($t7)
/* 085CF4 7F051184 8FA20090 */  lw    $v0, 0x90($sp)
/* 085CF8 7F051188 3C0141F0 */  li    $at, 0x41F00000 # 30.000000
/* 085CFC 7F05118C 33191000 */  andi  $t9, $t8, 0x1000
/* 085D00 7F051190 1720001A */  bnez  $t9, .Ljp7F0511FC
/* 085D04 7F051194 27A40044 */   addiu $a0, $sp, 0x44
/* 085D08 7F051198 8D090014 */  lw    $t1, 0x14($t0)
/* 085D0C 7F05119C 44810000 */  mtc1  $at, $f0
/* 085D10 7F0511A0 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 085D14 7F0511A4 AFA90044 */  sw    $t1, 0x44($sp)
/* 085D18 7F0511A8 C4480010 */  lwc1  $f8, 0x10($v0)
/* 085D1C 7F0511AC 8C470008 */  lw    $a3, 8($v0)
/* 085D20 7F0511B0 8D060010 */  lw    $a2, 0x10($t0)
/* 085D24 7F0511B4 8D050008 */  lw    $a1, 8($t0)
/* 085D28 7F0511B8 44818000 */  mtc1  $at, $f16
/* 085D2C 7F0511BC 44805000 */  mtc1  $zero, $f10
/* 085D30 7F0511C0 240A0002 */  li    $t2, 2
/* 085D34 7F0511C4 AFAA0014 */  sw    $t2, 0x14($sp)
/* 085D38 7F0511C8 E7A0001C */  swc1  $f0, 0x1c($sp)
/* 085D3C 7F0511CC E7A00018 */  swc1  $f0, 0x18($sp)
/* 085D40 7F0511D0 E7A80010 */  swc1  $f8, 0x10($sp)
/* 085D44 7F0511D4 E7B00024 */  swc1  $f16, 0x24($sp)
/* 085D48 7F0511D8 0FC2C675 */  jal   stanTestLineUnobstructed
/* 085D4C 7F0511DC E7AA0020 */   swc1  $f10, 0x20($sp)
/* 085D50 7F0511E0 10400005 */  beqz  $v0, .Ljp7F0511F8
/* 085D54 7F0511E4 8FAC0090 */   lw    $t4, 0x90($sp)
/* 085D58 7F0511E8 8FAB0044 */  lw    $t3, 0x44($sp)
/* 085D5C 7F0511EC 8D8D0014 */  lw    $t5, 0x14($t4)
/* 085D60 7F0511F0 116D0002 */  beq   $t3, $t5, .Ljp7F0511FC
/* 085D64 7F0511F4 00000000 */   nop
.Ljp7F0511F8:
/* 085D68 7F0511F8 00008025 */  move  $s0, $zero
.Ljp7F0511FC:
/* 085D6C 7F0511FC 12000006 */  beqz  $s0, .Ljp7F051218
/* 085D70 7F051200 00001025 */   move  $v0, $zero
/* 085D74 7F051204 8FA40090 */  lw    $a0, 0x90($sp)
/* 085D78 7F051208 0FC14195 */  jal   collect_or_interact_object
/* 085D7C 7F05120C 24050001 */   li    $a1, 1
/* 085D80 7F051210 10000002 */  b     .Ljp7F05121C
/* 085D84 7F051214 8FBF0034 */   lw    $ra, 0x34($sp)
.Ljp7F051218:
/* 085D88 7F051218 8FBF0034 */  lw    $ra, 0x34($sp)
.Ljp7F05121C:
/* 085D8C 7F05121C 8FB00030 */  lw    $s0, 0x30($sp)
/* 085D90 7F051220 27BD0090 */  addiu $sp, $sp, 0x90
/* 085D94 7F051224 03E00008 */  jr    $ra
/* 085D98 7F051228 00000000 */   nop
)
#endif

#endif


bool sub_GAME_7F050D30(PropRecord *prop, coord3d *arg1, struct coord2d *arg2, struct coord2d *arg3)
{
    if (prop->flags & PROPFLAG_ONSCREEN)
    {
        ObjectRecord *obj = prop->obj;
        Mtxf *matrix = getsubmatrix(obj->model);

        arg1->z = matrix->m[3][2];

        if (arg1->z < 0)
        {
            arg1->x = matrix->m[3][0];
            arg1->y = matrix->m[3][1];

            arg3->f[0] = 0;
            arg3->f[1] = 0;

            arg2->f[0] = 0;
            arg2->f[1] = 0;

            sub_GAME_7F03F90C(obj->model, &arg2->f[1], &arg2->f[0], &arg3->f[1], &arg3->f[0]);

            return TRUE;
        }
    }

    return FALSE;
}


void sub_GAME_7F050DE8(Model* model)
{
    ModelNode* node;
    ModelFileHeader* header;
    union ModelRoData* rodata;
    union ModelRwData* rwdata;

    header = model->obj;
    node = header->RootNode;

    while (node != NULL)
    {
        switch (node->Opcode & 0xFF)
        {
            case MODELNODE_OPCODE_DLCOLLISION:
                rodata = node->Data;
                rwdata = modelGetNodeRwData(model, node);

                if ((rwdata->DisplayListCollisions.Vertices != rodata->DisplayListCollisions.Vertices) && (sub_GAME_7F04B590(header, node) != 0))
                {
                    sub_GAME_7F09C044(rwdata->DisplayListCollisions.Vertices);
                    rwdata->DisplayListCollisions.Vertices = rodata->DisplayListCollisions.Vertices;
                }
                break;
            case MODELNODE_OPCODE_LOD:
                modelApplyDistanceRelations(model, node);
                break;
            case MODELNODE_OPCODE_SWITCH:
                modelApplyToggleRelations(model, node);
                break;
            case MODELNODE_OPCODE_HEAD:
                modelApplyHeadRelations(model, node);
                break;
        }

        if (node->Child)
        {
            node = node->Child;
        }
        else
        {
            while (node)
            {
                if (node->Next)
                {
                    node = node->Next;
                    break;
                }

                node = node->Parent;
            }
        }
    }
}


PropRecord *hatApplyToChr(HatRecord *hat, ChrRecord *chr, ModelFileHeader *filedata, PropRecord *prop, Model *model)
{
    prop = objInit((ObjectRecord*)hat, filedata, prop, model);

    if (prop && hat->model)
    {
        f32 scale = hat->extrascale * (1.0f / 256.0f);

        modelSetScale(hat->model, hat->model->scale * scale);
        hat->model->attachedto = chr->model;
        hat->model->attachedto_objinst = chr->model->obj->Switches[6];

        chrpropReparent(prop, chr->prop);
        chr->handle_positiondata_hat = prop;
    }

    return prop;
}


void hatLoadAndApplyToChr(HatRecord *hat, PropRecord *arg1)
{
    s32 unused;
    s32 obj_idx;
    obj_idx = (u32) hat->obj;
    modelLoad(obj_idx);
    hatApplyToChr(hat, arg1, PitemZ_entries[obj_idx].header, NULL, 0);
}


void hatAssignToChr(HatRecord* hat, ChrRecord* chr)
{
    hat->damage = (*(s32*)&hat->damage / M_U16_MAX_VALUE_F);
    hatLoadAndApplyToChr(hat, chr);
}


PropRecord *hatCreateForChr(ChrRecord *chr, s32 modelnum, u32 flags)
{
    ModelFileHeader *modeldef;
    PropRecord *prop;
    Model *model;
    HatRecord *hat;

    modeldef = PitemZ_entries[modelnum].header;

    modelLoad(modelnum);
    prop = chrpropAllocate();
    model = get_obj_instance_controller_for_header(modeldef);
    hat = hatCreate(prop == NULL, model == NULL, modeldef);

    if (prop == NULL)
    {
        prop = chrpropAllocate();
    }

    if (model == NULL)
    {
        model = get_obj_instance_controller_for_header(modeldef);
    }

    if (hat && prop && model)
    {
        HatRecord tmp = {
            0x0100, // extrascale
            0x0,    // state
            0x11,   // type
            0,      // obj
            0,      // pad
            0x00004000, // flags
            0,      // flags2
            NULL,   // prop
            NULL,   // model

            { 1.0f, 0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f, 0.0f,
              0.0f, 0.0f, 1.0f, 0.0f,
              0.0f, 0.0f, 0.0f, 1.0f
            }, // mtx

            { 0.0f, 0.0f, 0.0f }, // runtime_pos

            { 0x00000000 }, // runtime_bitflags
            NULL, // ptr_allocated_collisiondata_block
            NULL, // projectile/embedment
            0.0f, // maxdamage
            1000.0f, // damage
            { 0xFF, 0xFF, 0xFF, 0x00 }, // shadecol
            { 0xFF, 0xFF, 0xFF, 0x00 }, // nextcol
        };

        *hat = tmp;

        hat->obj = modelnum;
        hat->flags = flags | PROPFLAG_ASSIGNEDTOCHR;
        hat->pad = chr->chrnum;

        prop = hatApplyToChr(hat, chr, modeldef, prop, model);
    }
    else
    {
        if (model)
        {
            clear_model_obj(model);
        }

        if (prop)
        {
            chrpropFree(prop);
            prop = NULL;
        }
    }

    return prop;
}


// PD: weaponCreate
WeaponObjRecord* weaponCreate(bool musthaveprop, bool musthavemodel, ModelFileHeader *modeldef)
{
    s32 i;
    WeaponObjRecord *tmp;
    WeaponObjRecord *sp4c = NULL;
    WeaponObjRecord *sp48 = NULL;
    s32 sp44 = -1;
    s32 sp40 = -1;
    s32 sp3c = -1;

    for (i = g_NextWeaponSlot; TRUE; )
    {

        if (g_WeaponSlots[i].prop == NULL)
        {
            if (!musthaveprop && !musthavemodel)
            {
                sp44 = i;
                break;
            }
        }  else if ((g_WeaponSlots[i].runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT) == 0 && (g_WeaponSlots[i].state & 4) == 0)
        {
            WeaponObjRecord* slot = &g_WeaponSlots[i];
            if (((slot->timer <= 0) && (slot->prop->parent == NULL))
                    || (((slot->weaponnum == ITEM_REMOTEMINE) || (slot->weaponnum == ITEM_PROXIMITYMINE) || (slot->weaponnum == ITEM_TIMEDMINE))
                            && ((slot->prop->parent == NULL) || (slot->prop->parent->type == PROP_TYPE_OBJ) || (slot->prop->parent->type == PROP_TYPE_DOOR) || (slot->prop->parent->type == PROP_TYPE_WEAPON))))
            {
                if (!musthavemodel || modelmgrCanSlotFitRwdata(slot->model, modeldef))
                {
                    if ((slot->prop->flags & PROPFLAG_ONSCREEN) == 0 && sp40 < 0)
                    {
                        sp40 = i;
                    }

                    if (sp3c < 0)
                    {
                        sp3c = i;
                    }
                }
            }
        }

        i = (i + 1) % MAX_WEAPON_SLOTS;

        if (i == g_NextWeaponSlot)
        {
            break;
        }
    }

    if (sp44 >= 0)
    {
        g_NextWeaponSlot = (sp44 + 1) % MAX_WEAPON_SLOTS;
        return &g_WeaponSlots[sp44];
    }

    tmp = (WeaponObjRecord *)setupFindObjForReuse(PROPDEF_COLLECTABLE, (ObjectRecord **)&sp4c, (ObjectRecord **)&sp48, musthaveprop, musthavemodel, modeldef);

    if (tmp)
    {
        return tmp;
    }

    if (sp40 >= 0)
    {
        if (g_WeaponSlots[sp40].prop)
        {
            objFreePermanently((ObjectRecord *)&g_WeaponSlots[sp40], TRUE);
        }

        g_NextWeaponSlot = (sp40 + 1) % MAX_WEAPON_SLOTS;
        return (g_WeaponSlots + sp40);
    }

    if (sp4c)
    {
        if (sp4c->prop)
        {
            objFreePermanently((ObjectRecord *)sp4c, TRUE);
        }

        return sp4c;
    }

    if (sp3c >= 0)
    {
        if (g_WeaponSlots[sp3c].prop)
        {
            objFreePermanently((ObjectRecord *)&g_WeaponSlots[sp3c], TRUE);
        }

        g_NextWeaponSlot = (sp3c + 1) % MAX_WEAPON_SLOTS;
        return (g_WeaponSlots + sp3c);
    }

    if (sp48)
    {
        if (sp48->prop)
        {
            objFreePermanently((ObjectRecord *)sp48, TRUE);
        }

        return sp48;
    }

    return NULL;
}


void sub_GAME_7F051588(void)
{
    weaponCreate(FALSE, FALSE, NULL);
}


HatRecord *hatCreate(bool musthaveprop, bool musthavemodel, ModelFileHeader *modeldef)
{
	s32 i;
	HatRecord *tmp;
	HatRecord *sp5c = NULL;
	HatRecord *sp58 = NULL;
	s32 sp54 = -1;
	s32 var_s1 = -1;
	s32 var_s3 = -1;

	for (i = g_NextHatSlot; TRUE; )
	{
		if (g_HatSlots[i].prop == NULL)
		{
			if (!musthaveprop && !musthavemodel)
			{
				sp54 = i;
				break;
			}
		}
		else if ((g_HatSlots[i].runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT) == 0
				&& g_HatSlots[i].prop->parent == NULL
				&& (!musthavemodel || modelmgrCanSlotFitRwdata(g_HatSlots[i].model, modeldef)))
		{
			if ((g_HatSlots[i].prop->flags & PROPFLAG_ONSCREEN) == 0 && var_s1 < 0)
			{
				var_s1 = i;
			}

			if (var_s3 < 0)
			{
				var_s3 = i;
			}
		}

		i = (i + 1) % MAX_HAT_SLOTS;

		if (i == g_NextHatSlot)
		{
			break;
		}
	}

	if (sp54 >= 0)
	{
		g_NextHatSlot = (sp54 + 1) % MAX_HAT_SLOTS;
		return (g_HatSlots + sp54);
	}

	tmp = (HatRecord *)setupFindObjForReuse(PROPDEF_HAT, (ObjectRecord **)&sp5c, (ObjectRecord **)&sp58, musthaveprop, musthavemodel, modeldef);

	if (tmp)
	{
		return tmp;
	}

	if (var_s1 >= 0)
	{
		if (g_HatSlots[var_s1].prop)
		{
			objFreePermanently((ObjectRecord*)&g_HatSlots[var_s1], TRUE);
		}

		g_NextHatSlot = (var_s1 + 1) % MAX_HAT_SLOTS;
		return (g_HatSlots + var_s1);
	}

	if (sp5c)
	{
		if (sp5c->prop)
		{
			objFreePermanently((ObjectRecord*)sp5c, TRUE);
		}

		return sp5c;
	}

	if (var_s3 >= 0)
	{
		if (g_HatSlots[var_s3].prop)
		{
			objFreePermanently((ObjectRecord*)&g_HatSlots[var_s3], TRUE);
		}

		g_NextHatSlot = (var_s3 + 1) % MAX_HAT_SLOTS;
		return (g_HatSlots + var_s3);
	}

	if (sp58)
	{
		if (sp58->prop)
		{
			objFreePermanently((ObjectRecord*)sp58, TRUE);
		}

		return sp58;
	}

	return NULL;
}


HatRecord* sub_GAME_7F0518A8(void)
{
    return hatCreate(0, 0, NULL);
}


AmmoCrateRecord *ammocrateAllocate(void)
{
    s32 i;

    // Try to find a free one
    for (i = 0; i < MAX_AMMO_CRATES; i++)
    {
        if (g_AmmoCrates[i].prop == NULL)
        {
            return (g_AmmoCrates + i);
        }
    }

    // Find one that can be freed off-screen
    for (i = 0; i < MAX_AMMO_CRATES; i++)
    {
        if ((g_AmmoCrates[i].runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT) == 0
                && (g_AmmoCrates[i].state & 0x04) == 0
                && g_AmmoCrates[i].prop->parent == NULL
                && (g_AmmoCrates[i].prop->flags & 0x02) == 0)
        {
            objFreePermanently(&g_AmmoCrates[i], TRUE);
            return (g_AmmoCrates + i);
        }
    }

    // Find one that can be freed on-screen
    for (i = 0; i < MAX_AMMO_CRATES; i++)
    {
        if ((g_AmmoCrates[i].runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT) == 0
                && (g_AmmoCrates[i].state & 0x04) == 0
                && g_AmmoCrates[i].prop->parent == NULL)
        {
            objFreePermanently(&g_AmmoCrates[i], TRUE);
            return (g_AmmoCrates + i);
        }
    }

    return NULL;
}


void trigger_remote_mine_detonation(void)
{
    u32 uVar1 = 1 << (get_cur_playernum());
    g_RemoteMineOwnerTriggerFlag = uVar1 | g_RemoteMineOwnerTriggerFlag;
    sndPlaySfx(g_musicSfxBufferPtr, WATCH_DETONATE_MINE_SFX, NULL);
}


/**
 * Get Key with ID from Prop (or child of prop)
 * @param ID: ID of key
 * @param prop: Prop to search
 * @return: Key if found
 * @RenameTo: objGetKeyIfExist
*/
KeyRecord *check_if_entry_is_collectable(s32 ID, PropRecord *prop) //#MATCH
{
    KeyRecord * key;
    PropRecord *p;

    if (prop->type == PROPDEF_KEY)
    {
        key = prop->obj;
        if (ID == key->keyID)
        {
            return key;
        }
    }

    for (p = prop->child; p; p = p->prev)
    {
        key = check_if_entry_is_collectable(ID, p);
        if (key)
        {
            return key;
        }
    }
    return NULL;
}




/**
 * Get Key if has been "dropped"
 * @param KeyID: ID of Key to Find
 * @return: Key if found and "Dropped"
 * @RenameTo: objGetKeyIfDropped
*/
KeyRecord *weaponFindThrown(s32 KeyID) //MATCH
{
    KeyRecord  *obj;
    PropRecord *prop;

    for (prop = get_ptr_obj_pos_list_current_entry(); prop; prop = prop->prev)
    {
        obj = check_if_entry_is_collectable(KeyID, prop);
        if (obj && (!(obj->runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT)))
        {
            return obj;
        }
    }

    return NULL;
}

void add_obj_to_temp_proxmine_table(WeaponObjRecord* proxy)
{
    s32 i = 0;

    while (1) {
        if (temp_mine_table[i] == NULL)
        {
            temp_mine_table[i] = proxy;
            #ifdef DEBUG
                assert(i<PROXIMITYARRMAX);
            #endif

            return;
        }
        i++;
        if (i == 30)
        {
            return;
        }
    }
}


void remove_obj_from_temp_proxmine_table(WeaponObjRecord* proxy)
{
    s32 i = 0;

    while (1)
    {
        if (temp_mine_table[i] == proxy)
        {
            temp_mine_table[i] = NULL;
            return;
        }
        i++;
        if (i == 30)
        {
            return;
        }
    }
}


void detonate_proxmine_In_range(coord3d* pos)
{
    s32 i;
    for (i = 0; i < 30; i++)
    {
        WeaponObjRecord* obj = temp_mine_table[i];

        if (obj && (obj->timer == 1))
        {
            f32 diff_x;
            f32 diff_z;
            f32 diff_y;
            f32 dist_sqr;
            diff_x = pos->x - obj->runtime_pos.x;
            diff_y = pos->y - obj->runtime_pos.y;
            diff_z = pos->z - obj->runtime_pos.z;
            dist_sqr = (diff_x * diff_x) + (diff_y * diff_y) + (diff_z * diff_z);

            if (dist_sqr < 62500.0f)
            {
                obj->timer = 0;
            }
        }
    }
}


void check_guard_detonate_proxmine(void)
{
    ChrRecord* guard;
    s32 numslots;
    s32 i;

    numslots = get_numguards();

    for (i = 0; i < numslots; i++)
    {
        guard = &g_ChrSlots[i];
        if ((guard->model != NULL) && (guard->hidden & CHRHIDDEN_BACKGROUND_AI))
        {
            coord3d pos;
            chrlvGetPatrolPercentOrPosition(guard, &pos);
            detonate_proxmine_In_range(&pos);
        }
    }
}


void propweaponSetDual(WeaponObjRecord *leftweapon, WeaponObjRecord *rightweapon) //#MATCH
{
    leftweapon->LinkedWeaponType  = rightweapon->weaponnum;
    leftweapon->dualweapon        = rightweapon;
    rightweapon->LinkedWeaponType = leftweapon->weaponnum;
    rightweapon->dualweapon       = leftweapon;
}


PropRecord* complete_object_data_block_return_position_entry(WeaponObjRecord* obj, ModelFileHeader* model_header, PropRecord* prop, Model* model)
{
    prop = objInit((ObjectRecord*)obj, model_header, prop, model);
    if (prop != NULL)
    {
        prop->type = 4;
        weaponSetGunfireVisible(prop, 0);
    }

    return prop;
}


PropRecord* sub_GAME_7F051DD8(struct ObjectRecord* arg0, ModelFileHeader* arg1)
{
    PropRecord* prop;

    prop = objInitWithModelDef(arg0, arg1);
    if (prop != NULL)
    {
        prop->type = PROP_TYPE_WEAPON;
        weaponSetGunfireVisible(prop, 0);
    }
    return prop;
}


bool chrEquipWeapon(WeaponObjRecord *wep, ChrRecord *chr)
{
    WeaponObjRecord *wep2;
    GUNHAND hand = wep->flags & PROPFLAG_WEAPON_LEFTHANDED;

    if (wep->flags & PROPFLAG_WEAPON_LEFTHANDED)
    {
        hand = GUNLEFT;
    }
    else
    {
        hand = GUNRIGHT;
    }

    wep2 = wep;
    if (wep2->prop && wep2->model)
    {
        if (!(wep2->flags & PROPFLAG_CONCEAL_GUN))
        {
            if (!chr->weapons_held[hand])
            {
                wep2->model->attachedto = chr->model;

                if (hand == GUNRIGHT)
                {
                    wep2->model->attachedto_objinst = chr->model->obj->Switches[3];
                }
                else
                {
                    wep2->model->attachedto_objinst = chr->model->obj->Switches[5];
                }

                chr->weapons_held[hand] = wep2->prop;

                if (wep2->flags & PROPFLAG_IS_DOUBLE && chr->weapons_held[1 - hand])
                {
                    propweaponSetDual(wep2, chr->weapons_held[1 - hand]->obj);
                }
            }
            else
            {
                 #ifdef DEBUG
                    osSyncPrintf("attempted multiple attach!!!\n");
                #endif
                return FALSE;
            }
        }
        chrpropReparent(wep2->prop, chr->prop);
    }
    return TRUE;
}


PropRecord *sub_GAME_7F051F30(WeaponObjRecord *weapon, ChrRecord *chr, ModelFileHeader *modeldef, PropRecord *prop, Model *model)
{
	prop = complete_object_data_block_return_position_entry(weapon, modeldef, prop, model);

	if (prop && weapon->model)
	{
		f32 scale = weapon->extrascale * (1.0f / 256.0f);

		modelSetScale(weapon->model, weapon->model->scale * scale);
        chrEquipWeapon(weapon, chr);
        if (weapon->model);
	}

	return prop;
}


void sub_GAME_7F051FD4(WeaponObjRecord *weapon, ChrRecord *chr)
{
	u32 stack;
	s32 modelnum = weapon->obj;

	modelLoad(modelnum);
	sub_GAME_7F051F30(weapon, chr, PitemZ_entries[modelnum].header, 0, 0);
}


void sub_GAME_7F052030(WeaponObjRecord* arg0, ChrRecord* arg1)
{
    arg0->damage = (*(s32*)&arg0->damage) / M_U16_MAX_VALUE_F;
    sub_GAME_7F051FD4(arg0, arg1);
}


#ifdef NONMATCHING
/**
 * @param arg0: index into PitemZ_entries, which is enum PROP
 * @param arg1: object_weapon.gun_pickup value
 *
 * Address 0x7F05206C.
*/
WeaponObjRecord blank_08_object_preset_1 = {
    0x0100, //extrascale
    0x0, //state
    0x08, //type
    0, //obj
    1, //pad
    0x00000000, //flags
    0, //flags2
    NULL, // prop
    NULL, // model
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    }, //mtx
    { 0.0, 0.0, 0.0 }, //runtime_pos
    {0x00000000 }, //runtime_bitflags
    NULL, //ptr_allocated_collisiondata_block
    NULL, //projectile/embedment
    0.0f, //maxdamage
    1000.0f,//damage
    { 0xFF, 0xFF, 0xFF, 0x00 }, // shadecol
    { 0xFF, 0xFF, 0xFF, 0x00 }, // nextcol
    ITEM_UNARMED, //weaponnum
    -1, //LinkedWeaponType
    -1, //timer
    NULL //dualweapon
};
WeaponObjRecord *create_new_item_instance_of_model(s32 modelnum, ITEM_IDS weaponid)
{
    WeaponObjRecord *itemModel;
    s32              ObjInst;
    s32              sp20;
    s32              isObjInstAvailable;
    s32              lastObj;
    s32              ObjInst;
    s32             *temp_t1;
    s32             *temp_t4;
    s32             *temp_t8;
    WeaponObjRecord *NewGun;
    void            *temp_t5;
    s32              lastObj;
    s32              ObjInst;
    s32             *NewWep;
    s32             *phi_t1;
    s32             *phi_t4;
    void            *phi_t5;
    WeaponObjRecord *NewGun;

    itemModel = PitemZ_entries[modelnum];
    modelLoad();
    lastObj = chrpropAllocate();
    ObjInst = get_obj_instance_controller_for_header(itemModel);
    isObjInstAvailable = ObjInst == 0;
    ObjInst    = ObjInst;
    NewGun = weaponCreate(lastObj == 0, isObjInstAvailable, itemModel);
    lastObj  = lastObj;
    NewGun  = NewGun;
    if (lastObj == 0)
    {
        ObjInst   = ObjInst;
        lastObj = chrpropAllocate();
    }
    ObjInst = ObjInst;
    if (ObjInst == 0)
    {
        ObjInst = get_obj_instance_controller_for_header(itemModel);
    }
    if ((NewGun != 0) && (lastObj != 0) && (ObjInst != 0))
    {
        //struct copy
        // NewWep = New_WeaponObjRecord();
        // t5 = t4;
        NewWep = &blank_08_object_preset_1;
        phi_t1 = &sp20;
        do
        {
            temp_t8          = NewWep + 0xC;
            temp_t1          = phi_t1 + 0xC;
            temp_t1->unk - C = *NewWep;
            temp_t1->unk - 8 = temp_t8->unk - 8;
            temp_t1->unk - 4 = temp_t8->unk - 4;
            NewWep           = temp_t8;
            phi_t1           = temp_t1;
        } while (temp_t8 != (&blank_08_object_preset_1 + 0x84));
        temp_t1->unk0 = temp_t8->unk0;
        phi_t4        = &sp20;
        phi_t5        = NewGun;

        do
        {
            temp_t4          = phi_t4 + 0xC;
            temp_t5          = phi_t5 + 0xC;
            temp_t5->unk - C = *phi_t4;
            temp_t5->unk - 8 = temp_t4->unk - 8;
            temp_t5->unk - 4 = temp_t4->unk - 4;
            phi_t4           = temp_t4;
            phi_t5           = temp_t5;
        } while (temp_t4 != (&sp20 + 0x84));
        temp_t5->unk0  = temp_t4->unk0;
        NewGun->unk80 = weaponid;
        NewGun->unk4  = modelnum;
        complete_object_data_block_return_position_entry(NewGun, itemModel, lastObj, ObjInst);
    }
    else
    {
        NewGun = NULL;
        if (ObjInst != 0)
        {
            clear_model_obj(ObjInst);
        }
        if (lastObj != 0)
        {
            chrpropFree(lastObj);
        }
    }
    return NewGun;
    // itemModel = PitemZ_entries[arg0].header;

    // (result)->unk80 = (s8) arg1; // should be object_weapon.gun_pickup

}
#else
WeaponObjRecord blank_08_object_preset_1 = {
    0x0100, //extrascale
    0x0, //state
    0x08, //type
    0, //obj
    1, //pad
    0x00000000, //flags
    0, //flags2
    NULL, // prop
    NULL, // model
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    }, //mtx
    { 0.0, 0.0, 0.0 }, //runtime_pos
    {0x00000000 }, //runtime_bitflags
    NULL, //ptr_allocated_collisiondata_block
    NULL, //projectile/embedment
    0.0f, //maxdamage
    1000.0f,//damage
    { 0xFF, 0xFF, 0xFF, 0x00 }, // shadecol
    { 0xFF, 0xFF, 0xFF, 0x00 }, // nextcol
    ITEM_UNARMED, //weaponnum
    -1, //LinkedWeaponType
    -1, //timer
    NULL //dualweapon
};
GLOBAL_ASM(
.text
glabel create_new_item_instance_of_model
/* 086B9C 7F05206C 00047080 */  sll   $t6, $a0, 2
/* 086BA0 7F052070 27BDFF48 */  addiu $sp, $sp, -0xb8
/* 086BA4 7F052074 01C47023 */  subu  $t6, $t6, $a0
/* 086BA8 7F052078 000E7080 */  sll   $t6, $t6, 2
/* 086BAC 7F05207C 3C0F8004 */  lui   $t7, %hi(PitemZ_entries)
/* 086BB0 7F052080 01EE7821 */  addu  $t7, $t7, $t6
/* 086BB4 7F052084 8DEFA228 */  lw    $t7, %lo(PitemZ_entries)($t7)
/* 086BB8 7F052088 AFBF001C */  sw    $ra, 0x1c($sp)
/* 086BBC 7F05208C AFB10018 */  sw    $s1, 0x18($sp)
/* 086BC0 7F052090 AFB00014 */  sw    $s0, 0x14($sp)
/* 086BC4 7F052094 AFA500BC */  sw    $a1, 0xbc($sp)
/* 086BC8 7F052098 AFA400B8 */  sw    $a0, 0xb8($sp)
/* 086BCC 7F05209C 0FC15B0E */  jal   modelLoad
/* 086BD0 7F0520A0 AFAF00B4 */   sw    $t7, 0xb4($sp)
/* 086BD4 7F0520A4 0FC0E90C */  jal   chrpropAllocate
/* 086BD8 7F0520A8 00000000 */   nop
/* 086BDC 7F0520AC 00408025 */  move  $s0, $v0
/* 086BE0 7F0520B0 0FC1B025 */  jal   get_obj_instance_controller_for_header
/* 086BE4 7F0520B4 8FA400B4 */   lw    $a0, 0xb4($sp)
/* 086BE8 7F0520B8 2E040001 */  sltiu $a0, $s0, 1
/* 086BEC 7F0520BC 2C450001 */  sltiu $a1, $v0, 1
/* 086BF0 7F0520C0 8FA600B4 */  lw    $a2, 0xb4($sp)
/* 086BF4 7F0520C4 0FC1449B */  jal   weaponCreate
/* 086BF8 7F0520C8 AFA200AC */   sw    $v0, 0xac($sp)
/* 086BFC 7F0520CC 8FA700AC */  lw    $a3, 0xac($sp)
/* 086C00 7F0520D0 16000005 */  bnez  $s0, .L7F0520E8
/* 086C04 7F0520D4 00408825 */   move  $s1, $v0
/* 086C08 7F0520D8 0FC0E90C */  jal   chrpropAllocate
/* 086C0C 7F0520DC AFA700AC */   sw    $a3, 0xac($sp)
/* 086C10 7F0520E0 8FA700AC */  lw    $a3, 0xac($sp)
/* 086C14 7F0520E4 00408025 */  move  $s0, $v0
.L7F0520E8:
/* 086C18 7F0520E8 14E00004 */  bnez  $a3, .L7F0520FC
/* 086C1C 7F0520EC 00000000 */   nop
/* 086C20 7F0520F0 0FC1B025 */  jal   get_obj_instance_controller_for_header
/* 086C24 7F0520F4 8FA400B4 */   lw    $a0, 0xb4($sp)
/* 086C28 7F0520F8 00403825 */  move  $a3, $v0
.L7F0520FC:
/* 086C2C 7F0520FC 1220002C */  beqz  $s1, .L7F0521B0
/* 086C30 7F052100 00000000 */   nop
/* 086C34 7F052104 1200002A */  beqz  $s0, .L7F0521B0
/* 086C38 7F052108 00000000 */   nop
/* 086C3C 7F05210C 10E00028 */  beqz  $a3, .L7F0521B0
/* 086C40 7F052110 27A20020 */   addiu $v0, $sp, 0x20
/* 086C44 7F052114 3C188003 */  lui   $t8, %hi(blank_08_object_preset_1)
/* 086C48 7F052118 27182194 */  addiu $t8, %lo(blank_08_object_preset_1) # addiu $t8, $t8, 0x2194
/* 086C4C 7F05211C 27080084 */  addiu $t0, $t8, 0x84
/* 086C50 7F052120 00404825 */  move  $t1, $v0
.L7F052124:
/* 086C54 7F052124 8F010000 */  lw    $at, ($t8)
/* 086C58 7F052128 2718000C */  addiu $t8, $t8, 0xc
/* 086C5C 7F05212C 2529000C */  addiu $t1, $t1, 0xc
/* 086C60 7F052130 AD21FFF4 */  sw    $at, -0xc($t1)
/* 086C64 7F052134 8F01FFF8 */  lw    $at, -8($t8)
/* 086C68 7F052138 AD21FFF8 */  sw    $at, -8($t1)
/* 086C6C 7F05213C 8F01FFFC */  lw    $at, -4($t8)
/* 086C70 7F052140 1708FFF8 */  bne   $t8, $t0, .L7F052124
/* 086C74 7F052144 AD21FFFC */   sw    $at, -4($t1)
/* 086C78 7F052148 8F010000 */  lw    $at, ($t8)
/* 086C7C 7F05214C 00406025 */  move  $t4, $v0
/* 086C80 7F052150 02206825 */  move  $t5, $s1
/* 086C84 7F052154 244B0084 */  addiu $t3, $v0, 0x84
/* 086C88 7F052158 AD210000 */  sw    $at, ($t1)
.L7F05215C:
/* 086C8C 7F05215C 8D810000 */  lw    $at, ($t4)
/* 086C90 7F052160 258C000C */  addiu $t4, $t4, 0xc
/* 086C94 7F052164 25AD000C */  addiu $t5, $t5, 0xc
/* 086C98 7F052168 ADA1FFF4 */  sw    $at, -0xc($t5)
/* 086C9C 7F05216C 8D81FFF8 */  lw    $at, -8($t4)
/* 086CA0 7F052170 ADA1FFF8 */  sw    $at, -8($t5)
/* 086CA4 7F052174 8D81FFFC */  lw    $at, -4($t4)
/* 086CA8 7F052178 158BFFF8 */  bne   $t4, $t3, .L7F05215C
/* 086CAC 7F05217C ADA1FFFC */   sw    $at, -4($t5)
/* 086CB0 7F052180 8D810000 */  lw    $at, ($t4)
/* 086CB4 7F052184 02202025 */  move  $a0, $s1
/* 086CB8 7F052188 02003025 */  move  $a2, $s0
/* 086CBC 7F05218C ADA10000 */  sw    $at, ($t5)
/* 086CC0 7F052190 8FAE00BC */  lw    $t6, 0xbc($sp)
/* 086CC4 7F052194 A22E0080 */  sb    $t6, 0x80($s1)
/* 086CC8 7F052198 8FAF00B8 */  lw    $t7, 0xb8($sp)
/* 086CCC 7F05219C A62F0004 */  sh    $t7, 4($s1)
/* 086CD0 7F0521A0 0FC14764 */  jal   complete_object_data_block_return_position_entry
/* 086CD4 7F0521A4 8FA500B4 */   lw    $a1, 0xb4($sp)
/* 086CD8 7F0521A8 1000000A */  b     .L7F0521D4
/* 086CDC 7F0521AC 8FBF001C */   lw    $ra, 0x1c($sp)
.L7F0521B0:
/* 086CE0 7F0521B0 10E00003 */  beqz  $a3, .L7F0521C0
/* 086CE4 7F0521B4 00008825 */   move  $s1, $zero
/* 086CE8 7F0521B8 0FC1B08D */  jal   clear_model_obj
/* 086CEC 7F0521BC 00E02025 */   move  $a0, $a3
.L7F0521C0:
/* 086CF0 7F0521C0 52000004 */  beql  $s0, $zero, .L7F0521D4
/* 086CF4 7F0521C4 8FBF001C */   lw    $ra, 0x1c($sp)
/* 086CF8 7F0521C8 0FC0E921 */  jal   chrpropFree
/* 086CFC 7F0521CC 02002025 */   move  $a0, $s0
/* 086D00 7F0521D0 8FBF001C */  lw    $ra, 0x1c($sp)
.L7F0521D4:
/* 086D04 7F0521D4 02201025 */  move  $v0, $s1
/* 086D08 7F0521D8 8FB10018 */  lw    $s1, 0x18($sp)
/* 086D0C 7F0521DC 8FB00014 */  lw    $s0, 0x14($sp)
/* 086D10 7F0521E0 03E00008 */  jr    $ra
/* 086D14 7F0521E4 27BD00B8 */   addiu $sp, $sp, 0xb8
)
#endif



/**
 * Set removed flag on hand
 */
void chrSetWeaponFlag4(ChrRecord *chr, GUNHAND hand) //#MATCH
{
    if (chr->weapons_held[hand])
    {
        chr->weapons_held[hand]->weapon->runtime_bitflags |= RUNTIMEBITFLAG_REMOVE;
    }
}

WeaponObjRecord blank_08_object_preset_4001 = {
    0x0100, //extrascale
    0x0, //state
    0x08, //type
    0, //obj
    0x4001, //pad
    0x00000000, //flags
    0, //flags2
    NULL, //prop
    NULL, //model
    {
       1.0f, 0.0f, 0.0f, 0.0f,
       0.0f, 1.0f, 0.0f, 0.0f,
       0.0f, 0.0f, 1.0f, 0.0f,
       0.0f, 0.0f, 0.0f, 1.0f
    }, //mtx
    {0.0, 0.0, 0.0},//runtime_pos
    {0x00000000}, //runtime_bitflags
    NULL, //ptr_allocated_collisiondata_block
    NULL, //projectile/embedment
    0.0f, //maxdamage
    1000.0f, //damage
    { 0xFF, 0xFF, 0xFF, 0x00 }, // shadecol
    { 0xFF, 0xFF, 0xFF, 0x00 }, // nextcol
    ITEM_UNARMED, //weaponnu
    -1, //LinkedWeaponType
    -1, //timer
    NULL //dualweapon
};

/**
 * NTSC address 0x7F052214.
*/
PropRecord *something_with_generating_object(ChrRecord *self, s32 propid, ITEM_IDS itemid, s32 flags, WeaponObjRecord *weapon, ItemModelFileRecord *prop_header)
{
    Model *objinst;
    PropRecord *lastobjentry;

    if (!prop_header)
    {
        prop_header = PitemZ_entries[propid].header;
        modelLoad(propid);
    }

    lastobjentry = chrpropAllocate();
    objinst = get_obj_instance_controller_for_header((ModelFileHeader *)prop_header);

    if (!weapon)
    {
        weapon = weaponCreate(lastobjentry == NULL, objinst == NULL, (ModelFileHeader *)prop_header);
    }

    if (!lastobjentry)
    {
        lastobjentry = chrpropAllocate();
    }

    if (!objinst)
    {
        objinst = get_obj_instance_controller_for_header((ModelFileHeader *)prop_header);
    }

    if (weapon && lastobjentry && objinst)
    {
        WeaponObjRecord new_weapon = blank_08_object_preset_4001;
        *weapon = new_weapon;

        weapon->weaponnum = itemid;
        weapon->obj = propid;
        weapon->flags = flags | 0x4000;

        // pad = chrnum ???
        weapon->pad = self->chrnum;

        lastobjentry = sub_GAME_7F051F30(weapon, self, (ModelFileHeader *)prop_header, lastobjentry, objinst);
    }
    else
    {
        if (objinst)
        {
            clear_model_obj(objinst);
        }

        if (lastobjentry)
        {
            chrpropFree(lastobjentry);
            lastobjentry = NULL;
        }
    }

    return lastobjentry; //should be new weapon
}






/**
 * Add New Weapon to chr
 */
PropRecord *chrGiveWeapon(ChrRecord *self, s32 PropID, ITEM_IDS ItemID, s32 flags) //#MATCH
{
    return something_with_generating_object(self, PropID, ItemID, flags, NULL, NULL);
}





#ifdef NONMATCHING
void sub_GAME_7F0523F8(void) {

}
#else
u32 D_800322A4 = 0;
u32 D_800322A8[] = {1, 3};
u32 D_800322B0[] = {0, 0, 0, 0};
u32 D_800322C0[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
GLOBAL_ASM(
.late_rodata
glabel D_800532F0
.word 0x40490fdb /*3.1415927*/
.text
glabel sub_GAME_7F0523F8
/* 086F28 7F0523F8 27BDFF48 */  addiu $sp, $sp, -0xb8
/* 086F2C 7F0523FC AFBF001C */  sw    $ra, 0x1c($sp)
/* 086F30 7F052400 AFB10018 */  sw    $s1, 0x18($sp)
/* 086F34 7F052404 AFB00014 */  sw    $s0, 0x14($sp)
/* 086F38 7F052408 AFA500BC */  sw    $a1, 0xbc($sp)
/* 086F3C 7F05240C AFA600C0 */  sw    $a2, 0xc0($sp)
/* 086F40 7F052410 8C870004 */  lw    $a3, 4($a0)
/* 086F44 7F052414 00E02025 */  move  $a0, $a3
/* 086F48 7F052418 0FC08C0B */  jal   chrGetEquippedWeaponProp
/* 086F4C 7F05241C AFA700B4 */   sw    $a3, 0xb4($sp)
/* 086F50 7F052420 8FA700B4 */  lw    $a3, 0xb4($sp)
/* 086F54 7F052424 10400046 */  beqz  $v0, .L7F052540
/* 086F58 7F052428 00401825 */   move  $v1, $v0
/* 086F5C 7F05242C 8C500004 */  lw    $s0, 4($v0)
/* 086F60 7F052430 8E0E0064 */  lw    $t6, 0x64($s0)
/* 086F64 7F052434 31CF0800 */  andi  $t7, $t6, 0x800
/* 086F68 7F052438 55E0003F */  bnezl $t7, .L7F052538
/* 086F6C 7F05243C 90480001 */   lbu   $t0, 1($v0)
/* 086F70 7F052440 8E18000C */  lw    $t8, 0xc($s0)
/* 086F74 7F052444 3C098003 */  lui   $t1, %hi(D_800322A4)
/* 086F78 7F052448 252922A4 */  addiu $t1, %lo(D_800322A4) # addiu $t1, $t1, 0x22a4
/* 086F7C 7F05244C 0018CB00 */  sll   $t9, $t8, 0xc
/* 086F80 7F052450 07200038 */  bltz  $t9, .L7F052534
/* 086F84 7F052454 27A80068 */   addiu $t0, $sp, 0x68
/* 086F88 7F052458 8E110014 */  lw    $s1, 0x14($s0)
/* 086F8C 7F05245C 252B003C */  addiu $t3, $t1, 0x3c
.L7F052460:
/* 086F90 7F052460 8D210000 */  lw    $at, ($t1)
/* 086F94 7F052464 2529000C */  addiu $t1, $t1, 0xc
/* 086F98 7F052468 2508000C */  addiu $t0, $t0, 0xc
/* 086F9C 7F05246C AD01FFF4 */  sw    $at, -0xc($t0)
/* 086FA0 7F052470 8D21FFF8 */  lw    $at, -8($t1)
/* 086FA4 7F052474 AD01FFF8 */  sw    $at, -8($t0)
/* 086FA8 7F052478 8D21FFFC */  lw    $at, -4($t1)
/* 086FAC 7F05247C 152BFFF8 */  bne   $t1, $t3, .L7F052460
/* 086FB0 7F052480 AD01FFFC */   sw    $at, -4($t0)
/* 086FB4 7F052484 8D210000 */  lw    $at, ($t1)
/* 086FB8 7F052488 00003025 */  move  $a2, $zero
/* 086FBC 7F05248C AD010000 */  sw    $at, ($t0)
/* 086FC0 7F052490 906C0001 */  lbu   $t4, 1($v1)
/* 086FC4 7F052494 8CE4001C */  lw    $a0, 0x1c($a3)
/* 086FC8 7F052498 358D0002 */  ori   $t5, $t4, 2
/* 086FCC 7F05249C A06D0001 */  sb    $t5, 1($v1)
/* 086FD0 7F0524A0 0FC1B198 */  jal   modelFindNodeMtx
/* 086FD4 7F0524A4 8E25001C */   lw    $a1, 0x1c($s1)
/* 086FD8 7F0524A8 8FAE00BC */  lw    $t6, 0xbc($sp)
/* 086FDC 7F0524AC 24010001 */  li    $at, 1
/* 086FE0 7F0524B0 AFA20068 */  sw    $v0, 0x68($sp)
/* 086FE4 7F0524B4 15C10009 */  bne   $t6, $at, .L7F0524DC
/* 086FE8 7F0524B8 27A50020 */   addiu $a1, $sp, 0x20
/* 086FEC 7F0524BC 3C018005 */  lui   $at, %hi(D_800532F0)
/* 086FF0 7F0524C0 0FC161A2 */  jal   matrix_4x4_set_rotation_around_z
/* 086FF4 7F0524C4 C42C32F0 */   lwc1  $f12, %lo(D_800532F0)($at)
/* 086FF8 7F0524C8 8FA40068 */  lw    $a0, 0x68($sp)
/* 086FFC 7F0524CC 0FC1601A */  jal   matrix_4x4_multiply_in_place
/* 087000 7F0524D0 27A50020 */   addiu $a1, $sp, 0x20
/* 087004 7F0524D4 27AF0020 */  addiu $t7, $sp, 0x20
/* 087008 7F0524D8 AFAF0068 */  sw    $t7, 0x68($sp)
.L7F0524DC:
/* 08700C 7F0524DC 8E380008 */  lw    $t8, 8($s1)
/* 087010 7F0524E0 8704000E */  lh    $a0, 0xe($t8)
/* 087014 7F0524E4 0004C980 */  sll   $t9, $a0, 6
/* 087018 7F0524E8 0FC2F5C5 */  jal   dynAllocate
/* 08701C 7F0524EC 03202025 */   move  $a0, $t9
/* 087020 7F0524F0 AFA20078 */  sw    $v0, 0x78($sp)
/* 087024 7F0524F4 27A40068 */  addiu $a0, $sp, 0x68
/* 087028 7F0524F8 0FC1BC92 */  jal   instcalcmatrices
/* 08702C 7F0524FC 02202825 */   move  $a1, $s1
/* 087030 7F052500 8FA200C0 */  lw    $v0, 0xc0($sp)
/* 087034 7F052504 5040000F */  beql  $v0, $zero, .L7F052544
/* 087038 7F052508 8FBF001C */   lw    $ra, 0x1c($sp)
/* 08703C 7F05250C 8E0A0064 */  lw    $t2, 0x64($s0)
/* 087040 7F052510 02202825 */  move  $a1, $s1
/* 087044 7F052514 314B0080 */  andi  $t3, $t2, 0x80
/* 087048 7F052518 5560000A */  bnezl $t3, .L7F052544
/* 08704C 7F05251C 8FBF001C */   lw    $ra, 0x1c($sp)
/* 087050 7F052520 0FC1AC48 */  jal   sub_GAME_7F06B120
/* 087054 7F052524 8C440000 */   lw    $a0, ($v0)
/* 087058 7F052528 8FA900C0 */  lw    $t1, 0xc0($sp)
/* 08705C 7F05252C 10000004 */  b     .L7F052540
/* 087060 7F052530 AD220000 */   sw    $v0, ($t1)
.L7F052534:
/* 087064 7F052534 90480001 */  lbu   $t0, 1($v0)
.L7F052538:
/* 087068 7F052538 310CFFFD */  andi  $t4, $t0, 0xfffd
/* 08706C 7F05253C A04C0001 */  sb    $t4, 1($v0)
.L7F052540:
/* 087070 7F052540 8FBF001C */  lw    $ra, 0x1c($sp)
.L7F052544:
/* 087074 7F052544 8FB00014 */  lw    $s0, 0x14($sp)
/* 087078 7F052548 8FB10018 */  lw    $s1, 0x18($sp)
/* 08707C 7F05254C 03E00008 */  jr    $ra
/* 087080 7F052550 27BD00B8 */   addiu $sp, $sp, 0xb8
)
#endif





void redirect_object_collectability_routines(void)
{
    object_collectability_routines();
}





void weaponSetGunfireVisible(PropRecord *prop, s32 firing)
{
    ObjectRecord *obj = prop->obj;
    Model *model = obj->model;
    ModelNode *node;

    if (model && model->obj->Skeleton == &skeleton_prop_weapon) {
        node = model->obj->Switches[0];
        if (node) {
            struct ModelRwData_GunfireRecord *rwdata = modelGetNodeRwData(model, node);
            rwdata->visible = firing;
        }

        node = model->obj->Switches[2];
        if (node) {
            struct ModelRwData_BSPRecord *rwdata = modelGetNodeRwData(model, node);
            rwdata->visible = firing;
        }
    }
}



s32 weaponIsGunfireVisible(PropRecord *prop)
{
    ObjectRecord *obj = prop->obj;
    Model *model = obj->model;
    ModelNode *node;

    if (model && model->obj->Skeleton == &skeleton_prop_weapon) {
        node = model->obj->Switches[0];
        if (node) {
            struct ModelRwData_GunfireRecord *rwdata = modelGetNodeRwData(model, node);
            return rwdata->visible;
        }

        node = model->obj->Switches[2];
        if (node) {
            struct ModelRwData_BSPRecord *rwdata = modelGetNodeRwData(model, node);
            return rwdata->visible;
        }
    }

    return FALSE;
}




// getHatType
HATTYPE get_hat_model(PropRecord *prop) //#MATCH
{
    ObjectRecord *objinst = prop->obj;
    switch (objinst->obj)
    {
        case PROP_HATFURRY:
        case PROP_HATFURRYBROWN:
        case PROP_HATFURRYBLACK:
        {
            return HATTYPE_FURRY;
        }

        case PROP_HATTBIRD:
        case PROP_HATTBIRDBROWN:
        {
            return HATTYPE_BIRD;
        }

        case PROP_HATHELMET:
        case PROP_HATHELMETGREY:
        {
            return HATTYPE_HELMATE;
        }

        case PROP_HATMOON:

        {
            return HATTYPE_MOON;
        }
        case PROP_HATBERET:
        case PROP_HATBERETBLUE:
        case PROP_HATBERETRED:
        {
            return HATTYPE_BERRET;
        }

        case PROP_HATPEAKED:
        {
            return HATTYPE_PEAKED;
        }

        default:
        {
            return HATTYPE_OTHER;
        }
    }
}





/**
 * US address 7F0526EC.
*/
void door7F0526EC(DoorRecord *door, Mtxf *rhs)
{
    Mtxf lhs;
    struct coord3d sp54;
    struct coord3d sp48;
    BoundPadRecord* temp_v0_2;
    struct coord3d sp38;
    struct coord3d sp2C;

    if ((door->doorType == 5) || (door->doorType == 9))
    {
        temp_v0_2 = &g_CurrentSetup.boundpads[door->pad];

        sp38.f[0] = (temp_v0_2->up.f[1] * temp_v0_2->look.f[2]) - (temp_v0_2->up.f[2] * temp_v0_2->look.f[1]); // cross product
        sp38.f[1] = (temp_v0_2->up.f[2] * temp_v0_2->look.f[0]) - (temp_v0_2->up.f[0] * temp_v0_2->look.f[2]); // cross product
        sp38.f[2] = (temp_v0_2->up.f[0] * temp_v0_2->look.f[1]) - (temp_v0_2->up.f[1] * temp_v0_2->look.f[0]); // cross product

        sp54.f[0] = temp_v0_2->pos.f[0] + (temp_v0_2->up.f[0] * temp_v0_2->bbox.ymin);
        sp54.f[1] = temp_v0_2->pos.f[1] + (temp_v0_2->up.f[1] * temp_v0_2->bbox.ymin);
        sp54.f[2] = temp_v0_2->pos.f[2] + (temp_v0_2->up.f[2] * temp_v0_2->bbox.ymin);

        if (door->doorType == 9)
        {
            sp54.f[0] += sp38.f[0] * temp_v0_2->bbox.xmax;
            sp54.f[1] += sp38.f[1] * temp_v0_2->bbox.xmax;
            sp54.f[2] += sp38.f[2] * temp_v0_2->bbox.xmax;
        }
        else if (door->flags & PROPFLAG_DOOR_OPENTOFRONT)
        {
            sp54.f[0] += sp38.f[0] * temp_v0_2->bbox.xmax;
            sp54.f[1] += sp38.f[1] * temp_v0_2->bbox.xmax;
            sp54.f[2] += sp38.f[2] * temp_v0_2->bbox.xmax;
        }
        else
        {
            sp54.f[0] += sp38.f[0] * temp_v0_2->bbox.xmin;
            sp54.f[1] += sp38.f[1] * temp_v0_2->bbox.xmin;
            sp54.f[2] += sp38.f[2] * temp_v0_2->bbox.xmin;
        }

        sp48.f[0] = door->runtime_pos.f[0] - sp54.f[0];
        sp48.f[1] = door->runtime_pos.f[1] - sp54.f[1];
        sp48.f[2] = door->runtime_pos.f[2] - sp54.f[2];

        matrix_4x4_copy(&door->mtx, rhs);
        matrix_4x4_set_identity_and_position(&sp48, &lhs);
        matrix_4x4_multiply_in_place(&lhs, rhs);

        if (door->doorType == 9)
        {
            if (door->flags & PROPFLAG_DOOR_OPENTOFRONT)
            {
                matrix_4x4_set_rotation_around_z(M_TAU_F - ((door->openPosition * M_TAU_F) / 360.0f), &lhs);
            }
            else
            {
                matrix_4x4_set_rotation_around_z((door->openPosition * M_TAU_F) / 360.0f, &lhs);
            }
        }
        else if (door->flags & PROPFLAG_DOOR_OPENTOFRONT)
        {
            matrix_4x4_set_rotation_around_y(M_TAU_F - ((door->openPosition * M_TAU_F) / 360.0f), &lhs);
        }
        else
        {
            matrix_4x4_set_rotation_around_y((door->openPosition * M_TAU_F) / 360.0f, &lhs);
        }

        matrix_4x4_multiply_in_place(&lhs, rhs);
        matrix_4x4_set_identity_and_position(&sp54, &lhs);
        matrix_4x4_multiply_in_place(&lhs, rhs);
    }
    else if ((door->doorType == 6) || (door->doorType == 7))
    {
        matrix_4x4_copy(&door->mtx, rhs);
        matrix_4x4_set_position(&door->runtime_pos, rhs);
    }
    else
    {
        sp2C.f[0] = (door->frac * door->openPosition) + door->runtime_pos.x;
        sp2C.f[1] = (door->unkac * door->openPosition) + door->runtime_pos.y;
        sp2C.f[2] = (door->unkb0 * door->openPosition) + door->runtime_pos.z;

        matrix_4x4_copy(&door->mtx, rhs);
        matrix_4x4_set_position(&sp2C, rhs);
    }

    if (door->doorFlags & 8)
    {
        matrix_column_3_scalar_multiply_2(-1.0f, rhs);
    }
}



/**
 * NTSC address 0x7F052B00.
*/
void door7F052B00(DoorRecord *door)
{
    struct ModelRoData_BoundingBoxRecord *door_bb;
    Mtxf sp2C;

    door_bb = (struct ModelRoData_BoundingBoxRecord *)door->model->obj->RootNode->Child->Data;

    // struct copy
    door->bbox = *door_bb;

    if (door->doorFlags & DOORFLAG_0004)
    {
        if (door->doorType == DOORTYPE_VERTICAL)
        {
            door->bbox.Bounds.ymax = door_bb->Bounds.ymax + (door_bb->Bounds.ymin - door_bb->Bounds.ymax) * door->openPosition;
        }
        else
        {
            door->bbox.Bounds.xmin = door_bb->Bounds.xmin + (door_bb->Bounds.xmax - door_bb->Bounds.xmin) * door->openPosition;
        }
    }

    if (door->perimFrac <= door->openPosition)
    {
        door->ptr_allocated_collisiondata_block->unk00 = 0;

        return;
    }

    door7F0526EC(door, &sp2C);
    sub_GAME_7F03F540(&door->bbox, &sp2C, &door->ptr_allocated_collisiondata_block->unk04, door->ptr_allocated_collisiondata_block);

    if (door->doorType == DOORTYPE_VERTICAL)
    {
        door->ptr_allocated_collisiondata_block->unk48 = door->runtime_pos.f[1] + chrpropSumMatrixPosY(&door->bbox, &sp2C);
    }
    else if (door->doorType == DOORTYPE_FALLAWAY)
    {
        door->ptr_allocated_collisiondata_block->unk48 = door->runtime_pos.f[1] - 10000.0f;
    }
    else
    {
        door->ptr_allocated_collisiondata_block->unk48 = sp2C.m[3][1] + chrpropSumMatrixPosY(&door->bbox, &sp2C);

        if (door->doorFlags & DOORFLAG_0001)
        {
            door->ptr_allocated_collisiondata_block->unk48 -= 1000.0f;
        }
    }

    if (((door->doorType == DOORTYPE_EYE) && (0 < door->openPosition - (0.4f * door->maxFrac)))
        || ((door->doorType == DOORTYPE_IRIS) && (0 < door->openPosition - (0.4f * door->maxFrac)))
        )
    {

        door->ptr_allocated_collisiondata_block->unk44 = door->ptr_allocated_collisiondata_block->unk48 + 50.0f;
    }
    else if (door->doorType == DOORTYPE_FALLAWAY)
    {
        door->ptr_allocated_collisiondata_block->unk44 = door->runtime_pos.f[1] + 1000.0f;
    }
    else
    {
        door->ptr_allocated_collisiondata_block->unk44 = sp2C.m[3][1] + chrpropSumMatrixNegY(&door->bbox, &sp2C);

        if (door->doorFlags & DOORFLAG_0001)
        {
            door->ptr_allocated_collisiondata_block->unk44 += 1000.0f;
        }
    }

}



#ifdef NONMATCHING
/**
 * NTSC address 0x7F052D8C.
 * perfect dark void door0f08cb20(struct doorobj *door, Vtx *src, Vtx *dst, s32 numvertices)
 *
 * https://decomp.me/scratch/ccGWm
*/
void sub_GAME_7F052D8C(DoorRecord *door)
{
#define CYCLIC_NEXT1ALT(j) (j + 1) % 4
#define CYCLIC_NEXT2ALT(j) (j + 2) % 4
#define CYCLIC_NEXT3ALT(j) (j + 3) % 4

#define CYCLIC_NEXT1(j) (j + 1) % 4
#define CYCLIC_NEXT2(j) (j + 2) % 4
#define CYCLIC_NEXT3(j) (j + 3) % 4

    Model                                          *mdl;
    ModelNode                                      *mdlDLCNode;
    struct ModelRoData_DisplayList_CollisionRecord *src;
    struct ModelRwData_DisplayList_CollisionRecord *dst;
    s16                                             cutoff;
    s32                                             var_fp;
    s32                                             j;
    s32                                             k;

    Vertex                                         *psrc;
    Vertex                                         *pdst;

    if (door->doorFlags & DOORFLAG_0004)
    {
        mdl = door->model;
        mdlDLCNode = mdl->obj->RootNode->Child->Child; //Get the DL
        src = (struct ModelRoData_DisplayList_CollisionRecord *)mdlDLCNode->Data;
        dst = (struct ModelRwData_DisplayList_CollisionRecord *)modelGetNodeRwData(mdl, mdlDLCNode);

        if (door->doorType == DOORTYPE_VERTICAL)
        {
            cutoff = door->bbox.Bounds.ymax + 0.5f;
        }
        else
        {
            cutoff = door->bbox.Bounds.xmin + 0.5f;
        }

        dst->Vertices = dynAllocate7F0BD6C4(src->numVertices);

        for (var_fp = 0; var_fp < src->numVertices / 4; var_fp++) //block of 4 vertices (quad)
        {
            for (j = 0; j < 4; j++) //for each vertex in block, move and clamp to bounding box, if clamped, move texture coords so it doesnt look "squished"
            {
                psrc = &src->Vertices[var_fp * 4];
                pdst = &dst->Vertices[var_fp * 4];

                if (j == 0)
                {
                    pdst[j]       = psrc[j];
                    pdst[CYCLIC_NEXT1(j)] = psrc[CYCLIC_NEXT1(j)];
                    pdst[CYCLIC_NEXT2(j)] = psrc[CYCLIC_NEXT2(j)];
                    pdst[CYCLIC_NEXT3(j)] = psrc[CYCLIC_NEXT3(j)];
                    // if (1);
                }

                if (door->doorType == DOORTYPE_VERTICAL)
                {
                    //if current vtx is higher than "cutoff", clamp it to cutoff.
                    if (psrc[j].coord.y >= cutoff)
                    {
                        //if next and current x and z are equal AND y Not equal - Find the "below" vertex in a quad
                        if (psrc[CYCLIC_NEXT1(j)].coord.x == psrc[j].coord.x && psrc[CYCLIC_NEXT1(j)].coord.z == psrc[j].coord.z && psrc[CYCLIC_NEXT1(j)].coord.y != psrc[j].coord.y)
                        {
                            //InterpolatedValue = InitialValue + (Difference1) * (ChangeInValue) / (Difference2);
                            pdst[j].s = psrc[j].s + (psrc[j].coord.y - cutoff) * (psrc[CYCLIC_NEXT1(j)].s - psrc[j].s) / (psrc[j].coord.y - psrc[CYCLIC_NEXT1(j)].coord.y);
                            pdst[j].t = psrc[j].t + (psrc[j].coord.y - cutoff) * (psrc[CYCLIC_NEXT1(j)].t - psrc[j].t) / (psrc[j].coord.y - psrc[CYCLIC_NEXT1(j)].coord.y);
                        }
                        else if (psrc[CYCLIC_NEXT2(j)].coord.x == psrc[j].coord.x && psrc[CYCLIC_NEXT2(j)].coord.z == psrc[j].coord.z && psrc[CYCLIC_NEXT2(j)].coord.y != psrc[j].coord.y)
                        {
                            pdst[j].s = psrc[j].s + (psrc[j].coord.y - cutoff) * (psrc[CYCLIC_NEXT2(j)].s - psrc[j].s) / (psrc[j].coord.y - psrc[CYCLIC_NEXT2(j)].coord.y);
                            pdst[j].t = psrc[j].t + (psrc[j].coord.y - cutoff) * (psrc[CYCLIC_NEXT2(j)].t - psrc[j].t) / (psrc[j].coord.y - psrc[CYCLIC_NEXT2(j)].coord.y);
                        }
                        else if (psrc[CYCLIC_NEXT3(j)].coord.x == psrc[j].coord.x && psrc[CYCLIC_NEXT3(j)].coord.z == psrc[j].coord.z && psrc[CYCLIC_NEXT3(j)].coord.y != psrc[j].coord.y)
                        {
                            pdst[j].s = psrc[j].s + (psrc[j].coord.y - cutoff) * (psrc[CYCLIC_NEXT3(j)].s - psrc[j].s) / (psrc[j].coord.y - psrc[CYCLIC_NEXT3(j)].coord.y);
                            pdst[j].t = psrc[j].t + (psrc[j].coord.y - cutoff) * (psrc[CYCLIC_NEXT3(j)].t - psrc[j].t) / (psrc[j].coord.y - psrc[CYCLIC_NEXT3(j)].coord.y);
                        }

                        pdst[j].coord.y = cutoff;
                    }
                }
                else
                {
                    if (psrc[j].coord.x <= cutoff)
                    {
                        //if next and current y and z are equal AND x Not equal  - Find the "right" vertex in a quad
                        if (psrc[CYCLIC_NEXT1(j)].coord.y == psrc[j].coord.y && psrc[CYCLIC_NEXT1(j)].coord.z == psrc[j].coord.z && psrc[CYCLIC_NEXT1(j)].coord.x != psrc[j].coord.x)
                        {
                            pdst[j].s = psrc[j].s + (cutoff - psrc[j].coord.x) * (psrc[CYCLIC_NEXT1(j)].s - psrc[j].s) / (psrc[CYCLIC_NEXT1(j)].coord.x - psrc[j].coord.x);
                            pdst[j].t = psrc[j].t + (cutoff - psrc[j].coord.x) * (psrc[CYCLIC_NEXT1(j)].t - psrc[j].t) / (psrc[CYCLIC_NEXT1(j)].coord.x - psrc[j].coord.x);
                        }
                        else if (psrc[CYCLIC_NEXT2(j)].coord.y == psrc[j].coord.y && psrc[CYCLIC_NEXT2(j)].coord.z == psrc[j].coord.z && psrc[CYCLIC_NEXT2(j)].coord.x != psrc[j].coord.x)
                        {
                            pdst[j].s = psrc[j].s + (cutoff - psrc[j].coord.x) * (psrc[CYCLIC_NEXT2(j)].s - psrc[j].s) / (psrc[CYCLIC_NEXT2(j)].coord.x - psrc[j].coord.x);
                            pdst[j].t = psrc[j].t + (cutoff - psrc[j].coord.x) * (psrc[CYCLIC_NEXT2(j)].t - psrc[j].t) / (psrc[CYCLIC_NEXT2(j)].coord.x - psrc[j].coord.x);
                        }
                        else if (psrc[CYCLIC_NEXT3(j)].coord.y == psrc[j].coord.y && psrc[CYCLIC_NEXT3(j)].coord.z == psrc[j].coord.z && psrc[CYCLIC_NEXT3(j)].coord.x != psrc[j].coord.x)
                        {
                            pdst[j].s = psrc[j].s + (cutoff - psrc[j].coord.x) * (psrc[CYCLIC_NEXT3(j)].s - psrc[j].s) / (psrc[CYCLIC_NEXT3(j)].coord.x - psrc[j].coord.x);
                            pdst[j].t = psrc[j].t + (cutoff - psrc[j].coord.x) * (psrc[CYCLIC_NEXT3(j)].t - psrc[j].t) / (psrc[CYCLIC_NEXT3(j)].coord.x - psrc[j].coord.x);
                        }

                        pdst[j].coord.x = cutoff;
                    }
                }
            }
        }
    }
}

#else
void sub_GAME_7F052D8C(DoorRecord*);
GLOBAL_ASM(
.text
glabel sub_GAME_7F052D8C
/* 0878BC 7F052D8C 27BDFFA8 */  addiu $sp, $sp, -0x58
/* 0878C0 7F052D90 AFBF003C */  sw    $ra, 0x3c($sp)
/* 0878C4 7F052D94 AFBE0038 */  sw    $fp, 0x38($sp)
/* 0878C8 7F052D98 AFB70034 */  sw    $s7, 0x34($sp)
/* 0878CC 7F052D9C AFB60030 */  sw    $s6, 0x30($sp)
/* 0878D0 7F052DA0 AFB5002C */  sw    $s5, 0x2c($sp)
/* 0878D4 7F052DA4 AFB40028 */  sw    $s4, 0x28($sp)
/* 0878D8 7F052DA8 AFB30024 */  sw    $s3, 0x24($sp)
/* 0878DC 7F052DAC AFB20020 */  sw    $s2, 0x20($sp)
/* 0878E0 7F052DB0 AFB1001C */  sw    $s1, 0x1c($sp)
/* 0878E4 7F052DB4 AFB00018 */  sw    $s0, 0x18($sp)
/* 0878E8 7F052DB8 948E0098 */  lhu   $t6, 0x98($a0)
/* 0878EC 7F052DBC 0080B025 */  move  $s6, $a0
/* 0878F0 7F052DC0 31CF0004 */  andi  $t7, $t6, 4
/* 0878F4 7F052DC4 51E001E9 */  beql  $t7, $zero, .L7F05356C
/* 0878F8 7F052DC8 8FBF003C */   lw    $ra, 0x3c($sp)
/* 0878FC 7F052DCC 8C840014 */  lw    $a0, 0x14($a0)
/* 087900 7F052DD0 8C980008 */  lw    $t8, 8($a0)
/* 087904 7F052DD4 8F190000 */  lw    $t9, ($t8)
/* 087908 7F052DD8 8F2E0014 */  lw    $t6, 0x14($t9)
/* 08790C 7F052DDC 8DC50014 */  lw    $a1, 0x14($t6)
/* 087910 7F052DE0 0FC1B1E7 */  jal   modelGetNodeRwData
/* 087914 7F052DE4 8CB50004 */   lw    $s5, 4($a1)
/* 087918 7F052DE8 96CF009A */  lhu   $t7, 0x9a($s6)
/* 08791C 7F052DEC 24140004 */  li    $s4, 4
/* 087920 7F052DF0 0040B825 */  move  $s7, $v0
/* 087924 7F052DF4 168F000B */  bne   $s4, $t7, .L7F052E24
/* 087928 7F052DF8 3C013F00 */   li    $at, 0x3F000000 # 0.500000
/* 08792C 7F052DFC 3C013F00 */  li    $at, 0x3F000000 # 0.500000
/* 087930 7F052E00 44813000 */  mtc1  $at, $f6
/* 087934 7F052E04 C6C400E0 */  lwc1  $f4, 0xe0($s6)
/* 087938 7F052E08 46062200 */  add.s $f8, $f4, $f6
/* 08793C 7F052E0C 4600428D */  trunc.w.s $f10, $f8
/* 087940 7F052E10 440D5000 */  mfc1  $t5, $f10
/* 087944 7F052E14 00000000 */  nop
/* 087948 7F052E18 000DCC00 */  sll   $t9, $t5, 0x10
/* 08794C 7F052E1C 1000000A */  b     .L7F052E48
/* 087950 7F052E20 00196C03 */   sra   $t5, $t9, 0x10
.L7F052E24:
/* 087954 7F052E24 C6D000D4 */  lwc1  $f16, 0xd4($s6)
/* 087958 7F052E28 44819000 */  mtc1  $at, $f18
/* 08795C 7F052E2C 00000000 */  nop
/* 087960 7F052E30 46128100 */  add.s $f4, $f16, $f18
/* 087964 7F052E34 4600218D */  trunc.w.s $f6, $f4
/* 087968 7F052E38 440D3000 */  mfc1  $t5, $f6
/* 08796C 7F052E3C 00000000 */  nop
/* 087970 7F052E40 000DC400 */  sll   $t8, $t5, 0x10
/* 087974 7F052E44 00186C03 */  sra   $t5, $t8, 0x10
.L7F052E48:
/* 087978 7F052E48 86A4000C */  lh    $a0, 0xc($s5)
/* 08797C 7F052E4C 0FC2F5B1 */  jal   dynAllocate7F0BD6C4
/* 087980 7F052E50 A7AD0042 */   sh    $t5, 0x42($sp)
/* 087984 7F052E54 87AD0042 */  lh    $t5, 0x42($sp)
/* 087988 7F052E58 AEE20000 */  sw    $v0, ($s7)
/* 08798C 7F052E5C 86AE000C */  lh    $t6, 0xc($s5)
/* 087990 7F052E60 00001825 */  move  $v1, $zero
/* 087994 7F052E64 0000F025 */  move  $fp, $zero
/* 087998 7F052E68 05C10003 */  bgez  $t6, .L7F052E78
/* 08799C 7F052E6C 000E7883 */   sra   $t7, $t6, 2
/* 0879A0 7F052E70 25C10003 */  addiu $at, $t6, 3
/* 0879A4 7F052E74 00017883 */  sra   $t7, $at, 2
.L7F052E78:
/* 0879A8 7F052E78 19E001BB */  blez  $t7, .L7F053568
/* 0879AC 7F052E7C 00001825 */   move  $v1, $zero
.L7F052E80:
/* 0879B0 7F052E80 001E9900 */  sll   $s3, $fp, 4
.L7F052E84:
/* 0879B4 7F052E84 8EF80000 */  lw    $t8, ($s7)
/* 0879B8 7F052E88 24670002 */  addiu $a3, $v1, 2
/* 0879BC 7F052E8C 246C0003 */  addiu $t4, $v1, 3
/* 0879C0 7F052E90 03131021 */  addu  $v0, $t8, $s3
/* 0879C4 7F052E94 24720001 */  addiu $s2, $v1, 1
/* 0879C8 7F052E98 00032100 */  sll   $a0, $v1, 4
/* 0879CC 7F052E9C 00445821 */  addu  $t3, $v0, $a0
/* 0879D0 7F052EA0 06410004 */  bgez  $s2, .L7F052EB4
/* 0879D4 7F052EA4 32450003 */   andi  $a1, $s2, 3
/* 0879D8 7F052EA8 10A00002 */  beqz  $a1, .L7F052EB4
/* 0879DC 7F052EAC 00000000 */   nop
/* 0879E0 7F052EB0 24A5FFFC */  addiu $a1, $a1, -4
.L7F052EB4:
/* 0879E4 7F052EB4 0005C900 */  sll   $t9, $a1, 4
/* 0879E8 7F052EB8 03202825 */  move  $a1, $t9
/* 0879EC 7F052EBC 0059F821 */  addu  $ra, $v0, $t9
/* 0879F0 7F052EC0 04E10004 */  bgez  $a3, .L7F052ED4
/* 0879F4 7F052EC4 30EE0003 */   andi  $t6, $a3, 3
/* 0879F8 7F052EC8 11C00002 */  beqz  $t6, .L7F052ED4
/* 0879FC 7F052ECC 00000000 */   nop
/* 087A00 7F052ED0 25CEFFFC */  addiu $t6, $t6, -4
.L7F052ED4:
/* 087A04 7F052ED4 000E7900 */  sll   $t7, $t6, 4
/* 087A08 7F052ED8 8EAE0008 */  lw    $t6, 8($s5)
/* 087A0C 7F052EDC 01E03825 */  move  $a3, $t7
/* 087A10 7F052EE0 004F8021 */  addu  $s0, $v0, $t7
/* 087A14 7F052EE4 01D34821 */  addu  $t1, $t6, $s3
/* 087A18 7F052EE8 05810004 */  bgez  $t4, .L7F052EFC
/* 087A1C 7F052EEC 31980003 */   andi  $t8, $t4, 3
/* 087A20 7F052EF0 13000002 */  beqz  $t8, .L7F052EFC
/* 087A24 7F052EF4 00000000 */   nop
/* 087A28 7F052EF8 2718FFFC */  addiu $t8, $t8, -4
.L7F052EFC:
/* 087A2C 7F052EFC 00186100 */  sll   $t4, $t8, 4
/* 087A30 7F052F00 01275021 */  addu  $t2, $t1, $a3
/* 087A34 7F052F04 004C8821 */  addu  $s1, $v0, $t4
/* 087A38 7F052F08 01243021 */  addu  $a2, $t1, $a0
/* 087A3C 7F052F0C 01254021 */  addu  $t0, $t1, $a1
/* 087A40 7F052F10 14600021 */  bnez  $v1, .L7F052F98
/* 087A44 7F052F14 012C3821 */   addu  $a3, $t1, $t4
/* 087A48 7F052F18 8CC10000 */  lw    $at, ($a2)
/* 087A4C 7F052F1C AD610000 */  sw    $at, ($t3)
/* 087A50 7F052F20 8CD80004 */  lw    $t8, 4($a2)
/* 087A54 7F052F24 AD780004 */  sw    $t8, 4($t3)
/* 087A58 7F052F28 8CC10008 */  lw    $at, 8($a2)
/* 087A5C 7F052F2C AD610008 */  sw    $at, 8($t3)
/* 087A60 7F052F30 8CD8000C */  lw    $t8, 0xc($a2)
/* 087A64 7F052F34 AD78000C */  sw    $t8, 0xc($t3)
/* 087A68 7F052F38 8D010000 */  lw    $at, ($t0)
/* 087A6C 7F052F3C AFE10000 */  sw    $at, ($ra)
/* 087A70 7F052F40 8D0E0004 */  lw    $t6, 4($t0)
/* 087A74 7F052F44 AFEE0004 */  sw    $t6, 4($ra)
/* 087A78 7F052F48 8D010008 */  lw    $at, 8($t0)
/* 087A7C 7F052F4C AFE10008 */  sw    $at, 8($ra)
/* 087A80 7F052F50 8D0E000C */  lw    $t6, 0xc($t0)
/* 087A84 7F052F54 AFEE000C */  sw    $t6, 0xc($ra)
/* 087A88 7F052F58 8D410000 */  lw    $at, ($t2)
/* 087A8C 7F052F5C AE010000 */  sw    $at, ($s0)
/* 087A90 7F052F60 8D580004 */  lw    $t8, 4($t2)
/* 087A94 7F052F64 AE180004 */  sw    $t8, 4($s0)
/* 087A98 7F052F68 8D410008 */  lw    $at, 8($t2)
/* 087A9C 7F052F6C AE010008 */  sw    $at, 8($s0)
/* 087AA0 7F052F70 8D58000C */  lw    $t8, 0xc($t2)
/* 087AA4 7F052F74 AE18000C */  sw    $t8, 0xc($s0)
/* 087AA8 7F052F78 8CE10000 */  lw    $at, ($a3)
/* 087AAC 7F052F7C AE210000 */  sw    $at, ($s1)
/* 087AB0 7F052F80 8CEE0004 */  lw    $t6, 4($a3)
/* 087AB4 7F052F84 AE2E0004 */  sw    $t6, 4($s1)
/* 087AB8 7F052F88 8CE10008 */  lw    $at, 8($a3)
/* 087ABC 7F052F8C AE210008 */  sw    $at, 8($s1)
/* 087AC0 7F052F90 8CEE000C */  lw    $t6, 0xc($a3)
/* 087AC4 7F052F94 AE2E000C */  sw    $t6, 0xc($s1)
.L7F052F98:
/* 087AC8 7F052F98 96CF009A */  lhu   $t7, 0x9a($s6)
/* 087ACC 7F052F9C 568F00B5 */  bnel  $s4, $t7, .L7F053274
/* 087AD0 7F052FA0 84C50000 */   lh    $a1, ($a2)
/* 087AD4 7F052FA4 84C40002 */  lh    $a0, 2($a2)
/* 087AD8 7F052FA8 008D082A */  slt   $at, $a0, $t5
/* 087ADC 7F052FAC 14200162 */  bnez  $at, .L7F053538
/* 087AE0 7F052FB0 00000000 */   nop
/* 087AE4 7F052FB4 84C50000 */  lh    $a1, ($a2)
/* 087AE8 7F052FB8 85180000 */  lh    $t8, ($t0)
/* 087AEC 7F052FBC 57050039 */  bnel  $t8, $a1, .L7F0530A4
/* 087AF0 7F052FC0 85590000 */   lh    $t9, ($t2)
/* 087AF4 7F052FC4 85190004 */  lh    $t9, 4($t0)
/* 087AF8 7F052FC8 84CE0004 */  lh    $t6, 4($a2)
/* 087AFC 7F052FCC 572E0035 */  bnel  $t9, $t6, .L7F0530A4
/* 087B00 7F052FD0 85590000 */   lh    $t9, ($t2)
/* 087B04 7F052FD4 85090002 */  lh    $t1, 2($t0)
/* 087B08 7F052FD8 51240032 */  beql  $t1, $a0, .L7F0530A4
/* 087B0C 7F052FDC 85590000 */   lh    $t9, ($t2)
/* 087B10 7F052FE0 84C20008 */  lh    $v0, 8($a2)
/* 087B14 7F052FE4 85180008 */  lh    $t8, 8($t0)
/* 087B18 7F052FE8 008D7823 */  subu  $t7, $a0, $t5
/* 087B1C 7F052FEC 0302C823 */  subu  $t9, $t8, $v0
/* 087B20 7F052FF0 01F90019 */  multu $t7, $t9
/* 087B24 7F052FF4 0089C023 */  subu  $t8, $a0, $t1
/* 087B28 7F052FF8 00007012 */  mflo  $t6
/* 087B2C 7F052FFC 00000000 */  nop
/* 087B30 7F053000 00000000 */  nop
/* 087B34 7F053004 01D8001A */  div   $zero, $t6, $t8
/* 087B38 7F053008 00007812 */  mflo  $t7
/* 087B3C 7F05300C 01E2C821 */  addu  $t9, $t7, $v0
/* 087B40 7F053010 A5790008 */  sh    $t9, 8($t3)
/* 087B44 7F053014 17000002 */  bnez  $t8, .L7F053020
/* 087B48 7F053018 00000000 */   nop
/* 087B4C 7F05301C 0007000D */  break 7
.L7F053020:
/* 087B50 7F053020 2401FFFF */  li    $at, -1
/* 087B54 7F053024 17010004 */  bne   $t8, $at, .L7F053038
/* 087B58 7F053028 3C018000 */   lui   $at, 0x8000
/* 087B5C 7F05302C 15C10002 */  bne   $t6, $at, .L7F053038
/* 087B60 7F053030 00000000 */   nop
/* 087B64 7F053034 0006000D */  break 6
.L7F053038:
/* 087B68 7F053038 8518000A */  lh    $t8, 0xa($t0)
/* 087B6C 7F05303C 84C3000A */  lh    $v1, 0xa($a2)
/* 087B70 7F053040 84C40002 */  lh    $a0, 2($a2)
/* 087B74 7F053044 03037823 */  subu  $t7, $t8, $v1
/* 087B78 7F053048 008D7023 */  subu  $t6, $a0, $t5
/* 087B7C 7F05304C 01CF0019 */  multu $t6, $t7
/* 087B80 7F053050 85180002 */  lh    $t8, 2($t0)
/* 087B84 7F053054 00987023 */  subu  $t6, $a0, $t8
/* 087B88 7F053058 0000C812 */  mflo  $t9
/* 087B8C 7F05305C 00000000 */  nop
/* 087B90 7F053060 00000000 */  nop
/* 087B94 7F053064 032E001A */  div   $zero, $t9, $t6
/* 087B98 7F053068 00007812 */  mflo  $t7
/* 087B9C 7F05306C 01E3C021 */  addu  $t8, $t7, $v1
/* 087BA0 7F053070 15C00002 */  bnez  $t6, .L7F05307C
/* 087BA4 7F053074 00000000 */   nop
/* 087BA8 7F053078 0007000D */  break 7
.L7F05307C:
/* 087BAC 7F05307C 2401FFFF */  li    $at, -1
/* 087BB0 7F053080 15C10004 */  bne   $t6, $at, .L7F053094
/* 087BB4 7F053084 3C018000 */   lui   $at, 0x8000
/* 087BB8 7F053088 17210002 */  bne   $t9, $at, .L7F053094
/* 087BBC 7F05308C 00000000 */   nop
/* 087BC0 7F053090 0006000D */  break 6
.L7F053094:
/* 087BC4 7F053094 A578000A */  sh    $t8, 0xa($t3)
/* 087BC8 7F053098 10000073 */  b     .L7F053268
/* 087BCC 7F05309C 00000000 */   nop
/* 087BD0 7F0530A0 85590000 */  lh    $t9, ($t2)
.L7F0530A4:
/* 087BD4 7F0530A4 57250039 */  bnel  $t9, $a1, .L7F05318C
/* 087BD8 7F0530A8 84EE0000 */   lh    $t6, ($a3)
/* 087BDC 7F0530AC 854E0004 */  lh    $t6, 4($t2)
/* 087BE0 7F0530B0 84CF0004 */  lh    $t7, 4($a2)
/* 087BE4 7F0530B4 55CF0035 */  bnel  $t6, $t7, .L7F05318C
/* 087BE8 7F0530B8 84EE0000 */   lh    $t6, ($a3)
/* 087BEC 7F0530BC 85480002 */  lh    $t0, 2($t2)
/* 087BF0 7F0530C0 51040032 */  beql  $t0, $a0, .L7F05318C
/* 087BF4 7F0530C4 84EE0000 */   lh    $t6, ($a3)
/* 087BF8 7F0530C8 84C20008 */  lh    $v0, 8($a2)
/* 087BFC 7F0530CC 85590008 */  lh    $t9, 8($t2)
/* 087C00 7F0530D0 008DC023 */  subu  $t8, $a0, $t5
/* 087C04 7F0530D4 03227023 */  subu  $t6, $t9, $v0
/* 087C08 7F0530D8 030E0019 */  multu $t8, $t6
/* 087C0C 7F0530DC 0088C823 */  subu  $t9, $a0, $t0
/* 087C10 7F0530E0 00007812 */  mflo  $t7
/* 087C14 7F0530E4 00000000 */  nop
/* 087C18 7F0530E8 00000000 */  nop
/* 087C1C 7F0530EC 01F9001A */  div   $zero, $t7, $t9
/* 087C20 7F0530F0 0000C012 */  mflo  $t8
/* 087C24 7F0530F4 03027021 */  addu  $t6, $t8, $v0
/* 087C28 7F0530F8 A56E0008 */  sh    $t6, 8($t3)
/* 087C2C 7F0530FC 17200002 */  bnez  $t9, .L7F053108
/* 087C30 7F053100 00000000 */   nop
/* 087C34 7F053104 0007000D */  break 7
.L7F053108:
/* 087C38 7F053108 2401FFFF */  li    $at, -1
/* 087C3C 7F05310C 17210004 */  bne   $t9, $at, .L7F053120
/* 087C40 7F053110 3C018000 */   lui   $at, 0x8000
/* 087C44 7F053114 15E10002 */  bne   $t7, $at, .L7F053120
/* 087C48 7F053118 00000000 */   nop
/* 087C4C 7F05311C 0006000D */  break 6
.L7F053120:
/* 087C50 7F053120 8559000A */  lh    $t9, 0xa($t2)
/* 087C54 7F053124 84C3000A */  lh    $v1, 0xa($a2)
/* 087C58 7F053128 84C40002 */  lh    $a0, 2($a2)
/* 087C5C 7F05312C 0323C023 */  subu  $t8, $t9, $v1
/* 087C60 7F053130 008D7823 */  subu  $t7, $a0, $t5
/* 087C64 7F053134 01F80019 */  multu $t7, $t8
/* 087C68 7F053138 85590002 */  lh    $t9, 2($t2)
/* 087C6C 7F05313C 00997823 */  subu  $t7, $a0, $t9
/* 087C70 7F053140 00007012 */  mflo  $t6
/* 087C74 7F053144 00000000 */  nop
/* 087C78 7F053148 00000000 */  nop
/* 087C7C 7F05314C 01CF001A */  div   $zero, $t6, $t7
/* 087C80 7F053150 0000C012 */  mflo  $t8
/* 087C84 7F053154 0303C821 */  addu  $t9, $t8, $v1
/* 087C88 7F053158 15E00002 */  bnez  $t7, .L7F053164
/* 087C8C 7F05315C 00000000 */   nop
/* 087C90 7F053160 0007000D */  break 7
.L7F053164:
/* 087C94 7F053164 2401FFFF */  li    $at, -1
/* 087C98 7F053168 15E10004 */  bne   $t7, $at, .L7F05317C
/* 087C9C 7F05316C 3C018000 */   lui   $at, 0x8000
/* 087CA0 7F053170 15C10002 */  bne   $t6, $at, .L7F05317C
/* 087CA4 7F053174 00000000 */   nop
/* 087CA8 7F053178 0006000D */  break 6
.L7F05317C:
/* 087CAC 7F05317C A579000A */  sh    $t9, 0xa($t3)
/* 087CB0 7F053180 10000039 */  b     .L7F053268
/* 087CB4 7F053184 00000000 */   nop
/* 087CB8 7F053188 84EE0000 */  lh    $t6, ($a3)
.L7F05318C:
/* 087CBC 7F05318C 15C50036 */  bne   $t6, $a1, .L7F053268
/* 087CC0 7F053190 00000000 */   nop
/* 087CC4 7F053194 84EF0004 */  lh    $t7, 4($a3)
/* 087CC8 7F053198 84D80004 */  lh    $t8, 4($a2)
/* 087CCC 7F05319C 15F80032 */  bne   $t7, $t8, .L7F053268
/* 087CD0 7F0531A0 00000000 */   nop
/* 087CD4 7F0531A4 84E50002 */  lh    $a1, 2($a3)
/* 087CD8 7F0531A8 10A4002F */  beq   $a1, $a0, .L7F053268
/* 087CDC 7F0531AC 00000000 */   nop
/* 087CE0 7F0531B0 84C20008 */  lh    $v0, 8($a2)
/* 087CE4 7F0531B4 84EE0008 */  lh    $t6, 8($a3)
/* 087CE8 7F0531B8 008DC823 */  subu  $t9, $a0, $t5
/* 087CEC 7F0531BC 01C27823 */  subu  $t7, $t6, $v0
/* 087CF0 7F0531C0 032F0019 */  multu $t9, $t7
/* 087CF4 7F0531C4 00857023 */  subu  $t6, $a0, $a1
/* 087CF8 7F0531C8 0000C012 */  mflo  $t8
/* 087CFC 7F0531CC 00000000 */  nop
/* 087D00 7F0531D0 00000000 */  nop
/* 087D04 7F0531D4 030E001A */  div   $zero, $t8, $t6
/* 087D08 7F0531D8 0000C812 */  mflo  $t9
/* 087D0C 7F0531DC 03227821 */  addu  $t7, $t9, $v0
/* 087D10 7F0531E0 A56F0008 */  sh    $t7, 8($t3)
/* 087D14 7F0531E4 15C00002 */  bnez  $t6, .L7F0531F0
/* 087D18 7F0531E8 00000000 */   nop
/* 087D1C 7F0531EC 0007000D */  break 7
.L7F0531F0:
/* 087D20 7F0531F0 2401FFFF */  li    $at, -1
/* 087D24 7F0531F4 15C10004 */  bne   $t6, $at, .L7F053208
/* 087D28 7F0531F8 3C018000 */   lui   $at, 0x8000
/* 087D2C 7F0531FC 17010002 */  bne   $t8, $at, .L7F053208
/* 087D30 7F053200 00000000 */   nop
/* 087D34 7F053204 0006000D */  break 6
.L7F053208:
/* 087D38 7F053208 84EE000A */  lh    $t6, 0xa($a3)
/* 087D3C 7F05320C 84C3000A */  lh    $v1, 0xa($a2)
/* 087D40 7F053210 84C40002 */  lh    $a0, 2($a2)
/* 087D44 7F053214 01C3C823 */  subu  $t9, $t6, $v1
/* 087D48 7F053218 008DC023 */  subu  $t8, $a0, $t5
/* 087D4C 7F05321C 03190019 */  multu $t8, $t9
/* 087D50 7F053220 84EE0002 */  lh    $t6, 2($a3)
/* 087D54 7F053224 008EC023 */  subu  $t8, $a0, $t6
/* 087D58 7F053228 00007812 */  mflo  $t7
/* 087D5C 7F05322C 00000000 */  nop
/* 087D60 7F053230 00000000 */  nop
/* 087D64 7F053234 01F8001A */  div   $zero, $t7, $t8
/* 087D68 7F053238 0000C812 */  mflo  $t9
/* 087D6C 7F05323C 03237021 */  addu  $t6, $t9, $v1
/* 087D70 7F053240 A56E000A */  sh    $t6, 0xa($t3)
/* 087D74 7F053244 17000002 */  bnez  $t8, .L7F053250
/* 087D78 7F053248 00000000 */   nop
/* 087D7C 7F05324C 0007000D */  break 7
.L7F053250:
/* 087D80 7F053250 2401FFFF */  li    $at, -1
/* 087D84 7F053254 17010004 */  bne   $t8, $at, .L7F053268
/* 087D88 7F053258 3C018000 */   lui   $at, 0x8000
/* 087D8C 7F05325C 15E10002 */  bne   $t7, $at, .L7F053268
/* 087D90 7F053260 00000000 */   nop
/* 087D94 7F053264 0006000D */  break 6
.L7F053268:
/* 087D98 7F053268 100000B3 */  b     .L7F053538
/* 087D9C 7F05326C A56D0002 */   sh    $t5, 2($t3)
/* 087DA0 7F053270 84C50000 */  lh    $a1, ($a2)
.L7F053274:
/* 087DA4 7F053274 01A5082A */  slt   $at, $t5, $a1
/* 087DA8 7F053278 142000AF */  bnez  $at, .L7F053538
/* 087DAC 7F05327C 00000000 */   nop
/* 087DB0 7F053280 84C40002 */  lh    $a0, 2($a2)
/* 087DB4 7F053284 850F0002 */  lh    $t7, 2($t0)
/* 087DB8 7F053288 55E40039 */  bnel  $t7, $a0, .L7F053370
/* 087DBC 7F05328C 85580002 */   lh    $t8, 2($t2)
/* 087DC0 7F053290 85180004 */  lh    $t8, 4($t0)
/* 087DC4 7F053294 84D90004 */  lh    $t9, 4($a2)
/* 087DC8 7F053298 57190035 */  bnel  $t8, $t9, .L7F053370
/* 087DCC 7F05329C 85580002 */   lh    $t8, 2($t2)
/* 087DD0 7F0532A0 85090000 */  lh    $t1, ($t0)
/* 087DD4 7F0532A4 51250032 */  beql  $t1, $a1, .L7F053370
/* 087DD8 7F0532A8 85580002 */   lh    $t8, 2($t2)
/* 087DDC 7F0532AC 84C20008 */  lh    $v0, 8($a2)
/* 087DE0 7F0532B0 850F0008 */  lh    $t7, 8($t0)
/* 087DE4 7F0532B4 01A57023 */  subu  $t6, $t5, $a1
/* 087DE8 7F0532B8 01E2C023 */  subu  $t8, $t7, $v0
/* 087DEC 7F0532BC 01D80019 */  multu $t6, $t8
/* 087DF0 7F0532C0 01257823 */  subu  $t7, $t1, $a1
/* 087DF4 7F0532C4 0000C812 */  mflo  $t9
/* 087DF8 7F0532C8 00000000 */  nop
/* 087DFC 7F0532CC 00000000 */  nop
/* 087E00 7F0532D0 032F001A */  div   $zero, $t9, $t7
/* 087E04 7F0532D4 00007012 */  mflo  $t6
/* 087E08 7F0532D8 01C2C021 */  addu  $t8, $t6, $v0
/* 087E0C 7F0532DC A5780008 */  sh    $t8, 8($t3)
/* 087E10 7F0532E0 15E00002 */  bnez  $t7, .L7F0532EC
/* 087E14 7F0532E4 00000000 */   nop
/* 087E18 7F0532E8 0007000D */  break 7
.L7F0532EC:
/* 087E1C 7F0532EC 2401FFFF */  li    $at, -1
/* 087E20 7F0532F0 15E10004 */  bne   $t7, $at, .L7F053304
/* 087E24 7F0532F4 3C018000 */   lui   $at, 0x8000
/* 087E28 7F0532F8 17210002 */  bne   $t9, $at, .L7F053304
/* 087E2C 7F0532FC 00000000 */   nop
/* 087E30 7F053300 0006000D */  break 6
.L7F053304:
/* 087E34 7F053304 850F000A */  lh    $t7, 0xa($t0)
/* 087E38 7F053308 84C3000A */  lh    $v1, 0xa($a2)
/* 087E3C 7F05330C 84C50000 */  lh    $a1, ($a2)
/* 087E40 7F053310 01E37023 */  subu  $t6, $t7, $v1
/* 087E44 7F053314 01A5C823 */  subu  $t9, $t5, $a1
/* 087E48 7F053318 032E0019 */  multu $t9, $t6
/* 087E4C 7F05331C 850F0000 */  lh    $t7, ($t0)
/* 087E50 7F053320 01E5C823 */  subu  $t9, $t7, $a1
/* 087E54 7F053324 0000C012 */  mflo  $t8
/* 087E58 7F053328 00000000 */  nop
/* 087E5C 7F05332C 00000000 */  nop
/* 087E60 7F053330 0319001A */  div   $zero, $t8, $t9
/* 087E64 7F053334 00007012 */  mflo  $t6
/* 087E68 7F053338 01C37821 */  addu  $t7, $t6, $v1
/* 087E6C 7F05333C 17200002 */  bnez  $t9, .L7F053348
/* 087E70 7F053340 00000000 */   nop
/* 087E74 7F053344 0007000D */  break 7
.L7F053348:
/* 087E78 7F053348 2401FFFF */  li    $at, -1
/* 087E7C 7F05334C 17210004 */  bne   $t9, $at, .L7F053360
/* 087E80 7F053350 3C018000 */   lui   $at, 0x8000
/* 087E84 7F053354 17010002 */  bne   $t8, $at, .L7F053360
/* 087E88 7F053358 00000000 */   nop
/* 087E8C 7F05335C 0006000D */  break 6
.L7F053360:
/* 087E90 7F053360 A56F000A */  sh    $t7, 0xa($t3)
/* 087E94 7F053364 10000074 */  b     .L7F053538
/* 087E98 7F053368 A56D0000 */   sh    $t5, ($t3)
/* 087E9C 7F05336C 85580002 */  lh    $t8, 2($t2)
.L7F053370:
/* 087EA0 7F053370 57040039 */  bnel  $t8, $a0, .L7F053458
/* 087EA4 7F053374 84F90002 */   lh    $t9, 2($a3)
/* 087EA8 7F053378 85590004 */  lh    $t9, 4($t2)
/* 087EAC 7F05337C 84CE0004 */  lh    $t6, 4($a2)
/* 087EB0 7F053380 572E0035 */  bnel  $t9, $t6, .L7F053458
/* 087EB4 7F053384 84F90002 */   lh    $t9, 2($a3)
/* 087EB8 7F053388 85480000 */  lh    $t0, ($t2)
/* 087EBC 7F05338C 51050032 */  beql  $t0, $a1, .L7F053458
/* 087EC0 7F053390 84F90002 */   lh    $t9, 2($a3)
/* 087EC4 7F053394 84C20008 */  lh    $v0, 8($a2)
/* 087EC8 7F053398 85580008 */  lh    $t8, 8($t2)
/* 087ECC 7F05339C 01A57823 */  subu  $t7, $t5, $a1
/* 087ED0 7F0533A0 0302C823 */  subu  $t9, $t8, $v0
/* 087ED4 7F0533A4 01F90019 */  multu $t7, $t9
/* 087ED8 7F0533A8 0105C023 */  subu  $t8, $t0, $a1
/* 087EDC 7F0533AC 00007012 */  mflo  $t6
/* 087EE0 7F0533B0 00000000 */  nop
/* 087EE4 7F0533B4 00000000 */  nop
/* 087EE8 7F0533B8 01D8001A */  div   $zero, $t6, $t8
/* 087EEC 7F0533BC 00007812 */  mflo  $t7
/* 087EF0 7F0533C0 01E2C821 */  addu  $t9, $t7, $v0
/* 087EF4 7F0533C4 A5790008 */  sh    $t9, 8($t3)
/* 087EF8 7F0533C8 17000002 */  bnez  $t8, .L7F0533D4
/* 087EFC 7F0533CC 00000000 */   nop
/* 087F00 7F0533D0 0007000D */  break 7
.L7F0533D4:
/* 087F04 7F0533D4 2401FFFF */  li    $at, -1
/* 087F08 7F0533D8 17010004 */  bne   $t8, $at, .L7F0533EC
/* 087F0C 7F0533DC 3C018000 */   lui   $at, 0x8000
/* 087F10 7F0533E0 15C10002 */  bne   $t6, $at, .L7F0533EC
/* 087F14 7F0533E4 00000000 */   nop
/* 087F18 7F0533E8 0006000D */  break 6
.L7F0533EC:
/* 087F1C 7F0533EC 8558000A */  lh    $t8, 0xa($t2)
/* 087F20 7F0533F0 84C3000A */  lh    $v1, 0xa($a2)
/* 087F24 7F0533F4 84C50000 */  lh    $a1, ($a2)
/* 087F28 7F0533F8 03037823 */  subu  $t7, $t8, $v1
/* 087F2C 7F0533FC 01A57023 */  subu  $t6, $t5, $a1
/* 087F30 7F053400 01CF0019 */  multu $t6, $t7
/* 087F34 7F053404 85580000 */  lh    $t8, ($t2)
/* 087F38 7F053408 03057023 */  subu  $t6, $t8, $a1
/* 087F3C 7F05340C 0000C812 */  mflo  $t9
/* 087F40 7F053410 00000000 */  nop
/* 087F44 7F053414 00000000 */  nop
/* 087F48 7F053418 032E001A */  div   $zero, $t9, $t6
/* 087F4C 7F05341C 00007812 */  mflo  $t7
/* 087F50 7F053420 01E3C021 */  addu  $t8, $t7, $v1
/* 087F54 7F053424 15C00002 */  bnez  $t6, .L7F053430
/* 087F58 7F053428 00000000 */   nop
/* 087F5C 7F05342C 0007000D */  break 7
.L7F053430:
/* 087F60 7F053430 2401FFFF */  li    $at, -1
/* 087F64 7F053434 15C10004 */  bne   $t6, $at, .L7F053448
/* 087F68 7F053438 3C018000 */   lui   $at, 0x8000
/* 087F6C 7F05343C 17210002 */  bne   $t9, $at, .L7F053448
/* 087F70 7F053440 00000000 */   nop
/* 087F74 7F053444 0006000D */  break 6
.L7F053448:
/* 087F78 7F053448 A578000A */  sh    $t8, 0xa($t3)
/* 087F7C 7F05344C 1000003A */  b     .L7F053538
/* 087F80 7F053450 A56D0000 */   sh    $t5, ($t3)
/* 087F84 7F053454 84F90002 */  lh    $t9, 2($a3)
.L7F053458:
/* 087F88 7F053458 57240037 */  bnel  $t9, $a0, .L7F053538
/* 087F8C 7F05345C A56D0000 */   sh    $t5, ($t3)
/* 087F90 7F053460 84EE0004 */  lh    $t6, 4($a3)
/* 087F94 7F053464 84CF0004 */  lh    $t7, 4($a2)
/* 087F98 7F053468 55CF0033 */  bnel  $t6, $t7, .L7F053538
/* 087F9C 7F05346C A56D0000 */   sh    $t5, ($t3)
/* 087FA0 7F053470 84E40000 */  lh    $a0, ($a3)
/* 087FA4 7F053474 50850030 */  beql  $a0, $a1, .L7F053538
/* 087FA8 7F053478 A56D0000 */   sh    $t5, ($t3)
/* 087FAC 7F05347C 84C20008 */  lh    $v0, 8($a2)
/* 087FB0 7F053480 84F90008 */  lh    $t9, 8($a3)
/* 087FB4 7F053484 01A5C023 */  subu  $t8, $t5, $a1
/* 087FB8 7F053488 03227023 */  subu  $t6, $t9, $v0
/* 087FBC 7F05348C 030E0019 */  multu $t8, $t6
/* 087FC0 7F053490 0085C823 */  subu  $t9, $a0, $a1
/* 087FC4 7F053494 00007812 */  mflo  $t7
/* 087FC8 7F053498 00000000 */  nop
/* 087FCC 7F05349C 00000000 */  nop
/* 087FD0 7F0534A0 01F9001A */  div   $zero, $t7, $t9
/* 087FD4 7F0534A4 0000C012 */  mflo  $t8
/* 087FD8 7F0534A8 03027021 */  addu  $t6, $t8, $v0
/* 087FDC 7F0534AC A56E0008 */  sh    $t6, 8($t3)
/* 087FE0 7F0534B0 17200002 */  bnez  $t9, .L7F0534BC
/* 087FE4 7F0534B4 00000000 */   nop
/* 087FE8 7F0534B8 0007000D */  break 7
.L7F0534BC:
/* 087FEC 7F0534BC 2401FFFF */  li    $at, -1
/* 087FF0 7F0534C0 17210004 */  bne   $t9, $at, .L7F0534D4
/* 087FF4 7F0534C4 3C018000 */   lui   $at, 0x8000
/* 087FF8 7F0534C8 15E10002 */  bne   $t7, $at, .L7F0534D4
/* 087FFC 7F0534CC 00000000 */   nop
/* 088000 7F0534D0 0006000D */  break 6
.L7F0534D4:
/* 088004 7F0534D4 84F9000A */  lh    $t9, 0xa($a3)
/* 088008 7F0534D8 84C3000A */  lh    $v1, 0xa($a2)
/* 08800C 7F0534DC 84C50000 */  lh    $a1, ($a2)
/* 088010 7F0534E0 0323C023 */  subu  $t8, $t9, $v1
/* 088014 7F0534E4 01A57823 */  subu  $t7, $t5, $a1
/* 088018 7F0534E8 01F80019 */  multu $t7, $t8
/* 08801C 7F0534EC 84F90000 */  lh    $t9, ($a3)
/* 088020 7F0534F0 03257823 */  subu  $t7, $t9, $a1
/* 088024 7F0534F4 00007012 */  mflo  $t6
/* 088028 7F0534F8 00000000 */  nop
/* 08802C 7F0534FC 00000000 */  nop
/* 088030 7F053500 01CF001A */  div   $zero, $t6, $t7
/* 088034 7F053504 0000C012 */  mflo  $t8
/* 088038 7F053508 0303C821 */  addu  $t9, $t8, $v1
/* 08803C 7F05350C A579000A */  sh    $t9, 0xa($t3)
/* 088040 7F053510 15E00002 */  bnez  $t7, .L7F05351C
/* 088044 7F053514 00000000 */   nop
/* 088048 7F053518 0007000D */  break 7
.L7F05351C:
/* 08804C 7F05351C 2401FFFF */  li    $at, -1
/* 088050 7F053520 15E10004 */  bne   $t7, $at, .L7F053534
/* 088054 7F053524 3C018000 */   lui   $at, 0x8000
/* 088058 7F053528 15C10002 */  bne   $t6, $at, .L7F053534
/* 08805C 7F05352C 00000000 */   nop
/* 088060 7F053530 0006000D */  break 6
.L7F053534:
/* 088064 7F053534 A56D0000 */  sh    $t5, ($t3)
.L7F053538:
/* 088068 7F053538 1654FE52 */  bne   $s2, $s4, .L7F052E84
/* 08806C 7F05353C 02401825 */   move  $v1, $s2
/* 088070 7F053540 86A2000C */  lh    $v0, 0xc($s5)
/* 088074 7F053544 27DE0004 */  addiu $fp, $fp, 4
/* 088078 7F053548 04410003 */  bgez  $v0, .L7F053558
/* 08807C 7F05354C 00027083 */   sra   $t6, $v0, 2
/* 088080 7F053550 24410003 */  addiu $at, $v0, 3
/* 088084 7F053554 00017083 */  sra   $t6, $at, 2
.L7F053558:
/* 088088 7F053558 000E1880 */  sll   $v1, $t6, 2
/* 08808C 7F05355C 03C3082A */  slt   $at, $fp, $v1
/* 088090 7F053560 5420FE47 */  bnezl $at, .L7F052E80
/* 088094 7F053564 00001825 */   move  $v1, $zero
.L7F053568:
/* 088098 7F053568 8FBF003C */  lw    $ra, 0x3c($sp)
.L7F05356C:
/* 08809C 7F05356C 8FB00018 */  lw    $s0, 0x18($sp)
/* 0880A0 7F053570 8FB1001C */  lw    $s1, 0x1c($sp)
/* 0880A4 7F053574 8FB20020 */  lw    $s2, 0x20($sp)
/* 0880A8 7F053578 8FB30024 */  lw    $s3, 0x24($sp)
/* 0880AC 7F05357C 8FB40028 */  lw    $s4, 0x28($sp)
/* 0880B0 7F053580 8FB5002C */  lw    $s5, 0x2c($sp)
/* 0880B4 7F053584 8FB60030 */  lw    $s6, 0x30($sp)
/* 0880B8 7F053588 8FB70034 */  lw    $s7, 0x34($sp)
/* 0880BC 7F05358C 8FBE0038 */  lw    $fp, 0x38($sp)
/* 0880C0 7F053590 03E00008 */  jr    $ra
/* 0880C4 7F053594 27BD0058 */   addiu $sp, $sp, 0x58
)
#endif


/**
 * objToggleDoorPortal / doorActivatePortal
 * Toggles (Open/Closed) the portal linked with door
 * @param door: Door to toggle portal on
 */
void doorActivatePortal(DoorRecord *door)
{
    if (door->portalNumber >= 0)
    {
        bgToggleDataPortalsContrlBytes1Bit1(door->portalNumber, TRUE);
    }
}


/**
 * objToggleDoorPortal / doorDeactivatePortal
 * Toggles (Open/Closed) the portal linked with door
 * @param door: Door to toggle portal on
 */
void doorDeactivatePortal(DoorRecord *door) {
    if (door->portalNumber >= 0)
    {
        bgToggleDataPortalsContrlBytes1Bit1(door->portalNumber, FALSE);
    }
}


PropRecord* doorInit(DoorRecord* door, coord3d* pos, Mtxf* mtx, StandTile* stan, coord3d* coord, coord3d* centre) {
    PropRecord* prop;
    f32 scale;

    prop = objInitWithAutoModel((ObjectRecord* ) door);
    scale = PitemZ_entries[door->obj].scale;
    door->ptr_allocated_collisiondata_block = mempAllocBytesInBank(0x50U, MEMPOOL_STAGE);

    matrix_4x4_copy(mtx, &door->mtx);
    matrix_scalar_multiply(scale, door->mtx.m[0]);

    door->frac  = (f32) coord->x;
    door->unkac = (f32) coord->y;
    door->unkb0 = (f32) coord->z;

    if (door->flags & PROPFLAG_80000000) {
        door->openPosition = door->maxFrac;
    } else {
        door->openPosition = 0.0f;
    }

    door->speed = 0.0f;
    door->openstate = 0;
    door->unkbd = 0;
    door->linkedDoor = NULL;

    if (door->doorFlags & 4) {
        union ModelRoData *rodata = door->model->obj->RootNode->Child->Child->Data;
        door->unkcc = mempAllocBytesInBank(rodata->DisplayListCollisions.numVertices * sizeof(Vertex), MEMPOOL_STAGE);
    } else {
        door->unkcc = NULL;
    }

    door->portalNumber = -1;
    door->openSoundState = 0;
    door->closeSoundState = 0;

    prop->type = PROP_TYPE_DOOR;
    prop->door = door;
    prop->pos.x = pos->x;
    prop->pos.y = pos->y;
    prop->pos.z = pos->z;
    prop->stan = stan;

    door->runtime_pos.x = centre->x;
    door->runtime_pos.y = centre->y;
    door->runtime_pos.z = centre->z;
    door->flags |= PROPFLAG_00000100;

    door7F052B00(door);
    sub_GAME_7F052D8C(door);
    sub_GAME_7F0402B4(door->prop, &door->nextcol);

    door->shadecol.r = door->nextcol.r;
    door->shadecol.g = door->nextcol.g;
    door->shadecol.b = door->nextcol.b;
    door->shadecol.a = door->nextcol.a;

    return prop;
}


s32 sub_GAME_7F0537B8(f32 distance, f32 min, f32 max)
{
    s32 retval;

    if (distance <= 200.0f)
    {
        retval = SHRT_MAX;
    }
    else if (max <= distance)
    {
        retval = 0.0f;
    }
    else if (min <= distance)
    {
        retval = ((max - distance) * 10000.0f) / (max - min);
    }
    else
    {
        retval = SHRT_MAX - (s32)((sqrtf(distance - 200.0f) * 22767.0f) / sqrtf(min - 200.0f));
    }

    return retval;
}


s32 sub_GAME_7F053894(coord3d *pos, f32 low, f32 high)
{
    PropRecord *prop;
    s32 index;
    f32 shortest_distance;
    f32 diffx;
    f32 diffy;
    f32 diffz;
    f32 distance;
    s32 count;

    shortest_distance = high;
    count = getPlayerCount();

    for (index = 0; index < count; index++)
    {
        prop  = g_playerPointers[index]->prop;
        diffx = prop->pos.x - pos->x;
        diffy = prop->pos.y - pos->y;
        diffz = prop->pos.z - pos->z;
        distance = sqrtf(diffx * diffx + diffy * diffy + diffz * diffz);

        if (distance < shortest_distance)
        {
            shortest_distance = distance;
        }
    }
    return sub_GAME_7F0537B8(shortest_distance, low, high);
}


void chrobjSndCreatePostEvent(ALSoundState *state, coord3d *pos, f32 low, f32 high)
{
    sndCreatePostEvent(state, 8, sub_GAME_7F053894(pos, low, high));
}


s32 sub_GAME_7F0539B8(f32 vol)
{
    return sub_GAME_7F0537B8(vol, 5000.0f, 6000.0f);
}


s32 sub_GAME_7F0539E4(coord3d *pos)
{
    return sub_GAME_7F053894(pos, 5000.0f, 6000.0f);
}


void chrobjSndCreatePostEventDefault(ALSoundState *state, coord3d *pos)
{
    chrobjSndCreatePostEvent(state, pos, 5000.0f,  6000.0f);
}


void sub_GAME_7F053A3C(DoorRecord* arg0)
{
    s32 open_playing;
    s32 close_playing;
    s32 sp1C;

    open_playing = (arg0->openSoundState != NULL) && (sndGetPlayingState(arg0->openSoundState) != 0);
    close_playing = (arg0->closeSoundState != NULL) && (sndGetPlayingState(arg0->closeSoundState) != 0);

    if ((open_playing != 0) || (close_playing != 0))
    {

        sp1C = sub_GAME_7F0539E4(&arg0->prop->pos);

        if (lvlGetControlsLockedFlag() != 0)
        {
            sp1C = 0;
        }

        if (open_playing != 0)
        {
            #ifdef DEBUG
            assert( po->audiostate!=NULL);
            #endif
            sndCreatePostEvent(arg0->openSoundState, 8, sp1C);
        }

        if (close_playing != 0)
        {
            #ifdef DEBUG
            assert( po->audiostate2!=NULL);
            #endif
            sndCreatePostEvent(arg0->closeSoundState, 8, sp1C);
        }
    }
}


void door7F053B10(DoorRecord *door) //#MATCH
{
    if (door->openSoundState && sndGetPlayingState(door->openSoundState))
    {
        sndDeactivate(door->openSoundState);
    }

    if (door->closeSoundState && sndGetPlayingState(door->closeSoundState))
    {
        sndDeactivate(door->closeSoundState);
    }
}




void doorPlayOpenSound0(DoorRecord *door) {
    ALSoundState *soundState = NULL;
    ALSoundState *pendingState = NULL;

    door7F053B10(door);

    if (door->openSoundState == NULL)
    {
        pendingState = &door->openSoundState;
    }
    else if (door->closeSoundState == NULL)
    {
        pendingState = &door->closeSoundState;
    }

    switch (door->doorOpenSound)
    {
    case DOOR_OPEN_SOUND_01:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_SMART_CATCH1_SFX, NULL);
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, DOOR_SMART_SLIDE1_SFX, pendingState);
        }
        break;
    case DOOR_OPEN_SOUND_02:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_SMART_CATCH1_SFX, NULL);
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, TRAIN_SLIDE_DOOR_SLIDE_SFX, pendingState);
        }
        break;
    case DOOR_OPEN_SOUND_METAL:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, METAL_SLIDE_OPEN_SFX, NULL);
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, METAL_SLIDE_LOOP_SFX, pendingState);
        }
        break;
    case DOOR_OPEN_SOUND_04:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, HEAVY_SLIDE_OPEN_SFX, NULL);
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, HEAVY_SINGLE_LOOP_SFX, pendingState);
        }
        break;
    case DOOR_OPEN_SOUND_WOOD:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_WOOD_OPEN_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_06:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, TRAIN_SLIDE_DOOR_SLIDE_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_WOOD_2:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, TRAIN_SLIDE_DOOR_CATCH_SFX, NULL);
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, DOOR_WOOD_SLIDE_SFX, pendingState);
        }
        break;
    case DOOR_OPEN_SOUND_WOOD_3:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_WOOD_OPEN_SFX, NULL);
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, TRAIN_SLIDE_DOOR_SLIDE_SFX, pendingState);
        }
        break;
    case DOOR_OPEN_SOUND_09:
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, DOOR_SHUTTER_OPEN_SFX, pendingState);
        }
        break;
    case DOOR_OPEN_SOUND_METAL_2:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_METAL_OPEN_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_11:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, TRAIN_SLIDE_DOOR_SLIDE_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_METAL_3:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_METAL_OPEN3_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_13:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, TRAIN_SLIDE_DOOR_SLIDE_SFX, NULL);
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, TRAIN_SLIDE_DOOR_SLIDE_SFX, pendingState);
        }
        break;
    case DOOR_OPEN_SOUND_HYDROLIC:
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, DOOR_HYDRAL_CLOSE_SFX, pendingState);
        }
        break;
    case DOOR_OPEN_SOUND_STONE:
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, DOOR_SLIDE_STONE_OPEN_SFX, pendingState);
        }
        break;
    case DOOR_OPEN_SOUND_16:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, HEAVY_SLIDE_OPEN_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_METAL_4:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, TRAIN_SLIDE_DOOR_SLIDE_SFX, NULL);
        if (soundState != NULL)
        {
            chrobjSndCreatePostEventDefault(soundState, &door->prop->pos);
        }
        soundState = sndPlaySfx(g_musicSfxBufferPtr, METAL_SLIDE_OPEN_SFX, NULL);
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, METAL_SLIDE_LOOP_SFX, pendingState);
        }
        break;
    }

    if (soundState != NULL)
    {
        chrobjSndCreatePostEventDefault(soundState, &door->prop->pos);
    }

    sub_GAME_7F053A3C(door);
}






void doorPlayOpenSound1(DoorRecord *door) {
    ALSoundState *soundState = NULL;
    ALSoundState *pendingState = NULL;

    door7F053B10(door);

    if (door->openSoundState == NULL)
    {
        pendingState = &door->openSoundState;
    }
    else if (door->closeSoundState == NULL)
    {
        pendingState = &door->closeSoundState;
    }

    switch (door->doorOpenSound)
    {
    case DOOR_OPEN_SOUND_01:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_SMART_CATCH1_SFX, NULL);
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, DOOR_SMART_SLIDE1_SFX, pendingState);
        }
        break;
    case DOOR_OPEN_SOUND_02:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_SMART_CATCH1_SFX, NULL);
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, TRAIN_SLIDE_DOOR_SLIDE_SFX, pendingState);
        }
        break;
    case DOOR_OPEN_SOUND_METAL:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, METAL_SLIDE_OPEN_SFX, NULL);
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, METAL_SLIDE_LOOP_SFX, pendingState);
        }
        break;
    case DOOR_OPEN_SOUND_04:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, HEAVY_SLIDE_OPEN_SFX, NULL);
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, HEAVY_SINGLE_LOOP_SFX, pendingState);
        }
        break;
    case DOOR_OPEN_SOUND_WOOD_2:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, TRAIN_SLIDE_DOOR_CATCH_SFX, NULL);
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, DOOR_WOOD_SLIDE_SFX, pendingState);
        }
        break;
    case DOOR_OPEN_SOUND_WOOD_3:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_WOOD_OPEN_SFX, NULL);
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, TRAIN_SLIDE_DOOR_SLIDE_SFX, pendingState);
        }
        break;
    case DOOR_OPEN_SOUND_09:
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, DOOR_SHUTTER_OPEN_SFX, pendingState);
        }
        break;
    case DOOR_OPEN_SOUND_13:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, TRAIN_SLIDE_DOOR_SLIDE_SFX, NULL);
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, TRAIN_SLIDE_DOOR_SLIDE_SFX, pendingState);
        }
        break;
    case DOOR_OPEN_SOUND_HYDROLIC:
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, DOOR_HYDRAL_CLOSE_SFX, pendingState);
        }
        break;
    case DOOR_OPEN_SOUND_STONE:
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, DOOR_SLIDE_STONE_OPEN_SFX, pendingState);
        }
        break;
    case DOOR_OPEN_SOUND_16:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, HEAVY_SLIDE_OPEN_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_METAL_4:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, TRAIN_SLIDE_DOOR_SLIDE_SFX, NULL);
        if (soundState != NULL)
        {
            chrobjSndCreatePostEventDefault(soundState, &door->prop->pos);
        }
        soundState = sndPlaySfx(g_musicSfxBufferPtr, METAL_SLIDE_OPEN_SFX, NULL);
        if (pendingState != NULL)
        {
            sndPlaySfx(g_musicSfxBufferPtr, METAL_SLIDE_LOOP_SFX, pendingState);
        }
        break;
    }

    if (soundState != NULL)
    {
        chrobjSndCreatePostEventDefault(soundState, &door->prop->pos);
    }

    sub_GAME_7F053A3C(door);
}





void doorPlayCloseSound0(DoorRecord *door) {
    ALSoundState *soundState = NULL;

    door7F053B10(door);

    switch (door->doorOpenSound)
    {
    case DOOR_OPEN_SOUND_01:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_SMART_CATCH1_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_02:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_SMART_CATCH1_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_METAL:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, METAL_SLIDE_CLOSE_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_04:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, HEAVY_SLIDE_CLOSE_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_WOOD_2:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_SMART_CATCH1_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_WOOD_3:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_WOOD_CLOSE_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_09:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_SHUTTER_CLOSE_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_13:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, TRAIN_SLIDE_DOOR_SLIDE_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_HYDROLIC:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_HYDRAL_OPEN_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_STONE:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_SLIDE_STONE_CLOSE_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_16:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, HEAVY_SLIDE_CLOSE_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_METAL_4:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, METAL_SLIDE_CLOSE_SFX, NULL);
        break;
    }

    if (door); // Fix for recomp not matching

    if (soundState != NULL)
    {
        chrobjSndCreatePostEventDefault(soundState, &door->prop->pos);
    }

    sub_GAME_7F053A3C(door);
}





void doorPlayCloseSound1(DoorRecord *door)
{
    ALSoundState *soundState = NULL;

    door7F053B10(door);

    switch (door->doorOpenSound)
    {
    case DOOR_OPEN_SOUND_01:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_SMART_CATCH1_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_02:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_SMART_CATCH1_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_METAL:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, METAL_SLIDE_CLOSE_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_04:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, HEAVY_SLIDE_CLOSE_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_WOOD:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_WOOD_CLOSE_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_06:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, TRAIN_SLIDE_DOOR_SLIDE_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_WOOD_2:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_SMART_CATCH1_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_WOOD_3:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_WOOD_CLOSE_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_09:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_SHUTTER_CLOSE_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_METAL_2:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_METAL_CLOSE_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_11:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_METAL_CLOSE2_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_METAL_3:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_METAL_CLOSE3_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_13:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, TRAIN_SLIDE_DOOR_SLIDE_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_HYDROLIC:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_HYDRAL_OPEN_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_STONE:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, DOOR_SLIDE_STONE_CLOSE_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_16:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, HEAVY_SLIDE_CLOSE_SFX, NULL);
        break;
    case DOOR_OPEN_SOUND_METAL_4:
        soundState = sndPlaySfx(g_musicSfxBufferPtr, METAL_SLIDE_CLOSE_SFX, NULL);
        break;
    }

    if (door); // Fix for recomp not matching

    if (soundState != NULL) {
        chrobjSndCreatePostEventDefault(soundState, &door->prop->pos);
    }

    sub_GAME_7F053A3C(door);
}


/**
 * Play the door open sound and activate the door's portal,
 */
void doorStartOpen(DoorRecord *door)
{
    door->flags &= ~DOORFLAG_KEEPOPEN;
    door->runtime_bitflags |= RUNTIMEBITFLAG_BEENOPENED;

    doorPlayOpenSound0(door);
    doorActivatePortal(door);

    if (door->doorType == 8)
    {
        struct collision_data *col = door->ptr_allocated_collisiondata_block;
        door->flags |= DOORFLAG_CANNOT_ACTIVATE;
        door->perimFrac = 0;

        if (col) { col->unk00 = 0; }
        door->flags &= ~DOORFLAG_100;
    }
}


/**
 * Play the door close sound
 */
void doorStartClose(DoorRecord *door)
{
    door->flags &= ~DOORFLAG_KEEPOPEN;
    doorPlayOpenSound1(door);
}


void doorFinishOpen(DoorRecord *door)
{
    doorPlayCloseSound0(door);

    if (door->doorType == 8)
    {
        sub_GAME_7F03FDA8(door->prop);

        if (door->runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT)
        {
            door->projectile->flags |= 1;
            matrix_4x4_set_identity(&door->projectile->mtx);
        }
    }
}


void doorFinishClose(DoorRecord* door)
{
    doorPlayCloseSound1(door);
    doorDeactivatePortal(door);
}


/**
 * Apply the given state to an individual door (not its siblings).
 *
 * Handles playing door open/close sounds and activating the portal if opening.
 */
void doorSetOpenState(DoorRecord *door, s32 newstate)
{
    if (newstate == DOORSTATE_OPENING)
    {
        if (door->openstate == DOORSTATE_STATIONARY || door->openstate == DOORSTATE_WAITING)
        {
            doorStartOpen(door);
        }

        door->openstate = newstate;
    }
    else if (newstate == DOORSTATE_CLOSING)
    {
        if (door->openstate == DOORSTATE_STATIONARY && door->openPosition > 0)
        {
            doorStartClose(door);
        }

        if ((door->openstate != DOORSTATE_STATIONARY && door->openstate != DOORSTATE_WAITING) || door->openPosition > 0)
        {
            door->openstate = newstate;
        }
        else if (door->openstate == DOORSTATE_WAITING)
        {
            door->openstate = DOORSTATE_STATIONARY;
        }
    }
    else
    {
        door->openstate = newstate;
    }
}


void doorActivate(DoorRecord *door, DOORSTATE State) //#MATCH
{
    DoorRecord *linkeddoor;
    DOORSTATE   LinkedState = State;

    if (door->flags2 & 0x40000000) //Close first door before opening second
    {
        if (State == DOORSTATE_OPENING)
        {
            LinkedState = DOORSTATE_CLOSING;
            if (door->openstate == DOORSTATE_STATIONARY)
            {
                State = DOORSTATE_WAITING;
            }
        }
    }

    doorSetOpenState(door, State);

    linkeddoor = door->linkedDoor;

    while (linkeddoor && linkeddoor != door)
    {
        doorSetOpenState(linkeddoor, LinkedState);
        linkeddoor = linkeddoor->linkedDoor;
    };
}


bool doorIsClosed(DoorRecord *door)
{
    return ((door->openstate == DOORSTATE_STATIONARY) || (door->openstate == DOORSTATE_WAITING)) && (door->openPosition <= 0.0f);
}


s32 sub_GAME_7F054A64(PropRecord *prop, bbox2d *bbox)
{
    s32 room_ids[8];
    s32 *rooms;
    bool result = FALSE;
    s32 room_id;
    bbox2d bbox2;

    chraiGetPropRoomIds(prop, room_ids);
    rooms = room_ids;
    room_id = *rooms;

    while (room_id >= 0)
    {
        if (bgGet2dBboxByRoomId(room_id, &bbox2))
        {
            if (result)
            {
                if (bbox->min.x > bbox2.min.x)
                {
                    bbox->min.x = bbox2.min.x;
                }
                if (bbox->min.y > bbox2.min.y)
                {
                    bbox->min.y = bbox2.min.y;
                }
                if (bbox->max.x < bbox2.max.x)
                {
                    bbox->max.x = bbox2.max.x;
                }
                if (bbox->max.y < bbox2.max.y)
                {
                    bbox->max.y = bbox2.max.y;
                }
            }
            else
            {
                bbox->min.x = bbox2.min.x;
                bbox->min.y = bbox2.min.y;
                bbox->max.x = bbox2.max.x;
                bbox->max.y = bbox2.max.y;
            }
            result = TRUE;
        }
        rooms++;
        room_id = *rooms;
    }

    return result;
}


/**
 * Address 0x7F054B80.
*/
f32 chrobjFogVisRangeRelated(PropRecord *prop, f32 size)
{
    f32 ret;
#if defined(LEFTOVERDEBUG)
    struct NearFogRecord *nfd;
#else
    struct NearFogRecordF *nfd;
#endif
    f32 temp_f12;

    ret = 1.0f;
    nfd = fogGetNearFogValuesP();

    if ((nfd != NULL) && (nfd->MaxObfuscationRange < prop->zDepth))
    {
        temp_f12 = getPlayer_c_lodscalez();
        temp_f12 = ((((prop->zDepth - nfd->MaxObfuscationRange) * 100.0f) / size) + nfd->MaxObfuscationRange) * temp_f12;

        if (nfd->MaxVisRange <= temp_f12)
        {
            ret = 0.0f; //im invisible
        }
        else
        {
            if (nfd->NearFog < temp_f12)
            {
                ret = (nfd->MaxVisRange - temp_f12) / (nfd->MaxVisRange - nfd->NearFog);// power of fog (0 - 1 ) where 0 is full fog, and 1 is no fog
            }
        }
    }

    return ret;
}


bool sub_GAME_7F054C58(coord3d *coord, f32 arg1)
{
    bool result = TRUE;
    coord3d *ptr = (coord3d*)fogGetNearFogValuesP();
    coord3d tmp;
    f32 sp20;

    if (ptr != NULL)
    {
        coord3d *campos = bondviewGetCurrentPlayersPosition();
        Mtxf *mtx = camGetWorldToScreenMtxf();

        tmp.x = coord->x - campos->x;
        tmp.y = coord->y - campos->y;
        tmp.z = coord->z - campos->z;

        sp20 = tmp.f[0] * mtx->m[0][0] + tmp.f[1] * mtx->m[0][1] + tmp.f[2] * mtx->m[0][2];

        if (sp20 > ptr->z)
        {
            f32 scalez = getPlayer_c_lodscalez();
            sp20 = ((sp20 - ptr->z) * 100 / arg1 + ptr->z) * scalez;

            if (sp20 >= ptr->y)
            {
                result = FALSE;
            }
        }
    }

    return result;
}

// PD: func0f08e8ac
bool sub_GAME_7F054D6C(PropRecord *prop, coord3d *pos, f32 arg2, bool arg3)
{
    s32 room_ids[8];
    s32 *rooms;
    s32 roomnum;
    bool result;
    bbox2d bbox;

    result = FALSE;
    chraiGetPropRoomIds(prop, room_ids);
    rooms = room_ids;
    roomnum = *rooms;

    while (roomnum >= 0)
    {
        if (getROOMID_isRendered(roomnum) != 0)
        {
            if (fogPositionIsVisibleThroughFog(pos, arg2) && (!arg3 || sub_GAME_7F054C58(pos, arg2)))
            {
                if (sub_GAME_7F054A64(prop, &bbox) != 0)
                {
                    result = camIsPosInScreenBox(pos, arg2, &bbox);
                }
                else
                {
                    result = camIsPosInScreen(pos, arg2);
                }

                if (result)
                {
                    coord3d *campos = bondviewGetCurrentPlayersPosition();
                    f32 xdiff = pos->x - campos->x;
                    f32 ydiff = pos->y - campos->y;
                    f32 zdiff = pos->z - campos->z;

                    if (xdiff * xdiff + ydiff * ydiff + zdiff * zdiff > 32000 * 32000)
                    {
                        result = FALSE;
                    }
                }
            }

            break;
        }

        rooms++;
        roomnum = *rooms;
        result = FALSE;
    }

    return result;
}


/**
* Loaded to 7F054EA8.
*/
s32 updateDoorDisplacement(DoorRecord* door)
{
    int isMoving = 0;

    if (door->openstate == DOORSTATE_OPENING)
    {
        chrobjApplySpeed(&door->openPosition, door->maxFrac, &door->speed, door->accel, door->decel, door->maxSpeed);

        if (door->maxFrac <= door->openPosition)
        {
            door->openPosition = door->maxFrac;
        }
        else
        {
            if (door->openPosition <= 0.0f)
            {
                door->openPosition = 0.0f;
            }
        }

        isMoving = 1;
    }
    else if (door->openstate == DOORSTATE_CLOSING)
    {
        chrobjApplySpeed(&door->openPosition, 0.0f, &door->speed, door->accel, door->decel, door->maxSpeed);

        if (door->maxFrac <= door->openPosition)
        {
            door->openPosition = door->maxFrac;
        }
        else
        {
            if (door->openPosition <= 0.0f)
            {
                door->openPosition = 0.0f;
            }
        }

        isMoving = 1;
    }

    return isMoving;
}



/**
 * NTSC address 0x7F054FB4.
*/
void door7F054FB4(DoorRecord *door)
{
    Model *temp_a0;
    ModelNode *temp_a1;
    s32 var_s4;
    DoorRecord *var_s1;
    s32 var_s5;
    s32 var_a0;

    struct ModelRoData_DisplayList_CollisionRecord *temp_s0;
    struct ModelRwData_DisplayList_CollisionRecord *temp_v0_3;

    var_s4 = 0;
    var_s5 = 1;

    var_s1 = door;
    while (var_s1 != NULL)
    {
        var_s1->lastcalc60f = var_s1->openPosition;
        if (updateDoorDisplacement(var_s1) != 0)
        {
            var_s4 = 1;
        }

        var_s1 = var_s1->linkedDoor;

        if (var_s1 == door)
        {
            break;
        }
    }

    var_s1 = door;
    if ((var_s4 != 0))
    {
        while (var_s1 != NULL)
        {
            door7F052B00(var_s1);
            var_s5 = sub_GAME_7F0448A8(var_s1->prop);

            if (var_s5 == 0)
            {
                break;
            }

            var_s1 = var_s1->linkedDoor;

            if (var_s1 == door)
            {
                break;
            }
        }
    }

    var_s1 = door;
    while (var_s1 != NULL)
    {
        if (var_s4)
        {
            if (var_s5 != 0)
            {
                if (var_s1->openstate == DOORMODE_OPENING)
                {
                    if (var_s1->maxFrac <= var_s1->openPosition)
                    {
                        var_s1->openstate = 0;
                        var_s1->speed = 0.0f;
                        var_s1->openedTime = (u32) g_GlobalTimer;

                        doorFinishOpen(var_s1);
                    }
                }
                else if ((var_s1->openstate == DOORMODE_CLOSING) && (var_s1->openPosition <= 0.0f))
                {
                    var_s1->openstate = 0;
                    var_s1->speed = 0.0f;
                    var_s1->openedTime = 0;

                    doorFinishClose(var_s1);
                }

                sub_GAME_7F0402B4(var_s1->prop, &var_s1->nextcol);
            }
            else
            {
                var_s1->speed = 0.0f;
                var_s1->openPosition = var_s1->lastcalc60f;

                door7F052B00(var_s1);
            }

            sub_GAME_7F052D8C(var_s1);
        }
        else if  (var_s1->doorFlags & DOORFLAG_0004)
        {
            temp_a0 = var_s1->model;
            temp_a1 = temp_a0->obj->RootNode->Child->Child;
            temp_s0 = (struct ModelRoData_DisplayList_CollisionRecord *)temp_a1->Data;
            temp_v0_3 = (struct ModelRwData_DisplayList_CollisionRecord*)modelGetNodeRwData(temp_a0, temp_a1);

            if (temp_v0_3->Vertices != var_s1->unkcc)
            {
                for (var_a0 = 0; var_a0 < temp_s0->numVertices; var_a0++)
                {
                    // struct copy
                    var_s1->unkcc[var_a0] = temp_v0_3->Vertices[var_a0];
                }
            }

            temp_v0_3->Vertices = var_s1->unkcc;
        }

        var_s1->lastcalc60i = g_GlobalTimer;

        var_s1 = var_s1->linkedDoor;

        if (var_s1 == door)
        {
            break;
        }
    }
}


// PD: door0f08f604
void door7F05522C(DoorRecord *door, f32 *arg1, f32 *arg2, s32 altcoordsystem)
{
    f32 anglediff;
    PropRecord *playerprop;
    BoundPadRecord *pad;
    coord3d field_10;
    coord3d normal;
    f32 xmin;
    f32 xmax;
    coord3d playerpos;
    f32 angle2;
    f32 cosine;
    f32 sine;
    f32 angle;
    f32 y1;
    f32 x1;
    f32 playerangle;
    f32 anglediff2;
    f32 scale;
    f32 xbound;

    pad = &g_CurrentSetup.boundpads[door->pad];
    playerprop = get_curplayer_positiondata();

    if (1) { scale = 1.0f; }
    playerpos.f[0] = (((g_CurrentPlayer->field_488.theta_transform.x * 30.0f) * scale) * 0.75f) + playerprop->pos.x;
    playerpos.f[1] = playerprop->pos.y;
    playerpos.f[2] = (((g_CurrentPlayer->field_488.theta_transform.z * 30.0f) * scale) * 0.75f) + playerprop->pos.z;

    if (altcoordsystem != 0)
    {
        xmin = pad->bbox.xmin;
        xmax = pad->bbox.xmax;
        normal.f[0] = (pad->up.y * pad->look.z) - (pad->look.y * pad->up.z);
        normal.f[1] = (pad->up.z * pad->look.x) - (pad->look.z * pad->up.x);
        normal.f[2] = (pad->up.x * pad->look.y) - (pad->look.x * pad->up.y);
    }
    else
    {
        xmin = pad->bbox.ymin;
        xmax = pad->bbox.ymax;
        normal.f[0] = pad->up.x;
        normal.f[1] = pad->up.y;
        normal.f[2] = pad->up.z;
    }

    x1 = (pad->pos.x + (normal.x * xmin)) - playerpos.x;
    y1 = (pad->pos.z + (normal.z * xmin)) - playerpos.z;
    angle = atan2f(x1, y1);

    playerangle = get_curplay_horizontal_rotation_in_degrees();
    anglediff = angle - playerangle;

    scale = (angle - playerangle) + M_TAU_F;
    if (angle < playerangle)
    {
        anglediff = scale;
    }

    if (anglediff > M_PI_F)
    {
        anglediff = anglediff - M_TAU_F;
    }

    if (door->doorType == 5)
    {
        angle2 = (door->openPosition * M_TAU_F) / 360.0f;
        if (door->flags & 0x20000000)
        {
            angle2 = M_TAU_F - angle2;
        }

        cosine = cosf(angle2);
        sine = sinf(angle2);

        xbound = xmax - xmin;
        x1 = ((pad->pos.x + (normal.x * xmin)) + (xbound * ((normal.x * cosine) + (normal.z * sine)))) - playerpos.x;
        y1 = ((pad->pos.z + (normal.z * xmin)) + (xbound * (((-normal.x) * sine) + (normal.z * cosine)))) - playerpos.z;

        angle = atan2f(x1, y1);
        playerangle = get_curplay_horizontal_rotation_in_degrees();

        anglediff2 = angle - playerangle;
        if (angle < playerangle)
        {
            anglediff2 += M_TAU_F;
        }
        if (anglediff2 > M_PI_F)
        {
            anglediff2 -= M_TAU_F;
        }
    }
    else
    {
        x1 = (pad->pos.x + (normal.x * xmax)) - playerpos.x;
        y1 = (pad->pos.z + (normal.z * xmax)) - playerpos.z;

        angle = atan2f(x1, y1);
        playerangle = get_curplay_horizontal_rotation_in_degrees();

        anglediff2 = angle - playerangle;

        if (normal.x);
        if (xmax);

        if (angle < playerangle)
        {
            anglediff2 += M_TAU_F;
        }

        if (anglediff2 > M_PI_F)
        {
            anglediff2 -= M_TAU_F;
        }
    }

    if (anglediff < anglediff2)
    {
        *arg1 = anglediff;
        *arg2 = anglediff2;
    }
    else
    {
        *arg1 = anglediff2;
        *arg2 = anglediff;
    }
}


// PD: func0f08f968
bool door7F0555F8(DoorRecord *door, bool altcoordsystem)
{
    bool checkmore;
    f32 sp50;
    f32 sp4c;
    DoorRecord *sibling;
    f32 limit;
    f32 sp40;
    f32 sp3c;

    checkmore = TRUE;
    limit = 0.34906587f;

    if (g_InteractProp == NULL)
    {
        door7F05522C(door, &sp50, &sp4c, altcoordsystem);

        if ((sp50 >= -limit) && (sp50 <= limit) && (sp4c >= -limit) && (sp4c <= limit))
        {
            g_InteractProp = door->prop;
            checkmore = FALSE;
        }
        else
        {
            sibling = door->linkedDoor;

            while (sibling != NULL && sibling != door && (sp50 >= 0.0f || sp4c < 0.0f))
            {
                door7F05522C(sibling, &sp40, &sp3c, altcoordsystem);

                if ((sp50 > 0.0f) && (sp40 < sp50))
                {
                    sp50 = sp40;
                }

                if ((sp4c < 0.0f) && (sp4c < sp3c))
                {
                    sp4c = sp3c;
                }

                sibling = sibling->linkedDoor;
            }

            if ((sp4c - sp50) < M_PI_F && (sp50 < 0.0f) && (sp4c > 0.0f))
            {
                g_InteractProp = door->prop;
                checkmore = FALSE;
            }
        }
    }

    return checkmore;
}


bool doorTestForInteract(PropRecord *prop)
{
	bool checkmore;
	DoorRecord *door;
    bool maybe;
    PropRecord *playerprop;
    f32 xdiff;
    f32 ydiff;
    f32 zdiff;
    BoundPadRecord *boundpads;
    u8 rooms1[32];
    u8 rooms2[32];
    s32 unused[2];

    checkmore = TRUE;
    door = prop->door;

	if ((door->flags & PROPFLAG_CANNOT_ACTIVATE) == 0
			&& door->maxFrac > 0
			&& (prop->flags & PROPFLAG_ONSCREEN))
    {
		maybe = FALSE;
		playerprop = get_curplayer_positiondata();

		xdiff = door->runtime_pos.x - playerprop->pos.x;
		ydiff = door->runtime_pos.y - playerprop->pos.y;
		zdiff = door->runtime_pos.z - playerprop->pos.z;

		if (xdiff * xdiff + zdiff * zdiff < 40000.0f && ydiff < 200.0f && ydiff > -200.0f)
        {
			maybe = TRUE;
		}
        else
        {
            chraiGetPropRoomIds(prop, (s32*)rooms1);
            chraiGetPropRoomIds(playerprop, (s32*)rooms2);
            if (sub_GAME_7F03DB70((s32*)rooms1, (s32*)rooms2) != 0)
            {
                boundpads = &g_CurrentSetup.boundpads[door->pad];
                if (sub_GAME_7F03F598(&playerprop->pos, 150.0f, boundpads) != 0)
                {
                    maybe = TRUE;
                }
            }
		}

		if (maybe)
        {
            checkmore = door7F0555F8(door, FALSE);

            if (checkmore && (door->flags2 & PROPFLAG2_DOOR_ALTCOORDSYSTEM))
            {
                checkmore = door7F0555F8(door, TRUE);
            }
		}
	}

	return checkmore;
}


void doorActivateWrapper(PropRecord *prop) //#MATCH
{
    DoorRecord *door = prop->door;

    if ((door->openstate == DOORSTATE_OPENING) || (door->openstate == DOORSTATE_WAITING))
    {
        doorActivate(door, DOORSTATE_CLOSING);
    }
    else if (door->openstate == DOORSTATE_CLOSING)
    {
        doorActivate(door, DOORSTATE_OPENING);
    }
    else if (door->openstate == DOORSTATE_STATIONARY)
    {
        if (door->openPosition > 0.5f)
        {
            doorActivate(door, DOORSTATE_CLOSING);
        }
        else
        {
            doorActivate(door, DOORSTATE_OPENING);
        }
    }
    door->runtime_bitflags |= RUNTIMEBITFLAG_ACTIVATED;
    door->flags2 &= ~8;
    sub_GAME_7F03E6A0(prop);
}




#ifdef NONMATCHING
// https://decomp.me/scratch/B63wV 79.17%
s32 posIsInFrontOfDoor(PropRecord* prop, DoorRecord* door)
{
    BoundPadRecord* pad;
    f32 other;
    struct coord3d normal;
    f32 value = 0;

    pad = (BoundPadRecord*)&g_CurrentSetup.boundpads[door->pad];

    normal.f[0] = (pad->up.f[1] * pad->look.f[2]) - (pad->up.f[2] * pad->look.f[1]);
    normal.f[1] = (pad->up.f[2] * pad->look.f[0]) - (pad->up.f[0] * pad->look.f[2]);
    normal.f[2] = (pad->up.f[0] * pad->look.f[1]) - (pad->up.f[1] * pad->look.f[0]);

    value += (normal.f[0] * (prop->pos.f[0] - pad->pos.f[0]));
    value += (normal.f[1] * (prop->pos.f[1] - pad->pos.f[1]));
    value += (normal.f[2] * (prop->pos.f[2] - pad->pos.f[2]));

    other = value;

    if (door->doorFlags & 8)
    {
        other = -value;
    }

    if (other < 0)
    {
        return 0;
    }

    if (other > 0)
    {
        return 1;
    }


    return 1;
}

#else
s32 posIsInFrontOfDoor(PropRecord* prop, DoorRecord* door);
GLOBAL_ASM(
.text
glabel posIsInFrontOfDoor
/* 08A5A0 7F055A70 84AE0006 */  lh    $t6, 6($a1)
/* 08A5A4 7F055A74 3C188007 */  lui   $t8, %hi(g_CurrentSetup+0x1C)
/* 08A5A8 7F055A78 8F185D1C */  lw    $t8, %lo(g_CurrentSetup+0x1C)($t8)
/* 08A5AC 7F055A7C 000E7900 */  sll   $t7, $t6, 4
/* 08A5B0 7F055A80 01EE7821 */  addu  $t7, $t7, $t6
/* 08A5B4 7F055A84 000F7880 */  sll   $t7, $t7, 2
/* 08A5B8 7F055A88 01F81021 */  addu  $v0, $t7, $t8
/* 08A5BC 7F055A8C C4440010 */  lwc1  $f4, 0x10($v0)
/* 08A5C0 7F055A90 C4460020 */  lwc1  $f6, 0x20($v0)
/* 08A5C4 7F055A94 C44A001C */  lwc1  $f10, 0x1c($v0)
/* 08A5C8 7F055A98 C4520014 */  lwc1  $f18, 0x14($v0)
/* 08A5CC 7F055A9C 46062202 */  mul.s $f8, $f4, $f6
/* 08A5D0 7F055AA0 27BDFFE0 */  addiu $sp, $sp, -0x20
/* 08A5D4 7F055AA4 46125102 */  mul.s $f4, $f10, $f18
/* 08A5D8 7F055AA8 46044181 */  sub.s $f6, $f8, $f4
/* 08A5DC 7F055AAC E7A60004 */  swc1  $f6, 4($sp)
/* 08A5E0 7F055AB0 C4520018 */  lwc1  $f18, 0x18($v0)
/* 08A5E4 7F055AB4 C44A0014 */  lwc1  $f10, 0x14($v0)
/* 08A5E8 7F055AB8 C4440020 */  lwc1  $f4, 0x20($v0)
/* 08A5EC 7F055ABC 46125202 */  mul.s $f8, $f10, $f18
/* 08A5F0 7F055AC0 C44A000C */  lwc1  $f10, 0xc($v0)
/* 08A5F4 7F055AC4 460A2482 */  mul.s $f18, $f4, $f10
/* 08A5F8 7F055AC8 46124101 */  sub.s $f4, $f8, $f18
/* 08A5FC 7F055ACC E7A40008 */  swc1  $f4, 8($sp)
/* 08A600 7F055AD0 C448001C */  lwc1  $f8, 0x1c($v0)
/* 08A604 7F055AD4 C44A000C */  lwc1  $f10, 0xc($v0)
/* 08A608 7F055AD8 46085482 */  mul.s $f18, $f10, $f8
/* 08A60C 7F055ADC C4480010 */  lwc1  $f8, 0x10($v0)
/* 08A610 7F055AE0 C44A0018 */  lwc1  $f10, 0x18($v0)
/* 08A614 7F055AE4 46085282 */  mul.s $f10, $f10, $f8
/* 08A618 7F055AE8 460A9201 */  sub.s $f8, $f18, $f10
/* 08A61C 7F055AEC E7A8000C */  swc1  $f8, 0xc($sp)
/* 08A620 7F055AF0 C44A0000 */  lwc1  $f10, ($v0)
/* 08A624 7F055AF4 C4920008 */  lwc1  $f18, 8($a0)
/* 08A628 7F055AF8 94B90098 */  lhu   $t9, 0x98($a1)
/* 08A62C 7F055AFC 460A9001 */  sub.s $f0, $f18, $f10
/* 08A630 7F055B00 C44A0004 */  lwc1  $f10, 4($v0)
/* 08A634 7F055B04 C492000C */  lwc1  $f18, 0xc($a0)
/* 08A638 7F055B08 33280008 */  andi  $t0, $t9, 8
/* 08A63C 7F055B0C 460A9081 */  sub.s $f2, $f18, $f10
/* 08A640 7F055B10 C44A0008 */  lwc1  $f10, 8($v0)
/* 08A644 7F055B14 C4920010 */  lwc1  $f18, 0x10($a0)
/* 08A648 7F055B18 460A9301 */  sub.s $f12, $f18, $f10
/* 08A64C 7F055B1C 46060482 */  mul.s $f18, $f0, $f6
/* 08A650 7F055B20 44800000 */  mtc1  $zero, $f0
/* 08A654 7F055B24 46041282 */  mul.s $f10, $f2, $f4
/* 08A658 7F055B28 460A9180 */  add.s $f6, $f18, $f10
/* 08A65C 7F055B2C 460C4102 */  mul.s $f4, $f8, $f12
/* 08A660 7F055B30 46062400 */  add.s $f16, $f4, $f6
/* 08A664 7F055B34 11000002 */  beqz  $t0, .L7F055B40
/* 08A668 7F055B38 46008386 */   mov.s $f14, $f16
/* 08A66C 7F055B3C 46008387 */  neg.s $f14, $f16
.L7F055B40:
/* 08A670 7F055B40 4600703C */  c.lt.s $f14, $f0
/* 08A674 7F055B44 00000000 */  nop
/* 08A678 7F055B48 45020004 */  bc1fl .L7F055B5C
/* 08A67C 7F055B4C 460E003C */   c.lt.s $f0, $f14
/* 08A680 7F055B50 10000007 */  b     .L7F055B70
/* 08A684 7F055B54 00001025 */   move  $v0, $zero
/* 08A688 7F055B58 460E003C */  c.lt.s $f0, $f14
.L7F055B5C:
/* 08A68C 7F055B5C 24020001 */  li    $v0, 1
/* 08A690 7F055B60 45000003 */  bc1f  .L7F055B70
/* 08A694 7F055B64 00000000 */   nop
/* 08A698 7F055B68 10000001 */  b     .L7F055B70
/* 08A69C 7F055B6C 24020001 */   li    $v0, 1
.L7F055B70:
/* 08A6A0 7F055B70 03E00008 */  jr    $ra
/* 08A6A4 7F055B74 27BD0020 */   addiu $sp, $sp, 0x20
)
#endif


void doorsChooseSwingDirection(PropRecord *chrprop, DoorRecord *door)
{
    if ((door->flags & PROPFLAG_DOOR_TWOWAY) && door->openstate == PROPSTATE_NONE && door->openPosition == 0.0f)
    {
        bool infront = posIsInFrontOfDoor(chrprop, door);
        u32 wantflag = 0;

        if ((door->doorFlags & 8) == 0)
        {
            if (!infront)
            {
                wantflag = PROPFLAG_DOOR_OPENTOFRONT;
            }
        }
        else
        {
            if (infront)
            {
                wantflag = PROPFLAG_DOOR_OPENTOFRONT;
            }
        }

        // If flags are different
        if ((s32)((door->flags ^ wantflag) << 2) < 0)
        {
            // Toggle direction on door and siblings
            DoorRecord *sibling = door;

            do
            {
                sibling->flags ^= PROPFLAG_DOOR_OPENTOFRONT;
                sibling = sibling->linkedDoor;
            } while (sibling && sibling != door);
        }
    }
}


s32 propdoorInteract(PropRecord* doorprop)
{
    s32 unused;
    s32 sp28;
    PropRecord* playerprop;
    DoorRecord* door;
    textoverride* txt;

    door = doorprop->door;
    sp28 = 0;
    playerprop = get_curplayer_positiondata();

    if (door->keyflags == 0)
    {
        sp28 = 1;
    }
    else if (bondinvCheckHasKeyFlags(door->keyflags) != 0)
    {
        sp28 = 1;
    }
    else if (posIsInFrontOfDoor(playerprop, door) != 0)
    {
        if ((door->flags2 & PROPFLAG2_10000000) && !(door->flags2 & PROPFLAG2_08000000))
        {
            sp28 = 1;
        }
    }
    else if (!(door->flags2 & PROPFLAG2_10000000) && (door->flags2 & PROPFLAG2_08000000))
    {
        sp28 = 1;
    }

    if (doorIsPadlockFree(door) == 0)
    {
        sp28 = 0;
    }

    if (sp28 != 0)
    {
        doorsChooseSwingDirection(playerprop, door);
        doorActivateWrapper(doorprop);
    }
    else if ((door->openstate == DOORSTATE_STATIONARY) && (door->openPosition < 0.5f))
    {
        if (!(door->flags2 & PROPFLAG2_00000004))
        {
            txt = bondinvGetTextbyObj((ObjectRecord*)door);
            if ((txt != NULL) && (txt->pickuptext != 0))
            {
#ifdef VERSION_US
                hudmsgBottomShow(langGet((s32) txt->pickuptext));
#else
                jp_hudmsgBottomShow(langGet((s32) txt->pickuptext));
#endif
            }
            else
            {
#ifdef VERSION_US
                hudmsgBottomShow(langGet(0xA440));
#else
                jp_hudmsgBottomShow(langGet(0xA440));
#endif
            }
        }
        door->runtime_bitflags |= RUNTIMEBITFLAG_ACTIVATED;
        door->flags2 |= PROPFLAG2_00000008;
    }
    return 0;
}


void alarmActivate(void)
{
    if (alarm_timer < 1) {
        alarm_timer = 1;
    }
    return;
}

void deactivate_alarm_sound_effect(void)
{
    if ((ptr_alarm_sfx != 0) && (sndGetPlayingState(ptr_alarm_sfx) != AL_STOPPED)) {
        sndDeactivate(ptr_alarm_sfx);
    }
    return;
}

void alarmDeactivate(void)
{
  alarm_timer = 0;
  deactivate_alarm_sound_effect();
  return;
}

bool alarmIsActive(void)
{
  return (0 < alarm_timer);
}





void init_trigger_toxic_gas_effect(coord3d *source) //#MATCH
{
    activate_gas_sound_timer = TRUE;
    D_80030AE0               = 0.0f;
    D_80030AD0.x             = source->x;
    D_80030AD0.y             = source->y;
    D_80030AD0.z             = source->z;
    if (bossGetStageNum() == LEVELID_EGYPT)
    {
        gasTimeToFullOpacity = 120.0f;
        gasDoesDamageFlag = FALSE;
        return;
    }
    gasTimeToFullOpacity = 3600.0f;
    gasDoesDamageFlag = TRUE;
}






void check_deactivate_gas_sound(void)
{
    if ((ptr_gas_sound != NULL) && (sndGetPlayingState(ptr_gas_sound) != AL_STOPPED)) {
        sndDeactivate(ptr_gas_sound);
    }
    return;
}



bool check_if_toxic_gas_activated() //#MATCH
{
    return (toxic_gas_sound_timer > 0);
}


void handle_gas_damage(void)
{
    if (activate_gas_sound_timer != 0)
    {
        toxic_gas_sound_timer += g_GlobalTimerDelta;
        if (gasTimeToFullOpacity <= toxic_gas_sound_timer)
        {
            toxic_gas_sound_timer = gasTimeToFullOpacity;
            activate_gas_sound_timer = 0;
        }
    }

    if (toxic_gas_sound_timer > 0.0f && g_PlayerInvincible == 0)
    {
        fogSwitchToSolosky2(toxic_gas_sound_timer / gasTimeToFullOpacity);

        if (gasDoesDamageFlag == 0) { return; }

#ifdef VERSION_EU
        if (D_80030ADC < (g_GlobalTimer - 0xBB))
#else
        if (D_80030ADC < (g_GlobalTimer - 0xE1))
#endif
        {
            D_80030ADC = g_GlobalTimer;
            if (toxic_gas_sound_timer >= 600.0f)
            {
                sndPlaySfx((struct ALBankAlt_s* ) g_musicSfxBufferPtr, 0x62, NULL);
            }
            if (toxic_gas_sound_timer >= 1800.0f)
            {
                record_damage_kills(0.125f, 0.0f, 0.0f, -1, 0);
            }
        }

        if (D_80030AE0 < gasTimeToFullOpacity)
        {
            D_80030AE0 = D_80030AE0 + g_GlobalTimerDelta;
            if ((ptr_gas_sound == NULL) && (lvlGetControlsLockedFlag() == 0))
            {
                sndPlaySfx((struct ALBankAlt_s* ) g_musicSfxBufferPtr, 0x66, (ALSoundState* ) &ptr_gas_sound);
            }
            if (ptr_gas_sound != NULL)
            {
                chrobjSndCreatePostEventDefault(ptr_gas_sound, &D_80030AD0);
            }
        }
        else
        {
            if ((ptr_gas_sound != NULL) && (sndGetPlayingState(ptr_gas_sound) != 0))
            {
                sndDeactivate(ptr_gas_sound);
            }
        }
    }
}


void countdownTimerSetVisible(int clocklockbits, bool unset)
{
    if (unset)
    {
        clock_drawn_flag &= ~clocklockbits;
        return;
    }
    clock_drawn_flag |= clocklockbits;
}

bool is_clock_drawn_onscreen(void)
{
    return clock_drawn_flag == FALSE;
}

void countdownTimerSetValue(f32 time)
{
    clock_time = time;
}

f32 countdownTimerGetValue(void)
{
    return clock_time;
}

void countdownTimerSetRunning(bool enable)
{
    clock_enable = enable;
}

bool countdownTimerIsRunning(void)
{
    return clock_enable;
}

void if_enabled_reset_clock(void)
{
    if (clock_enable != 0) {
        clock_time = clock_time - g_GlobalTimerDelta;
    }
}

const char D_80052A44[] = ":\n";

/*
    Renders the on-screen countdown timer
    using minutes, seconds and milliseconds
    in the following format

    00 : 00 : 00

    Timer value is set using countdownTimerSetValue()
*/
Gfx *countdownTimerRender(Gfx *DL) {

    s32 mins;
    s32 secs;
    s32 ms;
    s32 valign_offset;
    s32 unused;
    f32 time;

    if (clock_drawn_flag == 0) {

        time = clock_time;
        if (time < 0.0f) {
            time = -time;
        }

        mins = (s32) floorFloat(time / 3600.0f);
        secs = (s32) floorFloat(time / 60.0f) - (mins * 60);
        ms = ((s32) floorFloat((time * 100.0f) / 60.0f) - (mins * 6000)) - (secs * 100);

        DL = microcode_constructor(DL);

        #if defined(VERSION_US) || defined(VERSION_JP)
            valign_offset = 18;
        #else
            valign_offset = 28;
        #endif

        // Minutes
        DL = gunDrawHudInteger(DL, (mins % 100) / 10, 0x82, HUDHALIGN_MIDDLE, ( viGetViewTop() + viGetViewHeight()) - valign_offset, HUDVALIGN_MIDDLE, 1);
        DL = gunDrawHudInteger(DL, mins % 10, 0x8A, HUDHALIGN_MIDDLE, ( viGetViewTop() + viGetViewHeight()) - valign_offset, HUDVALIGN_MIDDLE, 1);

        // :
        DL = gunDrawHudString(DL, &D_80052A44, 0x93, HUDHALIGN_MIDDLE, (viGetViewTop() + viGetViewHeight()) - valign_offset, HUDVALIGN_MIDDLE, 1);

        // Seconds
        DL = gunDrawHudInteger(DL, (secs % 60) / 10, 0x9C, HUDHALIGN_MIDDLE, (viGetViewTop() + viGetViewHeight()) - valign_offset, HUDVALIGN_MIDDLE, 1);
        DL = gunDrawHudInteger(DL, secs % 10, 0xA4, HUDHALIGN_MIDDLE, (viGetViewTop() + viGetViewHeight()) - valign_offset, HUDVALIGN_MIDDLE, 1);

        // :
        DL = gunDrawHudString(DL, &D_80052A44, 0xAD, HUDHALIGN_MIDDLE, (viGetViewTop() + viGetViewHeight()) - valign_offset, HUDVALIGN_MIDDLE, 1);

        // Milliseconds
        DL = gunDrawHudInteger(DL, (ms % 100) / 10, 0xB6, HUDHALIGN_MIDDLE, (viGetViewTop() + viGetViewHeight()) - valign_offset, HUDVALIGN_MIDDLE, 1);
        DL = gunDrawHudInteger(DL, ms % 10, 0xBE, HUDHALIGN_MIDDLE, (viGetViewTop() + viGetViewHeight()) - valign_offset, HUDVALIGN_MIDDLE, 1);

        DL = combiner_bayer_lod_perspective(DL);
    }

    return DL;
}

void handle_alarm_gas_timer_calldamage(void)
{
    if (alarmIsActive() != 0)
    {
        if ((ptr_alarm_sfx == 0) && (lvlGetControlsLockedFlag() == 0))
        {
            sndPlaySfx(g_musicSfxBufferPtr, ALARM3_SFX, &ptr_alarm_sfx);
        }

        alarm_timer = alarm_timer + g_ClockTimer;

        if (CHROBJ_GAS_TIMER < alarm_timer)
        {
            alarmDeactivate();
        }
    }

    handle_gas_damage();
    if_enabled_reset_clock();
    check_guard_detonate_proxmine();
    g_RemoteMineOwnerTriggerFlag = 0;

    return;
}





#ifdef NONMATCHING
// https://decomp.me/scratch/COEsd 99.07%
void sub_GAME_7F056690(void)
{
    Model *temp_s3;
    PropRecord *var_s2;
    ObjectRecord *temp_s1;
    ModelNode *temp_v0_2;
    struct ModelRoData_DisplayList_CollisionRecord *temp_s0;

    var_s2 = get_ptr_obj_pos_list_current_entry();

    for (; var_s2 != NULL; var_s2 = var_s2->prev)
    {
        if ((var_s2->type == 1) && !(var_s2->flags & 2) )
        {
            temp_s1 = var_s2->obj;

            if (temp_s1->state & 0x80)
            {
                temp_s3 = temp_s1->model;
                temp_v0_2 = sub_GAME_7F04B478(temp_s1);

                if (temp_v0_2 != NULL)
                {
                    temp_s0 = (struct ModelRoData_DisplayList_CollisionRecord *)temp_v0_2->Data;
                    if (temp_s0 != NULL)
                    {
                        if (sub_GAME_7F04B590(temp_s1->model->obj, temp_v0_2))
                        {
                            // temp_s3->datas should be struct ModelRwData_DisplayList_CollisionRecord here.
                            struct ModelRwData_DisplayList_CollisionRecord **cr = (struct ModelRwData_DisplayList_CollisionRecord**)temp_s3->datas;

                            if (temp_s0->Vertices != (Vertex*)cr[temp_s0->RwDataIndex])
                            {
                                objFreePermanently(temp_s1, 1);
                                return;
                            }
                        }

                        //continue;
                    }
                }
            }

           // break;
        }
    }

}
#else
GLOBAL_ASM(
.text
glabel sub_GAME_7F056690
/* 08B1C0 7F056690 27BDFFD0 */  addiu $sp, $sp, -0x30
/* 08B1C4 7F056694 AFBF002C */  sw    $ra, 0x2c($sp)
/* 08B1C8 7F056698 AFB40028 */  sw    $s4, 0x28($sp)
/* 08B1CC 7F05669C AFB30024 */  sw    $s3, 0x24($sp)
/* 08B1D0 7F0566A0 AFB20020 */  sw    $s2, 0x20($sp)
/* 08B1D4 7F0566A4 AFB1001C */  sw    $s1, 0x1c($sp)
/* 08B1D8 7F0566A8 0FC0E909 */  jal   get_ptr_obj_pos_list_current_entry
/* 08B1DC 7F0566AC AFB00018 */   sw    $s0, 0x18($sp)
/* 08B1E0 7F0566B0 1040002A */  beqz  $v0, .L7F05675C
/* 08B1E4 7F0566B4 00409025 */   move  $s2, $v0
/* 08B1E8 7F0566B8 24140001 */  li    $s4, 1
/* 08B1EC 7F0566BC 924E0000 */  lbu   $t6, ($s2)
.L7F0566C0:
/* 08B1F0 7F0566C0 568E0024 */  bnel  $s4, $t6, .L7F056754
/* 08B1F4 7F0566C4 8E520024 */   lw    $s2, 0x24($s2)
/* 08B1F8 7F0566C8 924F0001 */  lbu   $t7, 1($s2)
/* 08B1FC 7F0566CC 31F80002 */  andi  $t8, $t7, 2
/* 08B200 7F0566D0 57000020 */  bnezl $t8, .L7F056754
/* 08B204 7F0566D4 8E520024 */   lw    $s2, 0x24($s2)
/* 08B208 7F0566D8 8E510004 */  lw    $s1, 4($s2)
/* 08B20C 7F0566DC 92390002 */  lbu   $t9, 2($s1)
/* 08B210 7F0566E0 02202025 */  move  $a0, $s1
/* 08B214 7F0566E4 33280080 */  andi  $t0, $t9, 0x80
/* 08B218 7F0566E8 5100001A */  beql  $t0, $zero, .L7F056754
/* 08B21C 7F0566EC 8E520024 */   lw    $s2, 0x24($s2)
/* 08B220 7F0566F0 0FC12D1E */  jal   sub_GAME_7F04B478
/* 08B224 7F0566F4 8E330014 */   lw    $s3, 0x14($s1)
/* 08B228 7F0566F8 10400018 */  beqz  $v0, .L7F05675C
/* 08B22C 7F0566FC 00402825 */   move  $a1, $v0
/* 08B230 7F056700 8C500004 */  lw    $s0, 4($v0)
/* 08B234 7F056704 52000016 */  beql  $s0, $zero, .L7F056760
/* 08B238 7F056708 8FBF002C */   lw    $ra, 0x2c($sp)
/* 08B23C 7F05670C 8E290014 */  lw    $t1, 0x14($s1)
/* 08B240 7F056710 0FC12D64 */  jal   sub_GAME_7F04B590
/* 08B244 7F056714 8D240008 */   lw    $a0, 8($t1)
/* 08B248 7F056718 5040000E */  beql  $v0, $zero, .L7F056754
/* 08B24C 7F05671C 8E520024 */   lw    $s2, 0x24($s2)
/* 08B250 7F056720 960B001A */  lhu   $t3, 0x1a($s0)
/* 08B254 7F056724 8E6A0010 */  lw    $t2, 0x10($s3)
/* 08B258 7F056728 8E0D0008 */  lw    $t5, 8($s0)
/* 08B25C 7F05672C 000B6080 */  sll   $t4, $t3, 2
/* 08B260 7F056730 014C1021 */  addu  $v0, $t2, $t4
/* 08B264 7F056734 8C4E0000 */  lw    $t6, ($v0)
/* 08B268 7F056738 11AE0005 */  beq   $t5, $t6, .L7F056750
/* 08B26C 7F05673C 02202025 */   move  $a0, $s1
/* 08B270 7F056740 0FC10409 */  jal   objFreePermanently
/* 08B274 7F056744 24050001 */   li    $a1, 1
/* 08B278 7F056748 10000005 */  b     .L7F056760
/* 08B27C 7F05674C 8FBF002C */   lw    $ra, 0x2c($sp)
.L7F056750:
/* 08B280 7F056750 8E520024 */  lw    $s2, 0x24($s2)
.L7F056754:
/* 08B284 7F056754 5640FFDA */  bnezl $s2, .L7F0566C0
/* 08B288 7F056758 924E0000 */   lbu   $t6, ($s2)
.L7F05675C:
/* 08B28C 7F05675C 8FBF002C */  lw    $ra, 0x2c($sp)
.L7F056760:
/* 08B290 7F056760 8FB00018 */  lw    $s0, 0x18($sp)
/* 08B294 7F056764 8FB1001C */  lw    $s1, 0x1c($sp)
/* 08B298 7F056768 8FB20020 */  lw    $s2, 0x20($sp)
/* 08B29C 7F05676C 8FB30024 */  lw    $s3, 0x24($sp)
/* 08B2A0 7F056770 8FB40028 */  lw    $s4, 0x28($sp)
/* 08B2A4 7F056774 03E00008 */  jr    $ra
/* 08B2A8 7F056778 27BD0030 */   addiu $sp, $sp, 0x30
)
#endif





void drop_inventory(void)
{
    ChrRecord *playerchr;
    PropRecord *prop;
    enum ITEM_IDS item;
    enum PROP propid;

    playerchr = g_CurrentPlayer->prop->chr;

    chrSetWeaponFlag4(playerchr, GUNRIGHT);
    chrSetWeaponFlag4(playerchr, GUNLEFT);

    for (item = ITEM_FIST; item != ITEM_IDS_MAX; item++)
    {
        propid = getPropForHeldItem(item);

        if ((propid >= 0) && (bondinvHasInvItem(item) != 0))
        {
            prop = something_with_generating_object(playerchr, propid, item, 0x20000000, NULL, NULL);

            if (prop != NULL)
            {
                propobjSetDropped(prop, DROPTYPE_DEFAULT);
                objDrop(prop);
            }
        }
    }
}
