/*
 * bldc_drv.c
 *
 * Created: 27-11-2012 11:33:24
 *  Author: Vito
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "bldc_dvr.h"
#include "port_config.h"
#include "ac_config.h"
#include "usart_driver.h"


uint8_t p_ticker				= FALSE;
uint8_t bldc_run_stop			= FALSE;

volatile uint8_t bldc_direction = CW;
volatile uint8_t motor_state;

uint8_t new_position			= 0b001;
volatile uint8_t motor_step;

uint8_t ru_period_counter		= 0;
uint8_t	ru_step_length			= RAMP_UP_STEP_MAX;
uint8_t ru_step_length_cntr		= 0;
uint8_t ru_index				= 0;
uint8_t ru_step_before_running	= 0;

uint16_t p_regulation_per		= 0;
uint8_t p_regulation_scale_per  = 0;

volatile uint32_t top_average	= 200;

uint16_t duty_cycle				= 100;
uint16_t bldc_rpm				= 500;

typedef enum {MS_ALIGN=1,MS_RAMP_UP=2,MS_LAST_RAMP_UP=3,MS_RUNNING=4,MS_STOP=5} Motor_State;

uint8_t  ramp_up_time_table[] = 	{50,34,28,24,20,18,16,12,10,8,6};
uint16_t ramp_up_duty_table[] =		{430,437,445,455,462,470,476,483,494,509,519};					//{ 47.8, 48.6, 49.4, 50.6, 51.4, 52.2, 52.9, 53.7, 54.9, 56.5, 57.6 }%


/***************************************************************
***************** Regulatie timer*******************************
***************************************************************/
void setup_ticker(void)
{
	TCD0.PER		= TICKER_TOP;											//32Mhz is 250us
	TCD0.CTRLA		= TC_CLKSEL_DIV1_gc;						
	TCD0.INTCTRLA	= TC_OVFINTLVL_MED_gc;
}

ISR(TCD0_OVF_vect)
{
	p_ticker = TRUE;
}

void bldc_setup_rmp_timer(void)
{
	TCD1.PER		= 0xFFFF;
	TCD1.CTRLA		= TC_CLKSEL_DIV256_gc;
	TCD1.INTCTRLA	= TC_OVFINTLVL_MED_gc;
}

ISR(TCD1_OVF_vect)
{
	// RPM TO SLOW
}

void bldc_switch_communication(uint8_t position)
{
	if (bldc_run_stop == TRUE)
	{
		switch(position)
		{
			// cases according to rotor position
		case 0b001:  {Set_Q1Q6();motor_step=0b001;}
					 break;

		case 0b101:  {Set_Q1Q4();motor_step=0b101;}
					 break;

		case 0b100:  {Set_Q5Q4();motor_step=0b100;}
					 break;

		case 0b110:  {Set_Q5Q2();motor_step=0b110;}
					 break;

		case 0b010:  {Set_Q3Q2();motor_step=0b010;}
					 break;

		case 0b011:  {Set_Q3Q6();motor_step=0b011;}
					 break;
		default:	
					 Set_none();
					 break;																							
		}
	}
	
	else
	{
		Set_none();
	}		
}


void bldc_forward(void)
{
	bldc_direction = CW;
}

void bldc_backward(void)
{
	bldc_direction = CCW;
}

void bldc_stop(void)
{
	bldc_run_stop = FALSE;
	motor_state	  = MS_STOP;
	BLDC_SET_DUTY(0);
	TCE0.CTRLA = TC_CLKSEL_OFF_gc;
	TCE1.CTRLA = TC_CLKSEL_OFF_gc;
	AC_SET_DISABLE;
}


void setup_speed_interrupt(void)
{
	PORTF.INTCTRL  = PORT_INT0LVL_MED_gc;
	PORTF.INT0MASK = PIN1_bm | PIN2_bm;
	PORTF.PIN1CTRL = PORT_ISC_FALLING_gc;
	PORTF.PIN2CTRL = PORT_ISC_FALLING_gc;
}

void bldc_run(void)
{
	bldc_run_stop = TRUE;
	new_position  = 0b001;
	bldc_switch_communication(new_position);
	BLDC_SET_DUTY(ALIGN_DUTY);
	ru_period_counter = ALIGN_PER;
	motor_state = MS_ALIGN;
	ru_step_length = RAMP_UP_STEP_MAX;
	ru_index	= 0;	
}


