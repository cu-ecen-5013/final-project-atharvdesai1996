//*****************************************************************************
//
// uart_echo.c - Example for reading data from and writing data to the UART in
//               an interrupt driven fashion.
//
// Copyright (c) 2013-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 2.1.4.178 of the EK-TM4C1294XL Firmware Package.
//
//*****************************************************************************

//#include "uart_echo.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
//#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/tm4c1294ncpdt.h"
#include "driverlib/uart.h"
#include "driverlib/debug.h"
#include "utils/uartstdio.h"
#include "tmp102.h"
//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>UART Echo (uart_echo)</h1>
//!
//! This example application utilizes the UART to echo text.  The first UART
//! (connected to the USB debug virtual serial port on the evaluation board)
//! will be configured in 115,200 baud, 8-n-1 mode.  All characters received on
//! the UART are transmitted back to the UART.
//
//*****************************************************************************

//****************************************************************************
//
// System clock rate in Hz.
//
//****************************************************************************
uint32_t g_ui32SysClock;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif




void ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 9600, 16000000);
}

void ConfigureUART4(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA2_U4RX);
    ROM_GPIOPinConfigure(GPIO_PA3_U4TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART4_BASE, UART_CLOCK_PIOSC);
    ROM_UARTConfigSetExpClk(UART4_BASE,16000000, 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    //
    // Initialize the UART for console I/O.
    //
    //UARTStdioConfig(0, 115200, 16000000);
   // UARTEnable(UART4_BASE);
}


//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
/*void
UARTIntHandler(void)
{
    uint32_t ui32Status;

    //
    // Get the interrrupt status.
    //
    ui32Status = ROM_UARTIntStatus(UART0_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    ROM_UARTIntClear(UART0_BASE, ui32Status);

    //
    // Loop while there are characters in the receive FIFO.
    //
    while(ROM_UARTCharsAvail(UART0_BASE))
    {
        //
        // Read the next character fROM the UART and write it back to the UART.
        //
        ROM_UARTCharPutNonBlocking(UART0_BASE,
                                   ROM_UARTCharGetNonBlocking(UART0_BASE));

        //
        // Blink the LED to show a character transfer is occuring.
        //
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);

        //
        // Delay for 1 millisecond.  Each SysCtlDelay is about 3 clocks.
        //
        SysCtlDelay(g_ui32SysClock / (1000 * 3));

        //
        // Turn off the LED
        //
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0);
    }
}*/

//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
/*void
UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
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

//*****************************************************************************
//
// This example demonstrates how to send a string of data to the UART.
//
//*****************************************************************************
int
main(void)
{



        //
        // Set the clocking to run at 50 MHz from the PLL.
        //
        SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                           SYSCTL_OSC_MAIN);

        //
        // Initialize the UART and configure it for 115,200, 8-N-1 operation.
        //
        ConfigureUART();
        ConfigureUART4();

        //
                     // Set the clocking to run directly from the crystal at 120MHz.
                     //
                    /* g_ui32SysClock = ROM_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                                              SYSCTL_OSC_MAIN |
                                                              SYSCTL_USE_PLL |
                                                             SYSCTL_CFG_VCO_480), 120000000);*/




    //
    // Enable the GPIO port that is used for the on-board LED.
    //
   // ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    //
    // Enable the GPIO pins for the LED (PN0).
    //
    //ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

    //
    // Enable the peripherals used by this example.
    //
   /* ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);*/

    //
    // Enable processor interrupts.
    //
    //ROM_IntMasterEnable();

    //
    // Set GPIO A0 and A1 as UART pins.
    //
    //GPIOPinConfigure(GPIO_PA0_U0RX);
    //GPIOPinConfigure(GPIO_PA1_U0TX);
    //ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    /*ROM_UARTConfigSetExpClk(UART0_BASE, g_ui32SysClock, 9600,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));*/


    //
    // Enable the UART interrupt.
    //
    //ROM_IntEnable(INT_UART4);
  //  UARTIntRegister(UART4_BASE,uart_tx);
  //  UARTIntEnable(UART4_BASE,UART_INT_TX);
  //  UARTEnable(UART4_BASE);
   // ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    UARTprintf("test 9600");
    temp_init();
 //   while(1)
  //  {
        //UARTSend((uint8_t*)"In the loop",13);
       // UARTprintf("It's working");
       // while(UARTBusy(UART0_BASE));
    ///////////////////////////////////////////////////
    temp_read();
    temp_read();
    char ptr[] = "yy";
    int k=0, l=0;
    for(;;)
    {
    for(k=0; k<sizeof(ptr);k++)
    {
        UARTCharPutNonBlocking(UART4_BASE,ptr[k]);
        while(UARTBusy(UART4_BASE));
        UARTprintf("char sent:: %c\n",*(ptr+k));
       // k++;
    } //while(*(ptr+k) != '\0')
   // UARTprintf("ptr::: %d\n",sizeof(ptr));

    UARTCharPutNonBlocking(UART4_BASE,'\n');
    while(UARTBusy(UART4_BASE));
    }
    //////////////////////////////////////////////////
    int p=0;
    for(p=0; p<15; p++)
    {
        temp_read();
    }
   // }

    //
    // PROMpt for text to be entered.
    //
   // UARTSend((uint8_t *)"\033[2JEnter text: ", 16);

    //
    // Loop forever echoing data through the UART.
    //
   /* while(1)
    {
    }
    */
        return 0;
}



