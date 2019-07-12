#ifndef LIBWAV_H
#define LIBWAV_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include "libdsp.h"

/*sample size*/
#define PCM_SIZE sizeof(int16_t)

/*samplerate*/
#define CD_RATE		44100
#define DAT_RATE	48000

/* num of channels*/
#define	MONO	1
#define STEREO	2

typedef enum
{
	READ = 0,
	WRITE
}	t_RWtype;

#pragma pack(push, 1)

typedef struct
{
	uint16_t	*data;
	size_t		datalen;
}	t_buffIO;

typedef struct
{
	char		riff[4];				// "RIFF"
	uint32_t	overall_size;			// file_size
	char		wave[4];				// "WAVE"
	char		fmt_chunk_marker[4];	// "fmt " string with trailing null char
	uint32_t	length_of_fmt;			// length of the format data
	uint16_t	format_type;			// format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
	uint16_t	channels;				// channels' amount
	uint32_t	sample_rate;			// sampling rate (blocks per second)
	uint32_t	byterate;				// SampleRate * NumChannels * BitsPerSample/8
	uint16_t	block_align;			// NumChannels * BitsPerSample/8
	uint16_t	bits_per_sample;		// bits per sample, 8- 8bits, 16- 16 bits etc
	char		data_chunk_header[4];	// "DATA" or "FLLR"
	uint32_t	data_size;				// NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read
}	t_wavheader;

typedef struct
{
	t_ringbuff	**data;
	uint8_t		channels;
}	t_wavbuffer;

typedef struct
{
	FILE		*fs;
	char		*path;
	t_RWtype	RWtype;
	t_wavheader header;
	size_t		datalen;
	int16_t		*buffer;
	size_t		buflen;
}	t_wavfile;

#pragma pack(pop)

int			wav_headerinit(t_wavheader *wavheader, double duration, uint16_t channels, size_t samplerate);
int			wav_bufferinit(t_wavfile *wavfile, size_t length);

t_wavfile	*wav_rdopen(const char *path);
t_wavfile	*wav_wropen(const char *path, t_wavheader *header);

size_t		wav_getchannel(int16_t *buff, t_wavfile *wavfile, uint8_t channel, size_t length);
size_t		wav_setchannel(t_wavfile *wavfile, int16_t *buff, uint8_t channel, size_t length);

size_t		wav_read(t_wavfile *file);
size_t		wav_write(t_wavfile *file, size_t datalen);

void		wav_info(t_wavfile *wavfile);
void		wav_close(t_wavfile **wavfile);

#endif	//LIBFIX_H
