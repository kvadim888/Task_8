#include "libfix.h"
#include <conio.h>
#include <stdio.h>

#define STOP 1
#define CONTINUE 0

void add_test(double a, double b, int mode);
void sub_test(double a, double b, int mode);
void mul_test(double a, double b, int mode);

void mac_test(double *a, double *b, size_t len, int mode);
void msub_test(double *a, double *b, size_t len, int mode);


int main()
{
	add_test(0.5, 0.5, STOP);
	sub_test(0.5, 0.5, STOP);
	mul_test(0.5, 0.5, STOP);
	return 0;
}

void add_test(double a, double b, int mode)
{
	int32_t fix_a = float_to_fix(a);
	int32_t fix_b = float_to_fix(a);
	int32_t sum = fix_add(fix_a, fix_b);
	if (fabs(fix_to_float(sum) - (a + b)) > fix_to_float(1))
	{
		printf("\nFAIL: ");
		printf("%.9f + %.9f = %.9f  ", fix_to_float(fix_a), fix_to_float(fix_b), fix_to_float(sum));
		printf("(right answer: %.9f)\n", a + b);
		if (mode) exit(1);
	}
}

void sub_test(double a, double b, int mode)
{
	int32_t fix_a = float_to_fix(a);
	int32_t fix_b = float_to_fix(a);
	int32_t sub = fix_sub(fix_a, fix_b);
	if (fabs(fix_to_float(fix_sub) - (a - b)) > fix_to_float(1))
	{
		printf("\nFAIL: ");
		printf("%.9f - %.9f = %.9f  ", fix_to_float(fix_a), fix_to_float(fix_b), fix_to_float(sub));
		printf("(right answer: %.9f)\n", a - b);
		if (mode) exit(1);
	}
}

void mul_test(double a, double b, int mode)
{
	int32_t fix_a = float_to_fix(a);
	int32_t fix_b = float_to_fix(a);
	int32_t mul = fix_mul(fix_a, fix_b);
	if (fabs(fix_to_float(mul) - (a * b)) > fix_to_float(1))
	{
		printf("\nFAIL: ");
		printf("%.9f x %.9f = %.9f  ", fix_to_float(fix_a), fix_to_float(fix_b), fix_to_float(mul));
		printf("(right answer: %.9f)\n", a * b);
		if (mode) exit(1);
	}
}

void msub_test(double *a, double *b, size_t len, int mode)
{
	// int32_t fix_a = float_to_fix(a);
	// int32_t fix_b = float_to_fix(a);
	// int32_t mul = fix_mul(fix_a, fix_b);
	// if (fabs(fix_to_float(mul) - (a * b)) > fix_to_float(1))
	// {
	// 	printf("\nFAIL: ");
	// 	printf("%.9f x %.9f = %.9f  ", fix_to_float(fix_a), fix_to_float(fix_b), fix_to_float(mul));
	// 	printf("(right answer: %.9f)\n", a * b);
	// 	if (mode) exit(1);
	// }
}

void mac_test(double *a, double *b, size_t len, int mode)
{
	// int32_t fix_a = float_to_fix(a);
	// int32_t fix_b = float_to_fix(a);
	// int32_t mul = fix_mul(fix_a, fix_b);
	// if (fabs(fix_to_float(mul) - (a * b)) > fix_to_float(1))
	// {
	// 	printf("\nFAIL: ");
	// 	printf("%.9f x %.9f = %.9f  ", fix_to_float(fix_a), fix_to_float(fix_b), fix_to_float(mul));
	// 	printf("(right answer: %.9f)\n", a * b);
	// 	if (mode) exit(1);
	// }
}