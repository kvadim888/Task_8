#include <stdio.h>
#include <stdlib.h>

#include "libwav.h"
#include "libfix.h"
#include "libdsp.h"

int main(int ac, char **av)
{
	size_t len = 0;
	if (ac != 4)
	{
		printf("Not enought input arguments\n");
		exit(1);
	}

	t_wavfile *input = wav_rdopen(av[1]);
	wav_bufferinit(input, 200);
	t_wavfile *output = wav_wropen(av[2], &input->header);
	wav_bufferinit(output, 200);

	int32_t gain = dsp_db2gain(atof(av[3]));
	printf("\n gain = %.8f\n Control: 20log(%.8f)=%.8f\n",
			fix_to_float(gain), fix_to_float(gain), atof(av[3]));

	while ((len = wav_read(input)) > 0)
	{
		memset(output->buffer, 0, output->buflen);
		for (size_t i = 0; i < len; i++)
			output->buffer[i] = (int16_t)fix_mul(input->buffer[i], gain);
		len = wav_write(output, len);
		memset(input->buffer, 0, output->buflen);
	}
	fclose(input->fs);
	fclose(output->fs);

	// wav_close(&input);
	// wav_close(&output);
	return 0;
}
