#include "../qcommon/qcommon.h"

const char *SV_GetMapBaseName(const char *mapname)
{
	return FS_GetMapBaseName(mapname);
}

void SV_Heartbeat_f(void)
{
	svs.nextHeartbeatTime = 0x80000000;
}

int SV_KickClient(client_s *cl, char *playerName, int maxPlayerNameLen)
{
	int guid;

	if ( cl->netchan.remoteAddress.type == NA_LOOPBACK )
	{
		SV_SendServerCommand(0, SV_CMD_CAN_IGNORE, "%c \"EXE_CANNOTKICKHOSTPLAYER\"", 101);
		return 0;
	}
	else
	{
		if ( playerName )
		{
			I_strncpyz(playerName, cl->name, maxPlayerNameLen);
			I_CleanStr(playerName);
		}

		guid = cl->guid;
		SV_DropClient(cl, "EXE_PLAYERKICKED");
		cl->lastPacketTime = svs.time;

		return guid;
	}
}

client_s *SV_GetPlayerByName()
{
	char cleanName[64];
	const char *name;
	int i;
	client_s *client;

	if ( !com_sv_running->current.boolean )
		return 0;

	if ( SV_Cmd_Argc() > 1 )
	{
		name = SV_Cmd_Argv(1u);
		i = 0;
		client = svs.clients;

		while ( i < sv_maxclients->current.integer )
		{
			if ( client->state )
			{
				if ( !I_stricmp(client->name, name) )
					return client;

				I_strncpyz(cleanName, client->name, sizeof(cleanName));
				I_CleanStr(cleanName);

				if ( !I_stricmp(cleanName, name) )
					return client;
			}

			++i;
			++client;
		}

		Com_Printf("Player %s is not on the server\n", name);
		return 0;
	}
	else
	{
		Com_Printf("No player specified.\n");
		return 0;
	}
}

int SV_KickUser_f(char *playerName, int maxPlayerNameLen)
{
	int i;
	client_s *client;

	if ( com_sv_running->current.boolean )
	{
		if ( SV_Cmd_Argc() == 2 )
		{
			client = SV_GetPlayerByName();

			if ( client )
			{
				return SV_KickClient(client, playerName, maxPlayerNameLen);
			}
			else
			{
				if ( !I_stricmp(SV_Cmd_Argv(1u), "all") )
				{
					i = 0;
					client = svs.clients;

					while ( i < sv_maxclients->current.integer )
					{
						if ( client->state )
							SV_KickClient(client, 0, 0);

						++i;
						++client;
					}
				}

				return 0;
			}
		}
		else
		{
			Com_Printf("Usage: %s <player name>\n%s all = kick everyone\n", SV_Cmd_Argv(0), SV_Cmd_Argv(0));
			return 0;
		}
	}
	else
	{
		Com_Printf("Server is not running.\n");
		return 0;
	}
}

void SV_Drop_f(void)
{
	SV_KickUser_f(0, 0);
}

void SV_Ban_f(void)
{
	client_s *client;

	if ( com_sv_running->current.boolean )
	{
		if ( SV_Cmd_Argc() == 2 )
		{
			client = SV_GetPlayerByName();

			if ( client )
				SV_BanClient(client);
		}
		else
		{
			Com_Printf("Usage: banUser <player name>\n");
		}
	}
	else
	{
		Com_Printf("Server is not running.\n");
	}
}

client_s *SV_GetPlayerByNum()
{
	const char *num;
	int idnum;
	int i;

	if ( !com_sv_running->current.boolean )
		return 0;

	if ( SV_Cmd_Argc() > 1 )
	{
		num = SV_Cmd_Argv(1u);

		for ( i = 0; num[i]; ++i )
		{
			if ( num[i] <= 47 || num[i] > 57 )
			{
				Com_Printf("Bad slot number: %s\n", num);
				return 0;
			}
		}

		idnum = atoi(num);

		if ( idnum >= 0 && idnum < sv_maxclients->current.integer )
		{
			if ( svs.clients[idnum].state )
			{
				return &svs.clients[idnum];
			}
			else
			{
				Com_Printf("Client %i is not active\n", idnum);
				return 0;
			}
		}
		else
		{
			Com_Printf("Bad client slot: %i\n", idnum);
			return 0;
		}
	}
	else
	{
		Com_Printf("No player specified.\n");
		return 0;
	}
}

