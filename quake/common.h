/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// common.h  -- general definitions

#ifndef __COMMON_H
#define __COMMON_H

#if !defined BYTE_DEFINED
typedef unsigned char 		byte;
#define BYTE_DEFINED 1
#endif

// KJB Undefined true and false defined in SciTech's DEBUG.H header
#undef true
#undef false

typedef enum {false, true}	qboolean;

#ifdef __DJGPP__
int vsnprintf(char *str, size_t n, const char *fmt, va_list ap);
#endif
#if defined(__DJGPP__) || defined(_WIN32)
char *strtok_r(char *s, const char *delim, char **last);
#endif

/* from Quake3 */
#ifdef _WIN32
__inline int Q_vsnprintf (char *Dest, size_t Count, const char *Format, va_list Args)
{
	int ret = _vsnprintf(Dest, Count, Format, Args);
	Dest[Count - 1] = 0;	// null terminate
	return ret;
}
#else
#define Q_vsnprintf  vsnprintf
#endif

// FIXME: DG: the following is a duplication from qcommon.h
#ifdef __GNUC__ // gcc or clang

// validate arguments for printf-like functions
// STRIDX: index of format string in function arguments (first arg == 1)
// FIRSTARGIDX: index of first argument for the format string (usually STRIDX+1)
#define ATTRIBUTE_PRINTF(STRIDX, FIRSTARGIDX) __attribute__ ((format (printf, STRIDX, FIRSTARGIDX)))

#else // MSVC and other compilers

// sorry, no printf-style-format validation for you :P
#define ATTRIBUTE_PRINTF(STRIDX, FIRSTARGIDX)
#pragma warning (disable:4996)

#endif // __GNUC__

//============================================================================

typedef struct sizebuf_s
{
	qboolean	allowoverflow;	// if false, do a Sys_Error
	qboolean	overflowed;		// set to true if the buffer size failed
	byte		*data;
	int			maxsize;
	int			cursize;
} sizebuf_t;

void SZ_Alloc (sizebuf_t *buf, int startsize);
void SZ_Clear (sizebuf_t *buf);
void *SZ_GetSpace (sizebuf_t *buf, int length);
void SZ_Write (sizebuf_t *buf, void *data, int length);
void SZ_Print (sizebuf_t *buf, char *data);	// strcats onto the sizebuf

//============================================================================

typedef struct link_s
{
	struct link_s	*prev, *next;
} link_t;


void ClearLink (link_t *l);
void RemoveLink (link_t *l);
void InsertLinkBefore (link_t *l, link_t *before);
void InsertLinkAfter (link_t *l, link_t *after);

// (type *)STRUCT_FROM_LINK(link_t *link, type, member)
// ent = STRUCT_FROM_LINK(link,entity_t,order)
// FIXME: remove this mess!
#define	STRUCT_FROM_LINK(l,t,m) ((t *)((byte *)l - (int)&(((t *)0)->m)))

//============================================================================

#ifndef NULL
#define NULL ((void *)0)
#endif

//============================================================================

extern	qboolean	bigendien;

extern	short	(*BigShort) (short l);
extern	short	(*LittleShort) (short l);
extern	int		(*BigLong) (int l);
extern	int		(*LittleLong) (int l);
extern	float	(*BigFloat) (float l);
extern	float	(*LittleFloat) (float l);

//============================================================================

/* FS: Gamespy stuff */
#define SHOW_POPULATED_SERVERS 1
#define SHOW_ALL_SERVERS 2

void MSG_WriteChar (sizebuf_t *sb, int c);
void MSG_WriteByte (sizebuf_t *sb, int c);
void MSG_WriteShort (sizebuf_t *sb, int c);
void MSG_WriteLong (sizebuf_t *sb, int c);
void MSG_WriteFloat (sizebuf_t *sb, float f);
void MSG_WriteString (sizebuf_t *sb, char *s);
void MSG_WriteCoord (sizebuf_t *sb, float f);
void MSG_WriteAngle (sizebuf_t *sb, float f);
void MSG_WriteAngle16 (sizebuf_t *sb, float f); //johnfitz

extern	int			msg_readcount;
extern	qboolean	msg_badread;		// set if a read goes beyond end of message

void MSG_BeginReading (void);
int MSG_ReadChar (void);
int MSG_ReadByte (void);
int MSG_ReadShort (void);
int MSG_ReadLong (void);
float MSG_ReadFloat (void);
char *MSG_ReadString (void);

float MSG_ReadCoord (void);
float MSG_ReadAngle (void);
float MSG_ReadAngle16 (void); //johnfitz

//============================================================================

void Q_strcpy (char *dest, char *src);
void Q_strncpy (char *dest, char *src, int count);
int Q_strlen (char *str);
char *Q_strrchr (char *s, char c);
void Q_strcat (char *dest, char *src);
int Q_strcmp (char *s1, char *s2);
int Q_strncmp (char *s1, char *s2, int count);
int Q_strcasecmp (char *s1, char *s2);
int Q_strncasecmp (char *s1, char *s2, int n);
size_t Q_strlcpy (char *dst, const char *src, size_t siz); /* FS: From OpenBSD */
size_t Q_strlcat (char *dst, const char *src, size_t siz); /* FS: From OpenBSD */

//============================================================================

extern	char		com_token[1024];
extern	qboolean	com_eof;

char *COM_Parse (char *data);


extern	int		com_argc;
extern	char	**com_argv;

int COM_CheckParm (char *parm);

/* FS: Quake 2 stuff */
int COM_Argc (void);
char *COM_Argv (int arg);
void COM_ClearArgv (int arg);

void COM_Init (void);
void COM_InitArgv (int argc, char **argv);

char *COM_SkipPath (char *pathname);
void COM_StripExtension (char *in, char *out);
void COM_FilePath (char *in, char *out);
void COM_DefaultExtension (char *path, const char *extension);

// does a varargs printf into a temp buffer
char	*va(const char *format, ...) ATTRIBUTE_PRINTF(1, 2);

//============================================================================

extern int com_filesize;
struct cache_user_s;

extern	char	com_gamedir[MAX_OSPATH];

void COM_WriteFile (const char *filename, void *data, int len);
int COM_OpenFile (const char *filename, int *hndl);
int COM_FOpenFile (const char *filename, FILE **file);
void COM_CloseFile (int h);

void COM_FreeFile (void *buffer);
byte *COM_LoadFile (const char *path);

/* FS: New stuff */
int Q_tolower(int c);
int Q_toupper(int c);

/* FS: From Q2 */
char *COM_NextPath (char *prevpath);
void COM_FreeFileList (char **list, int n);
qboolean COM_ItemInList (char *check, int num, char **list);
char **COM_ListFiles (char *findname, int *numfiles, unsigned musthave, unsigned canthave);

extern	struct cvar_s	*registered;

extern	qboolean	standard_quake, rogue, hipnotic;
extern	qboolean	warpspasm, nehahra, extended_mod; /* FS: For Nehara */

void CompleteCommand (void); /* FS: Autocomplete commands */
void Com_sprintf (char *dest, size_t size, char *fmt, ...); /* FS: Added */

#endif // __COMMON_H
