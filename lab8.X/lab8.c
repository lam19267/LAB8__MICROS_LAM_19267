/*
 * File:   lab8.c
 * Author: gelp0
 *
 * Created on 19 de abril de 2021, 10:14 PM
 * * Laboratorio 07
 * Archivo:      lab4.S
 * Dispositivo:	PIC16F887
 * Autor:	Guillermo Lam
 * Compilador:	pic-as (v2.30), MPLABX V5.45
 * 
 * Programa:	contador con potenciometros
 * Hardware:	potenciometros y displays
 *
 * Creado: 14 abr, 2021
 * Ultima modificacion: 14 abr, 2021
 */
#pragma config FOSC = INTRC_NOCLKOUT        // Oscillator Selection bits (LP oscillator: Low-power crystal on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF          // Code Protection bit (Program memory code protection is enabled)
#pragma config CPD = OFF         // Data Code Protection bit (Data memory code protection is enabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 2.1V)
#pragma config WRT = OFF       // Flash Program Memory Self Write Enable bits (0000h to 0FFFh write protected, 1000h to 1FFFh may be modified by EECON control)

#define _XTAL_FREQ 4000000
#include <xc.h>
#include <stdint.h>

char sietes[10] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110,
    0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111}; 
//TABLA 7 SEGMENTOS
char centenas;  //variable de centenas
char decenas;   //variable de decenas
char unidades;  //variable de unidades
int  banderas;  //variable de banderas
char num;       //variable de num
char resultado; //variable de resultado
char resultados;//variable deresultados


void config(void);//funcion de config
char division(void);//funcion de division

void __interrupt() isr(void){ //interrupciones
    if(T0IF == 1){   
        PORTEbits.RE0 = 0;  //se prenden y se apagan los transistores      
        PORTEbits.RE1 = 1;      
        PORTD = sietes[decenas];  //se depliega decenas en el disp
        banderas = 1;               //se le asigna un valor a banderas para
        if (banderas == 1){         //seguir prendiendo los otros displays
            PORTEbits.RE1 = 0;  
            PORTEbits.RE2 = 1;
            PORTD = sietes[centenas]; //se despliega centenas en el disp   
            banderas = 2;}
        if (banderas == 2){
            PORTEbits.RE2 = 0;      
            PORTEbits.RE0 = 1;
            PORTD = sietes[unidades];  //se despliega unidades en el disp
            banderas = 0;}
        INTCONbits.T0IF = 0;    
        TMR0 = 255;}
    //se hace el reset del TMR0
    if(PIR1bits.ADIF == 1){
        if(ADCON0bits.CHS == 0)
            PORTC = ADRESH;
        
        else
            num = ADRESH;
            
        PIR1bits.ADIF = 0;
        }
    }

void main(void) {   //se pone el main
    config();       //se llama a la configuracion del TMR0 e interrupciones
    ADCON0bits.GO = 1;
    while(1){
        
        if (ADCON0bits.GO == 0){
            if (ADCON0bits.CHS == 1){
                ADCON0bits.CHS = 0;}
            else{
                ADCON0bits.CHS = 1;}
            __delay_us(100);
            ADCON0bits.GO = 1;
        }
        division();
    }
        //se pone la division en el loop
    }

void config(void){
    ANSEL = 0b00000011;   //se ponen los pines digitales
    ANSELH = 0b11111111;
  
    TRISA = 0b00000011;
    TRISC = 0x00;           //se asignan las salidas
    TRISD = 0x00;
    TRISE = 0x00;
    
    PORTA = 0x00;
    PORTB = 0x00;           //se limpian los PORTS
    PORTC = 0x00;
    PORTD = 0x00;
    PORTE = 0x00;
    
    OSCCONbits.IRCF2 = 0;   //se configura el oscilador
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 0;   
    OSCCONbits.SCS = 1;
    
    OPTION_REGbits.T0CS = 0;    //se configura el PreScaler del TMR0
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS2 = 1;
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;
   
    ADCON1bits.ADFM = 0;
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    
    ADCON0bits.ADCS0 = 1;
    ADCON0bits.ADCS1 = 1;
    ADCON0bits.CHS = 0;
    ADCON0bits.ADON = 1;
    
    
    INTCONbits.GIE = 1;         //se activan las interrupciones
    INTCONbits.T0IE = 1;
    INTCONbits.T0IF = 0;
    INTCONbits.PEIE = 1;
    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 0;
}

char division(void){            //se crea la funcion de division               //se mueve el valor del PORTC a una variable
    centenas = num/100;         //se divide esta variable por 100 y se asigna
    resultado = num%100;        //se guarda el residuo en otra variable
    decenas = resultado/10;     //se divide esta variable por 10 y se asigna
    resultados = resultado%10;  //se guarda el residuo en otra variable
    unidades = resultados;
    return 0;//se asigna esta variable
}