void SV_BanNum_f(void)
{
	client_s *client;

	if ( com_sv_running->current.boolean )
	{
		if ( SV_Cmd_Argc() == 2 )
		{
			client = SV_GetPlayerByNum();

			if ( client )
				SV_BanClient(client);
		}
		else
		{
			Com_Printf("Usage: banClient <client number>\n");
		}
	}
	else
	{
		Com_Printf("Server is not running.\n");
	}
}

void SV_TempBan_f(void)
{
	char name[64];
	int guid;

	guid = SV_KickUser_f(name, sizeof(name));

	if ( guid )
	{
		Com_Printf("%s (guid %i) was kicked for cheating\n", name, guid);
		SV_BanGuidBriefly(guid);
	}
}

void SV_Unban_f(void)
{
	if ( SV_Cmd_Argc() == 2 )
	{
		SV_UnbanClient(SV_Cmd_Argv(1u));
	}
	else
	{
		Com_Printf("Usage: unban <client name>\n");
	}
}

int SV_KickClient_f(char *playerName, int maxPlayerNameLen)
{
	client_s *client;

	if ( com_sv_running->current.boolean )
	{
		if ( SV_Cmd_Argc() == 2 )
		{
			client = SV_GetPlayerByNum();

			if ( client )
				return SV_KickClient(client, playerName, maxPlayerNameLen);
			else
				return 0;
		}
		else
		{
			Com_Printf("Usage: %s <client number>\n", SV_Cmd_Argv(0));
			return 0;
		}
	}
	else
	{
		Com_Printf("Server is not running.\n");
		return 0;
	}
}

void SV_TempBanNum_f(void)
{
	char name[64];
	int guid;

	guid = SV_KickClient_f(name, 64);

	if ( guid )
	{
		Com_Printf("%s (guid %i) was kicked for cheating\n", name, guid);
		SV_BanGuidBriefly(guid);
	}
}

void SV_DropNum_f(void)
{
	SV_KickClient_f(0, 0);
}

extern dvar_t *sv_mapname;
void SV_Status_f()
{
	int score;
	size_t len;
	int j;
	int k;
	client_t *client;
	int l;
	const char *s;
	int i;

	if ( com_sv_running->current.boolean )
	{
		Com_Printf("map: %s\n", sv_mapname->current.string);
		Com_Printf("num score ping guid   name            lastmsg address               qport rate\n");
		Com_Printf("--- ----- ---- ------ --------------- ------- --------------------- ----- -----\n");

		i = 0;
		client = svs.clients;

		while ( i < sv_maxclients->current.integer )
		{
			if ( client->state )
			{
				Com_Printf("%3i ", i);
				SV_GameClientNum(i);
				score = G_GetClientScore(client - svs.clients);
				Com_Printf("%5i ", score);

				if ( client->state == CS_CONNECTED )
				{
					Com_Printf("CNCT ");
				}
				else if ( client->state == CS_ZOMBIE )
				{
					Com_Printf("ZMBI ");
				}
				else if ( client->ping >= 9999 )
				{
					Com_Printf("%4i ", 9999);
				}
				else
				{
					Com_Printf("%4i ", client->ping);
				}

				Com_Printf("%6i ", client->guid);
				Com_Printf("%s^7", client->name);

				l = 16 - I_DrawStrlen(client->name);

				for ( j = 0; j < l; ++j )
					Com_Printf(" ");

				Com_Printf("%7i ", svs.time - client->lastPacketTime);
				s = NET_AdrToString(client->netchan.remoteAddress);
				Com_Printf("%s", s);

				len = strlen(s);

				for ( k = 0; k < (int)(22 - len); ++k )
					Com_Printf(" ");

				Com_Printf("%6i", client->netchan.qport);
				Com_Printf(" %5i", client->rate);
				Com_Printf("\n");
			}

			++i;
			++client;
		}

		Com_Printf("\n");
	}
	else
	{
		Com_Printf("Server is not running.\n");
	}
}

void SV_Serverinfo_f(void)
{
	Com_Printf("Server info settings:\n");
	Info_Print(Dvar_InfoString(DVAR_SERVERINFO | DVAR_SCRIPTINFO));
}

