# Collision Data (collision_data)

This document describes the structure and fields of the dynamic collision block representation (`struct collision_data`) used by the GoldenEye 007 engine for interactive objects and doors.

```c
struct collision_data {
    /**
     * Offset 0x00.
     * The number of edges/vertices in the 2D projected collision polygon (usually 4).
     * Set to 0 when collision is disabled (e.g., when a door is fully open).
     */
    s32 unk00;

    /**
     * Offset 0x04.
     * A rect4f structure containing the 4 vertices defining the 2D collision boundary
     * projected on the horizontal (X-Z) plane.
     */
    rect4f unk04;

    /**
     * Offsets 0x24 - 0x40.
     * Line equation coefficients / edge normal vectors used for point-in-polygon checks.
     * These define the equations of the 4 edges of the bounding quadrilateral.
     */
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    s32 unk40;

    /**
     * Offset 0x44.
     * The lower vertical boundary (ymin) of the collision block.
     */
    f32 unk44;

    /**
     * Offset 0x48.
     * The upper vertical boundary (ymax) of the collision block.
     */
    f32 unk48;
};
```

---

## State Save/Load and Restoration

### Serialization

During save state serialization, `ptr_allocated_collisiondata_block` is **not saved** to SRAM. This is because:

1. It is a raw memory pointer pointing to a dynamically allocated block in the stage memory pool (`MEMPOOL_STAGE`).
2. The exact values within this block are entirely dependent on the physical state (position, rotation matrix, open fraction) of the parent entity.

### Restoration

When loading a save state:

1. **Preserve the pointer:** Do not set `ptr_allocated_collisiondata_block` to `NULL` or try to deserialize a saved address. Since save states are only loadable within the same level instance, the memory block is already allocated and the pointer is valid.
2. **Recompute the contents:** Instead of copying raw values, trigger a full recomputation of the collision boundary by calling the engine's built-in updates:
    - For doors, call `door7F052B00(door);`.
    - For other objects, call `chrobjCollisionRelated(obj);`.
      These functions will read the door/object position, orientation matrix, and model bounding box, and completely recalculate all fields of the `collision_data` block.
