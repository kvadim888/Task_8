#include <stdio.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "getopt.h"
#include "libfix.h"
#include "libwav.h"
#include "libsignal.h"

#define	MONO	1
#define STEREO	2

#define SIGNAL_AMPLITUDE	(1U << 0)
#define SIGNAL_FREQUENCY	(1U << 1)
#define SIGNAL_PHASE		(1U << 2)
#define SIGNAL_RATE			(1U << 3)

#define TYPE_TONE			(1U << 0)
#define TYPE_LINSWEEP		(1U << 1)
#define TYPE_EXPSWEEP		(1U << 2)
#define TYPE_NOISE			(1U << 3)

int main(int ac, char **av)
{
	/* flags ----------------------------------------------------------------------------*/ 
	uint8_t		type_flag = 0;		// type of generator
	uint8_t		duration_flag = 0;	// duration of signal	
	uint8_t		signal_flag = 0;	// flag of signal settings (start)
	uint8_t		end_flag = 0;		// flag of signal settings (end)

	/* signal characteristics -----------------------------------------------------------*/
	double	duration = 10;			//	default 10 sec
	double	amplitude = 1;			//	default 1
	double	freq1 = 100;			//	default 100 Hz
	double	freq2 = freq1;			//	default freq1 == freq2
	double	phase = 0;				//	default 0

	/* properties of files --------------------------------------------------------------*/
	char		*filepath = NULL;	// name of file with generated signal
	int32_t		*FIR = NULL;		// FIR filter structure
	size_t		FIRlen = 0;

	/* parsing CLI arguments ------------------------------------------------------------*/
	if (ac < 2)
	{
		printf("Invalid number of args\n");
		exit(1);
	}

	int opt_index = 0;
	while ((opt_index = getopt(ac, av, "n:w:t:g:a:f:p:")) != -1)
	{
		switch (opt_index)
		{
		case 'n':
			filepath = optarg;
			break;
		case 'w':
			FIRlen = dsp_FIRimport(&FIR, optarg);
			assert(FIRlen > 0);
			break;
		case 'g':
			if (type_flag)
				break;
			if (strcmp(optarg, "tone") == 0)
				type_flag |= TYPE_TONE;
			else if (strcmp(optarg, "linear") == 0)
				type_flag |= TYPE_LINSWEEP;
			else if (strcmp(optarg, "exp") == 0)
				type_flag |= TYPE_EXPSWEEP;
			else if (strcmp(optarg, "noise") == 0)
				type_flag |= TYPE_NOISE;
			else
			{
				printf("%s: Invalid type of generator\n", optarg);
				exit(1);
			}
			break;
		case 'a':
			if ((signal_flag & SIGNAL_AMPLITUDE) == 0)
			{
				if (amplitude > 0)
				{
					printf("signal.amplitude > 0 dB\n");
					exit(1);
				}
				amplitude = dsp_db2gain(atof(optarg));
				signal_flag |= SIGNAL_AMPLITUDE;
			}
			else
			{
				if (amplitude > 0)
				{
					printf("end.amplitude > 0 dB\n");
					exit(1);
				}
				amplitude = dsp_db2gain(atof(optarg));
				end_flag |= SIGNAL_AMPLITUDE;
			}
			break;
		case 'f':
			if ((signal_flag & SIGNAL_FREQUENCY) == 0)
			{
				freq1 = atof(optarg);
				if (freq1 < 0)
				{
					printf("freq1 < 0\n");
					exit(1);
				}
				signal_flag |= SIGNAL_FREQUENCY;
			}
			else
			{
				freq2 = atof(optarg);
				if (freq2 < 0)
				{
					printf("freq2 < 0\n");
					exit(1);
				}
				end_flag |= SIGNAL_FREQUENCY;
			}
			break;
		case 'p':
			if ((signal_flag & SIGNAL_PHASE) == 0)
			{
				phase = atof(optarg);
				signal_flag |= SIGNAL_PHASE;
			}
			else
			{
				phase = atof(optarg);
				end_flag|= SIGNAL_PHASE;
			}
			break;
		case 't':
			duration = atof(optarg);
			if (duration <= 0)
			{
				printf("duration <= 0\n");
				exit(1);
			}
			break;
		case '?':
			printf("unknown option : %c", opt_index);
			exit(1);
		default:
			printf("optarg = %s\n", optarg);
			filepath = optarg;
			break;
		}
	}

	assert(FIR);

	/* WAV-file initialisation -----------------------*/
	t_wavheader wavheader;
	wav_headerinit(&wavheader, duration, STEREO, CD_RATE);
	t_wavfile *wavfile = wav_wropen(filepath, &wavheader);
	wav_bufferinit(wavfile, FIRlen);
	
	/* ring buffer initialisation --------------------*/
	t_ringbuff *ring = dsp_newring(FIRlen, (uint32_t)FIRlen - 1);

	t_sample sample;
	size_t length = 0;
	size_t len = 0;
	double time = 0;
	while (length < wavfile->header.data_size / sizeof(int16_t))
	{
		memset(wavfile->buffer, 0, wavfile->buflen);
		len = FIX_MIN(wavfile->header.data_size / sizeof(int16_t) - length, 
						wavfile->buflen / sizeof(int16_t));
		for (size_t i = 0; i < len; i += wavfile->header.channels)
		{
			wavfile->buffer[i] = signal_tone(amplitude, freq1, phase, time / wavfile->header.sample_rate);
			ring->buff[ring->carriage] = wavfile->buffer[i];
			sample.int32[1] = dsp_FIR(ring, FIR, FIRlen);
			wavfile->buffer[i + 1] = sample.int16[3];
			ring->carriage = (ring->carriage + 1) & (ring->length - 1);
			time++;
		}
		length += wav_write(wavfile, len);
	}
	wav_info(wavfile);
	fclose(wavfile->fs);
	return 0;
}