void SV_Systeminfo_f(void)
{
	Com_Printf("System info settings:\n");
	Info_Print(Dvar_InfoString(DVAR_SYSTEMINFO));
}

void SV_DumpUser_f(void)
{
	client_s *client;

	if ( com_sv_running->current.boolean )
	{
		if ( SV_Cmd_Argc() == 2 )
		{
			client = SV_GetPlayerByName();

			if ( client )
			{
				Com_Printf("userinfo\n");
				Com_Printf("--------\n");
				Info_Print(client->userinfo);
			}
		}
		else
		{
			Com_Printf("Usage: info <userid>\n");
		}
	}
	else
	{
		Com_Printf("Server is not running.\n");
	}
}

extern dvar_t *sv_gametype;
extern dvar_t *sv_serverid;
#ifdef LIBCOD
extern dvar_t *sv_kickbots;
#endif
void SV_MapRestart(int fast_restart)
{
	const char *dropreason;
	const char *cmd;
	int savepersist;
	client_t *cl;
	char mapname[64];
	int i;

	// make sure server is running
	if ( !com_sv_running->current.boolean )
	{
		Com_Printf( "Server is not running.\n" );
		return;
	}

	SV_SetGametype();
	I_strncpyz(sv.gametype, sv_gametype->current.string, 64);
	savepersist = G_GetSavePersist();

	if ( !sv_maxclients->modified && !strcasecmp(sv.gametype, sv_gametype->current.string) && fast_restart )
	{
		if ( com_frameTime != sv.start_frameTime )
		{
#if COMPILE_SQLITE == 1
			free_sqlite_db_stores_and_tasks();
#endif
			Dvar_ResetScriptInfo();
			SV_InitArchivedSnapshot();
			svs.snapFlagServerBit ^= 4u;
			sv_serverId_value = (((byte)sv_serverId_value + 1) & 0xF) + (sv_serverId_value & 0xF0);
			Dvar_SetInt(sv_serverid, sv_serverId_value);
			sv.start_frameTime = com_frameTime;
			sv.state = SS_LOADING;
			sv.restarting = 1;
			SV_RestartGameProgs(savepersist);

			for(i = 0; i < 3; ++i)
			{
				svs.time += 100;
				SV_RunFrame();
			}

			for(i = 0, cl = svs.clients; i < sv_maxclients->current.integer; ++i, ++cl)
			{
				if ( cl->state < CS_CONNECTED )
				{
					continue;
				}
#ifdef LIBCOD
				if (sv_kickbots->current.boolean && cl->bot)
				{
					SV_DropClient(cl, "EXE_DISCONNECTED");
					continue;
				}
#endif
				if ( savepersist )
					cmd = va("%c", 110);
				else
					cmd = va("%c", 66);

				SV_AddServerCommand(cl, 1, cmd);

				dropreason = ClientConnect(i, cl->clscriptid);

				if ( dropreason )
				{
					SV_DropClient(cl, dropreason);
					Com_Printf("SV_MapRestart_f: dropped client %i - denied!\n", i);
				}
				else if ( cl->state == CS_ACTIVE )
				{
					SV_ClientEnterWorld(cl, &cl->lastUsercmd);
				}
			}

			sv.state = SS_GAME;
			sv.restarting = 0;
		}
	}
	else
	{
		G_SetSavePersist(0);
		I_strncpyz(mapname, Dvar_GetString("mapname"), 64);
		FS_ConvertPath(mapname);
		SV_SpawnServer(mapname);
	}
}

void SV_MapRestart_f()
{
	SV_MapRestart(0);
}

void SV_FastRestart_f()
{
	SV_MapRestart(1);
}

