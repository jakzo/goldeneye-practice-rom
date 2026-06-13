#include "practice_states_bond.h"
#include "practice_states_globals.h"
#include "practice_states_utils.h"
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
extern int bondinvAddPropToInv(PropRecord *prop);
extern s32 g_EnterTankAudioState;
extern void *memcpy(void *dst, const void *src, size_t count);

static void save_player_state(SavedPlayerState *dst, struct player *src) {
    u8 *src_bytes = (u8 *)src;

    // Copy contiguous chunks
    memcpy(dst->block1, src_bytes + 0x0000, 0x0808);
    memcpy(dst->block2, src_bytes + 0x0870, 0x0854);
    memcpy(dst->block3, src_bytes + 0x10F0, 0x00B8);
    memcpy(dst->block4, src_bytes + 0x11B8, 0x06C0);
    memcpy(dst->block5, src_bytes + 0x29B8, 0x00B8);
}

static void load_player_state(struct player *dst, SavedPlayerState *src) {
    u8 *dst_bytes = (u8 *)dst;

    // Backup pointers to preserve live addresses
    s32 backup_field_5C = dst->field_5C;
    s32 backup_field_60 = dst->field_60;
    s32 backup_field_64 = dst->field_64;
    s32 backup_field_68 = dst->field_68;
    PropRecord *backup_prop = dst->prop;
    struct Model *backup_ptr_char_objectinstance = dst->ptr_char_objectinstance;
    Model *backup_model = dst->model;
    ObjectRecord *backup_hand_rocket[2];
    InvItem *backup_ptr_inventory_first_in_cycle = dst->ptr_inventory_first_in_cycle;
    InvItem *backup_p_itemcur = dst->p_itemcur;
    textoverride *backup_textoverrides = dst->textoverrides;

    backup_hand_rocket[0] = dst->hands[0].rocket;
    backup_hand_rocket[1] = dst->hands[1].rocket;

    // Copy contiguous chunks
    memcpy(dst_bytes + 0x0000, src->block1, 0x0808);
    memcpy(dst_bytes + 0x0870, src->block2, 0x0854);
    memcpy(dst_bytes + 0x10F0, src->block3, 0x00B8);
    memcpy(dst_bytes + 0x11B8, src->block4, 0x06C0);
    memcpy(dst_bytes + 0x29B8, src->block5, 0x00B8);

    // Restore pointers
    dst->field_5C = backup_field_5C;
    dst->field_60 = backup_field_60;
    dst->field_64 = backup_field_64;
    dst->field_68 = backup_field_68;
    dst->prop = backup_prop;
    dst->ptr_char_objectinstance = backup_ptr_char_objectinstance;
    dst->model = backup_model;

    dst->hands[0].rocket = backup_hand_rocket[0];
    dst->hands[1].rocket = backup_hand_rocket[1];

    dst->ptr_inventory_first_in_cycle = backup_ptr_inventory_first_in_cycle;
    dst->p_itemcur = backup_p_itemcur;
    dst->textoverrides = backup_textoverrides;
}

