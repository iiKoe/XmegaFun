/*
 * port_config.h
 *
 * Created: 27-11-2012 10:18:32
 *  Author: Vito
 */ 


#ifndef PORT_CONFIG_H_
#define PORT_CONFIG_H_


/* 
*  De poorten voor Transistors
*/

#define TRANSISTOR_CONFIG()	(PORTC.DIRSET |= PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm )		//PORT1 t/m PORT5

#define CL_ALL	0x00
#define U_VM	PIN0_bm
#define U_GND	PIN1_bm
#define V_VM	PIN2_bm
#define V_GND	PIN3_bm
#define W_VM	PIN4_bm
#define W_GND	PIN5_bm


#define Set_none()	(AWEXC.DTLSBUF = CL_ALL		 )
#define Set_Q1Q4()	(AWEXC.DTLSBUF = U_GND | V_VM)
#define Set_Q1Q6()	(AWEXC.DTLSBUF = U_GND | W_VM)
#define Set_Q3Q2()	(AWEXC.DTLSBUF = V_GND | U_VM)
#define Set_Q3Q6()	(AWEXC.DTLSBUF = V_GND | W_VM)
#define Set_Q5Q2()	(AWEXC.DTLSBUF = W_GND | U_VM)
#define Set_Q5Q4()	(AWEXC.DTLSBUF = W_GND | V_VM)


#define BLDC_SPEED_ADJUST()	  (PORTF.IN & 0x06)


#endif /* PORT_CONFIG_H_ */