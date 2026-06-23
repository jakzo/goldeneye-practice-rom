#line 1 "src/game/model.c"
#include <ultra64.h>
#include <memp.h>
#include "../rmon.h" /*<PR/rmon.h>*/
#include "chr.h"
#include "objecthandler.h"
#include "math_floor.h"
#include "math_ceil.h"
#include "quaternion.h"
#include "math_asinfacosf.h"
#include "math_unk_05A9E0.h"
#include "chrobjdata.h"
#include "bondview.h"
#include "random.h"
#include "gbi_extension.h"

// forward declarations

void sub_GAME_7F06D490(struct Model *arg0, struct ModelNode *arg1);
void sub_GAME_7F0755B0(void);
s32 loadAnimationFrame(struct ModelAnimation *, s32, struct ModelSkeleton*);

// end forward declarations





//newfile per EU
bool modelmgrCanSlotFitRwdata(Model *modelslot, ModelFileHeader *modeldef)
{
    return modeldef->numRecords <= 0
        || (modelslot->datas != NULL && modelslot->Type >= modeldef->numRecords);
}


#ifdef NONMATCHING
struct PropRecord * get_obj_instance_controller_for_header(struct ModelFileHeader* arg0)
{
    #ifdef DEBUG
    if (arg0 + 5 > 19) osSyncPrintf("WARNING: increase OISAVESIZE to %d!\n", *(arg0 + 5));
    #endif
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
}
#endif


void clear_model_obj(Model* model)
{
    model->obj = NULL;
}


#ifdef NONMATCHING
void get_aircraft_obj_instance_controller(void) {
    #ifdef DEBUG
    if (arg0 + 0x14 > 140) osSyncPrintf("WARNING: increase OISAVESIZE to %d!\n", *(arg0 + 0x14));
    #endif
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
}
#endif






void modelAttachHead(Model *model, ModelNode *node,  ModelFileHeader *head)
{
    modelAttachPart(model,model->obj,node,head);
#ifdef DEBUG
    if (model + 0x14 > 140 && g_ModelDistanceScale == 0) osSyncPrintf("WARNING: increase OASAVESIZE to %d!\n", *(model + 0x14));
#endif

    modelInitRwData(model,head->RootNode);
}

void clear_aircraft_model_obj(Model *objinstance)

{
    objinstance->obj = NULL;
    return;
}

void modelSetDistanceDisabled(s32 param_1) {
  g_ModelDistanceDisabled = param_1;
}

// PD: modelSetDistanceScale
void modelSetDistanceScale(f32 param_1) {
  g_ModelDistanceScale = param_1;
}





#ifdef NONMATCHING
void sub_GAME_7F06C418(void) {

}
#else

