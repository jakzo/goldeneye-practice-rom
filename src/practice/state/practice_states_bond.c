#include "practice_states_bond.h"
#include "practice_states_globals.h"
#include "bondinv.h"
#include "bondview.h"
#include "watch.h"
#include "chr.h"
#include "gun.h"
#include "player.h"
#include "player_2.h"
#include "practice_ui.h"
#include "objecthandler.h"
#include "chrai.h"
#include <bondconstants.h>
#include <bondgame.h>
#include <joy.h>
#include <music.h>
#include <os_extension.h>
#include <snd.h>
#include <ultra64.h>

extern void used_to_load_1st_person_model_on_demand(enum GUNHAND hand);

extern void bondviewApplyVertaTheta(void);
extern void chrpropDelist(PropRecord *prop);
extern PropRecord pos_data_entry[];
extern int bondinvAddPropToInv(PropRecord *prop);
extern s32 g_EnterTankAudioState;

static s32 get_prop_index(PropRecord *prop) {
    if (prop == NULL) {
        return -1;
    }
    return prop - pos_data_entry;
}

static PropRecord *get_prop_by_index(s32 index) {
    if (index < 0 || index >= POS_DATA_ENTRY_LEN) {
        return NULL;
    }
    return &pos_data_entry[index];
}

static PropRecord *get_safe_prop_by_index(s32 index) {
    PropRecord *prop = get_prop_by_index(index);
    if (prop != NULL && (prop->flags & PROPFLAG_ENABLED)) {
        return prop;
    }
    return NULL;
}

extern struct StandTile *standTileStart;

static s32 get_tile_offset(StandTile *tile) {
    if (tile == NULL || standTileStart == NULL) {
        return -1;
    }
    return (s32)((u8 *)tile - (u8 *)standTileStart);
}

static StandTile *get_tile_by_offset(s32 offset) {
    if (offset < 0 || standTileStart == NULL) {
        return NULL;
    }
    return (StandTile *)((u8 *)standTileStart + offset);
}

typedef struct {
    s32 type;
    s32 weapon_right;
    s32 weapon_left;
    s32 prop_index;
} SavedInvItem;

typedef struct {
    // Control / Camera State
    s32 unknown;

    // Position
    coord3d pos;
    coord3d pos2;
    coord3d pos3;
    coord3d offset;
    s32 room_pointer_offset;
    coord3d current_model_pos;
    coord3d previous_model_pos;
    coord3d current_room_pos;
    s32 curRoomIndex;
    coord3d bondprevpos;

    // Collision
    struct collision434 field_488;
    struct collision434 previous_collision_info;
    s32 field_2A6C;
    s32 field_2A70_offset;
    s32 field_488_current_tile_ptr_offset;
    s32 field_488_current_tile_ptr_for_portals_offset;
    s32 previous_collision_info_current_tile_ptr_offset;
    s32 previous_collision_info_current_tile_ptr_for_portals_offset;

    // Angles
    f32 vv_theta;
    f32 vv_verta;

    // Movement speeds / physics
    f32 speedsideways;
    f32 speedstrafe;
    f32 speedforwards;
    s32 speedmaxtime60;
    f32 speedboost;
    f32 vertical_bounce_adjust;
    f32 ducking_height_offset;
    s32 crouchpos;
    f32 field_A4;
    s32 field_AC;
    struct rect4f collision_bounds;

    // 3D Velocity
    f32 field_78;
    f32 field_7C;
    f32 field_80;
    f32 speedgo;
    coord3d bondshotspeed;
    f32 speedtheta;
    f32 speedverta;

    // Bounce Dampeners
    f32 field_84;
    f32 field_88;

    // Height Tracking
    f32 field_6C;
    f32 field_70;
    f32 stanHeight;

    // Smoothed / Render Coordinates
    coord3d field_3B8;
    f32 field_3C4;
    f32 field_3C8;
    f32 field_3CC;

    // Walk / Head Bob Animation State
    s32 resetheadpos;
    s32 resetheadrot;
    s32 resetheadtick;
    s32 headanim;
    f32 headdamp;
    s32 headwalkingtime60;
    f32 headamplitude;
    f32 sideamplitude;
    coord3d headpos;
    coord3d headlook;
    coord3d headup;
    coord3d headpossum;
    coord3d headlooksum;
    coord3d headupsum;
    coord3d headbodyoffset;
    f32 standheight;
    coord3d standbodyoffset;
    f32 standfrac;
    coord3d standlook[2];
    coord3d standup[2];
    s32 standcnt;

    // Visual model & properties
    s32 field_59C;
    s32 field_5A0;
    s32 field_5A4;
    s32 field_5A8;
    s32 field_5AC;
    s32 field_5B0;
    s32 field_5B4;
    s32 field_5B8;
    s8 animFlipFlag;
    s8 field_5BD;
    s8 field_5BE;
    s8 field_5BF;

    // Visual layout matrices / switches padding (excluding field_654 which is a runtime data pointer)
    s32 field_5C4;
    s32 field_5C8;
    s32 field_5CC;
    s32 field_5D0;
    s32 field_5D4;
    s32 field_5D8;
    s32 field_5DC;
    s32 field_5E0;
    s32 field_5E4;
    s32 field_5E8;
    s32 field_5EC;
    s32 field_5F0;
    s32 field_5F4;
    s32 field_5F8;
    s32 field_5FC;
    s32 field_600;
    s32 field_604;
    s32 field_608;
    s32 field_60C;
    s32 field_610;
    s32 field_614;
    s32 field_618;
    s32 field_61C;
    s32 field_620;
    s32 field_624;
    s32 field_628;
    s32 field_62C;
    s32 field_630;
    s32 field_634;
    s32 field_638;
    s32 field_63C;
    s32 field_640;
    s32 field_644;
    s32 field_648;
    s32 field_64C;
    s32 field_650;
    s32 field_658;
    s32 field_65C;
    s32 field_660;
    s32 field_664;
    s32 field_668;
    s32 field_66C;
    s32 field_670;
    s32 field_674;
    s32 field_678;
    s32 field_67C;
    s32 field_680;
    s32 field_684;
    s32 field_688;
    s32 field_68C;
    s32 field_690;
    s32 field_694;
    s32 field_698;
    s32 field_69C;
    s32 field_6A0;
    s32 field_6A4;
    s32 field_6A8;
    s32 field_6AC;
    s32 field_6B0;
    s32 field_6B4;
    s32 field_6B8;
    s32 field_6BC;
    s32 field_6C0;
    s32 field_6C4;
    s32 field_6C8;
    s32 field_6CC;

    Mtxf bondheadmatrices[4];

    // Health / Armor
    f32 bondhealth;
    f32 bondarmour;
    f32 oldhealth;
    f32 oldarmour;
    f32 apparenthealth;
    f32 apparentarmour;

    // Prop state
    bool has_prop;
    coord3d prop_pos;
    s32 prop_stan_offset;
    u8 prop_rooms[PROPRECORD_STAN_ROOM_LEN];

    // Inventory & Ammo state
    s32 ammoheldarr[30];
    s32 equipcuritem;
    s32 equipallguns;
    s32 hand_item[2];
    s32 num_inv_items;
    SavedInvItem inv_items[50];

    // Hand Weapon logical state
    s32 hands_weaponnum[2];
    s32 hands_weaponnum_watchmenu[2];
    s32 hands_previous_weapon[2];
    s32 hands_weapon_firing_status[2];
    s32 hands_weapon_hold_time[2];
    s32 hands_when_detonating_mines_is_0[2];
    s32 hands_weapon_current_animation[2];
    s32 hands_weapon_ammo_in_magazine[2];
    s32 hands_weapon_next_weapon[2];
    s32 hands_weapon_animation_trigger[2];
    s32 hands_field_8B8[2];
    s32 lock_hand_model[2];

    // Cheat status
    u8 bondinvincible;

    // HUD Display states
#if defined(VERSION_JP) || defined(VERSION_EU)
    f32 damageshowtime;
    f32 healthshowtime;
#else
    s32 damageshowtime;
    s32 healthshowtime;
#endif
    s32 healthshowmode;

    // Fading & Breathing
    f32 bondfadetime60;
    f32 bondfadetimemax60;
    f32 bondfadefracold;
    f32 bondfadefracnew;
    f32 bondbreathing;

    // Padding between breathing and watch state (unreferenced, init to 0)
    s32 field_1A0;
    s32 field_1A4;
    s32 field_1A8;
    s32 field_1AC;
    s32 field_1B0;
    s32 field_1B4;
    s32 field_1B8;
    s32 field_1BC;

    // Weapon Sway cycle
    f32 gunposamplitude;
    f32 gunxamplitude;
    f32 gunsync;
    f32 syncchange;
    f32 synccount;
    s32 syncoffset;
    f32 field_107C;
    f32 field_1080;

    // Buttons Input State
    u16 buttons_pressed;
    u16 prev_buttons_pressed;

    // Death State
    s32 bonddead;
    f32 thetadie;
    f32 vertadie;
    s32 bondtype;
    s32 startnewbonddie;
    s32 redbloodfinished;
    s32 deathanimfinished;
    s32 field_42c;
    s32 controldef;
    s32 field_D0;

    // Look-Centering State
    s32 movecentrerelease;
    s32 lookaheadcentreenabled;
    s32 automovecentreenabled;
    s32 fastmovecentreenabled;
    s32 automovecentre;
    s32 field_104;
    s32 field_10C;

    // Unreferenced / unknown state fields
    s32 field_100;
    s32 field_108;

    // Weapon Sway & Control State
    s32 magnetattracttime;
    f32 swaytarget;
    f32 swayoffset0;
    f32 swayoffset2;
    f32 field_1280;
    s32 players_cur_animation;
    f32 field_1288;

    // Watch & Pause Menu State
    s32 watch_pause_time;
    s32 timer_1C4;
    s32 watch_animation_state;
    s32 outside_watch_menu;
    s32 open_close_solo_watch_menu;
    s32 pausing_flag;
    f32 pause_starting_angle;
    f32 pause_target_angle;
    f32 pause_related;
    f32 pause_animation_counter;
    s32 step_in_view_watch_animation;
    f32 pause_watch_related;
    f32 pause_watch_related_scaled;
    f32 pause_watch_related_adjust;
    s32 field_218;
    s32 field_21C;
    f32 field_1D4;
    f32 field_1D8;
    f32 pause_watch_position;
    f32 field_1E0;
    f32 field_1E4;
    f32 field_1E8;
    f32 field_1EC;
    f32 field_1F0;
    f32 field_1F4;
    s32 field_1F8;
    s32 field_1FC;
    f32 field_210;
    f32 field_214;
    s32 something_with_watch_object_instance;
    s32 field_234;
    s32 field_238;
    s32 field_23C;
    s32 field_240;
    s32 watch_scale_destination;
    s32 field_248;
    s32 field_24C;
    s32 field_250;
    s32 field_254;

    // Watch state padding / unknown state fields (offsets 0x25C–0x3B0)
    s32 field_25C;
    s32 field_260;
    s32 field_264;
    s32 field_268;
    s32 field_26C;
    s32 field_270;
    s32 field_274;
    s32 field_278;
    s32 field_27C;
    s32 field_280;
    s32 field_284;
    s32 field_288;
    s32 field_28C;
    s32 field_290;
    s32 field_294;
    s32 field_298;
    s32 field_29C;
    s32 field_2A0;
    s32 field_2A4;
    s32 field_2A8;
    s32 field_2AC;
    s32 field_2B0;
    s32 field_2B4;
    s32 field_2B8;
    s32 field_2BC;
    s32 field_2C0;
    s32 field_2C4;
    s32 field_2C8;
    s32 field_2CC;
    s32 field_2D0;
    s32 field_2D4;
    s32 field_2D8;
    s32 field_2DC;
    s32 field_2E0;
    s32 field_2E4;
    s32 field_2E8;
    s32 field_2EC;
    s32 field_2F0;
    s32 field_2F4;
    s32 field_2F8;
    s32 field_2FC;
    s32 field_300;
    s32 field_304;
    s32 field_308;
    s32 field_30C;
    s32 field_310;
    s32 field_314;
    s32 field_318;
    s32 field_31C;
    s32 field_320;
    s32 field_324;
    s32 field_328;
    s32 field_32C;
    s32 field_330;
    s32 field_334;
    s32 field_338;
    s32 field_33C;
    s32 field_340;
    s32 field_344;
    s32 field_348;
    s32 field_34C;
    s32 field_350;
    s32 field_354;
    s32 field_358;
    s32 field_35C;
    s32 field_360;
    s32 field_364;
    s32 field_368;
    s32 field_36C;
    s32 field_370;
    s32 field_374;
    s32 field_378;
    s32 field_37C;
    s32 field_380;
    s32 field_384;
    s32 field_388;
    s32 field_38C;
    s32 field_390;
    s32 field_394;
    s32 field_398;
    s32 field_39C;
    s32 field_3A0;
    s32 field_3A4;
    s32 field_3A8;
    s32 field_3AC;
    s32 field_3B0;

    // Firing & Hand rendering visibility States
    s32 z_trigger_timer;
    s32 last_z_trigger_timer;
    s32 hand_invisible[2];
    s32 resetshadecol;
    s32 aimtype;
    s32 field_FCC;
    s32 field_FD0;
    s32 field_FD8;
    struct rgba_u8 tileColor;

    // Auto-Aim and manual aiming State
    s32 insightaimmode;
    s32 autoyaimenabled;
    f32 autoaimy;
    s32 autoaim_target_y_index;
    s32 autoyaimtime60;
    s32 autoxaimenabled;
    f32 autoaimx;
    s32 autoaim_target_x_index;
    s32 autoxaimtime60;
    coord2d crosshair_angle;
    f32 crosshair_x_pos;
    f32 crosshair_y_pos;
    f32 guncrossdamp;
    coord2d field_FFC;
    f32 gun_azimuth_angle;
    f32 gun_azimuth_turning;
    f32 gunaimdamp;
    coord3d field_1010;
    Mtxf field_101C;
    s32 gunammooff;
    s32 field_1068;
    s32 gunsightmode;
    s32 field_112C;

    // HUD Messages & Viewport Projection States
    s32 hudmessoff;
    s32 bondmesscnt;
    f32 c_screenwidth;
    f32 c_screenheight;
    f32 c_screenleft;
    f32 c_screentop;
    f32 c_perspnear;
    f32 c_perspfovy;
    f32 c_perspaspect;
    f32 c_halfwidth;
    f32 c_halfheight;

    // Screen Projection Bounds
    f32 screenxminf;
    f32 screenyminf;
    f32 screenxmaxf;
    f32 screenymaxf;

    // Camera Scale & Reciprocal Scale
    f32 c_scalex;
    f32 c_scaley;
    f32 c_recipscalex;
    f32 c_recipscaley;

    // Camera LOD Scale
    f32 c_scalelod60;
    f32 c_scalelod;
    f32 c_lodscalez;
    u32 c_lodscalezu32;

    // Camera Normalization Vectors
    coord3d c_cameratopnorm;
    coord3d c_cameraleftnorm;

    // Viewports
    Vp viewports[2];

    // Viewport Bounds
    s16 viewx;
    s16 viewy;
    s16 viewleft;
    s16 viewtop;

    // Control Type & Watch Menu Configuration
    s32 cur_player_control_type_0;
    s32 cur_player_control_type_1;
    f32 cur_player_control_type_2;
    s32 neg_vspacing_for_control_type_entry;
    u32 has_set_control_type_data;
    s32 cur_item_weapon_getname;

    // Health/Armor Scaling Multipliers
    f32 actual_health;
    f32 actual_armor;

    // GoldenEye Key objective status
    s32 copiedgoldeneye;

    // Camera/Sniper Zoom
    f32 sniper_zoom;
    f32 camera_zoom;
    f32 zoomintime;
    f32 zoomintimemax;
    f32 zoominfovy;
    f32 zoominfovyold;
    f32 zoominfovynew;
    f32 fovy;
    f32 aspect;

    // Color Tint & Screen Fades
    s32 colourscreenred;
    s32 colourscreengreen;
    s32 colourscreenblue;
    f32 colourscreenfrac;
    f32 colourfadetime60;
    f32 colourfadetimemax60;
    s32 colourfaderedold;
    s32 colourfaderednew;
    s32 colourfadegreenold;
    s32 colourfadegreennew;
    s32 colourfadeblueold;
    s32 colourfadebluenew;
    f32 colourfadefracold;
    f32 colourfadefracnew;

    // Cheat Button Buffer & Status
    u16 cheat_display_text_related[20];
    u8 something_with_cheat_text;
    u8 can_display_cheat_text;
    u8 field_12B7;

    // Favorite Weapon Stats
    gunheld gunheldarr[10];

    // HUD and Watch graphics overlay
    s32 equipmaxitems;
    s32 bloodImgIdx;
    struct damage_display_parent armor_display_values[23];
    struct damage_display_parent health_display_values[23];
    s32 watch_body_armor_bar_gdl;
    s32 watch_health_bar_gdl;
    struct WatchRectangle buffer_for_watch_greenbackdrop_vertices[WATCH_NUMBER_SCREENS];
    struct WatchRectangle buffer_for_watch_static_vertices[1];

    // Transition and Environmental State Tracking Fields
    s32 field_2A44[2];
    s16 field_2A04;
    f32 field_2A08;
    f32 field_2A0C;
    s32 autocrouchpos;
    s32 healthdisplaytime;
    s32 damagetype;
    s32 field_8C;

    // Multiplayer, Stats, and Viewport Height/Dampening State Tracking
    s32 healthDamageType;
    f32 field_29BC;
    f32 field_29C0;
    s32 mpmenuon;
    s32 mpmenumode;
    s32 mpquitconfirm;
    s32 mpjoywascentre;
    s32 deathcount;
    s32 num_suicides;
    s32 field_29E0;
    s32 last_kill_time[4];
    s32 field_29F4;
    s32 field_29F8;

    // Tank Physics and Turret State Tracking Fields
    s32 in_tank_flag;
    s32 g_PlayerTankProp_index;
    s32 g_WorldTankProp_index;
    f32 g_PlayerTankYOffset;
    f32 g_TankTurnSpeed;
    f32 g_TankOrientationAngle;
    f32 tank_turret_unused_angle;
    f32 g_TankTurretVerticalAngle;
    f32 g_TankTurretVerticalAngleRelated;
    f32 g_TankTurretOrientationAngleRad;
    f32 g_TankTurretOrientationAngleDeg;
    f32 tank_turret_turn_speed;
    s32 g_BondCanEnterTank;
    f32 g_TankTurretAngle;
    f32 g_TankTurretTurn;
    s32 g_ExplodeTankOnDeathFlag;
    s32 g_TankDamagePenaltyTicks;
    s32 g_EnterTankAudioState;

    // Camera Mode and Cutscene Transition States
    s32 g_CameraMode;
    s32 camera_mode;
    s32 g_CameraAfterCinema;
    f32 camera_transition_timer;
    s32 camera_fade_active;
    s32 intro_camera_index;
    s32 is_timer_active;
    s32 g_PlayerInvincible;

    // Head & Hand Animation/Rendering Progress Flags
    f32 field_5C0;
    s32 field_FC8;
} SavedBondState;

