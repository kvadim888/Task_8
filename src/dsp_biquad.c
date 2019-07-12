#include <stdio.h>

#define PRECISION30
#include "libdsp.h"

int32_t		dsp_filterBqI(t_biquadI *bq, int32_t input, t_noise_shape mode)
{
	t_sample	output = { 0 };

	fix_mac(&output.int64, bq->filter->a0, input);
	fix_mac(&output.int64, bq->filter->a1, bq->x1);
	fix_mac(&output.int64, bq->filter->a2, bq->x2);
	fix_msub(&output.int64, bq->filter->b1, bq->y1);
	fix_msub(&output.int64, bq->filter->b2, bq->y2);
    bq->x2 = bq->x1;
    bq->x1 = input;
    bq->y2 = bq->y1;
    bq->y1 = output.int32[1];
	bq->err = (mode == NOISE_SHAPE_ON)
			? output.int64 & (int64_t)0x800000003fffffff : 0;
    return output.int32[1];
}

int32_t		dsp_filterBqII(t_biquadII *bq, int32_t input, t_noise_shape mode)
{
	t_sample		output;

	output.int64 = bq->z1 + bq->err;
	fix_mac(&output.int64, input, bq->filter->a0);
	bq->z1 = bq->z2;
	fix_mac(&bq->z1, input, bq->filter->a1);
	fix_msub(&bq->z1, output.int32[1], bq->filter->b1);
	bq->z2 = 0;
	fix_mac(&bq->z2, input, bq->filter->a2);
	fix_msub(&bq->z2, output.int32[1], bq->filter->b2);
	bq->err = (mode == NOISE_SHAPE_ON)
			? output.int64 & (int64_t)0x800000003fffffff : 0;
    return output.int32[1];
}

t_bqcoeff	*dsp_newBiquad(t_biquad_type type, double peakGain,
							double Q, double Fc, double samplerate)
{
	t_bqcoeff	*filter = malloc(sizeof(t_bqcoeff));
    double		norm;
    double		V = pow(10, fabs(peakGain) / 20.0);
    double		K = tan(M_PI * Fc / samplerate);

	switch (type)
	{
	case biquad_LPF:
		norm = 1 / (1 + K / Q + K * K);
		filter->a0 = float_to_fix(K * K * norm);
		filter->a1 = fix_mul (2, filter->a0);
		filter->a2 = filter->a0;
		filter->b1 = float_to_fix(2 * (K * K - 1) * norm);
		filter->b2 = float_to_fix((1 - K / Q + K * K) * norm);
		break;

	case biquad_HPF:
		norm = 1 / (1 + K / Q + K * K);
		filter->a0 = float_to_fix(norm);
		filter->a1 = fix_mul(-2, filter->a0);
		filter->a2 = filter->a0;
		filter->b1 = float_to_fix(2 * (K * K - 1) * norm);
		filter->b2 = float_to_fix((1 - K / Q + K * K) * norm);
		break;

	case biquad_BPF:
		norm = 1 / (1 + K / Q + K * K);
		filter->a0 = float_to_fix(K / Q * norm);
		filter->a1 = 0;
		filter->a2 = -filter->a0;
		filter->b1 = float_to_fix(2 * (K * K - 1) * norm);
		filter->b2 = float_to_fix((1 - K / Q + K * K) * norm);
		break;

	default:
		printf("Undefined type of filter");
	}
	return (filter);
}