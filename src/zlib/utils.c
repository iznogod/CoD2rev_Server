#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zlib.h"
#include "ioapi.h"

#define Com_Memset memset

void *Z_TryMallocInternalZip( int size )
{
	void *buf;

	buf = malloc(size);
	if (buf)
		Com_Memset(buf, 0, size);

	return buf;
}

void Z_FreeInternalZip( void *ptr )
{
	free(ptr);
}

voidp zcalloc (voidp opaque, unsigned items, unsigned size)
{
	if (opaque) items += size - size; /* make compiler happy */
	return (voidp)Z_TryMallocInternalZip(items*size);
}

void  zcfree (voidp opaque, voidp ptr)
{
	Z_FreeInternalZip(ptr);
	if (opaque) return; /* make compiler happy */
}

z_const char * const z_errmsg[10] =
{
	(z_const char *)"need dictionary",     /* Z_NEED_DICT       2  */
	(z_const char *)"stream end",          /* Z_STREAM_END      1  */
	(z_const char *)"",                    /* Z_OK              0  */
	(z_const char *)"file error",          /* Z_ERRNO         (-1) */
	(z_const char *)"stream error",        /* Z_STREAM_ERROR  (-2) */
	(z_const char *)"data error",          /* Z_DATA_ERROR    (-3) */
	(z_const char *)"insufficient memory", /* Z_MEM_ERROR     (-4) */
	(z_const char *)"buffer error",        /* Z_BUF_ERROR     (-5) */
	(z_const char *)"incompatible version",/* Z_VERSION_ERROR (-6) */
	(z_const char *)""
};


/* ioapi.c -- IO base function header for compress/uncompress .zip
   files using zlib + zip or unzip API

   Version 1.01e, February 12th, 2005

   Copyright (C) 1998-2005 Gilles Vollant
*/



/* I've found an old Unix (a SunOS 4.1.3_U1) without all SEEK_* defined.... */

#ifndef SEEK_CUR
#define SEEK_CUR    1
#endif

#ifndef SEEK_END
#define SEEK_END    2
#endif

#ifndef SEEK_SET
#define SEEK_SET    0
#endif

voidpf ZCALLBACK fopen_file_func OF((
                                        voidpf opaque,
                                        const char* filename,
                                        int mode));

uLong ZCALLBACK fread_file_func OF((
                                       voidpf opaque,
                                       voidpf stream,
                                       void* buf,
                                       uLong size));

uLong ZCALLBACK fwrite_file_func OF((
                                        voidpf opaque,
                                        voidpf stream,
                                        const void* buf,
                                        uLong size));

long ZCALLBACK ftell_file_func OF((
                                      voidpf opaque,
                                      voidpf stream));

long ZCALLBACK fseek_file_func OF((
                                      voidpf opaque,
                                      voidpf stream,
                                      uLong offset,
                                      int origin));

int ZCALLBACK fclose_file_func OF((
                                      voidpf opaque,
                                      voidpf stream));

int ZCALLBACK ferror_file_func OF((
                                      voidpf opaque,
                                      voidpf stream));


voidpf ZCALLBACK fopen_file_func (opaque, filename, mode)
voidpf opaque;
const char* filename;
int mode;
{
	FILE* file = NULL;
	const char* mode_fopen = NULL;
	if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER)==ZLIB_FILEFUNC_MODE_READ)
		mode_fopen = "rb";
	else if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
		mode_fopen = "r+b";
	else if (mode & ZLIB_FILEFUNC_MODE_CREATE)
		mode_fopen = "wb";

	if ((filename!=NULL) && (mode_fopen != NULL))
		file = fopen(filename, mode_fopen);
	return file;
}


uLong ZCALLBACK fread_file_func (opaque, stream, buf, size)
voidpf opaque;
voidpf stream;
void* buf;
uLong size;
{
	uLong ret;
	ret = (uLong)fread(buf, 1, (size_t)size, (FILE *)stream);
	return ret;
}


uLong ZCALLBACK fwrite_file_func (opaque, stream, buf, size)
voidpf opaque;
voidpf stream;
const void* buf;
uLong size;
{
	uLong ret;
	ret = (uLong)fwrite(buf, 1, (size_t)size, (FILE *)stream);
	return ret;
}

long ZCALLBACK ftell_file_func (opaque, stream)
voidpf opaque;
voidpf stream;
{
	long ret;
	ret = ftell((FILE *)stream);
	return ret;
}

long ZCALLBACK fseek_file_func (opaque, stream, offset, origin)
voidpf opaque;
voidpf stream;
uLong offset;
int origin;
{
	int fseek_origin=0;
	long ret;
	switch (origin)
	{
	case ZLIB_FILEFUNC_SEEK_CUR :
		fseek_origin = SEEK_CUR;
		break;
	case ZLIB_FILEFUNC_SEEK_END :
		fseek_origin = SEEK_END;
		break;
	case ZLIB_FILEFUNC_SEEK_SET :
		fseek_origin = SEEK_SET;
		break;
	default:
		return -1;
	}
	ret = 0;
	fseek((FILE *)stream, offset, fseek_origin);
	return ret;
}

int ZCALLBACK fclose_file_func (opaque, stream)
voidpf opaque;
voidpf stream;
{
	int ret;
	ret = fclose((FILE *)stream);
	return ret;
}

int ZCALLBACK ferror_file_func (opaque, stream)
voidpf opaque;
voidpf stream;
{
	int ret;
	ret = ferror((FILE *)stream);
	return ret;
}

void fill_fopen_filefunc (pzlib_filefunc_def)
zlib_filefunc_def* pzlib_filefunc_def;
{
	pzlib_filefunc_def->zopen_file = fopen_file_func;
	pzlib_filefunc_def->zread_file = fread_file_func;
	pzlib_filefunc_def->zwrite_file = fwrite_file_func;
	pzlib_filefunc_def->ztell_file = ftell_file_func;
	pzlib_filefunc_def->zseek_file = fseek_file_func;
	pzlib_filefunc_def->zclose_file = fclose_file_func;
	pzlib_filefunc_def->zerror_file = ferror_file_func;
	pzlib_filefunc_def->opaque = NULL;
}