void _asmpp_func4(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
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





void set_vtxallocator(s32 param_1) {
  vtxallocator = param_1;
}


#if defined(LEFTOVERDEBUG)
//called after a debug print during failed model operation
void return_null(void) {
  return;
}
#endif


void sub_GAME_7F06C474(Model* model, coord3d* coord)
{
    Mtxf* mtx;
    f32 dist;
    f32 neg_x;
    f32 neg_y;
    f32 neg_z;
    f32 inv_dist;

    mtx = getsubmatrix(model);
    neg_x = -mtx->m[3][0];
    neg_y = -mtx->m[3][1];
    neg_z = -mtx->m[3][2];

    dist = sqrtf((neg_x * neg_x) + (neg_y * neg_y) + (neg_z * neg_z));
    if (dist > 0.0f)
    {
        inv_dist = 1.0f / (model->scale * dist);
        coord->f[0] = neg_x * inv_dist;
        coord->f[1] = neg_y * inv_dist;
        coord->f[2] = neg_z * inv_dist;
        return;
    }

    coord->f[0] = 0.0f;
    coord->f[1] = 0.0f;
    coord->f[2] = 1.0f / model->scale;
}


void sub_GAME_7F06C550(Model* model, coord3d* coord)
{
  sub_GAME_7F06C474(model, coord);
}


// PD: model0001a524
s32 modelFindNodeMtxIndex(ModelNode *node, s32 arg1)
{
    s32 index;
    union ModelRoData *rodata1;
    union ModelRoData *rodata2;
    union ModelRoData *rodata3;
    union ModelRoData *rodata4;

    while (node)
    {
        switch (node->Opcode & 0xff)
        {
            case MODELNODE_OPCODE_HEADER:
                rodata1 = node->Data;
                return (s16)rodata1->Header.ModelType;

            case MODELNODE_OPCODE_GROUP:
                rodata2 = node->Data;
                return rodata2->Group.MatrixIDs[arg1 == 0x200 ? 2 : (arg1 == 0x100 ? 1 : 0)];

            case MODELNODE_OPCODE_OP03:
                rodata3 = node->Data;
                return rodata3->Group.MatrixIDs[arg1 == 0x200 ? 2 : (arg1 == 0x100 ? 1 : 0)];

            case MODELNODE_OPCODE_GROUPSIMPLE:
                rodata4 = node->Data;
                return rodata4->GroupSimple.Group1;
                break;
        }

        node = node->Parent;
    }

    return -1;
}


// PD: model0001a5cc
Mtxf *modelFindNodeMtx(struct Model *model, struct ModelNode *node, s32 arg2) {
    s32 index = modelFindNodeMtxIndex(node, arg2);

    if (index >= 0) {
        return &model->render_pos[index].pos;
    }

    return NULL;
}


//rejoined per EU
// PD: model0001a60c
Mtxf *getsubmatrix(Model *objinst)
{
    #if defined(LEFTOVERDEBUG)
    if (!objinst)
    {
        osSyncPrintf("getsubmatrix: no objinst!\n");
        return_null();
    }
    if (!objinst->obj)
    {
        osSyncPrintf("getsubmatrix: objinst has no object!\n");
        return_null();
    }
    #endif
    return modelFindNodeMtx(objinst, objinst->obj->RootNode, 0);
}


void sub_GAME_7F06C710(Model* model, coord3d* pos)
{
    Mtxf* mtx;

    mtx = getsubmatrix(model);
    if (mtx != NULL)
    {
        pos->f[0] = (f32) mtx->m[3][0];
        pos->f[1] = (f32) mtx->m[3][1];
        pos->f[2] = (f32) mtx->m[3][2];
        return;
    }

    pos->f[0] = 0.0f;
    pos->f[1] = 0.0f;
    pos->f[2] = 0.0f;
}


f32 sub_GAME_7F06C768(Model *objinst)
{
    Mtxf *mtx = getsubmatrix(objinst);
    if (mtx != 0)
    {
        return -mtx->m[3][2];
    }
    return 0.0f;
}


/**
 * Address 0x7F06C79C.
*/
union ModelRwData* modelGetNodeRwData(Model *Objinst, ModelNode *root)
{
    s32 index  = 0;
    union ModelRwData **data = Objinst->datas;

    switch (root->Opcode & 0xff)
    {
        case MODELNODE_OPCODE_HEADER:
        {
            index = root->Data->Header.RwDataIndex;
            break;
        }
        case MODELNODE_OPCODE_DLCOLLISION:
        {
            index = root->Data->DisplayListCollisions.RwDataIndex;
            break;
        }
        case MODELNODE_OPCODE_OP07:
        {
            index = root->Data->Op07.RwDataIndex;
            break;
        }
        case MODELNODE_OPCODE_LOD:
        {
            index = root->Data->LOD.RwDataIndex;
            break;
        }
        case MODELNODE_OPCODE_SWITCH:
        {
            index = root->Data->Switch.RwDataIndex;
            break;
        }
        case MODELNODE_OPCODE_BSP:
        {
            index = root->Data->BSP.RwDataIndex;
            break;
        }
        case MODELNODE_OPCODE_OP11:
        {
            index = root->Data->Op11.RwDataIndex;
            break;
        }
        case MODELNODE_OPCODE_GUNFIRE:
        {
            index = root->Data->Gunfire.RwDataIndex;
            break;
        }
        case MODELNODE_OPCODE_HEAD:
        {
            index = root->Data->HeadPlaceholder.RwDataIndex;
            break;
        }
    }

    while (root->Parent)
    {
        root = root->Parent;
        if ((root->Opcode & 0xFF) == MODELNODE_OPCODE_HEAD)
        {
            ModelRwData_HeadPlaceholderRecord *tmp = modelGetNodeRwData(Objinst, root);
            data = tmp->RwDatas;
            break;
        }
    }

    return &data[index];
}



void getpartoffset(Model *objinst, ModelNode *part, coord3d *offset) //#MATCH - however OPCODE 3 needs defining
{
    #if defined(LEFTOVERDEBUG)
    if (!objinst)
    {
        osSyncPrintf("getpartoffset: no objinst!");
        return_null();
    }
    if (!part)
    {
        osSyncPrintf("getpartoffset: no partdesc!");
        return_null();
    }
    #endif
    switch (part->Opcode & 0xFF)
    {
        case MODELNODE_OPCODE_HEADER:
        {
            struct modeldata_root *root = modelGetNodeRwData(objinst, part);
            offset->x                   = root->pos.x;
            offset->y                   = root->pos.y;
            offset->z                   = root->pos.z;
            break;
        }
        case MODELNODE_OPCODE_GROUP:
        {
            ModelRoData_GroupRecord *prt = &part->Data->Group;
            offset->x                  = prt->Origin.x;
            offset->y                  = prt->Origin.y;
            offset->z                  = prt->Origin.z;
            break;
        }
        case MODELNODE_OPCODE_OP03:
        {
            ModelRoData_GroupSimpleRecord *prt = &part->Data->GroupSimple; //UNUSED at this time
            offset->x                        = prt->Origin.x;
            offset->y                        = prt->Origin.y;
            offset->z                        = prt->Origin.z;
            break;
        }
        case MODELNODE_OPCODE_GROUPSIMPLE:
        {
            ModelRoData_GroupSimpleRecord *prt = &part->Data->GroupSimple;
            offset->x                        = prt->Origin.x;
            offset->y                        = prt->Origin.y;
            offset->z                        = prt->Origin.z;
            break;
        }
        default:
        {
            offset->x = 0.0f;
            offset->y = 0.0f;
            offset->z = 0.0f;
            break;
        }
    }
}


void setpartoffset(Model *model, ModelNode *node, coord3d *pos)
{
#if defined(LEFTOVERDEBUG)
    if (!model) {
        osSyncPrintf("setpartoffset: no objinst!");
        return_null();
    }

    if (!node) {
        osSyncPrintf("setpartoffset: no partdesc!");
        return_null();
    }
    else
    {
        // huh?
    }
#endif
    switch (node->Opcode & 0xff)
    {
        case MODELNODE_OPCODE_HEADER:
            {
                ModelRwData_HeaderRecord *rwdata = modelGetNodeRwData(model, node);
                coord3d diff[1];

                diff[0].x = pos->x - rwdata->pos.x;
                diff[0].z = pos->z - rwdata->pos.z;

                rwdata->pos.x = pos->x;
                rwdata->pos.y = pos->y;
                rwdata->pos.z = pos->z;

                rwdata->unk24.x += diff[0].x; rwdata->unk24.z += diff[0].z;
                rwdata->unk34.x += diff[0].x; rwdata->unk34.z += diff[0].z;
                rwdata->unk40.x += diff[0].x; rwdata->unk40.z += diff[0].z;
                rwdata->unk4c.x += diff[0].x; rwdata->unk4c.z += diff[0].z;
            }
            break;
        case MODELNODE_OPCODE_GROUP:
            {
                ModelRoData_GroupRecord *rodata = &node->Data->Group;
                rodata->Origin.x = pos->x;
                rodata->Origin.y = pos->y;
                rodata->Origin.z = pos->z;
            }
            break;
        case MODELNODE_OPCODE_OP03:
            {
                ModelRoData_GroupRecord *rodata = &node->Data->Group;
                rodata->Origin.x = pos->x;
                rodata->Origin.y = pos->y;
                rodata->Origin.z = pos->z;
            }
            break;
        case MODELNODE_OPCODE_GROUPSIMPLE:
            {
                ModelRoData_GroupSimpleRecord *rodata = &node->Data->GroupSimple;
                rodata->Origin.x = pos->x;
                rodata->Origin.y = pos->y;
                rodata->Origin.z = pos->z;
            }
            break;
    }
}


void getsuboffset(Model *objinst, coord3d *offset) //#MATCH
{
    #if defined(LEFTOVERDEBUG )
    if (!objinst)
    {
        osSyncPrintf("getsuboffset: no objinst!");
        return_null();
    }

    if (!objinst->obj)
    {
        osSyncPrintf("getsuboffset: objinst has no object!");
        return_null();
    }
    #endif
    getpartoffset(objinst, objinst->obj->RootNode, offset);
}




void setsuboffset(Model *objinst, coord3d *offset) //#MATCH
{
    #if defined(LEFTOVERDEBUG )
    if (!objinst)
    {
        osSyncPrintf("setsuboffset: no objinst!");
        return_null();
    }
    if (!objinst->obj)
    {
        osSyncPrintf("setsuboffset: objinst has no object!");
        return_null();
    }
    #endif
    setpartoffset(objinst, objinst->obj->RootNode, offset);
}





/**
 * Address 0x7F06CC80.
 */
f32 getsubroty(Model *objinst)
{
    ModelNode *root;

    #if defined(LEFTOVERDEBUG)
    if(0)
    {
        // removed
    }

    if (objinst == NULL)
    {
        osSyncPrintf("getsubroty: no objinst!");
        return_null();
    }

    if(0)
    {
        // removed
    }

    if (objinst->obj == NULL)
    {
        osSyncPrintf("getsubroty: objinst has no object!");
        return_null();
    }

    if(0)
    {
        // removed
    }

    if (objinst->obj->RootNode == NULL)
    {
        osSyncPrintf("getsubroty: objinst has no root part!");
        return_null();
    }

    if(0)
    {
        // removed
    }
    #endif

    root = objinst->obj->RootNode;
    if ((root->Opcode & 0xFF) == 1)
    {
        return ((struct modeldata_root *)modelGetNodeRwData(objinst, root))->subroty;
    }

    return 0.0f;
}


void setsubroty(Model *model, f32 angle)
{
    ModelNode* node;
#if defined(LEFTOVERDEBUG)
    if (!model)
    {
        osSyncPrintf("setsubroty: no objinst!");
        return_null();
    }

    if (!model->obj) //< needs to be v1 not a1
    {
        osSyncPrintf("setsubroty: objinst has no object!");
        return_null();
    }

    if (!model->obj->RootNode)
    {
        osSyncPrintf("setsubroty: objinst has no root part!");
        return_null();
    }
#endif
    node = model->obj->RootNode;
    if ((node->Opcode & 0xff) == MODELNODE_OPCODE_HEADER)
    {
        ModelRwData_HeaderRecord *rwdata = modelGetNodeRwData(model, node);
        f32 diff = angle - rwdata->unk14;

        if (diff < 0) { diff += M_TAU_F; }

        rwdata->unk30 += diff;

        if (rwdata->unk30 >= M_TAU_F) { rwdata->unk30 -= M_TAU_F; }

        rwdata->unk20 += diff;

        if (rwdata->unk20 >= M_TAU_F) { rwdata->unk20 -= M_TAU_F; }

        rwdata->unk14 = angle;
    }
}


void modelSetScale(Model *objinst, f32 scale)
{
    objinst->scale = scale;
}


void sub_GAME_7F06CE84(Model* self, f32 arg1)
{
    self->unkb8 = arg1;
}


f32 getjointsize(Model *model, ModelNode *node)
{
    Model     *temp_a2;
    ModelNode *temp_a1;
    s32        temp_t7;

#if defined(LEFTOVERDEBUG)
    if (!model)
    {
        osSyncPrintf("getjointsize: no objinst!\n");
        return_null();
    }
#endif

    if (node)
    {
        do
        {
            switch (node->Opcode & 0xFF)
            {
                case MODELNODE_OPCODE_HEADER:
                {
                    ModelRoData_HeaderRecord *rodata = &node->Data->Header;
                    return rodata->GroupsAsF32 * model->scale;
                }
                case MODELNODE_OPCODE_GROUP:
                {
                    ModelRoData_GroupRecord *rodata = &node->Data->Group;
                    return rodata->BoundingVolumeRadius * model->scale;
                }
                case MODELNODE_OPCODE_OP03:
                {
                    ModelRoData_GroupRecord *rodata = &node->Data->Group;
                    return rodata->BoundingVolumeRadius * model->scale;
                }
                case MODELNODE_OPCODE_GROUPSIMPLE:
                {
                    ModelRoData_GroupSimpleRecord *rodata = &node->Data->GroupSimple;
                    return rodata->BoundingVolumeRadius * model->scale;
                }
                case MODELNODE_OPCODE_OP11:
                {
                    ModelRoData_Op11Record *rodata = &node->Data->Op11;
                    return rodata->BoundingVolumeRadius * model->scale;
                }
                case MODELNODE_OPCODE_GUNFIRE:
                {
                    ModelRoData_GunfireRecord *rodata = &node->Data->Gunfire;
                    return rodata->Scale * model->scale;
                }
                case MODELNODE_OPCODE_SHADOW:
                {
                    ModelRoData_ShadowRecord *rodata = &node->Data->Shadow;
                    return rodata->Scale * model->scale;
                }
                case MODELNODE_OPCODE_OP14:
                {
                    ModelRoData_Op14Record *rodata = &node->Data->Op14;
                    return rodata->Scale * model->scale;
                }
                case MODELNODE_OPCODE_INTERLINK:
                {
                    ModelRoData_InterlinkageRecord *rodata = &node->Data->Interlinkage;
                    return rodata->Scale * model->scale;
                }
                case MODELNODE_OPCODE_OP16:
                {
                    ModelNode_Op16Record *rodata = &node->Data->Op16;
                    return rodata->Scale * model->scale;
                }
                default:
                    node = node->Parent;
            }
        } while (node);
    }

    return 0.0f;
}


/**
 * Address 0x7F06D00C.
 * PD: model0001af80
*/
f32 getinstsize(Model *arg0)
{
    #if defined(LEFTOVERDEBUG)
    if (arg0 == NULL)
    {
        osSyncPrintf("getinstsize: no objinst!\n");
        return_null();
    }

    if (arg0->obj == NULL)
    {
        osSyncPrintf("getinstsize: no objdesc!\n");
        return_null();
    }
    #endif

    return arg0->obj->BoundingVolumeRadius * arg0->scale;
}



// PD: model0001af98
void interpolate3dVectors(vec3d *v, vec3d *w, float frac)
{
    v->x += (w->x - v->x) * frac;
    v->y += (w->y - v->y) * frac;
    v->z += (w->z - v->z) * frac;
  return;
}


// PD: model0001afe8
f32 sub_GAME_7F06D0CC(f32 arg0, f32 angle, f32 mult)
{
    f32 value = angle - arg0;

    if (angle < arg0)
    {
        value += M_TAU_F;
    }

    if (value < M_PI_F)
    {
        arg0 += value * mult;

        if (arg0 >= M_TAU_F)
        {
            arg0 -= M_TAU_F;
        }
    }
    else
    {
        arg0 -= (M_TAU_F - value) * mult;

        if (arg0 < 0)
        {
            arg0 += M_TAU_F;
        }
    }

    return arg0;
}


// PD: model0001b07c
void sub_GAME_7F06D160(coord3d *arg0, coord3d *arg1, f32 mult)
{
    arg0->x = sub_GAME_7F06D0CC(arg0->x, arg1->x, mult);
    arg0->y = sub_GAME_7F06D0CC(arg0->y, arg1->y, mult);
    arg0->z = sub_GAME_7F06D0CC(arg0->z, arg1->z, mult);
}


#ifdef NONMATCHING
void sub_GAME_7F06D1CC(void) {

}
#else

void _asmpp_func5(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
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
void sub_GAME_7F06D2E4(void) {

}
#else

void _asmpp_func6(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
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
void sub_GAME_7F06D3F4(void) {

}
#else

void _asmpp_func7(void) {





*(volatile float*)4 = -4.214055994530569e+19f;


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

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
void sub_GAME_7F06D490(void) {

}
#else

void _asmpp_func8(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func9(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
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








void subcalcpos(Model *arg0)
{
    struct ModelNode *root;

#if defined(LEFTOVERDEBUG)
    if (arg0 == NULL)
    {
        osSyncPrintf("subcalcpos: no objanim!\n");
        return_null();
    }

    if (arg0->obj == 0)
    {
        osSyncPrintf("subcalcpos: no objdesc!\n");
        return_null();
    }
#endif

    root = arg0->obj->RootNode;
    if ((root != NULL) && ((root->Opcode & 0xFF) == 1))
    {
        sub_GAME_7F06D490(arg0, root);
    }
}

void process_01_group_heading(ModelRenderData* renderdata, Model* model, ModelNode* node)
{
    union ModelRoData* rodata;
    union ModelRwData* rwdata;
    f32 scale;
    f32* pos;
    f32 unk14;
    Mtxf* var_a3;
    s32 modeltype;
    RenderPosView* renderpos;
    Mtxf sp20;

    rodata = node->Data;
    rwdata = modelGetNodeRwData(model, node);

    scale = model->scale;
    pos = &rwdata->Header.pos.x;
    unk14 = rwdata->Header.unk14;
    modeltype = (s16)rodata->Header.ModelType;
    renderpos = &model->render_pos[modeltype];

    if (node->Parent != NULL)
    {
        var_a3 = modelFindNodeMtx(model, node->Parent, 0);
    }
    else
    {
        var_a3 = renderdata->unk_matrix;
    }

    if (rwdata->Header.unk18 != 0.0f)
    {
        unk14 = sub_GAME_7F06D0CC(unk14, rwdata->Header.unk1c, rwdata->Header.unk18);
    }

    if (var_a3 != NULL)
    {
        matrix_4x4_set_position_and_rotation_around_y(pos, unk14, &sp20);

        if (scale != 1.0f)
        {
            matrix_scalar_multiply_2(scale, sp20.m[0]);
        }

        matrix_4x4_multiply_homogeneous(var_a3, &sp20, &renderpos->pos);
        return;
    }

    matrix_4x4_set_position_and_rotation_around_y(pos, unk14, &renderpos->pos);

    if (scale != 1.0f)
    {
        matrix_scalar_multiply_2(scale, renderpos->pos.m[0]);
    }
}


#ifdef NONMATCHING
void sub_GAME_7F06D8B0(void) {

}
#else

void _asmpp_func10(void) {















*(volatile float*)4 = -4.21405643433522e+19f;


switch (*(volatile int*)4) { case 0: case 1: case 2: case 3: case 4: ; }








*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
 } void _asmpp_large_func11(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
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
void sub_GAME_7F06DB5C(void) {

}
#else

void _asmpp_func12(void) {















*(volatile float*)4 = -4.214056874139871e+19f;


switch (*(volatile int*)4) { case 0: case 1: case 2: case 3: case 4: ; }








*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
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
}
#endif





#ifdef NONMATCHING
void sub_GAME_7F06DE04(void) {

}
#else

void _asmpp_func14(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
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
void sub_GAME_7F06DEC0(void) {

}
#else

void _asmpp_func15(void) {





*(volatile float*)4 = -4.214057313944522e+19f;


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
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
}
#endif





#ifdef NONMATCHING
void process_02_position(ModelRenderData *arg0, Model *model, ModelNode *node) {

}
#else
void process_02_position(ModelRenderData *arg0, Model *model, ModelNode *node);

void _asmpp_func17(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
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
}
#endif





#ifdef NONMATCHING
void sub_GAME_7F06E2B8(void) {

}
#else

void _asmpp_func19(void) {















*(volatile float*)4 = -4.214057753749173e+19f;


switch (*(volatile int*)4) { case 0: case 1: case 2: case 3: case 4: ; }








*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
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
}
#endif





#ifdef NONMATCHING
void sub_GAME_7F06E540(void) {

}
#else

void _asmpp_func21(void) {







*(volatile float*)4 = -4.214058193553824e+19f;


*(volatile float*)4 = -4.214058633358475e+19f;


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
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
void process_03_unknown(ModelRenderData *arg0, Model *model, ModelNode *node) {

}
#else
void process_03_unknown(ModelRenderData *arg0, Model *model, ModelNode *node);

void _asmpp_func22(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
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


void process_15_subposition(ModelRenderData* arg0, Model *model, ModelNode *node)
{
    union ModelRoData *rodata = node->Data;
    Mtxf *sp68;
    Mtxf sp28;
    s32 mtxindex = rodata->GroupSimple.Group1;
    RenderPosView *matrices = model->render_pos;

    if (node->Parent)
    {
        sp68 = modelFindNodeMtx(model, node->Parent, 0);
    }
    else
    {
        sp68 = arg0->unk_matrix;
    }

    if (sp68)
    {
        matrix_4x4_set_identity_and_position(&rodata->GroupSimple.Origin, &sp28);
        matrix_4x4_multiply_homogeneous(sp68, &sp28, &matrices[mtxindex]);
    }
    else
    {
        matrix_4x4_set_identity_and_position(&rodata->GroupSimple.Origin, &matrices[mtxindex]);
    }
}

/*
* Address: 0x7F06E858
*/
void modelUpdateDistanceRelations(Model* model, ModelNode* node)
{
    union ModelRoData *rodata = node->Data;
    union ModelRwData *rwdata = modelGetNodeRwData(model, node);
    Mtxf *mtx = modelFindNodeMtx(model, node, 0);
    f32 distance;

    if (g_ModelDistanceDisabled)
    {
        distance = 0;
    }
    else
    {
        distance = -mtx->m[3][2] * getPlayer_c_lodscalez();

        if (g_ModelDistanceScale != 1)
        {
            distance *= g_ModelDistanceScale;
        }
    }

    if (distance > rodata->LOD.MinDistance * model->scale || rodata->LOD.MinDistance == 0)
    {
        if (distance <= rodata->LOD.MaxDistance * model->scale)
        {
            rwdata->LOD.visible = TRUE;
            node->Child = rodata->LOD.Affects;
            return;
        }
    }

    rwdata->LOD.visible = FALSE;
    node->Child = NULL;
}

/*
* Address: 0x7F06E970
*/
void modelApplyDistanceRelations(Model* model, ModelNode* node)
{
    ModelRoData_LODRecord *rodata = &node->Data->LOD;
    ModelRwData_LODRecord *rwdata = modelGetNodeRwData(model, node);

    if (rwdata->visible)
    {
        node->Child = rodata->Affects;
    }
    else
    {
        node->Child = NULL;
    }
}


void modelApplyToggleRelations(Model* model, ModelNode* node)
{
    ModelRoData_SwitchRecord *rodata = &node->Data->Switch;
    ModelRwData_SwitchRecord *rwdata = modelGetNodeRwData(model, node);

    if (rwdata->visible)
    {
        node->Child = rodata->Controls;
    }
    else
    {
        node->Child = NULL;
    }
}


void modelApplyHeadRelations(Model* model, ModelNode* bodynode)
{
    struct ModelRwData_HeadPlaceholderRecord *rwdata = modelGetNodeRwData(model, bodynode);

    if (rwdata->ModelFileHeader)
    {
        ModelNode *headnode = rwdata->ModelFileHeader->RootNode;

        bodynode->Child = headnode;

        while (headnode)
        {
            headnode->Parent = bodynode;
            headnode = headnode->Next;
        }
    }
}


void modelApplyReorderRelationsByArg(ModelNode *basenode, bool visible)
{
    union ModelRoData *rodata = basenode->Data;
    ModelNode *node1;
    ModelNode *node2;
    ModelNode *loopnode;

    if (visible)
    {
        node1 = rodata->BSP.leftChild;
        node2 = rodata->BSP.rightChild;
    }
    else
    {
        node1 = rodata->BSP.rightChild;
        node2 = rodata->BSP.leftChild;
    }

    if (node1)
    {
        // I think what's happening here is there's two groups of siblings,
        // where node1 and node2 are the head nodes. Either group can be first,
        // and this is ensuring the node1 group is first.
        // Note that node2 might be NULL.

        basenode->Child = node1;
        node1->Prev = NULL;

        // Skip through node1's siblings until node2 is found or the end is
        // reached
        loopnode = node1;

        while (loopnode->Next && loopnode->Next != node2)
        {
            loopnode = loopnode->Next;
        }

        loopnode->Next = node2;

        if (node2)
        {
            // Append node2 and its siblings to node1's siblings
            node2->Prev = loopnode;
            loopnode = node2;

            while (loopnode->Next && loopnode->Next != node1)
            {
                loopnode = loopnode->Next;
            }

            loopnode->Next = NULL;
        }
    }
    else
    {
        basenode->Child = node2;

        if (node2)
        {
            node2->Prev = NULL;
        }
    }
}


void modelApplyReorderRelations(Model* model, ModelNode* node)
{
    union ModelRwData *rwdata = modelGetNodeRwData(model, node);
    modelApplyReorderRelationsByArg(node, rwdata->BSP.visible);
}


void modelUpdateReorderRelations(Model *model, ModelNode *node)
{
    union ModelRoData *rodata = node->Data;
    union ModelRwData *rwdata = modelGetNodeRwData(model, node);
    Mtxf *mtx = modelFindNodeMtx(model, node, 0);
    coord3d sp38;
    coord3d sp2c;
    f32 tmp;

    if (rodata->BSP.reserved == 0)
    {
        sp38.x = rodata->BSP.Vector.f[0];
        sp38.y = rodata->BSP.Vector.f[1];
        sp38.z = rodata->BSP.Vector.f[2];
        mtx4RotateVecInPlace(mtx, sp38.f);
    }
    else if (rodata->BSP.reserved == 2)
    {
        sp38.x = mtx->m[1][0] * rodata->BSP.Vector.f[1];
        sp38.y = mtx->m[1][1] * rodata->BSP.Vector.f[1];
        sp38.z = mtx->m[1][2] * rodata->BSP.Vector.f[1];
    }
    else if (rodata->BSP.reserved == 3)
    {
        sp38.x = mtx->m[2][0] * rodata->BSP.Vector.f[2];
        sp38.y = mtx->m[2][1] * rodata->BSP.Vector.f[2];
        sp38.z = mtx->m[2][2] * rodata->BSP.Vector.f[2];
    }
    else if (rodata->BSP.reserved == 1)
    {
        sp38.x = mtx->m[0][0] * rodata->BSP.Vector.f[0];
        sp38.y = mtx->m[0][1] * rodata->BSP.Vector.f[0];
        sp38.z = mtx->m[0][2] * rodata->BSP.Vector.f[0];
    }

    sp2c.x = rodata->BSP.Point.f[0];
    sp2c.y = rodata->BSP.Point.f[1];
    sp2c.z = rodata->BSP.Point.f[2];

    mtx4TransformVecInPlace(mtx, &sp2c);

    tmp = sp38.f[0] * sp2c.f[0] + sp38.f[1] * sp2c.f[1] + sp38.f[2] * sp2c.f[2];

    if (tmp < 0)
    {
        rwdata->BSP.visible = TRUE;
    }
    else
    {
        rwdata->BSP.visible = FALSE;
    }

    modelApplyReorderRelations(model, node);
}


void process_07_unknown(Model *model, ModelNode *node)
{
    union ModelRoData *rodata = node->Data;
    union ModelRwData *rwdata = modelGetNodeRwData(model, node);
    Mtxf *mtx = modelFindNodeMtx(model, node, 0);
    f32 ratio;
    f32 coord_multiplied;
    coord3d coord;
    s32 index1;
    f32 theta;
    s32 index2;
    s32 index3;

    sub_GAME_7F06C550(model, &coord);

    theta = acosf(((coord.x * mtx->m[1][0]) + (coord.y * mtx->m[1][1])) + (coord.z * mtx->m[1][2]));
    ratio = acosf((((coord.x * mtx->m[2][0]) + (coord.y * mtx->m[2][1])) + (coord.z * mtx->m[2][2])) / sinf(theta));
    coord_multiplied = ((coord.x * mtx->m[0][0]) + (coord.y * mtx->m[0][1])) + (coord.z * mtx->m[0][2]);

    if ((coord_multiplied < 0.0f) && (ratio > 0.0f))
    {
        ratio = M_TAU_F - ratio;
    }

    index1 = (theta * 64.0f) / M_TAU_F;

    index2 = (s32) ((ratio * M_U16_MAX_VALUE_F) / M_TAU_F);
    index2 += D_800360C4[index1].unk04;
    index2 = index2 >> D_800360C4[index1].unk0C;

    index3 = index2 + D_800360C4[index1].unk00;

    rwdata->Op07.index = rodata->Op07.Data[index3];
}


void modelUpdateRelationsQuick(Model *model, ModelNode *parent)
{
    ModelNode *node = parent->Child;
    ModelNode **unused_parent;

    while (node)
    {
        s32 type = node->Opcode & 0xff;
        bool dochildren = TRUE;

        switch (type)
        {
            case MODELNODE_OPCODE_HEADER:
            case MODELNODE_OPCODE_GROUP:
            case MODELNODE_OPCODE_OP03:
            case MODELNODE_OPCODE_OP11:
            case MODELNODE_OPCODE_GUNFIRE:
            case MODELNODE_OPCODE_SHADOW:
            case MODELNODE_OPCODE_OP14:
            case MODELNODE_OPCODE_INTERLINK:
            case MODELNODE_OPCODE_OP16:
            case MODELNODE_OPCODE_GROUPSIMPLE:
                dochildren = FALSE;
                break;
            case MODELNODE_OPCODE_LOD:
                modelUpdateDistanceRelations(model, node);
                break;
            case MODELNODE_OPCODE_BSP:
                modelUpdateReorderRelations(model, node);
                break;
            case MODELNODE_OPCODE_OP07:
                process_07_unknown(model, node);
                break;
            case MODELNODE_OPCODE_HEAD:
                modelApplyHeadRelations(model, node);
                break;
            case MODELNODE_OPCODE_DLCOLLISION:
                break;
        }

        if (dochildren && node->Child)
        {
            node = node->Child;
        }
        else
        {
            unused_parent = &parent;
            while (node)
            {
                if (node == parent->Parent)
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
}


void sub_GAME_7F06EFC4(Model *model)
{
    ModelNode *node = model->obj->RootNode;

    while (node)
    {
        u32 type = node->Opcode & 0xff;

        switch (type)
        {
            case MODELNODE_OPCODE_LOD:
                modelUpdateDistanceRelations(model, node);
                break;

            case MODELNODE_OPCODE_BSP:
                modelUpdateReorderRelations(model, node);
                break;

            case MODELNODE_OPCODE_OP07:
                process_07_unknown(model, node);
                break;

            case MODELNODE_OPCODE_SWITCH:
                modelApplyToggleRelations(model, node);
                break;

            case MODELNODE_OPCODE_HEAD:
                modelApplyHeadRelations(model, node);
                break;

            case MODELNODE_OPCODE_HEADER:
            case MODELNODE_OPCODE_DLCOLLISION:
            default:
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


void modelUpdateMatrices(ModelRenderData *arg0, Model *model)
{
    ModelNode *node = model->obj->RootNode;

    while (node)
    {
        u32 type = node->Opcode & 0xff;

        switch (type)
        {
            case MODELNODE_OPCODE_HEADER:
                process_01_group_heading(arg0, model, node);
                break;

            case MODELNODE_OPCODE_GROUP:
                process_02_position(arg0, model, node);
                break;

            case MODELNODE_OPCODE_OP03:
                process_03_unknown(arg0, model, node);
                break;

            case MODELNODE_OPCODE_GROUPSIMPLE:
                process_15_subposition(arg0, model, node);
                break;

            case MODELNODE_OPCODE_LOD:
                modelUpdateDistanceRelations(model, node);
                break;

            case MODELNODE_OPCODE_BSP:
                modelUpdateReorderRelations(model, node);
                break;

            case MODELNODE_OPCODE_OP07:
                process_07_unknown(model, node);
                break;

            case MODELNODE_OPCODE_SWITCH:
                modelApplyToggleRelations(model, node);
                break;

            case MODELNODE_OPCODE_HEAD:
                modelApplyHeadRelations(model, node);
                break;

            case MODELNODE_OPCODE_DLCOLLISION:
            default:
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


void instcalcmatrices(ModelRenderData* arg0, Model* arg1)
{
#if defined(LEFTOVERDEBUG)
    if (arg1 == NULL)
    {
        osSyncPrintf("instcalcmatrices: no objinst!\n");
        return_null();
    }

    if (arg0->unk_matrix == NULL)
    {
        osSyncPrintf("instcalcmatrices: no basemtx!\n");
        return_null();
    }

    if (arg0->mtxlist == NULL)
    {
        osSyncPrintf("instcalcmatrices: no mtxlist!\n");
        return_null();
    }
#endif
    arg1->render_pos = (RenderPosView* ) arg0->mtxlist;
    arg0->mtxlist += arg1->obj->numMatrices;
    modelUpdateMatrices((ModelRenderData* ) arg0, arg1);
}


/**
 * Address 0x7F06F2F8 (VERSION_US, VERSION_JP)
 * Address 0x7F06F670 (VERSION_EU)
*/
void subcalcmatrices(ModelRenderData *arg0, struct Model *arg1)
{
#if defined(LEFTOVERDEBUG)
    if (arg1 == NULL)
    {
        osSyncPrintf("subcalcmatrices: no objanim!\n");
        return_null();
    }

    if (arg0->unk_matrix == NULL)
    {
        osSyncPrintf("subcalcmatrices: no basemtx!\n");
        return_null();
    }

    if (arg0->mtxlist == NULL)
    {
        osSyncPrintf("subcalcmatrices: no mtxlist!\n");
        return_null();
    }
#endif

    if (arg1->anim != NULL)
    {
#if defined(LEFTOVERDEBUG)
        if ((arg1->attachedto != NULL) && (arg1->attachedto_objinst == NULL))
        {
            osSyncPrintf("subcalcmatrices: no attach for objinst!\n");
            return_null();
        }

        if (((s32) arg1->framea < 0) || ((s32) arg1->framea >= (s32) arg1->anim->unk04))
        {
            osSyncPrintf("subcalcmatrices: framea out of range!\n");
            return_null();
        }

        if (((s32) arg1->frameb < 0) || ((s32) arg1->frameb >= (s32) arg1->anim->unk04))
        {
            osSyncPrintf("subcalcmatrices: frameb out of range!\n");
            return_null();
        }

        if ((arg1->unk84 == 0) || ((arg1->unk84 != 0) && (arg1->anim2 != NULL)))
        {
            //
        }
        else
        {
            osSyncPrintf("subcalcmatrices: no anim2!\n");
            return_null();
        }

        if (
            (arg1->anim2 != NULL)
            && (
                (arg1->anim2 == NULL)
                || (arg1->frame2a < 0)
                || ((s32) arg1->frame2a >= (s32) arg1->anim2->unk04)
                )
            )
        {
            osSyncPrintf("subcalcmatrices: frame2a out of range!\n");
            return_null();
        }

        if (
            (arg1->anim2 == NULL)
            || (
                (arg1->anim2 != NULL)
                 && (arg1->frame2b >= 0)
                 && ((s32) arg1->frame2b < (s32) arg1->anim2->unk04)
                )
            )
        {
            //
        }
        else
        {
            osSyncPrintf("subcalcmatrices: frame2b out of range!\n");
            return_null();
        }
#endif

        arg1->unk34 = loadAnimationFrame(arg1->anim, arg1->framea, arg1->obj->Skeleton);

        if (arg1->unk2c != 0.0f)
        {
            arg1->unk38 = loadAnimationFrame(arg1->anim, arg1->frameb, arg1->obj->Skeleton);
        }

        if (arg1->anim2 != NULL)
        {
            arg1->unk64 = loadAnimationFrame(arg1->anim2, arg1->frame2a, arg1->obj->Skeleton);

            if (arg1->unk5c != 0.0f)
            {
                arg1->unk68 = loadAnimationFrame(arg1->anim2, arg1->frame2b, arg1->obj->Skeleton);
            }
        }

        sub_GAME_7F0755B0();
    }

    instcalcmatrices(arg0, arg1);
}

/**
 * Address 0x7F06F5AC.
*/
struct ModelAnimation * objecthandlerGetModelAnim(struct Model* model) {
    return model->anim;
}

s8 objecthandlerGetModelGunhand(Model *model) {
    return model->gunhand;
}

/**
 * Address 0x7F06F5BC.
*/
f32 objecthandlerGetModelField28(Model *model)
{
    return model->unk28;
}

f32 sub_GAME_7F06F5C4(Model *model)
{
    f32 unk3c;
    ModelAnimation *modelAnimation;

    unk3c = model->endframe;
    if (unk3c >= 0.0f)
    {
        return unk3c;
    }

    modelAnimation = model->anim;

    if (modelAnimation != NULL)
    {
        return modelAnimation->unk04 - 1;
    }
    return 0.0f;
}


f32 modelGetAnimSpeed(Model *model)
{
    return model->speed;
}


/**
 * Address 0x7F06F618.
 * PD: modelGetAbsAnimSpeed
*/
f32 modelGetAbsAnimSpeed(Model *model)
{
    f32 speed;

    speed = model->speed;

    if (speed < 0.0f)
    {
        speed = -speed;
    }

    return speed;
}

/**
 * Unused Function
*/
f32 modelGetEffectiveAnimSpeed(Model *model) {
    return modelGetAnimSpeed(model) * model->playspeed;
}


#ifdef NONMATCHING
s32 modelConstrainOrWrapAnimFrame(s32 frame, ModelAnimation *anim, f32 endframe)
{
    //s32 frame;
    //u16 animnum->unk4;
    //u16 animnum->unk4;

    //frame = frame;
    if (frame < 0)
    {
        if (animnum->unk7 & 1)
        {
            //animnum->unk4 = animnum->unk4;
            frame = animnum->unk4 - ((s32) -frame % (s32) animnum->unk4);
        }
        else
        {
            frame = 0;
        }
    }
    else if ((endframe >= 0.0f) && ((s32) endframe < frame))
    {
        frame = ceilFloatToInt(endframe);
    }
    else
    {
        //animnum->unk4 = animnum->unk4;
        if (frame >= (s32) animnum->unk4)
        {
            if (animnum->unk7 & 1)
            {
                frame = frame % (s32) animnum->unk4;
            }
            else
            {
                frame = animnum->unk4 - 1;
            }
        }
    }
    return frame;
}
#else
s32 modelConstrainOrWrapAnimFrame(s32 frame, ModelAnimation *anim, f32 endframe);

void _asmpp_func23(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

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
void modelCopyAnimForMerge(Model *model, f32 arg1)
{
    ModelNode      *temp_a1;
    f32             temp_f10;
    f32             temp_f16;
    f32             temp_f18;
    f32             temp_f4;
    f32             temp_f6;
    f32             temp_f8;
    modeldata_root *temp_v0_2;

    if ((arg1 > 0.0f) && (model->anim != 0))
    {
        temp_a1      = model->obj->RootNode;
        model->unk58 = model->unk28;
        model->unk5C = model->unk2C;
        model->unk54 = model->anim;
        model->unk25 = model->unk24;
        model->unk60 = model->unk30;
        model->unk62 = model->unk32;
        model->unk70 = model->speed;
        model->unk74 = model->newspeed;
        model->unk78 = model->oldspeed;
        model->unk7C = model->timespeed;
        model->unk80 = model->elapsespeed;
        model->unk6C = model->endframe;
        if ((temp_a1->Opcode & 0xFF) == 1)
        {
            temp_v0_2                   = modelGetNodeRwData(model, temp_a1);
            temp_f10                    = temp_v0_2->unk34.x;
            temp_f16                    = temp_v0_2->unk34.AsArray[1];
            temp_f18                    = temp_v0_2->unk34.AsArray[2];
            temp_f4                     = temp_v0_2->unk24.x;
            temp_f6                     = temp_v0_2->unk24.AsArray[1];
            temp_f8                     = temp_v0_2->unk24.AsArray[2];
            temp_v0_2->unk02            = 1;
            temp_v0_2->unk4c.x          = temp_f10;
            temp_v0_2->unk4c.AsArray[1] = temp_f16;
            temp_v0_2->unk4c.AsArray[2] = temp_f18;
            temp_v0_2->unk40.x          = temp_f4;
            temp_v0_2->unk40.AsArray[1] = temp_f6;
            temp_v0_2->unk40.AsArray[2] = temp_f8;
            return;
        }
        // Duplicate return node #5. Try simplifying control flow for better match
        return;
    }
    model->unk54 = NULL;
}
#else

void _asmpp_func24(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
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
void modelSetAnimation2(Model *model, void *anim, s32 arg2, f32 startframe, f32 half, f32 arg5)
{
    void           *sp80;
    f32             sp70;
    f32             sp6C;
    f32             sp64;
    f32             sp60;
    f32             sp5C;
    f32             sp58;
    f32             sp54;
    f32             sp50;
    f32             sp4C;
    f32             sp48;
    f32             sp44;
    f32             sp40;
    f32             sp30;
    s32             sp2C;
    ModelNode      *temp_a1;
    f32             temp_f0;
    f32             temp_f0_2;
    f32             temp_f0_3;
    f32             temp_f10;
    f32             temp_f12;
    f32             temp_f12_2;
    f32             temp_f14;
    f32             temp_f14_2;
    f32             temp_f2;
    f32             temp_f2_2;
    f32             temp_f2_3;
    f32             temp_f2_4;
    f32             temp_f4;
    f32             temp_f6;
    modeldata_root *temp_v0;
    f32             phi_f14;

    sp2C = model->anim == 0;
    if (model->unk54 != 0)
    {
        model->unk88 = arg5;
        model->unk8C = 0.0f;
        model->unk84 = 1.0f;
    }
    else
    {
        model->unk88 = 0.0f;
        model->unk84 = 0.0f;
    }
    model->anim  = anim;
    model->unk24 = arg2;
    model->endframe = -1.0f;
    model->speed = half;
    model->timespeed = 0.0f;
    modelSetAnimFrame(startframe, model, startframe, anim);
    model->unk26 = 0;
    temp_a1      = model->obj->RootNode;
    if ((temp_a1->Opcode & 0xFF) == 1)
    {
        sp80      = temp_a1->Data;
        temp_v0   = modelGetNodeRwData(model, temp_a1);
        sp5C.unk0 = D_80036244.unk0;
        temp_f2   = model->scale * model->unkB8;
        sp5C.unk4 = D_80036244.unk4;
        sp5C.unk8 = D_80036244.unk8;
        sp70      = temp_f2;
        sp58      = sub_GAME_7F06D3F4(*sp80, model->unk24, model->obj->Skeleton, model->anim, model->unk32, &sp5C);
        if (temp_f2 != 1.0f)
        {
            sp5C *= temp_f2;
            sp60 *= temp_f2;
            sp64 *= temp_f2;
        }
        sp6C      = cosf(temp_v0->unk14);
        temp_f0   = sinf(temp_v0->unk14);
        temp_f2_2 = model->unk2C;
        if (temp_f2_2 == 0.0f)
        {
            temp_f2_3                 = temp_v0->pos.x;
            temp_v0->unk34.x          = temp_f2_3;
            temp_v0->unk34.AsArray[2] = temp_v0->pos.AsArray[2];
            temp_v0->unk34.AsArray[1] = temp_v0->pos.AsArray[1] - temp_v0->ground;
            temp_v0->unk30            = temp_v0->unk14;
            temp_f4                   = (sp64 * temp_f0) + (temp_f2_3 + (sp5C * sp6C));
            sp4C                      = temp_f4;
            sp50                      = sp60;
            sp54                      = (sp64 * sp6C) + (temp_v0->unk34.AsArray[2] - (sp5C * temp_f0));
            temp_v0->unk24.x          = temp_f4;
            temp_v0->unk24.AsArray[1] = sp50;
            temp_v0->unk24.AsArray[2] = sp54;
            if (temp_v0->unk18 == 0.0f)
            {
                temp_f0_2      = temp_v0->unk30 + sp58;
                temp_v0->unk20 = temp_f0_2;
                if (temp_f0_2 >= M_TAU_F)
                {
                    temp_v0->unk20 -= M_TAU_F;
                }
            }
            temp_v0->unk1 = 1;
        }
        else
        {
            temp_f14                  = (sp64 * temp_f0) + (sp5C * sp6C);
            temp_f10                  = (sp64 * sp6C) + (-sp5C * temp_f0);
            sp30                      = temp_f10;
            sp44                      = sp60;
            temp_f6                   = temp_v0->pos.x + (temp_f14 * (1.0f - temp_f2_2));
            sp40                      = temp_f6;
            sp48                      = temp_v0->pos.AsArray[2] + (temp_f10 * (1.0f - model->unk2C));
            temp_v0->unk24.x          = temp_f6;
            temp_v0->unk24.AsArray[1] = sp44;
            temp_v0->unk24.AsArray[2] = sp48;
            temp_v0->unk34.x          = temp_v0->unk24.x - temp_f14;
            temp_f12                  = temp_v0->pos.AsArray[1] - temp_v0->ground;
            temp_f2_4                 = model->unk2C;
            temp_f12_2                = temp_v0->unk14;
            temp_v0->unk34.AsArray[1] = temp_f12 - (((sp60 - temp_f12) * temp_f2_4) / (1.0f - temp_f2_4));
            temp_v0->unk34.AsArray[2] = temp_v0->unk24.AsArray[2] - sp30;
            temp_f14_2                = temp_f12_2 - sp58;
            phi_f14                   = temp_f14_2;
            if (temp_f14_2 < 0.0f)
            {
                phi_f14 = temp_f14_2 + M_TAU_F;
            }
            temp_v0->unk30 = sub_GAME_7F06D0CC(temp_f12_2, phi_f14, model->unk2C);
            if (temp_v0->unk18 == 0.0f)
            {
                temp_f0_3      = temp_v0->unk30 + sp58;
                temp_v0->unk20 = temp_f0_3;
                if (temp_f0_3 >= M_TAU_F)
                {
                    temp_v0->unk20 -= M_TAU_F;
                }
            }
            temp_v0->unk1 = 1;
        }
        if (sp2C != 0)
        {
            temp_v0->unk34.AsArray[1] = temp_v0->unk24.AsArray[1];
        }
    }
}
#else

void _asmpp_func25(void) {









*(volatile float*)4 = -4.214059073163126e+19f;


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
}
#endif


void modelSetAnimationWithMerge(Model *model, ModelAnimation *modelAnimation, s32 flip, f32 startframe, f32 speed, f32 timemerge, s32 domerge) {
    if (domerge != 0) {
        modelCopyAnimForMerge(model, timemerge);
    }
    modelSetAnimation2(model, modelAnimation, flip, startframe, speed, timemerge);
}



void modelSetAnimation(Model *model, ModelAnimation *modelAnimation, s32 flip, f32 startframe, f32 speed, f32 merge) {
    modelCopyAnimForMerge(model, merge);
    modelSetAnimation2(model, modelAnimation, flip, startframe, speed, merge);
}


#ifdef NONMATCHING
void sub_GAME_7F06FCFC()
{

}
#else

void _asmpp_func28(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
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





void modelSetAnimLooping(Model *model, f32 loopframe, f32 loopmerge) {
    model->animlooping = 1;
    model->animloopframe = loopframe;
    model->animloopmerge = loopmerge;
}


void modelSetAnimEndFrame(Model *model, f32 endframe) {
    ModelAnimation *modelAnimation = model->anim;

    if ((modelAnimation != NULL) && (endframe < (modelAnimation->unk04 - 1))) {
        model->endframe = endframe;
    } else {
        model->endframe = -1.0f;
    }
}

void modelSetAnimFlipFunction(Model *model, void *callback) {
    model->animflipfunc = callback;
}


/**
 * Unused Function
*/
void sub_GAME_7F06FE44(Model *model, s32 arg1) {
    model->unk9c = arg1;
}

void modelSetAnimSpeed(Model *model, f32 anim_speed, f32 startframe) {

    if (startframe > 0.0f) {
        model->timespeed = startframe;
        model->newspeed = anim_speed;
        model->elapsespeed = 0.0f;
        model->oldspeed = model->speed;
        return;
    }

    model->speed = anim_speed;
    model->timespeed = 0.0f;
}

/**
 * @param arg0:
 * @param arg1:
 * @param arg2: must be non-zero.
 *
 * Address 0x7F06FE90.
*/
void sub_GAME_7F06FE90(Model *model, f32 arg1, f32 arg2)
{
    f32 temp_f0;
    f32 phi_f2;
    f32 t;

    temp_f0 = model->unk28;

    if (temp_f0 <= arg1)
    {
        phi_f2 = arg1 - temp_f0;
    }
    else
    {
        phi_f2 = ( (f32)model->anim->unk04 - temp_f0) + arg1;
    }

    t = model->speed + ((2.0f * phi_f2) / arg2);
    modelSetAnimSpeed(model, t, arg2);
}

void modelSetAnimPlaySpeed(Model *model, f32 animation_rate, f32 startframe) {
    if (startframe > 0.0f) {
        model->unkb0 = startframe;
        model->animrate = animation_rate;
        model->unkb4 = 0.0f;
        model->unkac = model->playspeed;
        return;
    }
    model->playspeed = animation_rate;
    model->unkb0 = 0.0f;
}


void sub_GAME_7F06FF5C(Model *model, s32 arg1) {
    model->unka0 = arg1;
}


void modelSetAnimFrame(Model* model, f32 frame)
{
    s32 framea;
    s32 frameb;
    bool forwards;

    framea = floorFloatToInt(frame);

    forwards = (model->speed >= 0);
    frameb = (forwards ? framea + 1 : framea - 1);

    model->framea = modelConstrainOrWrapAnimFrame(framea, model->anim, model->endframe);
    model->frameb = modelConstrainOrWrapAnimFrame(frameb, model->anim, model->endframe);

    if (model->framea == model->frameb)
    {
        model->unk2c = 0.0f;
        model->unk28 = model->framea;
    }
    else if (forwards)
    {
        f32 tmp = frame - framea;
        model->unk2c = tmp;
        model->unk28 = model->framea + tmp;
    }
    else
    {
        f32 tmp = 1.0f - (frame - (f32) frameb);
        model->unk2c = tmp;
        model->unk28 = model->frameb + (1.0f - tmp);
    }
}


void modelSetAnimFrame2(Model* model, f32 frame1, f32 frame2)
{
    s32 framea;
    s32 frameb;
    bool forwards;

    modelSetAnimFrame(model, frame1);

    if (model->anim2 != NULL)
    {
        framea = floorFloatToInt(frame2);

        forwards = (model->speed2 >= 0.0f);
        frameb = forwards ? (framea + 1) : (framea - 1);

        model->frame2a = modelConstrainOrWrapAnimFrame(framea, model->anim2, model->unk6c);
        model->frame2b = modelConstrainOrWrapAnimFrame(frameb, model->anim2, model->unk6c);

        if (model->frame2a == model->frame2b)
        {
            model->unk5c = 0.0f;
            model->unk58 = model->frame2a;
        }
        else if (forwards != 0)
        {
            f32 tmp = frame2 - framea;
            model->unk5c = tmp;
            model->unk58 = model->frame2a + tmp;
        }
        else
        {
            f32 tmp = 1.0f - (frame2 - (f32) frameb);
            model->unk5c = tmp;
            model->unk58 = model->frame2b + (1.0f - tmp);
        }
    }
}


/**
 * Address 0x7F0701D4.
*/
void modelSetAnimMergingEnabled(s32 arg0)
{
    g_ModelAnimMergingEnabled = arg0;
}


/**
 * Address 0x7F0701E0.
*/
u32 modelIsAnimMergingEnabled(void)
{
    return g_ModelAnimMergingEnabled;
}




#ifdef NONMATCHING
void modelSetAnimFrame2WithChrStuff(void) {

}
#else

void _asmpp_func29(void) {





*(volatile float*)4 = -4.21406039257708e+19f;


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
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

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
 } void _asmpp_large_func34(void) { 
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
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
void modelTickAnimQuarterSpeed(void) {

}
#else

void _asmpp_func35(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
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
}
#endif

/**
 * @brief Model Type 1: 1Cycle No Secondary
 * @param[in,out] renderdata append cycle, CC and RM to display List
 */
void modelApplyRenderModeType1(ModelRenderData *renderdata)
{
    gDPPipeSync(renderdata->gdl++);
    gDPSetCycleType(renderdata->gdl++, G_CYC_1CYCLE);

    if (renderdata->zbufferenabled)
    {
        gDPSetRenderMode(renderdata->gdl++, G_RM_AA_ZB_OPA_SURF2, G_RM_AA_ZB_OPA_SURF);
    }
    else
    {
        gDPSetRenderMode(renderdata->gdl++, G_RM_AA_OPA_SURF, G_RM_AA_OPA_SURF2);
    }

    gDPSetCombineMode(renderdata->gdl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
}

/**
 * @brief Model Type 3: GunLighting - Reduced Secondary Commands (guns)
    This Type Uses Vertex Alpha for Secondary Surfaces and uses the FOG Alpha value for applying Fog/"Lighting".
 * @param renderdata
 * @param isPrimary
 */
void modelApplyRenderModeType3(ModelRenderData *renderdata, bool isPrimary)
{
    if (renderdata->PropType == PROP_TYPE_VIEWER+1)
    {
        if (isPrimary)
        {
            u8 r, g, b, a;
            gDPPipeSync(renderdata->gdl++);
            gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);

            r = _SHIFTR(renderdata->fogcolour.word, 24, 8);
            g = _SHIFTR(renderdata->fogcolour.word, 16, 8);
            b = _SHIFTR(renderdata->fogcolour.word, 8, 8);
            a = _SHIFTR(renderdata->fogcolour.word, 0, 8);
            gDPSetFogColor(renderdata->gdl++, r, g, b, a);

            r = _SHIFTR(renderdata->envcolour.word, 24, 8);
            g = _SHIFTR(renderdata->envcolour.word, 16, 8);
            b = _SHIFTR(renderdata->envcolour.word, 8, 8);
            a = 0xFF;
            gDPSetEnvColor(renderdata->gdl++, r, g, b, a);

            gDPSetCombineLERP(renderdata->gdl++, TEXEL0, ENVIRONMENT, SHADE_ALPHA, ENVIRONMENT, TEXEL0, ENVIRONMENT, SHADE, ENVIRONMENT, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED);

            if (renderdata->zbufferenabled)
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_OPA_SURF2);
            }
            else
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_OPA_SURF2);
            }
        }
        else
        {
            if (renderdata->zbufferenabled)
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
            }
            else
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
            }
        }
    }
    else if (renderdata->PropType == PROP_TYPE_EXPLOSION+1)
    {
        if (isPrimary)
        {
            u8 r, g, b, a;
            gDPPipeSync(renderdata->gdl++);
            gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);

            r = _SHIFTR(renderdata->fogcolour.word, 24, 8);
            g = _SHIFTR(renderdata->fogcolour.word, 16, 8);
            b = _SHIFTR(renderdata->fogcolour.word, 8, 8);
            a = _SHIFTR(renderdata->fogcolour.word, 0, 8);
            gDPSetFogColor(renderdata->gdl++, r, g, b, a);

            r = _SHIFTR(renderdata->envcolour.word, 24, 8);
            g = _SHIFTR(renderdata->envcolour.word, 16, 8);
            b = _SHIFTR(renderdata->envcolour.word, 8, 8);
            a = _SHIFTR(renderdata->envcolour.word, 0, 8);
            gDPSetEnvColor(renderdata->gdl++, r, g, b, a);

            gDPSetCombineLERP(renderdata->gdl++, TEXEL0, ENVIRONMENT, SHADE_ALPHA, ENVIRONMENT, TEXEL0, 0, ENVIRONMENT, 0, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED);

            if (renderdata->zbufferenabled)
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
            }
            else
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
            }
        }
    }
    else if (renderdata->PropType == PROP_TYPE_SMOKE+1)
    {
        if ((renderdata->envcolour.word & 0xFF) == 0)
        {
            if (isPrimary)
            {
                u8 r = _SHIFTR(renderdata->fogcolour.word, 24, 8);
                u8 g = _SHIFTR(renderdata->fogcolour.word, 16, 8);
                u8 b = _SHIFTR(renderdata->fogcolour.word, 8, 8);
                u8 a = _SHIFTR(renderdata->fogcolour.word, 0, 8);

                gDPPipeSync(renderdata->gdl++);
                gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
                gDPSetFogColor(renderdata->gdl++, r, g, b, a);
                gDPSetEnvColor(renderdata->gdl++, 0xFF, 0xFF, 0xFF, 0xFF);
                gDPSetPrimColor(renderdata->gdl++, 0, 0, 0, 0, 0, (renderdata->envcolour.word >> 8) & 0xFF);

                gDPSetCombineLERP(renderdata->gdl++, TEXEL1, TEXEL0, LOD_FRACTION, TEXEL0, TEXEL1, TEXEL0, LOD_FRACTION, TEXEL0, COMBINED, 0, SHADE, 0, COMBINED, 0, SHADE, PRIMITIVE);

                if (renderdata->zbufferenabled)
                {
                    gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_OPA_SURF2);
                }
                else
                {
                    gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_OPA_SURF2);
                }
            }
            else
            {
                if (renderdata->zbufferenabled)
                {
                    gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
                }
                else
                {
                    gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
                }
            }
        }
        else
        {
            if (isPrimary)
            {
                u8 r = _SHIFTR(renderdata->fogcolour.word, 24, 8);
                u8 g = _SHIFTR(renderdata->fogcolour.word, 16, 8);
                u8 b = _SHIFTR(renderdata->fogcolour.word, 8, 8);
                u8 a = _SHIFTR(renderdata->fogcolour.word, 0, 8);

                gDPPipeSync(renderdata->gdl++);
                gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
                gDPSetFogColor(renderdata->gdl++, r, g, b, a);
                gDPSetEnvColor(renderdata->gdl++, 0, 0, 0, renderdata->envcolour.word & 0xFF);

                gDPSetCombineLERP(renderdata->gdl++, TEXEL1, TEXEL0, LOD_FRACTION, TEXEL0, 1, 0, SHADE, ENVIRONMENT, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED);

                if (renderdata->zbufferenabled)
                {
                    gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_TEX_EDGE2);
                }
                else
                {
                    gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_TEX_EDGE2);
                }
            }
            else
            {
                gDPSetPrimColor(renderdata->gdl++, 0, 0, 0, 0, 0, (renderdata->envcolour.word >> 8) & 0xFF);
                gDPSetCombineLERP(renderdata->gdl++, TEXEL1, TEXEL0, LOD_FRACTION, TEXEL0, SHADE, ENVIRONMENT, TEXEL0, 0, COMBINED, 0, SHADE, 0, 1, 0, PRIMITIVE, COMBINED);

                if (renderdata->zbufferenabled)
                {
                    gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_TEX_EDGE2);
                }
                else
                {
                    gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_TEX_EDGE2);
                }
            }
        }
    }
    else if (renderdata->PropType == PROP_TYPE_CHR+1)
    {
        if (isPrimary)
        {
            u8 r = _SHIFTR(renderdata->envcolour.word, 24, 8);
            u8 g = _SHIFTR(renderdata->envcolour.word, 16, 8);
            u8 b = _SHIFTR(renderdata->envcolour.word, 8, 8);
            u8 a = _SHIFTR(renderdata->envcolour.word, 0, 8);

            gDPPipeSync(renderdata->gdl++);
            gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
            gDPSetFogColor(renderdata->gdl++, r, g, b, a);

            gDPSetCombineMode(renderdata->gdl++, G_CC_TRILERP, G_CC_MODULATEIA2);

            if (renderdata->zbufferenabled)
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_OPA_SURF2);
            }
            else
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_OPA_SURF2);
            }
        }
        else
        {
            if (renderdata->zbufferenabled)
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
            }
            else
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
            }
        }
    }
    else if (renderdata->PropType == PROP_TYPE_WEAPON+1)
    {
        u8 r, g, b, a;
        if (isPrimary)
        {
            gDPPipeSync(renderdata->gdl++);
            gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);

            r = _SHIFTR(renderdata->fogcolour.word, 24, 8);
            g = _SHIFTR(renderdata->fogcolour.word, 16, 8);
            b = _SHIFTR(renderdata->fogcolour.word, 8, 8);
            a = _SHIFTR(renderdata->fogcolour.word, 0, 8);
            gDPSetFogColor(renderdata->gdl++, r, g, b, a);

            a = renderdata->envcolour.word & 0xFF;

            if (a < 255)
            {
                gDPSetEnvColor(renderdata->gdl++, 0xFF, 0xFF, 0xFF, a);

                if (renderdata->envcolour.word & 0xFF00)
                {
                    gDPSetCombineLERP(renderdata->gdl++, TEXEL1, TEXEL0, LOD_FRACTION, TEXEL0, 1, SHADE, ENVIRONMENT, 0, COMBINED, 0, SHADE, 0, COMBINED, 0, SHADE, 0);
                }
                else
                {
                    gDPSetCombineLERP(renderdata->gdl++, TEXEL1, TEXEL0, LOD_FRACTION, TEXEL0, 1, 0, ENVIRONMENT, 0, COMBINED, 0, SHADE, 0, COMBINED, 0, SHADE, 0);
                }
            }
            else
            {
                gDPSetCombineMode(renderdata->gdl++, G_CC_TRILERP, G_CC_MODULATEIA2);
            }

            if (renderdata->zbufferenabled)
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
            }
            else
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
            }
        }
        else
        {
            a = renderdata->envcolour.word & 0xFF;

            if (a < 255)
            {
                gDPSetCombineLERP(renderdata->gdl++, TEXEL1, TEXEL0, LOD_FRACTION, TEXEL0, TEXEL0, 0, ENVIRONMENT, 0, COMBINED, 0, SHADE, 0, COMBINED, 0, SHADE, 0);
            }
            else
            {
                gDPSetCombineMode(renderdata->gdl++, G_CC_TRILERP, G_CC_MODULATEIA2);
            }
        }
    }
    else
    {
        if (isPrimary)
        {
            gDPPipeSync(renderdata->gdl++);
            gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
            gDPSetCombineMode(renderdata->gdl++, G_CC_TRILERP, G_CC_MODULATEIA2);

            if (renderdata->zbufferenabled)
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_PASS, G_RM_AA_ZB_OPA_SURF2);
            }
            else
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_PASS, G_RM_AA_OPA_SURF2);
            }
        }
        else
        {
            if (renderdata->zbufferenabled)
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_PASS, G_RM_AA_ZB_XLU_SURF2);
            }
            else
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_PASS, G_RM_AA_XLU_SURF2);
            }
        }
    }
}

