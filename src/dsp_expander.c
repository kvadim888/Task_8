#include "libdsp.h"

t_expander	*dsp_newexpander(double threshold, double ratio,
							double attack_time, double release_time,
							size_t samplerate)
{
	t_expander *expander = malloc(sizeof(t_expander));
	if (expander == NULL)
		return NULL;
	expander->threshold = dsp_db2gain(threshold);
	expander->slope = float_to_fix(1 - (1 / ratio));
	if (attack_time != 0.0)
		expander->attack = float_to_fix(exp(-1.0 / (samplerate * attack_time)));
	else
		expander->attack = 0;
	if (release_time != 0.0)
		expander->release = float_to_fix(exp(-1.0 / (samplerate * release_time)));
	else
		exapnder->release = 0;
	expander->envelope = float_to_fix(0.0);
	return compressor;
}

int16_t	dsp_enxpander(t_ringbuff *input, t_expander *expander,
						size_t samplerate, size_t lookahead, size_t window)
{
	int32_t	RMS = 0;
	int32_t	mode = 0;
	int32_t gain = float_to_fix(1.0);
	RMS = dsp_ringRMS(input, lookahead, window);
	mode = (RMS > expander->envelope) ? compressor->attack : compressor->release;
	expander->envelope = fix_add(compressor->envelope, fix_mul(mode, compressor->envelope));
	expander->envelope = fix_add(compressor->envelope, fix_mul(fix_sub(float_to_fix(1.0), mode), RMS));
	if (expander->envelope > expander->threshold)
		gain = fix_mul(fix_sub(expander->envelope, expander->threshold), expander->slope);
    int32_t result = fix_add(input->buff[input->carriage], fix_mul(gain, input->buff[input->carriage]);
	if (result > (int32_t)INT16_MAX)
		return INT16_MAX;
	if (result < (int32_t)INT16_MIN)
		return INT16_MIN;
}
