#include "../opencj.h"

CJ_prevCommand prevCommand[MAX_CLIENTS];

int prevUseButtonStart[MAX_CLIENTS];
int prevMeleeButtonStart[MAX_CLIENTS];

bool loadPos[MAX_CLIENTS];

void CJ_hook_ClientThink(client_t *cl, usercmd_s *cmd)
{
    int clientNum = cl->gentity - g_entities;
    if((cmd->forwardmove != 127 && cmd->forwardmove != -127 && cmd->forwardmove != 0) || (cmd->rightmove != 127 && cmd->rightmove != -127 && cmd->rightmove != 0))
    {
        CJ_event_OnCheatStart(clientNum);
    }
    if(!(prevCommand[clientNum].buttons & KEY_MASK_FIRE) && (cmd->buttons & KEY_MASK_FIRE))
    {
        
    }
    else if((prevCommand[clientNum].buttons & KEY_MASK_FIRE) && !(cmd->buttons & KEY_MASK_FIRE))
    {
        
    }

    if(!(prevCommand[clientNum].buttons & KEY_MASK_MELEE) && (cmd->buttons & KEY_MASK_MELEE))
    {
        if(cmd->serverTime - prevMeleeButtonStart[clientNum] < 500)
        {
            if(level.clients[clientNum].sess.sessionState == SESS_STATE_PLAYING)
            {
                CJ_saveload_savePosition(clientNum);
                prevMeleeButtonStart[clientNum] = 0;
            }
        }
        else
        {
            prevMeleeButtonStart[clientNum] = cmd->serverTime;
        }
    }
    else if((prevCommand[clientNum].buttons & KEY_MASK_MELEE) && !(cmd->buttons & KEY_MASK_MELEE))
    {
        
    }

    if(!(prevCommand[clientNum].buttons & KEY_MASK_USE) && (cmd->buttons & KEY_MASK_USE))
    {
        if(cmd->serverTime - prevUseButtonStart[clientNum] < 500)
        {
            loadPos[clientNum] = true;
            prevUseButtonStart[clientNum] = 0;
        }
        else
        {
            prevUseButtonStart[clientNum] = cmd->serverTime;
        }
    }
    else if((prevCommand[clientNum].buttons & KEY_MASK_USE) && !(cmd->buttons & KEY_MASK_USE))
    {
        
    }

    if(!(prevCommand[clientNum].buttons & KEY_MASK_LEANLEFT) && (cmd->buttons & KEY_MASK_LEANLEFT))
    {
        
    }
    else if((prevCommand[clientNum].buttons & KEY_MASK_LEANLEFT) && !(cmd->buttons & KEY_MASK_LEANLEFT))
    {
        
    }

    if(!(prevCommand[clientNum].buttons & KEY_MASK_LEANRIGHT) && (cmd->buttons & KEY_MASK_LEANRIGHT))
    {
        
    }
    else if((prevCommand[clientNum].buttons & KEY_MASK_LEANRIGHT) && !(cmd->buttons & KEY_MASK_LEANRIGHT))
    {
        
    }

    if((prevCommand[clientNum].forwardmove) != 127 && cmd->forwardmove == 127)
    {
        
    }
    else if((prevCommand[clientNum].forwardmove) == 127 && cmd->forwardmove != 127)
    {
        
    }

    if((prevCommand[clientNum].forwardmove) != -127 && cmd->forwardmove == -127)
    {
        
    }
    else if((prevCommand[clientNum].forwardmove) == -127 && cmd->forwardmove != -127)
    {
        
    }

    if((prevCommand[clientNum].rightmove) != 127 && cmd->rightmove == 127)
    {
        
    }
    else if((prevCommand[clientNum].rightmove) == 127 && cmd->rightmove != 127)
    {
        
    }

    if((prevCommand[clientNum].rightmove) != -127 && cmd->rightmove == -127)
    {
        
    }
    else if((prevCommand[clientNum].rightmove) == -127 && cmd->rightmove != -127)
    {
        
    }
    prevCommand[clientNum].buttons = cmd->buttons;
    prevCommand[clientNum].forwardmove = cmd->forwardmove;
    prevCommand[clientNum].rightmove = cmd->rightmove;
    prevCommand[clientNum].serverTime = cmd->serverTime;
}