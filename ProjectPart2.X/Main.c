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

#include <xc.h>
#include <stdio.h>
#include "lcd_x8.h"
//#include "my_ser.h"

#define RowA PORTBbits.RB0
#define RowB PORTBbits.RB1
#define RowC PORTBbits.RB2
#define RowD PORTBbits.RB3
#define C1 PORTBbits.RB4
#define C2 PORTBbits.RB5
#define C3 PORTBbits.RB6
#define C4 PORTCbits.RC0

void setupPorts(void);
void disp_num(float num);
int get_num  (char ch);
char GetKey(void); 
char scan_key(void);

int k2, k1;
char ke, key, key1;
int position=0;
char Buffer1[16];

void main(void) {
    setupPorts();
    lcd_init();
    
    sprintf(Buffer1, " Ahmad -- Yahia ");
    lcd_gotoxy(1, 1);
    lcd_puts(Buffer1);
    for (int i=0;i<5;i++)
        delay_ms(1000); 
    lcd_putc('\f');
    
    while (1) {
        CLRWDT();
        delay_ms(200); 
        
        sprintf(Buffer1, "ENTER NUM 1 = ");
        lcd_gotoxy(1, 1);
        lcd_puts(Buffer1);
        position=15;
        
        //ke = scan_key(); //Scan the First Digit
        ke=GetKey();
        k2 = get_num(ke); //Converting Char into number
        lcd_gotoxy(position, 1);
        lcd_putc(ke);
     
        delay_ms(1000);
        lcd_putc('\f');
        
        sprintf(Buffer1, "ENTER NUM 2 = " );
        lcd_gotoxy(1, 1);
        lcd_puts(Buffer1);
        position=15;
        
        //read_byte_no_lib(); read keypad
        
        //key1 = scan_key(); //Scan Second digit
        key1=GetKey();
        k1 = get_num(key1); //Converting Char into number
        
        lcd_gotoxy(position, 1);
        lcd_putc(key1);
        
        delay_ms(1000);
        lcd_putc('\f');
        
        sprintf(Buffer1, "ENTER OP = " );
        lcd_gotoxy(1, 1);
        lcd_puts(Buffer1);
        position=11;
        
        key=GetKey();
        lcd_gotoxy(position, 1);
        lcd_putc(key);
        
        delay_ms(1000);
        lcd_putc('\f');
        
        sprintf(Buffer1, "  ***RESULT*** " );
        lcd_gotoxy(1, 1);
        lcd_puts(Buffer1);

        sprintf(Buffer1,"%c %c %c  = ",ke,key,key1);
        lcd_gotoxy(1, 2);
        lcd_puts(Buffer1);
        
        position=11;
        
        //send_byte_no_lib(x);print at tera term
        switch (key) {
            case '+':
                disp_num(k1 + k2);
                break;
            case '-':
                disp_num(k2 - k1);
                break;
            case '*':
                disp_num(k2 * k1);
                break;
            case '/':
                disp_num((float) k2 / k1);
                break;
        }
        
        for (int i = 0 ; i< 10 ; i++ )
            delay_ms(1000);
        
        lcd_putc('\f');
    }
    
    return;
}
void setupPorts(void) {    
    //ADCON1 = 0xF;
    //INTCON = 0;
    
    
    LATA = LATB = LATC = LATD = LATE =0;
    TRISA = 0xFF; 
    TRISB = 0b11110000; 
    TRISC = 0b10000001; 
    TRISD = 0x00; 
    TRISE = 0x00; 
    INTCON2 = 0;
}



char READ_SWITCHES(void) {
    RowA = 0;RowB = 1;RowC = 1;RowD = 1; 
    if (C1 == 0) {delay_ms(250);while (C1 == 0);return '7';}
    if (C2 == 0) {delay_ms(250);while (C2 == 0);return '8';}
    if (C3 == 0) {delay_ms(250);while (C3 == 0);return '9';}
    if (C4 == 0) {delay_ms(250);while (C4 == 0);return '/';}
    RowA = 1;RowB = 0;RowC = 1;RowD = 1; 
    if (C1 == 0) {delay_ms(250);while (C1 == 0);return '4';}
    if (C2 == 0) {delay_ms(250);while (C2 == 0);return '5';}
    if (C3 == 0) {delay_ms(250);while (C3 == 0);return '6';}
    if (C4 == 0) {delay_ms(250);while (C4 == 0);return 'x';}
    RowA = 1;RowB = 1;RowC = 0;RowD = 1; 
    if (C1 == 0) {delay_ms(250);while (C1 == 0);return '1';}
    if (C2 == 0) {delay_ms(250);while (C2 == 0);return '2';}
    if (C3 == 0) {delay_ms(250);while (C3 == 0);return '3';}
    if (C4 == 0) {delay_ms(250);while (C4 == 0);return '-';}
    RowA = 1;RowB = 1;RowC = 1;RowD = 0; 
    if (C1 == 0) {delay_ms(250);while (C1 == 0);return 'C';}
    if (C2 == 0) {delay_ms(250);while (C2 == 0);return '0';}
    if (C3 == 0) {delay_ms(250);while (C3 == 0);return '=';}
    if (C4 == 0) {delay_ms(250);while (C4 == 0);return '+';
    }
    return 'n'; // Means no key has been pressed
}

