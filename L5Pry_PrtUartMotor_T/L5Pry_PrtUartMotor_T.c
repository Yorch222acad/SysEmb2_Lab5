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

int freq = 120000000;
volatile uint32_t ui32Loop;

void LecBtn();
void checkUART(char rxBuffer[10], int *rxIndex);
void interactiveDelay(float time_sec, int *tIter);

int main(void)
{
    char rxBuffer[10];
    bool ledUart = true;
    int rxIndex = 0;
    int tIter = 0;
    bool BuzzerState = false;
    float time_sec = 2.0;

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


    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x07);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x11);
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
      LecBtn();
      GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0);  // Apaga PN0 y PN1
      checkUART(rxBuffer, &rxIndex);
      GPIOPinWrite(GPIO_PORTF_BASE, 0x01, 0);
      if (strncmp(rxBuffer, "buzzer", 6) == 0) {
        BuzzerState = true;
        memset(rxBuffer, 0, sizeof(rxBuffer));
        GPIOPinWrite(GPIO_PORTN_BASE, 0x04, 0x04);
        if (ledUart == true) {
          ledUart = false;
          GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0);
        } else {
          ledUart = true;
          GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0x10);
        }
      }
      if (BuzzerState == true) {
        interactiveDelay(time_sec, &tIter);
        if (tIter == 0) {
          BuzzerState = false;
          GPIOPinWrite(GPIO_PORTN_BASE, 0x04, 0);
        }
      }
    }
}

//============================================================================

void LecBtn() {
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
//------------------------------------------------------------------
void checkUART(char rxBuffer[10], int *rxIndex) {
  int c;
  while (UARTCharsAvail(UART0_BASE)) {
    c = UARTCharGetNonBlocking(UART0_BASE);

    if (c == -1) return;

    if (c == '\r' || c == '\n') {
      rxBuffer[*rxIndex] = '\0';
      *rxIndex = 0;
      GPIOPinWrite(GPIO_PORTF_BASE, 0x01, 0x01);
      for (ui32Loop = 0; ui32Loop < (freq/60); ui32Loop++) {}
    } else {
      if (*rxIndex < 9) {
        rxBuffer[*rxIndex] = (char)c;
        (*rxIndex)++;
      }
    }
  }
}
//------------------------------------------------------------------
void interactiveDelay(float time_sec, int *tIter){
  int TotalTimeIter = (int)(time_sec*10);
  if (*tIter == 0) {
    *tIter = TotalTimeIter;
  }
  for (ui32Loop = 0; ui32Loop < (freq/100); ui32Loop++) {}
  *tIter -= 1;
}