#ifdef LIBCOD
extern dvar_t *fs_library;
void manymaps_prepare(const char *mapname, int read)
{
	char map_check[MAX_OSPATH];
	char library_path[MAX_OSPATH];

	dvar_t *fs_homepath = Dvar_FindVar("fs_homepath");
	dvar_t *fs_game = Dvar_FindVar("fs_game");
	dvar_t *map = Dvar_FindVar("mapname");

	if (strlen(fs_library->current.string))
		strncpy(library_path, fs_library->current.string, sizeof(library_path));
	else
		snprintf(library_path, sizeof(library_path), "%s/%s/Library", fs_homepath->current.string, fs_game->current.string);

	Com_sprintf(map_check, MAX_OSPATH, "%s/%s.iwd", library_path, mapname);

#if PROTOCOL_VERSION < 117
	const char *stock_maps[] = { "mp_breakout", "mp_brecourt", "mp_burgundy", "mp_carentan", "mp_dawnville", "mp_decoy", "mp_downtown", "mp_farmhouse", "mp_leningrad", "mp_matmata", "mp_railyard", "mp_toujane", "mp_trainstation" };
#else
	const char *stock_maps[] = { "mp_breakout", "mp_brecourt", "mp_burgundy", "mp_carentan", "mp_dawnville", "mp_decoy", "mp_downtown", "mp_farmhouse", "mp_leningrad", "mp_matmata", "mp_railyard", "mp_toujane", "mp_trainstation", "mp_rhine", "mp_harbor" };
#endif

	bool map_found = false, from_stock_map = false;

	for (int i = 0; i < int( sizeof(stock_maps) / sizeof(stock_maps[0]) ); i++)
	{
		if (strcmp(map->current.string, stock_maps[i]) == 0)
		{
			from_stock_map = true;
			break;
		}
	}

	for (int i = 0; i < int( sizeof(stock_maps) / sizeof(stock_maps[0]) ); i++)
	{
		if (strcmp(mapname, stock_maps[i]) == 0)
		{
			map_found = true;

			if (from_stock_map) // When changing from stock map to stock map do not trigger manymap
				return;
			else
				break;
		}
	}

	int map_exists = access(map_check, F_OK) != -1;

	if (!map_exists && !map_found)
		return;

	DIR *dir;
	struct dirent *dir_ent;

	dir = opendir(library_path);

	if (!dir)
		return;

	while ((dir_ent = readdir(dir)) != NULL)
	{
		if (strcmp(dir_ent->d_name, ".") == 0 || strcmp(dir_ent->d_name, "..") == 0)
			continue;

		char fileDelete[MAX_OSPATH];
		Com_sprintf(fileDelete, MAX_OSPATH, "%s/%s/%s", fs_homepath->current.string, fs_game->current.string, dir_ent->d_name);

		if (access(fileDelete, F_OK) != -1)
		{
#ifdef _WIN32
			char delcmd[COD2_MAX_STRINGLENGTH];
			Com_sprintf(delcmd, sizeof(delcmd), "del /F %s", fileDelete);
			int unlink_success = system(delcmd) == 0;
#else
			int unlink_success = unlink(fileDelete) == 0;
#endif
			Com_Printf("manymaps> REMOVED OLD LINK: %s result of unlink: %s\n", fileDelete, unlink_success?"success":"failed");
		}
	}

	closedir(dir);

	if (map_exists)
	{
		char src[MAX_OSPATH];
		char dst[MAX_OSPATH];

		Com_sprintf(src, MAX_OSPATH, "%s/%s.iwd", library_path, mapname);
		Com_sprintf(dst, MAX_OSPATH, "%s/%s/%s.iwd", fs_homepath->current.string, fs_game->current.string, mapname);

		if (access(src, F_OK) != -1)
		{
#ifdef _WIN32
			char linkcmd[COD2_MAX_STRINGLENGTH];
			Com_sprintf(linkcmd, sizeof(linkcmd), "mklink /h %s %s", dst, src);
			int link_success = system(linkcmd) == 0;
#else
			int link_success = symlink(src, dst) == 0;
#endif
			Com_Printf("manymaps> NEW LINK: src=%s dst=%s result of link: %s\n", src, dst, link_success?"success":"failed");

			if (link_success && read == -1) // FS_LoadDir is needed when empty.iwd is missing (then .d3dbsp isn't referenced anywhere)
				FS_AddIwdFilesForGameDirectory(fs_homepath->current.string, fs_game->current.string);
		}
	}
}

int hook_findMap(const char *qpath, void **buffer)
{
	int read = FS_ReadFile(qpath, buffer);
	manymaps_prepare(Cmd_Argv(1), read);

	if (read != -1)
		return read;
	else
		return FS_ReadFile(qpath, buffer);
}