static SavedBondState g_SavedBondState;
bool g_HasSavedState = FALSE;

void save_bond_state(void) {
    if (g_CurrentPlayer == NULL) {
        return;
    }

    // Control / Camera State
    g_SavedBondState.unknown = g_CurrentPlayer->unknown;

    // Position
    g_SavedBondState.pos = g_CurrentPlayer->pos;
    g_SavedBondState.pos2 = g_CurrentPlayer->pos2;
    g_SavedBondState.pos3 = g_CurrentPlayer->pos3;
    g_SavedBondState.offset = g_CurrentPlayer->offset;
    g_SavedBondState.room_pointer_offset = get_tile_offset(g_CurrentPlayer->room_pointer);
    g_SavedBondState.current_model_pos = g_CurrentPlayer->current_model_pos;
    g_SavedBondState.previous_model_pos = g_CurrentPlayer->previous_model_pos;
    g_SavedBondState.current_room_pos = g_CurrentPlayer->current_room_pos;
    g_SavedBondState.curRoomIndex = g_CurrentPlayer->curRoomIndex;
    g_SavedBondState.bondprevpos = g_CurrentPlayer->bondprevpos;

    // Collision
    g_SavedBondState.field_488 = g_CurrentPlayer->field_488;
    g_SavedBondState.previous_collision_info = g_CurrentPlayer->previous_collision_info;
    g_SavedBondState.field_2A6C = g_CurrentPlayer->field_2A6C;
    g_SavedBondState.field_2A70_offset = get_tile_offset(g_CurrentPlayer->field_2A70);
    g_SavedBondState.field_488_current_tile_ptr_offset = get_tile_offset(g_CurrentPlayer->field_488.current_tile_ptr);
    g_SavedBondState.field_488_current_tile_ptr_for_portals_offset = get_tile_offset(g_CurrentPlayer->field_488.current_tile_ptr_for_portals);
    g_SavedBondState.previous_collision_info_current_tile_ptr_offset = get_tile_offset(g_CurrentPlayer->previous_collision_info.current_tile_ptr);
    g_SavedBondState.previous_collision_info_current_tile_ptr_for_portals_offset = get_tile_offset(g_CurrentPlayer->previous_collision_info.current_tile_ptr_for_portals);

    // Angles
    g_SavedBondState.vv_theta = g_CurrentPlayer->vv_theta;
    g_SavedBondState.vv_verta = g_CurrentPlayer->vv_verta;

    // Movement speeds / physics
    g_SavedBondState.speedsideways = g_CurrentPlayer->speedsideways;
    g_SavedBondState.speedstrafe = g_CurrentPlayer->speedstrafe;
    g_SavedBondState.speedforwards = g_CurrentPlayer->speedforwards;
    g_SavedBondState.speedmaxtime60 = g_CurrentPlayer->speedmaxtime60;
    g_SavedBondState.speedboost = g_CurrentPlayer->speedboost;
    g_SavedBondState.vertical_bounce_adjust = g_CurrentPlayer->vertical_bounce_adjust;
    g_SavedBondState.ducking_height_offset = g_CurrentPlayer->ducking_height_offset;
    g_SavedBondState.crouchpos = g_CurrentPlayer->crouchpos;
    g_SavedBondState.field_A4 = g_CurrentPlayer->field_A4;
    g_SavedBondState.field_AC = g_CurrentPlayer->field_AC;
    g_SavedBondState.collision_bounds = g_CurrentPlayer->collision_bounds;

    // 3D Velocity
    g_SavedBondState.field_78 = g_CurrentPlayer->field_78;
    g_SavedBondState.field_7C = g_CurrentPlayer->field_7C;
    g_SavedBondState.field_80 = g_CurrentPlayer->field_80;
    g_SavedBondState.speedgo = g_CurrentPlayer->speedgo;
    g_SavedBondState.bondshotspeed = g_CurrentPlayer->bondshotspeed;
    g_SavedBondState.speedtheta = g_CurrentPlayer->speedtheta;
    g_SavedBondState.speedverta = g_CurrentPlayer->speedverta;

    // Bounce Dampeners
    g_SavedBondState.field_84 = g_CurrentPlayer->field_84;
    g_SavedBondState.field_88 = g_CurrentPlayer->field_88;

    // Height Tracking
    g_SavedBondState.field_6C = g_CurrentPlayer->field_6C;
    g_SavedBondState.field_70 = g_CurrentPlayer->field_70;
    g_SavedBondState.stanHeight = g_CurrentPlayer->stanHeight;

    // Smoothed / Render Coordinates
    g_SavedBondState.field_3B8 = g_CurrentPlayer->field_3B8;
    g_SavedBondState.field_3C4 = g_CurrentPlayer->field_3C4;
    g_SavedBondState.field_3C8 = g_CurrentPlayer->field_3C8;
    g_SavedBondState.field_3CC = g_CurrentPlayer->field_3CC;

    // Walk / Head Bob Animation State
    g_SavedBondState.resetheadpos = g_CurrentPlayer->resetheadpos;
    g_SavedBondState.resetheadrot = g_CurrentPlayer->resetheadrot;
    g_SavedBondState.resetheadtick = g_CurrentPlayer->resetheadtick;
    g_SavedBondState.headanim = g_CurrentPlayer->headanim;
    g_SavedBondState.headdamp = g_CurrentPlayer->headdamp;
    g_SavedBondState.headwalkingtime60 = g_CurrentPlayer->headwalkingtime60;
    g_SavedBondState.headamplitude = g_CurrentPlayer->headamplitude;
    g_SavedBondState.sideamplitude = g_CurrentPlayer->sideamplitude;
    g_SavedBondState.headpos = g_CurrentPlayer->headpos;
    g_SavedBondState.headlook = g_CurrentPlayer->headlook;
    g_SavedBondState.headup = g_CurrentPlayer->headup;
    g_SavedBondState.headpossum = g_CurrentPlayer->headpossum;
    g_SavedBondState.headlooksum = g_CurrentPlayer->headlooksum;
    g_SavedBondState.headupsum = g_CurrentPlayer->headupsum;
    g_SavedBondState.headbodyoffset = g_CurrentPlayer->headbodyoffset;
    g_SavedBondState.standheight = g_CurrentPlayer->standheight;
    g_SavedBondState.standbodyoffset = g_CurrentPlayer->standbodyoffset;
    g_SavedBondState.standfrac = g_CurrentPlayer->standfrac;
    g_SavedBondState.standlook[0] = g_CurrentPlayer->standlook[0];
    g_SavedBondState.standlook[1] = g_CurrentPlayer->standlook[1];
    g_SavedBondState.standup[0] = g_CurrentPlayer->standup[0];
    g_SavedBondState.standup[1] = g_CurrentPlayer->standup[1];
    g_SavedBondState.standcnt = g_CurrentPlayer->standcnt;
    g_SavedBondState.animFlipFlag = g_CurrentPlayer->animFlipFlag;
    g_SavedBondState.field_5BD = g_CurrentPlayer->field_5BD;
    g_SavedBondState.field_5BE = g_CurrentPlayer->field_5BE;
    g_SavedBondState.field_5BF = g_CurrentPlayer->field_5BF;
    {
        s32 i;
        for (i = 0; i < 4; i++) {
            g_SavedBondState.bondheadmatrices[i] = g_CurrentPlayer->bondheadmatrices[i];
        }
    }

    // Health / Armor
    g_SavedBondState.bondhealth = g_CurrentPlayer->bondhealth;
    g_SavedBondState.bondarmour = g_CurrentPlayer->bondarmour;
    g_SavedBondState.oldhealth = g_CurrentPlayer->oldhealth;
    g_SavedBondState.oldarmour = g_CurrentPlayer->oldarmour;
    g_SavedBondState.apparenthealth = g_CurrentPlayer->apparenthealth;
    g_SavedBondState.apparentarmour = g_CurrentPlayer->apparentarmour;

    // Prop state
    if (g_CurrentPlayer->prop != NULL) {
        g_SavedBondState.has_prop = TRUE;
        g_SavedBondState.prop_pos = g_CurrentPlayer->prop->pos;
        g_SavedBondState.prop_stan_offset = get_tile_offset(g_CurrentPlayer->prop->stan);
        g_SavedBondState.prop_rooms[0] = g_CurrentPlayer->prop->rooms[0];
        g_SavedBondState.prop_rooms[1] = g_CurrentPlayer->prop->rooms[1];
        g_SavedBondState.prop_rooms[2] = g_CurrentPlayer->prop->rooms[2];
        g_SavedBondState.prop_rooms[3] = g_CurrentPlayer->prop->rooms[3];
    } else {
        g_SavedBondState.has_prop = FALSE;
    }

    // Inventory & Ammo state
    {
        s32 i;
        for (i = 0; i < 30; i++) {
            g_SavedBondState.ammoheldarr[i] = g_CurrentPlayer->ammoheldarr[i];
        }
    }
    g_SavedBondState.equipcuritem = g_CurrentPlayer->equipcuritem;
    g_SavedBondState.equipallguns = g_CurrentPlayer->equipallguns;
    g_SavedBondState.hand_item[0] = g_CurrentPlayer->hand_item[0];
    g_SavedBondState.hand_item[1] = g_CurrentPlayer->hand_item[1];

    // Clear saved inventory items
    {
        s32 i;
        for (i = 0; i < 50; i++) {
            g_SavedBondState.inv_items[i].type = -1;
            g_SavedBondState.inv_items[i].weapon_right = -1;
            g_SavedBondState.inv_items[i].weapon_left = -1;
            g_SavedBondState.inv_items[i].prop_index = -1;
        }
    }
    g_SavedBondState.num_inv_items = 0;

    {
        InvItem *first = g_CurrentPlayer->ptr_inventory_first_in_cycle;
        InvItem *item = first;
        s32 count = 0;
        if (item != NULL) {
            do {
                if (count >= 50) break;
                g_SavedBondState.inv_items[count].type = item->type;
                if (item->type == INV_ITEM_WEAPON) {
                    g_SavedBondState.inv_items[count].weapon_right = item->type_inv_item.type_weap.weapon;
                } else if (item->type == INV_ITEM_DUAL) {
                    g_SavedBondState.inv_items[count].weapon_right = item->type_inv_item.type_dual.weapon_right;
                    g_SavedBondState.inv_items[count].weapon_left = item->type_inv_item.type_dual.weapon_left;
                } else if (item->type == INV_ITEM_PROP) {
                    g_SavedBondState.inv_items[count].prop_index = get_prop_index(item->type_inv_item.type_prop.prop);
                }
                count++;
                item = item->next;
            } while (item != first && item != NULL);
        }
        g_SavedBondState.num_inv_items = count;
    }

    // Hand Weapon logical state
    {
        s32 i;
        for (i = 0; i < 2; i++) {
            g_SavedBondState.hands_weaponnum[i] = g_CurrentPlayer->hands[i].weaponnum;
            g_SavedBondState.hands_weaponnum_watchmenu[i] = g_CurrentPlayer->hands[i].weaponnum_watchmenu;
            g_SavedBondState.hands_previous_weapon[i] = g_CurrentPlayer->hands[i].previous_weapon;
            g_SavedBondState.hands_weapon_firing_status[i] = g_CurrentPlayer->hands[i].weapon_firing_status;
            g_SavedBondState.hands_weapon_hold_time[i] = g_CurrentPlayer->hands[i].weapon_hold_time;
            g_SavedBondState.hands_when_detonating_mines_is_0[i] = g_CurrentPlayer->hands[i].when_detonating_mines_is_0;
            g_SavedBondState.hands_weapon_current_animation[i] = g_CurrentPlayer->hands[i].weapon_current_animation;
            g_SavedBondState.hands_weapon_ammo_in_magazine[i] = g_CurrentPlayer->hands[i].weapon_ammo_in_magazine;
            g_SavedBondState.hands_weapon_next_weapon[i] = g_CurrentPlayer->hands[i].weapon_next_weapon;
            g_SavedBondState.hands_weapon_animation_trigger[i] = g_CurrentPlayer->hands[i].weapon_animation_trigger;
            g_SavedBondState.hands_field_8B8[i] = g_CurrentPlayer->hands[i].field_8B8;
            g_SavedBondState.lock_hand_model[i] = g_CurrentPlayer->lock_hand_model[i];
        }
    }

    // Cheat status
    g_SavedBondState.bondinvincible = g_CurrentPlayer->bondinvincible;

    // HUD Display states
    g_SavedBondState.damageshowtime = g_CurrentPlayer->damageshowtime;
    g_SavedBondState.healthshowtime = g_CurrentPlayer->healthshowtime;
    g_SavedBondState.healthshowmode = g_CurrentPlayer->healthshowmode;

    // Fading & Breathing
    g_SavedBondState.bondfadetime60 = g_CurrentPlayer->bondfadetime60;
    g_SavedBondState.bondfadetimemax60 = g_CurrentPlayer->bondfadetimemax60;
    g_SavedBondState.bondfadefracold = g_CurrentPlayer->bondfadefracold;
    g_SavedBondState.bondfadefracnew = g_CurrentPlayer->bondfadefracnew;
    g_SavedBondState.bondbreathing = g_CurrentPlayer->bondbreathing;

    // Padding between breathing and watch state
    g_SavedBondState.field_1A0 = g_CurrentPlayer->field_1A0;
    g_SavedBondState.field_1A4 = g_CurrentPlayer->field_1A4;
    g_SavedBondState.field_1A8 = g_CurrentPlayer->field_1A8;
    g_SavedBondState.field_1AC = g_CurrentPlayer->field_1AC;
    g_SavedBondState.field_1B0 = g_CurrentPlayer->field_1B0;
    g_SavedBondState.field_1B4 = g_CurrentPlayer->field_1B4;
    g_SavedBondState.field_1B8 = g_CurrentPlayer->field_1B8;
    g_SavedBondState.field_1BC = g_CurrentPlayer->field_1BC;

    // Weapon Sway cycle
    g_SavedBondState.gunposamplitude = g_CurrentPlayer->gunposamplitude;
    g_SavedBondState.gunxamplitude = g_CurrentPlayer->gunxamplitude;
    g_SavedBondState.gunsync = g_CurrentPlayer->gunsync;
    g_SavedBondState.syncchange = g_CurrentPlayer->syncchange;
    g_SavedBondState.synccount = g_CurrentPlayer->synccount;
    g_SavedBondState.syncoffset = g_CurrentPlayer->syncoffset;
    g_SavedBondState.field_107C = g_CurrentPlayer->field_107C;
    g_SavedBondState.field_1080 = g_CurrentPlayer->field_1080;

    // Buttons Input State
    g_SavedBondState.buttons_pressed = g_CurrentPlayer->buttons_pressed;
    g_SavedBondState.prev_buttons_pressed = g_CurrentPlayer->prev_buttons_pressed;

    // Death State
    g_SavedBondState.bonddead = g_CurrentPlayer->bonddead;
    g_SavedBondState.thetadie = g_CurrentPlayer->thetadie;
    g_SavedBondState.vertadie = g_CurrentPlayer->vertadie;
    g_SavedBondState.bondtype = g_CurrentPlayer->bondtype;
    g_SavedBondState.startnewbonddie = g_CurrentPlayer->startnewbonddie;
    g_SavedBondState.redbloodfinished = g_CurrentPlayer->redbloodfinished;
    g_SavedBondState.deathanimfinished = g_CurrentPlayer->deathanimfinished;
    g_SavedBondState.field_42c = g_CurrentPlayer->field_42c;
    g_SavedBondState.controldef = g_CurrentPlayer->controldef;
    g_SavedBondState.field_D0 = g_CurrentPlayer->field_D0;

    // Look-Centering State
    g_SavedBondState.movecentrerelease = g_CurrentPlayer->movecentrerelease;
    g_SavedBondState.lookaheadcentreenabled = g_CurrentPlayer->lookaheadcentreenabled;
    g_SavedBondState.automovecentreenabled = g_CurrentPlayer->automovecentreenabled;
    g_SavedBondState.fastmovecentreenabled = g_CurrentPlayer->fastmovecentreenabled;
    g_SavedBondState.automovecentre = g_CurrentPlayer->automovecentre;
    g_SavedBondState.field_104 = g_CurrentPlayer->field_104;
    g_SavedBondState.field_10C = g_CurrentPlayer->field_10C;

    // Unreferenced / unknown state fields
    g_SavedBondState.field_100 = g_CurrentPlayer->field_100;
    g_SavedBondState.field_108 = g_CurrentPlayer->field_108;

    // Weapon Sway & Control State
    g_SavedBondState.magnetattracttime = g_CurrentPlayer->magnetattracttime;
    g_SavedBondState.swaytarget = g_CurrentPlayer->swaytarget;
    g_SavedBondState.swayoffset0 = g_CurrentPlayer->swayoffset0;
    g_SavedBondState.swayoffset2 = g_CurrentPlayer->swayoffset2;
    g_SavedBondState.field_1280 = g_CurrentPlayer->field_1280;
    g_SavedBondState.players_cur_animation = g_CurrentPlayer->players_cur_animation;
    g_SavedBondState.field_1288 = g_CurrentPlayer->field_1288;

    // Watch & Pause Menu State
    g_SavedBondState.watch_pause_time = g_CurrentPlayer->watch_pause_time;
    g_SavedBondState.timer_1C4 = g_CurrentPlayer->timer_1C4;
    g_SavedBondState.watch_animation_state = g_CurrentPlayer->watch_animation_state;
    g_SavedBondState.outside_watch_menu = g_CurrentPlayer->outside_watch_menu;
    g_SavedBondState.open_close_solo_watch_menu = g_CurrentPlayer->open_close_solo_watch_menu;
    g_SavedBondState.pausing_flag = g_CurrentPlayer->pausing_flag;
    g_SavedBondState.pause_starting_angle = g_CurrentPlayer->pause_starting_angle;
    g_SavedBondState.pause_target_angle = g_CurrentPlayer->pause_target_angle;
    g_SavedBondState.pause_related = g_CurrentPlayer->pause_related;
    g_SavedBondState.pause_animation_counter = g_CurrentPlayer->pause_animation_counter;
    g_SavedBondState.step_in_view_watch_animation = g_CurrentPlayer->step_in_view_watch_animation;
    g_SavedBondState.pause_watch_related = g_CurrentPlayer->pause_watch_related;
    g_SavedBondState.pause_watch_related_scaled = g_CurrentPlayer->pause_watch_related_scaled;
    g_SavedBondState.pause_watch_related_adjust = g_CurrentPlayer->pause_watch_related_adjust;
    g_SavedBondState.field_218 = g_CurrentPlayer->field_218;
    g_SavedBondState.field_21C = g_CurrentPlayer->field_21C;
    g_SavedBondState.field_1D4 = g_CurrentPlayer->field_1D4;
    g_SavedBondState.field_1D8 = g_CurrentPlayer->field_1D8;
    g_SavedBondState.pause_watch_position = g_CurrentPlayer->pause_watch_position;
    g_SavedBondState.field_1E0 = g_CurrentPlayer->field_1E0;
    g_SavedBondState.field_1E4 = g_CurrentPlayer->field_1E4;
    g_SavedBondState.field_1E8 = g_CurrentPlayer->field_1E8;
    g_SavedBondState.field_1EC = g_CurrentPlayer->field_1EC;
    g_SavedBondState.field_1F0 = g_CurrentPlayer->field_1F0;
    g_SavedBondState.field_1F4 = g_CurrentPlayer->field_1F4;
    g_SavedBondState.field_1F8 = g_CurrentPlayer->field_1F8;
    g_SavedBondState.field_1FC = g_CurrentPlayer->field_1FC;
    g_SavedBondState.field_210 = g_CurrentPlayer->field_210;
    g_SavedBondState.field_214 = g_CurrentPlayer->field_214;
    g_SavedBondState.something_with_watch_object_instance = g_CurrentPlayer->something_with_watch_object_instance;
    g_SavedBondState.field_234 = g_CurrentPlayer->field_234;
    g_SavedBondState.field_238 = g_CurrentPlayer->field_238;
    g_SavedBondState.field_23C = g_CurrentPlayer->field_23C;
    g_SavedBondState.field_240 = g_CurrentPlayer->field_240;
    g_SavedBondState.watch_scale_destination = g_CurrentPlayer->watch_scale_destination;
    g_SavedBondState.field_248 = g_CurrentPlayer->field_248;
    g_SavedBondState.field_24C = g_CurrentPlayer->field_24C;
    g_SavedBondState.field_250 = g_CurrentPlayer->field_250;
    g_SavedBondState.field_254 = g_CurrentPlayer->field_254;

    // Watch state padding / unknown state fields (offsets 0x25C–0x3B0)
    g_SavedBondState.field_25C = g_CurrentPlayer->field_25C;
    g_SavedBondState.field_260 = g_CurrentPlayer->field_260;
    g_SavedBondState.field_264 = g_CurrentPlayer->field_264;
    g_SavedBondState.field_268 = g_CurrentPlayer->field_268;
    g_SavedBondState.field_26C = g_CurrentPlayer->field_26C;
    g_SavedBondState.field_270 = g_CurrentPlayer->field_270;
    g_SavedBondState.field_274 = g_CurrentPlayer->field_274;
    g_SavedBondState.field_278 = g_CurrentPlayer->field_278;
    g_SavedBondState.field_27C = g_CurrentPlayer->field_27C;
    g_SavedBondState.field_280 = g_CurrentPlayer->field_280;
    g_SavedBondState.field_284 = g_CurrentPlayer->field_284;
    g_SavedBondState.field_288 = g_CurrentPlayer->field_288;
    g_SavedBondState.field_28C = g_CurrentPlayer->field_28C;
    g_SavedBondState.field_290 = g_CurrentPlayer->field_290;
    g_SavedBondState.field_294 = g_CurrentPlayer->field_294;
    g_SavedBondState.field_298 = g_CurrentPlayer->field_298;
    g_SavedBondState.field_29C = g_CurrentPlayer->field_29C;
    g_SavedBondState.field_2A0 = g_CurrentPlayer->field_2A0;
    g_SavedBondState.field_2A4 = g_CurrentPlayer->field_2A4;
    g_SavedBondState.field_2A8 = g_CurrentPlayer->field_2A8;
    g_SavedBondState.field_2AC = g_CurrentPlayer->field_2AC;
    g_SavedBondState.field_2B0 = g_CurrentPlayer->field_2B0;
    g_SavedBondState.field_2B4 = g_CurrentPlayer->field_2B4;
    g_SavedBondState.field_2B8 = g_CurrentPlayer->field_2B8;
    g_SavedBondState.field_2BC = g_CurrentPlayer->field_2BC;
    g_SavedBondState.field_2C0 = g_CurrentPlayer->field_2C0;
    g_SavedBondState.field_2C4 = g_CurrentPlayer->field_2C4;
    g_SavedBondState.field_2C8 = g_CurrentPlayer->field_2C8;
    g_SavedBondState.field_2CC = g_CurrentPlayer->field_2CC;
    g_SavedBondState.field_2D0 = g_CurrentPlayer->field_2D0;
    g_SavedBondState.field_2D4 = g_CurrentPlayer->field_2D4;
    g_SavedBondState.field_2D8 = g_CurrentPlayer->field_2D8;
    g_SavedBondState.field_2DC = g_SavedBondState.field_2DC;
    g_SavedBondState.field_2E0 = g_SavedBondState.field_2E0;
    g_SavedBondState.field_2E4 = g_SavedBondState.field_2E4;
    g_SavedBondState.field_2E8 = g_SavedBondState.field_2E8;
    g_SavedBondState.field_2EC = g_SavedBondState.field_2EC;
    g_SavedBondState.field_2F0 = g_SavedBondState.field_2F0;
    g_SavedBondState.field_2F4 = g_SavedBondState.field_2F4;
    g_SavedBondState.field_2F8 = g_SavedBondState.field_2F8;
    g_SavedBondState.field_2FC = g_SavedBondState.field_2FC;
    g_SavedBondState.field_300 = g_SavedBondState.field_300;
    g_SavedBondState.field_304 = g_SavedBondState.field_304;
    g_SavedBondState.field_308 = g_SavedBondState.field_308;
    g_SavedBondState.field_30C = g_SavedBondState.field_30C;
    g_SavedBondState.field_310 = g_SavedBondState.field_310;
    g_SavedBondState.field_314 = g_SavedBondState.field_314;
    g_SavedBondState.field_318 = g_SavedBondState.field_318;
    g_SavedBondState.field_31C = g_SavedBondState.field_31C;
    g_SavedBondState.field_320 = g_SavedBondState.field_320;
    g_SavedBondState.field_324 = g_SavedBondState.field_324;
    g_SavedBondState.field_328 = g_SavedBondState.field_328;
    g_SavedBondState.field_32C = g_SavedBondState.field_32C;
    g_SavedBondState.field_330 = g_SavedBondState.field_330;
    g_SavedBondState.field_334 = g_SavedBondState.field_334;
    g_SavedBondState.field_338 = g_SavedBondState.field_338;
    g_SavedBondState.field_33C = g_SavedBondState.field_33C;
    g_SavedBondState.field_340 = g_SavedBondState.field_340;
    g_SavedBondState.field_344 = g_SavedBondState.field_344;
    g_SavedBondState.field_348 = g_SavedBondState.field_348;
    g_SavedBondState.field_34C = g_SavedBondState.field_34C;
    g_SavedBondState.field_350 = g_SavedBondState.field_350;
    g_SavedBondState.field_354 = g_SavedBondState.field_354;
    g_SavedBondState.field_358 = g_SavedBondState.field_358;
    g_SavedBondState.field_35C = g_SavedBondState.field_35C;
    g_SavedBondState.field_360 = g_SavedBondState.field_360;
    g_SavedBondState.field_364 = g_SavedBondState.field_364;
    g_SavedBondState.field_368 = g_SavedBondState.field_368;
    g_SavedBondState.field_36C = g_SavedBondState.field_36C;
    g_SavedBondState.field_370 = g_SavedBondState.field_370;
    g_SavedBondState.field_374 = g_SavedBondState.field_374;
    g_SavedBondState.field_378 = g_SavedBondState.field_378;
    g_SavedBondState.field_37C = g_SavedBondState.field_37C;
    g_SavedBondState.field_380 = g_SavedBondState.field_380;
    g_SavedBondState.field_384 = g_SavedBondState.field_384;
    g_SavedBondState.field_388 = g_SavedBondState.field_388;
    g_SavedBondState.field_38C = g_SavedBondState.field_38C;
    g_SavedBondState.field_390 = g_SavedBondState.field_390;
    g_SavedBondState.field_394 = g_SavedBondState.field_394;
    g_SavedBondState.field_398 = g_SavedBondState.field_398;
    g_SavedBondState.field_39C = g_SavedBondState.field_39C;
    g_SavedBondState.field_3A0 = g_SavedBondState.field_3A0;
    g_SavedBondState.field_3A4 = g_SavedBondState.field_3A4;
    g_SavedBondState.field_3A8 = g_SavedBondState.field_3A8;
    g_SavedBondState.field_3AC = g_SavedBondState.field_3AC;
    g_SavedBondState.field_3B0 = g_SavedBondState.field_3B0;

    // Firing & Hand rendering visibility States
    g_SavedBondState.z_trigger_timer = g_CurrentPlayer->z_trigger_timer;
    g_SavedBondState.last_z_trigger_timer = g_CurrentPlayer->last_z_trigger_timer;
    g_SavedBondState.hand_invisible[0] = g_CurrentPlayer->hand_invisible[0];
    g_SavedBondState.hand_invisible[1] = g_CurrentPlayer->hand_invisible[1];
    g_SavedBondState.resetshadecol = g_CurrentPlayer->resetshadecol;
    g_SavedBondState.aimtype = g_CurrentPlayer->aimtype;
    g_SavedBondState.field_FCC = g_CurrentPlayer->field_FCC;
    g_SavedBondState.field_FD0 = g_CurrentPlayer->field_FD0;
    g_SavedBondState.field_FD8 = g_CurrentPlayer->field_FD8;
    g_SavedBondState.tileColor = g_CurrentPlayer->tileColor;

    // Auto-Aim and manual aiming State
    g_SavedBondState.insightaimmode = g_CurrentPlayer->insightaimmode;
    g_SavedBondState.autoyaimenabled = g_CurrentPlayer->autoyaimenabled;
    g_SavedBondState.autoaimy = g_CurrentPlayer->autoaimy;
    g_SavedBondState.autoaim_target_y_index = get_prop_index(g_CurrentPlayer->autoaim_target_y);
    g_SavedBondState.autoyaimtime60 = g_CurrentPlayer->autoyaimtime60;
    g_SavedBondState.autoxaimenabled = g_CurrentPlayer->autoxaimenabled;
    g_SavedBondState.autoaimx = g_CurrentPlayer->autoaimx;
    g_SavedBondState.autoaim_target_x_index = get_prop_index(g_CurrentPlayer->autoaim_target_x);
    g_SavedBondState.autoxaimtime60 = g_CurrentPlayer->autoxaimtime60;
    g_SavedBondState.crosshair_angle = g_CurrentPlayer->crosshair_angle;
    g_SavedBondState.crosshair_x_pos = g_CurrentPlayer->crosshair_x_pos;
    g_SavedBondState.crosshair_y_pos = g_CurrentPlayer->crosshair_y_pos;
    g_SavedBondState.guncrossdamp = g_CurrentPlayer->guncrossdamp;
    g_SavedBondState.field_FFC = g_CurrentPlayer->field_FFC;
    g_SavedBondState.gun_azimuth_angle = g_CurrentPlayer->gun_azimuth_angle;
    g_SavedBondState.gun_azimuth_turning = g_CurrentPlayer->gun_azimuth_turning;
    g_SavedBondState.gunaimdamp = g_CurrentPlayer->gunaimdamp;
    g_SavedBondState.field_1010 = g_CurrentPlayer->field_1010;
    g_SavedBondState.field_101C = g_CurrentPlayer->field_101C;
    g_SavedBondState.gunammooff = g_CurrentPlayer->gunammooff;
    g_SavedBondState.field_1068 = g_CurrentPlayer->field_1068;
    g_SavedBondState.gunsightmode = g_CurrentPlayer->gunsightmode;
    g_SavedBondState.field_112C = g_CurrentPlayer->field_112C;

    // HUD Messages & Viewport Projection States
    g_SavedBondState.hudmessoff = g_CurrentPlayer->hudmessoff;
    g_SavedBondState.bondmesscnt = g_CurrentPlayer->bondmesscnt;
    g_SavedBondState.c_screenwidth = g_CurrentPlayer->c_screenwidth;
    g_SavedBondState.c_screenheight = g_CurrentPlayer->c_screenheight;
    g_SavedBondState.c_screenleft = g_CurrentPlayer->c_screenleft;
    g_SavedBondState.c_screentop = g_CurrentPlayer->c_screentop;
    g_SavedBondState.c_perspnear = g_CurrentPlayer->c_perspnear;
    g_SavedBondState.c_perspfovy = g_CurrentPlayer->c_perspfovy;
    g_SavedBondState.c_perspaspect = g_CurrentPlayer->c_perspaspect;
    g_SavedBondState.c_halfwidth = g_CurrentPlayer->c_halfwidth;
    g_SavedBondState.c_halfheight = g_CurrentPlayer->c_halfheight;

    // Screen Projection Bounds
    g_SavedBondState.screenxminf = g_CurrentPlayer->screenxminf;
    g_SavedBondState.screenyminf = g_CurrentPlayer->screenyminf;
    g_SavedBondState.screenxmaxf = g_CurrentPlayer->screenxmaxf;
    g_SavedBondState.screenymaxf = g_CurrentPlayer->screenymaxf;

    // Camera Scale & Reciprocal Scale
    g_SavedBondState.c_scalex = g_CurrentPlayer->c_scalex;
    g_SavedBondState.c_scaley = g_CurrentPlayer->c_scaley;
    g_SavedBondState.c_recipscalex = g_CurrentPlayer->c_recipscalex;
    g_SavedBondState.c_recipscaley = g_CurrentPlayer->c_recipscaley;

    // Camera LOD Scale
    g_SavedBondState.c_scalelod60 = g_CurrentPlayer->c_scalelod60;
    g_SavedBondState.c_scalelod = g_CurrentPlayer->c_scalelod;
    g_SavedBondState.c_lodscalez = g_CurrentPlayer->c_lodscalez;
    g_SavedBondState.c_lodscalezu32 = g_CurrentPlayer->c_lodscalezu32;

    // Camera Normalization Vectors
    g_SavedBondState.c_cameratopnorm = g_CurrentPlayer->c_cameratopnorm;
    g_SavedBondState.c_cameraleftnorm = g_CurrentPlayer->c_cameraleftnorm;

    // Viewports
    g_SavedBondState.viewports[0] = g_CurrentPlayer->viewports[0];
    g_SavedBondState.viewports[1] = g_CurrentPlayer->viewports[1];

    // Viewport Bounds
    g_SavedBondState.viewx = g_CurrentPlayer->viewx;
    g_SavedBondState.viewy = g_CurrentPlayer->viewy;
    g_SavedBondState.viewleft = g_CurrentPlayer->viewleft;
    g_SavedBondState.viewtop = g_CurrentPlayer->viewtop;

    // Control Type & Watch Menu Configuration
    g_SavedBondState.cur_player_control_type_0 = g_CurrentPlayer->cur_player_control_type_0;
    g_SavedBondState.cur_player_control_type_1 = g_CurrentPlayer->cur_player_control_type_1;
    g_SavedBondState.cur_player_control_type_2 = g_CurrentPlayer->cur_player_control_type_2;
    g_SavedBondState.neg_vspacing_for_control_type_entry = g_CurrentPlayer->neg_vspacing_for_control_type_entry;
    g_SavedBondState.has_set_control_type_data = g_CurrentPlayer->has_set_control_type_data;
    g_SavedBondState.cur_item_weapon_getname = g_CurrentPlayer->cur_item_weapon_getname;

    // Health/Armor Scaling Multipliers
    g_SavedBondState.actual_health = g_CurrentPlayer->actual_health;
    g_SavedBondState.actual_armor = g_CurrentPlayer->actual_armor;

    // GoldenEye Key objective status
    g_SavedBondState.copiedgoldeneye = g_CurrentPlayer->copiedgoldeneye;

    // Camera/Sniper Zoom
    g_SavedBondState.sniper_zoom = g_CurrentPlayer->sniper_zoom;
    g_SavedBondState.camera_zoom = g_CurrentPlayer->camera_zoom;
    g_SavedBondState.zoomintime = g_CurrentPlayer->zoomintime;
    g_SavedBondState.zoomintimemax = g_CurrentPlayer->zoomintimemax;
    g_SavedBondState.zoominfovy = g_CurrentPlayer->zoominfovy;
    g_SavedBondState.zoominfovyold = g_CurrentPlayer->zoominfovyold;
    g_SavedBondState.zoominfovynew = g_CurrentPlayer->zoominfovynew;
    g_SavedBondState.fovy = g_CurrentPlayer->fovy;
    g_SavedBondState.aspect = g_CurrentPlayer->aspect;

    // Color Tint & Screen Fades
    g_SavedBondState.colourscreenred = g_CurrentPlayer->colourscreenred;
    g_SavedBondState.colourscreengreen = g_CurrentPlayer->colourscreengreen;
    g_SavedBondState.colourscreenblue = g_CurrentPlayer->colourscreenblue;
    g_SavedBondState.colourscreenfrac = g_CurrentPlayer->colourscreenfrac;
    g_SavedBondState.colourfadetime60 = g_CurrentPlayer->colourfadetime60;
    g_SavedBondState.colourfadetimemax60 = g_CurrentPlayer->colourfadetimemax60;
    g_SavedBondState.colourfaderedold = g_CurrentPlayer->colourfaderedold;
    g_SavedBondState.colourfaderednew = g_CurrentPlayer->colourfaderednew;
    g_SavedBondState.colourfadegreenold = g_CurrentPlayer->colourfadegreenold;
    g_SavedBondState.colourfadegreennew = g_CurrentPlayer->colourfadegreennew;
    g_SavedBondState.colourfadeblueold = g_CurrentPlayer->colourfadeblueold;
    g_SavedBondState.colourfadebluenew = g_CurrentPlayer->colourfadebluenew;
    g_SavedBondState.colourfadefracold = g_CurrentPlayer->colourfadefracold;
    g_SavedBondState.colourfadefracnew = g_CurrentPlayer->colourfadefracnew;

    // Cheat Button Buffer & Status
    {
        s32 i;
        for (i = 0; i < 20; i++) {
            g_SavedBondState.cheat_display_text_related[i] = g_CurrentPlayer->cheat_display_text_related[i];
        }
    }
    g_SavedBondState.something_with_cheat_text = g_CurrentPlayer->something_with_cheat_text;
    g_SavedBondState.can_display_cheat_text = g_CurrentPlayer->can_display_cheat_text;
    g_SavedBondState.field_12B7 = g_CurrentPlayer->field_12B7;

    // Favorite Weapon Stats
    {
        s32 i;
        for (i = 0; i < 10; i++) {
            g_SavedBondState.gunheldarr[i] = g_CurrentPlayer->gunheldarr[i];
        }
    }

    // HUD and Watch graphics overlay
    g_SavedBondState.equipmaxitems = g_CurrentPlayer->equipmaxitems;
    g_SavedBondState.bloodImgIdx = g_CurrentPlayer->bloodImgIdx;
    {
        s32 i;
        for (i = 0; i < 23; i++) {
            g_SavedBondState.armor_display_values[i] = g_CurrentPlayer->armor_display_values[i];
            g_SavedBondState.health_display_values[i] = g_CurrentPlayer->health_display_values[i];
        }
    }
    g_SavedBondState.watch_body_armor_bar_gdl = g_CurrentPlayer->watch_body_armor_bar_gdl;
    g_SavedBondState.watch_health_bar_gdl = g_CurrentPlayer->watch_health_bar_gdl;
    {
        s32 i;
        for (i = 0; i < WATCH_NUMBER_SCREENS; i++) {
            g_SavedBondState.buffer_for_watch_greenbackdrop_vertices[i] = g_CurrentPlayer->buffer_for_watch_greenbackdrop_vertices[i];
        }
        g_SavedBondState.buffer_for_watch_static_vertices[0] = g_CurrentPlayer->buffer_for_watch_static_vertices[0];
    }

    // Transition and Environmental State Tracking Fields
    g_SavedBondState.field_2A44[0] = g_CurrentPlayer->field_2A44[0];
    g_SavedBondState.field_2A44[1] = g_CurrentPlayer->field_2A44[1];
    g_SavedBondState.field_2A04 = g_CurrentPlayer->field_2A04;
    g_SavedBondState.field_2A08 = g_CurrentPlayer->field_2A08;
    g_SavedBondState.field_2A0C = g_CurrentPlayer->field_2A0C;
    g_SavedBondState.autocrouchpos = g_CurrentPlayer->autocrouchpos;
    g_SavedBondState.healthdisplaytime = g_CurrentPlayer->healthdisplaytime;
    g_SavedBondState.damagetype = g_CurrentPlayer->damagetype;
    g_SavedBondState.field_8C = g_CurrentPlayer->field_8C;

    // Multiplayer, Stats, and Viewport Height/Dampening State Tracking
    g_SavedBondState.healthDamageType = g_CurrentPlayer->healthDamageType;
    g_SavedBondState.field_29BC = g_CurrentPlayer->field_29BC;
    g_SavedBondState.field_29C0 = g_CurrentPlayer->field_29C0;
    g_SavedBondState.mpmenuon = g_CurrentPlayer->mpmenuon;
    g_SavedBondState.mpmenumode = g_CurrentPlayer->mpmenumode;
    g_SavedBondState.mpquitconfirm = g_CurrentPlayer->mpquitconfirm;
    g_SavedBondState.mpjoywascentre = g_CurrentPlayer->mpjoywascentre;
    g_SavedBondState.deathcount = g_CurrentPlayer->deathcount;
    g_SavedBondState.num_suicides = g_CurrentPlayer->num_suicides;
    g_SavedBondState.field_29E0 = g_CurrentPlayer->field_29E0;
    {
        s32 i;
        for (i = 0; i < 4; i++) {
            g_SavedBondState.last_kill_time[i] = g_CurrentPlayer->last_kill_time[i];
        }
    }
    g_SavedBondState.field_29F4 = g_CurrentPlayer->field_29F4;
    g_SavedBondState.field_29F8 = g_CurrentPlayer->field_29F8;

    // Tank Physics and Turret State Tracking Fields
    g_SavedBondState.in_tank_flag = in_tank_flag;
    g_SavedBondState.g_PlayerTankProp_index = get_prop_index(g_PlayerTankProp);
    g_SavedBondState.g_WorldTankProp_index = get_prop_index(g_WorldTankProp);
    g_SavedBondState.g_PlayerTankYOffset = g_PlayerTankYOffset;
    g_SavedBondState.g_TankTurnSpeed = g_TankTurnSpeed;
    g_SavedBondState.g_TankOrientationAngle = g_TankOrientationAngle;
    g_SavedBondState.tank_turret_unused_angle = tank_turret_unused_angle;
    g_SavedBondState.g_TankTurretVerticalAngle = g_TankTurretVerticalAngle;
    g_SavedBondState.g_TankTurretVerticalAngleRelated = g_TankTurretVerticalAngleRelated;
    g_SavedBondState.g_TankTurretOrientationAngleRad = g_TankTurretOrientationAngleRad;
    g_SavedBondState.g_TankTurretOrientationAngleDeg = g_TankTurretOrientationAngleDeg;
    g_SavedBondState.tank_turret_turn_speed = tank_turret_turn_speed;
    g_SavedBondState.g_BondCanEnterTank = g_BondCanEnterTank;
    g_SavedBondState.g_TankTurretAngle = g_TankTurretAngle;
    g_SavedBondState.g_TankTurretTurn = g_TankTurretTurn;
    g_SavedBondState.g_ExplodeTankOnDeathFlag = g_ExplodeTankOnDeathFlag;
    g_SavedBondState.g_TankDamagePenaltyTicks = g_TankDamagePenaltyTicks;
    g_SavedBondState.g_EnterTankAudioState = g_EnterTankAudioState;

    // Camera Mode and Cutscene Transition States
    g_SavedBondState.g_CameraMode = g_CameraMode;
    g_SavedBondState.camera_mode = camera_mode;
    g_SavedBondState.g_CameraAfterCinema = g_CameraAfterCinema;
    g_SavedBondState.camera_transition_timer = camera_transition_timer;
    g_SavedBondState.camera_fade_active = camera_fade_active;
    g_SavedBondState.intro_camera_index = intro_camera_index;
    g_SavedBondState.is_timer_active = is_timer_active;
    g_SavedBondState.g_PlayerInvincible = g_PlayerInvincible;

    // Head & Hand Animation/Rendering Progress Flags
    g_SavedBondState.field_5C0 = g_CurrentPlayer->field_5C0;
    g_SavedBondState.field_FC8 = g_CurrentPlayer->field_FC8;

    // Visual layout matrices / switches padding (excluding field_654 which is a runtime data pointer)
    g_SavedBondState.field_59C = g_CurrentPlayer->field_59C;
    g_SavedBondState.field_5A0 = g_CurrentPlayer->field_5A0;
    g_SavedBondState.field_5A4 = g_CurrentPlayer->field_5A4;
    g_SavedBondState.field_5A8 = g_CurrentPlayer->field_5A8;
    g_SavedBondState.field_5AC = g_CurrentPlayer->field_5AC;
    g_SavedBondState.field_5B0 = g_CurrentPlayer->field_5B0;
    g_SavedBondState.field_5B4 = g_CurrentPlayer->field_5B4;
    g_SavedBondState.field_5B8 = g_CurrentPlayer->field_5B8;
    g_SavedBondState.field_5C4 = g_CurrentPlayer->field_5C4;
    g_SavedBondState.field_5C8 = g_CurrentPlayer->field_5C8;
    g_SavedBondState.field_5CC = g_CurrentPlayer->field_5CC;
    g_SavedBondState.field_5D0 = g_CurrentPlayer->field_5D0;
    g_SavedBondState.field_5D4 = g_CurrentPlayer->field_5D4;
    g_SavedBondState.field_5D8 = g_CurrentPlayer->field_5D8;
    g_SavedBondState.field_5DC = g_CurrentPlayer->field_5DC;
    g_SavedBondState.field_5E0 = g_CurrentPlayer->field_5E0;
    g_SavedBondState.field_5E4 = g_CurrentPlayer->field_5E4;
    g_SavedBondState.field_5E8 = g_CurrentPlayer->field_5E8;
    g_SavedBondState.field_5EC = g_CurrentPlayer->field_5EC;
    g_SavedBondState.field_5F0 = g_CurrentPlayer->field_5F0;
    g_SavedBondState.field_5F4 = g_CurrentPlayer->field_5F4;
    g_SavedBondState.field_5F8 = g_CurrentPlayer->field_5F8;
    g_SavedBondState.field_5FC = g_CurrentPlayer->field_5FC;
    g_SavedBondState.field_600 = g_CurrentPlayer->field_600;
    g_SavedBondState.field_604 = g_CurrentPlayer->field_604;
    g_SavedBondState.field_608 = g_CurrentPlayer->field_608;
    g_SavedBondState.field_60C = g_CurrentPlayer->field_60C;
    g_SavedBondState.field_610 = g_CurrentPlayer->field_610;
    g_SavedBondState.field_614 = g_CurrentPlayer->field_614;
    g_SavedBondState.field_618 = g_CurrentPlayer->field_618;
    g_SavedBondState.field_61C = g_CurrentPlayer->field_61C;
    g_SavedBondState.field_620 = g_CurrentPlayer->field_620;
    g_SavedBondState.field_624 = g_CurrentPlayer->field_624;
    g_SavedBondState.field_628 = g_CurrentPlayer->field_628;
    g_SavedBondState.field_62C = g_CurrentPlayer->field_62C;
    g_SavedBondState.field_630 = g_CurrentPlayer->field_630;
    g_SavedBondState.field_634 = g_CurrentPlayer->field_634;
    g_SavedBondState.field_638 = g_CurrentPlayer->field_638;
    g_SavedBondState.field_63C = g_CurrentPlayer->field_63C;
    g_SavedBondState.field_640 = g_CurrentPlayer->field_640;
    g_SavedBondState.field_644 = g_CurrentPlayer->field_644;
    g_SavedBondState.field_648 = g_CurrentPlayer->field_648;
    g_SavedBondState.field_64C = g_CurrentPlayer->field_64C;
    g_SavedBondState.field_650 = g_CurrentPlayer->field_650;
    g_SavedBondState.field_658 = g_CurrentPlayer->field_658;
    g_SavedBondState.field_65C = g_CurrentPlayer->field_65C;
    g_SavedBondState.field_660 = g_CurrentPlayer->field_660;
    g_SavedBondState.field_664 = g_CurrentPlayer->field_664;
    g_SavedBondState.field_668 = g_CurrentPlayer->field_668;
    g_SavedBondState.field_66C = g_CurrentPlayer->field_66C;
    g_SavedBondState.field_670 = g_CurrentPlayer->field_670;
    g_SavedBondState.field_674 = g_CurrentPlayer->field_674;
    g_SavedBondState.field_678 = g_CurrentPlayer->field_678;
    g_SavedBondState.field_67C = g_CurrentPlayer->field_67C;
    g_SavedBondState.field_680 = g_CurrentPlayer->field_680;
    g_SavedBondState.field_684 = g_CurrentPlayer->field_684;
    g_SavedBondState.field_688 = g_CurrentPlayer->field_688;
    g_SavedBondState.field_68C = g_CurrentPlayer->field_68C;
    g_SavedBondState.field_690 = g_CurrentPlayer->field_690;
    g_SavedBondState.field_694 = g_CurrentPlayer->field_694;
    g_SavedBondState.field_698 = g_CurrentPlayer->field_698;
    g_SavedBondState.field_69C = g_CurrentPlayer->field_69C;
    g_SavedBondState.field_6A0 = g_CurrentPlayer->field_6A0;
    g_SavedBondState.field_6A4 = g_CurrentPlayer->field_6A4;
    g_SavedBondState.field_6A8 = g_CurrentPlayer->field_6A8;
    g_SavedBondState.field_6AC = g_CurrentPlayer->field_6AC;
    g_SavedBondState.field_6B0 = g_CurrentPlayer->field_6B0;
    g_SavedBondState.field_6B4 = g_CurrentPlayer->field_6B4;
    g_SavedBondState.field_6B8 = g_CurrentPlayer->field_6B8;
    g_SavedBondState.field_6BC = g_CurrentPlayer->field_6BC;
    g_SavedBondState.field_6C0 = g_CurrentPlayer->field_6C0;
    g_SavedBondState.field_6C4 = g_CurrentPlayer->field_6C4;
    g_SavedBondState.field_6C8 = g_CurrentPlayer->field_6C8;
    g_SavedBondState.field_6CC = g_CurrentPlayer->field_6CC;
}