void bldc_main_routine(void)
{
	if(p_ticker == TRUE)
	{
		p_ticker = FALSE;
		p_regulation_scale_per ++;
		
		if (p_regulation_scale_per >= 4) // 1ms
		{
			p_regulation_scale_per = 0;
			
			switch(motor_state)
			{
				case (MS_ALIGN):
					ru_period_counter--;
					if (ru_period_counter==0){
						motor_state = MS_RAMP_UP;
					}							
				break;	
			
			
				case (MS_RAMP_UP):
				
					ru_period_counter ++;
					if (ru_period_counter >= RAMP_UP_PER)
					{
						ru_period_counter = 0;
						ru_step_length = ramp_up_time_table[ru_index];
						BLDC_SET_DUTY(ramp_up_duty_table[ru_index]);
					
						if (ru_index < RAMP_UP_INDEX_MAX)
						{
							ru_index ++;
						}
					
						else
						{
							motor_state = MS_LAST_RAMP_UP;
						}
					}
				
					ru_step_length_cntr ++;
					if (ru_step_length_cntr >= ru_step_length)
					{
						ru_step_length_cntr = 0;
						bldc_compute_new_position();
						bldc_switch_communication(new_position); //nog checken voor direction
					}
					
				break;	
				
				
				case (MS_LAST_RAMP_UP):
				
					ru_step_length_cntr ++;
					if( ru_step_length_cntr >= ru_step_length )
					{
						ru_step_length_cntr = 0;
						bldc_compute_new_position();
						bldc_switch_communication(new_position);
						ru_step_before_running ++;
						if (ru_step_before_running >= 255)
						{
							ru_step_before_running = 0;
							bldc_start_runningphase();// START RUNNING PHASE
							bldc_setup_rmp_timer();
							motor_state = MS_RUNNING;
						}
					}
				break;
				
				case (MS_RUNNING):
				
					p_regulation_per ++;
					if (p_regulation_per >= 100)			//40 ms
					{
						p_regulation_per = 0;
						BLDC_SET_DUTY(600);				// Duty cycle aka speed
						USART_Puts("\n Duty: ");
						USART_Puti(BLDC_GET_DUTY());
						USART_Puts("	RPM: ");
						USART_Puti(bldc_rpm);
					}
					
				break;
				
				
				case (MS_STOP):
		
					bldc_stop();
				
				break;
				
				default:
					motor_state = MS_STOP;
				break;	
			}									
		}					
	}		
}


void bldc_compute_new_position(void)
{
	if (bldc_direction==CCW)
	{
		switch(new_position)
		{
			/* ramp up CCW */
			case 0b001:
				new_position = 0b011;
			break;

			case 0b101:
				new_position = 0b001;
			break;

			case 0b100:
				new_position = 0b101;
			break;

			case 0b110:
				new_position = 0b100;
			break;

			case 0b010:
				new_position = 0b110;
			break;

			case 0b011:
				new_position = 0b010;
			break;

			default :
				new_position = 0b001;
			break;
		}

	}
	else
	{
		switch(new_position)
		{
			/* ramp_up CW */
			case 0b001:
				new_position = 0b101;
			break;

			case 0b101:
				new_position = 0b100;
			break;

			case 0b100:
				new_position = 0b110;
			break;

			case 0b110:
				new_position = 0b010;
			break;

			case 0b010:
				new_position = 0b011;
			break;

			case 0b011:
				new_position = 0b001;
			break;

			default :
				new_position = 0b001;
			break;
		}
	}
}


void bldc_start_runningphase(void)
{
	uint16_t top;
	top_average = TIMER_STARTUP;		// Timer at startup end
	top = (top_average >> 1);			// top = top/2
	
	// Stop Timers
	TCE0.CTRLA = TC_CLKSEL_OFF_gc;
	TCE1.CTRLA = TC_CLKSEL_OFF_gc;
	  
	// 30°
	TCE1.CNT = 0;
	TCE1.PER = 0xFFFF;
	TCE1.INTCTRLA = TC_OVFINTLVL_MED_gc;
	  
	// Demagnetisation
	TCE0.CNT = 0;
	TCE0.PER = (top_average >> 2);
	TCE0.INTCTRLA = TC_OVFINTLVL_MED_gc;
	  
	// Start Timers
	TCE0.CTRLA = TC_CLKSEL_DIV8_gc;
	TCE1.CTRLA = TC_CLKSEL_DIV8_gc;
	  
	//mc_init_timer0();
	
}

