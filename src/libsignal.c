#include <stdlib.h>
#include "libsignal.h"

int16_t		signal_harmonic(t_signal *start, t_signal *end,
							double duration, double time)
{
	t_sample	sample = { 0 };
	double		k_amp = (end->amplitude - start->amplitude) / duration;
	double		k_freq = (end->frequency - start->frequency) / duration;

	sample.int32[1] = float_to_fix((0.5 * k_amp * time + start->amplitude) *
		sin(2 * M_PI * time * (0.5 * k_freq * time + start->frequency)));
	return sample.int16[3];
}

int16_t		signal_tone(double amplitude, double frequency,
						double phase, double time)
{
	t_sample	sample = { 0 };

	sample.int32[1] = float_to_fix(amplitude *
										sin(2*M_PI * frequency * time + phase));
	return sample.int16[3];
}

int16_t		signal_linsweep(double amplitude, double freq1, double freq2,
							double phase, double duration, double time)
{
	t_sample	sample = { 0 };
	double		k = (freq2 - freq1) / duration;

	sample.int32[1] = float_to_fix(amplitude *
		sin(phase + 2 * M_PI * time * (0.5 * k * time + freq1)));
	return sample.int16[3];
}

int16_t		signal_expsweep(double amplitude, double freq1, double freq2,
							double phase, double duration, double time)
{
	t_sample	sample = { 0 };
	double		k = pow(freq2 / freq1, 1 / duration);

	sample.int32[1] = float_to_fix(amplitude *
			sin(phase + 2*M_PI * freq1 * ((pow(k, time) - 1) / log(k))));
	return sample.int16[3];
}

int16_t		signal_noise(double amplitude)
{
	t_sample	sample = { 0 };

	sample.int32[1] = float_to_fix(amplitude) * (0.5 - rand());
	return (int16_t)sample.int16[3];
}