/**
 * @brief Model Type 4: Normal Fog/Lighting object
    This Type Uses Vertex Alpha for Secondary Surfaces and uses the FOG Alpha value for applying Fog/"Lighting".
 * @param renderdata
 * @param isPrimary Type of DisplayList
 */
void modelApplyRenderModeType4(ModelRenderData *renderdata, bool isPrimary)
{
    if (renderdata->PropType == PROP_TYPE_VIEWER+1)
    {
        u8 r, g, b, a;
        gDPPipeSync(renderdata->gdl++);
        gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);

        r = _SHIFTR(renderdata->fogcolour.word, 24, 8);
        g = _SHIFTR(renderdata->fogcolour.word, 16, 8);
        b = _SHIFTR(renderdata->fogcolour.word, 8, 8);
        a = _SHIFTR(renderdata->fogcolour.word, 0, 8);
        gDPSetFogColor(renderdata->gdl++, r, g, b, a);

        r = _SHIFTR(renderdata->envcolour.word, 24, 8);
        g = _SHIFTR(renderdata->envcolour.word, 16, 8);
        b = _SHIFTR(renderdata->envcolour.word, 8, 8);
        a = 0xFF;
        gDPSetEnvColor(renderdata->gdl++, r, g, b, a);

        gDPSetCombineLERP(renderdata->gdl++, TEXEL0, ENVIRONMENT, SHADE_ALPHA, ENVIRONMENT, TEXEL0, ENVIRONMENT, SHADE, ENVIRONMENT, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED);

        if (isPrimary)
        {
            if (renderdata->zbufferenabled)
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_OPA_SURF2);
            }
            else
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_OPA_SURF2);
            }
        }
        else
        {
            if (renderdata->zbufferenabled)
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
            }
            else
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
            }
        }
    }
    else if (renderdata->PropType == PROP_TYPE_EXPLOSION+1)
    {
        u8 r, g, b, a;
        gDPPipeSync(renderdata->gdl++);
        gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);

        r = _SHIFTR(renderdata->fogcolour.word, 24, 8);
        g = _SHIFTR(renderdata->fogcolour.word, 16, 8);
        b = _SHIFTR(renderdata->fogcolour.word, 8, 8);
        a = _SHIFTR(renderdata->fogcolour.word, 0, 8);
        gDPSetFogColor(renderdata->gdl++, r, g, b, a);

        r = _SHIFTR(renderdata->envcolour.word, 24, 8);
        g = _SHIFTR(renderdata->envcolour.word, 16, 8);
        b = _SHIFTR(renderdata->envcolour.word, 8, 8);
        a = _SHIFTR(renderdata->envcolour.word, 0, 8);
        gDPSetEnvColor(renderdata->gdl++, r, g, b, a);

        gDPSetCombineLERP(renderdata->gdl++, TEXEL0, ENVIRONMENT, SHADE_ALPHA, ENVIRONMENT, TEXEL0, 0, ENVIRONMENT, 0, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED);

        if (renderdata->zbufferenabled)
        {
            gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
        }
        else
        {
            gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
        }
    }
    else if (renderdata->PropType == PROP_TYPE_SMOKE+1)
    {
        if ((renderdata->envcolour.word & 0xFF) == 0)
        {
            u8 r = _SHIFTR(renderdata->fogcolour.word, 24, 8);
            u8 g = _SHIFTR(renderdata->fogcolour.word, 16, 8);
            u8 b = _SHIFTR(renderdata->fogcolour.word, 8, 8);
            u8 a = _SHIFTR(renderdata->fogcolour.word, 0, 8);

            gDPPipeSync(renderdata->gdl++);
            gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
            gDPSetFogColor(renderdata->gdl++, r, g, b, a);
            gDPSetEnvColor(renderdata->gdl++, 0xFF, 0xFF, 0xFF, 0xFF);
            gDPSetPrimColor(renderdata->gdl++, 0, 0, 0, 0, 0, ((renderdata->envcolour.word >> 8 ) & 0xFF));

            if (isPrimary)
            {
                gDPSetCombineLERP(renderdata->gdl++, TEXEL1, TEXEL0, LOD_FRACTION, TEXEL0, TEXEL1, TEXEL0, LOD_FRACTION, TEXEL0, COMBINED, 0, SHADE, 0, COMBINED, 0, SHADE, PRIMITIVE);

                if (renderdata->zbufferenabled)
                {
                    gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_OPA_SURF2);
                }
                else
                {
                    gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_OPA_SURF2);
                }
            }
            else
            {
                gDPSetCombineLERP(renderdata->gdl++, TEXEL1, TEXEL0, LOD_FRACTION, TEXEL0, TEXEL1, TEXEL0, LOD_FRACTION, TEXEL0, COMBINED, 0, SHADE, 0, COMBINED, 0, SHADE, PRIMITIVE);

                if (renderdata->zbufferenabled)
                {
                    gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
                }
                else
                {
                    gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
                }
            }
        }
        else
        {
            u8 r = _SHIFTR(renderdata->fogcolour.word, 24, 8);
            u8 g = _SHIFTR(renderdata->fogcolour.word, 16, 8);
            u8 b = _SHIFTR(renderdata->fogcolour.word, 8, 8);
            u8 a = _SHIFTR(renderdata->fogcolour.word, 0, 8);

            gDPPipeSync(renderdata->gdl++);
            gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
            gDPSetFogColor(renderdata->gdl++, r, g, b, a);
            gDPSetEnvColor(renderdata->gdl++, 0, 0, 0, renderdata->envcolour.word & 0xFF);

            if (isPrimary)
            {
                gDPSetCombineLERP(renderdata->gdl++, TEXEL1, TEXEL0, LOD_FRACTION, TEXEL0, 1, 0, SHADE, ENVIRONMENT, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED);

                if (renderdata->zbufferenabled)
                {
                    gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_TEX_EDGE2);
                }
                else
                {
                    gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_TEX_EDGE2);
                }
            }
            else
            {
                gDPSetPrimColor(renderdata->gdl++, 0, 0, 0, 0, 0, (renderdata->envcolour.word >> 8) & 0xFF);
                gDPSetCombineLERP(renderdata->gdl++, TEXEL1, TEXEL0, LOD_FRACTION, TEXEL0, SHADE, ENVIRONMENT, TEXEL0, 0, COMBINED, 0, SHADE, 0, 1, 0, PRIMITIVE, COMBINED);

                if (renderdata->zbufferenabled)
                {
                    gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_TEX_EDGE2);
                }
                else
                {
                    gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_TEX_EDGE2);
                }
            }
        }
    }
    else if (renderdata->PropType == PROP_TYPE_CHR+1)
    {
        u8 r = _SHIFTR(renderdata->envcolour.word, 24, 8);
        u8 g = _SHIFTR(renderdata->envcolour.word, 16, 8);
        u8 b = _SHIFTR(renderdata->envcolour.word, 8, 8);
        u8 a = _SHIFTR(renderdata->envcolour.word, 0, 8);

        gDPPipeSync(renderdata->gdl++);
        gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
        gDPSetFogColor(renderdata->gdl++, r, g, b, a);

        gDPSetCombineMode(renderdata->gdl++, G_CC_TRILERP, G_CC_MODULATEIA2);

        if (isPrimary)
        {
            if (renderdata->zbufferenabled)
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_OPA_SURF2);
            }
            else
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_OPA_SURF2);
            }
        }
        else
        {
            if (renderdata->zbufferenabled)
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
            }
            else
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
            }
        }
    }
    else if (renderdata->PropType == PROP_TYPE_WEAPON+1)
    {
        u8 r, g, b, a;

        gDPPipeSync(renderdata->gdl++);
        gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);

        r = _SHIFTR(renderdata->fogcolour.word, 24, 8);
        g = _SHIFTR(renderdata->fogcolour.word, 16, 8);
        b = _SHIFTR(renderdata->fogcolour.word, 8, 8);
        a = _SHIFTR(renderdata->fogcolour.word, 0, 8);
        gDPSetFogColor(renderdata->gdl++, r, g, b, a);

        a = renderdata->envcolour.word & 0xFF;

        if (a < 255)
        {
            gDPSetEnvColor(renderdata->gdl++, 0xFF, 0xFF, 0xFF, a);

            if (isPrimary)
            {
                if (renderdata->envcolour.word & 0xFF00) //apply inverse vertex alpha if any
                {
                    gDPSetCombineLERP(renderdata->gdl++, TEXEL1, TEXEL0, LOD_FRACTION, TEXEL0, 1, SHADE, ENVIRONMENT, 0, COMBINED, 0, SHADE, 0, COMBINED, 0, SHADE, 0);
                }
                else
                {
                    gDPSetCombineLERP(renderdata->gdl++, TEXEL1, TEXEL0, LOD_FRACTION, TEXEL0, 1, 0, ENVIRONMENT, 0, COMBINED, 0, SHADE, 0, COMBINED, 0, SHADE, 0);
                }
            }
            else
            {
                gDPSetCombineLERP(renderdata->gdl++, TEXEL1, TEXEL0, LOD_FRACTION, TEXEL0, TEXEL0, 0, ENVIRONMENT, 0, COMBINED, 0, SHADE, 0, COMBINED, 0, SHADE, 0);
            }
        }
        else
        {
            gDPSetCombineMode(renderdata->gdl++, G_CC_TRILERP, G_CC_MODULATEIA2);
        }

        if (renderdata->zbufferenabled)
        {
            gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
        }
        else
        {
            gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
        }
    }
    else
    {
        gDPPipeSync(renderdata->gdl++);
        gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
        gDPSetFogColor(renderdata->gdl++, 0xFF, 0xFF, 0xFF, 0x00);
        gDPSetCombineMode(renderdata->gdl++, G_CC_TRILERP, G_CC_MODULATEIA2);

        if (isPrimary)
        {
            if (renderdata->zbufferenabled)
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_OPA_SURF2);
            }
            else
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_OPA_SURF2);
            }
        }
        else
        {
            if (renderdata->zbufferenabled)
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
            }
            else
            {
                gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
            }
        }
    }
}

