```c
// Level pathfinding/collision grid polygon tile.
struct StandTile {
  // Tile ID bitfield containing GroupID and other identifiers.
  u32 id : 24;

  // The room index this tile belongs to.
  u8 room;

  // Midpoint tile header/half-word links.
  union {
    StandTileHeaderMid headerMid;
    s16 half;
  } mid;

  // Tail-end links and connection bitfields.
  union {
    StandTileHeaderTail hdrTail;
    s16 half;
  } tail;

  // Array of points defining the vertices of the tile polygon.
  StandTilePoint points[];
}

// Global prop definition mapping objects, doors, and characters in the world.
struct PropRecord {
  // Prop type enum.
  u8 type;

  // Flags representing visibility and interaction states.
  u8 flags;

  // Ticks remaining before regenerating/respawning.
  s16 timetoregen;

  // Union pointing to the underlying entity type.
  union {
    struct ChrRecord *chr;
    struct ObjectRecord *obj;
    struct DoorRecord *door;
    struct WeaponObjRecord *weapon;
    struct Explosion *explosion;
    struct Smoke *smoke;
    struct Scorch *scorch;
    void *voidp;
  };

  // 3D world coordinate position.
  coord3d pos;

  // Pointer to the StandTile the prop is currently resting on.
  StandTile *stan;

  // Render depth for camera sorting.
  f32 zDepth;

  // Relationships inside the prop hierarchy:
  struct PropRecord *parent;
  struct PropRecord *child;
  struct PropRecord *prev;
  struct PropRecord *next;

  // The room indices this prop occupies (terminated by 0xFF).
  u8 rooms[PROPRECORD_STAN_ROOM_LEN];

  s32 unk30;
}

// 3D model instance representation in stage memory.
struct Model {
  s16 unk00;
  s16 Type;

  // Pointer to the character entity record.
  struct ChrRecord *chr;

  // Pointer to the model file header containing vertices and hierarchy.
  ModelFileHeader *obj;

  // View render matrix mappings.
  RenderPosView *render_pos;
  union ModelRwData **datas;

  // Model visual scale.
  f32 scale;

  // Attachment hierarchy relationships:
  struct Model *attachedto;
  ModelNode *attachedto_objinst;

  // Pointer to current animation controller.
  ModelAnimation *anim;

  s8 gunhand;
  s8 unk25;
  s8 animlooping;
  s8 unk27;

  f32 unk28;
  f32 unk2c;

  s16 framea;
}
```
