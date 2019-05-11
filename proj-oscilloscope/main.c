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
#include "functions.h"

static uint32_t systick_counter = 0;

int main()
{
    joystick_read_t js_read = {0, 0, 0};
    read_t sw1_read = {1, 1};
    read_t sw2_read = {1, 1};
    osc_t oscillocope = {512, 0, 0, 0, 0};
    read_t adc_read = {0, 0};
    /*  INIT SYSTICK */
    SysTick_Config(SystemCoreClock / 1000);
    /*  INIT LEDS */
    #if DEBUG && DEBUG_LED
        BSP_RGB_D_Init(0X01, 0X01, 0X01);
    #endif

    BSP_Joystick_Init();
    BSP_Button1_Init();
    BSP_Button2_Init();

    BSP_LCD_Init();
    BSP_LCD_FillScreen(LCD_BLACK);

    BSP_LCD_Drawaxes(LCD_WHITE, LCD_BLACK, "X", "Y", LCD_Bright_Green, "Y2",
                     LCD_Green, 1023, 0);
    #if DEBUG && DEBUG_SHOW_READ_VALUE
        BSP_LCD_DrawString(0, 1, "X=    ", LCD_YELLOW, LCD_BLACK);
        BSP_LCD_DrawString(8, 1, "Y=    ", LCD_YELLOW, LCD_BLACK);
        BSP_LCD_DrawString(16, 1, "S=    ", LCD_YELLOW, LCD_BLACK);
    #endif

    BSP_LCD_DrawString(1, 0, "VOLT", LCD_YELLOW, LCD_BLACK);
    BSP_LCD_DrawString(7, 0, "PERIOD", LCD_YELLOW, LCD_BLACK);
    BSP_LCD_DrawString(15, 0, "TRIG", LCD_BLACK, LCD_Gray25);

    /*  BSP_LCD_DrawFastHLine(0, 64, 128, LCD_Gray50);*/
    while (1)
    {       
        #if DEBUG && DEBUG_SHOW_READ_VALUE
            BSP_LCD_SetCursor(2, 1);
            BSP_LCD_OutUDec4(js_read.x, LCD_YELLOW);
            BSP_LCD_SetCursor(10, 1);
            BSP_LCD_OutUDec4(js_read.y, LCD_YELLOW);
            BSP_LCD_SetCursor(18, 1);
            BSP_LCD_OutUDec4(js_read.select, LCD_YELLOW);
        #endif

        //BSP_LCD_PlotPoint(js_read.y, LCD_CYAN);
        adc_read.last_read = adc_read.new_read;
        adc_read.new_read = getFunctionPoint(sinFunction, 0.3);

        if ((0 != oscillocope.nxt_point) && (0 != oscillocope.status))
        {
            BSP_LCD_Plot_VLine(adc_read.last_read, adc_read.new_read, LCD_CYAN);

            BSP_LCD_PlotPoint(oscillocope.trigger, LCD_Gray50);

            oscillocope.nxt_point = BSP_LCD_PlotIncrement();

            if (0 == oscillocope.nxt_point)
            {
                oscillocope.status = 0;
            }
        }
        else if ((adc_read.last_read < oscillocope.trigger) && \
            (oscillocope.trigger <= adc_read.new_read) && \
            (0 == oscillocope.status))
        {
            oscillocope.status = 1;
            BSP_LCD_PlotPoint(adc_read.new_read, LCD_CYAN); 

            BSP_LCD_PlotPoint(oscillocope.trigger, LCD_Gray50);

            oscillocope.nxt_point = BSP_LCD_PlotIncrement();
        }
       
        while (0 != (systick_counter % 100));
    }
}

void SysTick_Handler(void)
{
    systick_counter++;

    #if DEBUG && DEBUG_LED
        if (0 == (systick_counter % 500))
        {
            BSP_RGB_D_Toggle(0X01, 0X01, 0X01);
        }
    #endif
}
