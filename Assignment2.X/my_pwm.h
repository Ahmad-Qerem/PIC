#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H
#define _XTAL_FREQ   4000000UL 
#include <xc.h> // include processor files - each processor file is guarded.  

void init_pwm1(void);//10 bit accurcy
void set_pwm1_raw(unsigned int raw_value);//raw value 0 -- 1023
void set_pwm1_percent(float value);// value 0--100%
void set_pwm1_voltage(float value);// value 0--5V, 
void set_pwm1_general(float value, float min, float max);//value  range from min to max

#endif	/* XC_HEADER_TEMPLATE_H */

