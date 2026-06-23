#line 1 "src/game/chrobjhandler.c"
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
#include "random.h"
#include "unk_0B3200.h"
#include "textrelated.h"
#include "gun.h"
#include "fr.h"
#include "objective_status.h"


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

void _asmpp_func1(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func2(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
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

void _asmpp_func3(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func4(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
#endif





#ifdef NONMATCHING
// PD: func0f06b488
bool sub_GAME_7F041400(PropRecord *prop, coord3d *arg1, coord3d *arg2, coord3d *arg3, coord3d *arg4, coord3d *arg5, f32 *arg6) {

}
#else
bool sub_GAME_7F041400(PropRecord *prop, coord3d *arg1, coord3d *arg2, coord3d *arg3, coord3d *arg4, coord3d *arg5, f32 *arg6);

void _asmpp_func5(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func6(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func7(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
#endif





#ifdef NONMATCHING
void sub_GAME_7F0417DC(void) {

}
#else

void _asmpp_func8(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func9(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
#endif





#ifdef NONMATCHING
// PD: func0f06b610
bool sub_GAME_7F0419E4(ObjectRecord *obj, coord3d *arg1, coord3d *arg2, coord3d *arg3, f32 arg4, coord3d *arg5, coord3d *arg6, coord3d *arg7, coord3d *arg8, f32 *arg9) {

}
#else
bool sub_GAME_7F0419E4(ObjectRecord *obj, coord3d *arg1, coord3d *arg2, coord3d *arg3, f32 arg4, coord3d *arg5, coord3d *arg6, coord3d *arg7, coord3d *arg8, f32 *arg9);

void _asmpp_func10(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

 } void _asmpp_large_func11(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
#endif





#ifdef NONMATCHING
// PD: func0f06c28c
bool sub_GAME_7F041BB8(ChrRecord *chr, coord3d *arg1, coord3d *arg2, f32 arg3, coord3d *arg4, coord3d *arg5, coord3d *arg6, coord3d *arg7, f32 *arg8) {

}
#else
bool sub_GAME_7F041BB8(ChrRecord *chr, coord3d *arg1, coord3d *arg2, f32 arg3, coord3d *arg4, coord3d *arg5, coord3d *arg6, coord3d *arg7, f32 *arg8);

void _asmpp_func12(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func13(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
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

void _asmpp_func14(void) {








*(volatile float*)0 = -4.214055994530569e+19f;


*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func15(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func16(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func17(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func18(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
#endif





#ifdef NONMATCHING
void sub_GAME_7F042A0C(void) {

}
#else

void _asmpp_func19(void) {








*(volatile float*)0 = -4.21405643433522e+19f;


*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func20(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func21(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
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

void _asmpp_func22(void) {


















*(volatile float*)0 = -4.214056874139871e+19f;


*(volatile float*)0 = -4.214057313944522e+19f;


*(volatile float*)0 = -4.214057753749173e+19f;


*(volatile float*)0 = -4.214058193553824e+19f;


*(volatile float*)0 = -4.214058633358475e+19f;


*(volatile float*)0 = -4.214059073163126e+19f;


*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func23(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

 } void _asmpp_large_func24(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
#endif





#ifdef NONMATCHING
void sub_GAME_7F043650(void) {

}
#else

#if defined(LEFTOVERDEBUG)

void _asmpp_func25(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func26(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
#endif

#if !defined(LEFTOVERDEBUG)

void _asmpp_func27(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func28(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
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

void _asmpp_func29(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func30(void) { 





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func31(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func32(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func33(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
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


#ifdef NONMATCHING


// version US defines

#define AUTOGUN_P1_ACCEL 0.000011635529f
#define AUTOGUN_P1_DECEL AUTOGUN_P1_ACCEL
#define AUTOGUN_P1_MAXSPEED 0.00069813174f

#define AUTOGUN_P2_ACCEL 0.0000058177643f
#define AUTOGUN_P2_DECEL AUTOGUN_P1_ACCEL
#define AUTOGUN_P2_MAXSPEED 0.00034906587f

// https://decomp.me/scratch/c9RKZ 65.91%
// PD name: objTick
// Address: 7F0452D4
s32 object_interaction(struct PropRecord *arg0)
{
    struct coord3d sp694;
    s32 sp690;
    struct PropRecord *sp684;
    s32 sp680;
    f32 sp67C;
    s32 sp674;
    s32 sp670;
    s32 sp678;
    s32 sp664;
    struct coord3d sp658;
    struct coord3d sp64C;
    struct WeaponObjRecord *sp648;
    struct ModelRoData_BoundingBoxRecord * sp640;
    f32 sp63C;
    f32 sp638;
    struct coord3d sp62C;
    struct coord3d sp620;
    struct coord3d sp614;
    s32 sp610;
    s32 sp60C;
    // missing: sp608
    s32 sp604;
    struct coord3d sp5CC;
    Mtxf *sp58C;
    struct coord3d sp564;
    f32 sp550[4]; //quatf
    struct ModelRoData_BoundingBoxRecord *sp54C;
    s32 sp548;
    struct coord3d sp53C;
    struct coord3d sp530;
    f32 sp518;
    struct PropRecord* sp514;
    struct coord3d sp5xx; // temp variable, unknown stack position
    struct StandTile *sp4F0;
    f32 sp4D8;
    f32 sp4D4;
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
    struct StandTile *sp490;

    f32 sp47C;
    struct coord3d *sp478;
    struct StandTile *sp468;
    f32 sp460;
    struct coord3d sp450;
    struct StandTile *sp44C;
    f32 sp444;
    f32 sp440;
    f32 sp43C;
    f32 sp438;
    f32 sp434;
    f32 sp430;
    f32 sp42C;
    f32 sp424;
    f32 sp420;
    f32 sp418;
    f32 sp414;
    f32 sp40C;
    f32 sp408;
    f32 sp400;
    Mtxf sp3A4;
    struct DoorRecord *sp39C;
    f32 sp394;
    Mtxf *sp390;
    s32 sp38C;
    f32 sp384;
    f32 sp380;
    struct CCTVRecord *sp370;
    struct coord3d sp360;
    Mtxf sp320;
    struct AutogunRecord *sp318;
    struct coord3d sp308;
    f32 sp304;
    f32 sp300;
    Mtxf *sp2FC;
    s32 sp2F8;
    Mtxf sp2AC;
    Mtxf sp26C;
    struct coord3d *sp260;
    struct coord3d *sp25C;
    struct coord3d *sp258;
    struct coord3d *sp254;
    f32 sp250;
    f32 sp24C;
    f32 sp248;
    Mtxf sp200;
    struct coord3d *sp1FC;
    ModelRenderData sp1B0;
    Mtxf sp16C;
    struct coord3d *sp168;
    struct coord3d *sp164;
    struct coord3d *sp160;
    struct coord3d *sp15C;
    struct ModelRoData_BoundingBoxRecord *sp158;
    f32 sp154;
    s32 sp13C;
    s32 sp138;
    struct coord3d sp12C;
    struct coord3d sp120;
    s32 sp11C;
    struct coord3d sp110;
    struct StandTile *sp10C;
    struct StandTile *sp108;
    s32 sp104;
    struct PropRecord* sp100;
    Mtxf spB8;
    f32 sp94;
    struct beam *beam; //sp90

    Mtxf *sp70;                                     /* compiler-managed */
    Mtxf *sp6C;

    struct ObjectRecord *obj;
    struct Model *model;

    f32 temp_f12;
    s32 var_s2_2;
    s32 var_a0;
    struct StandTile *temp_s2;
    f32 temp_f14_2;
    Mtxf *temp_s2_4;
    f32 temp_f14_3;
    f32 temp_f20;
    f32 temp_f14_4;
    struct ALSoundState * sfx_state;
    s32 var_s2_3;
    f32 temp_f12_5;
    s32 var_v1_3;
    f32 temp_f0_13; // ~ sp510
    s32 var_s2_4;
    f32 var_f2;
    f32 temp_f0_14;
    f32 var_f12;
    f32 var_f2_3;
    struct PropRecord *temp_v0_19;
    f32 var_f12_3;
    s32 var_v0_3;
    f32 var_f0_2;
    f32 var_f12_5;
    f32 var_f2_6;
    f32 temp_f0_22;
    s32 var_v1_5;
    s32 var_s0;
    f32 *temp_s0_5;
    f32 *temp_v0_25;
    s32 var_s2_5;
    f32 var_f12_6;
    s32 var_s0_2;
    struct Model *temp_s0_6;
    struct PadRecord *var_v1_4;
    struct ModelFileHeader *temp_v0_29;
    s32 var_s2_6;
    struct ModelFileHeader *temp_v0_31;
    Mtxf *temp_a1_4;
    Mtxf *temp_s0_10;
    f32 temp_f0_31;
    s32 temp_v0_32;
    Mtxf *temp_a1_6;
    f32 var_f12_7;
    struct coord3d *temp_s0_13;
    struct coord3d *temp_s0_14;
    struct ModelNode** temp_v1_7;
    f32 var_f0_3;
    f32 *temp_v0_40;
    ModelNode **var_v1_7;
    Mtxf *temp_s0_21;
    ModelNode **temp_v1_8;
    f32 var_f12_9;
    s32 var_a0_6;
    ModelNode **temp_v1_10;
    Mtxf *temp_s2_7;
    struct coord3d *temp_v1_11;
    struct coord3d temp_fc3d_a;
    struct coord3d temp_fc3d_b;
    f32 temp_f20_4;
    f32 temp_f0_35;
    f32 var_f2_7;
    f32 temp_f2_23;
    f32 var_f4;

    obj = arg0->obj;
    model = obj->model;

    sp690 = 0;
    sp680 = 0;
    sp67C = 0.0f;
    sp674 = getPlayerCount();
    sp670 = 1;

    if (obj->runtime_bitflags & RUNTIMEBITFLAG_REMOVE)
    {
        objFree(obj, 0, obj->state & RUNTIMEBITFLAG_REMOVE);

        return 1;
    }
    else if (obj->runtime_bitflags & RUNTIMEBITFLAG_ISRETICK)
    {
        obj->runtime_bitflags &= ~RUNTIMEBITFLAG_ISRETICK;
    }
    else if (obj->runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT)
    {
        arg0->flags &= 0xFFFD;
        obj->runtime_bitflags |= RUNTIMEBITFLAG_ISRETICK;

        return 3;
    }

    if (sp674 == 1)
    {
        sp678 = 1;
    }
    else
    {
        sp678 = sub_GAME_7F09B4D8(get_cur_playernum()) == 0;

        if (obj->runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT)
        {
            if (obj->projectile->ownerprop != NULL)
            {
                sp678 = g_CurrentPlayer->prop == obj->projectile->ownerprop;
            }
        }
    }

    // mips2c line 559
    if (sp678)
    {
        if (obj->runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT)
        {
            struct Projectile *temp_s0 = obj->projectile;

            temp_s0->unkE0 += g_ClockTimer;

            if (temp_s0->unkE0 > 2400)
            {
                obj->runtime_bitflags |= RUNTIMEBITFLAG_REMOVE;
            }

            if (temp_s0->flags & 0x100)
            {
                struct PropRecord *projectile_prop;

                s32 var_s2 = 1;

                sp648 = (struct WeaponObjRecord *)obj;

                if (obj->type == PROPDEF_COLLECTABLE && sp648->weaponnum == ITEM_NULL86)
                {
                    var_s2 = 0;
                }

                projectile_prop = temp_s0->ownerprop;

                if (projectile_prop != NULL)
                {
                    sub_GAME_7F03D058(projectile_prop, 0);
                }

                sp664 = sub_GAME_7F042EB4(obj, &temp_s0->unkd4, &sp64C, &sp658, 0, var_s2);

                if (projectile_prop != NULL)
                {
                    sub_GAME_7F03D058(projectile_prop, 1);
                }

                if ((sp664 != 1) && (sp648 != NULL) && sp648->weaponnum == ITEM_NULL86)
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

            // mips2c line 608
            if (obj->projectile->flags & 1)
            {
                struct WeaponObjRecord *temp_s1 = (struct WeaponObjRecord *)obj;

                sp640 = sub_GAME_7F040078(obj);
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
                    if (temp_s0->unk1C < (10.0f/360.0f))
                    {
                        temp_s0->unkB4 += temp_s0->unk10.f[1] * g_GlobalTimerDelta;
                        temp_s0->unkB0 += temp_s0->unkB4 * g_GlobalTimerDelta;
                        temp_s0->unk1C += ((1.0f/90.0f) * g_GlobalTimerDelta);

                        if (temp_s0->unk1C > (10.0f/360.0f))
                        {
                            temp_s0->unk1C = (10.0f/360.0f);
                        }
                    }
                    else
                    {
                        if (sp694.f[1] < temp_s0->unkB0)
                        {
                            temp_s0->unkB4 += temp_s0->unk10.f[1] * g_GlobalTimerDelta;
                            temp_s0->unkB0 += (temp_s0->unkB4 * g_GlobalTimerDelta);
                            sp694.f[1] += 0.07f * (temp_s0->unkB0 - sp694.f[1]) * g_GlobalTimerDelta;
                        }
                        else
                        {
                            sp694.f[1] = temp_s0->unkB0;
                            temp_s0->flags &= ~0x20;
                            temp_s0->flags |= 0x10;
                            temp_s0->unk1C = 0.0f;
                            temp_s0->speed.f[1] = temp_s0->unkB4;
                        }
                    }
                }

                if (!(temp_s0->flags & 0x10))
                {
                    temp_s0->speed.f[1] += (temp_s0->unk10.f[1] + temp_s0->unk1C) * g_GlobalTimerDelta;
                    temp_f12 = temp_s0->speed.f[1];
                    temp_s0->speed.f[1] -= (0.2777778f * g_GlobalTimerDelta);
                    sp694.f[1] += g_GlobalTimerDelta * (temp_f12 + temp_s0->speed.f[1]) * 0.5f;
                }
                else
                {
                    temp_s0->speed.f[1] += (temp_s0->unk10.f[1] + temp_s0->unk1C) * g_GlobalTimerDelta;
                    sp694.f[1] += temp_s0->speed.f[1] * g_GlobalTimerDelta;
                }

                sp70 = &obj->mtx;
                sp6C = &obj->mtx;

                temp_s0->speed.f[0] += temp_s0->unk10.f[0] * g_GlobalTimerDelta;
                temp_s0->speed.f[2] += temp_s0->unk10.f[2] * g_GlobalTimerDelta;

                sp694.f[0] += temp_s0->speed.f[0] * g_GlobalTimerDelta;
                sp694.f[2] += temp_s0->speed.f[2] * g_GlobalTimerDelta;

                sub_GAME_7F057DF8(sp70, sp6C, g_ClockTimer, &g_ClockTimer);

                if ((obj->type == PROPDEF_COLLECTABLE) && (((temp_s1->weaponnum == 0x1D)) || (temp_s1->weaponnum == 0x1B) || (temp_s1->weaponnum == 0x1C) || (temp_s1->weaponnum == 0x21) || (temp_s1->weaponnum == 0x2F) || (temp_s1->weaponnum == 0x30) || (temp_s1->weaponnum == 0x22)))
                {
                    var_s2_2 = 1;
                }

                if (temp_s0->ownerprop != NULL)
                {
                    sub_GAME_7F03D058(temp_s0->ownerprop, 0);
                }

                sp664 = sub_GAME_7F042EB4(obj, &sp694.f[0], &sp614, &sp620, var_s2_2, 0);

                if (temp_s0->ownerprop != NULL)
                {
                    sub_GAME_7F03D058(temp_s0->ownerprop, 1);
                }

                sp690 = 1;

                // mps2c line 696
                if ((sp664 == 2) && ((D_80030B0C == NULL) || (((D_80030B0C->type != 3)) && (D_80030B0C->type != 6))))
                {
                    var_a0 = 0;

                    if ((D_80030B0C != NULL) && (D_80030B0C->obj->runtime_bitflags & 0x80))
                    {
                        var_a0 = 1;
                    }

                    if (var_a0 == 0)
                    {
                        projectileFree(temp_s0);

                        obj->projectile = NULL;
                        obj->runtime_bitflags &= ~0x80;

                        if (arg0->flags & 8)
                        {
                            arg0->flags |= 0x10;
                        }

                        chrobjSndCreatePostEventDefault(sndPlaySfx((struct ALBankAlt_s *) g_musicSfxBufferPtr, ATTACH_MINE_SFX, NULL), &arg0->pos);
                        sub_GAME_7F0577E8(((struct WeaponObjRecord *)obj)->weaponnum, arg0->stan->room);
                        sub_GAME_7F0439B8(obj, &sp614, arg0->stan, &sp620);

                        if (D_80030B0C != NULL)
                        {
                            temp_s2 = arg0->stan;

                            if (objEmbed(arg0, D_80030B0C, objinst, dword_CODE_bss_80075B74) != 0)
                            {
                                arg0->stan = temp_s2;
                                sp680 = 5;
                                sp604 = 1;
                            }
                        }
                    }
                }

                // mpis2c line 728
                if (sp604 == 0)
                {
                    if ((D_80030B0C != NULL) && (obj->type == PROPDEF_COLLECTABLE))
                    {
                        struct WeaponObjRecord *temp_weap = (struct WeaponObjRecord *)obj;

                        if (temp_weap->weaponnum == ITEM_THROWKNIFE)
                        {
                            if (D_80030B0C->type != 3)
                            {
                                if (D_80030B0C->type == 6 && D_80030B0C->obj != NULL)
                                {
                                    if (getPlayerPointerIndex(D_80030B0C) != get_cur_playernum())
                                    {
                                        //goto block_77;
                                    }
                                }
                            }
                            else
                            {
//block_77:;
                                struct Projectile *temp_v0_8 = obj->projectile;

                                if ((temp_v0_8->flags & 1)
                                    && ((s32) temp_v0_8->unk90 <= 0)
                                    && (obj->runtime_bitflags & 0x20)
                                    && (handles_shot_actors(D_80030B0C->chr, bodypartshot, &flt_CODE_bss_80075B78, ((struct WeaponObjRecord *)obj)->weaponnum, 1) != 0))
                                {
                                    sp604 = 1;

                                    if (temp_s0->unk8C > 0.0f)
                                    {
                                        temp_f14_2 = ((sp620.f[2] * temp_s0->speed.f[2]) + ((temp_s0->speed.f[0] * sp620.f[0]) + (temp_s0->speed.f[1] * sp620.f[1]))) * -(temp_s0->unk8C + 1.0f);
                                        temp_s0->speed.f[0] = temp_s0->speed.f[0] + (temp_f14_2 * sp620.f[0]);
                                        temp_s0->speed.f[1] = temp_s0->speed.f[1] + (temp_f14_2 * sp620.f[1]);
                                        temp_s0->speed.f[2] = temp_s0->speed.f[2] + (temp_f14_2 * sp620.f[2]);
                                    }

                                    if (!(temp_s0->flags & 0x200))
                                    {
                                        mtxLoadRandomRotation(sp6C);
                                    }

                                    temp_s0->unk90 += 1;
                                    recall_joy2_hits_edit_detail_edit_flag(((struct WeaponObjRecord *)obj)->weaponnum, D_80030B0C, -1);

                                    if ((D_80030B0C->flags & 2) && (bodypartshot != 0x64) && (bodypartshot != 0x6E))
                                    {
                                        temp_s2_4 = modelFindNodeMtx(objinst, dword_CODE_bss_80075B74, 0);

                                        sp5CC.f[0] = sp614.f[0];
                                        sp5CC.f[1] = sp614.f[1];
                                        sp5CC.f[2] = sp614.f[2];
                                        mtx4TransformVecInPlace(camGetWorldToScreenMtxf(), &sp5CC);

                                        sp5CC.f[0] += ((sp5CC.f[0] - temp_s2_4->m[3][0]) * 0.5f);
                                        sp5CC.f[1] += ((sp5CC.f[1] - temp_s2_4->m[3][1]) * 0.5f);
                                        sp5CC.f[2] += ((sp5CC.f[2] - temp_s2_4->m[3][2]) * 0.5f);

                                        sp5CC.f[0] -= (getjointsize(objinst, dword_CODE_bss_80075B74) * 0.5f * flt_CODE_bss_80075B88.f[0]);
                                        sp5CC.f[1] -= (getjointsize(objinst, dword_CODE_bss_80075B74) * 0.5f * flt_CODE_bss_80075B78.f[0]);
                                        sp5CC.f[2] -= (getjointsize(objinst, dword_CODE_bss_80075B74) * 0.5f * flt_CODE_bss_80075B88.f[0]);

                                        matrix_4x4_7F059E64(temp_s2_4, sp58C);
                                        mtx4TransformVecInPlace(sp58C, &sp5CC);
                                        sub_GAME_7F0221DC(objinst, bodypartshot, dword_CODE_bss_80075B74, &sp5CC);
                                    }
                                }
                            }
                        }
                        // mips2c line 795
                        else if (temp_weap->weaponnum == ITEM_NULL86)
                        {
                            if (D_80030B0C->type == PROP_TYPE_CHR)
                            {
                                sp604 = 1;
                                chrlvExplosionDamage((ChrRecord *) D_80030B0C->chr, &obj->runtime_pos, 2.0f, 1);
                            }
                            else if ((D_80030B0C->type == PROP_TYPE_OBJ) || (D_80030B0C->type == PROP_TYPE_WEAPON))
                            {
                                maybe_detonate_object(D_80030B0C->obj, 100.0f, &obj->runtime_pos, ITEM_NULL86, (s32) ((u32) (obj->runtime_bitflags & RUNTIMEBITFLAG_OWNER) >> RUNTIMEBITSHIFT_OWNER));
                            }

                            ((struct WeaponObjRecord *)obj)->timer = 0;
                        }
                    }
                }

                // mips2c line 811
                if (sp604 == 0)
                {
                    if (sp664 == 0)
                    {
                        if (temp_s0->unk8C > 0.0f)
                        {
                            temp_f14_3 = ((sp620.f[2] * temp_s0->speed.f[2]) + ((temp_s0->speed.f[0] * sp620.f[0]) + (temp_s0->speed.f[1] * sp620.f[1]))) * -(temp_s0->unk8C + 1.0f);

                            temp_s0->speed.f[0] += (temp_f14_3 * sp620.f[0]);
                            temp_s0->speed.f[1] += (temp_f14_3 * sp620.f[1]);
                            temp_s0->speed.f[2] += (temp_f14_3 * sp620.f[2]);

                            if ((temp_s0->speed.f[1] <= 0.0f) && ((temp_s0->speed.f[1] >= 0.0f) || (temp_f20 <= obj->runtime_pos.f[1])))
                            {
                                sp60C = 1;
                            }
                        }
                    }

                    // mips2c line 832
                    if (!(temp_s0->flags & 8))
                    {
                        sp63C = stanGetPositionYValue(arg0->stan, arg0->pos.f[0], arg0->pos.f[2]);
                        sp638 = chrpropSumMatrixPosY(sp640, &sp70[0]);
                        sp610 = (arg0->pos.f[1] < (sp63C - sp638));
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
                            arg0->pos.f[1] = (sp63C - sp638) + 4.0f;
                            obj->runtime_pos.f[1] = arg0->pos.f[1];
                        }
                        else
                        {
                            arg0->pos.f[1] = (sp614.f[1] - sp638) + 4.0f;
                            obj->runtime_pos.f[1] = arg0->pos.f[1];
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

                    //mips2c line 892
                    if (obj->type == 8)
                    {
                        if (temp_s1->weaponnum == 3)
                        {
                            sub_GAME_7F043650(temp_s1);
                        }
                        else if (temp_s1->weaponnum == 0x56)
                        {
                            if ((sp664 == 0) || (sp610 != 0))
                            {
                                temp_s1->timer = 0;
                            }
                            else
                            {
                                temp_f14_4 = (temp_s0->speed.f[2] * temp_s0->speed.f[2]) + ((temp_s0->speed.f[0] * temp_s0->speed.f[0]) + (temp_s0->speed.f[1] * temp_s0->speed.f[1]));
                                if (temp_f14_4 > 27777.773f)
                                {
                                    temp_s0->unk10.f[0] = 0.0f;
                                    temp_s0->unk10.f[1] = 0.0f;
                                    temp_s0->unk10.f[2] = 0.0f;
                                }
                                if ((s32) temp_s0->unkA8 >= 0x12D)
                                {
                                    temp_s0->unk1C = 0.0f;
                                    temp_s0->flags &= ~0x30;
                                }
                                else
                                {
                                    explosionCreateSmoke(&temp_s1->runtime_pos, arg0->stan, 8, arg0->rooms, (arg0->flags & 8) != 0);
                                }
                            }
                        }
                        else if (temp_s1->weaponnum == 0x57)
                        {
                            if ((sp610 != 0) || (sp60C != 0))
                            {
                                temp_s1->timer = 0;
                            }
                            else
                            {
                                explosionCreateSmoke(&obj->runtime_pos, arg0->stan, 9, arg0->rooms, (arg0->flags & 8) != 0);
                            }
                        }

                        if ((sp664 == 0) || (sp610 != 0))
                        {
                            if (temp_s0->unkAC < (D_80048380 - 2))
                            {
                                if ((temp_s1->weaponnum == 3) || (temp_s1->weaponnum == 2))
                                {
                                    sfx_state = sndPlaySfx((struct ALBankAlt_s *) g_musicSfxBufferPtr, KNIFE_HIT_WALL_SFX, NULL);
                                }
                                else
                                {
                                    sfx_state = sndPlaySfx((struct ALBankAlt_s *) g_musicSfxBufferPtr, DROP_GUN_SFX, NULL);
                                }

                                chrobjSndCreatePostEventDefault(sfx_state, &arg0->pos);
                            }

                            temp_s0->unkAC = D_80048380;
                        }
                    }
                }

                // mips2c line 960
                if ((temp_s1->runtime_bitflags & 0x80) && (temp_s0->flags & 0x400) && !(D_80048380 & 7))
                {
                    struct PropRecord *temp_v0_13 = temp_s1->prop;

                    sp564.f[0] = temp_s1->runtime_pos.f[0] + 400.0f;
                    sp564.f[1] = temp_s1->runtime_pos.f[1] - 1800.0f;
                    sp564.f[2] = temp_s1->runtime_pos.f[2];

                    if (!(D_80048380 & 0xF))
                    {
                        sp564.f[2] += 400.0f;
                    }
                    else
                    {
                        sp564.f[2] -= 400.0f;
                    }


                    // void explosionCreate(void *, struct coord3d *pos, struct StandTile *stan, s16 /* enum EXPLOSION_DEF */ explosionType, s32 flag, s32 playernum, u8 *rooms, s32 flag2);
                    explosionCreate(NULL, &sp564, temp_v0_13->stan, 0x14, 0, 0, temp_v0_13->rooms, 0);
                    if (((s32) D_80048380 % 40) == 0)
                    {
                        // void explosionCreateSmoke(coord3d *pos, StandTile *stan, s16 arg2, u8 *rooms, s32 arg4);
                        explosionCreateSmoke(&sp564, temp_v0_13->stan, 0xA, temp_v0_13->rooms, 1);
                    }
                }
            }
            // mips2c line 982
            else
            {
                var_s2_3 = 1;

                if (temp_s0->unk60 < 1.0f)
                {
                    temp_s0->unk60 += (temp_s0->unk64 * g_GlobalTimerDelta);

                    if (g_ClockTimer > 0)
                    {
                        temp_s0->unk64 *= 1.1f;
                    }

                    if ((temp_s0->unk60 > 1.0f) || (temp_s0->flags & 8))
                    {
                        temp_s0->unk60 = 1.0f;
                    }

                    //void quaternion_slerp(quatf q1, quatf q2, f32 t, quatf result);
                    quaternion_slerp((f32*)&temp_s0->unk68, (f32*)&temp_s0->unk78, temp_s0->unk60, (f32*)&sp550);
                    sp70 = &obj->mtx;

                    // void quaternion_to_matrix(quatf q, mat44f matrix);
                    quaternion_to_matrix((f32*)&sp550, (f32*)&obj->mtx);
                    matrix_column_1_scalar_multiply(temp_s0->unkC0, (f32*)sp70);
                    matrix_column_2_scalar_multiply(temp_s0->unkC4, (f32*)sp70);
                    matrix_column_3_scalar_multiply_2(temp_s0->unkC8, (f32*)sp70);
                    var_s2_3 = 0;
                }

                if (((temp_s0->speed.f[0] != 0.0f) || (temp_s0->speed.f[2] != 0.0f) || (temp_s0->unk60 < 1.0f)) && !(temp_s0->flags & 8))
                {
                    sp70 = &obj->mtx;
                    var_s2_3 = 0;
                    sp54C = sub_GAME_7F040078(obj);

                    for (sp548 = 0; sp548 < g_ClockTimer; sp548++)
                    {
                        sp694.f[0] += temp_s0->speed.f[0];
                        sp694.f[2] += temp_s0->speed.f[2];

                        if (temp_s0->unk60 >= 1.0f)
                        {
                            if (temp_s0->unk94 > 0.0f)
                            {
                                temp_f12_5 = (temp_s0->unk94 * g_GlobalTimerDelta) / sqrtf((temp_s0->speed.f[2] * temp_s0->speed.f[2]) + (temp_s0->speed.f[0] * temp_s0->speed.f[0]));

                                if (temp_f12_5 >= 1.0f)
                                {
                                    temp_s0->speed.f[0] = 0.0f;
                                    temp_s0->speed.f[2] = 0.0f;
                                }
                                else
                                {
                                    temp_s0->speed.f[0] -= (temp_s0->speed.f[0] * temp_f12_5);
                                    temp_s0->speed.f[2] -= (temp_s0->speed.f[2] * temp_f12_5);
                                }
                            }
                            else
                            {
                                temp_s0->speed.f[0] *= 0.9f;
                                temp_s0->speed.f[2] *= 0.9f;
                            }
                        }
                    }

                    // s32 sub_GAME_7F042EB4(struct ObjectRecord *arg0, f32 *arg1, struct coord3d *arg2, struct coord3d *arg3, s32 arg4, s32 arg5);
                    sub_GAME_7F042EB4(obj, &sp694.f[0], &sp530, &sp53C, 0, 0);
                    sp690 = 1;

                    arg0->pos.f[1] = stanGetPositionYValue(arg0->stan, arg0->pos.f[0], arg0->pos.f[2]) - (chrpropSumMatrixPosY(sp54C, sp70) + 4.0f);
                    obj->runtime_pos.f[1] = arg0->pos.f[1];


                    if ((temp_s0->speed.f[0] < 0.1f) && (temp_s0->speed.f[0] > -0.1f))
                    {
                        if ((temp_s0->speed.f[2] < 0.1f) && (temp_s0->speed.f[2] > -0.1f))
                        {
                            temp_s0->speed.f[2] = 0.0f;
                            temp_s0->speed.f[0] = 0.0f;
                        }
                    }
                }

                if ((var_s2_3 != 0) || (temp_s0->flags & 8))
                {
                    projectileFree(temp_s0);

                    obj->projectile = NULL;
                    obj->runtime_bitflags &= ~0x80;

                    if (arg0->flags & 8)
                    {
                        arg0->flags |= 0x10;
                    }
                }
            }
        }

        // mips2c line 1081
        if (sp690 != 0)
        {
            sp70 = (Mtxf *)&obj->runtime_pos;
            chrobjCollisionRelated(obj);
            setupUpdateObjectRoomPosition(obj);
            sub_GAME_7F0402B4(obj->prop, &obj->nextcol);
            detonate_proxmine_In_range((struct coord3d *)sp70);
        }

        // mips2c line 1090
        if (obj->type == PROPDEF_DOOR)
        {
            struct DoorRecord *door = (struct DoorRecord *)arg0->obj;

            sp67C = door->openPosition;

            if ((door->openedTime > 0) && (door->openedTime < (g_GlobalTimer - door->autoCloseFrames)) && (door->openstate == DOORSTATE_STATIONARY) && !(door->flags & 0x80000000))
            {
                doorActivate(door, DOORSTATE_CLOSING);
            }

            if (door->openstate == DOORSTATE_WAITING)
            {
                struct DoorRecord *linked_door = door->linkedDoor;

                var_v1_3 = 1;

                while ((linked_door != NULL) && (linked_door != door))
                {
                    if ((linked_door->openstate != DOORSTATE_STATIONARY) || (linked_door->openPosition > 0.0f))
                    {
                        var_v1_3 = 0;
                    }

                    linked_door = linked_door->linkedDoor;
                }

                if (var_v1_3 != 0)
                {
                    doorSetOpenState(door, DOORSTATE_OPENING);
                }
            }

            if ((door->doorType == 8) && (doorIsClosed(door) != 0) && (doorIsPadlockFree(door) != 0))
            {
                doorActivateWrapper(arg0);
            }

            if ((door->timer < g_GlobalTimer) || (g_ClockTimer == 0))
            {
                door7F054FB4(door);
            }
        }
        // mips2c line 1130
        else if (obj->type == PROPDEF_CCTV && !(obj->flags & PROPFLAG_CCTV_DISABLED))
        {
            struct CCTVRecord *camera = (struct CCTVRecord *)arg0->obj;
            // camera is temp_s0_3

            if (camera->unkD4 != 0)
            {
                sp518 = camera->unkCC;
            }
            else
            {
                sp518 = camera->unkD0;
            }

            sp514 = get_curplayer_positiondata();

            temp_f0_13 = camera->unkE8;
            var_s2_4 = 1;
            sp5xx.f[0] = sp514->pos.f[0] - obj->runtime_pos.f[0];
            sp5xx.f[1] = sp514->pos.f[1] - obj->runtime_pos.f[1];
            sp5xx.f[2] = sp514->pos.f[2] - obj->runtime_pos.f[2];

            if ((temp_f0_13 > 0.0f) && ((temp_f0_13 * temp_f0_13) < ((sp5xx.f[0] * sp5xx.f[0]) + (sp5xx.f[1] * sp5xx.f[1]) + (sp5xx.f[2] * sp5xx.f[2]))))
            {
                var_s2_4 = 0;
            }

            if (obj->flags & 0x20000000)
            {
                var_s2_4 = 0;
            }

            if (var_s2_4 != 0)
            {
                temp_f0_14 = atan2f(sp5xx.f[0], sp5xx.f[2]);

                var_f2 = camera->unkC8;

                if ( var_f2 < 0.0f)
                {
                    var_f2 += 6.2831855f;
                }
                else if (var_f2 >= 6.2831855f)
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

                if ((var_f12 > 0.7853982f) || (var_f12 < -0.7853982f))
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

                    if (camera->timer >= (s32) (300.0f * F_80030B14))
                    {
                        alarmActivate();
                        camera->timer = 0;
                    }
                }

                bondviewUpdateGuardTankFlagsRelated(sp514, 1);
            }

            if (camera->unkC8 < sp518)
            {
                if ((sp518 - ((camera->unkD8 * camera->unkD8 * 0.5f) / 0.00065449846f)) <= camera->unkC8)
                {
                    camera->unkD8 = (f32) (camera->unkD8 - (0.00065449846f * g_GlobalTimerDelta));

                    if (camera->unkD8 < 0.00065449846f)
                    {
                        camera->unkD8 = 0.00065449846f;
                    }
                }
                else
                {
                    if (camera->unkD8 < camera->unkDC)
                    {
                        var_f2_3 = camera->unkD8 + (0.00065449846f * g_GlobalTimerDelta);

                        if (camera->unkDC < var_f2_3)
                        {
                            var_f2_3 = camera->unkDC;
                        }

                        if (camera->unkC8 < (sp518 - ((var_f2_3 * var_f2_3 * 0.5f) / 0.00065449846f)))
                        {
                            camera->unkD8 = var_f2_3;
                        }
                    }
                }

                camera->unkC8 += (camera->unkD8 * g_GlobalTimerDelta);

                if (sp518 <= camera->unkC8)
                {
                    camera->unkC8 = sp518;
                    camera->unkD8 = 0.0f;
                    camera->unkD4 = 0;
                }
            }
            else
            {
                if (camera->unkC8 <= (sp518 + ((camera->unkD8 * camera->unkD8 * 0.5f) / 0.00065449846f)))
                {
                    camera->unkD8 = (f32) (camera->unkD8 - (0.00065449846f * g_GlobalTimerDelta));

                    if (camera->unkD8 < 0.00065449846f)
                    {
                        camera->unkD8 = 0.00065449846f;
                    }
                }
                else
                {
                    if (camera->unkD8 < camera->unkDC)
                    {
                        var_f2_3 = camera->unkD8 + (0.00065449846f * g_GlobalTimerDelta);

                        if (camera->unkDC < var_f2_3)
                        {
                            var_f2_3 = camera->unkDC;
                        }

                        if ((sp518 + ((var_f2_3 * var_f2_3 * 0.5f) / 0.00065449846f)) < camera->unkC8)
                        {
                            camera->unkD8 = var_f2_3;
                        }
                    }
                }

                camera->unkC8 -= (camera->unkD8 * g_GlobalTimerDelta);

                if (camera->unkC8 <= sp518)
                {
                    camera->unkC8 = sp518;
                    camera->unkD8 = 0.0f;
                    camera->unkD4 = 1;
                }
            }
        }
        // mips2c line 1286
        else if (obj->type == PROPDEF_AUTOGUN && !(obj->flags & PROPFLAG_IS_DRONE_GUN))
        {
            struct AutogunRecord *autogun = (struct AutogunRecord*)arg0->obj;
            // autogun is temp_s0_4

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
                else if ((autogun->rot_related == autogun->unk90) && (autogun->unk98 == autogun->unk9C))
                {
                    autogun->unk98 =  (((( RANDOMGETNEXT_F32() * 39.0f) + 1.0f) * 6.2831855f) / 360.0f);
                    autogun->rot_related = (RANDOMGETNEXT_F32() * 6.2831855f);
                }

                chrobjCallsApplySpeed(&autogun->unk90, autogun->rot_related, &autogun->unk94, AUTOGUN_P1_ACCEL, AUTOGUN_P1_DECEL, AUTOGUN_P1_MAXSPEED);
                chrobjCallsApplySpeed(&autogun->unk9C, autogun->unk98, &autogun->unkA0, AUTOGUN_P2_ACCEL, AUTOGUN_P2_DECEL, AUTOGUN_P2_MAXSPEED);
            }
            else
            {
                // mips2c line 1317
                sp4BC.f[2] = temp_v0_19->pos.f[0] - obj->runtime_pos.f[0];
                sp4BC.f[1] = (temp_v0_19->pos.f[1] - obj->runtime_pos.f[1]) - 20.0f;
                sp4BC.f[0] = temp_v0_19->pos.f[2] - obj->runtime_pos.f[2];

                sp4B8 = (sp4BC.f[2] * sp4BC.f[2]) + (sp4BC.f[0] * sp4BC.f[0]);

                sp4B0 = sqrtf(sp4B8);

                if (obj->flags & 0x08000000)
                {
                    sp4B8 += (sp4BC.f[1] * sp4BC.f[1]);
                    sp4B0 = sqrtf(sp4B8);
                }

                sp4B4 = sp4B0;

                sp4A0 = chrlvGetAimLimitAngle(sp4B8);
                sp4D8 = autogun->rot_related;
                sp4D4 = autogun->unk98;

                if (sp4B4 <= autogun->aimdist)
                {
                    // mips2c line 1339
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

                        if ((sp498 - autogun->unk9C) < 0.0f)
                        {
                            // empty
                        }

                        if ((var_f12_3 < 1.2217306f) && (var_f12_3 > -1.2217306f))
                        {
                            sp4AC = 1;
                        }
                    }

                    if (sp4AC != 0)
                    {
                        sp494 = sp49C - autogun->rot_related;
                        sp490 = arg0->stan;
                        if (sp494 < -3.1415927f)
                        {
                            sp494 += 6.2831855f;
                        }
                        else if (sp494 >= 3.1415927f)
                        {
                            sp494 -= 6.2831855f;
                        }

                        bondviewUpdateGuardTankFlagsRelated(temp_v0_19, 0);

                        if ((sp494 <= autogun->unk88)
                            && (autogun->unk8C <= sp494)
                            && (sub_GAME_7F0B0E24(&sp490, arg0->pos.f[0], arg0->pos.f[2], temp_v0_19->pos.f[0], temp_v0_19->pos.f[2], 0x1B, arg0->pos.f[1], arg0->pos.f[1], temp_v0_19->pos.f[1], temp_v0_19->pos.f[1]) != 0)
                            && (sp490 == temp_v0_19->stan))
                        {
                            obj->flags |= 0x20000000;
                            sp4A4 = 1;
                            sp4D8 = sp49C;
                            sp4D4 = sp498;
                        }
                        else
                        {
                            if ((autogun->unkB8 >= 0) && ((g_GlobalTimer - 0x78) < autogun->unkB8))
                            {
                                sp4D8 = autogun->unk90;
                                sp4D4 = autogun->unk9C;
                            }
                            else
                            {
                                sp4AC = 0;
                            }
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
                    sp4D8 += sp4A0 * 0.8f * sinf(((f32) ((s32) g_GlobalTimer % 120) * 6.2831855f) / 120.0f);

                    if (sp4D8 < 0.0f)
                    {
                        sp4D8 += 6.2831855f;
                    }

                    if (sp4D8 >= 6.2831855f)
                    {
                        sp4D8 -= 6.2831855f;
                    }
                }

                // mips2c linee 1427

                var_f0_2 = sp4D8 - autogun->rot_related;

                if (var_f0_2 < -3.1415927f)
                {
                    var_f0_2 += 6.2831855f;
                }
                else if (var_f0_2 >= 3.1415927f)
                {
                    var_f0_2 -= 6.2831855f;
                }

                if (autogun->unk88 < var_f0_2 || var_f0_2 < autogun->unk8C)
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

                chrobjCallsApplySpeed(&autogun->unk90, sp4D8, &autogun->unk94, 0.0008726647f, 0.0008726647f, autogun->speed);
                chrobjCallsApplySpeed(&autogun->unk9C, sp4D4, &autogun->unkA0, 0.0008726647f, 0.0008726647f, autogun->speed);

                var_f12_5 = sp4D8 - autogun->unk90;

                if (var_f12_5 < 0.0f)
                {
                    var_f12_5 += 6.2831855f;
                }

                if (var_f12_5 > 3.1415927f)
                {
                    var_f12_5 -= 6.2831855f;
                }

                var_f2_6 = sp4D4 - autogun->unk9C;

                if (var_f2_6 < 0.0f)
                {
                    var_f2_6 += 6.2831855f;
                }

                if (var_f2_6 > 3.1415927f)
                {
                    var_f2_6 -= 6.2831855f;
                }

                autogun->is_active = 0;

                if (sp4AC != 0)
                {
                    if ((var_f12_5 < sp4A0)
                        && (-sp4A0 < var_f12_5)
                        && (var_f2_6 < sp4A0)
                        && (-sp4A0 < var_f2_6))
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
                        temp_f0_22 = 2.0f * sp4A0;

                        if ((var_f12_5 < temp_f0_22)
                            && (-temp_f0_22 < var_f12_5)
                            && (var_f2_6 < temp_f0_22)
                            && (-temp_f0_22 < var_f2_6))
                        {
                            autogun->is_active = 1;
                            sp4A8 = 1;
                            if (sp4A4 != 0)
                            {
                                autogun->unkB8 = (s32) g_GlobalTimer;
                            }
                        }
                        else
                        {
                            if ((autogun->unkB8 >= 0) && ((g_GlobalTimer - 0x78) < autogun->unkB8))
                            {
                                autogun->is_active = 1;
                                sp4A8 = 1;
                            }
                        }
                    }
                }
                if (sp4A8 != 0)
                {
                    autogun->unkB0 += (0.009973311f * g_GlobalTimerDelta);

                    if (autogun->unkB0 > 0.5983986f)
                    {
                        autogun->unkB0 = 0.5983986f;
                    }
                }
                else if (autogun->unkB0 > 0.0f)
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
                    autogun->unkB4 += (autogun->unkB0 * g_GlobalTimerDelta);

                    while (autogun->unkB4 >= 6.2831855f)
                    {
                        autogun->unkB4 -= 6.2831855f;
                    }
                }
            }
        }
        // mips2c line 1550
        else if (obj->type == PROPDEF_VEHICHLE)
        {
            struct VehichleRecord *temp_s1 = (struct VehichleRecord *)obj;

            sp478 = NULL;
            sp47C = temp_s1->roty;

            ai((PropDefHeaderRecord*)temp_s1, 1);

            if (temp_s1->speedtime60 >= 0.0f)
            {
                if (temp_s1->speedtime60 <= g_GlobalTimerDelta)
                {
                    temp_s1->speed = temp_s1->speedaim;
                }
                else
                {
                    temp_s1->speed += (((temp_s1->speedaim - temp_s1->speed) * g_GlobalTimerDelta) / temp_s1->speedtime60);
                }

                temp_s1->speedtime60 -= g_GlobalTimerDelta;
            }

            var_s0 = 0;

            // mips2c line 1570

            if (!(obj->flags2 & 0x80000) && (objIsHealthy(obj) != 0) && ((temp_s1->speed > 0.0f) || (temp_s1->speedaim > 0.0f)))
            {
                var_s0 = sub_GAME_7F053894(&temp_s1->runtime_pos, 2000.0f, 3000.0f);
            }

            if (var_s0 > 0)
            {
                if (((temp_s1->Sound == NULL) || (sndGetPlayingState(temp_s1->Sound) == 0)) && (lvlGetControlsLockedFlag() == 0))
                {
                    sndPlaySfx((struct ALBankAlt_s *) g_musicSfxBufferPtr, TRUCK_RUN_SFX, temp_s1->Sound);
                }

                if (temp_s1->Sound != NULL)
                {
                    sndCreatePostEvent(temp_s1->Sound, 8, var_s0);
                }
            }
            else
            {
                if ((temp_s1->Sound != NULL) && (sndGetPlayingState(temp_s1->Sound) != 0))
                {
                    sndDeactivate(temp_s1->Sound);
                }
            }

            // mips2c line 1595

            if (temp_s1->path != NULL)
            {
                sp478 = &g_CurrentSetup.pads[g_CurrentSetup.pathwaypoints[temp_s1->path->waypoints[temp_s1->nextstep]].padID].pos;

                sp47C = atan2f(sp478->f[0] - temp_s1->runtime_pos.f[0], sp478->f[2] - temp_s1->runtime_pos.f[2]);

                if (temp_s1->flags & 0x20000000)
                {
                    temp_s1->roty = sp47C;
                    temp_s1->flags &= 0xDFFFFFFF;
                    sub_GAME_7F044B38(temp_s1);
                }
            }
            else if (temp_s1->flags & 0x20000000)
            {
                temp_s1->roty = atan2f(temp_s1->mtx.m[2][0], temp_s1->mtx.m[2][2]);
                temp_s1->flags &= 0xDFFFFFFF;
                sub_GAME_7F044B38(temp_s1);
            }

            // mips2c line 1614
            if (temp_s1->speed > 0.0f)
            {
                sp430 = 0.0f;

                sp468 = arg0->stan;
                sp438 = temp_s1->roty;
                sp434 = temp_s1->turnrot60;

                if (sp478 != NULL)
                {
                    sp430 = 0.0f;
                    sp43C = sinf(sp438);
                    sp440 = 0.0f;
                    sp444 = cosf(temp_s1->roty);

                    if (chrlvGeometryRelated7F02FC34(&temp_s1->runtime_pos, (struct coord3d *) &sp43C, sp478, 10.0f) != 0)
                    {
                        sp47C = temp_s1->roty;
                    }
                }

                chrobjCallsApplySpeed(&temp_s1->roty, sp47C, &temp_s1->turnrot60, 0.000109083085f, 0.00021816617f, 0.006544985f);

                while (temp_s1->roty >= 6.2831855f)
                {
                    temp_s1->roty -= 6.2831855f;
                }

                while (temp_s1->roty < 0.0f)
                {
                    temp_s1->roty += 6.2831855f;
                }

                if (sp47C == temp_s1->roty)
                {
                    if ((temp_s1->turnrot60 <= 0.00021816617f) && (temp_s1->turnrot60 >= -0.00021816617f))
                    {
                        temp_s1->turnrot60 = 0.0f;
                    }
                }

                temp_s0_5 = (f32*)model->obj->Switches[3]->Data;

                if (g_GlobalTimerDelta > 0.0f)
                {
                    sp430 = (temp_s1->roty - sp438) / g_GlobalTimerDelta;
                }
                if (sp430 < 0.0f)
                {
                    sp430 += 6.2831855f;
                }

                sp460 = sinf(sp430) * (temp_s0_5[2] * model->scale) * g_GlobalTimerDelta;
                sp43C = sinf(temp_s1->roty);
                sp440 = 0.0f;
                sp444 = cosf(temp_s1->roty);
                sp694.f[0] = (temp_s1->runtime_pos.f[0] + (temp_s1->speed * g_GlobalTimerDelta * sp43C)) - (sp444 * sp460);
                sp694.f[1] = temp_s1->runtime_pos.f[1];
                sp694.f[2] = (sp43C * sp460) + (temp_s1->runtime_pos.f[2] + (temp_s1->speed * g_GlobalTimerDelta * sp444));

                if ((sub_GAME_7F0B0E24(&sp468, arg0->pos.f[0], arg0->pos.f[2], sp694.f[0], sp694.f[2], 0x1F, 0.0f, 1.0f, 0.0f, 1.0f) != 0)
                    && (sub_GAME_7F0B18B8(&sp468, sp694.f[0], sp694.f[2], 10.0f, 0x1F, 0.0f, 1.0f) < 0))
                {
                    sp44C = arg0->stan;
                    sp450.f[0] = arg0->pos.f[0];
                    sp450.f[1] = arg0->pos.f[1];
                    sp450.f[2] = arg0->pos.f[2];
                    arg0->stan = sp468;
                    arg0->pos.f[0] = sp694.f[0];
                    temp_s1->runtime_pos.f[0] = sp694.f[0];
                    arg0->pos.f[2] = sp694.f[2];
                    temp_s1->runtime_pos.f[2] = sp694.f[2];
                    chrobjCollisionRelated(obj);
                    setupUpdateObjectRoomPosition(obj);
                    var_s2_5 = sub_GAME_7F0448A8(arg0);

                    if (var_s2_5 != 0)
                    {
                        temp_v0_25 = (f32*)model->obj->Switches[7]->Data;
                        sp424 = temp_s1->mtx.m[0][0] * temp_v0_25[1];
                        sp42C = temp_s1->mtx.m[0][2] * temp_v0_25[1];
                        sp418 = temp_s1->mtx.m[0][0] * temp_v0_25[2];
                        sp420 = temp_s1->mtx.m[0][2] * temp_v0_25[2];
                        sp40C = temp_s1->mtx.m[2][0] * temp_v0_25[5];
                        sp414 = temp_s1->mtx.m[2][2] * temp_v0_25[5];
                        sp400 = temp_s1->mtx.m[2][0] * temp_v0_25[6];
                        sp408 = temp_s1->mtx.m[2][2] * temp_v0_25[6];
                        sp468 = arg0->stan;
                        // temp_f0_25 = arg0->pos.f[0]
                        //temp_f2_13 = arg0->pos.f[2]

                        // temp_f0_26 = arg0->pos.f[0];
                        // temp_f2_14 = arg0->pos.f[2]
                        // temp_f0_27 = arg0->pos.f[0] + sp418,
                        // temp_f2_15 = arg0->pos.f[2] + sp420
                        // temp_f0_28 = arg0->pos.f[0],
                        // temp_f2_16 = arg0->pos.f[2]
                        // temp_f0_29 = arg0->pos.f[0] + sp424,
                        // temp_f2_17 = arg0->pos.f[2] + sp42C,

                        if ((walkTilesBetweenPoints_NoCallback(&sp468, arg0->pos.f[0], arg0->pos.f[2], sp40C + (arg0->pos.f[0] + sp424), sp414 + (arg0->pos.f[2] + sp42C)) == 0)
                            || (walkTilesBetweenPoints_NoCallback(&sp468, sp40C + (arg0->pos.f[0] + sp424), sp414 + (arg0->pos.f[2] + sp42C), sp40C + (arg0->pos.f[0] + sp418), sp414 + (arg0->pos.f[2] + sp420)) == 0)
                            || (walkTilesBetweenPoints_NoCallback(&sp468, sp40C + arg0->pos.f[0] + sp418, sp414 + arg0->pos.f[2] + sp420, sp400 + arg0->pos.f[0] + sp418, sp408 + arg0->pos.f[2] + sp420) == 0)
                            || (walkTilesBetweenPoints_NoCallback(&sp468, sp400 + (arg0->pos.f[0] + sp418), sp408 + (arg0->pos.f[2] + sp420), sp400 + (arg0->pos.f[0] + sp424), sp408 + (arg0->pos.f[2] + sp42C)) == 0)
                            || (walkTilesBetweenPoints_NoCallback(&sp468, sp400 + arg0->pos.f[0] + sp424, sp408 + arg0->pos.f[2] + sp42C, sp40C + arg0->pos.f[0] + sp424, sp414 + arg0->pos.f[2] + sp42C) == 0))
                        {
                            var_s2_5 = 0;
                        }
                    }

                    if (var_s2_5 != 0)
                    {
                        sub_GAME_7F044B38(temp_s1);
                        sub_GAME_7F0402B4(arg0, &temp_s1->nextcol);
                        detonate_proxmine_In_range(&temp_s1->runtime_pos);

                        if ((sp478 != NULL)
                            && (chrlvIsArrivingLaterallyAtPos(&sp450, &sp694, sp478, 100.0f) != 0))
                        {
                            temp_s1->nextstep++;
                            if (temp_s1->path[temp_s1->nextstep].waypoints[0] < 0)
                            {
                                temp_s1->path = NULL;
                                temp_s1->speedaim = 0.0f;
                                temp_s1->speedtime60 = 60.0f;
                            }
                        }
                    }
                    else
                    {
                        if (temp_s1->speedtime60 < 0.0f)
                        {
                            temp_s1->speedaim = (f32) temp_s1->speed;
                            temp_s1->speedtime60 = 60.0f;
                        }

                        temp_s1->speed = 0.0f;
                        temp_s1->roty = sp438;
                        temp_s1->turnrot60 = sp434;
                        arg0->stan = sp44C;
                        arg0->pos.f[0] = sp450.f[0];
                        obj->runtime_pos.f[0] = sp450.f[0];
                        arg0->pos.f[1] = sp450.f[1];
                        obj->runtime_pos.f[1] = sp450.f[1];
                        arg0->pos.f[2] = sp450.f[2];
                        obj->runtime_pos.f[2] = sp450.f[2];

                        chrobjCollisionRelated(obj);
                        setupUpdateObjectRoomPosition(obj);
                    }
                }
                else
                {
                    if (temp_s1->speedtime60 < 0.0f)
                    {
                        temp_s1->speedaim = (f32) temp_s1->speed;
                        temp_s1->speedtime60 = 60.0f;
                    }

                    temp_s1->speed = 0.0f;
                    temp_s1->roty = sp438;
                    temp_s1->turnrot60 = sp434;
                }
            }
            else if (temp_s1->flags & 0x20000000)
            {
                temp_s1->roty = atan2f(temp_s1->mtx.m[2][0], temp_s1->mtx.m[2][2]);
                temp_s1->flags &= 0xDFFFFFFF;
                sub_GAME_7F044B38(temp_s1);
            }
        }
        // mips2c line 1754
        else if (obj->type == PROPDEF_AIRCRAFT)
        {
            struct AircraftRecord *temp_s1 = (struct AircraftRecord*)obj;

            ai((PropDefHeaderRecord*)temp_s1, 1);

            temp_s0_6 = temp_s1->model;

            if (temp_s0_6->anim != NULL)
            {
                setsuboffset(temp_s0_6, &temp_s1->runtime_pos);

                if (temp_s0_6->anim == animation_table_ptrs2[AIRCRAFT_ANIMATION_plane_runway])
                {
                    sub_GAME_7F06CE84(temp_s0_6, 10.438f);
                    setsubroty(temp_s1->model, 3.1415927f);
                }
                else if (bossGetStageNum() == LEVELID_STATUE)
                {
                    sub_GAME_7F06CE84(temp_s1->model, 1.0438f);
                    setsubroty(temp_s1->model, 2.3561945f);
                }
                else if (bossGetStageNum() == LEVELID_FRIGATE)
                {
                    sub_GAME_7F06CE84(temp_s1->model, 1.0438f);
                    setsubroty(temp_s1->model, 3.9269907f);
                }
                else
                {
                    sub_GAME_7F06CE84(temp_s1->model, 1.0438f);
                    setsubroty(temp_s1->model, 0.0f);
                }

                modelTickAnimQuarterSpeed(temp_s1->model, g_ClockTimer, 1);
                subcalcpos(temp_s1->model);
                getsuboffset(temp_s1->model, &temp_s1->runtime_pos);
                arg0->pos.f[0] = temp_s1->runtime_pos.f[0];
                arg0->pos.f[2] = temp_s1->runtime_pos.f[2];

                if (isNotBoundPad(temp_s1->pad))
                {
                    var_v1_4 = &g_CurrentSetup.pads[temp_s1->pad];
                }
                else
                {
                    var_v1_4 = (PadRecord *)&g_CurrentSetup.boundpads[getBoundPadNum(temp_s1->pad)];
                }

                arg0->pos.f[1] = temp_s1->runtime_pos.f[1] + var_v1_4->pos.f[1];
                temp_s1->runtime_pos.f[1] = arg0->pos.f[1];
                setsuboffset(temp_s1->model, &temp_s1->runtime_pos);
            }

            var_f12_6 = temp_s1->speedtime60;

            if (var_f12_6 >= 0.0f)
            {
                if (var_f12_6 <= g_GlobalTimerDelta)
                {
                    var_f12_6 = temp_s1->speedtime60;
                    temp_s1->speed = (f32) temp_s1->speedaim;
                }
                else
                {
                    temp_s1->speed += (((temp_s1->speedaim - temp_s1->speed) * g_GlobalTimerDelta) / var_f12_6);
                }
                temp_s1->speedtime60 = (f32) (var_f12_6 - g_GlobalTimerDelta);
            }

            if (temp_s1->rotaryspeedtime >= 0.0f)
            {
                if (temp_s1->rotaryspeedtime <= g_GlobalTimerDelta)
                {
                    temp_s1->rotaryspeed = (f32) temp_s1->rotaryspeedaim;
                }
                else
                {
                    var_f12_6 = temp_s1->rotaryspeed;
                    temp_s1->rotaryspeed += (((temp_s1->rotaryspeedaim - temp_s1->rotaryspeed) * g_GlobalTimerDelta) / temp_s1->rotaryspeedtime);
                }

                temp_s1->rotaryspeedtime -= g_GlobalTimerDelta;
            }

            var_s0_2 = 0;

            if (!(temp_s1->flags2 & 0x80000)
                && (objIsHealthy(obj) != 0)
                && (temp_s1->rotaryspeed != 0.0f)
                && !(temp_s1->flags & 0x20000000))
            {
                var_s0_2 = sub_GAME_7F053894(&temp_s1->runtime_pos, 5000.0f, 6000.0f);
            }

            if (var_s0_2 > 0)
            {
                if (((temp_s1->Sound == NULL) || (sndGetPlayingState(temp_s1->Sound) == 0)) && (lvlGetControlsLockedFlag() == 0))
                {
                    sndPlaySfx((struct ALBankAlt_s *) g_musicSfxBufferPtr, HELI_RUN_SFX, temp_s1->Sound);
                }

                if (temp_s1->Sound != NULL)
                {
                    sndCreatePostEvent(temp_s1->Sound, 8, var_s0_2);
                }
            }
            else
            {
                if ((temp_s1->Sound != NULL) && (sndGetPlayingState(temp_s1->Sound) != 0))
                {
                    sndDeactivate(temp_s1->Sound);
                }
            }
        }
    }

    // mips2c line 1860
    if (obj->type == PROPDEF_TINTED_GLASS)
    {
        struct TintedGlassRecord *tinted_glass = (struct TintedGlassRecord *)arg0->obj;

        tinted_glass->calculatedopacity = glassCalculateOpacity(
            &obj->runtime_pos,
            tinted_glass->TintDist,
            tinted_glass->CullDist,
            tinted_glass->unk90);

        if ((tinted_glass->unk8C >= 0) && (sp674 == 1))
        {
            if (tinted_glass->calculatedopacity == 0xFF)
            {
                bgToggleDataPortalsContrlBytes1Bit1(tinted_glass->unk8C, 0);
            }
            else
            {
                bgToggleDataPortalsContrlBytes1Bit1(tinted_glass->unk8C, 1);
            }
        }

        sp670 = 0;
    }
    // mips2c line 1879
    else if (obj->type == PROPDEF_DOOR && ((struct DoorRecord*)obj)->doorFlags & 0x2)
    {
        struct DoorRecord *door = (struct DoorRecord *)arg0->obj;

        var_s2_6 = 1;
        door->calculatedopacity = glassCalculateOpacity(&obj->runtime_pos, door->TintDist, door->CullDist, 0.0f);

        if (sp674 == 1)
        {
            if ((door->calculatedopacity != 0xFF) || (door->openPosition > 0.0f))
            {
                var_s2_6 = 0;
            }

            temp_v0_29 = model->obj;

            if ((&skeleton_door == temp_v0_29->Skeleton)
                && ((modelGetNodeRwData(model, temp_v0_29->Switches[0]))->Header.unk00 == 0))
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

    // mips2c line 1905
    if (obj->type == PROPDEF_TANK && (get_ptr_for_players_tank() == arg0))
    {
        var_v1_5 = 1;
    }
    else
    {
        if (obj->flags2 & 0x04000000)
        {
            var_v1_5 = 1;
        }
        else
        {
            var_v1_5 = 0;

            if (!(obj->runtime_bitflags & 0x800) && !(obj->flags2 & 0x80000))
            {
                var_v1_5 = sub_GAME_7F054D6C(arg0, &obj->runtime_pos, getinstsize(model), sp670);
            }
        }
    }

    // mips2c line 1926
    if (var_v1_5 != 0)
    {
        if (sp678 != 0)
        {
            update_color_shading(&obj->shadecol, &obj->nextcol);
        }

        arg0->flags |= 2;
        model->render_pos = dynAllocate(model->obj->numMatrices << 6);
        // model->render_pos = temp_v0_30

        if (obj->type == PROPDEF_DOOR)
        {
            sp39C = (struct DoorRecord *)arg0->obj;

            door7F0526EC(sp39C, (Mtxf *)model->render_pos);
            matrix_4x4_multiply_homogeneous_in_place(camGetWorldToScreenMtxf(), (Mtxf *)model->render_pos);

            temp_v0_31 = model->obj;

            if (&skeleton_eyelid_door == temp_v0_31->Skeleton)
            {
                temp_a1_4 = &model->render_pos[1].pos;
                temp_s0_10 = (Mtxf *)temp_v0_31->Switches[0]->Data;

                sp394 = 6.2831855f - ((sp39C->openPosition * 6.2831855f) / 360.0f);

                matrix_4x4_set_rotation_around_x(sp394, temp_a1_4);
                matrix_4x4_set_position((struct coord3d *)temp_s0_10, temp_a1_4);
                matrix_4x4_multiply_in_place((Mtxf *)model->render_pos, temp_a1_4);

                temp_a1_4 = &model->render_pos[2].pos;
                temp_s0_10 = (Mtxf *)temp_v0_31->Switches[1]->Data;

                matrix_4x4_set_rotation_around_x(6.2831855f - sp394, temp_a1_4);
                matrix_4x4_set_position((struct coord3d *)temp_s0_10, temp_a1_4);
                matrix_4x4_multiply_in_place((Mtxf *)model->render_pos, temp_a1_4);
            }
            else if (&skeleton_iris_door == temp_v0_31->Skeleton)
            {
                sp384 = (sp39C->openPosition * 6.2831855f) / 360.0f;

                temp_f0_31 = sp39C->maxFrac * 0.3f;
                if (temp_f0_31 < sp39C->openPosition)
                {
                    sp380 = (((sp39C->maxFrac * (sp39C->openPosition - temp_f0_31)) / (sp39C->maxFrac - temp_f0_31)) * 6.2831855f) / 360.0f;

                    if (sp678 != 0)
                    {
                        if (sp67C <= temp_f0_31)
                        {
                            chrobjSndCreatePostEventDefault(sndPlaySfx((struct ALBankAlt_s *) g_musicSfxBufferPtr, METAL_SLIDE_OPEN_SFX, NULL), &arg0->pos);
                        }
                    }
                }
                else
                {
                    sp380 = 0.0f;

                    if (sp678 != 0)
                    {
                        if (temp_f0_31 < sp67C)
                        {
                            chrobjSndCreatePostEventDefault(sndPlaySfx((struct ALBankAlt_s *) g_musicSfxBufferPtr, METAL_SLIDE_CLOSE_SFX, NULL), &arg0->pos);
                        }
                    }
                }

                for (sp38C = 0; sp38C < 6; sp38C++)
                {
                    temp_v0_32 = sp38C * 2;

                    sp390 = (Mtxf *)model->obj->Switches[temp_v0_32 + 1]->Data;
                    temp_a1_6 = &model->render_pos[temp_v0_32 + 1].pos;
                    matrix_4x4_set_rotation_around_z(sp380, temp_a1_6);
                    matrix_4x4_set_position((struct coord3d *)sp390, temp_a1_6);
                    matrix_4x4_multiply_in_place((Mtxf *)model->render_pos, temp_a1_6);

                    sp390 = (Mtxf *)model->obj->Switches[temp_v0_32 + 2]->Data;
                    temp_a1_6 = &model->render_pos[temp_v0_32 + 2].pos;
                    matrix_4x4_set_rotation_around_z(sp384, temp_a1_6);
                    matrix_4x4_set_position((struct coord3d *)sp390, temp_a1_6);
                    matrix_4x4_multiply_in_place(temp_a1_6, temp_a1_6);
                }
            }
        }
        // mips2c line 2018
        else
        {
            matrix_4x4_copy(&obj->mtx, &sp3A4);
            matrix_4x4_set_position(&obj->runtime_pos, &sp3A4);
            matrix_4x4_multiply_homogeneous(camGetWorldToScreenMtxf(), &sp3A4, model->render_pos);

            if (obj->type == PROPDEF_CCTV)
            {
                struct CCTVRecord *sp370 = (struct CCTVRecord *)arg0->obj;

                var_f12_7 = sp370->unkC8;
                temp_s0_13 = (struct coord3d *)model->obj->Switches[0]->Data;

                if (var_f12_7 < 0.0f)
                {
                    var_f12_7 += 6.2831855f;
                }
                else if (var_f12_7 >= 6.2831855f)
                {
                    var_f12_7 -= 6.2831855f;
                }

                matrix_4x4_set_rotation_around_y(var_f12_7, &sp320);

                matrix_4x4_multiply(&sp320, &sp370->unk84, &model->render_pos[1].pos);

                sp360.f[0] = temp_s0_13->f[0];
                sp360.f[1] = temp_s0_13->f[1];
                sp360.f[2] = temp_s0_13->f[2];

                mtx4TransformVecInPlace(&sp3A4, &sp360);
                matrix_4x4_set_position(&sp360, &model->render_pos[1].pos);
                matrix_4x4_multiply_homogeneous_in_place(camGetWorldToScreenMtxf(), &model->render_pos[1].pos);
            }
            // mips2c line 2050
            else if (obj->type == PROPDEF_AUTOGUN)
            {
                struct AutogunRecord *sp318 = (struct AutogunRecord *)arg0->obj;

                sp304 = sp318->unk90 + 1.5707964f;
                sp300 = -sp318->unk9C;
                if (sp304 >= 6.2831855f)
                {
                    sp304 -= 6.2831855f;
                }

                temp_s0_14 = model->obj->Switches[1]->Data;
                sp308.f[0] = temp_s0_14->f[0];
                sp308.f[1] = temp_s0_14->f[1];
                sp308.f[2] = temp_s0_14->f[2];
                mtx4TransformVecInPlace(&sp3A4, &sp308);
                matrix_4x4_set_rotation_around_y(sp304, &model->render_pos[1].pos);
                matrix_4x4_set_position(&sp308, &model->render_pos[1].pos);
                matrix_scalar_multiply(sp318->model->scale, sp6C->m[0]);
                matrix_4x4_multiply_homogeneous_in_place(camGetWorldToScreenMtxf(), sp6C);

                matrix_4x4_set_rotation_around_z(sp300, &model->render_pos[2].pos);
                matrix_4x4_set_position((struct coord3d*)model->obj->Switches[2]->Data, &model->render_pos[2].pos);
                matrix_4x4_multiply_homogeneous_in_place(sp6C, &model->render_pos[2].pos);

                if (model->obj->Switches[3] != NULL)
                {
                    sp2FC = modelFindNodeMtx(model, model->obj->Switches[3], 0);
                    matrix_4x4_set_rotation_around_x(sp318->unkB4, sp2FC);
                    matrix_4x4_set_position(model->obj->Switches[3]->Data, sp2FC);
                    matrix_4x4_multiply_homogeneous_in_place(model->obj->Switches[2]->Data, sp2FC);
                }

                if (model->obj->Switches[4] != NULL)
                {
                    sp2FC = modelFindNodeMtx(model, model->obj->Switches[4], 0);
                    matrix_4x4_set_identity_and_position(model->obj->Switches[4]->Data, sp2FC);
                    matrix_4x4_multiply_homogeneous_in_place(model->obj->Switches[2]->Data, sp2FC);
                }

                if (model->obj->Switches[7] != NULL)
                {
                    sp2FC = modelFindNodeMtx(model, model->obj->Switches[7], 0);
                    matrix_4x4_set_rotation_around_x(sp318->unkB4, sp2FC);
                    matrix_4x4_set_position(model->obj->Switches[7]->Data, sp2FC);
                    matrix_4x4_multiply_homogeneous_in_place(model->obj->Switches[2]->Data, sp2FC);
                }
            }
            // mips2c line 2065
            else if (obj->type == PROPDEF_COLLECTABLE)
            {
                for (sp2F8 = 1; sp2F8 < model->obj->numMatrices; sp2F8++)
                {
                    matrix_4x4_set_identity(&model->render_pos[sp2F8].pos);
                }
            }
            // mips2c line 2081
            else if (obj->type == PROPDEF_RACK)
            {
                matrix_4x4_set_identity_and_position(model->obj->Switches[0]->Data, &model->render_pos[1].pos);
                matrix_4x4_multiply_homogeneous_in_place(&model->render_pos[0].pos, &model->render_pos[1].pos);

                matrix_4x4_set_identity_and_position(model->obj->Switches[1]->Data, &model->render_pos[2].pos);
                matrix_4x4_multiply_homogeneous_in_place(&model->render_pos[0].pos, &model->render_pos[2].pos);

                matrix_4x4_set_identity_and_position(model->obj->Switches[2]->Data, &model->render_pos[3].pos);
                matrix_4x4_multiply_homogeneous_in_place(&model->render_pos[0].pos, &model->render_pos[3].pos);

                matrix_4x4_set_identity_and_position(model->obj->Switches[3]->Data, &model->render_pos[4].pos);
                matrix_4x4_multiply_homogeneous_in_place(&model->render_pos[0].pos, &model->render_pos[4].pos);
            }
            // mips2c line 2099
            else if (obj->type == PROPDEF_VEHICHLE)
            {
                struct VehichleRecord *temp_s1 = (struct VehichleRecord *)obj;

                var_f0_3 = 0.0f;

                temp_v1_7 = model->obj->Switches;
                sp260 = temp_v1_7[1]->Data;
                sp25C = temp_v1_7[2]->Data;
                sp258 = temp_v1_7[3]->Data;
                sp254 = temp_v1_7[4]->Data;
                temp_v0_40 = temp_v1_7[5]->Data;

                if (sp678 != 0)
                {
                    var_f0_3 = (temp_s1->speed * g_GlobalTimerDelta * 6.2831855f) / ((temp_v0_40[4] - temp_v0_40[3]) * model->scale * 6.2831855f * 0.5f);

                    temp_s1->wheelxrot += var_f0_3;

                    while (temp_s1->wheelxrot >= 6.2831855f)
                    {
                        temp_s1->wheelxrot -= 6.2831855f;
                    }

                    while (temp_s1->wheelxrot < 0.0f)
                    {
                        temp_s1->wheelxrot += 6.2831855f;
                    }
                }

                temp_s1->wheelxrot += var_f0_3;

                while (temp_s1->wheelxrot >= 6.2831855f)
                {
                    temp_s1->wheelxrot -= 6.2831855f;
                }

                while (temp_s1->wheelxrot < 0.0f)
                {
                    temp_s1->wheelxrot += 6.2831855f;
                }

                matrix_4x4_set_rotation_around_x(temp_s1->wheelxrot, &sp2AC);

                if (temp_s1->speed > 0.0f)
                {
                    sp24C = temp_s1->turnrot60;
                    sp250 = (sp258->f[2] - sp260->f[2]) * model->scale;
                    if (sp24C < 0.0f)
                    {
                        sp24C = -sp24C;
                    }

                    sp248 = sinf(sp24C) * sp250;
                    temp_s1->wheelyrot = atan2f(sp248, (cosf(sp24C) * sp250) - (sp250 - temp_s1->speed));

                    if (temp_s1->wheelyrot < sp24C)
                    {
                        temp_s1->wheelyrot = sp24C;
                    }

                    if (temp_s1->turnrot60 > 0.0f)
                    {
                        temp_s1->wheelyrot = (6.2831855f - temp_s1->wheelyrot);
                    }
                }

                matrix_4x4_set_rotation_around_y(temp_s1->wheelyrot, &sp26C);
                matrix_4x4_copy(&sp2AC, &model->render_pos[3].pos);
                matrix_4x4_set_position(sp258, &model->render_pos[3].pos);
                matrix_4x4_multiply_homogeneous_in_place(&model->render_pos[0].pos, &model->render_pos[3].pos);

                matrix_4x4_copy(&sp2AC, &model->render_pos[4].pos);
                matrix_4x4_set_position(sp254, &model->render_pos[4].pos);
                matrix_4x4_multiply_homogeneous_in_place(&model->render_pos[0].pos, &model->render_pos[4].pos);
                matrix_4x4_multiply_homogeneous_in_place(&sp26C, &sp2AC);

                matrix_4x4_copy(&sp2AC, &model->render_pos[1].pos);
                matrix_4x4_set_position(sp260, &model->render_pos[1].pos);
                matrix_4x4_multiply_homogeneous_in_place(&model->render_pos[0].pos, &model->render_pos[1].pos);

                matrix_4x4_copy(&sp2AC, &model->render_pos[2].pos);
                matrix_4x4_set_position(sp25C, &model->render_pos[2].pos);
                matrix_4x4_multiply_homogeneous_in_place(&model->render_pos[0].pos, &model->render_pos[2].pos);
            }
            // mips2c line 2187
            else if (obj->type == PROPDEF_AIRCRAFT)
            {
                struct AircraftRecord *temp_s1 = (struct AircraftRecord*)obj;

                //temp_a1_19 = model->render_pos[1];
                sp1FC = model->obj->Switches[2]->Data;

                if ((g_ClockTimer > 0) && (sp678 != 0))
                {
                    temp_s1->rotoryrot += temp_s1->rotaryspeed;

                    while (temp_s1->rotoryrot >= 6.2831855f)
                    {
                        temp_s1->rotoryrot -= 6.2831855f;
                    }

                    while (temp_s1->rotoryrot < 0.0f)
                    {
                        temp_s1->rotoryrot += 6.2831855f;
                    }
                }

                if (temp_s1->model->anim != NULL)
                {
                    sp1B0 = D_80030B34;

                    sp1B0.unk_matrix = camGetWorldToScreenMtxf();
                    sp1B0.mtxlist = &model->render_pos[0].pos;
                    subcalcmatrices(&sp1B0, temp_s1->model);
                }
                else
                {
                    matrix_4x4_copy(&model->render_pos[0].pos, &model->render_pos[1].pos);
                }

                if (temp_s1->flags & 0x20000000)
                {
                    matrix_4x4_set_rotation_around_z(temp_s1->rotoryrot, &sp200);
                }
                else
                {
                    matrix_4x4_set_rotation_around_y(temp_s1->rotoryrot, &sp200);
                }

                matrix_4x4_copy(&sp200, &model->render_pos[2].pos);
                matrix_4x4_set_position(sp1FC, &model->render_pos[2].pos);
                matrix_4x4_multiply_homogeneous_in_place(&model->render_pos[1].pos, &model->render_pos[2].pos);

                var_v1_7 = model->obj->Switches;

                if (var_v1_7[3] != NULL)
                {
                    temp_s0_21 = modelFindNodeMtx(model, var_v1_7[3], 0);
                    matrix_4x4_set_rotation_around_x(temp_s1->rotoryrot, &sp200);
                    matrix_4x4_copy(&sp200, temp_s0_21);
                    matrix_4x4_set_position(var_v1_7[3]->Data, temp_s0_21);
                    matrix_4x4_multiply_homogeneous_in_place(&model->render_pos[1].pos, temp_s0_21);
                }

                if (var_v1_7[4] != NULL)
                {
                    temp_s0_21 = modelFindNodeMtx(model, var_v1_7[4], 0);
                    matrix_4x4_set_identity_and_position(var_v1_7[4]->Data, temp_s0_21);
                    matrix_4x4_multiply_homogeneous_in_place(&model->render_pos[1].pos, temp_s0_21);
                }
            }
            // mips2c line 2258
            else
            {
                if (obj->type == PROPDEF_TANK)
                {
                    struct TankRecord *temp_s1 = (struct TankRecord*)obj;

                    temp_v1_8 = model->obj->Switches;

                    sp168 = temp_v1_8[1]->Data;
                    sp164 = temp_v1_8[3]->Data;
                    sp160 = temp_v1_8[4]->Data;
                    sp15C = temp_v1_8[2]->Data;
                    sp158 = temp_v1_8[6]->Data;

                    sp154 = -temp_s1->turret_vertical_angle;
                    if (sp154 < 0.0f)
                    {
                        sp154 += 6.2831855f;
                    }

                    var_f12_9 = -temp_s1->turret_orientation_angle;
                    if (var_f12_9 < 0.0f)
                    {
                        var_f12_9 += 6.2831855f;
                    }

                    matrix_4x4_set_rotation_around_y(var_f12_9, &model->render_pos[1].pos);
                    matrix_4x4_set_position(sp168, &model->render_pos[1].pos);
                    matrix_4x4_multiply_homogeneous_in_place(&model->render_pos[0].pos, &model->render_pos[1].pos);

                    matrix_4x4_set_rotation_around_x(sp154, &model->render_pos[3].pos);
                    matrix_4x4_set_position(sp164, &model->render_pos[3].pos);
                    matrix_4x4_multiply_homogeneous_in_place(sp6C, &model->render_pos[3].pos);

                    matrix_4x4_set_rotation_around_y(1.5707964f, &model->render_pos[4].pos);
                    matrix_4x4_set_position(sp160, &model->render_pos[4].pos);
                    matrix_4x4_multiply_homogeneous_in_place(&model->render_pos[3].pos, &model->render_pos[4].pos);

                    matrix_4x4_set_identity_and_position(sp15C, &model->render_pos[2].pos);
                    matrix_4x4_multiply_homogeneous_in_place(sp6C, &model->render_pos[2].pos);
                    matrix_4x4_multiply_homogeneous(currentPlayerGetMatrix10D4(), &model->render_pos[1].pos, &sp16C);

                    sub_GAME_7F03F540(sp158, &sp16C, &temp_s1->rect, temp_s1->unk80);

                    if (model->obj->Switches[7] != NULL)
                    {
                        modelGetNodeRwData(model, model->obj->Switches[7])->Header.unk00 = (s16) temp_s1->is_firing_tank;
                    }

                    if (model->obj->Switches[8] != NULL)
                    {
                        modelGetNodeRwData(model, model->obj->Switches[8])->Header.unk00 = 0;
                    }
                }
            }
        }

        modelUpdateRelationsQuick(model, model->obj->RootNode);
        arg0->Unk18 = -model->render_pos[0].pos.m[3][2];

        chrobjWeaponTick(arg0);

        sp684 = arg0->child;

        while (sp684 != NULL)
        {
            sub_GAME_7F0442DC(sp684);
            sp684 = sp684->prev;
        }
    }
    else
    {
        arg0->flags &= 0xFFFD;

        chrobjWeaponTick(arg0);

        sp684 = arg0->child;

        while (sp684 != NULL)
        {
            sub_GAME_7F04424C(sp684);
            sp684 = sp684->prev;
        }
    }

    if (obj->runtime_bitflags & 0x100)
    {
        obj->runtime_bitflags &= ~0x100;

        maybe_detonate_object(
            obj,
            (RANDOMGETNEXT_F32() * 4.0f) + 2.0f,
            &arg0->pos,
            ITEM_UNARMED,
            (s32) ((u32) (obj->runtime_bitflags & RUNTIMEBITFLAG_OWNER) >> RUNTIMEBITSHIFT_OWNER));
    }

    if (sp678 != 0)
    {
        if (obj->type == PROPDEF_DOOR)
        {
            sub_GAME_7F053A3C((struct DoorRecord *)arg0->obj);
        }
        // mimps2c line 2355
        else if ((obj->type == PROPDEF_AUTOGUN) && (lvlGetControlsLockedFlag() == 0))
        {
            struct AutogunRecord *autogun = (struct AutogunRecord*)arg0->obj;
            // autogun is temp_s0_24

            sp13C = 0;
            sp138 = 0;

            if ((autogun->is_active != 0) && !(obj->flags & 0x10000000))
            {
                autogun->unkAC = (autogun->unkAC + 1) & 1;

                sp13C = autogun->unkAC == 0;

                if (model->obj->Switches[5] != 0)
                {
                    sp138 = autogun->unkAC == 1;
                }

                if (autogun->unkC0 < g_GlobalTimer)
                {
                    if ((autogun->unkC4 != NULL) && (sndGetPlayingState(autogun->unkC4) != 0))
                    {
                        sndDeactivate(autogun->unkC4);
                    }

                    if ((autogun->unkC8 != NULL) && (sndGetPlayingState(autogun->unkC8) != 0))
                    {
                        sndDeactivate(autogun->unkC8);
                    }

                    if (autogun->unkC4 == NULL)
                    {
                        sndPlaySfx((struct ALBankAlt_s *) g_musicSfxBufferPtr, GUN_B9_CANNON_SHORT_SFX, autogun->unkC4);
                        chrobjSndCreatePostEventDefault(autogun->unkC4, &arg0->pos);
                    }
                    else if (autogun->unkC8 == NULL)
                    {
                        sndPlaySfx((struct ALBankAlt_s *) g_musicSfxBufferPtr, GUN_B9_CANNON_SHORT_SFX, autogun->unkC8);
                        chrobjSndCreatePostEventDefault(autogun->unkC8, &arg0->pos);
                    }

                    autogun->unkC0 = (s32) (g_GlobalTimer + 2);
                }

                // mmips2c line 2396
                if ((sp13C != 0) || (sp138 != 0))
                {
                    sp11C = 1;
                    sp10C = NULL;
                    sp108 = arg0->stan;
                    sp104 = (autogun->unkAC & 3) == 0;
                    sp100 = get_curplayer_positiondata();
                    var_a0_6 = 5;
                    temp_v1_10 = model->obj->Switches;

                    if ((model->obj->Switches[7] != 0) && !(autogun->unkAC & 7))
                    {
                        var_a0_6 = 7;
                    }

                    if ((arg0->flags & 2) && temp_v1_10[var_a0_6] != NULL)
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

                    sp110.f[0] = (sp120.f[0] * 65536.0f) + sp12C.f[0];
                    sp110.f[1] = (sp120.f[1] * 65536.0f) + sp12C.f[1];
                    sp110.f[2] = (sp120.f[2] * 65536.0f) + sp12C.f[2];

                    sub_GAME_7F0B1CC4();

                    // mmips2c line 2441
                    if (sub_GAME_7F0B0E24(&sp108, sp12C.f[0], sp12C.f[2], sp110.f[0], sp110.f[2], 2, 100.0f, 100.0f, 0.0f, 1.0f) == 0)
                    {
                        chrlvStanLineDirIntersection(&sp12C, &sp120, &sp110);
                        sp10C = sp108;
                        sp110.f[0] -= 26.0f * sp120.f[0];
                        sp110.f[1] -= 26.0f * sp120.f[1];
                        sp110.f[2] -= 26.0f * sp120.f[2];
                    }

                    if (g_GlobalTimer == (s32) autogun->unkBC)
                    {
                        temp_fc3d_a.f[0] = sp100->pos.f[0] - sp12C.f[0];
                        temp_fc3d_a.f[1] = sp100->pos.f[1] - sp12C.f[1];
                        temp_fc3d_a.f[2] = sp100->pos.f[2] - sp12C.f[2];

                        temp_f20_4 = (temp_fc3d_a.f[0] * temp_fc3d_a.f[0]) + (temp_fc3d_a.f[1] * temp_fc3d_a.f[1]) + (temp_fc3d_a.f[2] * temp_fc3d_a.f[2]);

                        temp_fc3d_b.f[0] = sp110.f[0] - sp12C.f[0];
                        temp_fc3d_b.f[1] = sp110.f[1] - sp12C.f[1];
                        temp_fc3d_b.f[2] = sp110.f[2] - sp12C.f[2];

                        if ((temp_f20_4 <= ((temp_fc3d_b.f[0] * temp_fc3d_b.f[0]) + (temp_fc3d_b.f[1] * temp_fc3d_b.f[1]) + (temp_fc3d_b.f[2] * temp_fc3d_b.f[2])))
                            && (bondviewGetIfCurrentPlayerDamageShowTime() == 0))
                        {
                            temp_f0_35 = sqrtf(temp_f20_4);
                            var_f2_7 = 0.16f * g_GlobalTimerDelta * F_80030B1C;

                            if (temp_f0_35 > 200.0f)
                            {
                                var_f2_7 *= 200.0f / temp_f0_35;
                            }

                            autogun->unkD4 += var_f2_7;

                            if (autogun->unkD4 >= 1.0f)
                            {
                                bondviewCallRecordDamageKills(bondwalkItemGetDestructionAmount(ITEM_FNP90) * 0.125f * F_80030B20, autogun->unk90, -1, 1);

                                autogun->unkD4 = 0.0f;

                                if (bondviewGetIfCurrentPlayerDamageShowTime() != 0)
                                {
                                    sp11C = 0;
                                }
                            }
                        }
                    }

                    // mips2c line 2479
                    if (sp11C != 0)
                    {
                        if (sp10C != NULL)
                        {
                            sub_GAME_7F0A3E1C(&sp110, 1, 26.0f, (s16) sp10C->room);
                        }

                        recall_joy2_hits_edit_flag(ITEM_FNP90, &sp110, -1);
                    }
                    else
                    {
                        sp110.f[0] = sp100->pos.f[0];
                        sp110.f[1] = sp100->pos.f[1];
                        sp110.f[2] = sp100->pos.f[2];

                        recall_joy2_hits_edit_detail_edit_flag(ITEM_FNP90, sp100, -1);
                    }

                    // mips2c line 2494
                    if (sp104 != 0)
                    {
                        beam = autogun->beam;

                        beam->from.f[0] = sp12C.f[0];
                        beam->from.f[1] = sp12C.f[1];
                        beam->from.f[2] = sp12C.f[2];

                        beam->dir.f[0] = sp110.f[0] - beam->from.f[0];
                        beam->dir.f[1] = sp110.f[1] - beam->from.f[1];
                        beam->dir.f[2] = sp110.f[2] - beam->from.f[2];

                        sp94 = sqrtf((beam->dir.f[2] * beam->dir.f[2]) + ((beam->from.f[0] * beam->from.f[0]) + (beam->from.f[1] * beam->from.f[1])));

                        temp_f2_23 = 1.0f / sp94;

                        beam->dir.f[0] = (f32) (beam->dir.f[0] * temp_f2_23);
                        beam->dir.f[1] = (f32) (beam->dir.f[1] * temp_f2_23);
                        beam->dir.f[2] = (f32) (beam->dir.f[2] * temp_f2_23);

                        if (sp94 > 10000.0f)
                        {
                            sp94 = 10000.0f;
                        }

                        beam->age = 0;
                        beam->weaponnum = ITEM_FNP90;
                        beam->maxdist = sp94;

                        if (sp94 < 500.0f)
                        {
                            sp94 = 500.0f;
                        }

                        /* At some point the developers where exploring a laser auto turret */
                        if (beam->weaponnum == ITEM_LASER)
                        {
                            beam->speed = 0.25f * sp94;
                            beam->mindist = 0.6f * sp94;

                            if (beam->mindist > 3000.0f)
                            {
                                beam->mindist = 3000.0f;
                            }

                            var_f4 = (-0.1f - (RANDOMGETNEXT_F32() * 0.3f)) * sp94;
                        }
                        else
                        {
                            beam->speed = 0.2f * sp94;
                            beam->mindist = 0.2f * sp94;

                            if (beam->mindist > 3000.0f)
                            {
                                beam->mindist = 3000.0f;
                            }

                            var_f4 = ((2.0f * RANDOMGETNEXT_F32()) - 1.0f) * beam->speed;
                        }

                        beam->dist = var_f4;
                    }
                }
            }

            // mmips2c line 2553
            if (model->obj->Switches[5] != NULL)
            {
                modelGetNodeRwData(model, model->obj->Switches[5])->Header.unk00 = (s16) sp13C;
            }

            if (model->obj->Switches[7] != NULL)
            {
                modelGetNodeRwData(model, model->obj->Switches[7])->Header.unk00 = (s16) sp138;
            }
        }

        objDropRecursively(arg0);
    }

    if (sp680 == 5)
    {
        arg0->stan = NULL;
    }

    return sp680;
}
#else

#ifdef VERSION_US

void _asmpp_func34(void) {






































































































































*(volatile float*)0 = -4.2140595129677775e+19f;


*(volatile float*)0 = -4.214059952772429e+19f;


*(volatile float*)0 = -4.21406039257708e+19f;


*(volatile float*)0 = -4.214060832381731e+19f;


*(volatile float*)0 = -4.214061272186382e+19f;


*(volatile float*)0 = -4.214061711991033e+19f;


*(volatile float*)0 = -4.214062151795684e+19f;


*(volatile float*)0 = -4.214062591600335e+19f;


*(volatile float*)0 = -4.214063031404986e+19f;


*(volatile float*)0 = -4.2140634712096375e+19f;



*(volatile float*)0 = -4.2140639110142886e+19f;


*(volatile float*)0 = -4.21406435081894e+19f;



*(volatile float*)0 = -4.214064790623591e+19f;


*(volatile float*)0 = -4.214065230428242e+19f;


*(volatile float*)0 = -4.214065670232893e+19f;




*(volatile float*)0 = -4.214066110037544e+19f;


*(volatile float*)0 = -4.214066549842195e+19f;



*(volatile float*)0 = -4.214066989646846e+19f;


*(volatile float*)0 = -4.2140674294514975e+19f;


*(volatile float*)0 = -4.2140678692561486e+19f;


*(volatile float*)0 = -4.2140683090608e+19f;


*(volatile float*)0 = -4.214068748865451e+19f;


*(volatile float*)0 = -4.214069188670102e+19f;



*(volatile float*)0 = -4.214069628474753e+19f;



*(volatile float*)0 = -4.214070068279404e+19f;


*(volatile float*)0 = -4.214070508084055e+19f;


*(volatile float*)0 = -4.214070947888706e+19f;


*(volatile float*)0 = -4.2140713876933575e+19f;


*(volatile float*)0 = -4.2140718274980086e+19f;



*(volatile float*)0 = -4.21407226730266e+19f;


*(volatile float*)0 = -4.214072707107311e+19f;


*(volatile float*)0 = -4.214073146911962e+19f;


*(volatile float*)0 = -4.214073586716613e+19f;


 } void _asmpp_large_func35(void) { 




*(volatile float*)0 = -4.214074026521264e+19f;


*(volatile float*)0 = -4.214074466325915e+19f;



*(volatile float*)0 = -4.214074906130566e+19f;


*(volatile float*)0 = -4.2140753459352175e+19f;


*(volatile float*)0 = -4.2140757857398686e+19f;


*(volatile float*)0 = -4.21407622554452e+19f;



*(volatile float*)0 = -4.214076665349171e+19f;


*(volatile float*)0 = -4.214077105153822e+19f;


*(volatile float*)0 = -4.214077544958473e+19f;


*(volatile float*)0 = -4.214077984763124e+19f;



*(volatile float*)0 = -4.214078424567775e+19f;


*(volatile float*)0 = -4.214078864372426e+19f;



*(volatile float*)0 = -4.2140793041770775e+19f;


*(volatile float*)0 = -4.2140797439817286e+19f;


*(volatile float*)0 = -4.21408018378638e+19f;


*(volatile float*)0 = -4.214080623591031e+19f;



*(volatile float*)0 = -4.214081063395682e+19f;


*(volatile float*)0 = -4.214081503200333e+19f;


*(volatile float*)0 = -4.214081943004984e+19f;


*(volatile float*)0 = -4.214082382809635e+19f;


*(volatile float*)0 = -4.214082822614286e+19f;


*(volatile float*)0 = -4.2140832624189374e+19f;


*(volatile float*)0 = -4.2140837022235886e+19f;


*(volatile float*)0 = -4.21408414202824e+19f;


*(volatile float*)0 = -4.214084581832891e+19f;


*(volatile float*)0 = -4.214085021637542e+19f;


*(volatile float*)0 = -4.214085461442193e+19f;


*(volatile float*)0 = -4.214085901246844e+19f;


*(volatile float*)0 = -4.214086341051495e+19f;


*(volatile float*)0 = -4.214086780856146e+19f;


*(volatile float*)0 = -4.2140872206607974e+19f;


*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func36(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func37(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func38(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func39(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func40(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func41(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func42(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func43(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func44(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func45(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func46(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func47(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func48(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func49(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func50(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func51(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func52(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func53(void) { 



*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func54(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func55(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func56(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func57(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func58(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func59(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func60(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func61(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func62(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func63(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
 } void _asmpp_large_func64(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func65(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func66(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func67(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func68(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func69(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func70(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

 } void _asmpp_large_func71(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

 } void _asmpp_large_func72(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func73(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

 } void _asmpp_large_func74(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

 } void _asmpp_large_func75(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func76(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func77(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func78(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func79(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
#endif

#ifdef VERSION_JP

void _asmpp_func80(void) {






































































































































*(volatile float*)0 = -4.2140876604654486e+19f;


*(volatile float*)0 = -4.2140881002701e+19f;


*(volatile float*)0 = -4.214088540074751e+19f;


*(volatile float*)0 = -4.214088979879402e+19f;


*(volatile float*)0 = -4.214089419684053e+19f;


*(volatile float*)0 = -4.214089859488704e+19f;


*(volatile float*)0 = -4.214090299293355e+19f;


*(volatile float*)0 = -4.214090739098006e+19f;


*(volatile float*)0 = -4.2140911789026574e+19f;


*(volatile float*)0 = -4.2140916187073085e+19f;



*(volatile float*)0 = -4.21409205851196e+19f;


*(volatile float*)0 = -4.214092498316611e+19f;



*(volatile float*)0 = -4.214092938121262e+19f;


*(volatile float*)0 = -4.214093377925913e+19f;


*(volatile float*)0 = -4.214093817730564e+19f;




*(volatile float*)0 = -4.214094257535215e+19f;


*(volatile float*)0 = -4.214094697339866e+19f;



*(volatile float*)0 = -4.2140951371445174e+19f;


*(volatile float*)0 = -4.2140955769491685e+19f;


*(volatile float*)0 = -4.21409601675382e+19f;


*(volatile float*)0 = -4.214096456558471e+19f;


*(volatile float*)0 = -4.214096896363122e+19f;


*(volatile float*)0 = -4.214097336167773e+19f;


*(volatile float*)0 = -4.214097775972424e+19f;


*(volatile float*)0 = -4.214098215777075e+19f;




*(volatile float*)0 = -4.214098655581726e+19f;


*(volatile float*)0 = -4.2140990953863774e+19f;


*(volatile float*)0 = -4.2140995351910285e+19f;


*(volatile float*)0 = -4.21409997499568e+19f;


*(volatile float*)0 = -4.214100414800331e+19f;


*(volatile float*)0 = -4.214100854604982e+19f;



*(volatile float*)0 = -4.214101294409633e+19f;


*(volatile float*)0 = -4.214101734214284e+19f;


 } void _asmpp_large_func81(void) { 



*(volatile float*)0 = -4.214102174018935e+19f;


*(volatile float*)0 = -4.214102613823586e+19f;



*(volatile float*)0 = -4.2141030536282374e+19f;



*(volatile float*)0 = -4.2141034934328885e+19f;


*(volatile float*)0 = -4.21410393323754e+19f;


*(volatile float*)0 = -4.214104373042191e+19f;


*(volatile float*)0 = -4.214104812846842e+19f;


*(volatile float*)0 = -4.214105252651493e+19f;



*(volatile float*)0 = -4.214105692456144e+19f;


*(volatile float*)0 = -4.214106132260795e+19f;


*(volatile float*)0 = -4.214106572065446e+19f;


*(volatile float*)0 = -4.214107011870097e+19f;



*(volatile float*)0 = -4.2141074516747485e+19f;



*(volatile float*)0 = -4.2141078914794e+19f;


*(volatile float*)0 = -4.214108331284051e+19f;


*(volatile float*)0 = -4.214108771088702e+19f;


*(volatile float*)0 = -4.214109210893353e+19f;


*(volatile float*)0 = -4.214109650698004e+19f;



*(volatile float*)0 = -4.214110090502655e+19f;


*(volatile float*)0 = -4.214110530307306e+19f;


*(volatile float*)0 = -4.214110970111957e+19f;


*(volatile float*)0 = -4.2141114099166085e+19f;


*(volatile float*)0 = -4.21411184972126e+19f;


*(volatile float*)0 = -4.214112289525911e+19f;


*(volatile float*)0 = -4.214112729330562e+19f;


*(volatile float*)0 = -4.214113169135213e+19f;


*(volatile float*)0 = -4.214113608939864e+19f;


*(volatile float*)0 = -4.214114048744515e+19f;


*(volatile float*)0 = -4.214114488549166e+19f;


*(volatile float*)0 = -4.214114928353817e+19f;


*(volatile float*)0 = -4.2141153681584685e+19f;


*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func82(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

 } void _asmpp_large_func83(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func84(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func85(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func86(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func87(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func88(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

 } void _asmpp_large_func89(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
 } void _asmpp_large_func90(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func91(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func92(void) { 



*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

 } void _asmpp_large_func93(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func94(void) { 



*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func95(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func96(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func97(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func98(void) { 



*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func99(void) { 





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func100(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func101(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

 } void _asmpp_large_func102(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func103(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func104(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func105(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func106(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func107(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func108(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func109(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func110(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func111(void) { 






*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func112(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func113(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func114(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func115(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func116(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func117(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func118(void) { 




*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func119(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
 } void _asmpp_large_func120(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func121(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func122(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func123(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func124(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func125(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func126(void) { 



*(volatile int*)0 = 0;
}
#endif

#ifdef VERSION_EU

void _asmpp_func127(void) {






































































































































*(volatile float*)0 = -4.21411580796312e+19f;


*(volatile float*)0 = -4.214116247767771e+19f;


*(volatile float*)0 = -4.214116687572422e+19f;


*(volatile float*)0 = -4.214117127377073e+19f;


*(volatile float*)0 = -4.214117567181724e+19f;


*(volatile float*)0 = -4.214118006986375e+19f;


*(volatile float*)0 = -4.214118446791026e+19f;


*(volatile float*)0 = -4.214118886595677e+19f;


*(volatile float*)0 = -4.2141193264003285e+19f;


*(volatile float*)0 = -4.21411976620498e+19f;



*(volatile float*)0 = -4.214120206009631e+19f;


*(volatile float*)0 = -4.214120645814282e+19f;



*(volatile float*)0 = -4.214121085618933e+19f;


*(volatile float*)0 = -4.214121525423584e+19f;


*(volatile float*)0 = -4.214121965228235e+19f;




*(volatile float*)0 = -4.214122405032886e+19f;


*(volatile float*)0 = -4.214122844837537e+19f;



*(volatile float*)0 = -4.2141232846421885e+19f;


*(volatile float*)0 = -4.21412372444684e+19f;


*(volatile float*)0 = -4.214124164251491e+19f;


*(volatile float*)0 = -4.214124604056142e+19f;


*(volatile float*)0 = -4.214125043860793e+19f;


*(volatile float*)0 = -4.214125483665444e+19f;


*(volatile float*)0 = -4.214125923470095e+19f;


*(volatile float*)0 = -4.214126363274746e+19f;




*(volatile float*)0 = -4.214126803079397e+19f;


*(volatile float*)0 = -4.2141272428840485e+19f;


*(volatile float*)0 = -4.2141276826887e+19f;


*(volatile float*)0 = -4.214128122493351e+19f;


*(volatile float*)0 = -4.214128562298002e+19f;


*(volatile float*)0 = -4.214129002102653e+19f;



*(volatile float*)0 = -4.214129441907304e+19f;


*(volatile float*)0 = -4.214129881711955e+19f;


 } void _asmpp_large_func128(void) { 



*(volatile float*)0 = -4.214130321516606e+19f;


*(volatile float*)0 = -4.214130761321257e+19f;



*(volatile float*)0 = -4.2141312011259085e+19f;



*(volatile float*)0 = -4.21413164093056e+19f;


*(volatile float*)0 = -4.214132080735211e+19f;


*(volatile float*)0 = -4.214132520539862e+19f;


*(volatile float*)0 = -4.214132960344513e+19f;


*(volatile float*)0 = -4.214133400149164e+19f;



*(volatile float*)0 = -4.214133839953815e+19f;


*(volatile float*)0 = -4.214134279758466e+19f;


*(volatile float*)0 = -4.214134719563117e+19f;


*(volatile float*)0 = -4.2141351593677685e+19f;



*(volatile float*)0 = -4.2141355991724196e+19f;



*(volatile float*)0 = -4.214136038977071e+19f;


*(volatile float*)0 = -4.214136478781722e+19f;


*(volatile float*)0 = -4.214136918586373e+19f;


*(volatile float*)0 = -4.214137358391024e+19f;


*(volatile float*)0 = -4.214137798195675e+19f;



*(volatile float*)0 = -4.214138238000326e+19f;


*(volatile float*)0 = -4.214138677804977e+19f;


*(volatile float*)0 = -4.2141391176096285e+19f;


*(volatile float*)0 = -4.2141395574142796e+19f;


*(volatile float*)0 = -4.214139997218931e+19f;


*(volatile float*)0 = -4.214140437023582e+19f;


*(volatile float*)0 = -4.214140876828233e+19f;


*(volatile float*)0 = -4.214141316632884e+19f;


*(volatile float*)0 = -4.214141756437535e+19f;


*(volatile float*)0 = -4.214142196242186e+19f;


*(volatile float*)0 = -4.214142636046837e+19f;


*(volatile float*)0 = -4.2141430758514885e+19f;


*(volatile float*)0 = -4.2141435156561396e+19f;


*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func129(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

 } void _asmpp_large_func130(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func131(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func132(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func133(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func134(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func135(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

 } void _asmpp_large_func136(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
 } void _asmpp_large_func137(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func138(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func139(void) { 



*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func140(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func141(void) { 





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func142(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func143(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func144(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func145(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func146(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func147(void) { 





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func148(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func149(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func150(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func151(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func152(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func153(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func154(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func155(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
 } void _asmpp_large_func156(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

 } void _asmpp_large_func157(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func158(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func159(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func160(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func161(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func162(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func163(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func164(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
 } void _asmpp_large_func165(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func166(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
 } void _asmpp_large_func167(void) { 




*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func168(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func169(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func170(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func171(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func172(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func173(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
#endif


#endif


#ifdef NONMATCHING
Gfx * sub_GAME_7F049B58(Gfx *arg0) {

}
#else

void _asmpp_func174(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
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

void _asmpp_func175(void) {







































*(volatile float*)0 = -4.214143955460791e+19f;


*(volatile float*)0 = -4.214144395265442e+19f;


*(volatile float*)0 = -4.214144835070093e+19f;


*(volatile float*)0 = -4.214145274874744e+19f;


*(volatile float*)0 = -4.214145714679395e+19f;


*(volatile float*)0 = -4.214146154484046e+19f;


*(volatile float*)0 = -4.214146594288697e+19f;


*(volatile float*)0 = -4.2141470340933485e+19f;


*(volatile float*)0 = -4.2141474738979996e+19f;


*(volatile float*)0 = -4.214147913702651e+19f;


*(volatile float*)0 = -4.214148353507302e+19f;


*(volatile float*)0 = -4.214148793311953e+19f;


*(volatile float*)0 = -4.214149233116604e+19f;



*(volatile float*)0 = -4.214149672921255e+19f;


*(volatile float*)0 = -4.214150112725906e+19f;


*(volatile float*)0 = -4.214150552530557e+19f;


*(volatile float*)0 = -4.2141509923352084e+19f;



*(volatile float*)0 = -4.2141514321398596e+19f;


*(volatile float*)0 = -4.214151871944511e+19f;



*(volatile float*)0 = -4.214152311749162e+19f;


*(volatile float*)0 = -4.214152751553813e+19f;


*(volatile float*)0 = -4.214153191358464e+19f;



*(volatile float*)0 = -4.214153631163115e+19f;


*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func176(void) { 



*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func177(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func178(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;


*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func179(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
 } void _asmpp_large_func180(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func181(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func182(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func183(void) { 





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
#endif
#ifdef VERSION_JP

void _asmpp_func184(void) {







































*(volatile float*)0 = -4.214154070967766e+19f;


*(volatile float*)0 = -4.214154510772417e+19f;


*(volatile float*)0 = -4.2141549505770684e+19f;


*(volatile float*)0 = -4.2141553903817196e+19f;


*(volatile float*)0 = -4.214155830186371e+19f;


*(volatile float*)0 = -4.214156269991022e+19f;


*(volatile float*)0 = -4.214156709795673e+19f;


*(volatile float*)0 = -4.214157149600324e+19f;


*(volatile float*)0 = -4.214157589404975e+19f;


*(volatile float*)0 = -4.214158029209626e+19f;


*(volatile float*)0 = -4.214158469014277e+19f;


*(volatile float*)0 = -4.2141589088189284e+19f;


*(volatile float*)0 = -4.2141593486235795e+19f;



*(volatile float*)0 = -4.214159788428231e+19f;


*(volatile float*)0 = -4.214160228232882e+19f;


*(volatile float*)0 = -4.214160668037533e+19f;


*(volatile float*)0 = -4.214161107842184e+19f;



*(volatile float*)0 = -4.214161547646835e+19f;


*(volatile float*)0 = -4.214161987451486e+19f;



*(volatile float*)0 = -4.214162427256137e+19f;


*(volatile float*)0 = -4.2141628670607884e+19f;


*(volatile float*)0 = -4.2141633068654395e+19f;



*(volatile float*)0 = -4.214163746670091e+19f;


*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func185(void) { 



*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func186(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func187(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;


*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func188(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
 } void _asmpp_large_func189(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func190(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func191(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func192(void) { 





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
#endif
#ifdef VERSION_EU

void _asmpp_func193(void) {







































*(volatile float*)0 = -4.214164186474742e+19f;


*(volatile float*)0 = -4.214164626279393e+19f;


*(volatile float*)0 = -4.214165066084044e+19f;


*(volatile float*)0 = -4.214165505888695e+19f;


*(volatile float*)0 = -4.214165945693346e+19f;


*(volatile float*)0 = -4.214166385497997e+19f;


*(volatile float*)0 = -4.2141668253026484e+19f;


*(volatile float*)0 = -4.2141672651072995e+19f;


*(volatile float*)0 = -4.214167704911951e+19f;


*(volatile float*)0 = -4.214168144716602e+19f;


*(volatile float*)0 = -4.214168584521253e+19f;


*(volatile float*)0 = -4.214169024325904e+19f;


*(volatile float*)0 = -4.214169464130555e+19f;



*(volatile float*)0 = -4.214169903935206e+19f;


*(volatile float*)0 = -4.214170343739857e+19f;


*(volatile float*)0 = -4.2141707835445084e+19f;


*(volatile float*)0 = -4.2141712233491595e+19f;



*(volatile float*)0 = -4.214171663153811e+19f;


*(volatile float*)0 = -4.214172102958462e+19f;



*(volatile float*)0 = -4.214172542763113e+19f;


*(volatile float*)0 = -4.214172982567764e+19f;


*(volatile float*)0 = -4.214173422372415e+19f;



*(volatile float*)0 = -4.214173862177066e+19f;


*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func194(void) { 



*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func195(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func196(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;


*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func197(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
 } void _asmpp_large_func198(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func199(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func200(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func201(void) { 





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
#endif

#endif




#ifdef NONMATCHING
void sub_GAME_7F04AC20(void) {

}
#else

void _asmpp_func202(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func203(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func204(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func205(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
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

void _asmpp_func206(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func207(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func208(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func209(void) { 





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func210(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
#endif

#ifdef VERSION_EU

void _asmpp_func211(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func212(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func213(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func214(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func215(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
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

void _asmpp_func216(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
 } void _asmpp_large_func217(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
 } void _asmpp_large_func218(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
#endif

#ifdef VERSION_EU

void _asmpp_func219(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func220(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func221(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
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

void _asmpp_func222(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func223(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func224(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func225(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func226(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func227(void) { 




*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
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

void _asmpp_func228(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
#endif





#ifdef NONMATCHING

#else

void _asmpp_func229(void) {






*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
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

void _asmpp_func230(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func231(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func232(void) { 



*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
 } void _asmpp_large_func233(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
#endif





#ifdef NONMATCHING
void sub_GAME_7F04E720(PropRecord* prop, struct ShotData* shotdata) {

}
#else
void sub_GAME_7F04E720(PropRecord*, struct ShotData*);

void _asmpp_func234(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func235(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
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

void _asmpp_func236(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
 } void _asmpp_large_func237(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func238(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
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


void append_text_picked_up(u8 *buffer, AMMOTYPE param2, u32 param3)
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

void _asmpp_func239(void) {






















































*(volatile float*)0 = -4.214174301981717e+19f;


*(volatile float*)0 = -4.214174741786368e+19f;


*(volatile float*)0 = -4.2141751815910195e+19f;


*(volatile float*)0 = -4.214175621395671e+19f;


*(volatile float*)0 = -4.214176061200322e+19f;



*(volatile float*)0 = -4.214176501004973e+19f;



*(volatile float*)0 = -4.214176940809624e+19f;


*(volatile float*)0 = -4.214177380614275e+19f;


*(volatile float*)0 = -4.214177820418926e+19f;



*(volatile float*)0 = -4.214178260223577e+19f;


*(volatile float*)0 = -4.214178700028228e+19f;


*(volatile float*)0 = -4.2141791398328795e+19f;


*(volatile float*)0 = -4.214179579637531e+19f;


*(volatile float*)0 = -4.214180019442182e+19f;




*(volatile float*)0 = -4.214180459246833e+19f;



*(volatile float*)0 = -4.214180899051484e+19f;


*(volatile float*)0 = -4.214181338856135e+19f;


*(volatile float*)0 = -4.214181778660786e+19f;



*(volatile float*)0 = -4.214182218465437e+19f;



*(volatile float*)0 = -4.214182658270088e+19f;


*(volatile float*)0 = -4.2141830980747395e+19f;



*(volatile float*)0 = -4.214183537879391e+19f;


*(volatile float*)0 = -4.214183977684042e+19f;


*(volatile float*)0 = -4.214184417488693e+19f;


*(volatile float*)0 = -4.214184857293344e+19f;


*(volatile float*)0 = -4.214185297097995e+19f;


*(volatile float*)0 = -4.214185736902646e+19f;



*(volatile float*)0 = -4.214186176707297e+19f;


*(volatile float*)0 = -4.214186616511948e+19f;


*(volatile float*)0 = -4.2141870563165995e+19f;


*(volatile float*)0 = -4.214187496121251e+19f;


*(volatile float*)0 = -4.214187935925902e+19f;



*(volatile float*)0 = -4.214188375730553e+19f;


 } void _asmpp_large_func240(void) { 



*(volatile float*)0 = -4.214188815535204e+19f;


*(volatile float*)0 = -4.214189255339855e+19f;



*(volatile float*)0 = -4.214189695144506e+19f;


*(volatile float*)0 = -4.214190134949157e+19f;




*(volatile float*)0 = -4.214190574753808e+19f;


*(volatile float*)0 = -4.2141910145584595e+19f;


*(volatile float*)0 = -4.214191454363111e+19f;


*(volatile float*)0 = -4.214191894167762e+19f;



*(volatile float*)0 = -4.214192333972413e+19f;


*(volatile float*)0 = -4.214192773777064e+19f;


*(volatile float*)0 = -4.214193213581715e+19f;



*(volatile float*)0 = -4.214193653386366e+19f;


*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func241(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;


*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
#endif
#ifdef VERSION_JP

void _asmpp_func242(void) {






















































*(volatile float*)0 = -4.214194093191017e+19f;


*(volatile float*)0 = -4.214194532995668e+19f;


*(volatile float*)0 = -4.2141949728003195e+19f;


*(volatile float*)0 = -4.214195412604971e+19f;


*(volatile float*)0 = -4.214195852409622e+19f;



*(volatile float*)0 = -4.214196292214273e+19f;



*(volatile float*)0 = -4.214196732018924e+19f;


*(volatile float*)0 = -4.214197171823575e+19f;


*(volatile float*)0 = -4.214197611628226e+19f;



*(volatile float*)0 = -4.214198051432877e+19f;


*(volatile float*)0 = -4.214198491237528e+19f;


*(volatile float*)0 = -4.2141989310421795e+19f;


*(volatile float*)0 = -4.214199370846831e+19f;


*(volatile float*)0 = -4.214199810651482e+19f;




*(volatile float*)0 = -4.214200250456133e+19f;



*(volatile float*)0 = -4.214200690260784e+19f;


*(volatile float*)0 = -4.214201130065435e+19f;


*(volatile float*)0 = -4.214201569870086e+19f;



*(volatile float*)0 = -4.214202009674737e+19f;



*(volatile float*)0 = -4.214202449479388e+19f;


*(volatile float*)0 = -4.2142028892840395e+19f;



*(volatile float*)0 = -4.2142033290886906e+19f;


*(volatile float*)0 = -4.214203768893342e+19f;


*(volatile float*)0 = -4.214204208697993e+19f;


*(volatile float*)0 = -4.214204648502644e+19f;


*(volatile float*)0 = -4.214205088307295e+19f;


*(volatile float*)0 = -4.214205528111946e+19f;



*(volatile float*)0 = -4.214205967916597e+19f;


*(volatile float*)0 = -4.214206407721248e+19f;


*(volatile float*)0 = -4.2142068475258995e+19f;


*(volatile float*)0 = -4.2142072873305506e+19f;


*(volatile float*)0 = -4.214207727135202e+19f;



*(volatile float*)0 = -4.214208166939853e+19f;


 } void _asmpp_large_func243(void) { 



*(volatile float*)0 = -4.214208606744504e+19f;


*(volatile float*)0 = -4.214209046549155e+19f;



*(volatile float*)0 = -4.214209486353806e+19f;


*(volatile float*)0 = -4.214209926158457e+19f;




*(volatile float*)0 = -4.214210365963108e+19f;


*(volatile float*)0 = -4.2142108057677595e+19f;


*(volatile float*)0 = -4.2142112455724106e+19f;


*(volatile float*)0 = -4.214211685377062e+19f;



*(volatile float*)0 = -4.214212125181713e+19f;


*(volatile float*)0 = -4.214212564986364e+19f;


*(volatile float*)0 = -4.214213004791015e+19f;



*(volatile float*)0 = -4.214213444595666e+19f;


*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func244(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;


*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
#endif

#ifdef VERSION_EU

void _asmpp_func245(void) {






















































*(volatile float*)0 = -4.214213884400317e+19f;


*(volatile float*)0 = -4.214214324204968e+19f;


*(volatile float*)0 = -4.2142147640096195e+19f;


*(volatile float*)0 = -4.2142152038142706e+19f;


*(volatile float*)0 = -4.214215643618922e+19f;



*(volatile float*)0 = -4.214216083423573e+19f;



*(volatile float*)0 = -4.214216523228224e+19f;


*(volatile float*)0 = -4.214216963032875e+19f;


*(volatile float*)0 = -4.214217402837526e+19f;



*(volatile float*)0 = -4.214217842642177e+19f;


*(volatile float*)0 = -4.214218282446828e+19f;


*(volatile float*)0 = -4.2142187222514794e+19f;


*(volatile float*)0 = -4.2142191620561306e+19f;


*(volatile float*)0 = -4.214219601860782e+19f;




*(volatile float*)0 = -4.214220041665433e+19f;



*(volatile float*)0 = -4.214220481470084e+19f;


*(volatile float*)0 = -4.214220921274735e+19f;


*(volatile float*)0 = -4.214221361079386e+19f;



*(volatile float*)0 = -4.214221800884037e+19f;



*(volatile float*)0 = -4.214222240688688e+19f;


*(volatile float*)0 = -4.2142226804933394e+19f;



*(volatile float*)0 = -4.2142231202979906e+19f;


*(volatile float*)0 = -4.214223560102642e+19f;


*(volatile float*)0 = -4.214223999907293e+19f;


*(volatile float*)0 = -4.214224439711944e+19f;


*(volatile float*)0 = -4.214224879516595e+19f;


*(volatile float*)0 = -4.214225319321246e+19f;



*(volatile float*)0 = -4.214225759125897e+19f;


*(volatile float*)0 = -4.214226198930548e+19f;


*(volatile float*)0 = -4.2142266387351994e+19f;


*(volatile float*)0 = -4.2142270785398505e+19f;


*(volatile float*)0 = -4.214227518344502e+19f;



*(volatile float*)0 = -4.214227958149153e+19f;


 } void _asmpp_large_func246(void) { 



*(volatile float*)0 = -4.214228397953804e+19f;


*(volatile float*)0 = -4.214228837758455e+19f;



*(volatile float*)0 = -4.214229277563106e+19f;


*(volatile float*)0 = -4.214229717367757e+19f;




*(volatile float*)0 = -4.214230157172408e+19f;


*(volatile float*)0 = -4.2142305969770594e+19f;


*(volatile float*)0 = -4.2142310367817105e+19f;


*(volatile float*)0 = -4.214231476586362e+19f;



*(volatile float*)0 = -4.214231916391013e+19f;


*(volatile float*)0 = -4.214232356195664e+19f;


*(volatile float*)0 = -4.214232796000315e+19f;



*(volatile float*)0 = -4.214233235804966e+19f;


*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func247(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;


*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
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

void _asmpp_func248(void) {












*(volatile float*)0 = -4.214233675609617e+19f;


*(volatile float*)0 = -4.214234115414268e+19f;


*(volatile float*)0 = -4.2142345552189194e+19f;


*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func249(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func250(void) { 



*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func251(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
#endif

#ifdef VERSION_JP

void _asmpp_func252(void) {












*(volatile float*)0 = -4.2142349950235705e+19f;


*(volatile float*)0 = -4.214235434828222e+19f;


*(volatile float*)0 = -4.214235874632873e+19f;


*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func253(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

 } void _asmpp_large_func254(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func255(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func256(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
#endif

#ifdef VERSION_EU

void _asmpp_func257(void) {












*(volatile float*)0 = -4.214236314437524e+19f;


*(volatile float*)0 = -4.214236754242175e+19f;


*(volatile float*)0 = -4.214237194046826e+19f;


*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func258(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

 } void _asmpp_large_func259(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func260(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func261(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
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

void _asmpp_func262(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
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

void _asmpp_func263(void) {








*(volatile float*)0 = -4.214237633851477e+19f;


*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
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

void _asmpp_func264(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func265(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func266(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func267(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
 } void _asmpp_large_func268(void) { 




*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
 } void _asmpp_large_func269(void) { 



*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
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

void _asmpp_func270(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
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

void _asmpp_func271(void) {





*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;

*(volatile int*)0 = 0;

*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
*(volatile int*)0 = 0;
}
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

