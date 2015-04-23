#include "system_config.h"
#include "peripheral/ports/plib_ports.h"


void SYS_PORTS_Initialize(void)
{
    /* AN and CN Pins Initialization */
    PLIB_PORTS_AnPinsModeSelect(PORTS_ID_0, SYS_PORT_AD1PCFG, PORTS_PIN_MODE_DIGITAL);
    PLIB_PORTS_CnPinsPullUpEnable(PORTS_ID_0, SYS_PORT_CNPUE);
    PLIB_PORTS_CnPinsEnable(PORTS_ID_0, SYS_PORT_CNEN);
    PLIB_PORTS_ChangeNoticeEnable(PORTS_ID_0);
    
    
    /* PORT D Initialization */
    ANSELBbits.ANSB13 = 0; //Make RB13 digital
     //Use digital ouput pin to power OLED --> this calibrates screen starting pos
     //each time the pic is reset
//    ANSELBbits.ANSB2 = 0; //B2 (Pin 6) as digital pin (OLED Power)
    PLIB_PORTS_DirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_B, 0x80); //B7:LED, B2: OLED Power
//    PLIB_PORTS_PinSet (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2); //B2 high to power OLED
    PLIB_PORTS_OpenDrainEnable(PORTS_ID_0, PORT_CHANNEL_B, SYS_PORT_D_ODC);
    
}


/*******************************************************************************
 End of File
*/
