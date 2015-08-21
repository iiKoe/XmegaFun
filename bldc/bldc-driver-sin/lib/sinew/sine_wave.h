/*
 * Library for creating sine table of arbitrary length.
 * By: Vito
 */
#ifndef __SINE_WAVE_H__
#define __SINE_WAVE_H__

int sinew_generate(uint16_t **sine_table, uint16_t samples, uint16_t amp, uint16_t off);

#endif /*__SINE_WAVE_H__*/
