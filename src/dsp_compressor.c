#include "libdsp.h"

t_compressor	*dsp_newcompressor(double threshold, double ratio,
									double attack_time, double release_time,
									size_t samplerate)
{
	t_compressor *compressor = malloc(sizeof(t_compressor));
	if (compressor == NULL)
		return NULL;
	compressor->threshold = dsp_db2gain(threshold);
	compressor->slope = float_to_fix(1 - (1 / ratio));
	if (attack_time != 0.0)
		compressor->attack = float_to_fix(exp(-1.0 / (samplerate * attack_time)));
	else
		compressor->attack = 0;
	if (release_time != 0.0)
		compressor->release = float_to_fix(exp(-1.0 / (samplerate * release_time)));
	else
		compressor->release = 0;
	compressor->envelope = float_to_fix(0.0);
	return compressor;
}

int16_t	dsp_compressor(t_ringbuff *input, t_compressor *compressor,
						size_t samplerate, size_t lookahead, size_t window)
{
	int32_t	RMS = 0;
	int32_t	mode = 0;
	int32_t gain = float_to_fix(1.0);
	RMS = dsp_ringRMS(input, lookahead, window);
	mode = (RMS > compressor->envelope) ? compressor->attack : compressor->release;
	compressor->envelope = fix_add(compressor->envelope, fix_mul(mode, compressor->envelope));
	compressor->envelope = fix_add(compressor->envelope, fix_mul(fix_sub(float_to_fix(1.0), mode), RMS));
	if (compressor->envelope > compressor->threshold)
		gain = fix_sub(gain, fix_mul(fix_sub(compressor->envelope, compressor->threshold), compressor->slope));
	return fix_mul(gain, input->buff[input->carriage]);
}
