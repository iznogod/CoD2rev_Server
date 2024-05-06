#include "../opencj.h"

extern CJ_savedPosition *savedPositions[];

void CJ_hook_ClientDisconnect(gentity_s *ent)
{
    int clientNum = ent - g_entities;
    CJ_savedPosition *save = savedPositions[clientNum];
    while(save != NULL)
    {
        CJ_savedPosition *prevSave = save->prevSave;
        free(save);
        save = prevSave;
    }
}