void save_bond_state(SavedBondState *dst) {
    if (g_CurrentPlayer == NULL) {
        return;
    }

    // 1. Perform structural copy of only active player fields
    save_player_state(&dst->saved_player, g_CurrentPlayer);

    // 2. Perform pointer-to-offset/index conversions for tile and prop pointers
    dst->room_pointer_offset = get_tile_offset(g_CurrentPlayer->room_pointer);
    dst->field_488_current_tile_ptr_offset = get_tile_offset(g_CurrentPlayer->field_488.current_tile_ptr);
    dst->field_488_current_tile_ptr_for_portals_offset = get_tile_offset(g_CurrentPlayer->field_488.current_tile_ptr_for_portals);
    dst->previous_collision_info_current_tile_ptr_offset = get_tile_offset(g_CurrentPlayer->previous_collision_info.current_tile_ptr);
    dst->previous_collision_info_current_tile_ptr_for_portals_offset = get_tile_offset(g_CurrentPlayer->previous_collision_info.current_tile_ptr_for_portals);
    dst->field_2A70_offset = get_tile_offset(g_CurrentPlayer->field_2A70);

    dst->autoaim_target_y_index = get_prop_index(g_CurrentPlayer->autoaim_target_y);
    dst->autoaim_target_x_index = get_prop_index(g_CurrentPlayer->autoaim_target_x);

    // 3. Save player's physical prop state
    if (g_CurrentPlayer->prop != NULL) {
        dst->has_prop = TRUE;
        dst->prop_pos = g_CurrentPlayer->prop->pos;
        dst->prop_stan_offset = get_tile_offset(g_CurrentPlayer->prop->stan);
        dst->prop_rooms[0] = g_CurrentPlayer->prop->rooms[0];
        dst->prop_rooms[1] = g_CurrentPlayer->prop->rooms[1];
        dst->prop_rooms[2] = g_CurrentPlayer->prop->rooms[2];
        dst->prop_rooms[3] = g_CurrentPlayer->prop->rooms[3];
    } else {
        dst->has_prop = FALSE;
    }

    // 4. Save player inventory linked list to flat array
    {
        s32 i;
        for (i = 0; i < 50; i++) {
            dst->inv_items[i].type = -1;
            dst->inv_items[i].weapon_right = -1;
            dst->inv_items[i].weapon_left = -1;
            dst->inv_items[i].prop_index = -1;
        }
    }
    dst->num_inv_items = 0;

    {
        InvItem *first = g_CurrentPlayer->ptr_inventory_first_in_cycle;
        InvItem *item = first;
        s32 count = 0;
        if (item != NULL) {
            do {
                if (count >= 50) break;
                dst->inv_items[count].type = item->type;
                if (item->type == INV_ITEM_WEAPON) {
                    dst->inv_items[count].weapon_right = item->type_inv_item.type_weap.weapon;
                } else if (item->type == INV_ITEM_DUAL) {
                    dst->inv_items[count].weapon_right = item->type_inv_item.type_dual.weapon_right;
                    dst->inv_items[count].weapon_left = item->type_inv_item.type_dual.weapon_left;
                } else if (item->type == INV_ITEM_PROP) {
                    dst->inv_items[count].prop_index = get_prop_index(item->type_inv_item.type_prop.prop);
                }
                count++;
                item = item->next;
            } while (item != first && item != NULL);
        }
    }
}

