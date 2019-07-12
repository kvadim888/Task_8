#include <stdio.h>
#include "libwav.h"
#include "libfix.h"
#include "libdsp.h"

#define CHANNEL_1 1
#define CHANNEL_2 2

int main(int ac, char **av)
{
	size_t		len = 0;

	if (ac != 4)
	{
		printf("Not enought input arguments\n");
		exit(1);
	}

	t_wavfile *input = wav_rdopen(av[1]);
	t_wavfile *output = wav_wropen(av[2], &input->header);

	int32_t *FIR = NULL;
	size_t FIRlen = dsp_FIRimport(&FIR, av[3]);

	wav_bufferinit(input, FIRlen);
	wav_bufferinit(output, FIRlen);

	t_ringbuff *ring_left = dsp_newring(FIRlen * 2, (uint32_t)FIRlen);
	t_ringbuff *ring_right = dsp_newring(FIRlen * 2, (uint32_t)FIRlen);

	len = wav_read(input);
	if (len != input->buflen)
	{
		printf("input file is too small\n");
		exit(1);
	}

	wav_getchannel(ring_left->buff, input, CHANNEL_1, FIRlen);
	wav_getchannel(ring_right->buff, input, CHANNEL_2, FIRlen);

	t_sample sample;

	while ((len = wav_read(input)) > 0)
	{
		wav_getchannel(&ring_left->buff[ring_left->carriage & (ring_left->length - 1)], input, CHANNEL_1, FIRlen);
		wav_getchannel(&ring_right->buff[ring_right->carriage & (ring_right->length - 1)], input, CHANNEL_2, FIRlen);
		for (size_t i = 0; i < output->buflen; i += output->header.channels)
		{
			sample.int32[1] = dsp_FIR(ring_left, FIR, FIRlen);
			output->buffer[i] = sample.int16[3];
			ring_left->carriage = (ring_left->carriage + 1) & (ring_left->length - 1);
			sample.int32[1] = dsp_FIR(ring_right, FIR, FIRlen);
			output->buffer[i + 1] = sample.int16[3];
			ring_right->carriage = (ring_right->carriage + 1) & (ring_right->length - 1);
		}
		len = wav_write(output, len);
		memset(input->buffer, 0, input->buflen);
		memset(output->buffer, 0, output->buflen);
	}
	fclose(input->fs);
	fclose(output->fs);
	// wav_close(&input);
	// wav_close(&output);
	return 0;
}

