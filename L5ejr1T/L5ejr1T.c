#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "string.h"
#include "driverlib/pin_map.h"
// Uart libraries:
#include "driverlib/uart.h"
#include "utils/uartstdio.c"

int main(void)
{
    int freq = 120000000;
    char msg[] = "hello\n";
    char data[100];

    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),freq); 

    // Habilitar perifericos:
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    
    // Verificar perifericos:
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))  {}
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) {}
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))  {}
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))  {}

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x01); // Enable pin 0 and 1

    // Configurar Uart:
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, 0x03);

    UARTStdioConfig(0, 9600, freq);

    while(1)
    {
        UARTgets(data, 100);
        GPIOPinWrite(GPIO_PORTN_BASE, 0x01, 0x01);
        SysCtlDelay(freq/2);
        GPIOPinWrite(GPIO_PORTN_BASE, 0x01, 0);
        SysCtlDelay(freq/2);
    }
}
