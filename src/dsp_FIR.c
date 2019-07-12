#include "libdsp.h"

size_t		dsp_FIRimport(int32_t **buff, const char * path)
{
	size_t		datalen;
	FILE		*file;
	char		str[100];

	if ((file = fopen(path, "r")) == NULL)
	{
		printf("%s: Unable to open file for reading\n", path);
		exit(1);
	}
	printf("\n filename: %s\n", path);
	if (buff == NULL)
		return 0;
	while (fgets(str, 90, file))
	{
		if (str[0] == '%')
			printf("%s", str);
		if (strncmp(str, "% Filter Length", 15) == 0)
			datalen = atoi(str+22);
		if (strncmp(str, "Numerator:", 10) == 0)
			break;
		memset(str, 0, 100);
	}
	*buff = malloc(datalen * sizeof(int32_t));
	double *coefs = malloc(datalen * sizeof(double));
	double norm = 0;
	memset(str, 0, 100);
	for (size_t i = 0; i < datalen; i++)
	{
		fgets(str, 90, file);
		coefs[i] = atof(str);
		norm += coefs[i];
		memset(str, 0, 100);
	}
	for (size_t i = 0; i < datalen; i++)
	{
		coefs[i] /= norm;
		(*buff)[i] = float_to_fix(coefs[i]);
	}
	fclose(file);
	return datalen;
}

int32_t		dsp_FIR(t_ringbuff *input, int32_t *coeffs, size_t filterlen)
{
	uint64_t	result = 0;
#ifdef __XTENSA__
	ae_f16		sample;
#else
	t_sample	sample;
#endif

	for (size_t i = 0; i < filterlen; i++)
	{
#ifdef __XTENSA__
		sample = input->buff[(input->carriage - i + input->length) & (input->length - 1)];
		fix_mac(&result, sample, coeffs[i]);
#else
		sample.int16[3] = input->buff[(input->carriage - i + input->length) & (input->length - 1)];
		fix_mac(&result, sample.int32[1], coeffs[i]);
#endif
	}
	return fix_round(result);
}
