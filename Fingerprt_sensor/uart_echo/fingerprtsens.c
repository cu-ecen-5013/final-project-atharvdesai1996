/* Author : Atharv Desai
 * Fingerprint sensor C file for GT-521F32 sensor
 * References: 1) https://github.com/sparkfun/Fingerprint_Scanner-TTL/blob/master/src/FPS_GT511C3.h
 *             2) GT-521F52_Programming_guide
 *             3) GT-521FX2_datasheet_V1.1__003_.pdf
 */


#include "fingerprtsens.h"


void Send_cmd_pkt(uint8_t parameter, uint8_t code)
{
    uint8_t x = 0;
    uint8_t command_packet[CMDPKT_SIZE] = { CMD_START_1, CMD_START_2, COMMAND_DEVICE_ID_1, COMMAND_DEVICE_ID_2 ,parameter, 0,0,0, code,0,0,0 };

    uint16_t Checksum_value = Checksum(command_packet);
    command_packet[10] = Checksum_value & 0x00FF;
    command_packet[11] = (Checksum(command_packet) & 0xFF00) >> 8;

    for(x=0; x < CMDPKT_SIZE; x++)
    {
        UARTCharPut(UART3_BASE, command_packet[x]);
    }
    UARTprintf(" Sending packet ");
    for(x=0; x < CMDPKT_SIZE; x++)
       {
           UARTprintf("%X ",command_packet[x]);
           while(UARTBusy(UART3_BASE));
       }
    UARTprintf(" \n \n");
}


uint8_t Send_rsp_pkt(void)
{
    uint8_t x = 0;
    //uint16_t temp=0;
    //uint16_t myresp =0;
    uint8_t response_pkt[12];

    for (x=0; x < 12; x++)
        {
        response_pkt[x] = (uint8_t)(UARTCharGet(UART3_BASE)&0x000000FF);
        }
    UARTprintf(" Received packet ");
    for(x=0; x < 12; x++)
       {
           UARTprintf("%X ",response_pkt[x]);
       }
    UARTprintf(" \n \n");
  return response_pkt[8];
}


void interrupt_handler(void)
{
    uint8_t checker =0;

    IntMasterDisable();  //Disable interrupts to the processor
    UARTprintf("In the interrupt handler\n");
    Send_cmd_pkt(1,CMD_CMOSLED);
    Send_rsp_pkt();
    UARTprintf("Checked above response\n");
    //UARTSend((uint8_t *)"Interrupt\n\r",12);
    if(GPIOIntStatus(ICPCK_PORT,true) & ICPCK_PIN)               //Returns the ICPCK raw interrupt status
    {
        //UARTSend((uint8_t *)"Port-Stat",18);
        GPIOIntClear(ICPCK_PORT, GPIOIntStatus(ICPCK_PORT,false) & ICPCK_PIN) ; //Clears the specified ICPCK interrupt source
        capture_fingerprt();
        Send_rsp_pkt();
        Identify_fingerprt();
        checker = Send_rsp_pkt();
        if(checker==48)    //GT-521F32 can store upto 200 fingerprints
                 {
                     UARTprintf("Fingerprint Matched : %d \n", checker);
                     //UARTSend((uint8_t *)" Fingerprint Matched\n\r",50);

                 }
                 else
                 {
                     UARTprintf("Not Matched  %d \n", checker);
                     //UARTSend((uint8_t *)"Not Matched\n",50);
                 }
      
       

    }
   
    IntMasterEnable();   // Enable the interrupt again
    UARTprintf("\n//////////////\n");
    Send_cmd_pkt(0,CMD_CMOSLED);
    Send_rsp_pkt();
}


uint8_t Identify_fingerprt(void)
{
   // Send_cmd_pkt(0,CMD_CMOSLED);

   // Send_cmd_pkt(1,CMD_CMOSLED);
   //// UARTSend((uint8_t *)"Idn\n\r",9);
   UARTprintf(" Identify_fingerprint\n");
    Send_cmd_pkt(0, CMD_IDENTIFY);  // to check if the acquired fingerprint is stored or not
    //return(Send_rsp_pkt());
    return 0;
}

void interrupt_config(void)      // Reference :https://www.ti.com/lit/ug/spmu298d/spmu298d.pdf Pg 280
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);  //At power-up, all peripherals are disabled which must be enabled

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC))  //Wait for the GPIOC module to be ready.
    {
    }

    GPIOPinTypeGPIOInput(ICPCK_PORT, ICPCK_PIN);  // Setting PC6 as input pin
    // Reference 1 :https://www.ti.com/lit/ug/spmu298d/spmu298d.pdf Pg 260
    // Reference 2 :https://github.com/jajoosiddhant/Two-Factor-Authentication-System
    GPIOPadConfigSet(ICPCK_PORT, ICPCK_PIN, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPD);  // Weak pull down with 8mA

    // Register the port-level interrupt handler which is written above.
    GPIOIntRegister(GPIO_PORTC_BASE, interrupt_handler);
    // Make pin 6 rising edge triggered interrupt
    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_RISING_EDGE);
    // Enable the interrupt
    GPIOIntEnable(ICPCK_PORT, ICPCK_PIN);


}


 void boot_fingerprt(void)
 {
     Send_cmd_pkt(0,CMD_CMOSLED);
     Send_cmd_pkt(1,CMD_CMOSLED);
     Send_cmd_pkt(0,CMD_CMOSLED);
     Send_cmd_pkt(1,CMD_CMOSLED);
 }

 uint8_t capture_fingerprt(void)
 {
     //Send_cmd_pkt(0,CMD_CMOSLED);
     //Send_cmd_pkt(1,CMD_CMOSLED);
     //UARTSend((uint8_t *)"Capture-finger\n\r",25);
    UARTprintf(" \n Capture Fingerprint \n");
     Send_cmd_pkt(0, CMD_CAPTUREFINGER);
     //return(Send_rsp_pkt());
     return 0;
 }

// Referred https://github.com/jajoosiddhant/Two-Factor-Authentication-System basic understanding checksum calculation
uint16_t Checksum(uint8_t cmd_packet[])
{
    uint8_t x=0;
    uint16_t checksumtotal=0;

    while (x<10)         // First 10 packets required of whole 12 size
    {
        checksumtotal = checksumtotal + cmd_packet[x];
        x++;
    }
    return checksumtotal;
}


/*void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    //
    // Loop while there are more characters to send.
    //
    while(ui32Count--)
    {
        //
        // Write the next character to the UART.
        //
        ROM_UARTCharPutNonBlocking(UART0_BASE, *pui8Buffer++);
    }
}*/



