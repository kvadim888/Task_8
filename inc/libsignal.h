#ifndef LIBSIGNAL_H
#define LIBSIGNAL_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "libfix.h"

typedef struct	s_signal
{
	double	amplitude;
	double	frequency;
	double	phase;
	double	samplerate;
}			t_signal;

int16_t		signal_harmonic(t_signal *start,
							t_signal *end,
							double duration,
							double time);

int16_t		signal_tone(double amplitude,
						double frequency,
						double phase,
						double time);

int16_t		signal_linsweep(double amplitude,
							double freq1,
							double freq2,
							double phase,
							double duration,
							double time);

int16_t		signal_expsweep(double amplitude, 
							double freq1,
							double freq2,
							double phase,
							double duration,
							double time);

int16_t		signal_noise(double amplitude);

#endif //LIBSIGNAL_H
