#include "../qcommon/qcommon.h"
#include "../qcommon/netchan.h"
#include "../server/server.h"

#ifndef _CJ_
#define _CJ_

#include "events/events.h"

#define COMPILE_RATELIMITER 1

typedef struct
{
	int serverTime;
	int buttons;
	char forwardmove;
	char rightmove;
} CJ_prevCommand;

typedef struct CJ_savedPosition_s
{
    CJ_savedPosition_s *prevSave;
    gentity_t *groundEntity;
    vec3_t origin;
    vec3_t angles;
} CJ_savedPosition;

void CJ_hook_ClientThink(client_t *cl, usercmd_s *cmd);
void CJ_hook_ClientEndFrame(gentity_s *entity);

void CJ_hook_ClientConnect(gentity_s *ent);
void CJ_hook_ClientDisconnect(gentity_s *ent);

void CJ_hook_Jump_Start(pmove_t *pm, pml_t *pml, float height);
void CJ_hook_Jump_End(pmove_t *pm, pml_t *pml);

bool CJ_saveload_savePosition(int clientNum);
bool CJ_saveload_loadPosition(int clientNum, int backwardsCount);

void CJ_iprintln(int clientNum, char *message);

#endif