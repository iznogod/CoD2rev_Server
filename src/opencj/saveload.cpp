#include "opencj.h"

CJ_savedPosition *savedPositions[MAX_CLIENTS];

bool CJ_saveload_savePosition(int clientNum)
{
    if(g_entities[clientNum].s.groundEntityNum != 1023)
    {
        CJ_savedPosition *newSave = (CJ_savedPosition*)malloc(sizeof(CJ_savedPosition));
        if(newSave == NULL)
        {
            return false;
        }
        newSave->prevSave = savedPositions[clientNum];
        savedPositions[clientNum] = newSave;
        newSave->origin[0] = g_entities[clientNum].client->ps.origin[0];
        newSave->origin[1] = g_entities[clientNum].client->ps.origin[1];
        newSave->origin[2] = g_entities[clientNum].client->ps.origin[2];
        newSave->angles[0] = g_entities[clientNum].client->ps.viewangles[0];
        newSave->angles[1] = g_entities[clientNum].client->ps.viewangles[1];
        newSave->angles[2] = g_entities[clientNum].client->ps.viewangles[2];
        newSave->groundEntity = &g_entities[g_entities[clientNum].s.groundEntityNum];
		CJ_iprintln(clientNum, "^2Position saved");
        return true;
    }
    else
    {
		CJ_iprintln(clientNum, "^1Cannot save in air");
        return false;
    }
}

bool CJ_saveload_loadPosition(int clientNum, int backwardsCount)
{
    #define PMF_DUCKED 0x2
#ifdef COD4
	#define PMF_PRONE 0x1
#endif
	#define EF_TELEPORT_BIT 0x2
    CJ_savedPosition *save = savedPositions[clientNum];
    while(backwardsCount && save != NULL)
    {
        save = save->prevSave;
        backwardsCount--;
    }
    if(save == NULL)
    {
        printf("Too far back\n");
        return false;
    }
    gentity_t *ent = &g_entities[clientNum];
#ifdef COD4
    bool isUsingTurret = ((ent->client->ps.otherFlags & 4) != 0  && (ent->client->ps.eFlags & 0x300) != 0);
#else
	bool isUsingTurret = ((ent->client->ps.pm_flags & 0x800000) != 0  && (ent->client->ps.eFlags & 0x300) != 0);
#endif

	//stop using MGs
	if(isUsingTurret)
	{
		G_ClientStopUsingTurret(&g_entities[ent->client->ps.viewlocked_entNum]);
	}

	G_EntUnlink(ent);

	//unlink client from linkto() stuffs
	if (ent->r.linked)
	{
		SV_UnlinkEntity(ent);
	}

	//clear flags
	ent->client->ps.pm_flags &= (PMF_DUCKED | PMF_PRONE);//keep stance
	ent->client->ps.eFlags ^= EF_TELEPORT_BIT; //alternate teleport flag, unsure why

	//set times
	ent->client->ps.pm_time = 0;
	ent->client->ps.jumpTime = 0; //to reset wallspeed effects

	//set origin
    ent->client->ps.origin[0] = save->origin[0];
    ent->client->ps.origin[1] = save->origin[1];
    ent->client->ps.origin[2] = save->origin[2];
    G_SetOrigin(ent, save->origin);

	//reset velocity
	ent->client->ps.velocity[0] = 0;
	ent->client->ps.velocity[1] = 0;
	ent->client->ps.velocity[2] = 0;

#ifdef COD4
	ent->client->ps.sprintState.sprintButtonUpRequired = 0;
	ent->client->ps.sprintState.sprintDelay = 0;
	ent->client->ps.sprintState.lastSprintStart = 0;
	ent->client->ps.sprintState.lastSprintEnd = 0;
	ent->client->ps.sprintState.sprintStartMaxLength = 0;
#endif

	//pretend we're not proning so that prone angle is ok after having called SetClientViewAngle (otherwise it gets a correction)
	int flags = ent->client->ps.pm_flags;
	ent->client->ps.pm_flags &= ~PMF_PRONE;
	ent->client->sess.cmd.serverTime = level.time; //if this isnt set then errordecay takes place

	SetClientViewAngle(ent, save->angles);

	//create a pmove object and execute to bypass the errordecay thing
	pmove_t pm;
	memset(&pm, 0, sizeof(pm));
	pm.ps = &ent->client->ps;
	memcpy(&pm.cmd, &ent->client->sess.cmd, sizeof(pm.cmd));
    pm.cmd.serverTime = level.time - 50;

    ent->client->sess.oldcmd.serverTime = level.time - 100;
	pm.oldcmd = ent->client->sess.oldcmd;
	pm.tracemask = 42008593;
	pm.handler = 1;
	Pmove(&pm);

	//reset velocity
	ent->client->ps.velocity[0] = 0;
	ent->client->ps.velocity[1] = 0;
	ent->client->ps.velocity[2] = 0;

	//restore prone if any
	ent->client->ps.pm_flags = flags;

	//stop shellshock
	ent->client->ps.shellshockIndex = 0;
	ent->client->ps.shellshockTime = 0;
	ent->client->ps.shellshockDuration = 0;

	//set health
	ent->health = 100;
	ent->maxHealth = 100;
	level.clients[clientNum].sess.maxHealth = 100;
	level.clients[clientNum].ps.stats[0] = 100;
	level.clients[clientNum].ps.stats[2] = 100;

	SV_LinkEntity(ent);
	CJ_iprintln(clientNum, "^2Position loaded");
    return true;
}