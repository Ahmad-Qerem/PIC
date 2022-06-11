#pragma config OSC = XT         // Oscillator Selection bits (XT oscillator)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)
#pragma config WDT = ON         // Watchdog Timer Enable bit (WDT enabled)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (00C000-00FFFFh) not code-protected)
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (00C000-00FFFFh) not write-protected)
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)
#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "lcd_x8.h"
#include "my_adc.h"
#include "my_pwm.h"
#define STARTVALUE 60536
#define STARTVALUE1 3036
#define WINTER_T 40
#define SUMMER_T 60
void setupPorts(void);
void setupInterupt(void);
void setupTimers(void);
void TMR3_reset(void);
void TMR0_isr(void);
void TMR3_isr(void);
void EXT_Int0_isr(void);
void EXT_Int1_isr(void);
void EXT_Int2_isr(void);
void print_data(void);
void RC_isr(void);
void send_string_no_lib(unsigned char *p);
char Buffer1[17], Buffer2[17], Buffer3[17], Buffer4[17];
float RT,SP,OT,FanSpeed;
int HS,H,C,MD,HCP;
int GlobalConter,TonCounter;
unsigned int RPS_C = 0;
char Hstate='N',Cstate='N',HCstate='N',Received='$';
char MDS[17]="";
char MD0S[17]="MD:OFF          ";
char MD1S[17]="MD:Heat         ";
char MD2S[17]="MD:Cool         ";
char MD3S[17]="MD:Auto HC HS:  ";
void main(void) {
    setupPorts();
    setupInterupt();
    setupTimers();
    lcd_init();
    init_pwm1();//uses timer 2
    setupSerial();
    init_adc_no_lib();
    TRISCbits.RC0 = 1;
    RT=0,SP=0,OT=0,H=0,C=0,MD=0,HCP=0,HS=0,FanSpeed=0,GlobalConter=0,TonCounter=0;
    PIE1bits.TXIE=0;
    sprintf(Buffer1, "RT:  0.0C    H C");
    lcd_gotoxy(1, 1);
    lcd_puts(Buffer1);
    sprintf(Buffer2, "SP:  0.0C    N N");
    lcd_gotoxy(1, 2);
    lcd_puts(Buffer2);
    sprintf(Buffer3, "OT:  0.0C R: 0.0%%");
    lcd_gotoxy(1, 3);
    lcd_puts(Buffer3);
    sprintf(Buffer4, "MD:OFF          ");
    lcd_gotoxy(1, 4);
    lcd_puts(Buffer4); 
    while (1){
        CLRWDT();
        delay_ms(200);
        
        if (PORTBbits.RB3==0||Received=='S'){
            delay_ms(200);
            MD=0; 
            Received='$';
        }
        RT = read_adc_voltage(2);
        RT = RT * 100;
        SP = read_adc_voltage(0);
        SP = SP * 100 / 5;
        OT = read_adc_voltage(1);
        OT = OT * 100 / 5;
        if (MD==0){
            RT=0,SP=0,OT=0,H=0,C=0,HCP=0;
            HCstate='R';
            set_pwm1_percent(0);
            strcpy(MDS,MD0S);
            FanSpeed = 0;
        }else if (MD==1){
            PORTCbits.RC2 = 0;
            HCP=H*5;
        }else if (MD==2){
            PORTCbits.RC5 = 0;
            HCP=C*5;
            set_pwm1_percent(HCP);
        }else if (MD==3) {
            if (OT > SUMMER_T) {
                float coolError = RT - SP;
                if (coolError > 0) {
                    H=0;
                    PORTCbits.RC5 = 0;
                    C = coolError * 10;
                    if (C < 25) C = 25;
                    set_pwm1_percent(C);
                }
                if (RT < (SP - HS)) {
                    C=0;
                    H=10;
                    set_pwm1_percent(0);
                }
            } // Control Cool
            else if (OT < WINTER_T) {
                float HeatError = SP - RT;
                if(HeatError > 0){
                    H = 2 * HeatError; 
                    if(H >20) H = 20 ;
                    if(H < 10) H = 10;
                    if (SP > 52) H = 20;
                    C=0;
                    set_pwm1_percent(0);
                }
                if(RT > (SP + HS)) {
                    H = 0;
                    C = 0;
                    HCP = 0;
                    PORTCbits.RC5 = 0;
                    set_pwm1_percent(0);
                }
            } // Control Heat
            else {
                float HeatError = SP - RT;
                if (HeatError > 0) {
                    H = HeatError;
                    if (H > 20) H = 20;
                    if (H < 10) H = 10;
                    if (SP > 52) H = 20;
                    C=0;
                    set_pwm1_percent(0);
                } else {
                    float CoolError = RT - SP;
                    H=0;
                    //PORTCbits.RC5 = 0;
                    C = CoolError * 10;
                    if (C < 25) C = 25;
                    set_pwm1_percent(C);
                }     
            }   
            
        }
        FanSpeed = RPS_C/7.0;
        print_data();
    }
    return;
}
void interrupt high_priority highIsr(void) 
{
    if (INTCONbits.INT0IF) EXT_Int0_isr();
    else if (INTCON3bits.INT2F) EXT_Int2_isr();
    else if (INTCON3bits.INT1F) EXT_Int1_isr();
    else if (PIR2bits.TMR3IF) TMR3_isr();
    else if (INTCONbits.TMR0IF) TMR0_isr();
    else if (PIR1bits.RCIF) RC_isr();
}
void RC_isr(void) {
    //PIR1bits.RCIF = 0;
    if (RCSTAbits.FERR || RCSTAbits.OERR)//check for error
    {
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
    }else 
        Received = RCREG;
}
void TMR3_reset(void) {
    TMR3H = (unsigned char) ((STARTVALUE >> 8) & 0x00FF);
    TMR3L = (unsigned char) (STARTVALUE & 0x00FF);
}
void setupPorts(void){
    ADCON0 = 0;
    ADCON1 = 0b00001100; //3 analog channels, change this according to your application
    TRISA = 0xFF; 
    TRISB = 0xFF; 
    TRISC = 0x80; 
    TRISD = 0x00; 
    TRISE = 0x00; 
}
void setupInterupt(void){
    RCONbits.IPEN = 0;
    INTCON = 0;
    INTCON2 = 0;
    INTCON3 = 0;
    INTCONbits.INT0IE=1;//E ITN 0 RB0
    INTCON2bits.INTEDG0=1;
    INTCON3bits.INT1E=1;
    INTCON2bits.INTEDG1=1;
    INTCON3bits.INT2E=1;
    INTCON2bits.INTEDG2=1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
    INTCONbits.INT0IF=0;
    INTCON3bits.INT1F = 0;
    INTCON3bits.INT2F = 0;
}
void setupTimers(void){
    //TIMERE 0
    T0CON = 0;
    INTCONbits.T0IF = 0;
    T0CONbits.T0PS0 = 1;
    T0CONbits.T0PS1 = 1;
    T0CONbits.T0PS2 = 0;
    TMR0H = (unsigned char) ((STARTVALUE1 >> 8) & 0x00FF);
    TMR0L = (unsigned char) (STARTVALUE1 & 0x00FF);
    //TIMERE 1
    T1CONbits.TMR1CS = 1; //external clock ,emasuring the speed of the fan in RPS
    T1CONbits.T1CKPS1 = 0;
    T1CONbits.T1CKPS0 = 0;
    TMR1H = 0;
    TMR1L = 0;
    INTCONbits.GIE = 1; //enable only timer 0 interrupt
    INTCONbits.T0IE = 1;
    T1CONbits.TMR1ON = 1;
    T0CONbits.TMR0ON = 1;
    //TIMERE 3
    T3CON=0;
    T3CONbits.RD16=1;
    T3CONbits.T3CKPS=0b10;
    TMR3H = (unsigned char) ((STARTVALUE >> 8) & 0x00FF);
    TMR3L = (unsigned char) (STARTVALUE & 0x00FF);
    PIE1=0;
    
    PIE1bits.RCIE=1;
    //PIE1bits.TXIE=1;
    PIR1bits.RCIF=0;
    PIR1bits.TXIF=0;
    
    PIE2=0;
    PIE2bits.TMR3IE = 1;
    PIR2bits.TMR3IF = 0;
    T3CONbits.TMR3ON=1;    
}
void TMR0_isr(void){
    INTCONbits.T0IF = 0;
    RPS_C = ((unsigned int) TMR1H << 8) | (TMR1L); 
    TMR0H = (unsigned char) ((STARTVALUE1 >> 8) & 0x00FF);
    TMR0L = (unsigned char) (STARTVALUE1 & 0x00FF);
    TMR1H = 0;
    TMR1L = 0;
}
void TMR3_isr(void){
    TMR3_reset();
    PIR2bits.TMR3IF = 0;
    if (GlobalConter < 20) {
        if (MD == 1 || MD==3) {
            if (GlobalConter < H) {
                PORTCbits.RC5 = 1;
            } else {
                PORTCbits.RC5 = 0;
            }
            GlobalConter += 1;
        }
    } else {
        GlobalConter = 0;
    }
}
void EXT_Int0_isr(void) {
    INTCONbits.INT0F = 0;
    switch (MD) {
        case 0:
            MD = 1;strcpy(MDS,MD1S);HCstate='H';H=0;C=0;HCP=0;
            break;
        case 1:MD = 2;strcpy(MDS,MD2S);HCstate='C';H=0;C=0;HCP=0;
            break;
        case 2:MD = 3;PORTCbits.RC5 = 0;set_pwm1_percent(0);TMR3_reset();strcpy(MDS,MD3S);HCstate='R';H=0;C=0;HCP=0;
            break;
        case 3:MD = 1;strcpy(MDS,MD1S);set_pwm1_percent(0);HCstate='H';H=0;C=0;HCP=0;
        default:
            break;
    }  
}
void EXT_Int1_isr(void){
    INTCON3bits.INT1F = 0;
    if (MD == 1) {
        if (H > 0) {
            H--; 
        }
    } else if (MD == 2) {
        if (C > 0) {
            C--;
        }
    } else if (MD == 3) {
        if (HS > 0) {
            HS--;
        }
    }
}
void EXT_Int2_isr(void) {
    INTCON3bits.INT2F = 0;
    if (MD == 1) {
        if (H < 20) {
            H++;
        }
    } else if (MD == 2) {
        if (C < 20) {
            C++;
        }
    } else if (MD == 3) {
        if (HS < 4) {
            HS++;
        }
    }
}
void print_data(void){
    sprintf(Buffer1, "RT:%5.1fC    H C",RT);
    lcd_gotoxy(1, 1);
    lcd_puts(Buffer1);
    
    if (C>0)
        Cstate='Y';
    else 
        Cstate='N';
    if (H>0)
        Hstate='Y';
    else 
        Hstate='N';
    
    sprintf(Buffer2, "SP:%5.1fC    %c %c", SP, Hstate, Cstate);
    lcd_gotoxy(1, 2);
    lcd_puts(Buffer2);    
    
    if (HCstate!='R'){
        sprintf(Buffer3, "OT:%5.1fC %c:%3d%%",OT,HCstate,HCP);
    }else {
        sprintf(Buffer3, "OT:%5.1fC R:%4.1f",OT,FanSpeed);
    }
    
    lcd_gotoxy(1, 3);
    lcd_puts(Buffer3);
    
    if (MD!=3){
        sprintf(Buffer4,MDS);
    }else {
        sprintf(Buffer4,"MD:Auto HC HS:%d",HS);
    }
    lcd_gotoxy(1, 4);
    lcd_puts(Buffer4);
    
    if (Received=='M' && MD!=0) {
        char Buffer[64];
        char MODE[8];
        char HeaterState[6];
        char CoolerState[6];
        if (MD==1){
            strcpy(MODE,"Heat");
        }else if (MD==2){
            strcpy(MODE,"Cool");
        }else if (MD==3){
            strcpy(MODE,"Auto HC");
        }
        if (H>0){
            strcpy(HeaterState,"ON ,");
        }else{
            strcpy(HeaterState,"OFF ,");
        }
        if (C>0){
            strcpy(CoolerState,"ON \r\n");
        }else {
            strcpy(CoolerState,"OFF \r\n");
        }
        sprintf(Buffer,"RT:%4.1fC , SP:%4.1fC , OT:%4.1f\r\n",RT,SP,OT);
        send_string_no_lib(Buffer);
        strcpy(Buffer,"");
        sprintf(Buffer,"MODE: ");
        send_string_no_lib(Buffer);
        sprintf(Buffer,MODE);
        send_string_no_lib(Buffer);
        
        sprintf(Buffer," , Heater: ");
        send_string_no_lib(Buffer);
        sprintf(Buffer,HeaterState);
        send_string_no_lib(Buffer);
        
        sprintf(Buffer,"Cooler: ");
        send_string_no_lib(Buffer);
        sprintf(Buffer,CoolerState);
        send_string_no_lib(Buffer);
        
        send_string_no_lib((unsigned char *)"----------------------------\r\r\n");
    }
}