/*
 * Driver for setting the power for the BLDC driver.
 * By: Vito
 */
#ifndef __SET_POWER_H__
#define __SET_POWER_H__

static inline uint16_t spow_calc_table_idx(uint16_t last_index)
{
    uint16_t new_index;

    new_index = last_index + (BLDC_DAC_SAMPLES_1_4 - last_index) * 2;

    return new_index;
}

void spow_set_current(uint16_t current);
void spow_set_period(uint16_t period);

#endif /*__SET_POWER_H__*/