/**
 * @brief Model Type 2: 2Cycle No Secondary
 * @param[in,out] renderdata append cycle, CC and RM to display List
 */
void modelApplyRenderModeType2(ModelRenderData *renderdata)
{
    gDPPipeSync(renderdata->gdl++);
    gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);

    if (renderdata->zbufferenabled)
    {
        gDPSetRenderMode(renderdata->gdl++, G_RM_PASS, G_RM_AA_ZB_OPA_SURF2);
    }
    else
    {
        gDPSetRenderMode(renderdata->gdl++, G_RM_PASS, G_RM_AA_OPA_SURF2);
    }

    gDPSetCombineMode(renderdata->gdl++, G_CC_TRILERP, G_CC_MODULATEIA2);
}


void modelApplyCullMode(ModelRenderData *renderdata)
{
    if (renderdata->cullmode == CULLMODE_NONE)
    {
        gSPClearGeometryMode(renderdata->gdl++, G_CULL_BOTH);
    }
    else if (renderdata->cullmode == CULLMODE_FRONT)
    {
        gSPSetGeometryMode(renderdata->gdl++, G_CULL_FRONT);
    }
    else if (renderdata->cullmode == CULLMODE_BACK)
    {
        gSPSetGeometryMode(renderdata->gdl++, G_CULL_BACK);
    }
}


