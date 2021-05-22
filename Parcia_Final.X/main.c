/*
/////////////////// DATOS DEL PROGRAMA ////////////////////
//  TTITULO: Parcial_Final
//  MICRO: PIC16F15244-CNANO
//  ESTUDIANTES: 
?	Juan Felipe Betancourth Ledezma-2420191018
?	Braian Alejandro Tuberquia Arciniegas 2420182022
?	Willy Alejandro Cortes Lozano ? 2420151007
 * 
//  PROFESOR: Harold F MURCIA
//  FECHA: 23 de mayo de 2021
 * 
 * 
 * 
///////////// CONFIGURACION del MCU ////////////////// */

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <conio.h>


#pragma config FEXTOSC = OFF    // External Oscillator Mode Selection bits->Oscillator not enabled
#pragma config RSTOSC = HFINTOSC_1MHZ    // Power-up Default Value for COSC bits->HFINTOSC (1 MHz)
#pragma config CLKOUTEN = OFF    // Clock Out Enable bit->CLKOUT function is disabled; I/O function on RA4
#pragma config VDDAR = HI    // VDD Range Analog Calibration Selection bit->Internal analog systems are calibrated for operation between VDD = 2.3V - 5.5V

// CONFIG2
#pragma config MCLRE = EXTMCLR    // Master Clear Enable bit->If LVP = 0, MCLR pin is MCLR; If LVP = 1, RA3 pin function is MCLR
#pragma config PWRTS = PWRT_OFF    // Power-up Timer Selection bits->PWRT is disabled
#pragma config WDTE = OFF    // WDT Operating Mode bits->WDT disabled; SEN is ignored
#pragma config BOREN = ON    // Brown-out Reset Enable bits->Brown-out Reset Enabled, SBOREN bit is ignored
#pragma config BORV = LO    // Brown-out Reset Voltage Selection bit->Brown-out Reset Voltage (VBOR) set to 1.9V
#pragma config PPS1WAY = ON    // PPSLOCKED One-Way Set Enable bit->The PPSLOCKED bit can be cleared and set only once in software
#pragma config STVREN = ON    // Stack Overflow/Underflow Reset Enable bit->Stack Overflow or Underflow will cause a reset

// CONFIG4
#pragma config BBSIZE = BB512    // Boot Block Size Selection bits->512 words boot block size
#pragma config BBEN = OFF    // Boot Block Enable bit->Boot Block is disabled
#pragma config SAFEN = OFF    // SAF Enable bit->SAF is disabled
#pragma config WRTAPP = OFF    // Application Block Write Protection bit->Application Block is not write-protected
#pragma config WRTB = OFF    // Boot Block Write Protection bit->Boot Block is not write-protected
#pragma config WRTC = OFF    // Configuration Registers Write Protection bit->Configuration Registers are not write-protected
#pragma config WRTSAF = OFF    // Storage Area Flash (SAF) Write Protection bit->SAF is not write-protected
#pragma config LVP = ON    // Low Voltage Programming Enable bit->Low Voltage programming enabled. MCLR/Vpp pin function is MCLR. MCLRE Configuration bit is ignored.

// CONFIG5
#pragma config CP = OFF    // User Program Flash Memory Code Protection bit->User Program Flash Memory code protection is disabled



///////////// DEFINICIONES  //////////////////
#define _XTAL_FREQ 1000000
#define LED0_SetHigh()            LATAbits.LATA2 = 1;
#define LED0_SetLow()             LATAbits.LATA2 = 0;
#define ACQ_US_DELAY 10
///////////////

///////////// VARIABLES GLOBALES  //////////////////
uint16_t dutyCycle10 = 10;          // DUTYCUCLE 10%
uint16_t dutyCycle25 = 256;          // DUTYCUCLE 25%
uint16_t dutyCycle50 = 512;         // DUTYCUCLE 50%
uint16_t dutyCycle75 = 750;         // DUTYCUCLE 75%
uint16_t dutyCycle100 = 1023;       // DUTYCUCLE 100%
uint16_t my_dutyCycle = 0;            // DUTYCUCLE 0%
///////////// DECLARACI?N DE FUNCIONES Y PROCEDIMIENTOS ///////////////////
void PIN_MANAGER_Initialize(void)
{
    // LATx registers
    LATA = 0x00;
    LATB = 0x00;
    LATC = 0x00;

    // TRISx registers
    TRISA = 0x3B;
    TRISB = 0xF0;
    TRISC = 0xFF;

    // ANSELx registers
    ANSELC = 0xFB;
    ANSELB = 0xF0;
    ANSELA = 0x33;

    // WPUx registers
    WPUB = 0x00;
    WPUA = 0x04;
    WPUC = 0x04;

    // ODx registers
    ODCONA = 0x00;
    ODCONB = 0x00;
    ODCONC = 0x00;

    // SLRCONx registers
    SLRCONA = 0x37;
    SLRCONB = 0xF0;
    SLRCONC = 0xFF;

    // INLVLx registers
    INLVLA = 0x3F;
    INLVLB = 0xF0;
    INLVLC = 0xFF;

    // TRISx registers
    TRISA2 = 0;          // Definiendo puerto A2 como salida digital --LED
    TRISC2 = 1;          // Definiendo puerto C2 como entada digital--BOTON
    WPUC2  = 1;          // Activando resistencia d pull-up

    RA2PPS = 0x03;                     // RA2->PWM3:PWM3OUT;
    RA1PPS = 0x04;                     // RA1->PWM4:PWM4OUT;
}



