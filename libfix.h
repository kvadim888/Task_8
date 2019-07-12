#ifndef LIBFIX_H
#define LIBFIX_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h>
#include <stdio.h>

#include <assert.h>

#ifdef __XTENSA__
	#include <xtensa/hal.h>
	#include <xtensa/config/defs.h>
	#include <xtensa/tie/xt_hifi2.h>
#endif

#ifdef PRECISION30
	#define PRECISION		30 
#else
	#define PRECISION		31 
#endif

#define SCALE			(double)(1LL << PRECISION)

#define FIX_MIN(x,y)	((x)<(y)) ? (x) : (y)
#define FIX_MAX(x,y)	((x)<(y)) ? (y) : (x)

#define M_PI            3.14159265358979323846

typedef union
{
	uint64_t	int64;
	uint32_t	int32[2];
	uint16_t	int16[4];
	uint8_t		int8[8];
}				t_sample;

int32_t	float_to_fix(double num);
double	fix_to_float(int32_t num);

int32_t	fix_saturate(int64_t num);
int32_t	fix_round(int64_t num);

inline int32_t fix_add(int32_t a, int32_t b);
inline int32_t fix_sub(int32_t a, int32_t b);

int32_t	fix_mul(int32_t a, int32_t b);
int32_t fix_div(int32_t a, int32_t b);

int32_t	fix_mac(int64_t *acc, int32_t a, int32_t b);
int32_t	fix_msub(int64_t *acc, int32_t a, int32_t b);

int32_t	fix_leftshift(int32_t num, int8_t shift);
int32_t	fix_rightshift(int32_t num, int8_t shift);

int32_t fix_sqrt(int32_t num);

#endif	//LIBFIX_H
