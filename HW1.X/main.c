/*
 * File:   main.c
 * Author: Sherif Mostafa
 *
 * Created on April 2, 2015,07:15 AM
 */

#include<xc.h>                      // processor SFR definitions Special Function registers
#include<sys/attribs.h>             // __ISR macro


// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // not boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // Primary Oscillator: High Speed Chrystal
#pragma config OSCIOFNC = OFF // free up secondary osc pins by turning sosc off
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // slowest wdt
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 40MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz:8MHz / 2 = 4MHz
#pragma config FPLLMUL = MUL_20 // multiply clock after FPLLIDIV  4MHz * 20 = 80MHz
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 40MHz: 80MHz / 2 = 40MHz
#pragma config UPLLIDIV = DIV_2 // divide 8MHz input clock, then multiply by 12 to get 48MHz for USB
//HOW DO I MULTIPLY THE usb CLOCK BY 12????????????????????????????????
#pragma config UPLLEN = ON // USB clock on


// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = ON //allow only one reconfiguration
#pragma config IOL1WAY = ON // allow only one reconfiguration
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // controlled by USB module

//Function Prototype
int readADC(void);


// Timer1 ISR function Frequency: 10kHz   Priority: Level 7
void __ISR(_TIMER_1_VECTOR, IPL7SOFT) LEDBrightness()
{
//  This logic could be run in the ewhile(1) loop, but will not guarantee
//  that OC1RS is reset every time, resulting in a less smooth dimmer i.e.
    unsigned int ADCval;
    ADCval = readADC();            // read ADC (0-1024)
    OC1RS =  (40000*ADCval)/1024;   //convert ADV calue to duty cycle
    IFS0bits.T1IF = 0;             // clear interrupt flag
}


int main(void)
{
    //Startup code to run as fast as possible and get pins back from bad defaults
    __builtin_disable_interrupts(); //Disable interrupts for configuration

    // set the CP0 CONFIG register to indicate that
    // kseg0 is cacheable (0x3) or uncacheable (0x2)
    // see Chapter 2 "CPU for Devices with M4K Core"
    // of the PIC32 reference manual
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210582);

    //no cache on this chip!

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;
    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;
    // disable JTAG to be able to use TDI, TDO, TCK, TMS as digital
    DDPCONbits.JTAGEN = 0;

    // Pin B13 as USER Pin
    ANSELBbits.ANSB13 = 0;  // pin B13 as digital
    TRISBbits.TRISB13 = 1;  // pin B13 as input

    // Pin B7 as digital output (LED1)
    TRISBbits.TRISB7 = 0;  // pin B7 as output
    LATBbits.LATB7 = 1;    // LED1 ON

    // Pin B15 as output compare OC1, using Timer2 (1kHz)
    RPB15Rbits.RPB15R = 0b0101; // RB15 as OC1 (see Ouput Pin Selection TAble 11-2)
    T2CONbits.TCKPS = 0;   // Prescaler N=1
    PR2 = 39999;  // period = (PR2+1) * N * 25ns = 1ms, 10kHz --> period = 1ms
    TMR2 = 0; // initialize timer to 0
    OC1CONbits.OCM = 0b110; // PWM mode without fault pin;
    OC1RS = 10000; // 25% duty cycle (PR2+1)/2 = 10000
    OC1R = 20000; // initialize OC1 on; larter read-only
    T2CONbits.ON = 1; // turn on Timer2
    OC1CONbits.ON = 1; // turn on OC1

    // set up A0 as AN0
    ANSELAbits.ANSA0 = 1;
    AD1CON3bits.ADCS = 3;
    AD1CHSbits.CH0SA = 0;
    AD1CON1bits.ADON = 1;

    // Set up timer1 for ISR
    T1CONbits.TCKPS = 0b01;         // Prescaler N=8
//    T1CONbits.TGATE = 0;            //(default)
//    T1CONbits.TCS = 0;              //(default)
    PR1 = 499;	// period = (PR1+1) * N * 25ns = 0.1ms, 10 kHz
    TMR1 = 0;                       // initialize timer to 0
    T1CONbits.ON = 1;               // turn on Timer1
    // Set Interrupt for Timer1
    IPC1bits.T1IP = 7;              // INT step 4: priority 7
    IPC1bits.T1IS = 0;              //             subpriority 0
    IFS0bits.T1IF = 0;              // INT step 5: clear interrupt flag
    IEC0bits.T1IE = 1;              // INT step 6: enable interrupt

    __builtin_enable_interrupts(); //re enable interrupts after configuration is complete


    // Main while loop
    while (1)
    {
        _CP0_SET_COUNT(0); // Reset core counter
        while(_CP0_GET_COUNT() < 10000000) //The CP0 timer runs at 20kHZ (Half the core frequency)
		{ if (PORTBbits.RB13 == 0){
                    break; } } //Skip the wait if USER Pin B13 is pushed

        if (PORTBbits.RB13 == 1){ LATBINV = 0x0080; } // toggle LED1
        else{ LATBbits.LATB7 = 1; } // LED1 ON

//*****This logic is better in an an ISR to guarantee a smoother dimmer*****//
//        unsigned int ADCval;
//        ADCval = readADC();            // read ADC (0-1024)
//        OC1RS =  40000* ADCval/1024;   //convert ADV calue to duty cycle
    }
}

// ADC value reading
int readADC(void)
{
    int elapsed = 0;
    int finishtime = 0;
    int sampletime = 20;
    int a = 0;

    AD1CON1bits.SAMP = 1;
    elapsed = _CP0_GET_COUNT();
    finishtime = elapsed + sampletime;
    while (_CP0_GET_COUNT() < finishtime) {;}
    AD1CON1bits.SAMP = 0;
	while (!AD1CON1bits.DONE) {;}
    a = ADC1BUF0;
    return a;
}