/*
 * File:   main.c
 * Author: olate
 *
 * Created on September 13, 2021, 7:15 AM
 */


#include <xc.h>
// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF         // Data EEPROM Memory Code Protection bit (Data EEPROM code-protected)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF          // Flash Program Memory Code Protection bit (All program memory code-protected)

#define _XTAL_FREQ 20000000
//#define RS RB6
//#define EN RB5
//#define D4 RB4
//#define D5 RB3
//#define D6 RB2
//#define D7 RB1
#include "LCDLIBRARY1.h"

int phcnADCvalue=0;
int generatorADCvalue=0;
int switched=1;
//int toggle=0;
#define phcnControl RD2
#define generatorControl RD3
#define neutralLineControl RC4
int phcnON=0;
int generatorON=0;
#define servo RC2
#define thresh 920

void main(void) {
    RC2=0;
    TRISB=0x00;
    PORTB=0x00;
    ADCON1=0xC2;
    ADCON0=0xC1;
    TRISA=0xFF;
    TRISD=0xC0;
    
    PORTD=0x00;  
    TRISC=0x00;
    
//    for(int i=0;i<50;i++){
//        RC2=1;
//        __delay_us(2000);
//        RC2=0;
//        __delay_us(18000);
//    }
//    __delay_ms(1000);
//    for(int i=0;i<50;i++){
//        RC2=1;
//        __delay_us(1500);
//        RC2=0;
//        __delay_us(18500);
//    }
//    __delay_ms(1000);
//    for(int i=0;i<50;i++){
//        RC2=1;
//        __delay_us(1000);
//        RC2=0;
//        __delay_us(19000);
//    }
//    __delay_ms(1000);
    
    config();
    CLRDISP();
    CLRDISP();
    CURSOR(FIRSTROW,3);
    LCDWRITE("ATS SYSTEM");
    
    neutralLineControl=0;   //to be 0 when source is PHCN and 1 when sourcer is generator
    phcnControl=0;
//    generatorControl=1;
//    __delay_ms(3000);
    generatorControl=0;
    
    
    while(1){
        CLRDISP();
        CURSOR(FIRSTROW,3);
        LCDWRITE("ATS SYSTEM");
         __delay_ms(100);
        ADCON0bits.CHS=1;
         __delay_us(20);
        ADCON0bits.GO_nDONE=1;
        while(ADCON0bits.GO_nDONE);
        phcnADCvalue=(ADRESH<<8)+ADRESL;
        __delay_us(100);
        ADCON0bits.CHS=0;
         __delay_us(20);
        ADCON0bits.GO_nDONE=1;
        while(ADCON0bits.GO_nDONE);
        generatorADCvalue=(ADRESH<<8)+ADRESL;
        __delay_us(100);
        
        if(phcnADCvalue<920){
            //source is phcn
            if(phcnON==0){
                generatorControl=0;//first of all turn off all disconnect from all source and then set neutral to phcn-neutral
                phcnControl=0;
                neutralLineControl=0;//set set neutral to phcn-neutral
                __delay_ms(250);
                phcnControl=1;//set live to phcn live
                phcnON=1;
                generatorON=0;
            }
            /*
             * +
             * 1.switch choke
             * 2.check if generator has turned off
             * 3.if generator has turned off return choke
             */
            if(switched>=1){//to switch off gen by turning servo to turn off choke
                switched=0;
                for(int i=0;i<50;i++){
                    RC2=1;
                    __delay_us(2000);
                    RC2=0;
                    __delay_us(18000);
                }
            }
            CURSOR(SECONDROW,0);
            LCDWRITE("phcn");
            __delay_ms(2500);
            RC3=1;
        }else{
            phcnControl=0;
            if(generatorADCvalue<920){
                if(generatorON==0){
                    generatorControl=0;//first of all turn off all disconnect from all source and then set neutral to generator-neutral
                    phcnControl=0;
                    neutralLineControl=1;//set set neutral to generator-neutral
                    __delay_ms(250);
                    generatorControl=1;//set live to generator live
                    generatorON=1;
                    phcnON=0;
                    for(int i=0;i<50;i++){//to activate choke so gen can turn on later
                        RC2=1;
                        __delay_us(1000);
                        RC2=0;
                        __delay_us(19000);
                    }
                    switched=1;
                }
                CURSOR(SECONDROW,0);
                LCDWRITE("generator");
                __delay_ms(2500);
            }else{
                CURSOR(SECONDROW,0);
                LCDWRITE("null");
            }
        }
        __delay_ms(100);
    }
    __delay_ms(250);
    return;
}
