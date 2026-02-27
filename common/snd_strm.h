/*
Copyright 2026 Frank Sapone

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
S_StreamRawSamples function is GPL code re-adapated from Q2E OGG streaming S_RawSamples function.
----------------------------------------------------------------------
Copyright (C) 1997-2001 Id Software, Inc.

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

#ifndef __SND_STRM_H
#define __SND_STRM_H

typedef signed short      strm_int16_t;

typedef enum
{
	STREAM_MP3,
	STREAM_WAV,
	STREAM_FLAC
} stream_type_t;

typedef struct
{
	long totallen;
	long datarate;
	unsigned int sampleRate;
	unsigned int channels;
	void *drmp3;
	void *drwav;
	void *drflac;
	stream_type_t type;
} hSTREAM;

typedef struct stream_s
{
	hSTREAM*	handle;
	char		name[MAX_OSPATH];
	qboolean	active;
	qboolean	is3D;
	qboolean	looping;
	float		volume;
	int			leftvol;
	int			rightvol;
} stream_t;

void S_Destroy_Stream (stream_t *stream);
void S_StreamUpdate (void);
void S_StreamInit (void);
void S_StreamShutdown (void);
void S_StreamRestart (void);
void S_StartStreamBackgroundTrack (const char *name);
void S_StopStreamBackgroundTrack (void);
void S_PauseStreamBackgroundTrack (void);
void S_ResumeStreamBackgroundTrack (void);

#define MAX_STREAMS	1 /* FS: One channel, but can support more as this code is also adapted for use in Daikatana 1.3 which makes extensive use of additional streaming channels. */

extern stream_t stream_channels[MAX_STREAMS];

#endif // __SND_STRM_H