void bldc_calc_rpm(void)
{
	uint16_t rpm = TCD1.CCA;
	bldc_rpm = ((uint32_t)bldc_rpm*15 + rpm ) >> 4;
	TCD1.CCA = 0;
}

ISR(TCE1_OVF_vect)
{
	if (bldc_run_stop == TRUE)
	{  
       // Stop Timers
        TCE1.CTRLA = TC_CLKSEL_OFF_gc;
        TCE1.CNT = 0;        
        TCE1.PER = 0xFFFF;
        // Start Timers
        TCE1.CTRLA = TC_CLKSEL_DIV8_gc;
                
        switch(motor_step)
		{
          case (0b100):
            if (bldc_direction==CCW)	{Set_Q1Q4();motor_step=0b101;}
            else						{Set_Q5Q2();motor_step=0b110;}
          break;
      
          case (0b110):
            if (bldc_direction==CCW)	{Set_Q5Q4();motor_step=0b100;}
            else						{Set_Q3Q2();motor_step=0b010;}
          break;
      
          case (0b010):
            if (bldc_direction==CCW)	{Set_Q5Q2();motor_step=0b110;}
            else						{Set_Q3Q6();motor_step=0b011;}
          break;
      
          case (0b011):
            if (bldc_direction==CCW)	{Set_Q3Q2();motor_step=0b010;}
            else						{Set_Q1Q6();motor_step=0b001;}
          break;
      
          case (0b001):
            if (bldc_direction==CCW)	{Set_Q3Q6();motor_step=0b011;}
            else						{Set_Q1Q4();motor_step=0b101;}
          break;
      
          case (0b101):
            if (bldc_direction==CCW)	{Set_Q1Q6();motor_step=0b001;}
            else						{Set_Q5Q4();motor_step=0b100;}
          break;
      
          default :
            Set_none(); // all switches are switched OFF
            motor_step=0b010;
          break;
      
		} 
	}	
}


ISR(TCE0_OVF_vect)
{
	
	TCE0.CTRLA = TC_CLKSEL_OFF_gc;
	  
	switch(motor_step)
	{
		case (0b100):
			//g_mc_read_enable = TRUE;
			bldc_calc_rpm();
			AC_MUX_SELECT_2_0;
			AC_SET_ENABLE;
		break;

		case (0b110):
			AC_MUX_SELECT_3_1;
			AC_SET_ENABLE;
		break;

		case (0b010):
			AC_MUX_SELECT_4_7;
			AC_SET_ENABLE;
		  
		break;

		case (0b011):
			AC_MUX_SELECT_2_0;
			AC_SET_ENABLE;
		break;

		case (0b001):
			AC_MUX_SELECT_3_1;
			AC_SET_ENABLE;
		break;

		case (0b101):
			AC_MUX_SELECT_4_7;
			AC_SET_ENABLE;
		break;

		default :
			motor_step=0b010;
		break;
	}
}


ISR(ACA_AC0_vect)
{
	uint16_t top;
	uint16_t tmp1;
	uint16_t tmp2;
	uint8_t  temp;
	  
	// Stop Timers
	TCE0.CTRLA = TC_CLKSEL_OFF_gc;
	TCE1.CTRLA = TC_CLKSEL_OFF_gc;
	  
	top = TCE1.CNT;
	  
	top_average = ((uint32_t)top_average*15 + top ) >> 4;
	tmp1 = (top_average >> 1);
	tmp2 = tmp1 + (top_average >> 2);

	TCE0.CNT = 0;
	TCE1.CNT = 0;
	  
	TCE1.PER = tmp1;
	TCE0.PER = tmp2;
	  
	// Start Timers
	TCE0.CTRLA = TC_CLKSEL_DIV8_gc;
	TCE1.CTRLA = TC_CLKSEL_DIV8_gc;
	  
	AC_SET_DISABLE;
}