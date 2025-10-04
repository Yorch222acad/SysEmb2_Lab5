/* LEEME
	Laboratorio 5 - 1er Fase Proyecto

	Hardware - Pines usados:
	- PJ0: Botón 1 (Señal motor1 - Raspberry)
	- PJ1: Botón 2 (Señal motor2 - Raspberry)
	- PN0: LED indicador botón 1 (motor1)
	- PN1: LED indicador botón 2 (motor2)
	- PN2: Buzzer
	- PF0: LED indicador actividad UART
	- PF4: LED indicador recepción comando UART
	- PA0: UART0 RX (USB)
	- PA1: UART0 TX (USB)

	Descripción:
	Programa para la placa EK-TM4C1294XL que realiza las siguientes funciones:
	- Lee el estado de dos botones integrados en la placa (PJ0 y PJ1). Al presionar PJ0 envía por UART "motor1"
		y enciende el LED PN0. Al presionar PJ1 envía por UART "motor2" y enciende el LED PN1.
	- Escucha comandos por UART. Si recibe "buzzer", enciende el buzzer (PN2) durante 2 segundos.
	- El LED PF4 indica la actividad del UART (se enciende al recibir un comando).
	- El LED PF0 parpadea cada vez que se recibe un comando por UART.
*/

// Librerías:
//-----------------------------{ Mínimas
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
//}----------------------------{ Uart
#include "driverlib/uart.h"
#include "utils/uartstdio.c"
//}----------------------------{ Adicionales
#include "string.h"
#include "driverlib/pin_map.h"
//}--

// ======================================================

// Variables globales:
int freq = 120000000;
volatile uint32_t ui32Loop;

// Prototipos de funciones:
void LecBtn();
void checkUART(char rxBuffer[10], int *rxIndex);
void interactiveDelay(float time_sec, int *tIter);
void toogleUart(bool *ledUart);

// =======================================================

int main(void)
{
	// Variables main:
	//-------------------------{ Uart 
	char rxBuffer[10];
	int rxIndex = 0;
	bool ledUart = true;
	//}------------------------{ InteractiveDelay 
	int tIter = 0;
	float time_sec = 2.0;
	//}--
	bool BuzzerState = false; // Buzzer

	SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ  // Configuración del reloj de la placa
						| SYSCTL_OSC_MAIN
						| SYSCTL_USE_PLL 
						| SYSCTL_CFG_VCO_480),freq); 

	// Habilitar perifericos:
	//---------------------------------------------{ Uart 
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	//}--------------------------------------------{ GPIO 
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	//}--

	// Verificar perifericos:
	//-----------------------------------------------------{ Uart 
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))  {}
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) {}
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))  {}
	//-----------------------------------------------------{ GPIO 
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ))  {}
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))  {}
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))  {}
	//}--

	// Configurar pines 
	//--------------------------------------------------------------{ GPIO Outputs
	GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x07); // PN0,PN1,PN2
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x11);	// PF0,PF4
	//}-------------------------------------------------------------{ GPIO Inputs
	GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, 0x03); // PJ0, PJ1
	GPIOPadConfigSet(GPIO_PORTJ_BASE, 0x03, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	//}-------------------------------------------------------------{ Uart
	GPIOPinConfigure(GPIO_PA0_U0RX); // USB
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, 0x03);
	//}--

	UARTStdioConfig(0, 9600, freq);

	while(1)
	{
		GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0);  // Apaga PN0 y PN1
		LecBtn();
		checkUART(rxBuffer, &rxIndex);
		GPIOPinWrite(GPIO_PORTF_BASE, 0x01, 0);
		// Activar Buzzer al recibir por Uart "buzzer" {=>
		if (strncmp(rxBuffer, "buzzer", 6) == 0) {
			BuzzerState = true;
			memset(rxBuffer, 0, sizeof(rxBuffer));
			GPIOPinWrite(GPIO_PORTN_BASE, 0x04, 0x04);
			toogleUart(&ledUart);
		}
		// <=} Desactivar Buzzer después de 2 segundos {=>
		if (BuzzerState == true) {
			interactiveDelay(time_sec, &tIter);
			if (tIter == 0) {
				BuzzerState = false;
				GPIOPinWrite(GPIO_PORTN_BASE, 0x04, 0);
			}
		}
		// <=}
	}
}

//============================================================================

// Si se presiona el botón, enviar mensaje por Uart: "motor1" o "motor2"
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
// Verificar si hay datos recibidos por Uart y almacenarlos en rxBuffer
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
// Retardo interactivo que permite ejecutar otras tareas
void interactiveDelay(float time_sec, int *tIter){
	int TotalTimeIter = (int)(time_sec*10);
	if (*tIter == 0) {
		*tIter = TotalTimeIter;
	}
	for (ui32Loop = 0; ui32Loop < (freq/100); ui32Loop++) {}
	*tIter -= 1;
}
//------------------------------------------------------------------
// Alternar estado del LED de UART
void toogleUart(bool *ledUart){
	if (*ledUart == true) {
		*ledUart = false;
		GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0);
	} else {
		*ledUart = true;
		GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0x10);
	}
}