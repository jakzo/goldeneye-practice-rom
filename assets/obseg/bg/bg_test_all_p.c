#include "bg_all_p.h"

extern u32 point_table_binary[];
extern u32 pri_mapping_binary[];

/*
 * A single, deliberately plain room. The two triangles cover a 600 metre
 * square; collision is supplied by the matching stan resource.
 */
struct bg_header header = {
    0, &room_data_table, &portal_data_table, &global_visibility_commands, 0};

struct room_data_table_entry room_data_table[] = {
    {0},
    {&point_table_binary, &pri_mapping_binary, 0, 0.0f, 0.0f, 0.0f},
    {0},
};

u32 global_visibility_commands[] = {0x00010000, 0};

struct portal_data_table_entry portal_data_table[] = {
    {0},
};

/* Four grey N64 vertices, Rare-1172 compressed. */
u32 point_table_binary[] = {
    0x1172EBBA, 0xC0C0D005, 0xC4205050, 0x50F0BFD4,
    0x00930FC2, 0x303E480E, 0x990F0000,
};

/* Minimal display list drawing the plane, Rare-1172 compressed. */
u32 pri_mapping_binary[] = {
    0x11727BCE, 0x0001BB18, 0x44981804, 0x18187632, 0x30CBF248,
    0x389EF8A3, 0xB680455E, 0x60F2FF5D, 0x0C028C0C, 0x8C0C0CBB,
    0x193418FF, 0x03C10109, 0x06260606, 0x66BB5D0C, 0x824C107D,
    0x3C405A81, 0x613BD814, 0x0586DF40, 0xB2A0A0E0, 0x3F8B0183,
    0x031F90BD, 0x1F2CCE25, 0x02A145E4, 0x7640ED03, 0x00000000,
};