bool hook_SV_MapExists(const char *mapname)
{
	bool map_exists = SV_MapExists(mapname);

	if (map_exists)
	{
		return map_exists;
	}
	else
	{
		char map_check[MAX_OSPATH];
		char library_path[MAX_OSPATH];

		dvar_t *fs_homepath = Dvar_FindVar("fs_homepath");
		dvar_t *fs_game = Dvar_FindVar("fs_game");

		if (strlen(fs_library->current.string))
			strncpy(library_path, fs_library->current.string, sizeof(library_path));
		else
			snprintf(library_path, sizeof(library_path), "%s/%s/Library", fs_homepath->current.string, fs_game->current.string);

		Com_sprintf(map_check, MAX_OSPATH, "%s/%s.iwd", library_path, mapname);

		return access(map_check, F_OK) != -1;
	}
}
#endif

extern dvar_t *sv_cheats;
void SV_Map_f()
{
	char mapname[64];
	char *qpath;
	const char *base;
	const char *map;

	map = SV_Cmd_Argv(1u);

	if ( *map )
	{
		base = SV_GetMapBaseName(map);
		I_strncpyz(mapname, base, 64);
		I_strlwr(mapname);
		qpath = va("maps/mp/%s.%s", mapname, GetBspExtension());

#ifdef LIBCOD
		if ( hook_findMap(qpath, 0) != -1 )
#else
		if ( FS_ReadFile(qpath, 0) != -1 )
#endif
		{
			FS_ConvertPath(mapname);
			SV_SpawnServer(mapname);
			Dvar_SetBool(sv_cheats, I_stricmp(SV_Cmd_Argv(0), "devmap") == 0);
		}
		else
			Com_Printf("Can't find map %s\n", qpath);
	}
}

extern dvar_t *sv_mapRotationCurrent;
const char* UI_GetMapRotationToken()
{
	char *token;
	const char *value;

	value = sv_mapRotationCurrent->current.string;
	token = Com_Parse(&value);

	if ( value )
	{
		Dvar_SetString(sv_mapRotationCurrent, value);
		return token;
	}
	else
	{
		Dvar_SetString(sv_mapRotationCurrent, "");
		return 0;
	}
}

extern dvar_t *sv_mapRotation;
void SV_MapRotate_f()
{
	const char *token;

	Com_Printf("map_rotate...\n\n");
	Com_Printf("\"sv_mapRotation\" is:\"%s\"\n\n", sv_mapRotation->current.string);
	Com_Printf("\"sv_mapRotationCurrent\" is:\"%s\"\n\n", sv_mapRotationCurrent->current.string);

	if ( !*sv_mapRotationCurrent->current.string )
		Dvar_SetString(sv_mapRotationCurrent, sv_mapRotation->current.string);

	token = UI_GetMapRotationToken();

	if ( !token )
	{
		Dvar_SetString(sv_mapRotationCurrent, sv_mapRotation->current.string);
		token = UI_GetMapRotationToken();
	}

	while ( 1 )
	{
		if ( !token )
		{
			Com_Printf("No map specified in sv_mapRotation - forcing map_restart.\n");
			SV_FastRestart_f();
			return;
		}

		if ( strcasecmp(token, "gametype") )
			break;

		token = UI_GetMapRotationToken();

		if ( !token )
		{
			Com_Printf("No gametype specified after 'gametype' keyword in sv_mapRotation - forcing map_restart.\n");
			SV_FastRestart_f();
			return;
		}

		Com_Printf("Setting g_gametype: %s.\n", token);

		if ( com_sv_running->current.boolean )
		{
			if ( strcasecmp(sv_gametype->current.string, token) )
				G_SetSavePersist(0);
		}

		Dvar_SetString(sv_gametype, token);
retry:
		token = UI_GetMapRotationToken();
	}

	if ( strcasecmp(token, "map") )
	{
		Com_Printf("Unknown keyword '%s' in sv_mapRotation.\n", token);
		goto retry;
	}

	token = UI_GetMapRotationToken();

	if ( token )
	{
		Com_Printf("Setting map: %s.\n", token);
		Cbuf_ExecuteText(EXEC_NOW, va("map %s\n", token));
	}
	else
	{
		Com_Printf("No map specified after 'map' keyword in sv_mapRotation - forcing map_restart.\n");
		SV_FastRestart_f();
	}
}

void SV_GameCompleteStatus_f(void)
{
	if ( com_sv_running->current.boolean )
		SV_MasterGameCompleteStatus();
}