void modelRenderNodeGundl(ModelRenderData* renderdata, ModelNode* arg1)
{
    ModelRoData_DisplayListRecord* rodata = &arg1->Data->DisplayList;

    if (renderdata->unk18 == 0)
    {
        if ((renderdata->flags & 1) && rodata->Primary)
        {
            gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_COL1, osVirtualToPhysical(rodata->BaseAddr));

            if (renderdata->cullmode)
            {
                modelApplyCullMode(renderdata);
            }

            if (rodata->ModelType == 1)
            {
                modelApplyRenderModeType1(renderdata);
            }
            else if (rodata->ModelType == 3)
            {
                modelApplyRenderModeType3(renderdata, 1);
            }
            else if (rodata->ModelType == 4)
            {
                modelApplyRenderModeType4(renderdata, 1);
            }
            else if (rodata->ModelType == 2)
            {
                modelApplyRenderModeType2(renderdata);
            }

            gSPDisplayList(renderdata->gdl++, rodata->Primary);

            if ((rodata->ModelType == 3) && rodata->Secondary)
            {
                modelApplyRenderModeType3(renderdata, 0);
                gSPDisplayList(renderdata->gdl++, rodata->Secondary);
            }
        }

        if ((renderdata->flags & 2) && rodata->Primary && (rodata->ModelType == 4) && rodata->Secondary)
        {
            gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_COL1, osVirtualToPhysical(rodata->BaseAddr));

            if (renderdata->cullmode)
            {
                modelApplyCullMode(renderdata);
            }

            modelApplyRenderModeType4(renderdata, 0);
            gSPDisplayList(renderdata->gdl++, rodata->Secondary);
        }
    }
}

/*

A1 is primary = 1, secondary = 0
Inside the T8 or whatever temporary register indicates gun or not gun (0 = gun, or UseZ = 1), for different render mode

Bool UseZ //guns = false
Bool


Model Type 0: NoSetup.
    type 0 Has No DL Setup and will use whaterver is currently set.

Model Type 1: 1Cycle No Sec
    E700000000000000 pipesync()
    BA00140200000000 CycleType(1c)
    if UseZ
      B900031D00552078 SetRendermode(AA_ZB_OPA_1) //cin ain cmem amem
    else
      B900031D00552048 SetRendermode(AA_OPA_1) //cin ain cmem amem
    end if
    FC121824FF33FFFF SetCombine(MODULATERGBA)
    No Secondary

Model Type 2: 2Cycle No Sec
    E700000000000000 pipesync()
    BA00140200100000 CycleType(2c)
    if UseZ
      B900031D0C192078 SetRendermode(AA_ZB_OPA_2) // cin 0 cin 1 //colour only
    else
      B900031D0C192048 SetRendermode(AA_OPA_2) // cin 0 cin 1 //colour only
    end if
    FC26A0041F1093FF SetCombine(TRILERP, MODULATERGBA)
    No Secondary
Model Type 3: GunLighting - Reduced Secondary Commands (guns)
    This Type Uses Vertex Alpha for Secondary Surfaces and uses the FOG Alpha value for applying Fog/"Lighting".
    Primary
    E700000000000000 pipesync()
    BA00140200100000 CycleType(2c)
    F800000000000026 SetFogColor(0,0,0,38)
    if dltype = full
      if guard
        FB0000005A0000FF SetEnvColor(90,0,0,255)
        FC1598045FFEDBF8 SetCombine(((Texel0-Env)*ShadeA+Env)
                         ((Texel0-Env)*Shade+Env),
                         MODULATERGB_DECALA)
      else if prop
        FB000000FFFFFFFF SetEnvColor(255,255,255,255)
        FA00000000000000 SetPrimColor(0,0,0,0)
        FC26A0041F1093FB SetCombine(TRILERP, MODULATERGB_ADDPRIM_A)
      endif
    else
      FC26A0041F1093FF SetCombine(TRILERP, MODULATERGBA)
    endif
    if UseZ
      B900031DC4112078 SetRendermode(AA_ZB_OPA_StanFOG_2)
    else
      B900031DC4112048 SetRendermode(AA_OPA_StanFOG_2) //acvg
    endif

    Secondary
    if UseZ
      B900031DC41049D8 SetRendermode(AA_Zcmp_XLU_StanFOG_2)
    else
      B900031DC41041C8 SetRendermode(AA_OPA_StanFOG_2)//FcBl ClrOnCvg
    endif

Model Type 4: Normal Fog/Lighting object
    This Type Uses Vertex Alpha for Secondary Surfaces and uses the FOG Alpha value for applying Fog/"Lighting".
    Primary
    E700000000000000 pipesync()
    BA00140200100000 CycleType(2c)
    F800000000000026 SetFogColor(0,0,0,38)
    if dltype = full
      if guard
        FB0000005A0000FF SetEnvColor(90,0,0,255)
        FC1598045FFEDBF8 SetCombine(((Texel0-Env)*ShadeA+Env)
                         ((Texel0-Env)*Shade+Env),
                         MODULATERGB_DECALA)
      else if prop
        FB000000FFFFFFFF SetEnvColor(255,255,255,255)
        FA00000000000000 SetPrimColor(0,0,0,0)
        FC26A0041F1093FB SetCombine(TRILERP, MODULATERGB_ADDPRIM_A)
      endif
    else
      FC26A0041F1093FF SetCombine(TRILERP, MODULATERGBA)
    endif
    if UseZ
      B900031DC4112078 SetRendermode(AA_ZB_OPA_StanFOG_2)
    else
      B900031DC4112048 SetRendermode(AA_OPA_StanFOG_2) //acvg
    endif

    Secondary
    E700000000000000 pipesync()
    BA00140200100000 CycleType(2c)
    F800000000000026 SetFogColor(0,0,0,38)
    if dltype = full
      if guard
        FB0000005A0000FF SetEnvColor(90,0,0,255)
        FC1598045FFEDBF8 SetCombine(((Texel0-Env)*ShadeA+Env)
                         ((Texel0-Env)*Shade+Env),
                         MODULATERGB_DECALA)
      else if prop
        FB000000FFFFFFFF SetEnvColor(255,255,255,255)
        FA00000000000000 SetPrimColor(0,0,0,0)
        FC26A0041F1093FB SetCombine(TRILERP, MODULATERGB_ADDPRIM_A)
      endif
    else
      FA00000000000000 SetPrimColor(0,0,0,0)
      FC26A0041F1093FB SetCombine(TRILERP, MODULATERGB_ADDPRIM_A)
    endif
    if UseZ
      B900031DC41049D8 SetRendermode(AA_Zcmp_XLU_StanFOG_2)
    else
      B900031DC41041C8 SetRendermode(AA_OPA_StanFOG_2)//FcBl ClrOnCvg
    endif
*/

