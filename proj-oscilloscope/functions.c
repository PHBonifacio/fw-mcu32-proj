#include <stdbool.h>
#include "functions.h"
#include "TM4C123GH6PM.h"
#include <math.h>

#define SYSDIV2 4

void PLL_Init(void)
{

    //CMSIS
    // 1) Habilitar o divisor fe frequ�ncia SYSDIV
    SYSCTL->RCC |= 0x000400000;
    // 2) Configurar o sistema para usar o RCC2 para recursos avan�ados como
    //    PLL de 400 MHz e divis�o do clock por valores n�o inteiros
    SYSCTL->RCC2 |= 0x80000000; // USERCC2
    // 3) Bypass do PLL enqunato inicializar
    SYSCTL->RCC2 |= 0x00000800; // BYPASS2, PLL bypass
    // 4) Selecionar o valor do cristal e a fonto do oscilador
    SYSCTL->RCC &= ~0x000007C0;  // limpar o campo XTAL
    SYSCTL->RCC += 0x00000540;   // configurar para um cristal de 16 MHz
    SYSCTL->RCC2 &= ~0x00000070; // limpar o campo do oscilador
    SYSCTL->RCC2 += 0x00000000;  // configurar a fonte como main oscillator
    // 5) Ativar o PLL limpando o PWRDN
    SYSCTL->RCC2 &= ~0x00002000;
    // 6) Ajustar o divisor desejado e o LSB do divisor do sistema (bit 22 para 400 MHz de PLL)
    SYSCTL->RCC2 |= 0x40000000;                 // usar 400 MHz de PLL
    SYSCTL->RCC2 = (SYSCTL->RCC2 & ~0x1FC00000) + (SYSDIV2 << 22);           // configurar para clock de 80 MHz
    // 7) Aguardar o PLL come�ar a operar fazendo polling no PLLLRIS (PLL Raw Interrupt Status)
    while ((SYSCTL->RIS & 0x00000040) == 0)
    {
    }; // Aguargar pelo bit PLLLRIS
    // 8) Habilitar o PLL limpando o BYPASS
    SYSCTL->RCC2 &= ~0x00000800;

    SystemCoreClockUpdate(); 
}

void UART0_Init(uint32_t baud)
{
    float aux = 0;
    //1 -  Ativar o clock da UART0
    SYSCTL->RCGCUART |= 0x01;
    //2 -  Ativar o clock do PORTA
    SYSCTL->RCGCGPIO |= 0x01;
    //3 - Desabilitar a UART0
    UART0->CTL = 0x00;
    //4 - Definir a parte inteira do baud rate
    aux = (SystemCoreClock / (16 * ((uint32_t)baud)));

    UART0->IBRD = aux; // IBRD = int(16.000.000 / (16 * 115.200)) = int(8,6805)
    //5 - Definir a parte fracion�ria do baud rate

    baud = baud - ((uint32_t)baud);
    UART0->FBRD = (uint32_t)((baud * 64) + 0.5); // FBRD = int(0,6805 * 64 + 0,5) = 44
    //6 - Formato do pacote (8 bits de dados, sem bit de paridade, um stop bit, FIFOs habilitadas)
    UART0->LCRH = 0x70;
    //7 - Habilitar a UART0
    UART0->CTL = 0x301;
    //8 - Habilitar fun��es alternativas para os pinos PA1-0
    GPIOA->AFSEL |= 0x03;
    //9 - Habilitar fun��o digital nos pinos PA1-0
    GPIOA->DEN |= 0x03;
    //10 - Configurar a fun��o alternativa dos pinos PA1-0 como UART
    GPIOA->PCTL = (GPIOA->PCTL & 0xFFFFFF00) + 0x00000011;
    //11 - Desabilitar fun��o anal�gica dos pinos PA1-0
    GPIOA->AMSEL &= ~0x03;
}

uint32_t sinFunction (float x)
{
	return (sin(x) * 1024/2) + 512;
}

uint32_t squareFunction (float x)
{
	//return (!((int)x%2)) ? 1024:0;
	return (sin(x) > 0) ? 1024:0;
}

uint32_t getFunctionPoint (uint32_t(*func)(float),float dx){
	static float x = 0;
	
	x += dx;
	return func(x);
	
}

