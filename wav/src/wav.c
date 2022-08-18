#include "common.h"
#include "wav.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

riff_t
read_riff(buffer_t buf)
{
    riff_t riff;
    memcpy(riff.chunk_id, buf.p, 4);
    riff.chunk_id[4] = '\0';

    riff.chunk_sz           = pack_le(buf.p+4, 4);

    memcpy(riff.format, buf.p+8, 4);
    riff.format[4] = '\0';

    memcpy(riff.subchunk1_id, buf.p+12, 4);
    riff.subchunk1_id[4] = '\0';

    riff.subchunk1_sz       = pack_le(buf.p+16, 4);
    riff.audio_format       = pack_le(buf.p+20, 2);
    riff.num_channels       = pack_le(buf.p+22, 2);
    riff.sample_rate        = pack_le(buf.p+24, 4);
    riff.byte_rate          = pack_le(buf.p+28, 4);
    riff.block_align        = pack_le(buf.p+32, 2);
    riff.bits_per_sample    = pack_le(buf.p+34, 2);

    memcpy(riff.subchunk2_id, buf.p+36, 4);
    riff.subchunk2_id[4] = '\0';

    riff.subchunk2_sz       = pack_le(buf.p+40, 4);

    riff.data = malloc(riff.subchunk2_sz);
    memcpy(riff.data, buf.p+44, riff.subchunk2_sz);

    assert(strcmp(riff.chunk_id, "RIFF") == 0);
    assert(riff.chunk_sz == 4 + (8 + riff.subchunk1_sz) + (8 + riff.subchunk2_sz));
    assert(strcmp(riff.format, "WAVE") == 0);
    assert(strcmp(riff.subchunk1_id, "fmt ") == 0);
    assert(riff.audio_format == 1);
    assert(riff.num_channels >= 1 && riff.num_channels <= 2);
    assert(riff.byte_rate == riff.sample_rate * riff.num_channels * riff.bits_per_sample/8);
    assert(riff.block_align == riff.num_channels * riff.bits_per_sample/8);
    assert(riff.bits_per_sample >= 8);
    assert(riff.bits_per_sample <= 32);
    assert(riff.bits_per_sample % 8 == 0);
    assert(strcmp(riff.subchunk2_id, "data") == 0);

    return riff;
}

riff_t
riff_create_from_wav(const wav_t * wav)
{
    riff_t riff;

    strcpy(riff.chunk_id, "RIFF");
    strcpy(riff.format, "WAVE");
    strcpy(riff.subchunk1_id, "fmt ");
    riff.subchunk1_sz = 16;
    riff.audio_format = 1;
    riff.num_channels = wav->nchannels;
    riff.sample_rate = wav->sample_rate;
    riff.byte_rate = wav->sample_rate * wav->nchannels * wav->bits_per_sample/8;
    riff.block_align = wav->nchannels * wav->bits_per_sample/8;
    riff.bits_per_sample = wav->bits_per_sample;
    strcpy(riff.subchunk2_id, "data");
    riff.subchunk2_sz = wav->nsamples * wav->nchannels * wav->bits_per_sample/8;
    riff.chunk_sz = 4 + (8 + riff.subchunk1_sz) + (8 + riff.subchunk2_sz);
    riff.data = malloc(riff.subchunk2_sz);

    u32 byps = wav->bits_per_sample/8;

    u32 i, j;
    for (i = 0; i < wav->nsamples * wav->nchannels; i++) {
        if (wav->nchannels == 1)
            for (j = 0; j < byps; j++)
                riff.data[i*byps+j] = *((u8*)(wav->samples[0]+i)+j);
        else if (wav->nchannels == 2)
            for (j = 0; j < byps; j++)
                riff.data[i*byps+j] = *((u8*)(wav->samples[i%2]+i/2)+j);
    }

    return riff;
}

void
riff_to_file(const riff_t * riff, FILE * fp)
{
    // TODO: ensure endian correctness

    // chunk descriptor
    fprintf(fp, "%s", riff->chunk_id);
    fwrite(&riff->chunk_sz, 4, 1, fp);
    fprintf(fp, "%s", riff->format);
    fprintf(fp, "%s", riff->subchunk1_id);

    // fmt subchunk
    fwrite(&riff->subchunk1_sz, 4, 1, fp);
    fwrite(&riff->audio_format, 2, 1, fp);
    fwrite(&riff->num_channels, 2, 1, fp);
    fwrite(&riff->sample_rate, 4, 1, fp);
    fwrite(&riff->byte_rate, 4, 1, fp);
    fwrite(&riff->block_align, 2, 1, fp);
    fwrite(&riff->bits_per_sample, 2, 1, fp);

    // data subchunk
    fprintf(fp, "%s", riff->subchunk2_id);
    fwrite(&riff->subchunk2_sz, 4, 1, fp);
    fwrite(riff->data, 1, riff->subchunk2_sz, fp);
}

