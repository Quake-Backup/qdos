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

#ifndef __ZONE_H
#define __ZONE_H

typedef struct zhead_s
{
	struct zhead_s	*prev, *next;
	short	magic;
	short	tag;			// for group free
	size_t	size;
} zhead_t;

extern zhead_t		z_chain;

void Z_Free (void *ptr);
void *Z_Malloc (size_t size);			// returns 0 filled memory
void *Z_TagMalloc (size_t size, int tag);
void Z_FreeTags (int tag);

// large block stack allocation routines
void	*Hunk_Begin (size_t maxsize);
void	*Hunk_Alloc (size_t size);
void	Hunk_Free (void *buf);
size_t	Hunk_End (void);

#endif // __ZONE_H
