/**
 * \file snd-win.c
 * \brief Shared Windows sound backend
 *
 * Copyright (c) 1997 Ben Harrison, Skirmantas Kligys, Robert Ruehlmann,
 * and others
 *
 * This work is free software; you can redistribute it and/or modify it
 * under the terms of either:
 *
 * a) the GNU General Public License as published by the Free Software
 *    Foundation, version 2, or
 *
 * b) the "Angband licence":
 *    This software may be copied and distributed for educational, research,
 *    and not for profit purposes provided that this copyright and statement
 *    are included in all such copies.  Other copyrights may also apply.
 *
 */
#include "angband.h"

#if defined(WINDOWS) && defined(SOUND) && !defined(SOUND_SDL) && !defined(SOUND_SDL2)

#include <windows.h>
#include <mmsystem.h>
#include "sound.h"
#include "snd-win.h"

/* Supported file types */
enum {
	WIN_NULL = 0,
	WIN_MP3,
	WIN_WAV
};

static const struct sound_file_type supported_sound_files[] = { {".mp3", WIN_MP3},
							 {".wav", WIN_WAV},
							 {"", WIN_NULL} };

typedef struct
{
	int		type;
	MCI_OPEN_PARMS	op;
	char		*filename;
} win_sample;

/**
 * Load a sound
 */
static bool load_sound_win(const char *filename, int ftyp,
		struct sound_data *sd)
{
	win_sample *sample = NULL;

	sample = (win_sample *)(sd->plat_data);

	switch (ftyp) {
		case WIN_MP3:
			if (!sample)
				sample = mem_zalloc(sizeof(*sample));

			/* Open if not already */
			if (!sample->op.wDeviceID) {
				sample->op.dwCallback = 0;
				sample->op.lpstrDeviceType = (char*)MCI_ALL_DEVICE_ID;
				sample->op.lpstrElementName = filename;
				sample->op.lpstrAlias = NULL;

				/* Open command */
				if (mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_WAIT,
						(size_t)(&sample->op)) != 0)
					sample->op.wDeviceID = 0;
			}

			if (0 != sample->op.wDeviceID) {
				sd->status = SOUND_ST_LOADED;
			} else {
				mem_free(sample);
				sample = NULL;
			}
			break;

		case WIN_WAV:
			if (!sample)
				sample = mem_alloc(sizeof(*sample));

			sample->filename = mem_zalloc(strlen(filename) + 1);
			my_strcpy(sample->filename, filename, strlen(filename) + 1);
			sd->status = SOUND_ST_LOADED;
			break;

		default:
			plog_fmt("Sound: Oops - Unsupported file type");
			break;
	}

	if (sample) {
		sample->type = ftyp;
	}
	sd->plat_data = (void *)sample;

	return (NULL != sample);
}

/**
 * Play a sound
 */
static bool play_sound_win(struct sound_data *sd)
{
	MCI_PLAY_PARMS pp = {0};

	win_sample *sample = (win_sample *)(sd->plat_data);

	if (sample) {
		switch (sample->type) {
			case WIN_MP3:
				if (sample->op.wDeviceID) {
					/* Play command */
					pp.dwCallback = 0;
					pp.dwFrom = 0;
					return (!mciSendCommand(sample->op.wDeviceID, MCI_PLAY, MCI_FROM, (size_t)&pp));
				}
				break;

			case WIN_WAV:
				if (sample->filename)
				{
					/* If another sound is currently playing, stop it */
					if (PlaySound(NULL, 0, SND_PURGE))
						/* Play the sound, catch errors */
						return PlaySound(sample->filename, 0, SND_FILENAME | SND_ASYNC);
				}
				break;
			default:
				/* Not supported */
				break;
		}
	}

	return true;
}

static bool unload_sound_win(struct sound_data *sd)
{
	win_sample *sample = (win_sample *)(sd->plat_data);

	if (sample) {
		switch (sample->type) {
			case WIN_MP3:
				if (sample->op.wDeviceID)
					mciSendCommand(sample->op.wDeviceID, MCI_CLOSE, MCI_WAIT, (size_t)(&sample->op));

				break;

			case WIN_WAV:
				mem_free(sample->filename);
				break;

			default:
				break;
		}

		mem_free(sample);
		sd->plat_data = NULL;
		sd->status = SOUND_ST_UNKNOWN;
	}

	return true;
}

static bool open_audio_win(void)
{
	return true;
}

static bool close_audio_win(void)
{
	PlaySound(NULL, 0, 0);
	return true;
}

static const struct sound_file_type *supported_files_win(void)
{
	return supported_sound_files;
}

/**
 * Initialize sound
 */
errr init_sound_win(struct sound_hooks *hooks, int argc, char **argv)
{
	hooks->open_audio_hook = open_audio_win;
	hooks->supported_files_hook = supported_files_win;
	hooks->close_audio_hook = close_audio_win;
	hooks->load_sound_hook = load_sound_win;
	hooks->unload_sound_hook = unload_sound_win;
	hooks->play_sound_hook = play_sound_win;

	/* Success */
	return (0);
}
#endif /* SOUND && !SOUND_SDL && !SOUND_SDL2 */