void
print_riff(const riff_t * riff)
{
    printf("chunk_id: %s\n", riff->chunk_id);
    printf("chunk_sz: %u\n", riff->chunk_sz);
    printf("format: %s\n", riff->format);
    printf("\n");
    printf("subchunk1_id: %s\n", riff->subchunk1_id);
    printf("subchunk1_sz: %u\n", riff->subchunk1_sz);
    printf("audio_format: %d\n", riff->audio_format);
    printf("num_channels: %d\n", riff->num_channels);
    printf("sample_rate: %d\n", riff->sample_rate);
    printf("byte_rate: %d\n", riff->byte_rate);
    printf("block_align: %d\n", riff->block_align);
    printf("bits_per_sample: %d\n", riff->bits_per_sample);
    printf("\n");
    printf("subchunk2_id: %s\n", riff->subchunk2_id);
    printf("subchunk2_sz: %u\n", riff->subchunk2_sz);

    buffer_t data_buf;
    data_buf.p = riff->data;
    data_buf.n = riff->subchunk2_sz;
    hex_dump(data_buf, stdout);

    u16 bips = riff->bits_per_sample;
    u16 byps = bips/8;
    u32 nchannels = riff->num_channels;
    u32 nsamples = riff->subchunk2_sz/(byps*nchannels);

    size_t i;
    for (i = 0; i < nsamples*nchannels; i++) {
        s16 sample = pack_le(riff->data + i*byps, byps);
        if (nchannels == 1) {
            printf("%4d\n", sample);
        } else {
            if (i % 64 == 0)
                printf("\n");
            if (i % 2 == 0)
                printf("left: %5d ", sample);
            else
                printf("right: %5d\n", sample);
        }
    }
}

void
riff_destroy(const riff_t * riff) {
    free(riff->data);
}

wav_t
wav_create_from_riff(const riff_t * riff)
{
    wav_t wav;
    u16 byps = riff->bits_per_sample/8;
    size_t nchannels = riff->num_channels;
    size_t nsamples  = riff->subchunk2_sz/(byps*nchannels);
    wav.nchannels = nchannels;
    wav.sample_rate = riff->sample_rate;
    wav.bits_per_sample = riff->bits_per_sample;
    wav.nsamples = nsamples;
    wav.sample_cap = nsamples;
    wav.samples[0] = malloc(sizeof(*wav.samples[0])*nsamples);
    wav.samples[1] = (nchannels == 2) ? malloc(sizeof(*wav.samples[1])*nsamples) : NULL;

    size_t i;
    for (i = 0; i < nsamples*nchannels; i++) {
        if (nchannels == 1)
            wav.samples[0][i] = pack_le(riff->data + i*byps, byps);
        else if (nchannels == 2)
            wav.samples[i%2][i/2] = pack_le(riff->data + i*byps, byps);
    }

    return wav;
}

wav_t
wav_create_empty(int sample_rate, int bits_per_sample, int nchannels)
{
    wav_t wav;

    wav.nchannels = nchannels;
    wav.sample_rate = sample_rate;
    wav.bits_per_sample = bits_per_sample;
    wav.nsamples = 0;
    wav.sample_cap = 0;
    wav.samples[0] = NULL;
    wav.samples[1] = NULL;

    return wav;
}

wav_t
wav_create_empty_default() {
    return wav_create_empty(44100, 16, 2);
}

// TODO: amplitude should be 0 to 1 (or 0 to 100)
void
wav_append_tone(wav_t * wav, int hz, int dur_ms, int ampl)
{
    size_t prev_nsamples = wav->nsamples;
    size_t nsamples = dur_ms*wav->sample_rate/1000;

    wav->nsamples += nsamples;
    if (wav->nsamples >= wav->sample_cap) {
        wav->sample_cap = wav->nsamples;
        wav->samples[0] = realloc(wav->samples[0], sizeof(*wav->samples[0])*wav->sample_cap);
        wav->samples[1] = (wav->nchannels == 2) ? realloc(wav->samples[1], sizeof(*wav->samples[1])*wav->sample_cap) : NULL;
    }

    size_t i;
    for (i = 0; i < nsamples*wav->nchannels; i++) {
        // TODO: make this configurable
        int _ampl = ampl;
        if (i > nsamples*wav->nchannels - 1000) {
            _ampl *= (nsamples*wav->nchannels - i)/1000.0;
        } else if (i < 1000) {
            _ampl *= i/1000.0;
        }
        if (wav->nchannels == 1)
            wav->samples[0][prev_nsamples+i] = (int)(sin(i*hz*2*M_PI/wav->sample_rate)*_ampl);
        else if (wav->nchannels == 2)
            wav->samples[i%2][prev_nsamples+i/2] = (int)(sin(i*hz*2*M_PI/wav->sample_rate)*_ampl);
    }
}

void
wav_print(const wav_t * wav)
{
    size_t i;
    for (i = 0; i < wav->nsamples; i++) {
        if (wav->nchannels == 1) {
            s16 sample = wav->samples[0][i];
            printf("sample: %x\n", sample);
        } else {
            s16 lsample = wav->samples[0][i];
            s16 rsample = wav->samples[1][i];
            if (i % 8 == 0)
                printf("\n");
            printf("(%5d, %5d) ", lsample, rsample);
        }
    }
    printf("\n");
}

void
wav_destroy(const wav_t * wav)
{
    free(wav->samples[0]);
    free(wav->samples[1]);
}