void load_bond_state(void) {
    if (g_CurrentPlayer == NULL) {
        return;
    }

    // Control / Camera State
    g_CurrentPlayer->unknown = g_SavedBondState.unknown;

    // Position
    g_CurrentPlayer->pos = g_SavedBondState.pos;
    g_CurrentPlayer->pos2 = g_SavedBondState.pos2;
    g_CurrentPlayer->pos3 = g_SavedBondState.pos3;
    g_CurrentPlayer->offset = g_SavedBondState.offset;
    g_CurrentPlayer->room_pointer = get_tile_by_offset(g_SavedBondState.room_pointer_offset);
    g_CurrentPlayer->current_model_pos = g_SavedBondState.current_model_pos;
    g_CurrentPlayer->previous_model_pos = g_SavedBondState.previous_model_pos;
    g_CurrentPlayer->current_room_pos = g_SavedBondState.current_room_pos;
    g_CurrentPlayer->curRoomIndex = g_SavedBondState.curRoomIndex;
    g_CurrentPlayer->bondprevpos = g_SavedBondState.bondprevpos;

    // Collision
    g_CurrentPlayer->field_488 = g_SavedBondState.field_488;
    g_CurrentPlayer->field_488.current_tile_ptr = get_tile_by_offset(g_SavedBondState.field_488_current_tile_ptr_offset);
    g_CurrentPlayer->field_488.current_tile_ptr_for_portals = get_tile_by_offset(g_SavedBondState.field_488_current_tile_ptr_for_portals_offset);

    g_CurrentPlayer->previous_collision_info = g_SavedBondState.previous_collision_info;
    g_CurrentPlayer->previous_collision_info.current_tile_ptr = get_tile_by_offset(g_SavedBondState.previous_collision_info_current_tile_ptr_offset);
    g_CurrentPlayer->previous_collision_info.current_tile_ptr_for_portals = get_tile_by_offset(g_SavedBondState.previous_collision_info_current_tile_ptr_for_portals_offset);

    g_CurrentPlayer->field_2A6C = g_SavedBondState.field_2A6C;
    g_CurrentPlayer->field_2A70 = get_tile_by_offset(g_SavedBondState.field_2A70_offset);

    // Angles
    g_CurrentPlayer->vv_theta = g_SavedBondState.vv_theta;
    g_CurrentPlayer->vv_verta = g_SavedBondState.vv_verta;

    // Recalculate derived look parameters from new vv_theta / vv_verta
    bondviewApplyVertaTheta();

    // Movement speeds / physics
    g_CurrentPlayer->speedsideways = g_SavedBondState.speedsideways;
    g_CurrentPlayer->speedstrafe = g_SavedBondState.speedstrafe;
    g_CurrentPlayer->speedforwards = g_SavedBondState.speedforwards;
    g_CurrentPlayer->speedmaxtime60 = g_SavedBondState.speedmaxtime60;
    g_CurrentPlayer->speedboost = g_SavedBondState.speedboost;
    g_CurrentPlayer->vertical_bounce_adjust = g_SavedBondState.vertical_bounce_adjust;
    g_CurrentPlayer->ducking_height_offset = g_SavedBondState.ducking_height_offset;
    g_CurrentPlayer->crouchpos = g_SavedBondState.crouchpos;
    g_CurrentPlayer->field_A4 = g_SavedBondState.field_A4;
    g_CurrentPlayer->field_AC = g_SavedBondState.field_AC;
    g_CurrentPlayer->collision_bounds = g_CurrentPlayer->collision_bounds;

    // 3D Velocity
    g_CurrentPlayer->field_78 = g_SavedBondState.field_78;
    g_CurrentPlayer->field_7C = g_SavedBondState.field_7C;
    g_CurrentPlayer->field_80 = g_SavedBondState.field_80;
    g_CurrentPlayer->speedgo = g_SavedBondState.speedgo;
    g_CurrentPlayer->bondshotspeed = g_SavedBondState.bondshotspeed;
    g_CurrentPlayer->speedtheta = g_SavedBondState.speedtheta;
    g_CurrentPlayer->speedverta = g_SavedBondState.speedverta;

    // Bounce Dampeners
    g_CurrentPlayer->field_84 = g_SavedBondState.field_84;
    g_CurrentPlayer->field_88 = g_SavedBondState.field_88;

    // Height Tracking
    g_CurrentPlayer->field_6C = g_SavedBondState.field_6C;
    g_CurrentPlayer->field_70 = g_SavedBondState.field_70;
    g_CurrentPlayer->stanHeight = g_SavedBondState.stanHeight;

    // Smoothed / Render Coordinates
    g_CurrentPlayer->field_3B8 = g_SavedBondState.field_3B8;
    g_CurrentPlayer->field_3C4 = g_SavedBondState.field_3C4;
    g_CurrentPlayer->field_3C8 = g_SavedBondState.field_3C8;
    g_CurrentPlayer->field_3CC = g_SavedBondState.field_3CC;

    // Walk / Head Bob Animation State
    g_CurrentPlayer->resetheadpos = g_SavedBondState.resetheadpos;
    g_CurrentPlayer->resetheadrot = g_SavedBondState.resetheadrot;
    g_CurrentPlayer->resetheadtick = g_SavedBondState.resetheadtick;
    g_CurrentPlayer->headanim = g_SavedBondState.headanim;
    g_CurrentPlayer->headdamp = g_SavedBondState.headdamp;
    g_CurrentPlayer->headwalkingtime60 = g_SavedBondState.headwalkingtime60;
    g_CurrentPlayer->headamplitude = g_SavedBondState.headamplitude;
    g_CurrentPlayer->sideamplitude = g_SavedBondState.sideamplitude;
    g_CurrentPlayer->headpos = g_SavedBondState.headpos;
    g_CurrentPlayer->headlook = g_SavedBondState.headlook;
    g_CurrentPlayer->headup = g_CurrentPlayer->headup;
    g_CurrentPlayer->headpossum = g_SavedBondState.headpossum;
    g_CurrentPlayer->headlooksum = g_SavedBondState.headlooksum;
    g_CurrentPlayer->headupsum = g_SavedBondState.headupsum;
    g_CurrentPlayer->headbodyoffset = g_SavedBondState.headbodyoffset;
    g_CurrentPlayer->standheight = g_SavedBondState.standheight;
    g_CurrentPlayer->standbodyoffset = g_SavedBondState.standbodyoffset;
    g_CurrentPlayer->standfrac = g_SavedBondState.standfrac;
    g_CurrentPlayer->standlook[0] = g_SavedBondState.standlook[0];
    g_CurrentPlayer->standlook[1] = g_SavedBondState.standlook[1];
    g_CurrentPlayer->standup[0] = g_SavedBondState.standup[0];
    g_CurrentPlayer->standup[1] = g_SavedBondState.standup[1];
    g_CurrentPlayer->standcnt = g_SavedBondState.standcnt;
    g_CurrentPlayer->animFlipFlag = g_SavedBondState.animFlipFlag;
    g_CurrentPlayer->field_5BD = g_SavedBondState.field_5BD;
    g_CurrentPlayer->field_5BE = g_SavedBondState.field_5BE;
    g_CurrentPlayer->field_5BF = g_SavedBondState.field_5BF;
    {
        s32 i;
        for (i = 0; i < 4; i++) {
            g_CurrentPlayer->bondheadmatrices[i] = g_SavedBondState.bondheadmatrices[i];
        }
    }

    // Health / Armor
    g_CurrentPlayer->bondhealth = g_SavedBondState.bondhealth;
    g_CurrentPlayer->bondarmour = g_SavedBondState.bondarmour;
    g_CurrentPlayer->oldhealth = g_SavedBondState.oldhealth;
    g_CurrentPlayer->oldarmour = g_SavedBondState.oldarmour;
    g_CurrentPlayer->apparenthealth = g_SavedBondState.apparenthealth;
    g_CurrentPlayer->apparentarmour = g_SavedBondState.apparentarmour;

    // Sync player's world PropRecord
    if (g_SavedBondState.has_prop) {
        if (g_CurrentPlayer->prop != NULL) {
            chrpropDeregisterRooms(g_CurrentPlayer->prop);

            g_CurrentPlayer->prop->pos = g_SavedBondState.prop_pos;
            g_CurrentPlayer->prop->stan = get_tile_by_offset(g_SavedBondState.prop_stan_offset);
            g_CurrentPlayer->prop->rooms[0] = g_SavedBondState.prop_rooms[0];
            g_CurrentPlayer->prop->rooms[1] = g_SavedBondState.prop_rooms[1];
            g_CurrentPlayer->prop->rooms[2] = g_SavedBondState.prop_rooms[2];
            g_CurrentPlayer->prop->rooms[3] = g_SavedBondState.prop_rooms[3];

            chrpropRegisterRooms(g_CurrentPlayer->prop);
        } else {
            g_CurrentPlayer->prop = chrpropAllocate();
            if (g_CurrentPlayer->prop != NULL) {
                g_CurrentPlayer->prop->obj = NULL;
                g_CurrentPlayer->prop->type = PROP_TYPE_VIEWER;
                g_CurrentPlayer->prop->pos = g_SavedBondState.prop_pos;
                g_CurrentPlayer->prop->stan = get_tile_by_offset(g_SavedBondState.prop_stan_offset);
                g_CurrentPlayer->prop->rooms[0] = g_SavedBondState.prop_rooms[0];
                g_CurrentPlayer->prop->rooms[1] = g_SavedBondState.prop_rooms[1];
                g_CurrentPlayer->prop->rooms[2] = g_SavedBondState.prop_rooms[2];
                g_CurrentPlayer->prop->rooms[3] = g_SavedBondState.prop_rooms[3];

                chrpropActivate(g_CurrentPlayer->prop);
                chrpropEnable(g_CurrentPlayer->prop);
                chrpropRegisterRooms(g_CurrentPlayer->prop);
            }
        }
    } else {
        if (g_CurrentPlayer->prop != NULL) {
            chrpropDeregisterRooms(g_CurrentPlayer->prop);
            chrpropDelist(g_CurrentPlayer->prop);
            chrpropDisable(g_CurrentPlayer->prop);
            chrpropFree(g_CurrentPlayer->prop);
            g_CurrentPlayer->prop = NULL;
        }
    }

    // Sync player's 3D visual Model instance
    if (g_CurrentPlayer->ptr_char_objectinstance != NULL) {
        setsuboffset(g_CurrentPlayer->ptr_char_objectinstance, &g_CurrentPlayer->pos);
        setsubroty(g_CurrentPlayer->ptr_char_objectinstance, get_curplay_horizontal_rotation_in_degrees());
    }

    // Re-initialize player inventory, preserving text overrides
    {
        textoverride *saved_overrides = g_CurrentPlayer->textoverrides;
        bondinvReinitInv();
        g_CurrentPlayer->textoverrides = saved_overrides;
    }
    g_CurrentPlayer->equipallguns = g_SavedBondState.equipallguns;

    // Restore saved inventory items
    {
        s32 i;
        for (i = 0; i < g_SavedBondState.num_inv_items; i++) {
            s32 type = g_SavedBondState.inv_items[i].type;
            if (type == INV_ITEM_WEAPON) {
                bondinvAddInvItem(g_SavedBondState.inv_items[i].weapon_right);
            } else if (type == INV_ITEM_DUAL) {
                bondinvAddDoublesInvItem(g_SavedBondState.inv_items[i].weapon_right, g_SavedBondState.inv_items[i].weapon_left);
            } else if (type == INV_ITEM_PROP) {
                PropRecord *prop = get_prop_by_index(g_SavedBondState.inv_items[i].prop_index);
                if (prop != NULL) {
                    bondinvAddPropToInv(prop);
                }
            }
        }
    }

    // Restore ammo counts
    {
        s32 i;
        for (i = 0; i < 30; i++) {
            g_CurrentPlayer->ammoheldarr[i] = g_SavedBondState.ammoheldarr[i];
        }
    }

    // Restore equipcuritem
    g_CurrentPlayer->equipcuritem = g_SavedBondState.equipcuritem;

    // Restore weapon transition buffers before updating hand items
    g_CurrentPlayer->field_2A44[0] = g_SavedBondState.field_2A44[0];
    g_CurrentPlayer->field_2A44[1] = g_SavedBondState.field_2A44[1];

    // Restore hand weapon logical state and trigger model loading directly
    {
        s32 hand;
        for (hand = 0; hand < 2; hand++) {
            ITEM_IDS saved_weapon = g_SavedBondState.hands_weaponnum[hand];

            if (g_CurrentPlayer->hand_item[hand] != saved_weapon) {
                g_CurrentPlayer->hand_invisible[hand] = -3;
                g_CurrentPlayer->field_2A44[hand] = saved_weapon;
                g_CurrentPlayer->hand_item[hand] = ITEM_UNARMED;
                g_CurrentPlayer->hands[hand].weapon_current_animation = 0;
                g_CurrentPlayer->lock_hand_model[hand] = 0;
                used_to_load_1st_person_model_on_demand(hand);
            } else {
                g_CurrentPlayer->hands[hand].weapon_current_animation = g_SavedBondState.hands_weapon_current_animation[hand];
            }

            g_CurrentPlayer->hands[hand].weaponnum = saved_weapon;
            g_CurrentPlayer->hands[hand].weaponnum_watchmenu = g_SavedBondState.hands_weaponnum_watchmenu[hand];
            g_CurrentPlayer->hands[hand].previous_weapon = g_SavedBondState.hands_previous_weapon[hand];
            g_CurrentPlayer->hands[hand].weapon_firing_status = g_SavedBondState.hands_weapon_firing_status[hand];
            g_CurrentPlayer->hands[hand].weapon_hold_time = g_SavedBondState.hands_weapon_hold_time[hand];
            g_CurrentPlayer->hands[hand].when_detonating_mines_is_0 = g_SavedBondState.hands_when_detonating_mines_is_0[hand];
            g_CurrentPlayer->hands[hand].weapon_ammo_in_magazine = g_SavedBondState.hands_weapon_ammo_in_magazine[hand];
            g_CurrentPlayer->hands[hand].weapon_next_weapon = g_SavedBondState.hands_weapon_next_weapon[hand];
            g_CurrentPlayer->hands[hand].weapon_animation_trigger = g_SavedBondState.hands_weapon_animation_trigger[hand];
            g_CurrentPlayer->hands[hand].field_8B8 = g_SavedBondState.hands_field_8B8[hand];
            g_CurrentPlayer->lock_hand_model[hand] = g_SavedBondState.lock_hand_model[hand];
            g_CurrentPlayer->hands[hand].audioHandle = NULL;
        }
    }

    // Cheat status
    g_CurrentPlayer->bondinvincible = g_SavedBondState.bondinvincible;

    // HUD Display states
    g_CurrentPlayer->damageshowtime = g_SavedBondState.damageshowtime;
    g_CurrentPlayer->healthshowtime = g_SavedBondState.healthshowtime;
    g_CurrentPlayer->healthshowmode = g_SavedBondState.healthshowmode;

    // Fading & Breathing
    g_CurrentPlayer->bondfadetime60 = g_SavedBondState.bondfadetime60;
    g_CurrentPlayer->bondfadetimemax60 = g_SavedBondState.bondfadetimemax60;
    g_CurrentPlayer->bondfadefracold = g_SavedBondState.bondfadefracold;
    g_CurrentPlayer->bondfadefracnew = g_SavedBondState.bondfadefracnew;
    g_CurrentPlayer->bondbreathing = g_SavedBondState.bondbreathing;

    // Padding between breathing and watch state
    g_CurrentPlayer->field_1A0 = g_SavedBondState.field_1A0;
    g_CurrentPlayer->field_1A4 = g_SavedBondState.field_1A4;
    g_CurrentPlayer->field_1A8 = g_SavedBondState.field_1A8;
    g_CurrentPlayer->field_1AC = g_SavedBondState.field_1AC;
    g_CurrentPlayer->field_1B0 = g_SavedBondState.field_1B0;
    g_CurrentPlayer->field_1B4 = g_SavedBondState.field_1B4;
    g_CurrentPlayer->field_1B8 = g_SavedBondState.field_1B8;
    g_CurrentPlayer->field_1BC = g_SavedBondState.field_1BC;

    // Weapon Sway cycle
    g_CurrentPlayer->gunposamplitude = g_SavedBondState.gunposamplitude;
    g_CurrentPlayer->gunxamplitude = g_SavedBondState.gunxamplitude;
    g_CurrentPlayer->gunsync = g_SavedBondState.gunsync;
    g_CurrentPlayer->syncchange = g_SavedBondState.syncchange;
    g_CurrentPlayer->synccount = g_SavedBondState.synccount;
    g_CurrentPlayer->syncoffset = g_SavedBondState.syncoffset;
    g_CurrentPlayer->field_107C = g_SavedBondState.field_107C;
    g_CurrentPlayer->field_1080 = g_SavedBondState.field_1080;

    // Buttons Input State
    g_CurrentPlayer->buttons_pressed = g_SavedBondState.buttons_pressed;
    g_CurrentPlayer->prev_buttons_pressed = g_SavedBondState.prev_buttons_pressed;

    // Death State
    g_CurrentPlayer->bonddead = g_SavedBondState.bonddead;
    g_CurrentPlayer->thetadie = g_SavedBondState.thetadie;
    g_CurrentPlayer->vertadie = g_SavedBondState.vertadie;
    g_CurrentPlayer->bondtype = g_SavedBondState.bondtype;
    g_CurrentPlayer->startnewbonddie = g_SavedBondState.startnewbonddie;
    g_CurrentPlayer->redbloodfinished = g_SavedBondState.redbloodfinished;
    g_CurrentPlayer->deathanimfinished = g_SavedBondState.deathanimfinished;
    g_CurrentPlayer->field_42c = g_SavedBondState.field_42c;
    g_CurrentPlayer->controldef = g_SavedBondState.controldef;
    g_CurrentPlayer->field_D0 = g_SavedBondState.field_D0;

    // Look-Centering State
    g_CurrentPlayer->movecentrerelease = g_SavedBondState.movecentrerelease;
    g_CurrentPlayer->lookaheadcentreenabled = g_SavedBondState.lookaheadcentreenabled;
    g_CurrentPlayer->automovecentreenabled = g_SavedBondState.automovecentreenabled;
    g_CurrentPlayer->fastmovecentreenabled = g_SavedBondState.fastmovecentreenabled;
    g_CurrentPlayer->automovecentre = g_SavedBondState.automovecentre;
    g_CurrentPlayer->field_104 = g_SavedBondState.field_104;
    g_CurrentPlayer->field_10C = g_SavedBondState.field_10C;

    // Unreferenced / unknown state fields
    g_CurrentPlayer->field_100 = g_SavedBondState.field_100;
    g_CurrentPlayer->field_108 = g_SavedBondState.field_108;

    // Weapon Sway & Control State
    g_CurrentPlayer->magnetattracttime = g_SavedBondState.magnetattracttime;
    g_CurrentPlayer->swaytarget = g_SavedBondState.swaytarget;
    g_CurrentPlayer->swayoffset0 = g_SavedBondState.swayoffset0;
    g_CurrentPlayer->swayoffset2 = g_SavedBondState.swayoffset2;
    g_CurrentPlayer->field_1280 = g_SavedBondState.field_1280;
    g_CurrentPlayer->players_cur_animation = g_SavedBondState.players_cur_animation;
    g_CurrentPlayer->field_1288 = g_SavedBondState.field_1288;

    // Watch & Pause Menu State
    g_CurrentPlayer->watch_pause_time = g_SavedBondState.watch_pause_time;
    g_CurrentPlayer->timer_1C4 = g_SavedBondState.timer_1C4;
    g_CurrentPlayer->watch_animation_state = g_SavedBondState.watch_animation_state;
    g_CurrentPlayer->outside_watch_menu = g_SavedBondState.outside_watch_menu;
    g_CurrentPlayer->open_close_solo_watch_menu = g_SavedBondState.open_close_solo_watch_menu;
    g_CurrentPlayer->pausing_flag = g_SavedBondState.pausing_flag;
    g_CurrentPlayer->pause_starting_angle = g_SavedBondState.pause_starting_angle;
    g_CurrentPlayer->pause_target_angle = g_SavedBondState.pause_target_angle;
    g_CurrentPlayer->pause_related = g_SavedBondState.pause_related;
    g_CurrentPlayer->pause_animation_counter = g_SavedBondState.pause_animation_counter;
    g_CurrentPlayer->step_in_view_watch_animation = g_SavedBondState.step_in_view_watch_animation;
    g_CurrentPlayer->pause_watch_related = g_SavedBondState.pause_watch_related;
    g_CurrentPlayer->pause_watch_related_scaled = g_SavedBondState.pause_watch_related_scaled;
    g_CurrentPlayer->pause_watch_related_adjust = g_SavedBondState.pause_watch_related_adjust;
    g_CurrentPlayer->field_218 = g_SavedBondState.field_218;
    g_CurrentPlayer->field_21C = g_SavedBondState.field_21C;
    g_CurrentPlayer->field_1D4 = g_SavedBondState.field_1D4;
    g_CurrentPlayer->field_1D8 = g_SavedBondState.field_1D8;
    g_CurrentPlayer->pause_watch_position = g_SavedBondState.pause_watch_position;
    g_CurrentPlayer->field_1E0 = g_SavedBondState.field_1E0;
    g_CurrentPlayer->field_1E4 = g_SavedBondState.field_1E4;
    g_CurrentPlayer->field_1E8 = g_SavedBondState.field_1E8;
    g_CurrentPlayer->field_1EC = g_SavedBondState.field_1EC;
    g_CurrentPlayer->field_1F0 = g_SavedBondState.field_1F0;
    g_CurrentPlayer->field_1F4 = g_SavedBondState.field_1F4;
    g_CurrentPlayer->field_1F8 = g_SavedBondState.field_1F8;
    g_CurrentPlayer->field_1FC = g_SavedBondState.field_1FC;
    g_CurrentPlayer->field_210 = g_SavedBondState.field_210;
    g_CurrentPlayer->field_214 = g_SavedBondState.field_214;
    g_CurrentPlayer->something_with_watch_object_instance = g_SavedBondState.something_with_watch_object_instance;
    g_CurrentPlayer->field_234 = g_SavedBondState.field_234;
    g_CurrentPlayer->field_238 = g_SavedBondState.field_238;
    g_CurrentPlayer->field_23C = g_SavedBondState.field_23C;
    g_CurrentPlayer->field_240 = g_SavedBondState.field_240;
    g_CurrentPlayer->watch_scale_destination = g_SavedBondState.watch_scale_destination;
    g_CurrentPlayer->field_248 = g_SavedBondState.field_248;
    g_CurrentPlayer->field_24C = g_SavedBondState.field_24C;
    g_CurrentPlayer->field_250 = g_SavedBondState.field_250;
    g_CurrentPlayer->field_254 = g_SavedBondState.field_254;

    // Watch state padding / unknown state fields (offsets 0x25C–0x3B0)
    g_CurrentPlayer->field_25C = g_SavedBondState.field_25C;
    g_CurrentPlayer->field_260 = g_SavedBondState.field_260;
    g_CurrentPlayer->field_264 = g_SavedBondState.field_264;
    g_CurrentPlayer->field_268 = g_SavedBondState.field_268;
    g_CurrentPlayer->field_26C = g_SavedBondState.field_26C;
    g_CurrentPlayer->field_270 = g_SavedBondState.field_270;
    g_CurrentPlayer->field_274 = g_SavedBondState.field_274;
    g_CurrentPlayer->field_278 = g_SavedBondState.field_278;
    g_CurrentPlayer->field_27C = g_SavedBondState.field_27C;
    g_CurrentPlayer->field_280 = g_SavedBondState.field_280;
    g_CurrentPlayer->field_284 = g_SavedBondState.field_284;
    g_CurrentPlayer->field_288 = g_SavedBondState.field_288;
    g_CurrentPlayer->field_28C = g_SavedBondState.field_28C;
    g_CurrentPlayer->field_290 = g_SavedBondState.field_290;
    g_CurrentPlayer->field_294 = g_SavedBondState.field_294;
    g_CurrentPlayer->field_298 = g_SavedBondState.field_298;
    g_CurrentPlayer->field_29C = g_SavedBondState.field_29C;
    g_CurrentPlayer->field_2A0 = g_SavedBondState.field_2A0;
    g_CurrentPlayer->field_2A4 = g_SavedBondState.field_2A4;
    g_CurrentPlayer->field_2A8 = g_SavedBondState.field_2A8;
    g_CurrentPlayer->field_2AC = g_SavedBondState.field_2AC;
    g_CurrentPlayer->field_2B0 = g_SavedBondState.field_2B0;
    g_CurrentPlayer->field_2B4 = g_SavedBondState.field_2B4;
    g_CurrentPlayer->field_2B8 = g_SavedBondState.field_2B8;
    g_CurrentPlayer->field_2BC = g_SavedBondState.field_2BC;
    g_CurrentPlayer->field_2C0 = g_SavedBondState.field_2C0;
    g_CurrentPlayer->field_2C4 = g_SavedBondState.field_2C4;
    g_CurrentPlayer->field_2C8 = g_SavedBondState.field_2C8;
    g_CurrentPlayer->field_2CC = g_SavedBondState.field_2CC;
    g_CurrentPlayer->field_2D0 = g_SavedBondState.field_2D0;
    g_CurrentPlayer->field_2D4 = g_SavedBondState.field_2D4;
    g_CurrentPlayer->field_2D8 = g_SavedBondState.field_2D8;
    g_CurrentPlayer->field_2DC = g_SavedBondState.field_2DC;
    g_CurrentPlayer->field_2E0 = g_SavedBondState.field_2E0;
    g_CurrentPlayer->field_2E4 = g_SavedBondState.field_2E4;
    g_CurrentPlayer->field_2E8 = g_SavedBondState.field_2E8;
    g_CurrentPlayer->field_2EC = g_SavedBondState.field_2EC;
    g_CurrentPlayer->field_2F0 = g_SavedBondState.field_2F0;
    g_CurrentPlayer->field_2F4 = g_SavedBondState.field_2F4;
    g_CurrentPlayer->field_2F8 = g_SavedBondState.field_2F8;
    g_CurrentPlayer->field_2FC = g_SavedBondState.field_2FC;
    g_CurrentPlayer->field_300 = g_SavedBondState.field_300;
    g_CurrentPlayer->field_304 = g_SavedBondState.field_304;
    g_CurrentPlayer->field_308 = g_SavedBondState.field_308;
    g_CurrentPlayer->field_30C = g_SavedBondState.field_30C;
    g_CurrentPlayer->field_310 = g_SavedBondState.field_310;
    g_CurrentPlayer->field_314 = g_SavedBondState.field_314;
    g_CurrentPlayer->field_318 = g_SavedBondState.field_318;
    g_CurrentPlayer->field_31C = g_SavedBondState.field_31C;
    g_CurrentPlayer->field_320 = g_SavedBondState.field_320;
    g_CurrentPlayer->field_324 = g_SavedBondState.field_324;
    g_CurrentPlayer->field_328 = g_SavedBondState.field_328;
    g_CurrentPlayer->field_32C = g_SavedBondState.field_32C;
    g_CurrentPlayer->field_330 = g_SavedBondState.field_330;
    g_CurrentPlayer->field_334 = g_SavedBondState.field_334;
    g_CurrentPlayer->field_338 = g_SavedBondState.field_338;
    g_CurrentPlayer->field_33C = g_SavedBondState.field_33C;
    g_CurrentPlayer->field_340 = g_SavedBondState.field_340;
    g_CurrentPlayer->field_344 = g_SavedBondState.field_344;
    g_CurrentPlayer->field_348 = g_SavedBondState.field_348;
    g_CurrentPlayer->field_34C = g_SavedBondState.field_34C;
    g_CurrentPlayer->field_350 = g_SavedBondState.field_350;
    g_CurrentPlayer->field_354 = g_SavedBondState.field_354;
    g_CurrentPlayer->field_358 = g_SavedBondState.field_358;
    g_CurrentPlayer->field_35C = g_SavedBondState.field_35C;
    g_CurrentPlayer->field_360 = g_SavedBondState.field_360;
    g_CurrentPlayer->field_364 = g_SavedBondState.field_364;
    g_CurrentPlayer->field_368 = g_SavedBondState.field_368;
    g_CurrentPlayer->field_36C = g_SavedBondState.field_36C;
    g_CurrentPlayer->field_370 = g_SavedBondState.field_370;
    g_CurrentPlayer->field_374 = g_SavedBondState.field_374;
    g_CurrentPlayer->field_378 = g_SavedBondState.field_378;
    g_CurrentPlayer->field_37C = g_SavedBondState.field_37C;
    g_CurrentPlayer->field_380 = g_SavedBondState.field_380;
    g_CurrentPlayer->field_384 = g_SavedBondState.field_384;
    g_CurrentPlayer->field_388 = g_SavedBondState.field_388;
    g_CurrentPlayer->field_38C = g_SavedBondState.field_38C;
    g_CurrentPlayer->field_390 = g_SavedBondState.field_390;
    g_CurrentPlayer->field_394 = g_SavedBondState.field_394;
    g_CurrentPlayer->field_398 = g_SavedBondState.field_398;
    g_CurrentPlayer->field_39C = g_SavedBondState.field_39C;
    g_CurrentPlayer->field_3A0 = g_SavedBondState.field_3A0;
    g_CurrentPlayer->field_3A4 = g_SavedBondState.field_3A4;
    g_CurrentPlayer->field_3A8 = g_SavedBondState.field_3A8;
    g_CurrentPlayer->field_3AC = g_SavedBondState.field_3AC;
    g_CurrentPlayer->field_3B0 = g_SavedBondState.field_3B0;

    // Firing & Hand rendering visibility States
    g_CurrentPlayer->z_trigger_timer = g_SavedBondState.z_trigger_timer;
    g_CurrentPlayer->last_z_trigger_timer = g_SavedBondState.last_z_trigger_timer;
    g_CurrentPlayer->hand_invisible[0] = g_SavedBondState.hand_invisible[0];
    g_CurrentPlayer->hand_invisible[1] = g_SavedBondState.hand_invisible[1];
    g_CurrentPlayer->resetshadecol = g_SavedBondState.resetshadecol;
    g_CurrentPlayer->aimtype = g_SavedBondState.aimtype;
    g_CurrentPlayer->field_FCC = g_SavedBondState.field_FCC;
    g_CurrentPlayer->field_FD0 = g_SavedBondState.field_FD0;
    g_CurrentPlayer->field_FD8 = g_SavedBondState.field_FD8;
    g_CurrentPlayer->tileColor = g_SavedBondState.tileColor;

    // Auto-Aim and manual aiming State
    g_CurrentPlayer->insightaimmode = g_SavedBondState.insightaimmode;
    g_CurrentPlayer->autoyaimenabled = g_SavedBondState.autoyaimenabled;
    g_CurrentPlayer->autoaimy = g_SavedBondState.autoaimy;
    g_CurrentPlayer->autoaim_target_y = get_safe_prop_by_index(g_SavedBondState.autoaim_target_y_index);
    g_CurrentPlayer->autoyaimtime60 = g_SavedBondState.autoyaimtime60;
    g_CurrentPlayer->autoxaimenabled = g_SavedBondState.autoxaimenabled;
    g_CurrentPlayer->autoaimx = g_SavedBondState.autoaimx;
    g_CurrentPlayer->autoaim_target_x = get_safe_prop_by_index(g_SavedBondState.autoaim_target_x_index);
    g_CurrentPlayer->autoxaimtime60 = g_SavedBondState.autoxaimtime60;
    g_CurrentPlayer->crosshair_angle = g_SavedBondState.crosshair_angle;
    g_CurrentPlayer->crosshair_x_pos = g_SavedBondState.crosshair_x_pos;
    g_CurrentPlayer->crosshair_y_pos = g_SavedBondState.crosshair_y_pos;
    g_CurrentPlayer->guncrossdamp = g_SavedBondState.guncrossdamp;
    g_CurrentPlayer->field_FFC = g_SavedBondState.field_FFC;
    g_CurrentPlayer->gun_azimuth_angle = g_SavedBondState.gun_azimuth_angle;
    g_CurrentPlayer->gun_azimuth_turning = g_SavedBondState.gun_azimuth_turning;
    g_CurrentPlayer->gunaimdamp = g_SavedBondState.gunaimdamp;
    g_CurrentPlayer->field_1010 = g_SavedBondState.field_1010;
    g_CurrentPlayer->field_101C = g_SavedBondState.field_101C;
    g_CurrentPlayer->gunammooff = g_SavedBondState.gunammooff;
    g_CurrentPlayer->field_1068 = g_SavedBondState.field_1068;
    g_CurrentPlayer->gunsightmode = g_SavedBondState.gunsightmode;
    g_CurrentPlayer->field_112C = g_SavedBondState.field_112C;

    // HUD Messages & Viewport Projection States
    g_CurrentPlayer->hudmessoff = g_SavedBondState.hudmessoff;
    g_CurrentPlayer->bondmesscnt = g_SavedBondState.bondmesscnt;
    g_CurrentPlayer->c_screenwidth = g_SavedBondState.c_screenwidth;
    g_CurrentPlayer->c_screenheight = g_SavedBondState.c_screenheight;
    g_CurrentPlayer->c_screenleft = g_SavedBondState.c_screenleft;
    g_CurrentPlayer->c_screentop = g_SavedBondState.c_screentop;
    g_CurrentPlayer->c_perspnear = g_SavedBondState.c_perspnear;
    g_CurrentPlayer->c_perspfovy = g_SavedBondState.c_perspfovy;
    g_CurrentPlayer->c_perspaspect = g_SavedBondState.c_perspaspect;
    g_CurrentPlayer->c_halfwidth = g_CurrentPlayer->c_halfwidth;
    g_CurrentPlayer->c_halfheight = g_CurrentPlayer->c_halfheight;

    // Screen Projection Bounds
    g_CurrentPlayer->screenxminf = g_SavedBondState.screenxminf;
    g_CurrentPlayer->screenyminf = g_SavedBondState.screenyminf;
    g_CurrentPlayer->screenxmaxf = g_SavedBondState.screenxmaxf;
    g_CurrentPlayer->screenymaxf = g_SavedBondState.screenymaxf;

    // Camera Scale & Reciprocal Scale
    g_CurrentPlayer->c_scalex = g_SavedBondState.c_scalex;
    g_CurrentPlayer->c_scaley = g_SavedBondState.c_scaley;
    g_CurrentPlayer->c_recipscalex = g_SavedBondState.c_recipscalex;
    g_CurrentPlayer->c_recipscaley = g_SavedBondState.c_recipscaley;

    // Camera LOD Scale
    g_CurrentPlayer->c_scalelod60 = g_SavedBondState.c_scalelod60;
    g_CurrentPlayer->c_scalelod = g_SavedBondState.c_scalelod;
    g_CurrentPlayer->c_lodscalez = g_SavedBondState.c_lodscalez;
    g_CurrentPlayer->c_lodscalezu32 = g_SavedBondState.c_lodscalezu32;

    // Camera Normalization Vectors
    g_CurrentPlayer->c_cameratopnorm = g_SavedBondState.c_cameratopnorm;
    g_CurrentPlayer->c_cameraleftnorm = g_SavedBondState.c_cameraleftnorm;

    // Viewports
    g_CurrentPlayer->viewports[0] = g_SavedBondState.viewports[0];
    g_CurrentPlayer->viewports[1] = g_SavedBondState.viewports[1];

    // Viewport Bounds
    g_CurrentPlayer->viewx = g_SavedBondState.viewx;
    g_CurrentPlayer->viewy = g_SavedBondState.viewy;
    g_CurrentPlayer->viewleft = g_SavedBondState.viewleft;
    g_CurrentPlayer->viewtop = g_SavedBondState.viewtop;

    // Control Type & Watch Menu Configuration
    g_CurrentPlayer->cur_player_control_type_0 = g_SavedBondState.cur_player_control_type_0;
    g_CurrentPlayer->cur_player_control_type_1 = g_SavedBondState.cur_player_control_type_1;
    g_CurrentPlayer->cur_player_control_type_2 = g_SavedBondState.cur_player_control_type_2;
    g_CurrentPlayer->neg_vspacing_for_control_type_entry = g_SavedBondState.neg_vspacing_for_control_type_entry;
    g_CurrentPlayer->has_set_control_type_data = g_SavedBondState.has_set_control_type_data;
    g_CurrentPlayer->cur_item_weapon_getname = g_SavedBondState.cur_item_weapon_getname;

    // Health/Armor Scaling Multipliers
    g_CurrentPlayer->actual_health = g_SavedBondState.actual_health;
    g_CurrentPlayer->actual_armor = g_SavedBondState.actual_armor;

    // GoldenEye Key objective status
    g_CurrentPlayer->copiedgoldeneye = g_SavedBondState.copiedgoldeneye;

    // Camera/Sniper Zoom
    g_CurrentPlayer->sniper_zoom = g_SavedBondState.sniper_zoom;
    g_CurrentPlayer->camera_zoom = g_SavedBondState.camera_zoom;
    g_CurrentPlayer->zoomintime = g_SavedBondState.zoomintime;
    g_CurrentPlayer->zoomintimemax = g_SavedBondState.zoomintimemax;
    g_CurrentPlayer->zoominfovy = g_SavedBondState.zoominfovy;
    g_CurrentPlayer->zoominfovyold = g_SavedBondState.zoominfovyold;
    g_CurrentPlayer->zoominfovynew = g_SavedBondState.zoominfovynew;
    g_CurrentPlayer->fovy = g_SavedBondState.fovy;
    g_CurrentPlayer->aspect = g_SavedBondState.aspect;

    // Color Tint & Screen Fades
    g_CurrentPlayer->colourscreenred = g_SavedBondState.colourscreenred;
    g_CurrentPlayer->colourscreengreen = g_SavedBondState.colourscreengreen;
    g_CurrentPlayer->colourscreenblue = g_SavedBondState.colourscreenblue;
    g_CurrentPlayer->colourscreenfrac = g_SavedBondState.colourscreenfrac;
    g_CurrentPlayer->colourfadetime60 = g_SavedBondState.colourfadetime60;
    g_CurrentPlayer->colourfadetimemax60 = g_SavedBondState.colourfadetimemax60;
    g_CurrentPlayer->colourfaderedold = g_SavedBondState.colourfaderedold;
    g_CurrentPlayer->colourfaderednew = g_SavedBondState.colourfaderednew;
    g_CurrentPlayer->colourfadegreenold = g_SavedBondState.colourfadegreenold;
    g_CurrentPlayer->colourfadegreennew = g_SavedBondState.colourfadegreennew;
    g_CurrentPlayer->colourfadeblueold = g_SavedBondState.colourfadeblueold;
    g_CurrentPlayer->colourfadebluenew = g_SavedBondState.colourfadebluenew;
    g_CurrentPlayer->colourfadefracold = g_SavedBondState.colourfadefracold;
    g_CurrentPlayer->colourfadefracnew = g_SavedBondState.colourfadefracnew;

    // Cheat Button Buffer & Status
    {
        s32 i;
        for (i = 0; i < 20; i++) {
            g_CurrentPlayer->cheat_display_text_related[i] = g_SavedBondState.cheat_display_text_related[i];
        }
    }
    g_CurrentPlayer->something_with_cheat_text = g_SavedBondState.something_with_cheat_text;
    g_CurrentPlayer->can_display_cheat_text = g_SavedBondState.can_display_cheat_text;
    g_CurrentPlayer->field_12B7 = g_SavedBondState.field_12B7;

    // Favorite Weapon Stats
    {
        s32 i;
        for (i = 0; i < 10; i++) {
            g_CurrentPlayer->gunheldarr[i] = g_SavedBondState.gunheldarr[i];
        }
    }

    // HUD and Watch graphics overlay
    g_CurrentPlayer->equipmaxitems = g_SavedBondState.equipmaxitems;
    g_CurrentPlayer->bloodImgIdx = g_SavedBondState.bloodImgIdx;
    {
        s32 i;
        for (i = 0; i < 23; i++) {
            g_CurrentPlayer->armor_display_values[i] = g_SavedBondState.armor_display_values[i];
            g_CurrentPlayer->health_display_values[i] = g_SavedBondState.health_display_values[i];
        }
    }
    g_CurrentPlayer->watch_body_armor_bar_gdl = g_SavedBondState.watch_body_armor_bar_gdl;
    g_CurrentPlayer->watch_health_bar_gdl = g_SavedBondState.watch_health_bar_gdl;
    {
        s32 i;
        for (i = 0; i < WATCH_NUMBER_SCREENS; i++) {
            g_CurrentPlayer->buffer_for_watch_greenbackdrop_vertices[i] = g_SavedBondState.buffer_for_watch_greenbackdrop_vertices[i];
        }
        g_CurrentPlayer->buffer_for_watch_static_vertices[0] = g_SavedBondState.buffer_for_watch_static_vertices[0];
    }

    // Restore transition and environmental tracking fields
    g_CurrentPlayer->field_2A04 = g_SavedBondState.field_2A04;
    g_CurrentPlayer->field_2A08 = g_SavedBondState.field_2A08;
    g_CurrentPlayer->field_2A0C = g_SavedBondState.field_2A0C;
    g_CurrentPlayer->autocrouchpos = g_SavedBondState.autocrouchpos;
    g_CurrentPlayer->healthdisplaytime = g_SavedBondState.healthdisplaytime;
    g_CurrentPlayer->damagetype = g_SavedBondState.damagetype;
    g_CurrentPlayer->field_8C = g_SavedBondState.field_8C;

    // Restore multiplayer, stats, and viewport height/dampening state tracking
    g_CurrentPlayer->healthDamageType = g_SavedBondState.healthDamageType;
    g_CurrentPlayer->field_29BC = g_SavedBondState.field_29BC;
    g_CurrentPlayer->field_29C0 = g_SavedBondState.field_29C0;
    g_CurrentPlayer->mpmenuon = g_SavedBondState.mpmenuon;
    g_CurrentPlayer->mpmenumode = g_SavedBondState.mpmenumode;
    g_CurrentPlayer->mpquitconfirm = g_SavedBondState.mpquitconfirm;
    g_CurrentPlayer->mpjoywascentre = g_SavedBondState.mpjoywascentre;
    g_CurrentPlayer->deathcount = g_SavedBondState.deathcount;
    g_CurrentPlayer->num_suicides = g_SavedBondState.num_suicides;
    g_CurrentPlayer->field_29E0 = g_SavedBondState.field_29E0;
    {
        s32 i;
        for (i = 0; i < 4; i++) {
            g_CurrentPlayer->last_kill_time[i] = g_SavedBondState.last_kill_time[i];
        }
    }
    g_CurrentPlayer->field_29F4 = g_SavedBondState.field_29F4;
    g_CurrentPlayer->field_29F8 = g_SavedBondState.field_29F8;

    // Restore tank physics and turret state tracking fields
    in_tank_flag = g_SavedBondState.in_tank_flag;
    g_PlayerTankProp = get_safe_prop_by_index(g_SavedBondState.g_PlayerTankProp_index);
    g_WorldTankProp = get_safe_prop_by_index(g_SavedBondState.g_WorldTankProp_index);
    g_PlayerTankYOffset = g_SavedBondState.g_PlayerTankYOffset;
    g_TankTurnSpeed = g_SavedBondState.g_TankTurnSpeed;
    g_TankOrientationAngle = g_SavedBondState.g_TankOrientationAngle;
    tank_turret_unused_angle = g_SavedBondState.tank_turret_unused_angle;
    g_TankTurretVerticalAngle = g_SavedBondState.g_TankTurretVerticalAngle;
    g_TankTurretVerticalAngleRelated = g_SavedBondState.g_TankTurretVerticalAngleRelated;
    g_TankTurretOrientationAngleRad = g_SavedBondState.g_TankTurretOrientationAngleRad;
    g_TankTurretOrientationAngleDeg = g_SavedBondState.g_TankTurretOrientationAngleDeg;
    tank_turret_turn_speed = g_SavedBondState.tank_turret_turn_speed;
    g_BondCanEnterTank = g_SavedBondState.g_BondCanEnterTank;
    g_TankTurretAngle = g_SavedBondState.g_TankTurretAngle;
    g_TankTurretTurn = g_SavedBondState.g_TankTurretTurn;
    g_ExplodeTankOnDeathFlag = g_SavedBondState.g_ExplodeTankOnDeathFlag;
    g_TankDamagePenaltyTicks = g_SavedBondState.g_TankDamagePenaltyTicks;
    g_EnterTankAudioState = g_SavedBondState.g_EnterTankAudioState;

    // Restore camera mode and cutscene transition states
    g_CameraMode = g_SavedBondState.g_CameraMode;
    camera_mode = g_SavedBondState.camera_mode;
    g_CameraAfterCinema = g_SavedBondState.g_CameraAfterCinema;
    camera_transition_timer = g_SavedBondState.camera_transition_timer;
    camera_fade_active = g_SavedBondState.camera_fade_active;
    intro_camera_index = g_SavedBondState.intro_camera_index;
    is_timer_active = g_SavedBondState.is_timer_active;
    g_PlayerInvincible = g_SavedBondState.g_PlayerInvincible;

    // Restore head and hand animation/rendering progress flags
    g_CurrentPlayer->field_5C0 = g_SavedBondState.field_5C0;
    g_CurrentPlayer->field_FC8 = g_SavedBondState.field_FC8;

    // Restore visual layout matrices / switches padding (excluding field_654 which is a runtime data pointer)
    g_CurrentPlayer->field_59C = g_SavedBondState.field_59C;
    g_CurrentPlayer->field_5A0 = g_SavedBondState.field_5A0;
    g_CurrentPlayer->field_5A4 = g_SavedBondState.field_5A4;
    g_CurrentPlayer->field_5A8 = g_SavedBondState.field_5A8;
    g_CurrentPlayer->field_5AC = g_SavedBondState.field_5AC;
    g_CurrentPlayer->field_5B0 = g_SavedBondState.field_5B0;
    g_CurrentPlayer->field_5B4 = g_SavedBondState.field_5B4;
    g_CurrentPlayer->field_5B8 = g_SavedBondState.field_5B8;
    g_CurrentPlayer->field_5C4 = g_SavedBondState.field_5C4;
    g_CurrentPlayer->field_5C8 = g_SavedBondState.field_5C8;
    g_CurrentPlayer->field_5CC = g_SavedBondState.field_5CC;
    g_CurrentPlayer->field_5D0 = g_SavedBondState.field_5D0;
    g_CurrentPlayer->field_5D4 = g_SavedBondState.field_5D4;
    g_CurrentPlayer->field_5D8 = g_SavedBondState.field_5D8;
    g_CurrentPlayer->field_5DC = g_SavedBondState.field_5DC;
    g_CurrentPlayer->field_5E0 = g_SavedBondState.field_5E0;
    g_CurrentPlayer->field_5E4 = g_SavedBondState.field_5E4;
    g_CurrentPlayer->field_5E8 = g_SavedBondState.field_5E8;
    g_CurrentPlayer->field_5EC = g_SavedBondState.field_5EC;
    g_CurrentPlayer->field_5F0 = g_SavedBondState.field_5F0;
    g_CurrentPlayer->field_5F4 = g_SavedBondState.field_5F4;
    g_CurrentPlayer->field_5F8 = g_SavedBondState.field_5F8;
    g_CurrentPlayer->field_5FC = g_SavedBondState.field_5FC;
    g_CurrentPlayer->field_600 = g_SavedBondState.field_600;
    g_CurrentPlayer->field_604 = g_SavedBondState.field_604;
    g_CurrentPlayer->field_608 = g_SavedBondState.field_608;
    g_CurrentPlayer->field_60C = g_SavedBondState.field_60C;
    g_CurrentPlayer->field_610 = g_SavedBondState.field_610;
    g_CurrentPlayer->field_614 = g_SavedBondState.field_614;
    g_CurrentPlayer->field_618 = g_SavedBondState.field_618;
    g_CurrentPlayer->field_61C = g_SavedBondState.field_61C;
    g_CurrentPlayer->field_620 = g_SavedBondState.field_620;
    g_CurrentPlayer->field_624 = g_SavedBondState.field_624;
    g_CurrentPlayer->field_628 = g_SavedBondState.field_628;
    g_CurrentPlayer->field_62C = g_SavedBondState.field_62C;
    g_CurrentPlayer->field_630 = g_SavedBondState.field_630;
    g_CurrentPlayer->field_634 = g_SavedBondState.field_634;
    g_CurrentPlayer->field_638 = g_SavedBondState.field_638;
    g_CurrentPlayer->field_63C = g_SavedBondState.field_63C;
    g_CurrentPlayer->field_640 = g_SavedBondState.field_640;
    g_CurrentPlayer->field_644 = g_SavedBondState.field_644;
    g_CurrentPlayer->field_648 = g_SavedBondState.field_648;
    g_CurrentPlayer->field_64C = g_SavedBondState.field_64C;
    g_CurrentPlayer->field_650 = g_SavedBondState.field_650;
    g_CurrentPlayer->field_658 = g_SavedBondState.field_658;
    g_CurrentPlayer->field_65C = g_SavedBondState.field_65C;
    g_CurrentPlayer->field_660 = g_SavedBondState.field_660;
    g_CurrentPlayer->field_664 = g_SavedBondState.field_664;
    g_CurrentPlayer->field_668 = g_SavedBondState.field_668;
    g_CurrentPlayer->field_66C = g_SavedBondState.field_66C;
    g_CurrentPlayer->field_670 = g_SavedBondState.field_670;
    g_CurrentPlayer->field_674 = g_SavedBondState.field_674;
    g_CurrentPlayer->field_678 = g_SavedBondState.field_678;
    g_CurrentPlayer->field_67C = g_SavedBondState.field_67C;
    g_CurrentPlayer->field_680 = g_SavedBondState.field_680;
    g_CurrentPlayer->field_684 = g_SavedBondState.field_684;
    g_CurrentPlayer->field_688 = g_SavedBondState.field_688;
    g_CurrentPlayer->field_68C = g_SavedBondState.field_68C;
    g_CurrentPlayer->field_690 = g_SavedBondState.field_690;
    g_CurrentPlayer->field_694 = g_SavedBondState.field_694;
    g_CurrentPlayer->field_698 = g_SavedBondState.field_698;
    g_CurrentPlayer->field_69C = g_SavedBondState.field_69C;
    g_CurrentPlayer->field_6A0 = g_SavedBondState.field_6A0;
    g_CurrentPlayer->field_6A4 = g_SavedBondState.field_6A4;
    g_CurrentPlayer->field_6A8 = g_SavedBondState.field_6A8;
    g_CurrentPlayer->field_6AC = g_SavedBondState.field_6AC;
    g_CurrentPlayer->field_6B0 = g_SavedBondState.field_6B0;
    g_CurrentPlayer->field_6B4 = g_SavedBondState.field_6B4;
    g_CurrentPlayer->field_6B8 = g_SavedBondState.field_6B8;
    g_CurrentPlayer->field_6BC = g_SavedBondState.field_6BC;
    g_CurrentPlayer->field_6C0 = g_SavedBondState.field_6C0;
    g_CurrentPlayer->field_6C4 = g_SavedBondState.field_6C4;
    g_CurrentPlayer->field_6C8 = g_SavedBondState.field_6C8;
    g_CurrentPlayer->field_6CC = g_SavedBondState.field_6CC;

    // Re-generate watch menu GDLs and backdrop segment pointers to update absolute physical addresses
    {
        extern Gfx *sub_GAME_7F0A3330(Gfx *arg0, void *arg1, s32 arg2);
        extern void sub_GAME_7F0A69A8(void);
        extern struct WatchVertex *setup_watch_rectangles(struct WatchVertex *vtx, s32 startx, s32 startz, s32 width, s32 height, s32 horizontal_offset, s32 vertical_offset);
        extern Gfx *sub_GAME_7F0A3B40(Gfx *gdl, s32 *arg1);

        sub_GAME_7F0A3330(&g_CurrentPlayer->watch_body_armor_bar_gdl, OS_K0_TO_PHYSICAL(&g_CurrentPlayer->armor_display_values), 0x2E);
        sub_GAME_7F0A3330(&g_CurrentPlayer->watch_health_bar_gdl, OS_K0_TO_PHYSICAL(&g_CurrentPlayer->health_display_values), 0x2E);

        sub_GAME_7F0A69A8();

        {
            Gfx *ptr_b = g_CurrentPlayer->buffer_for_watch_greenbackdrop_DL;
            struct WatchVertex *ptr_a = &g_CurrentPlayer->buffer_for_watch_greenbackdrop_vertices->vtx[0];
            s32 i;
            for (i = 0; i < (WATCH_NUMBER_SCREENS * WATCH_SCREEN_SELECT_RECTANGLE_HSTEP); i += WATCH_SCREEN_SELECT_RECTANGLE_HSTEP) {
                struct WatchVertex *ptr_copy = ptr_a;
                ptr_a = setup_watch_rectangles(ptr_a, i, 0, 0x64, 0x14, -0x12B, 0x136);
                ptr_b = sub_GAME_7F0A3B40(ptr_b, OS_K0_TO_PHYSICAL(ptr_copy));
            }
            gSPEndDisplayList(ptr_b);

            {
                struct WatchVertex *ptr_copy = (struct WatchVertex *)g_CurrentPlayer->buffer_for_watch_static_vertices;
                ptr_b = g_CurrentPlayer->buffer_for_watch_static_DL;
                setup_watch_rectangles(&g_CurrentPlayer->buffer_for_watch_static_vertices->vtx[0], 0, 0, 0x398, 0x14, -0x1CC, 0);
                ptr_b = sub_GAME_7F0A3B40(ptr_b, OS_K0_TO_PHYSICAL(ptr_copy));
                gSPEndDisplayList(ptr_b);
            }
        }
    }
}