/**
* 7F072A0C
* DisplayList Setups Depend on Object Type, Prop Guard or Gun.
These are applied to each part of an object at runtime and can be overridden. loading the next part will use these values once more.
GeometryMode is not in setup and is persistent accross parts.
*/
void modelRenderNodeDl(ModelRenderData *renderdata, Model *model, ModelNode *node)
{
    union ModelRoData *rodata = node->Data;

    if (!renderdata->unk18)
    {
        if (renderdata->flags & 1)
        {
            union ModelRwData *rwdata = modelGetNodeRwData(model, node);

            if (rwdata->DisplayListCollisions.gdl)
            {
                gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_COL1, osVirtualToPhysical(rodata->DisplayListCollisions.BaseAddr));

                if (renderdata->cullmode)
                {
                    modelApplyCullMode(renderdata);
                }

                if (rodata->DisplayListCollisions.ModelType == 1)
                {
                    modelApplyRenderModeType1(renderdata);
                }
                else if (rodata->DisplayListCollisions.ModelType == 3)
                {
                    modelApplyRenderModeType3(renderdata, TRUE);
                }
                else if (rodata->DisplayListCollisions.ModelType == 4)
                {
                    modelApplyRenderModeType4(renderdata, TRUE);
                }
                else if (rodata->DisplayListCollisions.ModelType == 2)
                {
                    modelApplyRenderModeType2(renderdata);
                }

                gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_VTX, osVirtualToPhysical(rwdata->DisplayListCollisions.Vertices));

                gSPDisplayList(renderdata->gdl++, rwdata->DisplayListCollisions.gdl);

                if (rodata->DisplayListCollisions.ModelType == 3 && rodata->DisplayListCollisions.Secondary)
                {
                    modelApplyRenderModeType3(renderdata, FALSE);
                    gSPDisplayList(renderdata->gdl++, rodata->DisplayListCollisions.Secondary);
                }
            }
        }

        if (renderdata->flags & 2)
        {
            union ModelRwData *rwdata = modelGetNodeRwData(model, node);

            if (rwdata->DisplayListCollisions.gdl && rodata->DisplayListCollisions.ModelType == 4 && rodata->DisplayListCollisions.Secondary)
            {
                gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_COL1, osVirtualToPhysical(rodata->DisplayListCollisions.BaseAddr));

                if (renderdata->cullmode)
                {
                    modelApplyCullMode(renderdata);
                }

                gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_VTX, osVirtualToPhysical(rwdata->DisplayListCollisions.Vertices));

                modelApplyRenderModeType4(renderdata, FALSE);

                gSPDisplayList(renderdata->gdl++, rodata->DisplayListCollisions.Secondary);
            }
        }
    }
}


void sub_GAME_7F072C10(ModelRenderData *param_1, struct Model *param_2, struct ModelNode *param_3)
{
    return;
}


/**
 * Star gunfire is a muzzle flash in a first person perspective, where the
 * muzzle flash has 3 or 4 "arms" that flare out from the main body.
 *
 * This function reads vertices from the model definition, tweaks them randomly,
 * writes them to a newly allocated vertices table and queues the node's
 * displaylist to the renderdata's DL.
 */
void dorottex(ModelRenderData *renderdata, ModelNode *node)
{
    if (renderdata->unk18 == 0 && (renderdata->flags & 2))
    {

        ModelRoData_DisplayListPrimaryRecord *rodata = &node->Data->DisplayListPrimary;
        s32 i;

        if (rodata->Primary)
        {
            Vertex *src;
            Vertex *dst;

            src = (Vertex *) rodata->Vertices;

#ifndef VERSION_EU
            if (vtxallocator != NULL)
            {
            }
            else
            {
                osSyncPrintf("dorottex: no vtx allocator!\n");
                return_null();
            }
#endif
            dst = vtxallocator(rodata->numVertices * 4);

            gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_VTX, osVirtualToPhysical(dst));
            gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_COL1, osVirtualToPhysical(rodata->BaseAddr));

            gDPSetFogColor(renderdata->gdl++, 0x00, 0x00, 0x00, 0x00);
            gSPDisplayList(renderdata->gdl++, rodata->Primary);

            for (i = 0; i < rodata->numVertices; i++)
            {
                u16 rand1 = (randomGetNext() << 10) & 0xffff;
                s32 s4 = ((coss(rand1) << 5) * 181) >> 18;
                s32 s3 = ((sins(rand1) << 5) * 181) >> 18;
                s32 s1 = (u32)randomGetNext() >> 31;
                s32 mult = 0x10000 - (randomGetNext() & 0x3fff);
                s32 corner1 = 0x200 + s3;
                s32 corner2 = 0x200 - s3;
                s32 corner3 = 0x200 - s4;
                s32 corner4 = 0x200 + s4;

                dst[0] = src[0];
                dst[1] = src[1];
                dst[2] = src[2];
                dst[3] = src[3];

                dst[0].s = corner3;
                dst[0].t = corner2;
                dst[0].coord.x = (src[(s1 + 0) % 4].coord.x * mult) >> 16;
                dst[0].coord.y = (src[(s1 + 0) % 4].coord.y * mult) >> 16;
                dst[0].coord.z = (src[(s1 + 0) % 4].coord.z * mult) >> 16;

                dst[1].s = corner1;
                dst[1].t = corner3;
                dst[1].coord.x = (src[(s1 + 1) % 4].coord.x * mult) >> 16;
                dst[1].coord.y = (src[(s1 + 1) % 4].coord.y * mult) >> 16;
                dst[1].coord.z = (src[(s1 + 1) % 4].coord.z * mult) >> 16;

                dst[2].s = corner4;
                dst[2].t = corner1;
                dst[2].coord.x = (src[(s1 + 2) % 4].coord.x * mult) >> 16;
                dst[2].coord.y = (src[(s1 + 2) % 4].coord.y * mult) >> 16;
                dst[2].coord.z = (src[(s1 + 2) % 4].coord.z * mult) >> 16;

                dst[3].s = corner2;
                dst[3].t = corner4;
                dst[3].coord.x = (src[(s1 + 3) % 4].coord.x * mult) >> 16;
                dst[3].coord.y = (src[(s1 + 3) % 4].coord.y * mult) >> 16;
                dst[3].coord.z = (src[(s1 + 3) % 4].coord.z * mult) >> 16;

                src += 4;
                dst += 4;
            }
        }
    }
}


void sub_GAME_7F073038(ModelRenderData *renderdata, struct sImageTableEntry *tconfig, s32 arg2)
{
    texSelect(&renderdata->gdl, tconfig, arg2, renderdata->zbufferenabled, 2);
}


void sub_GAME_7F07306C(s32 param_1,struct Model *param_2,struct ModelNode *param_3)
{
    return;
}


