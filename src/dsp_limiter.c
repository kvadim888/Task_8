#include "libdsp.h"

t_limiter	*dsp_newlimiter(double threshold, double attack_time,
							double release_time, size_t samplerate)
{
	t_limiter *limiter = malloc(sizeof(t_compressor));
	if (limiter == NULL)
		return NULL;
	limiter->threshold = dsp_db2gain(threshold);
	if (attack_time != 0.0)
		limiter->attack = float_to_fix(exp(-1.0 / (samplerate * attack_time)));
	else
		limiter->attack = 0;
	if (release_time != 0.0)
		limiter->release = float_to_fix(exp(-1.0 / (samplerate * release_time)));
	else
		limiter->release = 0;
	limiter->envelope = float_to_fix(0.0);
	return limiter;
}

int16_t	dsp_limiter(t_ringbuff *input, t_limiter *limiter,
					size_t samplerate, size_t lookahead, size_t window)
{
	int32_t	RMS = 0;
	int32_t	mode = 0;
	int32_t gain = float_to_fix(1.0);
	RMS = dsp_ringRMS(input, lookahead, window);
       mode = (RMS > limiter->envelope) ? limiter->attack : limiter->release;
	limiter->envelope = fix_add(limiter->envelope, fix_mul(mode, limiter->envelope));
	limiter->envelope = fix_add(limiter->envelope, fix_mul(fix_sub(float_to_fix(1.0), mode), RMS));
	if (limiter->envelope > limiter->threshold)
		gain = gain - fix_sub(limiter->envelope, limiter->threshold);
	return fix_mul(gain, input->buff[(input->carriage) % input->length]);
}