/*
 * bldc_dvr.h
 *
 * Created: 27-11-2012 11:33:45
 *  Author: Vito
 */ 


#ifndef BLDC_DVR_H_
#define BLDC_DVR_H_


#define TICKER_TOP 8000

#define CW	1
#define CCW 0

#define TRUE  1
#define FALSE 0

#define ALIGN_DUTY			700								// 78.4
#define ALIGN_PER			200
#define RAMP_UP_STEP_MAX	 40
#define RAMP_UP_PER			 50
#define RAMP_UP_INDEX_MAX	 10

#define TIMER_STARTUP	  47000

#define BLDC_FORWARD()			(bldc_direction = CW )
#define BLDC_BACKWARD()			(bldc_direction = CCW)

#define BLDC_SET_DUTY(_duty)	(TCC0.CCABUF = _duty)
#define BLDC_GET_DUTY()			(TCC0.CCABUF)

void setup_ticker(void);
void bldc_setup_rmp_timer(void);
void bldc_switch_communication(uint8_t position);			//Motor state 
void bldc_forward(void);									//Motor direction forward
void bldc_backward(void);									
void bldc_stop(void);					
void setup_speed_interrupt(void);							
void bldc_run(void);
void bldc_main_cycle(void);
void bldc_main_routine(void);
void bldc_compute_new_position(void);
void bldc_start_runningphase(void);
void bldc_calc_rpm(void);


#endif /* BLDC_DVR_H_ */