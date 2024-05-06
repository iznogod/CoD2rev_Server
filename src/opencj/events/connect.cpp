#include "../opencj.h"

extern CJ_prevCommand prevCommand[];
extern bool loadPos[];
extern CJ_savedPosition *savedPositions[];
extern int prevUseButtonStart[];
extern int prevMeleeButtonStart[];

void CJ_hook_ClientConnect(gentity_s *ent)
{
    int clientNum = ent - g_entities;
    prevCommand[clientNum].buttons = 0;
    prevCommand[clientNum].forwardmove = 0;
    prevCommand[clientNum].rightmove = 0;
    prevCommand[clientNum].serverTime = level.time;
    savedPositions[clientNum] = NULL;
    loadPos[clientNum] = false;
    prevUseButtonStart[clientNum] = 0;
    prevMeleeButtonStart[clientNum] = 0;
}