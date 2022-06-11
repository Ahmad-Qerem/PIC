
#include "my_pwm.h"

// See chapter 15 page 141 (PWM mode = 11xx)
void init_pwm1(void)//10 bit accurcy
{
    PR2 = 255; //10 bit accurcy
    T2CON = 0;
    T2CONbits.T2CKPS=0b11;
    CCP1CON = 0x0C; // Select PWM mode 
    //CCP1CONbits.CCP1M = 0x0C // can also be set like that
    T2CONbits.TMR2ON = 1;  // turn the timer on 
    TRISCbits.RC2 =0;  //CCP1 is output
}

void set_pwm1_raw(unsigned int raw_value)//raw value 0 -- 1023 corresponds to  0--100%
{
    CCPR1L = (raw_value >> 2) & 0x00FF; // Load the upper 8 bit in CCPL1
    CCP1CONbits.DC1B = raw_value & 0x0003; //first two bits in bits 4, 5 of CCP1COn
    
   // Another way for Writing to 2 LSBs of pwm duty cycle in CCPCON register
    //CCP1CON = ((unsigned char)(CCPICON1 & 0xCF) | ((raw_value & 0x0003)<<4));
}
void set_pwm1_percent(float value)// value 0--100%, corresponds to 0--1023
{
    float tmp = value*1023.0/100.0;//scale 0--100 to 0--1023
    int raw_val = (int)(tmp +0.5); // for rounding
    if ( raw_val> 1023) raw_val = 1023;// Do not exceed max value
    set_pwm1_raw(raw_val);
}
void set_pwm1_voltage(float value)// value 0--5V,  corresponds to 0--1023
{
    float tmp = value*1023.0/5.0; // scale 0--5 to 0--1023
    int raw_val = (int)(tmp +0.5); // for rounding
    if ( raw_val> 1023) raw_val = 1023; // Do not exceed max value
    set_pwm1_raw(raw_val);
    
}
//Technically this function replaces the previous 2
void set_pwm1_general(float value, float min, float max)//value  range from min to max
{
    float tmp = (value - min)*1023.0/(max - min); //corresponds to 0--1023, 0--10%
    int raw_val = (int)(tmp +0.5);// for rounding
    if ( raw_val> 1023) raw_val = 1023;// Do not exceed max value
    set_pwm1_raw(raw_val);
    
}