void OSCILLATOR_Initialize(void)
{
    OSCEN = 0x00;                                                               // MFOEN disabled; LFOEN disabled; ADOEN disabled; HFOEN disabled;
    OSCFRQ = 0x00;                                                              // HFFRQ0 1_MHz
    OSCTUNE = 0x00;
}

void TMR2_Initialize(void)
{
    T2CLKCON = 0x01;                                                            // T2CS FOSC/4;
    T2HLT = 0x00;                                                               // T2PSYNC Not Synchronized; T2MODE Software control; T2CKPOL Rising Edge; T2CKSYNC Not Synchronized;
    T2RST = 0x00;
    T2PR = 249;
    T2TMR = 0x00;
    PIR1bits.TMR2IF = 0;                                                        // Clearing IF flag.
    T2CON = 0b10000000;                                                         // T2CKPS 1:1; T2OUTPS 1:1; TMR2ON on;
}

 void PWM3_Initialize(void)
 {
    PWM3CON = 0x90;                                                             // PWM3POL active_low; PWM3EN enabled;
    PWM3DCH = 0x3E;
    PWM3DCL = 0x40;
 }



 void PWM3_LoadDutyValue(uint16_t dutyValue)
    {     
        PWM3DCH = (dutyValue & 0x03FC)>>2;                                         // Writing to 8 MSBs of PWM duty cycle in PWMDCH register
        PWM3DCL = (dutyValue & 0x0003)<<6;                                         // Writing to 2 LSBs of PWM duty cycle in PWMDCL register
    }
 

/////////////  INICIO DEL PROGRAMA PRINCIPAL //////////////////////////



 void main(void)
{
    PIN_MANAGER_Initialize();
    OSCILLATOR_Initialize();
    TMR2_Initialize();
    PWM3_Initialize();
 
    while(1){
       
        //5 intervalos en 2 segundos 
        //Cada intervalo de 400ms

    if (PORTCbits.RC2==0){                  //Si se presiona el Boton
      PWM3_LoadDutyValue(my_dutyCycle);     //on --> 0%
      __delay_ms(400);
      PWM3_LoadDutyValue(dutyCycle10);     //on --> 10%
      __delay_ms(400);
      PWM3_LoadDutyValue(dutyCycle25);     //on --> 25%
      __delay_ms(400);
      PWM3_LoadDutyValue(dutyCycle50);     //on --> 50%
       __delay_ms(400);
      PWM3_LoadDutyValue(dutyCycle75);     //on --> 75%
       __delay_ms(400);
      PWM3_LoadDutyValue(dutyCycle100);     //on --> 100%
       __delay_ms(400);
    }
    while(PORTCbits.RC2==0){
        PWM3_LoadDutyValue(dutyCycle100);     //Se mantine on --> 100%
    }
    if (PORTCbits.RC2==1){                  // NO se presiona el Boton
      PWM3_LoadDutyValue(dutyCycle100);     //off --> 100%
      __delay_ms(400);
      PWM3_LoadDutyValue(dutyCycle75);     //off --> 75%
      __delay_ms(400);
      PWM3_LoadDutyValue(dutyCycle50);     //off --> 50%
      __delay_ms(400);
      PWM3_LoadDutyValue(dutyCycle25);     //off --> 25%
      __delay_ms(400);
      PWM3_LoadDutyValue(dutyCycle10);     //off --> 10%
      __delay_ms(400);   
      PWM3_LoadDutyValue(my_dutyCycle);     //off --> 0%
      __delay_ms(400);   
    } 
    while(PORTCbits.RC2==1){
        PWM3_LoadDutyValue(my_dutyCycle);     //Se mantine off --> 0%
    }
    } 
}
     