char GetKey(void){
    char S = 'n';              // Assume no key pressed
    while(S=='n')              // Wait untill a key is pressed
        S = READ_SWITCHES();   // Scan the keys again and again
    return S;                  //when key pressed then return its value
}
char scan_key() //Scan the Pressed Key by user 
{
    unsigned char c = 's';
    while (c != 'a') {
        RowA = 1;RowB = 0;RowC = 0;RowD = 0;
        if (C1 == 1 && RowA == 1) {
            delay_ms(500);
            lcd_putc('7');
            return '7';
        }
            c = 'a';
        
        if (C2 == 1 && RowA == 1) {
            delay_ms(500);
            lcd_putc('8');
            return '8';
        }
            c = 'a';
        
        if (C3 == 1 &9& RowA == 1) {
            delay_ms(500);
            lcd_putc('9');   
            return '9';
        }
            c = 'a';
        
        if (C4 == 1 && RowA == 1) {
            delay_ms(500);
            lcd_putc('/');
            
            return '/';
            c = 'a';
        }
        RowA = 0;RowB = 1;RowC = 0;RowD = 0;
        if (C1 == 1 && RowB == 1) {
            lcd_putc('4');
            delay_ms(500);
            return '4';
            c = 'a';
        }
        if (C2 == 1 && RowB == 1) {
            lcd_putc('5');
            delay_ms(500);
            return '5';
            c = 'a';
        }
        if (C3 == 1 && RowB == 1) {
            lcd_putc('6');
            delay_ms(500);
            return '6';
            c = 'a';
        }
        if (C4 == 1 && RowB == 1) {
            lcd_putc('*');
            delay_ms(500);
            return '*';
            c = 'a';
        }
        
        RowA = 0;RowB = 0;RowC = 1;RowD = 0;
        if (C1 == 1 && RowC == 1) {
            lcd_putc('1');
            delay_ms(500);
            return '1';
            c = 'a';
        }
        if (C2 == 1 && RowC == 1) {
            lcd_putc('2');
            delay_ms(500);
            return '2';
            c = 'a';
        }
        if (C3 == 1 && RowC == 1) {
            lcd_putc('3');
            delay_ms(500);
            return '3';
            c = 'a';
        }
        if (C4 == 1 && RowC == 1) {
            lcd_putc('-');
            delay_ms(500);
            return '-';
            c = 'a';
        }
        RowA = 0;RowB = 0;RowC = 0;RowD = 1;
        if (C2 == 1 && RowD == 1) {
            lcd_putc('0'); 
            delay_ms(500);
            return '0';
            c = 'a';
        }
        if (C4 == 1 && RowD == 1) {
            lcd_putc('+');
            delay_ms(500);
            return '+';
            c = 'a';
        }
    }
    return 0;
}

int get_num(char ch) //converting character into integer
{
    switch (ch) {
        case '0': return 0;
            break;
        case '1': return 1;
            break;
        case '2': return 2;
            break;
        case '3': return 3;
            break;
        case '4': return 4;
            break;
        case '5': return 5;
            break;
        case '6': return 6;
            break;
        case '7': return 7;
            break;
        case '8': return 8;
            break;
        case '9': return 9;
            break;
    }
    return 0;
}

void disp_num(float num) //Displays calculated value on LCD
{
    unsigned char UnitDigit = 0; //Contains unit digit of calculated value
    unsigned char TenthDigit = 0; //contains 10th digit of calculated value
    unsigned char decimal = 0;
    int j, numb;

    j = (int) (num * 10);
    numb = (int) num;

    if (numb < 0) {
        numb = -1 * numb; // Make number positive
        lcd_gotoxy(position++, 2);
        lcd_putc('-');
        
        //lcddata('-'); // Display a negative sign on LCD
    }

    TenthDigit = (numb / 10); // Findout Tenth Digit

    if (TenthDigit != 0){ // If it is zero, then don't display
        lcd_gotoxy(position++, 2);
        lcd_putc(TenthDigit + 0x30);
        //lcddata(TenthDigit + 0x30); // Make Char of TenthDigit and then display it on LCD
    }
    UnitDigit = numb - (TenthDigit * 10);
    lcd_gotoxy(position++, 2);
    lcd_putc(UnitDigit + 0x30);
    //lcddata(UnitDigit + 0x30); // Make Char of UnitDigit and then display it on LCD
    lcd_gotoxy(position++, 2);
    lcd_putc('.');
    //lcddata('.');
    decimal = (j % 10) + 0x30; //Display If any value after Decimal Point
    lcd_gotoxy(position++, 2);
    lcd_putc(decimal);
    //lcddata(decimal);

    //lcdcmd(0x01);
}