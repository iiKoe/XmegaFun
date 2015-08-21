/*
 * Driver for setting the power for the BLDC driver.
 * By: Vito
 */
#include <stdio.h>
#include <inttype.h>

#include "set_power.h"

void spow_set_current(uint16_t current)
{
    uint16_t dac_amp;

    dac_amp = 2048 + DAC_AMP * 15; /* 2048 + DAC_AMP * 750 * 2 */
    //Ajust amplitude of dac to set current
    sinew_set_amp(dac_amp);
}

void spow_set_period(uint16_t period)
{
    //Set period for dac update
}
