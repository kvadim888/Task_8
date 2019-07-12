#include "libdsp.h"
#include "libfix.h"

int32_t		dsp_db2gain(double gain)
{
	double scale = pow(10, gain / 20);
	return float_to_fix(scale);
}

t_ringbuff	*dsp_newring(size_t length, uint32_t offset)
{
	t_ringbuff *ringbuff = malloc(sizeof(t_ringbuff));
	if (ringbuff == NULL)
		return NULL;
	memset(ringbuff, 0, sizeof(t_ringbuff));
	ringbuff->length = length;
	ringbuff->buff = calloc(ringbuff->length, sizeof(int16_t));
	ringbuff->carriage = offset % ringbuff->length;
	return ringbuff;
}

void	dsp_ringload(t_ringbuff *ring, const int16_t *buff, size_t length, size_t offset)
{
	assert(buff);
	assert(ring->length >= length);
	for (size_t i = 0; i < length; ++i)
		ring->buff[(i + offset) % ring->length] = buff[i];
}

void	dsp_ringpull(int16_t *buff, t_ringbuff *ring, size_t length, size_t offset)
{
	assert(buff);
	assert(ring->length >= length);
	for (size_t i = 0; i < length; ++i)
		buff[i] = ring->buff[(i + offset) % ring->length];
}

int32_t	dsp_ringRMS(t_ringbuff *ring, size_t offset, size_t amount)
{
	int64_t		sum = 0;
	t_sample	sample;

	assert(amount <= ring->length);
	if (amount == 0)
		return 0;
	for (size_t i = 0; i < amount; ++i)
	{
		sample.int16[3] =
				ring->buff[(ring->carriage + offset + i) % ring->length];
		fix_mac(&sum, sample.int32[1], sample.int32[1]);
	}
	return fix_sqrt(fix_div(sum, (int32_t)amount));
}
