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
//-------------------------------
#define MaxBuffer 10

void LecBtn(int freq) {
  volatile uint32_t ui32Loop;
  if ((GPIOPinRead(GPIO_PORTJ_BASE, 0x01) == 0)) {
    UARTprintf("motor1\n");
    GPIOPinWrite(GPIO_PORTN_BASE, 0x01, 0x01);
    for (ui32Loop = 0; ui32Loop < (freq/60); ui32Loop++) {}
  }
  if ((GPIOPinRead(GPIO_PORTJ_BASE, 0x02) == 0)) {
    UARTprintf("motor2\n");
    GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0x02);
    for (ui32Loop = 0; ui32Loop < (freq/60); ui32Loop++) {}
  }
}

void checkUART(char rxBuffer[MaxBuffer], int freq) {
  int rxIndex = 0;
  int c;
  volatile uint32_t ui32Loop;
  // Mientras haya caracteres en el buffer del UART
  while (UARTCharsAvail(UART0_BASE)) {
    // Leer carácter (no bloquea)
    c = UARTCharGetNonBlocking(UART0_BASE);

    // Seguridad: -1 significa "no hay nada" (aunque no debería pasar dentro del while)
    if (c == -1) return;

    // Detecta fin de línea
    if (c == '\r' || c == '\n') {
      rxBuffer[rxIndex] = '\0';  // Cierra string
      rxIndex = 0;               // Reinicia buffer
      GPIOPinWrite(GPIO_PORTF_BASE, 0x01, 0x01);
      for (ui32Loop = 0; ui32Loop < (freq/60); ui32Loop++) {}
    }
    else {
      // Evitar overflow de buffer
      if (rxIndex < (MaxBuffer - 1)) {
        rxBuffer[rxIndex++] = (char)c;
      }
    }
  }
}

int main(void)
{
    int freq = 120000000;
    char rxBuffer[MaxBuffer];
    bool ledUart = true;

    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),freq); 

    // Habilitar perifericos:
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    //-------------------------------------------
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    
    // Verificar perifericos:
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))  {}
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) {}
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))  {}
    //-----------------------------------------------------
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ))  {}
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))  {}
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))  {}


    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x03);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x03);
    //--------------------------------------------------------------
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, 0x03);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, 0x03, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    //--------------------------------------------------------------
    // Configurar Uart:
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, 0x03);

    UARTStdioConfig(0, 9600, freq);

    while(1)
    {
      LecBtn(freq);
      GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0);  // Apaga PN0 y PN1
      checkUART(rxBuffer, freq);
      GPIOPinWrite(GPIO_PORTF_BASE, 0x01, 0);
      if (strcmp(rxBuffer, "buzzer") == 0) {
        if (ledUart == true) {
          ledUart = false;
          GPIOPinWrite(GPIO_PORTF_BASE, 0x02, 0);
        } else {
          ledUart = true;
          GPIOPinWrite(GPIO_PORTF_BASE, 0x02, 0x02);
        }
      }
    }
}