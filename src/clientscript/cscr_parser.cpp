#include "../qcommon/qcommon.h"
#include "clientscript_public.h"

#ifdef TESTING_LIBRARY
#define scrVarPub (*((scrVarPub_t*)( 0x08394000 )))
#else
extern scrVarPub_t scrVarPub;
#endif

typedef struct scrParserGlob_s
{
	struct OpcodeLookup *opcodeLookup;
	unsigned int opcodeLookupMaxLen;
	unsigned int opcodeLookupLen;
	struct SourceLookup *sourcePosLookup;
	unsigned int sourcePosLookupMaxLen;
	unsigned int sourcePosLookupLen;
	unsigned int sourceBufferLookupMaxLen;
	const char *currentCodePos;
	unsigned int currentSourcePosCount;
	struct SaveSourceBufferInfo *saveSourceBufferLookup;
	unsigned int saveSourceBufferLookupLen;
	int delayedSourceIndex;
	int threadStartSourceIndex;
} scrParserGlob_t;
static_assert((sizeof(scrParserGlob_t) == 0x34), "ERROR: scrParserGlob_t size is invalid!");

#ifdef TESTING_LIBRARY
#define scrParserGlob (*((scrParserGlob_t*)( 0x08283EA0 )))
#else
scrParserGlob_t scrParserGlob;
#endif

#ifdef TESTING_LIBRARY
#define scrParserPub (*((scrParserPub_t*)( 0x08283ED4 )))
#else
scrParserPub_t scrParserPub;
#endif

char g_EndPos;

unsigned int Scr_GetLineNumInternal(const char *buf, unsigned int sourcePos, const char **startLine, int *col)
{
	unsigned int lineNum;

	*startLine = buf;
	lineNum = 0;

	while ( sourcePos )
	{
		if ( !*buf )
		{
			*startLine = buf + 1;
			++lineNum;
		}
		++buf;
		--sourcePos;
	}

	*col = buf - *startLine;
	return lineNum;
}

unsigned int Scr_GetLineInfo(const char *buf, unsigned int sourcePos, int *col, char *line)
{
	char b, c;
	const char *startLine;
	unsigned int linenum;
	int i;
	int length;

	linenum = Scr_GetLineNumInternal(buf, sourcePos, &startLine, col);
	length = strlen(startLine);

	if ( length > MAX_STRING_CHARS - 1 )
		length = MAX_STRING_CHARS - 1;

	for ( i = 0; i <= length; ++i )
	{
		b = startLine[i];

		if ( b == 9 )
			c = 32;
		else
			c = b;

		line[i] = c;
	}

	if ( line[length - 1] == 13 )
		line[length - 1] = 0;

	return linenum;
}

void Scr_PrintSourcePos(conChannel_t channel, const char *filename, const char *buf, unsigned int sourcePos)
{
	const char *save;
	int i;
	int col;
	unsigned int line;
	char string[MAX_STRING_CHARS];

	line = Scr_GetLineInfo(buf, sourcePos, &col, string);

	if ( scrParserGlob.saveSourceBufferLookup )
		save = " (savegame)";
	else
		save = "";

	Com_PrintMessage(channel, va("(file '%s'%s, line %d)\n", filename, save, line + 1));
	Com_PrintMessage(channel, va("%s\n", string));

	for ( i = 0; i < col; ++i )
		Com_PrintMessage(channel, " ");

	Com_PrintMessage(channel, "*\n");
}

OpcodeLookup* Scr_GetPrevSourcePosOpcodeLookup(const char *codePos)
{
	int middle;
	signed int high;
	signed int low;

	low = 0;
	high = scrParserGlob.opcodeLookupLen - 1;

	while ( low <= high )
	{
		middle = (high + low) / 2;

		if ( codePos < scrParserGlob.opcodeLookup[middle].codePos )
		{
			high = middle - 1;
		}
		else
		{
			low = middle + 1;

			if ( middle + 1 == scrParserGlob.opcodeLookupLen || codePos < scrParserGlob.opcodeLookup[low].codePos )
				return &scrParserGlob.opcodeLookup[middle];
		}
	}

	return 0;
}

unsigned int Scr_GetPrevSourcePos(const char *codePos, unsigned int index)
{
	return scrParserGlob.sourcePosLookup[Scr_GetPrevSourcePosOpcodeLookup(codePos)->sourcePosIndex + index].sourcePos;
}

unsigned int Scr_GetSourceBuffer(const char *codePos)
{
	signed int bufferIndex;

	for ( bufferIndex = scrParserPub.sourceBufferLookupLen - 1;
	        bufferIndex > 0
	        && (!scrParserPub.sourceBufferLookup[bufferIndex].codePos
	            || scrParserPub.sourceBufferLookup[bufferIndex].codePos > codePos);
	        --bufferIndex )
	{
		;
	}

	return bufferIndex;
}

void Scr_PrintPrevCodePos(conChannel_t channel, const char *codePos, unsigned int index)
{
	unsigned int bufferIndex;

	if ( !codePos )
	{
		Com_PrintMessage(channel, "<frozen thread>\n");
		return;
	}

	if ( codePos == &g_EndPos )
	{
		Com_PrintMessage(channel, "<removed thread>\n");
	}
	else
	{
		if ( scrVarPub.developer )
		{
			if ( scrVarPub.programBuffer && Scr_IsInOpcodeMemory(codePos) )
			{
				bufferIndex = Scr_GetSourceBuffer(codePos - 1);
				Scr_PrintSourcePos(
				    channel,
				    scrParserPub.sourceBufferLookup[bufferIndex].buf,
				    scrParserPub.sourceBufferLookup[bufferIndex].sourceBuf,
				    Scr_GetPrevSourcePos(codePos - 1, index));
				return;
			}
		}
		else if ( Scr_IsInOpcodeMemory(codePos - 1) )
		{
			Com_PrintMessage(channel, va("@ %d\n", codePos - scrVarPub.programBuffer));
			return;
		}

		Com_PrintMessage(channel, va("%s\n\n", codePos));
	}
}

void CompileError(unsigned int sourcePos, const char *format, ...)
{
	char buf[MAX_STRING_CHARS];
	va_list argptr;

	va_start( argptr, format );
	Q_vsnprintf( buf, sizeof( buf ), format, argptr );
	va_end( argptr );

	if ( !scrVarPub.evaluate )
	{
		Com_Printf("\n");
		Com_Printf("******* script compile error *******\n");

		if ( scrVarPub.developer )
		{
			Com_Printf("%s: ", buf);
			Scr_PrintSourcePos(CON_CHANNEL_DONT_FILTER, scrParserPub.scriptfilename, scrParserPub.sourceBuf, sourcePos);
		}
		else
		{
			Com_Printf("%s\n", buf);
		}

		Com_Printf("************************************\n");
		Com_Error(ERR_SCRIPT_DROP, "script compile error\n(see console for details)\n");
	}

	if ( !scrVarPub.error_message )
		scrVarPub.error_message = va("%s", buf);
}