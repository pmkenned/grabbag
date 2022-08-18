#include "wav.h"
#include <stdio.h>
#include <stdlib.h>

const char * program_name;

#define NRATIO 1.12246

int notes[] = {
    261, // C4
    294,
    330,
    349,
    392,
    440,
    494,
    523, // C5
};

const size_t nnotes = NELEMS(notes);

int main(int argc, char * argv[])
{
    program_name = argv[0];
    //if (argc < 2) {
    //    fprintf(stderr, "usage: %s FILE\n", program_name);
    //    exit(1);
    //}

    wav_t wav;
    riff_t oriff;
    if (argc >= 2) {
        buffer_t file_contents = read_file(argv[1]);
        riff_t riff = read_riff(file_contents);
        free(file_contents.p);
        print_riff(&riff);
        wav = wav_create_from_riff(&riff);
        riff_destroy(&riff);
        wav_print(&wav);
        oriff = riff_create_from_wav(&wav);
    } else {
        wav = wav_create_empty_default();
        size_t i;
        for (i = 0; i < nnotes; i++) {
            //int freq = notes[rand()*nnotes/RAND_MAX];
            int freq = notes[i % nnotes];
            wav_append_tone(&wav, freq, 100, 1500);
        }
        oriff = riff_create_from_wav(&wav);
    }

    FILE * fp = fopen("output.wav", "w");
    riff_to_file(&oriff, fp);
    fclose(fp);

    wav_destroy(&wav);
    riff_destroy(&oriff);

    return 0;
}
