#include "my_adc.h"
void init_adc_no_lib(void) {
    ADCON1 = 0b00001100; //3 analog channels, change this according to your application
    ADCON0 = 0;
    ADCON0bits.ADON = 1; // turn adc on 
    //ADCON2 = 0b00001001; //left justified , 8 bits, , 2 Tad, Fosc/8
    ADCON2 = 0b10001001; // ADFM= 1 right justified 10 bits, 2 Tad, Fosc/8
}

int read_adc_raw_no_lib(unsigned char channel) {
    int raw_value; //0?1023, or short raw_value, need 16 bit value
    ADCON0bits.CHS = channel ;
    ADCON0bits.GO = 1;          // start conversion
    while (ADCON0bits.GO) {}; // wait until conversion is done, you can put CLRWDT() in the loop, but not necessary
    raw_value = ADRESH << 8 | ADRESL;  // 10 bit, need to shift the bits right, this puts the result in one 16 bit variable
    return raw_value; // 16 bit , it is actually 10 bit 0 ---1023
}

float read_adc_temp(unsigned char channel) {
    int raw_value;
    float voltage;
    raw_value = read_adc_raw_no_lib(channel);
    voltage = (raw_value * 5) / 1024.0;
    float temp ;
    temp= voltage*40;
    //temp = (raw_value * ( 200 - (0)) /1023.0  + (0)); // parentheses used to illustrate max  and min range
    return temp;
}

float read_adc_temp2(unsigned char channel) {
    int raw_value;
    float voltage;
    raw_value = read_adc_raw_no_lib(channel);
    voltage = (raw_value * 5) / 1024.0;
    float temp ;    
    temp= voltage*300;
    return temp;
}