/*
 * Library for creating sine table of abetrary length.
 * By: Vito
 */
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#include "sine_wave.h"

/*
 * Generate a cosine of specified samples, amplitude and offset.
 */
int sinew_generate(uint16_t **sine_table, uint16_t samples, uint16_t amp, uint16_t off)
{
    uint16_t *data;
    uint16_t i;
    double freq;

    data = (uint16_t *)malloc(sizeof(uint16_t) * samples);
    if (!data)
        return 0;

    --samples;
    freq = 1.0/(double)samples;
    for(i=0; i<samples; i++) {
        data[i] = off + (uint16_t)((double)amp * cos(2 * M_PI * freq * (double)i));
    }

    *sine_table = data;

    return 1;
}