void load_bond_state(SavedBondState *src) {
    s32 preload_hand_item[2];
    textoverride *live_textoverrides;

    if (g_CurrentPlayer == NULL) {
        return;
    }

    // 1. Backup the hand weapon items and text overrides held before loading state
    preload_hand_item[0] = g_CurrentPlayer->hand_item[0];
    preload_hand_item[1] = g_CurrentPlayer->hand_item[1];
    live_textoverrides = g_CurrentPlayer->textoverrides;

    // 2. Perform copy of only active player fields from saved state
    load_player_state(g_CurrentPlayer, &src->saved_player);

    // 3. Resolve tile pointers and target prop pointers from saved offsets/indices
    g_CurrentPlayer->room_pointer = get_tile_by_offset(src->room_pointer_offset);
    g_CurrentPlayer->field_488.current_tile_ptr = get_tile_by_offset(src->field_488_current_tile_ptr_offset);
    g_CurrentPlayer->field_488.current_tile_ptr_for_portals = get_tile_by_offset(src->field_488_current_tile_ptr_for_portals_offset);
    g_CurrentPlayer->previous_collision_info.current_tile_ptr = get_tile_by_offset(src->previous_collision_info_current_tile_ptr_offset);
    g_CurrentPlayer->previous_collision_info.current_tile_ptr_for_portals = get_tile_by_offset(src->previous_collision_info_current_tile_ptr_for_portals_offset);
    g_CurrentPlayer->field_2A70 = get_tile_by_offset(src->field_2A70_offset);

    g_CurrentPlayer->autoaim_target_y = get_safe_prop_by_index(src->autoaim_target_y_index);
    g_CurrentPlayer->autoaim_target_x = get_safe_prop_by_index(src->autoaim_target_x_index);

    // 4. Recalculate derived look parameters from restored vv_theta / vv_verta
    bondviewApplyVertaTheta();

    // 5. Sync player's world PropRecord properties (rather than overwriting pointer)
    if (src->has_prop) {
        if (g_CurrentPlayer->prop != NULL) {
            chrpropDeregisterRooms(g_CurrentPlayer->prop);

            g_CurrentPlayer->prop->pos = src->prop_pos;
            g_CurrentPlayer->prop->stan = get_tile_by_offset(src->prop_stan_offset);
            g_CurrentPlayer->prop->rooms[0] = src->prop_rooms[0];
            g_CurrentPlayer->prop->rooms[1] = src->prop_rooms[1];
            g_CurrentPlayer->prop->rooms[2] = src->prop_rooms[2];
            g_CurrentPlayer->prop->rooms[3] = src->prop_rooms[3];

            chrpropRegisterRooms(g_CurrentPlayer->prop);
        } else {
            g_CurrentPlayer->prop = chrpropAllocate();
            if (g_CurrentPlayer->prop != NULL) {
                g_CurrentPlayer->prop->obj = NULL;
                g_CurrentPlayer->prop->type = PROP_TYPE_VIEWER;
                g_CurrentPlayer->prop->pos = src->prop_pos;
                g_CurrentPlayer->prop->stan = get_tile_by_offset(src->prop_stan_offset);
                g_CurrentPlayer->prop->rooms[0] = src->prop_rooms[0];
                g_CurrentPlayer->prop->rooms[1] = src->prop_rooms[1];
                g_CurrentPlayer->prop->rooms[2] = src->prop_rooms[2];
                g_CurrentPlayer->prop->rooms[3] = src->prop_rooms[3];

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

    // 6. Sync player's 3D visual Model instance
    if (g_CurrentPlayer->ptr_char_objectinstance != NULL) {
        setsuboffset(g_CurrentPlayer->ptr_char_objectinstance, &g_CurrentPlayer->pos);
        setsubroty(g_CurrentPlayer->ptr_char_objectinstance, get_curplay_horizontal_rotation_in_degrees());
    }

    // 7. Re-initialize player inventory, preserving text overrides
    {
        bondinvReinitInv();
        g_CurrentPlayer->textoverrides = live_textoverrides;
    }

    // 8. Restore saved inventory items
    {
        s32 i;
        for (i = 0; i < src->num_inv_items; i++) {
            s32 type = src->inv_items[i].type;
            if (type == INV_ITEM_WEAPON) {
                bondinvAddInvItem(src->inv_items[i].weapon_right);
            } else if (type == INV_ITEM_DUAL) {
                bondinvAddDoublesInvItem(src->inv_items[i].weapon_right, src->inv_items[i].weapon_left);
            } else if (type == INV_ITEM_PROP) {
                PropRecord *prop = get_prop_by_index(src->inv_items[i].prop_index);
                if (prop != NULL) {
                    bondinvAddPropToInv(prop);
                }
            }
        }
    }

    // 9. Restore hand weapon model loading and logical animation triggers
    {
        struct hand *saved_hands = (struct hand *)src->saved_player.block2;
        s32 hand;
        for (hand = 0; hand < 2; hand++) {
            ITEM_IDS saved_weapon = saved_hands[hand].weaponnum;

            if (preload_hand_item[hand] != saved_weapon) {
                g_CurrentPlayer->hand_invisible[hand] = -3;
                g_CurrentPlayer->field_2A44[hand] = saved_weapon;
                g_CurrentPlayer->hand_item[hand] = ITEM_UNARMED;
                g_CurrentPlayer->hands[hand].weapon_current_animation = 0;
                g_CurrentPlayer->lock_hand_model[hand] = 0;
                used_to_load_1st_person_model_on_demand(hand);
            } else {
                g_CurrentPlayer->hands[hand].weapon_current_animation = saved_hands[hand].weapon_current_animation;
            }

            g_CurrentPlayer->hands[hand].audioHandle = NULL;
        }
    }

    // 10. Re-generate watch menu GDLs and backdrop segment pointers to update absolute physical addresses
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
