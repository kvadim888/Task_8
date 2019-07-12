#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include <math.h>

#include "libfix.h"
#include "libwav.h"
#include "libsignal.h"
#include "libdsp.h"

#ifdef _WIN32
	#include <windows.h>
	#include "getopt.h"
#elif defined __unix__
	#include <unistd.h>
#endif

int main(int ac, char **av)
{
	assert(ac == 2);
	char *filepath = av[1];
	double duration = 20.0; /* seconds */

	/* WAV-file initialization -----------------------------------------------*/
	t_wavheader wavheader;
	wav_headerinit(&wavheader, duration, PCM_SIZE, STEREO, CD_RATE);
	t_wavfile *wavfile = wav_wropen(filepath, &wavheader);
	wav_bufferinit(&wavfile, LOOKAHEAD_SIZE);
	t_wavbuffer wavbuffer;

	t_signal start =
	{
		.amplitude = 0.1,
		.frequency = 100,
		.phase = 0,
		.samplerate = 44100
	};

	t_signal end =
	{
		.amplitude = 1,
		.frequency = 5000,
		.phase = 0,
		.samplerate = 44100
	};

	t_sample sample = { 0 };

	double threshold = -5; //dB 
	double ratio = 4.0;
	double attack_time = 0.0001; // seconds
	double release_time = 0.001; // seconds

	size_t bufflen = 0;
	size_t len = 0; // loop counter


	t_ringbuff *ring = dsp_newring(WINDOW_SIZE + 2 * LOOKAHEAD_SIZE, 0);
	t_compressor *compressor = dsp_newcompressor(threshold, ratio, attack_time,
				release_time, wavfile->header.sample_rate);

	double time = 0;

	for (size_t i = 0; i < LOOKAHEAD_SIZE + WINDOW_SIZE; i++)
	{
		ring->buff[(ring->carriage + i) & (ring->length - 1)]
			= signal_harmonic(&start, &end, duration, time / wavheader.sample_rate);
		time++;
	}

	while (length < wavfile->datalen)
	{
		memset(wavfile->buffer, 0, wavfile->buflen);
		len = FIX_MIN(wavfile->header.datalen - length, LOOKAHEAD_SIZE);
		for (size_t i = 0; i < len; i += wavfile->header.channels)
		{
			ring->buff[(ring->carriage + LOOKAHEAD_SIZE + WINDOW_SIZE) & (ring->length - 1)]
				= signal_harmonic(&start, &end, duration, time / wavheader.sample_rate);
			wavfile->buffer[i] = ring->buff[ring->carriage];
			wavfile->buffer[i + 1] = dsp_compressor(ring, compressor, wavfile->header.sample_rate, LOOKAHEAD_SIZE, WINDOW_SIZE);
			ring->carriage = (ring->carriage + 1) & (ring->length - 1);
			time++;
		}
		length += wav_write(wavfile, len);
	}
	wav_info(wavfile);
	wav_close(&wavfile);
	return 0;
} 