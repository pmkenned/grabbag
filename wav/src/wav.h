#ifndef WAV_H
#define WAV_H

#include "common.h"

#define MAX_CHANNELS 2

typedef struct {
    char chunk_id[5];
    u32 chunk_sz;
    char format[5];
    char subchunk1_id[5];
    u32 subchunk1_sz;
    u16 audio_format;
    u16 num_channels;
    u32 sample_rate;
    u32 byte_rate;
    u16 block_align;
    u16 bits_per_sample;
    char subchunk2_id[5];
    u32 subchunk2_sz;
    u8 * data;
} riff_t;

typedef struct {
    u32 * samples[MAX_CHANNELS];
    u32 sample_rate;
    size_t nsamples;
    size_t sample_cap;
    size_t nchannels;
    u16 bits_per_sample;
} wav_t;

riff_t read_riff(buffer_t buf);
void print_riff(const riff_t * riff);
void riff_destroy(const riff_t * riff);
riff_t riff_create_from_wav(const wav_t * wav);
void riff_to_file(const riff_t * riff, FILE * fp);

wav_t wav_create_empty(int sample_rate, int bits_per_sample, int nchannels);
wav_t wav_create_empty_default();
wav_t wav_create_from_riff(const riff_t * riff);
void wav_append_tone(wav_t * wav, int hz, int dur_ms, int ampl);
void wav_print(const wav_t * wav);
void wav_destroy(const wav_t * wav);

#endif /* WAV_H */