void SV_KillServer_f(void)
{
	Com_Shutdown("EXE_SERVERKILLED");
}

void SV_ScriptUsage_f(void)
{
	Scr_DumpScriptThreads();
}

void SV_StringUsage_f(void)
{
	MT_DumpTree();
}

/*
==================
SV_ConSay_f
==================
*/
void SV_ConSay_f( void )
{
	char    *p;
	char text[MAX_STRING_CHARS];

	// make sure server is running
	if ( !com_sv_running->current.boolean )
	{
		Com_Printf( "Server is not running.\n" );
		return;
	}

	if ( Cmd_Argc() < 2 )
	{
		return;
	}

	strcpy( text, "console: " );
	p = Cmd_Args(1);

	if ( *p == '"' )
	{
		p++;
		p[strlen( p ) - 1] = 0;
	}

	I_strncat(text, sizeof(text), p);
	SV_SendServerCommand(0, SV_CMD_CAN_IGNORE, "%c \"\x15%s\"", 104, text);
}

/*
==================
SV_ConTell_f
==================
*/
void SV_ConTell_f( void )
{
	char    *p;
	char text[MAX_STRING_CHARS];
	client_t *cl;
	int num;

	// make sure server is running
	if ( !com_sv_running->current.boolean )
	{
		Com_Printf( "Server is not running.\n" );
		return;
	}

	if ( Cmd_Argc() < 3 )
	{
		return;
	}

	num = atoi(Cmd_Argv(1));

	if (num < 0 || num > sv_maxclients->current.integer)
		return;

	cl = &svs.clients[num];

	if (cl->state != CS_ACTIVE)
	{
		return;
	}

	strcpy( text, "console: " );
	p = Cmd_Args(2);

	if ( *p == '"' )
	{
		p++;
		p[strlen( p ) - 1] = 0;
	}

	I_strncat(text, sizeof(text), p);
	SV_SendServerCommand(cl, SV_CMD_CAN_IGNORE, "%c \"\x15%s\"", 104, text);
}

void SV_AddDedicatedCommands()
{
	Cmd_AddCommand("say", SV_ConSay_f);
	Cmd_AddCommand("tell", SV_ConTell_f);
}

void SV_AddOperatorCommands()
{
	static qboolean initailized = 0;

	if ( !initailized )
	{
		initailized = 1;

		Cmd_AddCommand("heartbeat", SV_Heartbeat_f);
		Cmd_AddCommand("onlykick", SV_Drop_f);
		Cmd_AddCommand("banUser", SV_Ban_f);
		Cmd_AddCommand("banClient", SV_BanNum_f);
		Cmd_AddCommand("kick", SV_TempBan_f);
		Cmd_AddCommand("tempBanUser", SV_TempBan_f);
		Cmd_AddCommand("tempBanClient", SV_TempBanNum_f);
		Cmd_AddCommand("unbanUser", SV_Unban_f);
		Cmd_AddCommand("clientkick", SV_DropNum_f);
		Cmd_AddCommand("status", SV_Status_f);
		Cmd_AddCommand("serverinfo", SV_Serverinfo_f);
		Cmd_AddCommand("systeminfo", SV_Systeminfo_f);
		Cmd_AddCommand("dumpuser", SV_DumpUser_f);
		Cmd_AddCommand("map_restart", SV_MapRestart_f);
		Cmd_AddCommand("fast_restart", SV_FastRestart_f);
		Cmd_AddCommand("map", SV_Map_f);
		Cmd_SetAutoComplete("map", "maps/mp", "d3dbsp");
		Cmd_AddCommand("map_rotate", SV_MapRotate_f);
		Cmd_AddCommand("gameCompleteStatus", SV_GameCompleteStatus_f);
		Cmd_AddCommand("devmap", SV_Map_f);
		Cmd_SetAutoComplete("devmap", "maps/mp", "d3dbsp");
		Cmd_AddCommand("killserver", SV_KillServer_f);

		if ( com_dedicated->current.integer )
			SV_AddDedicatedCommands();

		Cmd_AddCommand("scriptUsage", SV_ScriptUsage_f);
		Cmd_AddCommand("stringUsage", SV_StringUsage_f);
	}
}

void SV_RemoveOperatorCommands()
{
	;
}