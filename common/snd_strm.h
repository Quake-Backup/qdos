#ifndef __SND_STRM_H
#define __SND_STRM_H

typedef signed short      strm_int16_t;

typedef struct
{
	long totallen;
	long datarate;
	void *drmp3;
	void *drwav;
	void *drflac;
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
void S_StartStreamBackgroundTrack (const char *name);
void S_StopStreamBackgroundTrack (void);
void S_PauseStreamBackgroundTrack (void);
void S_ResumeStreamBackgroundTrack (void);

#define MAX_STREAMS	1

extern stream_t stream_channels[MAX_STREAMS];

#endif // __SND_STRM_H
