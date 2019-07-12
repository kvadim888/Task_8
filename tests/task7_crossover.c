#include <stdio.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "getopt.h"
#include "libfix.h"
#include "libwav.h"
#include "libdsp.h"
#include "libsignal.h"

#define	MONO	1
#define STEREO	2

int main(int ac, char **av)
{
	/* flags ----------------------------------------------------------------------------*/ 
	uint8_t		type_flag = 0;		// type of generator
	uint8_t		duration_flag = 0;	// duration of signal	
	uint8_t		signal_flag = 0;	// flag of signal settings (start)
	uint8_t		end_flag = 0;		// flag of signal settings (end)

	/* signal characteristics -----------------------------------------------------------*/
	double	duration = 10;		//	default 10 sec
	double	amplitude = 1;		//	default 1
	double	freq1 = 100;		//	default 100 Hz
	double	freq2 = 200*freq1;	//	default freq1 == freq2
	double	phase = 0;			//	default 0

	/* properties of files --------------------------------------------------------------*/
	char	*filepath = NULL;	// name of file with generated signal

	/* parsing CLI arguments ------------------------------------------------------------*/
	if (ac < 2)
	{
		printf("Invalid number of args\n");
		exit(1);
	}

	filepath = av[1];

	/* Biquad filter properties ---------------------------------------------------------*/
	double K = 0;
	double peakGain = 0;
	double Fc = 500;
	double bandwidth = 0.9;

	double coeff = -cos(2 * M_PI * Fc / CD_RATE) / (sin(2 * M_PI * Fc / CD_RATE) + 1);
	int32_t C = float_to_fix(coeff);
	t_allpass LPF = { .C = C, .z1 = 0 };
	t_allpass HPF = { .C = C, .z1 = 0 };

	t_biquadII LPF =
	{
		.filter = dsp_newBiquad(biquad_LPF, peakGain, bandwidth, Fc, samplerate),
		.z1 = 0, .z2 = 0,
		.err = 0
	};

	t_biquadII HPF =
	{
		.filter = dsp_newBiquad(biquad_HPF, peakGain, bandwidth, 3*Fc, samplerate),
		.z1 = 0, .z2 = 0,
		.err = 0
	};

	/* WAV-file initialisation -----------------------*/
	t_wavheader wavheader;
	wav_headerinit(&wavheader, duration, STEREO, CD_RATE);
	t_wavfile *wavfile = wav_wropen(filepath, &wavheader);
	wav_bufferinit(wavfile, 100);
	
	t_sample sample;

	t_sample low_out;
	t_sample high_out;

	size_t length = 0;
	size_t len = 0;
	double time = 0;
	while (length < wavfile->datalen)
	{
		memset(wavfile->buffer, 0, wavfile->buflen);
		len = FIX_MIN(wavfile->datalen - length, wavfile->buflen);
		for (size_t i = 0; i < len; i += wavfile->header.channels)
		{
			sample.int16[3] = signal_expsweep(amplitude, freq1, freq2, phase, duration, time / wavfile->header.sample_rate);
			dsp_crossover(&low_out.int32[1], &high_out.int32[1], sample.int32[1], &LPF, &HPF);
			wavfile->buffer[i] = low_out.int16[3];
			wavfile->buffer[i + 1] = high_out.int16[3];
			time++;
		}
		length += wav_write(wavfile, len);
	}
	wav_info(wavfile);
	fclose(wavfile->fs);
	return 0;
}