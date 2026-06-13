```c
// Saved: yes
// Circular string buffer storing up to 5 queued HUD message strings. Each slot is up to 101 (0x65) bytes.
char stringbuffer_lowerleft[0x5][0x65];
// Saved: yes
// Current index in the circular buffer where the next HUD status bar message will be written. Bounded between 0 and 4.
s32 status_bar_text_buffer_index;
// Saved: yes
// Timer/flag for displaying the status bar HUD. If greater than 0, the status bar is rendered and this value decrements each frame.
s32 display_statusbar;
// Saved: yes
// Font table mapping/character indices for the first line of status bar text (JP/EU version).
s32 dword_CODE_bss_jp80079CEC[5];
// Saved: yes
// Font table mapping/character indices for the second line of status bar text (JP/EU version).
s32 dword_CODE_bss_jp80079Cd8[5];
// Saved: yes
// Font table mapping/character index for the first line of status bar text (US version).
s32 copy_1stfonttable;
// Saved: yes
// Font table mapping/character index for the second line of status bar text (US version).
s32 copy_2ndfonttable;

// Saved: yes
// Tank Physics and Turret State Tracking Globals:
// - in_tank_flag: Boolean/state flag indicating if player is inside the tank (1 = in tank, 0 = regular gameplay).
// - g_PlayerTankProp: Pointer to the tank PropRecord currently ridden by player.
// - g_WorldTankProp: Pointer to the primary level stage tank PropRecord.
// - g_PlayerTankYOffset: Bounded vertical hover translation/clearance distance offset of the tank from the floor.
// - g_TankTurnSpeed: Angular turn rate/velocity of the tank object.
// - g_TankOrientationAngle: Absolute horizontal driving direction rotation angle of the tank (yaw).
// - tank_turret_unused_angle: Unreferenced auxiliary turret angle float.
// - g_TankTurretVerticalAngle: Vertical orientation angle (pitch) of the tank turret weapon.
// - g_TankTurretVerticalAngleRelated: Secondary vertical turret rotation parameter.
// - g_TankTurretOrientationAngleRad: Turret azimuth/yaw angle relative to the tank base chassis (in radians).
// - g_TankTurretOrientationAngleDeg: Turret azimuth/yaw angle relative to the tank base chassis (in degrees).
// - tank_turret_turn_speed: Azimuth/yaw turn speed of the turret.
// - g_BondCanEnterTank: Boolean permit flag allowing Bond to board/disembark the tank.
// - g_TankTurretAngle: Absolute turret rotation yaw angle.
// - g_TankTurretTurn: Turret yaw rotation rate.
// - g_ExplodeTankOnDeathFlag: Config state flag deciding whether the tank detonates when player dies inside it.
// - g_TankDamagePenaltyTicks: Damage penalty/overheat cooldown tick timer.
// - g_EnterTankAudioState: State engine of the tank motor run sounds (0 = NOT_RUNNING, 1 = STARTING, 2 = RUNNING).
s32 in_tank_flag;
struct PropRecord *g_PlayerTankProp;
struct PropRecord *g_WorldTankProp;
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

// Saved: yes
// Camera Mode and Cutscene Transition Globals:
// - g_CameraMode: Current active rendering camera viewport mode.
// - camera_mode: Target camera transition state.
// - g_CameraAfterCinema: Restoration camera mode to trigger when cinema sweeps finish.
// - camera_transition_timer: Camera interpolation transition frame/clock timer.
// - camera_fade_active: Active camera overlay fade transition state flag.
// - intro_camera_index: Active sweep intro camera ID index.
// - is_timer_active: Level run mission timer gate flag.
// - g_PlayerInvincible: Global cheats vulnerability setting.
enum CAMERAMODE g_CameraMode;
enum CAMERAMODE camera_mode;
enum CAMERAMODE g_CameraAfterCinema;
f32 camera_transition_timer;
s32 camera_fade_active;
s32 intro_camera_index;
s32 is_timer_active;
s32 g_PlayerInvincible;

// State of the player (Bond).
struct player {
  // Saved: yes
  // Flag indicating player control state / viewport focus.
  // Values:
  // - 0: Normal gameplay mode (processes portal geometry, wall/floor collisions, respects physical speed).
  // - 1: Cutscene/Bypass mode (bypasses collision and portal calculations, camera snaps/moves directly to the coordinates in pos. Used during intro sweeps, death cameras, or scripted control sequences).
  s32 unknown;

  // Saved: yes
  // 3D coordinate of the player's camera position (eye level/head) in the game world.
  coord3d pos;

  // Saved: yes
  // Camera look-at target position (point in 3D space the camera is directed towards), used primarily for cutscenes, transition cameras, or frozen/free camera ticks.
  coord3d pos2;

  // Saved: yes
  // 3D coordinate of the player's feet / floor level, adjusted based on level stand tile heights.
  coord3d pos3;

  // Saved: yes
  // Visual camera offset/translation vector.
  coord3d offset;

  // Saved: yes (as room_pointer_offset relative to standTileStart)
  // Pointer to the level geometry StandTile the player is currently on.
  // Address Safety: Fully safe across level reloads and console resets because it is serialized as a byte offset relative to standTileStart and reconstructed on load.
  StandTile *room_pointer;

  // Saved: yes (restored via deregistration/registration functions)
  // Pointer to the player's world PropRecord which tracks its position, active stand tile, and registered rooms in the game's spatial grid.
  // Address Safety: The pointer itself is safe to load within the same level run. However, moving the player prop requires deregistering it from its old rooms (using `chrpropDeregisterRooms`) and registering it to its new rooms (using `chrpropRegisterRooms`) to prevent coordinate-room desync and crashes.
  PropRecord *prop;

  // Recomputed: yes (synced via setsuboffset and setsubroty)
  // Pointer to the player's 3D visual model instance in stage memory.
  struct Model *ptr_char_objectinstance;

  // Saved: yes
  // Origin coordinates of the current level room the player is in, used to transform rendering matrices to prevent N64 floating point precision issues.
  // Scaling:
  // - current_model_pos: scaled origin coordinates of the current room (multiplied by `room_data_float2` which is the global coordinate scaling factor of the level geometry).
  // - previous_model_pos: scaled origin coordinates of the previous frame's room.
  // - current_room_pos: unscaled origin coordinates of the current room (obtained by multiplying `current_model_pos` by `room_data_float1` which is `1.0f / room_data_float2`).
  coord3d current_model_pos;
  coord3d previous_model_pos;
  coord3d current_room_pos;

  // Saved: yes
  // Index into the active rendering matrices allocation buffer array (roomMatrices / roomOwners of size 300) to keep track of the room origin mapping for viewport rendering.
  s32 curRoomIndex;

  // Saved: yes
  // Portal boundary and collision data structure representing player's logical bounds, collision positions, and portal travel logic.
  struct collision434 field_488;

  // Saved: yes
  // Look angles (horizontal and vertical orientation in degrees).
  // - vv_theta: Horizontal look rotation. Bounded between 0.0f and 360.0f (degrees). Used to calculate radial direction.
  // - vv_verta: Vertical look rotation (pitch). Bounded between -80.0f (looking straight down) and 80.0f (looking straight up).
  f32 vv_theta;
  f32 vv_verta;

  // Recomputed: yes (calculated on load by bondviewApplyVertaTheta)
  // Derived trigonometric values of the look angles:
  // - vv_costheta, vv_sintheta: cached sine and cosine of vv_theta.
  // - vv_verta360: vertical look angle normalized to [0, 360].
  // - vv_cosverta, vv_sinverta: cached sine and cosine of vv_verta360.
  f32 vv_costheta;
  f32 vv_sintheta;
  f32 vv_verta360;
  f32 vv_cosverta;
  f32 vv_sinverta;

  // Saved: yes
  // Movement speeds and physics multipliers:
  // - speedsideways: Current velocity moving sideways (left/right) relative to camera heading. Bounded typically by joystick limits.
  // - speedstrafe: Horizontal joystick deflection component. Range: -70 to +70 (or equivalent scaled joystick inputs).
  // - speedforwards: Current velocity moving forwards/backwards relative to camera heading. Bounded by joystick and current speed limits.
  // - speedmaxtime60: Continuous forward-running frame/tick counter. Increments by g_ClockTimer (60Hz US, 50Hz JP/EU) when running forward at full speed. Bounded between 0 and THREE_SECOND_TICKS (180 US, 150 JP/EU). Resets to 0 if forward movement ceases.
  // - speedboost: Running speed acceleration factor. Bounded between 1.0f (SPEED_REGULAR_MAX) and 1.25f (SPEED_RUN_MAX). When running forward at full speed continuously for three seconds (speedmaxtime60 >= THREE_SECOND_TICKS), it increments by 0.01f (SPEED_TICK_ADJUST) per frame up to 1.25f. Decays back to 1.0f when full forward speed is not maintained.
  f32 speedsideways;
  f32 speedstrafe;
  f32 speedforwards;
  s32 speedmaxtime60;
  f32 speedboost;

  // Saved: yes
  // Crouch/duck positioning and height offsets:
  // - vertical_bounce_adjust: Visual camera bounce vertical adjustment applied on landing impact or when descending stairs/ramps.
  // - ducking_height_offset: Vertical visual/logical camera height offset. Bounded between 0.0f (standing) and -100.0f (fully crouched / FULL_CROUCH_OFFSET). Interpolates smoothly towards the target height matching crouchpos.
  // - crouchpos: Target crouch state enum/flag.
  //   Values:
  //   - 0: CROUCH_SQUAT (fully crouched / target height offset -100.0f)
  //   - 1: CROUCH_HALF (kneeling / target height offset -60.0f)
  //   - 2: CROUCH_STAND (standing / target height offset 0.0f)
  // - field_A4: Crouch transition factor. Tracks progress moving between crouch height offsets.
  // - field_AC: Active collision/process flag for the player (0 = inactive/frozen, 1 = active).
  // - collision_bounds: The player's bounding box boundaries used for 2D floor/wall collision checks.
  f32 vertical_bounce_adjust;
  f32 ducking_height_offset;
  s32 crouchpos;
  f32 field_A4;
  s32 field_AC;
  struct rect4f collision_bounds;

  // Saved: yes
  // Health and armor values:
  // - bondhealth, bondarmour: Actual logical player health and armor. Bounded between 0.0f (no health/armor) and 1.0f (full health/armor).
  // - oldhealth, oldarmour: The health and armor values from the previous frame. Bounded between 0.0f and 1.0f.
  // - apparenthealth, apparentarmour: Visual health/armor level displayed on the HUD bars. Bounded between 0.0f and 1.0f. Interpolates smoothly towards bondhealth and bondarmour over time (sliding bar effect).
  f32 bondhealth;
  f32 bondarmour;
  f32 oldhealth;
  f32 oldarmour;
  f32 apparenthealth;
  f32 apparentarmour;

  // Saved: yes
  // Array of 23 damage display elements caching the visual layout, vertex positions, and colors of the armor and health slices for HUD rendering.
  struct damage_display_parent armor_display_values[23];
  struct damage_display_parent health_display_values[23];

  // Saved: yes
  // Previous frame's camera coordinates, used to prevent physics/collision jumps on the frame immediately following a load.
  coord3d bondprevpos;

  // Saved: yes
  // X, Y, Z velocity/momentum vector (units: coordinate units per frame):
  // - field_78: Sideways velocity (X axis). Positive values move right, negative left.
  // - field_7C: Vertical velocity (Y axis). Bounded by terminal velocity. Negative when falling, positive when jumping or pushed up.
  // - field_80: Forward/backward velocity (Z axis). Positive moves forward, negative backward.
  // - speedgo: Forward/backward acceleration factor. Bounded between 0.0f (stopped) and 1.0f (full speed).
  // - bondshotspeed: Recoil velocity vector applied to the player upon taking gunshots or explosive blast impact.
  // - speedtheta, speedverta: Rotational look speeds (yaw/pitch rates) per frame.
  f32 field_78;
  f32 field_7C;
  f32 field_80;
  f32 speedgo;
  coord3d bondshotspeed;
  f32 speedtheta;
  f32 speedverta;

  // Saved: yes
  // Dampened landing/impact vertical bounce offsets.
  f32 field_84;
  f32 field_88;

  // Saved: yes
  // Height tracking variables:
  // - field_6C: raw height above the ground before dampening/gravity checks.
  // - field_70: smoothed floor/ground level tracking height.
  // - stanHeight: height of the StandTile currently below the player.
  f32 field_6C;
  f32 field_70;
  f32 stanHeight;

  // Saved: yes
  // Coordinate smoothing exponential accumulator:
  // - field_3B8: smoothed coordinate accumulator vector.
  // - field_3C4, field_3C8, field_3CC: derived X, Y, Z camera coordinates calculated from field_3B8. Overwriting these avoids 1-frame coordinate rendering jumps or visual rubberbanding.
  coord3d field_3B8;
  f32 field_3C4;
  f32 field_3C8;
  f32 field_3CC;

  // Saved: yes
  // Previous frame's collision context structure.
  struct collision434 previous_collision_info;
  // Saved: yes
  // field_2A6C: Collision/clipping state flag (1 = active, 0 = inactive). Set to 1 under certain room/floor transition conditions to handle player height-clipping adjustments.
  s32 field_2A6C;
  // Saved: yes (as field_2A70_offset relative to standTileStart)
  // field_2A70: Pointer to StandTile used as a collision target fallback when player height-clipping or fall transitions occur.
  // Address Safety: Fully safe across level reloads and console resets because it is serialized as a byte offset relative to standTileStart and reconstructed on load.
  struct StandTile *field_2A70;

  // Saved: yes
  // Walk & Head Bob Animation State:
  // - resetheadpos, resetheadrot, resetheadtick: head position and rotation reset flags.
  // - headanim: active head bob/walk animation index.
  // - headdamp: look/head dampening multiplier.
  // - headwalkingtime60: current time/frame index of the walking animation cycle. Overwriting this resets the walk cycle.
  // - headamplitude, sideamplitude: vertical and side amplitudes of the head bob cycle.
  // - headpos, headlook, headup: current frame walk bob offsets/look vectors.
  // - headpossum, headlooksum, headupsum: accumulated walk bob offsets/look vectors that drive camera position changes.
  // - headbodyoffset, standbodyoffset: visual body-relative offsets.
  // - standheight, standfrac: stationary ground offsets and interpolation fraction.
  // - standlook, standup: array of 2 look/up vectors representing standing state history.
  // - standcnt: stand state accumulator.
  // - animFlipFlag: Flip/toggle state of the head/camera bobbing animation cycle. Gets inverted when the bobbing cycle crosses certain limits to change the tilt/direction of camera animations.
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
  s8 animFlipFlag;
  // Saved: yes
  // field_5C0: Active frame counter/timing position float of the head bob/walk animation.
  f32 field_5C0;
  // Saved: yes
  // 4x4 floating point matrices tracking camera translation, pitch, roll, and yaw rotation offsets during the head/camera bobbing animation cycle.
  Mtxf bondheadmatrices[4];

  // Saved: yes
  // Inventory & Ammo Arrays:
  // - ammoheldarr: current ammunition counts for each ammo type.
  // - ptr_inventory_first_in_cycle: pointer to the first item in the inventory circular linked list. Saved address-safely as flat item definitions and reconstructed on load using inventory APIs.
  // - equipallguns: All Guns cheat flag.
  // - equipcuritem: current equipped weapon/item ID.
  s32 ammoheldarr[30];
  InvItem *ptr_inventory_first_in_cycle;
  s32 equipallguns;
  s32 equipcuritem;

  // Saved: yes
  // Array tracking cumulative holding time for each weapon slot, used to compute the "Favorite Weapon" statistic at the mission end screen.
  // - weapon1, weapon2: weapon IDs representing a single or dual-wield weapon configuration.
  // - totaltime: cumulative ticks/frames this weapon configuration has been active/held during the mission (initialized to -1 for unused slots).
  gunheld gunheldarr[10];

  // Saved: yes
  // Maximum number of items the inventory list structure is allocated to hold (typically 30/0x1E, allocated dynamically at stage startup).
  s32 equipmaxitems;

  // Saved: yes
  // Ping-pong index (0 or 1) indicating the currently active blood drop overlay texture buffer to render when damage is taken.
  s32 bloodImgIdx;

  // Saved: yes
  // Weapons & Hands Rendering:
  // - weapon/item IDs currently loaded in the left and right hands.
  ITEM_IDS hand_item[2];
  // Saved: yes
  // structure for each hand tracking animation status, next weapons, and weapon_ammo_in_magazine. Rebuilt address-safely; triggers model reloading via currentPlayerEquipWeaponWrapper if hand weapons change on load.
  struct hand hands[2];
  // Saved: yes
  // field_FC8: Firing/reloading hand animation progress flag (typically set to 1 during active firing/reloading frames).
  s32 field_FC8;

  // Saved: no (recomputed on load)
  // Dynamically allocated model buffer workspace for each hand viewport. Each buffer is 0x14820 (84,032) bytes, allocated via mempAllocBytesInBank() from MEMPOOL_STAGE during player initialization (init_player_BONDdata_stats). The buffer holds decompressed first-person weapon model data (vertices, textures, rendering commands). NOT safe to save directly — pointer values become stale after level reloads because the memory pool is re-allocated. The buffer contents are regenerated on load via used_to_load_1st_person_model_on_demand() which writes weapon model data into the freshly allocated buffer. Only populated when a weapon model is actually loaded (suits, triggers, watchlaser).
  ModelFileHeader *ptr_hand_weapon_buffer[2];

  // Saved: no (recomputed on load)
  // Local copy of the weapon model file header for each hand, populated when the weapon model is loaded into the hand buffer. Contains pointers to the model's node hierarchy (RootNode), skeleton (Skeleton), switches (Switches), and textures (Textures). These internal pointers are offsets into the corresponding ptr_hand_weapon_buffer, so they become stale when the buffer is re-allocated on load. The struct itself is saved as raw bytes but only meaningful when paired with buffer contents. Regenerated on load via used_to_load_1st_person_model_on_demand() which calls load_object_fill_header() to repopulate the header from the weapon's model file data.
  ModelFileHeader copy_of_body_obj_header[2];

  // Saved: no (recomputed on load)
  // Texture pool state tracker for each hand, initialized when the weapon model is loaded. Each struct is 16 bytes containing:
  // - start: pointer to the start of the texture pool memory region within the hand buffer
  // - end: pointer to the end boundary of the pool
  // - leftpos: current allocation position (byte offset into the pool)
  // - rightpos: current allocation position (text entry pointer into the pool)
  // NOT safe to save directly — these are pointers/offsets into the dynamically allocated ptr_hand_weapon_buffer that become stale on reload. The texture pool entries (struct tex linked list) are stored within the buffer itself, so saving the buffer contents preserves them. Re-initialized on load via texInitPool() which resets start/leftpos/rightpos to the new buffer address, then load_object_fill_header() populates the pool with texture entries from the weapon model file data.
  struct texpool item_related[2];

  // Saved: yes
  // Player status & cheat settings:
  // Player invincibility status flag. Bounded to 0 (vulnerable) or 1 (invincible, active cheat). When 1, all incoming gunshots and explosive damage to player health are bypassed.
  u8 bondinvincible;

  // Saved: yes
  // Buffer storing the last 20 controller button presses, used to detect button cheat code inputs during gameplay.
  u16 cheat_display_text_related[20];
  // Saved: yes
  // Flag indicating if a cheat code activation message is currently being displayed on the screen.
  u8 something_with_cheat_text;
  // Saved: yes
  // Counter/flag tracking cheat code entry progress and validation status. Bounded between 0 and 20.
  u8 can_display_cheat_text;
  // Saved: yes
  // Unused padding byte near cheat buttons/flags in struct layout.
  u8 field_12B7;

  // Saved: yes
  // HUD Display overlay states:
  // - damageshowtime: Remaining display duration (in frames/ticks) for the fullscreen damage flash or HUD warnings. Bounded from -1 (hidden/inactive) up to total duration (e.g. 120 frames).
  // - healthshowtime: Remaining display duration (in frames/ticks) for the health/armor HUD overlay bars. Bounded from -1 (hidden/inactive) up to total display duration. Counts up or down and resets to -1 when animation finishes.
#if defined(VERSION_JP) || defined(VERSION_EU)
  f32 damageshowtime;
  f32 healthshowtime;
#else
  s32 damageshowtime;
  s32 healthshowtime;
#endif
  // Saved: yes
  // Unused padding/unreferenced field (0x00fc).
  s32 healthshowmode;

  // Saved: yes
  // Default screen fade transition timers/fractions (used when transitioning between rooms, starting missions, etc.):
  // Current frame timer for the default screen fade. Bounded from 0.0f to bondfadetimemax60. Set to -1.0f when no default screen fade is active.
  f32 bondfadetime60;
  // Saved: yes
  // Total duration (in frames) of the default screen fade transition.
  f32 bondfadetimemax60;
  // Saved: yes
  // Starting default screen opacity fraction. Bounded between 0.0f (transparent) and 1.0f (fully opaque).
  f32 bondfadefracold;
  // Saved: yes
  // Target/ending default screen opacity fraction. Bounded between 0.0f and 1.0f.
  f32 bondfadefracnew;
  // Saved: yes
  // Player breathing cycle intensity. Bounded between 0.0f and 1.0f. Drives the frequency/amplitude of camera height bobbing when standing idle.
  f32 bondbreathing;

  // Saved: yes
  // Weapon Sway cycle details:
  // vertical and horizontal weapon sway amplitudes.
  f32 gunposamplitude;
  f32 gunxamplitude;
  // Saved: yes
  // weapon sway timing and synchronization state variables.
  f32 gunsync;
  f32 syncchange;
  f32 synccount;
  s32 syncoffset;
  // Saved: yes
  // smoothed breathing and sway accumulators.
  f32 field_107C;
  f32 field_1080;

  // Saved: yes
  // RGB channels of screen color tint (0-255). Damage flashes are red (255, 0, 0) and fade transitions are black (0, 0, 0).
  s32 colourscreenred;
  s32 colourscreengreen;
  s32 colourscreenblue;
  // Saved: yes
  // Active screen color overlay transparency fraction. Bounded between 0.0f (no overlay) and 1.0f (opaque).
  f32 colourscreenfrac;
  // Saved: yes
  // Color tint fade timers (in frames). Bounded from 0.0f to colourfadetimemax60.
  f32 colourfadetime60;
  f32 colourfadetimemax60;
  // Saved: yes
  // Starting and target RGB color/fraction boundaries for fade interpolation.
  s32 colourfaderedold;
  s32 colourfaderednew;
  s32 colourfadegreenold;
  s32 colourfadegreennew;
  s32 colourfadeblueold;
  s32 colourfadebluenew;
  f32 colourfadefracold;
  f32 colourfadefracnew;

  // Saved: yes
  // crosshair_angle: Final calculated 2D viewport coordinates of the crosshair (X, Y). Recomputed on aim input.
  coord2d crosshair_angle;
  // Saved: yes
  // crosshair_x_pos, crosshair_y_pos: Raw horizontal and vertical offsets of the crosshair. Bounded by manual aim movement limits (-1.0f to 1.0f).
  f32 crosshair_x_pos;
  f32 crosshair_y_pos;
  // Saved: yes
  // guncrossdamp, gunaimdamp: Dampening interpolation factors for crosshair and gun model aiming drift (0.0f to 1.0f).
  f32 guncrossdamp;
  f32 gunaimdamp;
  // Saved: yes
  // field_FFC: 2D screen coordinate projection of the weapon/bullet direction (X, Y). Transformed into 3D coords for shot trajectory projection.
  coord2d field_FFC;
  // Saved: yes
  // gun_azimuth_angle, gun_azimuth_turning: Horizontal gun yaw angle and turning velocity when aiming (in radians).
  f32 gun_azimuth_angle;
  f32 gun_azimuth_turning;
  // Saved: yes
  // field_1010: 3D look/aim angle vector (pitch, yaw, roll). Y is initialized to -M_PI.
  coord3d field_1010;
  // Saved: yes
  // field_101C: Float 3D rotation matrix (Mtxf) tracking the gun's visual orientation, generated from field_1010.
  Mtxf field_101C;
  // Saved: yes
  // last_z_trigger_timer: Ticks/frames elapsed since the Z button trigger was last released. Bounded typically to 0 and above, used to track time offsets for thrown weapon cooking (e.g. grenades).
  s32 last_z_trigger_timer;
  // Saved: yes
  // copiedgoldeneye: Boolean flag indicating if the GoldenEye Key has been analyzed/copied (Control Room objective status). Bounded to FALSE (0) or TRUE (1).
  s32 copiedgoldeneye;
  // Saved: yes
  // gunammooff: Bitfield of reasons for hiding the HUD ammo display (shares GUNSIGHTREASON_ constants). 0 if ammo is visible.
  s32 gunammooff;
  // Saved: yes
  // field_1068: Padding/unreferenced field in C source (likely padding or used in assembly).
  s32 field_1068;
  // Saved: yes
  // Current FOV values of the camera. Bounded between 60.0f (default) and 7.0f (maximum Sniper zoom).
  f32 sniper_zoom;
  f32 camera_zoom;

  // Saved: yes
  // Bitfield of reasons for hiding the crosshair sight (0 if sight is visible).
  //   - 0x01 (GUNSIGHTREASON_1): Watch menu active / paused.
  //   - 0x02 (GUNSIGHTREASON_NOTAIMING): Sight hidden because manual aim button is not held.
  //   - 0x04 (GUNSIGHTREASON_NOCONTROL): Sight hidden because the player lacks control.
  //   - 0x10 (GUNSIGHTREASON_DAMAGE): Sight hidden due to taking damage.
  s32 gunsightmode;

  // Saved: yes
  // insightaimmode: Boolean flag representing whether the player is currently manually aiming/sighting (holding R button to bring up aiming sight). Range: 0 (FALSE), 1 (TRUE).
  s32 insightaimmode;
  // Saved: yes
  // autoyaimenabled: Boolean/state flag indicating if vertical auto-aim is active (snapping target vertically). Range: 0 (disabled), 1 (enabled).
  s32 autoyaimenabled;
  // Saved: yes
  // autoaimy: Calculated vertical angular offset applied by auto-aim. Range: positive/negative floats in radians.
  f32 autoaimy;
  // Saved: yes
  // autoaim_target_y: Pointer to the PropRecord of the entity currently targeted vertically by auto-aim (stored as an index in the saved state to avoid stale pointer crashes).
  struct PropRecord *autoaim_target_y;
  // Saved: yes
  // autoyaimtime60: Frame count/timer for vertical auto-aim target lock. Increments each frame target is tracked, resets to 0 when target is lost.
  s32 autoyaimtime60;
  // Saved: yes
  // autoxaimenabled: Boolean/state flag indicating if horizontal auto-aim is active (snapping target horizontally). Range: 0 (disabled), 1 (enabled).
  s32 autoxaimenabled;
  // Saved: yes
  // autoaimx: Calculated horizontal angular offset applied by auto-aim. Range: positive/negative floats in radians.
  f32 autoaimx;
  // Saved: yes
  // autoaim_target_x: Pointer to the PropRecord of the entity currently targeted horizontally by auto-aim (stored as an index in the saved state to avoid stale pointer crashes).
  struct PropRecord *autoaim_target_x;
  // Saved: yes
  // autoxaimtime60: Frame count/timer for horizontal auto-aim target lock. Increments each frame target is tracked, resets to 0 when target is lost.
  s32 autoxaimtime60;
  // Saved: yes
  // Padding/unreferenced field in C source (likely padding or used in assembly).
  s32 field_112C;
  // Saved: yes
  // Transition timer and duration in frames/ticks for watch/sniper zoom transitions. Bounded between 0.0f and zoomintimemax (usually 30.0f or 45.0f).
  f32 zoomintime;
  f32 zoomintimemax;
  // Saved: yes
  // Current, starting, and ending target FOV angles (in degrees) for watch/sniper transitions.
  f32 zoominfovy;
  f32 zoominfovyold;
  f32 zoominfovynew;
  // Saved: yes
  // Render perspective parameters. Default FOV is 60.0f, default aspect is 1.333f (4:3) or 1.777f (16:9).
  f32 fovy;
  f32 aspect;

  // Saved: yes
  // Screen coordinate projection bounds (min/max X/Y as floats).
  // Define the range of screen coordinates used for world-to-screen projection calculations.
  // Derived from viewport dimensions and used to clamp projected 3D positions to screen space.
  f32 screenxminf;
  f32 screenyminf;
  f32 screenxmaxf;
  f32 screenymaxf;

  // Saved: yes
  // Camera scale and reciprocal scale factors for world-to-screen projection.
  // - c_scalex, c_scaley: Multiplicative scale factors applied during screen coordinate transformation.
  // - c_recipscalex, c_recipscaley: Reciprocal (1/scale) values for inverse transformations.
  f32 c_scalex;
  f32 c_scaley;
  f32 c_recipscalex;
  f32 c_recipscaley;

  // Saved: yes
  // Camera Level-of-Detail (LOD) scale factors for model detail selection.
  // - c_scalelod60: LOD scale factor at 60Hz tick rate.
  // - c_scalelod: Current LOD scale multiplier applied to model rendering detail.
  // - c_lodscalez: Z-depth based LOD scaling factor (float).
  // - c_lodscalezu32: Z-depth LOD scale stored as unsigned 32-bit integer (bitcast of c_lodscalez).
  f32 c_scalelod60;
  f32 c_scalelod;
  f32 c_lodscalez;
  u32 c_lodscalezu32;

  // Saved: yes
  // Camera normalization vectors for view orientation.
  // - c_cameratopnorm: Normalized "up" vector for the camera's local coordinate system.
  // - c_cameraleftnorm: Normalized "left" vector for the camera's local coordinate system.
  coord3d c_cameratopnorm;
  coord3d c_cameraleftnorm;

  // Saved: yes
  // N64 graphics library viewport structures (array of 2 for dual-screen support).
  // Each Vp contains vscale[4] (scale, 2-bit fraction) and vtrans[4] (translate, 2-bit fraction) arrays.
  // Controls the final screen-space projection and positioning of rendered geometry.
  Vp viewports[2];

  // Saved: yes
  // Viewport positioning bounds (signed 16-bit, 2-bit fraction format).
  // - viewx, viewy: Viewport center offset (scaled by 4 for 2-bit fraction).
  // - viewleft, viewtop: Viewport top-left corner coordinates (scaled by 4 for 2-bit fraction).
  s16 viewx;
  s16 viewy;
  s16 viewleft;
  s16 viewtop;

  // Saved: yes
  // Bitmasks tracking currently held and previous frame controller button presses. Range: 0 to 0xFFFF.
  u16 buttons_pressed;
  u16 prev_buttons_pressed;

  // Saved: yes
  // Boolean flag indicating if Bond is dead (0 = alive, 1 = dead).
  s32 bonddead;
  // Saved: yes
  // Saved horizontal/vertical camera look angles when dying.
  f32 thetadie;
  f32 vertadie;
  // Saved: yes
  // Player mesh/character model variant index during active state or cutscene/death.
  s32 bondtype;
  // Saved: yes
  // Boolean trigger indicating whether a new player death sequence has started (0 = FALSE, 1 = TRUE).
  s32 startnewbonddie;
  // Saved: yes
  // Boolean flag indicating if the fullscreen red blood drop overlay has finished drawing (0 = active/not finished, 1 = finished).
  s32 redbloodfinished;
  // Saved: yes
  // Boolean flag indicating if player's first-person death animation sequence has completed.
  s32 deathanimfinished;
  // Saved: yes
  // Padding/unreferenced field.
  s32 field_42c;
  // Saved: yes
  // Input configuration mode override during death animation sequence.
  s32 controldef;
  // Saved: yes
  // Death animation state flag. Set to 0 on initialization, 1 when the player dies and the death animation sequence commences.
  s32 field_D0;

  // Saved: yes
  // movecentrerelease: Boolean flag indicating if look-centering has been overridden by the player looking up/down manually. Resets to FALSE when the joystick is near neutral (deflection between -40 and 40).
  s32 movecentrerelease;
  // Saved: yes
  // lookaheadcentreenabled: Global configuration setting determining whether the camera auto-centers when moving forward.
  s32 lookaheadcentreenabled;
  // Saved: yes
  // automovecentreenabled: Active state flag indicating if auto-centering (look-ahead) is enabled for the current player's movement.
  s32 automovecentreenabled;
  // Saved: yes
  // fastmovecentreenabled: Active state flag determining whether fast centering is active (which snaps the vertical angle back to the horizon much faster when running).
  s32 fastmovecentreenabled;
  // Saved: yes
  // automovecentre: Boolean flag indicating if centering is currently happening (i.e. the camera is auto-panning back to center because the player is moving/running forward and look-ahead is active).
  s32 automovecentre;
  // Saved: yes
  // field_104: Auto-centering active flag. Set to 1 when the vertical camera pitch is actively auto-panning/centering back to the horizon, and 0 otherwise.
  s32 field_104;
  // Saved: yes
  // field_10C: Auto-centering override flag. Set to 1 when the player manually looks up/down (which cancels/overrides auto-centering), and 0 when centering is allowed.
  s32 field_10C;

  // Saved: yes
  // field_100: Unreferenced padding field. Never read or written in any C source. Declared in both bondview.h and initBondDATA.h but has zero references. Likely struct layout padding or leftover from original code. Initialized to 0 in player reset functions.
  s32 field_100;
  // Saved: yes
  // field_108: Unknown control/state flag. Reset to 0 in init_player_BONDdata and player_2 reset. No known readers or writers beyond initialization. May be used in non-matched assembly. Safe to save/restore as a plain s32.
  s32 field_108;

  // Saved: no (unsaved, recomputed each frame)
  // field_5C: Pointer to a dynamically allocated fixed-point 4x4 matrix (Mtx). Allocated via dynAllocateMatrix() in sub_GAME_7F0876C4 (camera setup). Stores the modelview matrix converted from float via guMtxF2L. Used in the display list via gSPMatrix in bondviewGfxPlayerField5cMatrix. NOT saved because the pointer is an absolute address into the dynamic memory pool that becomes stale across saves. The matrix data is purely derived from camera position and player position, both of which are already saved and will be recomputed on the next frame.
  s32 field_5C;
  // Saved: no (unsaved, recomputed each frame)
  // field_60: Pointer to a dynamically allocated fixed-point 4x4 matrix (Mtx). Allocated via dynAllocateMatrix(). Data is copied from field_5C via sub_GAME_7F059334 (matrix copy). Stored in field_10C4 via currentPlayerSetMatrix10C4. NOT saved for the same reasons as field_5C — recomputed each frame from saved camera/position state.
  s32 field_60;
  // Saved: no (unsaved, recomputed each frame)
  // field_64: Pointer to a dynamically allocated float 4x4 matrix (Mtxf). Built via matrix_4x4_7F059424 which constructs a camera-to-world (inverse view) transformation matrix from cam_pos, cam_look, cam_up. Stored in field_10CC via currentPlayerSetMatrix10CC. NOT saved because the matrix is recomputed each frame from camera parameters that are already preserved through position/angle state.
  s32 field_64;
  // Saved: no (unsaved, recomputed each frame)
  // field_68: Pointer to a dynamically allocated float 4x4 matrix (Mtxf). Built via matrix_4x4_7F059708 which constructs a world-to-camera (view) transformation matrix from cam_pos, cam_look, cam_up. Stored in field_10D4 via currentPlayerSetMatrix10D4. NOT saved because the matrix is recomputed each frame from camera parameters that are already preserved through position/angle state.
  s32 field_68;

  // Saved: yes
  // magnetattracttime: Active duration of auto-aim magnetic attraction in frames. Increments by g_ClockTimer when a target is locked, and resets to -1 when the lock is lost.
  s32 magnetattracttime;
  // Saved: yes
  // swaytarget: Target sideways camera tilt sway offset (e.g., -75.0f for lean left, 75.0f for lean right, 0.0f for neutral). Bounded between -75.0f and 75.0f.
  f32 swaytarget;
  // Saved: yes
  // swayoffset0: Current horizontal camera translation sway offset (roll/yaw translation), interpolated towards the sway target.
  f32 swayoffset0;
  // Saved: yes
  // swayoffset2: Current vertical/depth camera translation sway offset, interpolated towards the sway target.
  f32 swayoffset2;
  // Saved: yes
  // field_1280: Padding/unreferenced field in the C source (likely padding or used in assembly).
  f32 field_1280;
  // Saved: yes
  // players_cur_animation: Player's active character animation sequence index. Only initialized to 0, not referenced in C (likely padding or used in assembly).
  s32 players_cur_animation;
  // Saved: yes
  // field_1288: Padding/unreferenced field in the C source (likely padding or used in assembly).
  f32 field_1288;

  // Saved: yes
  // watch_pause_time: The elapsed frame counter/timer for the current watch menu animation state.
  s32 watch_pause_time;
  // Saved: yes
  // timer_1C4: Watch menu sub-tick transition/frame timer, accumulating clock ticks (g_ClockTimer) to time specific transition phases.
  s32 timer_1C4;
  // Saved: yes
  // watch_animation_state: Active watch menu animation state index (0 = gameplay, 1 = watch opening sweep, 2 = fully open, etc.). Bounded by enum WATCH_ANIMATION_STATE_IDS (0 to 13).
  s32 watch_animation_state;
  // Saved: yes
  // outside_watch_menu: Boolean/state flag that is TRUE (1) if the player is in regular gameplay (outside the watch menu), and FALSE (0) if inside the watch menu.
  s32 outside_watch_menu;
  // Saved: yes
  // open_close_solo_watch_menu: Toggle/request flag set to TRUE (1) when the Start button is pressed to request opening/closing the watch menu, and FALSE (0) otherwise.
  s32 open_close_solo_watch_menu;
  // Saved: yes
  // pausing_flag: Boolean flag that is TRUE (1) during active pause transition (while the watch is sweeping up/down), and FALSE (0) otherwise.
  s32 pausing_flag;
  // Saved: yes
  // pause_starting_angle: Player's vertical camera pitch angle (in degrees) when the watch menu was opened, used as the start point for pitch interpolation.
  f32 pause_starting_angle;
  // Saved: yes
  // pause_target_angle: Target pitch angle (in degrees) for the camera to reach during the pause transition (usually -40.0f to look at the watch, or original pitch when returning).
  f32 pause_target_angle;
  // Saved: yes
  // pause_related: Current vertical camera pitch angle during the pause transition, which interpolates between pause_starting_angle and pause_target_angle.
  f32 pause_related;
  // Saved: yes
  // pause_animation_counter: Float frame counter for watch screen slide-up/down animations, bounded between 0.0f (retracted) and 20.0f (fully visible).
  f32 pause_animation_counter;
  // Saved: yes
  // step_in_view_watch_animation: Step index in the watch menu slide animation: 0 = hidden, 1 = moving up, 2 = moving down, 3 = fully open.
  s32 step_in_view_watch_animation;
  // Saved: yes
  // pause_watch_related: Target duration or frame count for the watch menu animation sweep to complete.
  f32 pause_watch_related;
  // Saved: yes
  // pause_watch_related_scaled: Calculated animation speed multiplier used to scale frame increments based on target duration.
  f32 pause_watch_related_scaled;
  // Saved: yes
  // pause_watch_related_adjust: Float sub-frame progress tracking accumulator used when scaling the watch animation speed.
  f32 pause_watch_related_adjust;
  // Saved: yes
  // field_218: State variable tracking phase/type of watch transition. 0 = none, 1 = opening sweep, 2 = screen/page change, 3 = closing sweep.
  s32 field_218;
  // Saved: yes
  // field_21C: Watch menu transition override trigger flag (1 = active transition, 0 = idle).
  s32 field_21C;
  // Saved: yes
  // field_1D4: Float watch matrix/rotation/position sweep transition value 1.
  f32 field_1D4;
  // Saved: yes
  // field_1D8: Float watch matrix/rotation/position sweep transition value 2.
  f32 field_1D8;
  // Saved: yes
  // pause_watch_position: Current physical translation/slide offset coordinate of the watch on the screen (0.0f = off screen, up to 1.0f or more when fully raised).
  f32 pause_watch_position;
  // Saved: yes
  // field_1E0: Float watch matrix/rotation/position sweep transition value 3.
  f32 field_1E0;
  // Saved: yes
  // field_1E4: Float watch matrix/rotation/position sweep transition value 4.
  f32 field_1E4;
  // Saved: yes
  // field_1E8: Float watch matrix/rotation/position sweep transition value 5.
  f32 field_1E8;
  // Saved: yes
  // field_1EC: Float watch matrix/rotation/position sweep transition value 6.
  f32 field_1EC;
  // Saved: yes
  // field_1F0: Float watch matrix/rotation/position sweep transition value 7.
  f32 field_1F0;
  // Saved: yes
  // field_1F4: Float watch matrix/rotation/position sweep transition value 8.
  f32 field_1F4;
  // Saved: yes
  // field_1F8: Watch matrix/rotation/position sweep transition flag/state value 9.
  s32 field_1F8;
  // Saved: yes
  // field_1FC: Watch matrix/rotation/position sweep transition flag/state value 10.
  s32 field_1FC;
  // Saved: yes
  // field_210: Watch matrix/rotation/position sweep transition float value 11.
  f32 field_210;
  // Saved: yes
  // field_214: Watch matrix/rotation/position sweep transition float value 12.
  f32 field_214;
  // Saved: yes
  // something_with_watch_object_instance: Pointer index or reference number for the active 3D watch object model instance in the stage.
  s32 something_with_watch_object_instance;

  // Saved: yes
  // field_234: Watch sweep animation frame/timer step 1.
  s32 field_234;
  // Saved: yes
  // field_238: Watch sweep animation frame/timer step 2.
  s32 field_238;
  // Saved: yes
  // field_23C: Watch sweep animation frame/timer step 3.
  s32 field_23C;
  // Saved: yes
  // field_240: Watch sweep animation frame/timer step 4.
  s32 field_240;
  // Saved: yes
  // watch_scale_destination: Target sizing/scale state destination index for watch scaling transition animations.
  s32 watch_scale_destination;
  // Saved: yes
  // field_248: Watch sweep animation frame/timer step 5.
  s32 field_248;
  // Saved: yes
  // field_24C: Watch sweep animation frame/timer step 6.
  s32 field_24C;
  // Saved: yes
  // field_250: Watch sweep animation frame/timer step 7.
  s32 field_250;
  // Saved: yes
  // field_254: Watch sweep animation frame/timer step 8.
  s32 field_254;
  // Saved: yes
  // Vertex buffer coordinates and colors used for rendering the watch menu green backdrop pages.
  struct WatchRectangle buffer_for_watch_greenbackdrop_vertices[WATCH_NUMBER_SCREENS];
  // Saved: yes
  // Vertex buffer coordinates and colors used for static elements rendered in the watch menu backdrop overlay.
  struct WatchRectangle buffer_for_watch_static_vertices[1];
  // Saved: yes
  // hand_invisible: Array tracking left/right hand model rendering visibility. Values: 0 = visible, >0 = forced invisible, <0 = model reload trigger (e.g. -3 mid-switch).
  s32 hand_invisible[2];
  // Saved: yes
  // z_trigger_timer: Ticks/frames elapsed since the Z button trigger was pressed, driving continuous fire cooldown cycles and dual-wield triggers.
  s32 z_trigger_timer;
  // Saved: yes
  // resetshadecol: Boolean flag (0 = FALSE, 1 = TRUE) indicating if the weapon model's shading/lighting colors should be reset/recalculated on the next frame.
  s32 resetshadecol;
  // Saved: yes
  // aimtype: Active weapon aiming configuration mode index (typically 0).
  s32 aimtype;
  // Saved: yes
  // field_FCC: Current frame's logical Z-trigger (fire button) press status, used for firing weapon, detonating mines, and cooking/priming throwing weapons.
  s32 field_FCC;
  // Saved: yes
  // field_FD0: Previous frame's logical Z-trigger press status. Used in conjunction with field_FCC to detect edge triggers (e.g. button releases).
  s32 field_FD0;
  // Saved: yes
  // field_FD8: Firing hand index used during dual-wielding to track which hand is currently the active firing hand. Alternates between 0 (right hand) and 1 (left hand).
  s32 field_FD8;
  // Saved: yes
  // RGBA color representing the current StandTile's lighting color applied to weapon/hands model rendering shading.
  struct rgba_u8 tileColor;
  // Saved: yes
  // hudmessoff: Bitfield/boolean of reasons for hiding the HUD message display (e.g. 0 = visible, non-zero = reasons why it is hidden).
  s32 hudmessoff;
  // Saved: yes
  // bondmesscnt: Tick timer tracking how long the current HUD message remains on the screen. Bounded typically between -1 (no message) and 120 (max message duration).
  s32 bondmesscnt;
  // Saved: yes
  // c_screenwidth: Current viewport width in pixels (typically 320.0f).
  f32 c_screenwidth;
  // Saved: yes
  // c_screenheight: Current viewport height in pixels (typically 240.0f).
  f32 c_screenheight;
  // Saved: yes
  // c_screenleft: Viewport left edge coordinate offset (x-axis screen coordinate).
  f32 c_screenleft;
  // Saved: yes
  // c_screentop: Viewport top edge coordinate offset (y-axis screen coordinate).
  f32 c_screentop;
  // Saved: yes
  // c_perspnear: Near clipping plane distance for the 3D perspective projection (typically 10.0f).
  f32 c_perspnear;
  // Saved: yes
  // c_perspfovy: Viewport projection field-of-view angle (vertical FOV in degrees).
  f32 c_perspfovy;
  // Saved: yes
  // c_perspaspect: Aspect ratio (width divided by height) used for perspective projection calculations.
  f32 c_perspaspect;
  // Saved: yes
  // c_halfwidth: Half of the viewport screen width in pixels.
  f32 c_halfwidth;
  // Saved: yes
  // c_halfheight: Half of the viewport screen height in pixels.
  f32 c_halfheight;

  // Saved: yes
  // Health/Armor Scaling Multipliers:
  // - actual_health: Maximum health scaling factor used in damage calculations. Default 1.0f (normal health). Set to 2.0f by "Double Health" cheat, 10.0f by "Ten Lives" cheat. Damage is divided by this value when applied to bondhealth.
  // - actual_armor: Maximum armor scaling factor used in damage calculations. Default 1.0f (normal armor). Set to 2.0f by "Double Armor" cheat. Damage is divided by this value when applied to bondarmour.
  f32 actual_health;
  f32 actual_armor;

  // Saved: yes
  // Control Type & Watch Menu Configuration:
  // - cur_player_control_type_0: Primary watch menu controller layout index. Determines which weapon layout is displayed (HONEY=0, SOLITARE=1, KISSY=2, GOODNIGHT=3, PLENTY=4, GALORE=5, DOMINO=6, GOODHEAD=7, CINEMA=8). Set via watch menu page navigation.
  // - cur_player_control_type_1: Secondary watch menu controller layout index. Mirrors cur_player_control_type_0 in most cases.
  // - cur_player_control_type_2: Float version of the controller layout index (cur_player_control_type_0 cast to f32). Used in watch menu rendering calculations.
  // - neg_vspacing_for_control_type_entry: Negative vertical spacing value for watch menu entries. Calculated as -(langsize * type) where langsize depends on the layout. Controls vertical spacing between weapon slots in the watch menu.
  // - has_set_control_type_data: Boolean flag (0 or 1) indicating whether the control type data has been initialized. Set to TRUE after level load or watch menu init to prevent uninitialized layout access.
  // - cur_item_weapon_getname: Weapon/item ID representing the active item currently selected or held. Used by name and model queries (e.g. getCurrentWeaponOrItem).
  s32 cur_player_control_type_0;
  s32 cur_player_control_type_1;
  f32 cur_player_control_type_2;
  s32 neg_vspacing_for_control_type_entry;
  u32 has_set_control_type_data;
  s32 cur_item_weapon_getname;

  // Saved: yes
  // Transition, Environmental, and Clipping State tracking fields:
  // - field_2A44: Array of weapon IDs representing the target weapon currently being transitioned/swapped to. Used by the hand-switching logic.
  // - field_2A04: Active room index for player prop registration fallback. Used for deregistering/registering player prop rooms.
  // - field_2A08: Tank turret vertical rotation angle (when in control of a tank).
  // - field_2A0C: Tank turret vertical rotation rate/velocity.
  // - autocrouchpos: Target automatic crouch position enum when standing under low ceilings or vents (e.g. CROUCH_STAND or CROUCH_SQUAT).
  // - healthdisplaytime: Multiplayer health/armor overlay display timer. Bounded typically to 0, 50, or 60.
  // - damagetype: Damage level display category index (0 to 7) based on health fraction, used to determine damage flash color/intensity.
  // - field_8C: Clipping collision tick countdown timer used when transitioning rooms or floor heights.
  ITEM_IDS field_2A44[2];
  s16 field_2A04;
  f32 field_2A08;
  f32 field_2A0C;
  s32 autocrouchpos;
  s32 healthdisplaytime;
  s32 damagetype;
  s32 field_8C;

  // Saved: yes
  // Multiplayer, Stats, and Viewport Height/Dampening State Tracking:
  // - healthDamageType: Timings index for health/armor bar display transition (0 to 7), based on health fraction.
  // - field_29BC: Viewport camera perspective height scalar (based on player perspective options and bob animations).
  // - field_29C0: Vertical landing impact shake/dampening displacement offset (decays back to 0.0f).
  // - mpmenuon: Active multiplayer watch menu display flag (menu is open/rendering).
  // - mpmenumode: Multiplayer watch menu active mode/page state index.
  // - mpquitconfirm: Multiplayer quit confirmation dialog active status.
  // - mpjoywascentre: Multiplayer joystick centering calibration validation state.
  // - deathcount: Cumulative deaths stat counter.
  // - num_suicides: Cumulative suicides stat counter.
  // - field_29E0: Spawn point cyclical indexing accumulator.
  // - last_kill_time: Timestamps array tracking recent player kills.
  // - field_29F4: Mission timer offset tracking alive duration/spawn time.
  // - field_29F8: Multiplayer kills-in-one-life counter.
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

  // Fade & Breathing — s32 padding, unreferenced. Saved and restored as raw s32 values.
  s32 field_1A0; ... s32 field_1BC; // Padding between breathing and watch state. (SAVED)

  // Saved: yes
  // Watch state padding / unknown state fields (offsets 0x25C–0x3B0).
  // These 87 consecutive s32 fields sit between the watch state fields (ending at field_254 / pause_watch_related_adjust at 0x258)
  // and buttons_pressed (at 0x3B4). None are pointers — all plain s32 values.
  //
  // Specific investigated fields:
  // - field_25C (offset 0x25C): Animation state parameter that updates during watch slide, rotation, and scaling transitions.
  // - field_260 (offset 0x260): Floating-point scaling multiplier for random weapon matrix/translation shaking/static calculations.
  // - field_2EC (offset 0x2EC): Passed by address (&g_CurrentPlayer->field_2EC) to _sub_GAME_7F075FAC in sub_GAME_7F07E7CC (bondview.c:6723).
  //   This is the watch 3D model setup function used when opening the watch menu. It works alongside
  //   something_with_watch_object_instance and the watch model header to configure the watch 3D model.
  // - field_364 (offset 0x364): Part of the watch menu's opening/closing transition and slide rotation/scaling state math.
  //
  // Address Safety: All plain s32 values — no pointer staleness risk. Safe to save/restore as raw values.
  s32 field_25C;
  s32 field_260;
  s32 field_264 ... field_2E8;
  s32 field_2EC; // Only field with known C usage — watch 3D model setup parameter
  s32 field_2F0 ... field_360;
  s32 field_364; // Watch transition and slide state parameter
  s32 field_368 ... field_3B0;

  // Visual model & properties
  Model *model; // Pointer to 3D visual model instance. NOT saved/restored — the existing model pointer is kept as-is on load.
  s32 field_59C; ... s32 field_5B8; // Visual layout matrices / switches padding. Saved and restored.
  s8 field_5BD; s8 field_5BE; s8 field_5BF; // Padding bytes around animFlipFlag for struct alignment. Saved and restored.
  s32 field_5C4 ... s32 field_650;
  s32 field_654; // runtime data pointer (allocated fresh on model creation).
  s32 field_658 ... s32 field_6CC; // Visual layout matrices / switches padding. Saved and restored.

  // Saved: partially (recomputed: yes)
  // Graphics display list buffer for the body armor bar graphic in the watch menu and HUD.
  // Actually a Gfx array of size 37 (296 bytes). The subsequent decompiled fields (field_19FC to field_1B1C) represent the rest of this buffer.
  // Its contents are regenerated on post-load using sub_GAME_7F0A3330 to ensure physical address pointers are correct.
  s32 watch_body_armor_bar_gdl;
  // Display list buffer referenced by watch_body_armor_bar_gdl.
  s32 field_19FC ... field_1B1C;

  // Saved: partially (recomputed: yes)
  // Graphics display list buffer for the health bar graphic in the watch menu and HUD.
  // Actually a Gfx array of size 37 (296 bytes). The subsequent decompiled fields (field_212C to field_224C) represent the rest of this buffer.
  // Its contents are regenerated on post-load using sub_GAME_7F0A3330 to ensure physical address pointers are correct.
  s32 watch_health_bar_gdl;
  // Display list buffer referenced by watch_health_bar_gdl.
  s32 field_212C ... field_224C;


  // Rendering Matrices & Camera projection variables

  // Saved: no (unsaved, recomputed each frame)
  // field_10C4: Pointer to a dynamically allocated RSP fixed-point 4x4 matrix (Mtx) used for modelview/camera projection. NOT saved because the memory pointer is volatile. Recalculated automatically on the next frame from player position and camera angles.
  Mtx* field_10C4;

  // Saved: no (unsaved, recomputed each frame)
  // field_10C8: Pointer to a dynamically allocated RSP fixed-point 4x4 matrix (Mtx) used for secondary viewport projection. NOT saved because the memory pointer is volatile. Recalculated automatically on the next frame.
  Mtx* field_10C8;

  // Saved: no (unsaved, recomputed each frame)
  // field_10CC: Pointer to the camera's float view matrix (Mtxf) representing the world-to-camera projection. NOT saved because it is re-derived dynamically every frame from current camera parameters.
  Mtxf* field_10CC;

  // Saved: no (unsaved, recomputed each frame)
  // field_10D0: Pointer to dynamically allocated viewport or camera matrix metadata. NOT saved; automatically re-initialized and re-evaluated by the camera pipeline on the next frame.
  s32 field_10D0;

  // Saved: no (unsaved, recomputed each frame)
  // field_10D4: Pointer to the camera's active view float matrix (Mtxf). NOT saved as it is recalculated on the next frame from camera parameters.
  Mtxf* field_10D4;

  // Saved: no (unsaved, recomputed each frame)
  // projmatrix: Pointer to the dynamically allocated RSP fixed-point perspective projection matrix (Mtx). NOT saved because it is recomputed based on screen aspect ratio and FOV.
  Mtx* projmatrix;

  // Saved: no (unsaved, recomputed each frame)
  // projmatrixf: Pointer to the float perspective projection matrix (Mtxf). NOT saved because it is re-derived dynamically every frame from current camera parameters.
  Mtxf* projmatrixf;

  // Saved: no (unsaved, recomputed each frame)
  // field_10E0: Pointer to dynamically allocated viewport or camera projection metadata. NOT saved; dynamically allocated and set during camera viewport initialization.
  s32 field_10E0;

  // Saved: no (unsaved, recomputed each frame)
  // field_10E4: Pointer to dynamically allocated viewport or camera projection metadata. NOT saved; dynamically allocated and set during camera viewport initialization.
  s32 field_10E4;

  // Saved: no (unsaved, recomputed each frame)
  // field_10E8: Pointer to the previous frame's float view matrix (Mtxf) (pushed from field_10CC). NOT saved as it is updated when field_10CC is set on the next frame.
  Mtxf* field_10E8;

  // Saved: no (unsaved, recomputed each frame)
  // field_10EC: Pointer to the previous frame's float matrix (Mtxf) (pushed from field_10D4). NOT saved as it is updated when field_10D4 is set on the next frame.
  Mtxf* field_10EC;

  // Inventory & Ammo Arrays

  // Saved: no (unsaved, recomputed when damage occurs)
  // bloodImgCur: Pointer to the texture resource for the active/current fullscreen blood drop effect drawn on damage. NOT saved because it is a direct pointer to volatile graphics memory.
  u8 *bloodImgCur;

  // Saved: no (unsaved, recomputed when damage occurs)
  // bloodImgNxt: Pointer to the next texture resource/buffer in the blood drop sequence. NOT saved.
  u8 *bloodImgNxt;

  // Saved: no (unsaved, recomputed when damage occurs)
  // bloodImgBufPtrArray: Array of two texture pointers mapping the double-buffered blood splatter overlay assets. NOT saved.
  u8 *bloodImgBufPtrArray[2];

  // Saved: no (unsaved, reconstructed on load)
  // p_itemcur: Pointer to the current equipped InvItem structure. NOT saved because InvItem links are absolute heap addresses that can become stale. The equipped item state is fully restored by reloading the item/weapon models and updating equipcuritem on post-load.
  InvItem *p_itemcur;

  // Saved: partially (preserved on load)
  // textoverrides: Pointer to text override structures (e.g. customized weapon/objective names). During state restoration, the existing pointer is preserved as-is before rebuilding the inventory to prevent leaking memory or breaking custom overrides.
  textoverride *textoverrides;

  // Gaps & Watch display lists (Not saved, reconstructed dynamically on load)

  // Saved: partially (recomputed: yes)
  // field_1B20 ... field_2124: Gap A: Unnamed padding/private fields between the watch body armor bar graphic display list and the health bar graphic display list (1544 bytes).
  s32 field_1B20; ... s32 field_2124;

  // Saved: partially (recomputed: yes)
  // field_2250 ... field_2854: Gap B: Unnamed fields between the watch health bar graphic display list and the watch backdrop display lists (1544 bytes). Includes watch graphics display list command buffers (like buffer_for_watch_greenbackdrop_DL) that are dynamically reconstructed on post-load using game functions to ensure valid physical address pointers.
  s32 field_2250; ... s32 field_2854;

  // Saved: partially (recomputed: yes)
  // field_28E0 ... field_2994: Gap C: Unnamed fields between the green backdrop display list and the watch static display list (184 bytes). Dynamically reconstructed on post-load to ensure correct pointers.
  s32 field_28E0; ... s32 field_2994;

  // Saved: partially (recomputed: yes)
  // field_29B0 ... field_29B4: Gap D: Unnamed padding fields between the static watch display list and healthDamageType (8 bytes). Not saved/restored.
  s32 field_29B0; ... s32 field_29B4;

  // Control type & viewport bounds (remaining)

  // Saved: no (unsaved, trailing padding)
  // field_2A74, field_2A78, field_2A7C: Trailing padding/unreferenced fields at the end of the player structure. Initialized to 0 and not used elsewhere.
  s32 field_2A74;
  s32 field_2A78;
  s32 field_2A7C;
}

// Collision data representing player bounds, collision coordinates, and portal tracking.
struct collision434 {
  // Pointer to the current level StandTile the player is standing on.
  // Address Safety: Serialized as a byte offset relative to standTileStart and reconstructed on load.
  struct StandTile *current_tile_ptr;

  // Exact coordinates resolved after collision calculations.
  coord3d collision_position;

  // View rotation vector component that affects Bond's movement (but not the viewport).
  struct coord3d theta_transform;

  // Alternate/previous floor level coordinate.
  coord3d pos3;

  // Radius of the player's bounding cylinder for collision checks.
  f32 collision_radius;

  // Alternate/previous camera eye level coordinate.
  coord3d pos;

  // Visual camera rotation applied vector.
  struct coord3d applied_view;

  // Secondary visual camera rotation vector.
  struct coord3d applied_view2;

  // Pointer to the StandTile crossed during portal/room transitions.
  // Address Safety: Serialized as a byte offset relative to standTileStart and reconstructed on load.
  StandTile *current_tile_ptr_for_portals;
}

// Doubly-linked circular inventory item node.
struct InvItem {
  s32 type; // Item type: 1 = INV_ITEM_WEAPON, 2 = INV_ITEM_PROP, 3 = INV_ITEM_DUAL
  union {
    struct invitem_weap type_weap; // weapon ID
    struct invitem_prop type_prop; // PropRecord pointer for keys/objectives
    struct invitem_dual type_dual; // right/left hand weapon IDs for dual wielding
  } type_inv_item;
  struct InvItem *next; // circular pointer to next item
  struct InvItem *prev; // circular pointer to previous item
}

// Logical status, ammo tracking, and state of each hand viewport/model.
struct hand {
  ITEM_IDS weaponnum; // current active logical weapon
  ITEM_IDS weaponnum_watchmenu; // weapon selected in watch menu or -1
  ITEM_IDS previous_weapon; // previously held weapon
  s8 weapon_firing_status; // firing status
  s32 weapon_hold_time; // duration weapon has been held
  s32 when_detonating_mines_is_0; // mine detonator logic status
  s32 weapon_current_animation; // active animation index (0 = idle, 9 = reload, 0xE = draw)
  s32 weapon_ammo_in_magazine; // bullets currently in the magazine
  s32 weapon_next_weapon; // weapon being equipped
  s32 weapon_animation_trigger; // animation trigger flag
  ALSoundState *audioHandle; // absolute audio handle pointer (nullified on load to prevent crashes)
}
```
