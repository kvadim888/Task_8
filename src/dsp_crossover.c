#include "libdsp.h"

//void	dsp_crossover(int32_t *low, int32_t *high, int32_t input, t_allpass *lpf, t_allpass *hpf)
//{
//	*low = dsp_allpasslpf(lpf->C, *low, input, &lpf->z1);
//	*high = dsp_allpasshpf(hpf->C, *high, input, &hpf->z1);
//}

void	dsp_crossover(int32_t *low, int32_t *high, int32_t input,
					t_biquadII *LPF, t_biquadII *HPF)
{
	*low = dsp_filterBqII(LPF, input, NOISE_SHAPE_ON);
	*high = dsp_filterBqII(HPF, input, NOISE_SHAPE_ON);
}
