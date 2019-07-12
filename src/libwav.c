#include "libwav.h"
#include "libdsp.h"
#include <math.h>

int	wav_headerinit(t_wavheader *wavheader, double duration, uint16_t channels, size_t samplerate)
{
	if (wavheader == NULL)
		return 1;
	memcpy(wavheader->riff, "RIFF", strlen("RIFF"));
	wavheader->overall_size = 36;
	memcpy(wavheader->wave, "WAVE", strlen("WAVE"));
	memcpy(wavheader->fmt_chunk_marker, "fmt ", strlen("fmt "));
	wavheader->length_of_fmt = 16;
	wavheader->format_type = 1;
	wavheader->channels = channels;
	wavheader->sample_rate = (uint32_t)samplerate;
	memcpy(wavheader->data_chunk_header, "data", strlen("data"));
	wavheader->block_align = wavheader->channels * sizeof(int16_t);
	wavheader->bits_per_sample = sizeof(int16_t) * 8;
	wavheader->byterate = wavheader->sample_rate * wavheader->block_align;
	wavheader->data_size = (uint32_t)round(duration * wavheader->byterate);
	wavheader->overall_size += wavheader->data_size;
	return 0;
}

void	wav_info(t_wavfile *wavfile)
{
	if (wavfile == NULL)
		return;
	printf("\nFilename: %s\n", wavfile->path);
	printf("Type: %s\n", (wavfile->RWtype) ? "WRITE_ONLY" : "READ_ONLY");

	t_wavheader *header = &wavfile->header;
	printf("Riff:\t%.4s \n", header->riff);
	printf("Overall_size:\t%u b (%u Kb)\n",
		header->overall_size, header->overall_size / 1024);
	printf("Wave:\t%.4s \n", header->wave);
	printf("Fmt_chunk_marker:\t%.4s \n", header->fmt_chunk_marker);
	printf("Length_of_fmt:\t%u\n", header->length_of_fmt);
	printf("Format_type:\t%u\n", header->format_type);
	printf("Channels:\t%u\n", header->channels);
	printf("Sample_rate:\t%u\n", header->sample_rate);
	printf("Byterate:\t%u\n", header->byterate);
	printf("Block_align:\t%u\n", header->block_align);
	printf("Bits_per_sample:\t%u\n", header->bits_per_sample);
	printf("Data_chunk_header:\t%.4s \n", header->data_chunk_header);
	printf("Data_size:\t%u\n", header->data_size);
	printf("Number of samples:\t%lu \n", header->data_size / header->block_align);
	printf("Duration = %f sec\n", (float)header->data_size / header->byterate);
}

int	wav_bufferinit(t_wavfile *wavfile, size_t length)
{
	if (wavfile == NULL)
		return 1;
	wavfile->buflen = length * wavfile->header.channels;
	wavfile->buffer = calloc(wavfile->buflen, sizeof(int16_t));
	return 0;
}

t_wavfile	*wav_rdopen(const char *path)
{
	t_wavfile *wavfile;

	wavfile = malloc(sizeof(t_wavfile));
	if ((wavfile->fs = fopen(path, "rb")) == NULL)
	{
		printf("%s: Unable to open file for reading\n", path);
		exit(1);
	}
	if (fread(&wavfile->header, sizeof(uint8_t), 44, wavfile->fs) < 44)
	{
		printf("%s: Invalid file\n", path);
		exit(1);
	}
	wavfile->datalen = wavfile->header.data_size / sizeof(int16_t);
	wavfile->path = _strdup(path);
	wavfile->RWtype = READ;
	printf("Reading to file: %s\n", wavfile->path);
	wav_info(wavfile);
	return (wavfile);
}

t_wavfile	*wav_wropen(const char *path, t_wavheader *header)
{
	t_wavfile *wavfile;

	wavfile = malloc(sizeof(t_wavfile));
	if ((wavfile->fs = fopen(path, "wb")) == NULL)
	{
		printf("%s: Unable to open file for writing\n", path);
		exit(1);
	}
	memcpy(&wavfile->header, header, sizeof(t_wavheader));
	if (fwrite(&wavfile->header, sizeof(uint8_t), 44, wavfile->fs) < 44)
	{
		printf("%s: Writing error\n", path);
		exit(1);
	}
	wavfile->datalen = wavfile->header.data_size / sizeof(int16_t);
	wavfile->path = _strdup(path);
	wavfile->RWtype = WRITE;
	printf("Writing to file: %s\n", wavfile->path);
	wav_info(wavfile);
	return (wavfile);
}

size_t	wav_getchannel(int16_t *buff, t_wavfile *wavfile, uint8_t channel, size_t length)
{
	assert(channel > 0 && channel <= wavfile->header.channels);
	size_t len = FIX_MIN(length, wavfile->buflen / wavfile->header.channels);
	for (size_t i = 0; i < len; ++i)
		buff[i] = wavfile->buffer[i + channel - 1];
	return len;
}

size_t	wav_setchannel(t_wavfile *wavfile, int16_t *buff, uint8_t channel, size_t length)
{
	assert(channel > 0 && channel <= wavfile->header.channels);
	size_t len = FIX_MIN(length, wavfile->buflen / wavfile->header.channels);
	for (size_t i = 0; i < len; ++i)
		wavfile->buffer[i + channel - 1] = buff[i];
	return len;
}

size_t	wav_read(t_wavfile *wavfile)
{
	if (wavfile == NULL || wavfile->fs == NULL)
		return 0;
	size_t len = fread(wavfile->buffer, sizeof(int16_t), wavfile->buflen, wavfile->fs);
	return len;
}

size_t	wav_write(t_wavfile *wavfile, size_t datalen)
{
	if (wavfile == NULL || wavfile->fs == NULL)
		return 0;
	size_t len = fwrite(wavfile->buffer, sizeof(int16_t), datalen, wavfile->fs);
	return len;
}

void	wav_close(t_wavfile **wavfile)
{
	if (!wavfile || !*wavfile)
		return;
	fclose((*wavfile)->fs);
	printf("file %s closing\n", (*wavfile)->path);
	if ((*wavfile)->buffer != NULL)
		free((*wavfile)->buffer);
}