// CONFIG1H
#pragma config OSC = XT         // Oscillator Selection bits (XT oscillator)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = ON         // Watchdog Timer Enable bit (WDT enabled)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "my_adc.h"
#include "lcd_x8.h"
#define STARTVALUE  3036
void setupPorts(void);
void setupInterrupts(void);
void timer0_isr(void);
void reloadTimer0(void);
void EXT_Int2_isr(void);
void EXT_Int1_isr(void);
void EXT_Int0_isr(void);
int clock_setup_SMH = 0; //circulate sec,10sec,min,10min,hour (0,1,2,3,4) respectively
int sec = 0, min = 0, hour = 0;
unsigned int Totsec=0;//max 36000 = 10 hour
int CK_state = 0, HT_state = 0; //cocker and heaterstate (0=off, 1 =on)
char Buffer1[16], Buffer2[16], Buffer3[16], Buffer4[16];//ot write at lcd
float CT=0;
float SP=0;
char StateOFF[4]="OFF";
char StateON[4]=" ON";
char m0[14]="SEC          ";
char m1[14]="SEC          ";
char m2[14]="10SEC        ";
char m3[14]="MIN          ";
char m4[14]="10MIN        ";
char m5[14]="HR           ";
void main(void) {
    setupPorts();
    setupInterrupts();
    lcd_init();
    init_adc_no_lib();
    sprintf(Buffer1, "Time : %02d:%02d:%02d ", hour, min, sec);
    lcd_gotoxy(1, 1);
    lcd_puts(Buffer1);
    sprintf(Buffer2, "CT:%5.1fC CK:OFF",CT);
    lcd_gotoxy(1, 2);
    lcd_puts(Buffer2);
    sprintf(Buffer3, "SP:%5.1fC HT:OFF",SP);
    lcd_gotoxy(1, 3);
    lcd_puts(Buffer3);
    sprintf(Buffer4, "MD:SEC          ");
    lcd_gotoxy(1, 4);
    lcd_puts(Buffer4);
    clock_setup_SMH = 0, sec = 0, min = 0, hour = 0, CK_state = 0, HT_state = 0;
    PORTCbits.RC5 = 0;
    while (1) {
        CLRWDT();
        delay_ms(200);
        if (PORTBbits.RB3 == 0) {
            delay_ms(200);
            if (clock_setup_SMH == 0) {
                Totsec = Totsec + 1;
            } else if (clock_setup_SMH == 1) {
                Totsec = Totsec + 10;
            } else if (clock_setup_SMH == 2) {
                Totsec = Totsec + 60;
            }else if (clock_setup_SMH == 3) {
                Totsec = Totsec + 600;
            }else if (clock_setup_SMH == 4) {
                Totsec = Totsec + 3600;
            }
            if (Totsec<0 ||Totsec>36000)Totsec=36000;
            hour = (Totsec/3600); 
            min = (Totsec -(3600*hour))/60;
            sec = (Totsec -(3600*hour)-(min*60));
            sprintf(Buffer1, "Time : %02d:%02d:%02d ", hour, min, sec);
            lcd_gotoxy(1, 1);
            lcd_puts(Buffer1);
        } else if (PORTBbits.RB4 == 0) {
            delay_ms(200);
            if (clock_setup_SMH == 0) {
                Totsec = Totsec - 1;
            } else if (clock_setup_SMH == 1) {
                Totsec = Totsec - 10;
            } else if (clock_setup_SMH == 2) {
                Totsec = Totsec - 60;
            }else if (clock_setup_SMH == 3) {
                Totsec = Totsec - 600;
            }else if (clock_setup_SMH == 4) {
                Totsec = Totsec - 3600;
            }
            if (Totsec<0 ||Totsec>36000 )Totsec=0;
            hour = (Totsec/3600); 
            min = (Totsec -(3600*hour))/60;
            sec = (Totsec -(3600*hour)-(min*60));
            sprintf(Buffer1, "Time : %02d:%02d:%02d ", hour, min, sec);
            lcd_gotoxy(1, 1);
            lcd_puts(Buffer1);
        } else if (PORTBbits.RB5 == 0) {
            delay_ms(200);
            clock_setup_SMH = 0;
            sec = 0, min = 0, hour = 0;
            Totsec=0;
            CK_state = 0, HT_state = 0;
            CT=0;
            SP=0;
            strcpy(m0,m1); 
            PORTCbits.RC5=0;
            sprintf(Buffer1, "Time : 00:00:00 ");
            lcd_gotoxy(1, 1);
            lcd_puts(Buffer1);
            sprintf(Buffer2, "CT:%5.1fC CK:OFF",CT);
            lcd_gotoxy(1, 2);
            lcd_puts(Buffer2);
            sprintf(Buffer3, "SP:%5.1fC HT:OFF",SP);
            lcd_gotoxy(1, 3);
            lcd_puts(Buffer3);
            sprintf(Buffer4, "MD:SEC          ");
            lcd_gotoxy(1, 4);
            lcd_puts(Buffer4);
        }
    }
    return;
}

void setupPorts(void) {    
    TRISA = 0xFF; 
    TRISB = 0xFF; 
    TRISC = 0x80; 
    TRISD = 0x00; 
    TRISE = 0x00; 
}

