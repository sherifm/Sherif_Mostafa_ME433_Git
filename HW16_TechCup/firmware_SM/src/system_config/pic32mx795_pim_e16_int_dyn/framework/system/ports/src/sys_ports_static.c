/*******************************************************************************
  SYS PORTS Static Functions for PORTS System Service

  Company:
    Microchip Technology Inc.

  File Name:
    sys_ports_static.c

  Summary:
    SYS PORTS static function implementations for the Ports System Service.

  Description:
    The Ports System Service provides a simple interface to manage the ports
    on Microchip microcontrollers. This file defines the static implementation for the 
    Ports System Service.
    
  Remarks:
    Static functions incorporate all system ports configuration settings as
    determined by the user via the Microchip Harmony Configurator GUI.  It provides 
    static version of the routines, eliminating the need for an object ID or 
    object handle.

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#include "system_config.h"
#include "peripheral/ports/plib_ports.h"



void SYS_PORTS_Initialize(void)
{
//    /* AN and CN Pins Initialization */
//    PLIB_PORTS_AnPinsModeSelect(PORTS_ID_0, SYS_PORT_AD1PCFG, PORTS_PIN_MODE_DIGITAL);
//    PLIB_PORTS_CnPinsPullUpEnable(PORTS_ID_0, SYS_PORT_CNPUE);
//    PLIB_PORTS_CnPinsEnable(PORTS_ID_0, SYS_PORT_CNEN);
//    PLIB_PORTS_ChangeNoticeEnable(PORTS_ID_0);
//
//    /* PORT B Initialization */
//    PLIB_PORTS_DirectionOutputSet( PORTS_ID_0, PORT_CHANNEL_A,  SYS_PORT_A_TRIS ^ 0xFFFF);
//    PLIB_PORTS_OpenDrainEnable(PORTS_ID_0, PORT_CHANNEL_A, SYS_PORT_A_ODC);

    /* AN and CN Pins Initialization */
    PLIB_PORTS_AnPinsModeSelect(PORTS_ID_0, SYS_PORT_AD1PCFG, PORTS_PIN_MODE_DIGITAL);
    PLIB_PORTS_CnPinsPullUpEnable(PORTS_ID_0, SYS_PORT_CNPUE);
    PLIB_PORTS_CnPinsEnable(PORTS_ID_0, SYS_PORT_CNEN);
    PLIB_PORTS_ChangeNoticeEnable(PORTS_ID_0);


    /* PORT B Initialization */
    PLIB_PORTS_DirectionOutputSet( PORTS_ID_0, PORT_CHANNEL_B,0x8080);//Pins B7(LED),B15(LED) as digital outputs
    PLIB_PORTS_OpenDrainEnable(PORTS_ID_0, PORT_CHANNEL_B, 0);

    __builtin_disable_interrupts();
    // set the CP0 CONFIG register to indicate that
    // kseg0 is cacheable (0x3) or uncacheable (0x2)
    // see Chapter 2 "CPU for Devices with M4K Core"
    // of the PIC32 reference manual
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // no cache on this chip!

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to be able to use TDI, TDO, TCK, TMS as digital
    DDPCONbits.JTAGEN = 0;

    // set up USER pin as input
    ANSELBbits.ANSB13 = 0;
    TRISBbits.TRISB13 = 1;

    //Set phase pins
    //   PH1--> B7
    RPB7Rbits.RPB7R = 0b001; //Peripheral requires output pin
    TRISBbits.TRISB7 = 0; //Make it an output
    LATBbits.LATB7 = 1; //Set it high- this initializes as the forward direction

    //   PH2-->B14
    ANSELBbits.ANSB14 = 0; //Make it digital
    RPB14Rbits.RPB14R = 0b001;
    TRISBbits.TRISB14 = 0; //Make it an output
    LATBbits.LATB14 = 1; //Set it high- this initializes as the forward direction

    //Set enable pins
    //   EN1--> B5
    RPB5Rbits.RPB5R = 0b0101; //Set to OC2
    TRISBbits.TRISB5 = 0; //Make it an output

    //   EN2-->B15
    //ANSELBbits.ANSB15 = 0; //Make it digital
    RPB15Rbits.RPB15R = 0b0101; //Set to OC1
    TRISBbits.TRISB14 = 0; //Make it an output

    //Set timers:
    //This is to set up PWM frequency for OC pin- this will be altered using potentiometer
    T2CONbits.TCKPS = 0;     // Timer2 prescaler N=1 (1:4)
    PR2 = 1999;                 // period = (PR2+1) * N * 12.5 ns = 1 ms, 1 kHz
    TMR2 = 0;
    OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
    OC1CONbits.OCTSEL = 0; //Using Timer 2 for OC1
//    OC1RS = 2000;               //duty cycle = OC1RS/(PR2+1) 25% duty cycle to begin with
    OC1R = 2000;                //initialize before turning OC1 on;
    T2CONbits.ON = 1;        // turn on Timer2
    OC1CONbits.ON = 1;       // turn on OC1

    //This is to set up PWM frequency for OC pin- this will be altered using potentiometer
    T3CONbits.TCKPS = 0;     // Timer2 prescaler N=1 (1:4)
    PR3 = 1999;                 // period = (PR2+1) * N * 12.5 ns = 1 ms, 1 kHz
    TMR3 = 0;
    OC2CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
    OC2CONbits.OCTSEL = 1;     //Using Timer3 for OC3
//    OC2RS = 2000;               //duty cycle = OC1RS/(PR2+1)
    OC2R = 2000;                //initialize before turning OC1 on;
    T3CONbits.ON = 1;        // turn on Timer2
    OC2CONbits.ON = 1;       // turn on OC3

    __builtin_enable_interrupts();

}


/*******************************************************************************
 End of File
*/
