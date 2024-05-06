#include "../opencj.h"

extern bool loadPos[];

void CJ_hook_ClientEndFrame(gentity_s *entity)
{
    int clientNum = entity - g_entities;
    if(level.clients[clientNum].sess.sessionState == SESS_STATE_PLAYING)
    {
        if(loadPos[clientNum])
        {
            CJ_saveload_loadPosition(clientNum, 0);
            loadPos[clientNum] = 0;
        }
    }
    else
    {
        loadPos[clientNum] = false;
    }
}