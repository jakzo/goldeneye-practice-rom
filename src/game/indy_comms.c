#include <ultra64.h>
#include "indy_commands.h"
#include "indy_comms.h"

#ifdef PRACTICE_ROM
void indycommInit(void) {
#else
s32 indycommInit(void) {
#endif
    if (indy_ready != 1)
    {
        indy_ready = 1;
        indycommHostinit();
    }
}

void indycomm_removed(void) {
    #ifdef DEBUG
        //removed
    #endif
}

void indycommHostinit(void) {
    if (indy_ready)
    {
        indycmdSendInitPacket();
    }
}

void indycommHostLoadFile(char *filename, u8 *targetloc)
{
    u8 response1 [8];
    u8 response2 [4];
    u32 size;
  
    if (indy_ready)
    {
        indycmdSendLoadFile(filename,0x400000);
        indycmdReceiveFile(response1,response2,(u32)&size,targetloc);
    }
    return;
}

void indycommHostSendDump(char *filename, u8 *data, u32 size)
{
    u8 auStack4 [4];
  
    if (indy_ready) 
    {
#ifdef __GNUC__
        indycmdSendDump(filename, size, (struct indy_resource_entry_header *)data);
#else
        indycmdSendDump(filename, size, data);
#endif
        indycmdAckSendDump(auStack4);
    }
    return;
}

void indycommHostRamRomLoad(char *filename, u8 *target, s32 size)
{
    u32 uStack4;
    u32 uStack8;
    u32 uStack12;
  
    if (indy_ready)
    {
#ifdef __GNUC__
        indycmdSendRamRomLoad(filename,(u32)target,size);
#else
        indycmdSendRamRomLoad(filename,target,size);
#endif
        indycmdReceiveRamRom(&uStack4,&uStack8,&uStack12);
    }
    return;
}

void indycommHostSaveFile(char *filename, s32 size, u8 * data)
{
    u8 auStack4 [4];
  
    if (indy_ready)
    {
        indycmdSendHostExportFile(filename,data,size);
        indycmdAckHostExportFile(auStack4);
    }
}

u8 * indycommHostCheckFileExists(char *name, s32 *size)
{
    u8 *response;  
    if (!indy_ready) {
        return NULL;
    } else {
        indycmdSendHostCheckFileExists(name);
#ifdef __GNUC__
        indycmdAckHostCheckFileExists((u8 *)&response,(u8 *)size);
#else
        indycmdAckHostCheckFileExists(&response,size);
#endif
    }
    return response;
}

u8 *indycommHostSendCmd(u8 *cmdstr)
{
    u8 *local_4;  
    if (!indy_ready) {
        return NULL;
    }
    else {
        indycmdSendHostCmdPacket(cmdstr);
#ifdef __GNUC__
        indycmdAckHostCmdPacket((u8 *)&local_4);
#else
        indycmdAckHostCmdPacket(&local_4);
#endif
    }
    return local_4;
}

void indycommHost7F0D0124(void) {
    if (indy_ready)
    {
        rmonStatus();
    }
}

void indycommHostCloseConnection(void) {
    indycommHostSendCmd("sleep 5; /etc/killall ghost gload");
}
