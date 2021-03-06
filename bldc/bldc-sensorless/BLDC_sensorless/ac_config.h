/*
 * ac_config.h
 *
 * Created: 22-1-2013 13:07:43
 *  Author: Vito
 */ 


#ifndef AC_CONFIG_H_
#define AC_CONFIG_H_

#define AC_SETUP			(ACA.AC0CTRL	=	AC_INTMODE_BOTHEDGES_gc | AC_INTLVL_MED_gc)

#define AC_MUX_SELECT_2_0 	(ACA.AC0MUXCTRL	=	(ACA.AC0MUXCTRL & ~ (AC_MUXPOS_gm | AC_MUXNEG_gm)) | (AC_MUXPOS_PIN2_gc | AC_MUXNEG_PIN0_gc))
#define AC_MUX_SELECT_3_1	(ACA.AC0MUXCTRL =	(ACA.AC0MUXCTRL & ~ (AC_MUXPOS_gm | AC_MUXNEG_gm)) | (AC_MUXPOS_PIN3_gc | AC_MUXNEG_PIN1_gc))
#define AC_MUX_SELECT_4_7	(ACA.AC0MUXCTRL =	(ACA.AC0MUXCTRL & ~ (AC_MUXPOS_gm | AC_MUXNEG_gm)) | (AC_MUXPOS_PIN4_gc | AC_MUXNEG_PIN7_gc))

#define AC_SET_ENABLE		(ACA.AC0CTRL	=	(ACA.AC0CTRL	& ~ AC_HSMODE_bm )|  AC_ENABLE_bm)
#define AC_SET_DISABLE		(ACA.AC0CTRL   &=	~AC_ENABLE_bm)



#endif /* AC_CONFIG_H_ */