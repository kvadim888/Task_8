#ifndef LIBDSP_H
#define LIBDSP_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "libfix.h"

/*===========  BASIC DATA STRUCTURES  ========================================*/

#define RING_BUFF_LEN 128

typedef struct	s_ringbuff
{
	int16_t		*buff;		/* head of buffer */
	uint16_t	carriage;	/* carriage for moving on buffer */
	size_t		length;		/* length of buffer */
}				t_ringbuff;

/*============================================================================*/

/*===========  TOOLS  ============================ libdsp.c ==================*/

int32_t		dsp_db2gain(double gain);
int32_t		dsp_ringRMS(t_ringbuff *ring, size_t offset, size_t amount);

t_ringbuff	*dsp_newring(size_t length, uint32_t offset);
void		dsp_ringload(t_ringbuff *ring, const int16_t *buff,
						size_t length, size_t offset);
void		dsp_ringpull(int16_t *buff, t_ringbuff *ring,
						size_t length, size_t offset);

/*============================================================================*/

/*===========  FIR FILTER  ========================= dsp_FIR.c ===============*/

size_t	dsp_FIRimport(int32_t **buff, const char * path);
int32_t	dsp_FIR(t_ringbuff *input, int32_t *coeffs, size_t filterlen);

/*============================================================================*/

/*===========  BIQUAD IIR FILTER  =============== dsp_biquad.c ===============*/

// #define PRECISION30

typedef struct
{
	int32_t		a0;
	int32_t		a1;
	int32_t		a2;
	int32_t		b1;
	int32_t		b2;
}				t_bqcoeff;

typedef struct
{
	t_bqcoeff	*filter;
	int32_t		x1;
	int32_t		x2;
	int32_t		y1;
	int32_t		y2;
	int64_t		err;
}				t_biquadI;

typedef struct
{
	t_bqcoeff	*filter;
	int64_t		z1;
	int64_t		z2;
	int64_t		err;
}				t_biquadII;

typedef enum { NOISE_SHAPE_OFF = 0, NOISE_SHAPE_ON }	t_noise_shape;

typedef	enum
{
    biquad_LPF,
    biquad_HPF,
    biquad_BPF,
    biquad_LSF,
    biquad_HSF
}		t_biquad_type;

int32_t		dsp_filterBqI(t_biquadI *bq, int32_t input, t_noise_shape mode);

int32_t		dsp_filterBqII(t_biquadII *bq, int32_t input, t_noise_shape mode);

t_bqcoeff	*dsp_newBiquad(t_biquad_type type, double peakGain,
							double Q, double Fc, double samplerate);

// #undef PRECISION30

/*============================================================================*/

/*===========  ALLPASS FILTER  ================== dsp_allpass.c ==============*/

typedef struct
{
	int32_t	z1;
	int32_t	C;
}		t_allpass;

int32_t	dsp_allpassCoeff(double Fc, double samplerate); // coefficient C

int32_t	dsp_allpass(int32_t C, int32_t output, int32_t input, int32_t *z1);

int32_t	dsp_allpassLPF(int32_t C, int32_t output, int32_t input, int32_t *z1);

int32_t	dsp_allpassHPF(int32_t C, int32_t output, int32_t input, int32_t *z1);

/*============================================================================*/

/*===========  CROSSOVER  ======================= dsp_crossover.c ============*/

/*
**	BAD VARIANT
**	
** void	dsp_crossover(int32_t *low,			// low frequency output pointer
** 						int32_t *high,		// high frequency output pointer
** 						int32_t input,		// input value
** 						t_allpass *LPF,		// low-pass filter (based on all-pass)
** 						t_allpass *HPF);	// high-pass filter (based on all-pass)
*/

void	dsp_crossover(int32_t *low,		// low frequency output pointer
					int32_t *high,		// high frequency output pointer
					int32_t input,		// input value
					t_biquadII *LPF,	// low-pass filter (based on biquad)
					t_biquadII *HPF);	// high-pass filter (based on biquad)

/*============================================================================*/

#define LOOKAHEAD_SIZE	135	// equals to 3 msec (44100 Hz)
#define WINDOW_SIZE		LOOKAHEAD_SIZE / 3	// equals to 1 msec (44100 Hz)

/*===========  COMPRESSOR  ====================== dsp_compressor.c ===========*/

typedef struct
{
	int32_t attack;
	int32_t release;
	int32_t slope;
	int32_t threshold;
	int32_t envelope;
}	t_compressor;

t_compressor	*dsp_newcompressor( double threshold,	// threshold (in dB)
									double ratio,		// compression percentage
									double attack_time,
									double release_time,
									size_t samplerate);


int16_t 		dsp_compressor( t_ringbuff *input,		// signal
								t_compressor *compressor,
								size_t samplerate,	// 44100 Hz default
								size_t lookahead,
								size_t window);

/*============================================================================*/

/*===========  LIMITER  ========================== dsp_limiter.c =============*/

typedef struct
{
	int32_t attack;
	int32_t release;
	int32_t threshold;
	int32_t envelope;
}	t_limiter;

t_limiter	*dsp_newlimiter(double threshold,	// threshold (in dB)
							double attack_time,
							double release_time,
							size_t samplerate);

int16_t 	dsp_limiter(t_ringbuff *input,		// signal
					t_limiter *limiter,
					size_t samplerate,	// 44100 Hz default
					size_t lookahead,
					size_t window);

/*===========  EXPANDER  ======================== dsp_expander.c =============*/

typedef struct
{
	int32_t attack;
	int32_t release;
	int32_t slope;
	int32_t threshold;
	int32_t envelope;
}	t_expander;

t_expander	dsp_newexpander(double threshold,	// threshold (in dB)
							double ratio,		// expander percentage
							double attack_time,
							double release_time,
							size_t samplerate);

int16_t		dsp_enxpander(	t_ringbuff *input,		// signal
							t_expander *expander,
							size_t samplerate,	// 44100 Hz default
							size_t lookahead,
							size_t window);

/*============================================================================*/

#endif //LIBWAV_H