void dotube(ModelRenderData* renderdata, Model* model, ModelNode* node)
{
    s32 rw_index_sel;
    s32 rw_index_sel2;
    s32 c_entry_count;
    struct ModelRoData_Op07Record *rodata2;
    s32 c_entry2_count;
    s32 c_entry_index;
    s32 renderpos_index;
    ModelNode *node_from_07;
    struct ModelRoData_Child *c_entry2;
    Vertex *vtx2;
    u8 *entry2_04;
    s32 unused1;
    bool swap_order;
    struct ModelRoData_Op07Record *rodata;
    struct ModelRoData_Child *c_entry;
    struct ModelRwData_Op07Record *rwdata;
    Vertex *vtx_10;
    Vertex *vtx_10_2;
    RenderPosView *render_pos2;
    RenderPosView *render_pos;
    s32 rw_index2;
    s32 rw_index;
    Vertex *vtx1;
    struct ModelRwData_Op07Record *rwdata2;
    u8 *entry_04;
    s32 unused2;
    s32 renderpos_index2;
    s32 unused3;

    rodata = &node->Data->Op07;
    rwdata = &modelGetNodeRwData(model, node)->Op07;

    if (rodata->unk00 != NULL)
    {
        node_from_07 = rodata->unk00;
    }
    else
    {
        node_from_07 = rodata->unk04;
    }

    rodata2 = &node_from_07->Data->Op07;
    rwdata2 = &modelGetNodeRwData(model, node_from_07)->Op07;
    swap_order = 1;

    if (renderdata->flags & 1)
    {
        renderpos_index2 = modelFindNodeMtxIndex(node, 0);
        render_pos2 = &model->render_pos[renderpos_index2];
        rw_index = rwdata->index;
        rw_index2 = rwdata2->index;
        c_entry = &rodata->Children[rw_index];

        if (rodata->unk00 != NULL)
        {
            rw_index_sel = rw_index2;
            rw_index_sel2 = rw_index;
            renderpos_index = modelFindNodeMtxIndex(node, 0x200);
        }
        else
        {
            rw_index_sel = rw_index;
            rw_index_sel2 = rw_index2;
            renderpos_index = modelFindNodeMtxIndex(rodata->unk04, 0x200);
        }

        render_pos = &model->render_pos[renderpos_index];

        c_entry_index = ((rw_index_sel2 - rw_index_sel) + rodata->NumChildren) % rodata->NumChildren;

        if ((c_entry_index >= 2) && (c_entry_index < 7))
        {
            if (c_entry_index < 4)
            {
                c_entry_index = ((c_entry_index / 2) + rw_index_sel + rodata->NumChildren) % rodata->NumChildren;
            }
            else
            {
                c_entry_index = ((rw_index_sel - ((8 - c_entry_index) / 2)) + rodata->NumChildren) % rodata->NumChildren;
                swap_order = 0;
            }
        }
        else if ((c_entry_index >= 0xA) && (c_entry_index < 0xF))
        {
            if (c_entry_index >= 0xD)
            {
                c_entry_index = ((rw_index_sel - ((0x10 - c_entry_index) / 2)) + rodata->NumChildren) % rodata->NumChildren;
            }
            else
            {
                c_entry_index = (((c_entry_index - 8) / 2) + rw_index_sel + rodata->NumChildren) % rodata->NumChildren;
                swap_order = 0;
            }
        }
        else
        {
            if ((c_entry_index >= 7) && (c_entry_index < 0xA))
            {
                swap_order = 0;
            }
            c_entry_index = rw_index_sel;
        }

        entry_04 = c_entry->unk04;

        gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_COL1, osVirtualToPhysical(rodata->BaseAddr));

        for (c_entry_count = c_entry->NumEntries; c_entry_count > 0; c_entry_count--)
        {
            switch (*entry_04)
            {
                case MODELNODE_CHILD_VTX:
                    {
                        struct ModelRoData_Child_Vtx* child_vtx = ((struct ModelRoData_Child_Vtx*)entry_04);
#if defined(LEFTOVERDEBUG)
                        if (vtxallocator == NULL)
                        {
                            osSyncPrintf("dotube: no vtx allocator!\n");
                            return_null();
                        }
#endif
                        vtx1 = vtxallocator(2);
                        vtx2 = &vtx1[1];

                        *vtx1 = rodata->Vertices[child_vtx->VtxIndex];
                        *vtx2 = rodata->Vertices[child_vtx->VtxIndex+1];

                        if (rodata->unk04 != NULL)
                        {
                            c_entry2 = &rodata->Children[c_entry_index];
                            entry2_04 = c_entry2->unk04;

                            for (c_entry2_count = c_entry2->NumEntries; c_entry2_count > 0; c_entry2_count--)
                            {
                                struct ModelRoData_Child_Vtx* entry2_04_child = ((struct ModelRoData_Child_Vtx*)entry2_04);
                                if (entry2_04_child->Type == (u8) 1) {
                                    vtx_10   = &rodata->Vertices[entry2_04_child->VtxIndex];
                                    vtx_10_2 = vtx_10+1;

                                    vtx1->coord.AsArray[0] = vtx_10->coord.AsArray[0];
                                    vtx1->coord.AsArray[1] = vtx_10->coord.AsArray[1];
                                    vtx1->coord.AsArray[2] = vtx_10->coord.AsArray[2];

                                    vtx2->coord.AsArray[0] = vtx_10_2->coord.AsArray[0];
                                    vtx2->coord.AsArray[1] = vtx_10_2->coord.AsArray[1];
                                    vtx2->coord.AsArray[2] = vtx_10_2->coord.AsArray[2];
                                    break;
                                }

                                switch (*entry2_04)
                                {
                                    case MODELNODE_CHILD_VTX:
                                        entry2_04 += sizeof(struct ModelRoData_Child_Vtx);
                                        break;
                                    case MODELNODE_CHILD_IMAGE:
                                        entry2_04 += sizeof(struct ModelRoData_Child_Image);
                                        break;
                                    case MODELNODE_CHILD_TRI:
                                        entry2_04 += sizeof(struct ModelRoData_Child_Tri);
                                        break;
                                }
                            }

                        }
                        else
                        {
                            c_entry2 = &rodata2->Children[c_entry_index];
                            entry2_04 = c_entry2->unk04;

                            for (c_entry2_count = c_entry2->NumEntries; c_entry2_count > 0; c_entry2_count--)
                            {
                                struct ModelRoData_Child_Vtx* entry2_04_child = ((struct ModelRoData_Child_Vtx*)entry2_04);
                                if (entry2_04_child->Type == (u8) 1)
                                {
                                    vtx_10   = &rodata2->Vertices[entry2_04_child->VtxIndex];
                                    vtx_10_2 = vtx_10 + 1;
                                    if (swap_order != 0)
                                    {
#if defined(LEFTOVERDEBUG)
                                        if (vtx_10->coord.AsArray);
#endif
                                        vtx1->coord.AsArray[0] = vtx_10_2->coord.AsArray[0];
                                        vtx1->coord.AsArray[1] = vtx_10_2->coord.AsArray[1];
                                        vtx1->coord.AsArray[2] = vtx_10_2->coord.AsArray[2];

                                        vtx2->coord.AsArray[0] = vtx_10->coord.AsArray[0];
                                        vtx2->coord.AsArray[1] = vtx_10->coord.AsArray[1];
                                        vtx2->coord.AsArray[2] = vtx_10->coord.AsArray[2];
                                    }
                                    else
                                    {
                                        vtx1->coord.AsArray[0] = vtx_10->coord.AsArray[0];
                                        vtx1->coord.AsArray[1] = vtx_10->coord.AsArray[1];
                                        vtx1->coord.AsArray[2] = vtx_10->coord.AsArray[2];

                                        vtx2->coord.AsArray[0] = vtx_10_2->coord.AsArray[0];
                                        vtx2->coord.AsArray[1] = vtx_10_2->coord.AsArray[1];
                                        vtx2->coord.AsArray[2] = vtx_10_2->coord.AsArray[2];
                                    }
                                    break;
                                }

                                switch (*entry2_04)
                                {
                                    case MODELNODE_CHILD_VTX:
                                        entry2_04 += sizeof(struct ModelRoData_Child_Vtx);
                                        break;

                                    case MODELNODE_CHILD_IMAGE:
                                        entry2_04 += sizeof(struct ModelRoData_Child_Image);
                                        break;

                                    case MODELNODE_CHILD_TRI:
                                        entry2_04 += sizeof(struct ModelRoData_Child_Tri);
                                        break;
                                }
                            }
                        }

                        gSPMatrix(renderdata->gdl++, osVirtualToPhysical(render_pos), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                        gSPVertex(renderdata->gdl++, osVirtualToPhysical(vtx1), 2, 0);
                        gSPMatrix(renderdata->gdl++, osVirtualToPhysical(render_pos2), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                        gSPVertex(renderdata->gdl++, osVirtualToPhysical(&rodata->Vertices[child_vtx->VtxIndex] + 2), (u32)(child_vtx->unk01 - 2), 2);

                        entry_04 += sizeof(struct ModelRoData_Child_Vtx);

                        break;
                    }

                case MODELNODE_CHILD_IMAGE:
                    {
                        struct ModelRoData_Child_Image* child_image = (struct ModelRoData_Child_Image*)entry_04;
                        if (child_image->ImageIndex != 0xFF)
                        {
                            sub_GAME_7F073038(renderdata, &rodata->Images[child_image->ImageIndex], 1);
                            entry_04 += sizeof(struct ModelRoData_Child_Image);
                        }
                        else
                        {
                            sub_GAME_7F073038(renderdata, NULL, 1);
                            entry_04 += sizeof(struct ModelRoData_Child_Image);
                        }
                        break;
                    }

                case MODELNODE_CHILD_TRI:
                    {
                        struct ModelRoData_Child_Tri* child_tri = (struct ModelRoData_Child_Tri*)entry_04;
                        gSP1Triangle(renderdata->gdl++, child_tri->VtxIndex1, child_tri->VtxIndex2, child_tri->VtxIndex3, 0);
                        entry_04 += sizeof(struct ModelRoData_Child_Tri);
                        break;
                    }
            }
        }
    }
}


void sub_GAME_7F0737EC(s32 param_1,struct Model *param_2, struct ModelNode *param_3)
{
    return;
}


void sub_GAME_7F0737FC(s32 param_1,struct Model *param_2,struct ModelNode *param_3)
{
    return;
}


// PD: modelRenderNodeChrGunfire
void dogfnegx(ModelRenderData *renderdata, Model *model, ModelNode *node)
{
    u32 unused[3];
    f32 negspc0;
    ModelRoData_GunfireRecord *rodata = &node->Data->Gunfire;
    union ModelRwData *rwdata = modelGetNodeRwData(model, node);
    sImageTableEntry *tconfig;
    f32 spf0;
    f32 spec;
    coord3d spe0;
    f32 spdc;
    f32 spd8;
    f32 spd4;
    f32 spd0;
    f32 spcc;
    f32 spc8;
    f32 spc4;
    f32 spc0;
    f32 spbc;
    f32 negspcc;
    f32 negspc8;
    f32 scale;
    Mtxf *mtx;
    f32 tmp;
    coord3d sp9c;
    coord3d sp90;
    Vertex vtxtemplate = D_800363E0;
    Vertex *vertices;
    f32 distance;

    if ((renderdata->flags & 2) && rwdata->Gunfire.visible)
    {
        s32 index = modelFindNodeMtxIndex(node, 0);
        mtx = &model->render_pos[index].pos;

        spe0.x = -(rodata->Offset.f[0] * mtx->m[0][0] + rodata->Offset.f[1] * mtx->m[1][0] + rodata->Offset.f[2] * mtx->m[2][0] + mtx->m[3][0]);
        spe0.y = -(rodata->Offset.f[0] * mtx->m[0][1] + rodata->Offset.f[1] * mtx->m[1][1] + rodata->Offset.f[2] * mtx->m[2][1] + mtx->m[3][1]);
        spe0.z = -(rodata->Offset.f[0] * mtx->m[0][2] + rodata->Offset.f[1] * mtx->m[1][2] + rodata->Offset.f[2] * mtx->m[2][2] + mtx->m[3][2]);

        distance = sqrtf(spe0.f[0] * spe0.f[0] + spe0.f[1] * spe0.f[1] + spe0.f[2] * spe0.f[2]);

        if (distance > 0)
        {
            f32 tmp = 1 / (model->scale * distance);
            spe0.f[0] *= tmp;
            spe0.f[1] *= tmp;
            spe0.f[2] *= tmp;
        }
        else
        {
            spe0.f[0] = 0;
            spe0.f[1] = 0;
            spe0.f[2] = 1 / model->scale;
        }

        spec = acosf(spe0.f[0] * mtx->m[1][0] + spe0.f[1] * mtx->m[1][1] + spe0.f[2] * mtx->m[1][2]);
        spf0 = acosf(-(spe0.f[0] * mtx->m[2][0] + spe0.f[1] * mtx->m[2][1] + spe0.f[2] * mtx->m[2][2]) / sinf(spec));

        tmp = -(spe0.f[0] * mtx->m[0][0] + spe0.f[1] * mtx->m[0][1] + spe0.f[2] * mtx->m[0][2]);

        if (tmp < 0)
        {
            spf0 = M_TAU_F - spf0;
        }

        spdc = cosf(spf0);
        spd8 = sinf(spf0);
        spd4 = cosf(spec);
        spd0 = sinf(spec);

        scale = 0.75f + (randomGetNext() % 128) * (1.0f / 256.0f); // 0.75 to 1.25

        sp9c.f[0] = rodata->Size.f[0] * scale;
        sp9c.f[1] = rodata->Size.f[1] * scale;
        sp9c.f[2] = rodata->Size.f[2] * scale;

        spcc = sp9c.f[0] * spdc * 0.5f;
        spc8 = sp9c.f[2] * spd8 * 0.5f;
        spc4 = sp9c.f[1] * spd0 * 0.5f;

        spc0 = sp9c.f[0] * spd4 * spd8 * 0.5f;
        spbc = sp9c.f[2] * spd4 * spdc * 0.5f;

        negspcc = -spcc;
        negspc8 = -spc8;
        negspc0 = -spc0;

        sp90.f[0] = rodata->Offset.f[0] - sp9c.f[0] * 0.5f;
        sp90.f[1] = rodata->Offset.f[1];
        sp90.f[2] = rodata->Offset.f[2];

#if defined (LEFTOVERDEBUG)
        if (vtxallocator == NULL) {
            osSyncPrintf("dogfnegx: no vtx allocator!\n");
            return_null();
        }
#endif

        vertices = vtxallocator(4);

        vertices[0] = vtxtemplate;
        vertices[1] = vtxtemplate;
        vertices[2] = vtxtemplate;
        vertices[3] = vtxtemplate;

        vertices[0].coord.x = sp90.f[0] + negspcc + negspc0;
        vertices[0].coord.y = sp90.f[1] - spc4;
        vertices[0].coord.z = sp90.f[2] - negspc8 + -spbc;
        vertices[1].coord.x = sp90.f[0] + negspcc - negspc0;
        vertices[1].coord.y = sp90.f[1] + spc4;
        vertices[1].coord.z = sp90.f[2] - negspc8 - -spbc;
        vertices[2].coord.x = sp90.f[0] - negspcc - negspc0;
        vertices[2].coord.y = sp90.f[1] + spc4;
        vertices[2].coord.z = sp90.f[2] + negspc8 - -spbc;
        vertices[3].coord.x = sp90.f[0] - negspcc + negspc0;
        vertices[3].coord.y = sp90.f[1] - spc4;
        vertices[3].coord.z = sp90.f[2] + negspc8 + -spbc;

        gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_COL1, osVirtualToPhysical(rodata->BaseAddr));

        if (rodata->Image)
        {
            s32 centre;
            u16 sp62;
            s32 sp5c;
            s32 sp58;

            tconfig = rodata->Image;

            sp62 = (randomGetNext() * 1024) & 0xffff;
            sp5c = (coss(sp62) * tconfig->width * 0xb5) >> 18;
            sp58 = (sins(sp62) * tconfig->width * 0xb5) >> 18;

            centre = tconfig->width << 4;

            vertices[0].s = centre - sp5c;
            vertices[0].t = centre - sp58;
            vertices[1].s = centre + sp58;
            vertices[1].t = centre - sp5c;
            vertices[2].s = centre + sp5c;
            vertices[2].t = centre + sp58;
            vertices[3].s = centre - sp58;
            vertices[3].t = centre + sp5c;

            sub_GAME_7F073038(renderdata, tconfig, 4);
        }
        else
        {
            sub_GAME_7F073038(renderdata, NULL, 1);
        }

        gSPSetGeometryMode(renderdata->gdl++, G_CULL_BACK);
        gSPMatrix(renderdata->gdl++, osVirtualToPhysical(mtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPVertex(renderdata->gdl++, osVirtualToPhysical(vertices), 4, 0);
        if (1);
        gDPTri2(renderdata->gdl++, 0, 1, 2, 2, 3, 0);
    }
}


void sub_GAME_7F073FC8(s32 arg0)
{
    D_800363F0 = arg0;
}



#ifdef NONMATCHING
void doshadow(void) {

}
#else
#ifndef VERSION_EU
//D:80054A94
const char aDoshadowNoVtxAllocator[] = "doshadow: no vtx allocator!\n";

void _asmpp_func38(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
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
}
#endif
#ifdef VERSION_EU

void _asmpp_func42(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
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
}
#endif
#endif





void sub_GAME_7F074514(s32 param_1,struct Model *param_2,struct ModelNode *param_3)
{
    return;
}


void sub_GAME_7F074524(Gfx *param_1,struct Model *param_2, struct ModelNode *param_3)
{
    return;
}








void sub_GAME_7F074534(ModelRenderData* data, Model* model, ModelNode* node) {
    u32 id = node->Opcode & 0xFF;
    switch (id) {
    case MODELNODE_OPCODE_LOD:
        modelApplyDistanceRelations(model, node);
        return;
    case MODELNODE_OPCODE_SWITCH:
        modelApplyToggleRelations(model, node);
        return;
    case MODELNODE_OPCODE_HEAD:
        modelApplyHeadRelations(model, node);
        return;
    case MODELNODE_OPCODE_BSP:
        modelApplyReorderRelations(model, node);
        return;
    case MODELNODE_OPCODE_OP11:
        sub_GAME_7F0737FC(data, model, node);
        return;
    case MODELNODE_OPCODE_GUNFIRE:
        dogfnegx(data, model, node);
        return;
    case MODELNODE_OPCODE_SHADOW:
        doshadow(data, model, node);
        return;
    case MODELNODE_OPCODE_BBOX:
        sub_GAME_7F074514(data, model, node);
        return;
    case MODELNODE_OPCODE_OP17:
        sub_GAME_7F074524(data, model, node);
        return;
    case MODELNODE_OPCODE_DL:
        modelRenderNodeGundl(data, node);
        return;
    case MODELNODE_OPCODE_DLCOLLISION:
        modelRenderNodeDl(data, model, node);
        return;
    case MODELNODE_OPCODE_OP20:
        sub_GAME_7F072C10(data, model, node);
        return;
    case MODELNODE_OPCODE_DLPRIMARY:
        dorottex(data, node);
        return;
    case MODELNODE_OPCODE_OP05:
        sub_GAME_7F07306C(data, model, node);
        return;
    case MODELNODE_OPCODE_OP07:
        dotube(data, model, node);
        return;
    case MODELNODE_OPCODE_OP06:
        sub_GAME_7F0737EC(data,model,node);
        return;
    case MODELNODE_OPCODE_HEADER:
    case MODELNODE_OPCODE_GROUP:
    case MODELNODE_OPCODE_OP03:
    case MODELNODE_OPCODE_OP14:
    case MODELNODE_OPCODE_INTERLINK:
    case MODELNODE_OPCODE_OP16:
    default:
        return;
    }
}





#ifdef NONMATCHING
void subdraw(void) {

}
#else
#ifndef VERSION_EU
//D:80054AB4
const char aSubdrawNoGfxlist[] = "subdraw: no gfxlist!\n";
//D:80054ACC
const char aSubdrawObjectNotInitialised0xX[] = "subdraw: object not initialised! (0x%X)\n";

void _asmpp_func46(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
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

void _asmpp_func47(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
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


void sub_GAME_7F074790(ModelRenderData* arg0, Model* arg1)
{
    subcalcpos(arg1);
    subcalcmatrices(arg0, arg1);
    subdraw((s32) arg0, arg1);
}


#ifdef NONMATCHING
void sub_GAME_7F0747D0(void) {

}
#else

void _asmpp_func48(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
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
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
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
void sub_GAME_7F074C68(void) {

}
#else

void _asmpp_func51(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
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
void sub_GAME_7F074CAC(void) {

}
#else

void _asmpp_func52(void) {


*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
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
}
#endif





#ifdef NONMATCHING
void sub_GAME_7F0752FC(void) {

}
#else

void _asmpp_func56(void) {






























*(volatile float*)4 = -4.214060832381731e+19f;


switch (*(volatile int*)4) { case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20: case 21: case 22: ; }








*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
*(volatile int*)4 = 0;

*(volatile int*)4 = 0;
*(volatile int*)4 = 0;
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
}
#endif





#ifdef NONMATCHING
void sub_GAME_7F07549C(s32 arg0, f32 *arg1, f32 *arg2, ModelNode *arg3)
{
    //uses sh vs sw
    arg3->Opcode = 0;
    sub_GAME_7F0752FC(arg0, arg1, arg2, arg3);
}
#else

void _asmpp_func58(void) {


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
 * Address 7F0754BC.
 * Copy animation from ROM to RAM
*/
s32 loadAnimationFrame(ModelAnimation* anim, s32 frame, ModelSkeleton* unused)
{
    s32 ret;
    s32 source;
    s32 frameSize;
    u32 dest;
    u32 size;

    ret = 0;
    frameSize = anim->unk0E >> 3; // divide by 8

    if (anim->address & 0x80000000) // If animation's address is in RAM
    {
        // Load that frame from RAM
        ret = anim->address + (frame * frameSize);
    }
    else if (D_80036414 != NULL) // should never be NULL after sub_GAME_7F0009E0 is called
    {
        // Get dest from this D_80036414 which points to an array. Align to 16 bytes.
        dest = ((u32) (D_80036414->animBufferPtr2 + 15) >> 4) * 16;
        ret = dest;

        // Get source of this animation in ROM with the offset of the frame we'll load
        source = anim->address + (frame * frameSize);
        if (source & 1)
        {
            source--;
            frameSize++;
            ret++;
        }

        // Size of frame but 16-bytes aligned. Observed to be 80 bytes. Might differ for non-guards.
        size = ((u32) (frameSize + 15) >> 4) * 16;

        // This copies one animation frame from ROM to the destination in RAM
        romCopy((void* ) dest, (void* ) source, size);

        // Increment this which serves nothing
        D_80036414->uselessPointer += 1;

        // Set this to point to the end of the copied frame
        // This allows to copy another frame after this one
        D_80036414->animBufferPtr2 = dest + size;
    }
    return ret;
}


/**
 * Address 7F0755B0.
*/
void sub_GAME_7F0755B0(void)
{
    if (D_80036414 != NULL) // should never be NULL after sub_GAME_7F0009E0 is called
    {
        // Reset the pointer to point to the start of the array
        D_80036414->animBufferPtr2 = D_80036414->animBufferPtr1;
        D_80036414->uselessPointer = NULL;
    }
}


#define PROMOTE(var) \
    if (var) \
        var = (void *)((u32)var + diff)

void modelPromoteNodeOffsetsToPointers(ModelNode *node, u32 vma, u32 fileramaddr)
{
    s32 diff = fileramaddr - vma;
    s32 i;

    while (node)
    {
        u32 type = node->Opcode & 0xff;

        PROMOTE(node->Data);
        PROMOTE(node->Parent);
        PROMOTE(node->Next);
        PROMOTE(node->Prev);
        PROMOTE(node->Child);

        switch (type)
        {
            case MODELNODE_OPCODE_HEADER:
                {
                    ModelRoData_HeaderRecord* rodata = &node->Data->Header;
                    PROMOTE(rodata->FirstGroup);
                    break;
                }

            case MODELNODE_OPCODE_GROUP:
                {
                    ModelRoData_GroupRecord* rodata = &node->Data->Group;
                    PROMOTE(rodata->ChildGroup);
                    break;
                }

            case MODELNODE_OPCODE_OP03:
                {
                    ModelRoData_GroupRecord* rodata = &node->Data->Group;
                    PROMOTE(rodata->ChildGroup);
                    break;
                }

            case MODELNODE_OPCODE_DL:
                {
                    ModelRoData_DisplayListRecord* rodata = &node->Data->DisplayList;
                    PROMOTE(rodata->Vertices);
                    rodata->BaseAddr = (void *)fileramaddr;
                    break;
                }

            case MODELNODE_OPCODE_DLCOLLISION:
                {
                    ModelRoData_DisplayList_CollisionRecord* rodata = &node->Data->DisplayListCollisions;
                    PROMOTE(rodata->Vertices);
                    PROMOTE(rodata->CollisionVertices);
                    PROMOTE(rodata->PointUsage);
                    for (i = 0; i < rodata->numCollisionVertices; i++)
                    {
                        PROMOTE(rodata->CollisionVertices[i].LinkedTo);
                    }
                    rodata->BaseAddr = (void *)fileramaddr;
                    break;
                }

            case MODELNODE_OPCODE_OP20:
                {
                    ModelRoData_HeaderRecord* rodata = &node->Data->Header;
                    PROMOTE(rodata->FirstGroup);
                    break;
                }

            case MODELNODE_OPCODE_OP05:
                {
                    ModelRoData_Op05Record* rodata = &node->Data->Op05;

                    // shared with op07
                    PROMOTE(rodata->Children);
                    PROMOTE(rodata->Vertices);
                    PROMOTE(rodata->Images);
                    for (i = 0; i < rodata->NumChildren; i++)
                    {
                        PROMOTE(rodata->Children[i].unk04);
                    }

                    rodata->BaseAddr = (void *)fileramaddr;
                    break;
                }

            case MODELNODE_OPCODE_OP07:
                {
                    ModelRoData_Op07Record* rodata = &node->Data->Op07;
                    PROMOTE(rodata->unk00);
                    PROMOTE(rodata->unk04);

                    // shared with op05
                    PROMOTE(rodata->Children);
                    PROMOTE(rodata->Vertices);
                    PROMOTE(rodata->Images);
                    for (i = 0; i < rodata->NumChildren; i++)
                    {
                        PROMOTE(rodata->Children[i].unk04);
                    }

                    rodata->BaseAddr = (void *)fileramaddr;
                    break;
                }

            case MODELNODE_OPCODE_OP06:
                {
                    ModelRoData_Op06Record* rodata = &node->Data->Op06;
                    rodata->BaseAddr = (void *)fileramaddr;
                    break;
                }

            case MODELNODE_OPCODE_LOD:
                {
                    ModelRoData_LODRecord* rodata = &node->Data->LOD;
                    PROMOTE(rodata->Affects);
                    node->Child = rodata->Affects;
                    break;
                }

            case MODELNODE_OPCODE_SWITCH:
                {
                    ModelRoData_SwitchRecord* rodata = &node->Data->Switch;
                    PROMOTE(rodata->Controls);
                    break;
                }

            case MODELNODE_OPCODE_BSP:
                {
                    ModelRoData_BSPRecord* rodata = &node->Data->BSP;
                    PROMOTE(rodata->leftChild);
                    PROMOTE(rodata->rightChild);
                    break;
                }

            case MODELNODE_OPCODE_OP17:
                {
                    ModelRoData_GroupRecord* rodata = &node->Data->Group;
                    PROMOTE(rodata->ChildGroup);
                    break;
                }

            case MODELNODE_OPCODE_OP11:
                {
                    ModelRoData_Op11Record* rodata = &node->Data->Op11;
                    PROMOTE(rodata->unk0c[15]);
                    rodata->BaseAddr = (void *)fileramaddr;
                    break;
                }

            case MODELNODE_OPCODE_GUNFIRE:
                {
                    ModelRoData_GunfireRecord* rodata = &node->Data->Gunfire;
                    PROMOTE(rodata->Image);
                    rodata->BaseAddr = (void *)fileramaddr;
                    break;
                }

            case MODELNODE_OPCODE_SHADOW:
                {
                    ModelRoData_ShadowRecord* rodata = &node->Data->Shadow;
                    PROMOTE(rodata->image);
                    PROMOTE(rodata->Header);
                    rodata->BaseAddr = (void *)fileramaddr;
                    break;
                }

            case MODELNODE_OPCODE_DLPRIMARY:
                {
                    ModelRoData_DisplayListPrimaryRecord* rodata = &node->Data->DisplayListPrimary;
                    PROMOTE(rodata->Vertices);
                    rodata->BaseAddr = (void *)fileramaddr;
                    break;
                }

            default:
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

/**
 * Address 7F075A90.
*/
void sub_GAME_7F075A90(ModelFileHeader *header, s32 vma, u32 addr) {
    s32 diff = addr - vma;
    s32 i;

    for(i = 0;i < header->numSwitches;i++)
    {
        PROMOTE(header->Switches[i]);
    }
    modelPromoteNodeOffsetsToPointers(header->RootNode, vma, addr);
}

/**
 * Address 7F075B08.
*/
void REMOVED_sub_GAME_7F075B08(s32 param_1,s32 param_2,s32 param_3,s32 param_4)
{
    return;
}


s32 modelCalculateRwDataIndexes(ModelNode *basenode)
{
    u16 len = 0;
    ModelNode *node = basenode;
    union ModelRoData *rodata;

    while (node)
    {
        u32 type = node->Opcode & 0xff;

        switch (type)
        {
            case MODELNODE_OPCODE_HEADER:
                if (1)
                {
                    ModelRoData_HeaderRecord *rodata = &node->Data->Header;
                    rodata->RwDataIndex = len;
                    len += sizeof(struct ModelRwData_HeaderRecord) / 4;
                    break;
                }
            case MODELNODE_OPCODE_OP07:
                if (1)
                {
                    ModelRoData_Op07Record *rodata = &node->Data->Op07;
                    rodata->RwDataIndex = len;
                    len += sizeof(struct ModelRwData_Op07Record) / 4;
                    break;
                }
            case MODELNODE_OPCODE_LOD:
                if (1)
                {
                    ModelRoData_LODRecord *rodata = &node->Data->LOD;
                    rodata->RwDataIndex = len;
                    len += sizeof(struct ModelRwData_LODRecord) / 4;
                    node->Child = rodata->Affects;
                    break;
                }
            case MODELNODE_OPCODE_SWITCH:
                if (1)
                {
                    ModelRoData_SwitchRecord *rodata = &node->Data->Switch;
                    rodata->RwDataIndex = len;
                    len += sizeof(struct ModelRwData_SwitchRecord) / 4;
                    node->Child = rodata->Controls;
                    break;
                }
            case MODELNODE_OPCODE_HEAD:
                if (1)
                {
                    ModelRoData_HeadPlaceholderRecord *rodata = &node->Data->HeadPlaceholder;
                    rodata->RwDataIndex = len;
                    len += sizeof(struct ModelRwData_HeadPlaceholderRecord) / 4;
                    node->Child = NULL;
                    break;
                }
            case MODELNODE_OPCODE_BSP:
                if (1)
                {
                    ModelRoData_BSPRecord *rodata = &node->Data->BSP;
                    rodata->RwDataIndex = len;
                    len += sizeof(struct ModelRwData_BSPRecord) / 4;
                    modelApplyReorderRelationsByArg(node, FALSE);
                    break;
                }
            case MODELNODE_OPCODE_OP11:
                if (1)
                {
                    ModelRoData_Op11Record *rodata = &node->Data->Op11;
                    rodata->RwDataIndex = len;
                    len += sizeof(struct ModelRwData_Op11Record) / 4;
                    break;
                }
            case MODELNODE_OPCODE_GUNFIRE:
                if (1)
                {
                    ModelRoData_GunfireRecord *rodata = &node->Data->Gunfire;
                    rodata->RwDataIndex = len;
                    len += sizeof(struct ModelRwData_GunfireRecord) / 4;
                    break;
                }
            case MODELNODE_OPCODE_DLCOLLISION:
                if (1)
                {
                    ModelRoData_DisplayList_CollisionRecord *rodata = &node->Data->DisplayListCollisions;
                    rodata->RwDataIndex = len;
                    len += sizeof(struct ModelRwData_DisplayList_CollisionRecord) / 4;
                    break;
                }
            default:
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
                if (node == basenode->Parent)
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

    return len;
}


void modelCalculateRwDataLen(struct ModelFileHeader *objheader)
{
  #if defined(LEFTOVERDEBUG)
    objheader->isLoaded = 1;
  #endif
    objheader->numRecords = modelCalculateRwDataIndexes(objheader->RootNode);
}


void modelInitRwData(Model *model, ModelNode *startnode)
{
    ModelNode *node = startnode;

    while (node)
    {
        u32 type = node->Opcode & 0xFF;

        switch (type)
        {
            case MODELNODE_OPCODE_HEADER:
                if (1)
                {
                    ModelRwData_HeaderRecord* rwdata = &modelGetNodeRwData(model, node)->Header;

                    rwdata->unk00 = 0;
                    rwdata->ground = 0;
                    rwdata->pos.x = 0;
                    rwdata->pos.y = 0;
                    rwdata->pos.z = 0;
                    rwdata->unk14 = 0;
                    rwdata->unk18 = 0;
                    rwdata->unk1c = 0;

                    rwdata->unk01 = 0;
                    rwdata->unk34.x = 0;
                    rwdata->unk34.y = 0;
                    rwdata->unk34.z = 0;
                    rwdata->unk30 = 0;
                    rwdata->unk24.x = 0;
                    rwdata->unk24.y = 0;
                    rwdata->unk24.z = 0;
                    rwdata->unk20 = 0;

                    rwdata->unk02 = 0;
                    rwdata->unk4c.x = 0;
                    rwdata->unk4c.y = 0;
                    rwdata->unk4c.z = 0;
                    rwdata->unk40.x = 0;
                    rwdata->unk40.y = 0;
                    rwdata->unk40.z = 0;
                    rwdata->unk5c = 0;
                    break;
                }

            case MODELNODE_OPCODE_OP07:
                if (1)
                {
                    ModelRwData_Op07Record* rwdata = &modelGetNodeRwData(model, node)->Op07;
                    rwdata->index = 0;
                    break;
                }


            case MODELNODE_OPCODE_LOD:
                if (1)
                {
                    ModelRoData_LODRecord* rodata = &node->Data->LOD;
                    ModelRwData_LODRecord* rwdata = &modelGetNodeRwData(model, node)->LOD;
                    rwdata->visible = FALSE;
                    node->Child = rodata->Affects;
                    break;
                }

            case MODELNODE_OPCODE_SWITCH:
                if (1)
                {
                    ModelRoData_SwitchRecord* rodata = &node->Data->Switch;
                    ModelRwData_SwitchRecord* rwdata = &modelGetNodeRwData(model, node)->Switch;
                    rwdata->visible = TRUE;
                    node->Child = rodata->Controls;
                    break;
                }

            case MODELNODE_OPCODE_HEAD:
                if (1)
                {
                    ModelRwData_HeadPlaceholderRecord* rwdata = &modelGetNodeRwData(model, node)->HeadPlaceholder;
                    rwdata->ModelFileHeader = NULL;
                    rwdata->RwDatas = NULL;
                    break;
                }

            case MODELNODE_OPCODE_BSP:
                if (1)
                {
                    ModelRwData_BSPRecord* rwdata = &modelGetNodeRwData(model, node)->BSP;
                    rwdata->visible = FALSE;
                    modelApplyReorderRelations(model, node);
                    break;
                }

            case MODELNODE_OPCODE_OP11:
                if (1)
                {
                    ModelRwData_Op11Record* rwdata = &modelGetNodeRwData(model, node)->Op11;
                    rwdata->unk00 = FALSE;
                    break;
                }

            case MODELNODE_OPCODE_GUNFIRE:
                if (1)
                {
                    ModelRwData_GunfireRecord* rwdata = &modelGetNodeRwData(model, node)->Gunfire;
                    rwdata->visible = FALSE;
                    break;
                }

            case MODELNODE_OPCODE_DLCOLLISION:
                if (1)
                {
                    ModelRoData_DisplayList_CollisionRecord* rodata = &node->Data->DisplayListCollisions;
                    ModelRwData_DisplayList_CollisionRecord* rwdata = &modelGetNodeRwData(model, node)->DisplayListCollisions;
                    rwdata->Vertices = rodata->Vertices;
                    rwdata->gdl = rodata->Primary;
                    break;
                }

            default:
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
                if (node == startnode->Parent)
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
}


void modelInit(struct Model *objinst, struct ModelFileHeader *header, u32 *data)
{
  objinst->obj = header;
  objinst->datas = data;
  objinst->Type = -1;
  objinst->attachedto = NULL;
  objinst->attachedto_objinst = NULL;
  objinst->scale = 1.0;
  modelInitRwData(objinst, header->RootNode);
}


// PD: animInit
void animInit(struct Model *objinst, struct ModelFileHeader *header, u32 *data)
{
    modelInit(objinst, header, data);
    objinst->anim = NULL;
    objinst->anim2 = NULL;
    objinst->animlooping = 0;
    objinst->animflipfunc = 0;
    objinst->unk9c = 0;
    objinst->unka0 = 0;
    objinst->unk2c = 0.0f;
    objinst->timespeed = 0.0f;
    objinst->unk5c = 0.0f;
    objinst->unk7c = 0.0f;
    objinst->unk84 = 0.0f;
    objinst->unk88 = 0.0f;
    objinst->unkb0 = 0.0f;
    objinst->speed = 1.0f;
    objinst->speed2 = 1.0f;
    objinst->playspeed = 1.0f;
    objinst->unkb8 = 1.0f;
    objinst->endframe = -1.0f;
    objinst->unk6c = -1.0f;
}


// PD: model00023108
void modelAttachPart(Model *pmodel, ModelFileHeader *pmodeldef, ModelNode *pnode, ModelFileHeader *cmodeldef)
{
    ModelRwData_HeadPlaceholderRecord *rwdata = modelGetNodeRwData(pmodel, pnode);
    ModelNode *node;

    rwdata->ModelFileHeader = cmodeldef;
    rwdata->RwDatas = &pmodel->datas[pmodeldef->numRecords];

    pnode->Child = cmodeldef->RootNode;

    node = pnode->Child;

    while (node)
    {
        node->Parent = pnode;
        node = node->Next;
    }

    pmodeldef->numRecords += modelCalculateRwDataIndexes(pnode->Child);
}


/**
 * This function can be called repeatedly to iterate a model's display lists.
 *
 * On the first call, the value passed as nodeptr should point to a NULL value.
 * Each time the function is called, it will update *gdlptr to point to the next
 * display list, and will update *nodeptr to point to the current node. On
 * subsequent calls, the same values should be passed as nodeptr and gdlptr so
 * the function can continue correctly.
 *
 * Note that some node types support multiple display lists, so the function
 * may return the same node while it iterates the display lists for that node.
 */
void modelIterateDisplayLists(ModelFileHeader *fileheader, ModelNode **nodeptr, Gfx **gdlptr)
{
    ModelNode *node = *nodeptr;
    union ModelRoData *rodata;
    Gfx *gdl = NULL;

    if (node == NULL)
    {
        node = fileheader->RootNode;
    }

    while (node)
    {
        u32 type = node->Opcode & 0xff;

        switch (type)
        {
            case MODELNODE_OPCODE_DL:
                rodata = node->Data;

                if (node != *nodeptr)
                {
                    gdl = rodata->DisplayList.Primary;
                }
                else if (rodata->DisplayList.Secondary != *gdlptr)
                {
                    gdl = rodata->DisplayList.Secondary;
                }
                break;

            case MODELNODE_OPCODE_DLCOLLISION:
                rodata = node->Data;

                if (node != *nodeptr)
                {
                    gdl = rodata->DisplayListCollisions.Primary;
                }
                else if (rodata->DisplayListCollisions.Secondary != *gdlptr)
                {
                    gdl = rodata->DisplayListCollisions.Secondary;
                }
                break;

            case MODELNODE_OPCODE_DLPRIMARY:
                rodata = node->Data;

                if (node != *nodeptr)
                {
                    gdl = rodata->DisplayListPrimary.Primary;
                }
                break;

            case MODELNODE_OPCODE_LOD:
                rodata = node->Data;
                node->Child = rodata->LOD.Affects;
                break;

            case MODELNODE_OPCODE_SWITCH:
                rodata = node->Data;
                node->Child = rodata->Switch.Controls;
                break;

            case MODELNODE_OPCODE_BSP:
                modelApplyReorderRelationsByArg(node, TRUE);
                break;
        }

        if (gdl) { break; }

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

    *gdlptr = gdl;
    *nodeptr = node;
}


void modelNodeReplaceGdl(u32 arg0, ModelNode *node, Gfx *find, Gfx *replacement)
{
    union ModelRoData *rodata;
    u32 type = node->Opcode & 0xff;

    switch (type) {
        case MODELNODE_OPCODE_DL:
            rodata = node->Data;

            if (rodata->DisplayList.Primary == find)
            {
                rodata->DisplayList.Primary = replacement;
                return;
            }

            if (rodata->DisplayList.Secondary == find)
            {
                rodata->DisplayList.Secondary = replacement;
                return;
            }
            break;

        case MODELNODE_OPCODE_DLCOLLISION:
            rodata = node->Data;

            if (rodata->DisplayListCollisions.Primary == find)
            {
                rodata->DisplayListCollisions.Primary = replacement;
                return;
            }

            if (rodata->DisplayListCollisions.Secondary == find)
            {
                rodata->DisplayListCollisions.Secondary = replacement;
                return;
            }
            break;

        case MODELNODE_OPCODE_DLPRIMARY:
            rodata = node->Data;

            if (rodata->DisplayListPrimary.Primary == find)
            {
                rodata->DisplayListPrimary.Primary = replacement;
                return;
            }
            break;
    }
}
