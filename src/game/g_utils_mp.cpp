#include "../qcommon/qcommon.h"
#include "g_shared.h"
#include "../clientscript/clientscript_public.h"
#include "../server/server.h"

#ifdef TESTING_LIBRARY
#define level (*((level_locals_t*)( 0x0859B400 )))
#else
extern level_locals_t level;
#endif

#ifdef TESTING_LIBRARY
#define g_entities ((gentity_t*)( 0x08665480 ))
#else
extern gentity_t g_entities[];
#endif

#ifdef TESTING_LIBRARY
#define entityHandlers ((entityHandler_t*)( 0x08167880 ))
#else
extern entityHandler_t entityHandlers[] = {};
#endif

XModel* cached_models[MAX_MODELS];

const char* G_ModelName(int modelIndex)
{
	return SV_GetConfigstringConst(modelIndex + 334);
}

int G_FindConfigstringIndex(const char *name, int start, int max, int create, const char *errormsg)
{
	const char *s1;
	int i;

	if ( !name || !*name )
		return 0;

	for ( i = 1; i < max; ++i )
	{
		s1 = SV_GetConfigstringConst(start + i);

		if ( !*s1 )
			break;

		if ( !strcasecmp(s1, name) )
			return i;
	}

	if ( create )
	{
		if ( i == max )
		{
			Com_Error(ERR_DROP, va("G_FindConfigstringIndex: overflow (%d): %s", start, name));
		}

		SV_SetConfigstring(start + i, name);
		return i;
	}
	else
	{
		if ( errormsg )
		{
			Scr_Error(va("%s \"%s\" not precached", errormsg, name));
		}

		return 0;
	}
}

int G_LocalizedStringIndex(const char *string)
{
	if ( *string )
		return G_FindConfigstringIndex(string, 1310, 256, level.initializing, "localized string");
	else
		return 0;
}

int G_ShaderIndex(const char *string)
{
	char dest[64];

	strcpy(dest, string);
	I_strlwr(dest);

	return G_FindConfigstringIndex(dest, 1566, 128, level.initializing, "shader");
}

int G_TagIndex(const char *name)
{
	return G_FindConfigstringIndex(name, 110, 32, 1, 0);
}

XModel* G_CachedModelForIndex(int modelIndex)
{
	return cached_models[modelIndex];
}

unsigned int G_ModelIndex(const char *name)
{
	const char *modelName;
	signed int i;

	if ( !*name )
		return 0;

	for ( i = 1; i < MAX_MODELS; ++i )
	{
		modelName = SV_GetConfigstringConst(i + 334);

		if ( !*modelName )
			break;

		if ( !strcasecmp(modelName, name) )
			return i;
	}

	if ( !level.initializing )
	{
		Scr_Error(va("model '%s' not precached", name));
	}

	if ( i == MAX_MODELS )
		Com_Error(ERR_DROP, "G_ModelIndex: overflow");

	cached_models[i] = SV_XModelGet(name);
	SV_SetConfigstring(i + 334, name);

	return i;
}

void G_OverrideModel(int modelIndex, const char *defaultModelName)
{
	XModel *model;
	const char *modelName;

	modelName = G_ModelName(modelIndex);
	model = SV_XModelGet(defaultModelName);
	cached_models[modelIndex] = model;
	Hunk_OverrideDataForFile(FILEDATA_XMODEL, modelName + 7, model);
}

void G_SetModel(gentity_s *ent, const char *modelName)
{
	if ( *modelName )
		ent->model = G_ModelIndex(modelName);
	else
		ent->model = 0;
}

void G_SetOrigin(gentity_s *ent, const float *origin)
{
	VectorCopy(origin, ent->s.pos.trBase);
	ent->s.pos.trType = TR_STATIONARY;
	ent->s.pos.trTime = 0;
	ent->s.pos.trDuration = 0;
	VectorSet(ent->s.pos.trDelta, 0, 0, 0);
	VectorCopy(origin, ent->r.currentOrigin);
}

void G_SetAngle(gentity_s *ent, const float *angle)
{
	VectorCopy(angle, ent->s.apos.trBase);
	ent->s.apos.trType = TR_STATIONARY;
	ent->s.apos.trTime = 0;
	ent->s.apos.trDuration = 0;
	VectorSet(ent->s.pos.trDelta, 0, 0, 0);
	VectorCopy(angle, ent->r.currentAngles);
}

void G_DObjCalcPose(gentity_s *ent)
{
	int partBits[4];
	void (*controller)(struct gentity_s *, int *);

	memset(partBits, 255, sizeof(partBits));

	if ( !SV_DObjCreateSkelForBones(ent, partBits) )
	{
		SV_DObjCalcAnim(ent, partBits);

		controller = entityHandlers[ent->handler].controller;

		if ( controller )
			controller(ent, partBits);

		SV_DObjCalcSkel(ent, partBits);
	}
}

void G_DObjCalcBone(gentity_s *ent, int boneIndex)
{
	int partBits[4];
	void (*controller)(struct gentity_s *, int *);

	if ( !SV_DObjCreateSkelForBone(ent, boneIndex) )
	{
		SV_DObjGetHierarchyBits(ent, boneIndex, partBits);
		SV_DObjCalcAnim(ent, partBits);

		controller = entityHandlers[ent->handler].controller;

		if ( controller )
			controller(ent, partBits);

		SV_DObjCalcSkel(ent, partBits);
	}
}

float G_random()
{
	return (float)rand() / 2147483600.0;
}

float G_crandom()
{
	return G_random() * 2.0 - 1.0;
}

void G_InitGentity(gentity_s *ent)
{
	ent->nextFree = 0;
	ent->r.inuse = 1;
	Scr_SetString(&ent->classname, scr_const.noclass);
	ent->s.number = ent - g_entities;
	ent->r.ownerNum = 1023;
	ent->eventTime = 0;
	ent->freeAfterEvent = 0;
}