void setupInterrupts(void) {
    INTCON = 0; // disable interrupts first, then enable the ones u want
    INTCONbits.INT0E = 1;
    INTCONbits.T0IE = 1;
    INTCON2 = 0;
    INTCON2bits.INTEDG0 = 0;
    INTCON3 = 0;
    INTCON3bits.INT1E = 1;
    INTCON3bits.INT2E = 1;
    INTCON2bits.INTEDG1 = 0;
    INTCON2bits.INTEDG2 = 0;
    T0CON = 0;
    reloadTimer0();
    RCONbits.IPEN = 0; // Disable Interrupt priority , All are high
    PIE1 = 0;
    PIR1 = 0;
    PIE2 = 0; // all interrupts in PIE are disabled
    INTCONbits.GIEH = 1; // enable global interrupt bits
    INTCONbits.GIEL = 1; // enable global interrupt bits    
    T0CONbits.T0PS = 0b011;
    T0CONbits.TMR0ON = 1; //start time
}
//void __interrupt(high_priority) highIsr(void)
void interrupt high_priority highIsr(void)
{ 
    if (INTCONbits.TMR0IF) timer0_isr();
    else if (INTCONbits.INT0IF) EXT_Int0_isr();
    else if (INTCON3bits.INT2F) EXT_Int2_isr();
    else if (INTCON3bits.INT1F) EXT_Int1_isr();
}

void timer0_isr(void) {
    INTCONbits.TMR0IF = 0;
    if (CK_state){
        unsigned char channe2 = 2;
        unsigned char channe0 = 0;    
        CT = read_adc_temp2((unsigned char) channe2);
        SP = read_adc_temp((unsigned char) channe0);
        if (CT < SP+1){
            //heater on
            PORTCbits.RC5=1;
            HT_state=1;
        }else if (CT > SP-1){
            //heater off
            PORTCbits.RC5=0;
            HT_state=0;
        }else {
            //nothing
        }             
        if (Totsec>0){
            Totsec=Totsec-1;
        }else {
            clock_setup_SMH = 0; 
            sec = 0, min = 0, hour = 0;
            Totsec=0;//max 36000
            CK_state = 0, HT_state = 0; 
            CT=0;
            SP=0;
            strcpy(m0, m1);
            PORTCbits.RC5=0;
            sprintf(Buffer1, "Time : 00:00:00 ");
            lcd_gotoxy(1, 1);
            lcd_puts(Buffer1);
            sprintf(Buffer2, "CT:%5.1fC CK:OFF",CT);
            lcd_gotoxy(1, 2);
            lcd_puts(Buffer2);
            sprintf(Buffer3, "SP:%5.1fC HT:OFF",SP);
            lcd_gotoxy(1, 3);
            lcd_puts(Buffer3);
            sprintf(Buffer4, "MD:SEC          ");
            lcd_gotoxy(1, 4);
            lcd_puts(Buffer4);  
            // turn buzzer on
            for (int i=0 ; i <8 ; i++){
                PORTCbits.RC1=!PORTCbits.RC1;
                delay_ms(1000);
            }
            return;
        }
    }
    hour = (Totsec/3600); 
	min = (Totsec -(3600*hour))/60;
	sec = (Totsec -(3600*hour)-(min*60));
    reloadTimer0();
    sprintf(Buffer1, "Time : %02d:%02d:%02d ", hour, min, sec);
    lcd_gotoxy(1, 1);
    lcd_puts(Buffer1);
    if (CK_state){
        sprintf(Buffer2, "CT:%5.1fC CK:%s",CT,StateON);
        lcd_gotoxy(1, 2);
        lcd_puts(Buffer2);
    }else {
        sprintf(Buffer2, "CT:%5.1fC CK:%s",CT,StateOFF);
        lcd_gotoxy(1, 2);
        lcd_puts(Buffer2);    
    }
    if (HT_state){
        sprintf(Buffer3, "SP:%5.1fC HT:%s",SP,StateON);
        lcd_gotoxy(1, 3);
        lcd_puts(Buffer3);
    }else {
        sprintf(Buffer3, "SP:%5.1fC HT:%s",SP,StateOFF);
        lcd_gotoxy(1, 3);
        lcd_puts(Buffer3);    
    } 
}

void reloadTimer0(void) {
    TMR0H = (unsigned char) ((STARTVALUE >> 8) & 0x00FF);
    TMR0L = (unsigned char) (STARTVALUE & 0x00FF);
}
void EXT_Int2_isr(void) {
    //stop and pause
    INTCON3bits.INT2F = 0;
    CK_state=0;
    HT_state=0;
    PORTCbits.RC5=0;
    PORTDbits.RD1 = CK_state;
}
void EXT_Int1_isr(void) {
    //start and resum
    INTCON3bits.INT1F = 0;
    if (Totsec>0){ 
    CK_state=1;
    HT_state=1;
    PORTCbits.RC5=1;
    PORTDbits.RD1 = CK_state;
    }
}

void EXT_Int0_isr(void) {
    INTCONbits.INT0IF = 0; 
        if (clock_setup_SMH == 0) {
            clock_setup_SMH = 1;
        } else if (clock_setup_SMH == 1) {
            clock_setup_SMH = 2;
        } else if (clock_setup_SMH == 2) {
            clock_setup_SMH = 3;
        } else if (clock_setup_SMH == 3) {
            clock_setup_SMH = 4;
        } else if (clock_setup_SMH == 4) {
            clock_setup_SMH = 0;
        }
        switch (clock_setup_SMH) {
            case 0:strcpy(m0, m1);
                break;
            case 1:strcpy(m0, m2);
                break;
            case 2:strcpy(m0, m3);
                break;    
            case 3:strcpy(m0, m4);
                break;
            case 4:strcpy(m0, m5);
                break;
            default: strcpy(m0, m1);
        }
        sprintf(Buffer4,"MD:%s" ,m0);
        lcd_gotoxy(1, 4);
        lcd_puts(Buffer4);        
}