/**
 * @file main.c
 * @author Pedro H. Bonifacio (pedrobonifa@gmail.com)
 * @brief 
 * @version 1.0
 * @date 2019-04-27
 * @license MIT
 * 
 * @copyright Copyright (c) 2019
 * 
 */

/*  INCLUDE BOARD INFO */
#include <TM4C123GH6PM.h>
/*  INCLUDE LIBRARIES */
#include <stdint.h>
#include "defines.h"
#include "BSP.h"
#include "tm4c123gh6pmX.h"

static uint32_t systick_counter = 0;

int main()
{
    /*  INIT SYSTICK */
    SysTick_Config(SystemCoreClock / 1000);

    /*  INIT LEDS */
    BSP_RGB_D_Init(0X01, 0X01, 0X01);
	while(1)
	{
	}
}

void SysTick_Handler(void)
{
    systick_counter++;

    if (0 == (systick_counter % 500))
    {
        BSP_RGB_D_Toggle(0X01, 0X01, 0X01);
    }
}
