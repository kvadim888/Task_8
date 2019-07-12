#include "libfix.h"

int32_t	fix_abs(int32_t num)
{
	if (num == INT32_MIN)
		return INT32_MAX;
	return (num & 0x7fff);
}

int32_t	float_to_fix(double num)
{
	if (num < fix_to_float(INT32_MIN))
		return INT32_MIN;
	if (num >= fix_to_float(INT32_MAX))
		return INT32_MAX;
	return (int32_t)(num * SCALE);
}

double	fix_to_float(int32_t num)
{
	return num / SCALE;
}

int32_t	fix_saturate(int64_t num)
{
	t_sample tmp;
	if (num > (int64_t)INT32_MAX)
		return (INT32_MAX);
	if (num < (int64_t)INT32_MIN)
		return (INT32_MIN);
	tmp.int64 = num;
	return tmp.int32[0];
}

int32_t	fix_round(int64_t num)
{
#ifdef __XTENSA__
	return AE_ROUND32F64SASYM(num);
#else
	t_sample tmp;
	tmp.int64 = num + ((uint64_t)1 << (PRECISION - 1));
	return tmp.int32[1];
#endif
}

inline int32_t fix_add(int32_t a, int32_t b)
{
#ifdef __XTENSA__
	return (int32_t)AE_F32_ADDS_F32((ae_f32)a, (ae_f32)b);
#else
	return fix_saturate((int64_t)a + b);
#endif
}

inline int32_t fix_sub(int32_t a, int32_t b)
{
#ifdef __XTENSA__
	return (int32_t)AE_F32_SUBS_F32((ae_f32)a, (ae_f32)b);
#else
	return fix_saturate((int64_t)a - b);
#endif
}

int32_t fix_mul(int32_t a, int32_t b)
{
#ifdef __XTENSA__
	return (int32_t)AE_F32_MULF_F32((ae_f32)a, (ae_f32)b);
#else
	return fix_round(((int64_t)a * b) << (32 - PRECISION));
#endif
}

int32_t fix_div(int32_t a, int32_t b)
{
	return float_to_fix(fix_to_float(a)/fix_to_float(b));
}

int32_t fix_mac(int64_t *acc, int32_t a, int32_t b)
 {
#ifdef __XTENSA__
	ae_f64 accum = *acc;
	AE_MULAF32S_LL(accum, (ae_f32x2)a, (ae_f32x2)b);
	*acc = accum;
#else
	*acc += ((int64_t)a * b) << (32 - PRECISION);
#endif
 	return fix_round(*acc);
 }
 
int32_t fix_msub(int64_t *acc, int32_t a, int32_t b)
 {
#ifdef __XTENSA__
	ae_f64 accum = *acc;
	AE_MULAF32S_LL(accum, -(ae_f32x2)a, (ae_f32x2)b);
	*acc = accum;
#else
	*acc -= ((int64_t)a * b) << (32 - PRECISION);
#endif
 	return fix_round(*acc);
 }

int32_t	fix_leftshift(int32_t num, int8_t shift)
{
#ifdef __XTENSA__
	return AE_SLAA32S_scalar(num, shift);
#else
	int64_t res = num;
	return fix_saturate(res << shift);
#endif
}

int32_t	fix_rightshift(int32_t num, int8_t shift)
{
#ifdef __XTENSA__
	return AE_SRAA32S_scalar(num, shift);
#else
	t_sample res;
	res.int32[1] = num;
	return fix_round((int64_t)res.int64 >> shift);
#endif
}

int32_t fix_sqrt(int32_t num)
{
	t_sample	sample;
	int32_t		answer, x;
	int64_t		temp;
	 
	assert(num >= 0);
	if (num == 0) 
		return num;
	sample.int32[1] = num;
	answer = 0;
	for(x = (1 << 31); x > 0; x >>= 1)
	{
		answer |= x;
		temp = ((int32_t)answer * answer) << 1;
		if (temp == sample.int32[1])
			break;
		if (temp > sample.int32[1])
			answer ^= x;
	}
	return answer;
}

//int32_t fix_div(int32_t a, int32_t b)
//{
//	t_sample	sample;
//	int32_t		answer, x;
//	int64_t		temp;
//
//	assert(b);
//	// printf("a = %d b = %d\n", a, b);
//	if (a == 0)
//		return 0;
//	answer = 0;
//	for(x = (1 << 31); x > 0; x >>= 1)
//	{
//		answer |= x;
//		temp = fix_mul(answer, b);
//		if (temp == a)
//			break;
//		if (temp > a)
//			answer ^= x;
//	}
//	return answer;
//}
