#include "libdsp.h"

int32_t	dsp_allpass_init(double Fc, double samplerate)
{
	double C = -cos(2 * M_PI * Fc / samplerate)
									/(1 + sin(2 * M_PI * Fc / samplerate));
	return float_to_fix(C);
}

int32_t	dsp_allpass(int32_t C, int32_t output, int32_t input, int32_t *z1)
{
	t_sample sample;
	sample.int64 = ((int64_t)*z1 << 31) + C * input - C * output;
	*z1 = input;
	sample.int64 = (int64_t)sample.int64 >> 31;
	return fix_saturate(sample.int64);
}

int32_t	dsp_allpassLPF(int32_t C, int32_t output, int32_t input, int32_t *z1)
{
	return ((int64_t)input + dsp_allpass(C, output, input, z1)) >> 1;
}

int32_t	dsp_allpassHPF(int32_t C, int32_t output, int32_t input, int32_t *z1)
{
	return ((int64_t)input - dsp_allpass(C, output, input, z1)) >> 1;
}
