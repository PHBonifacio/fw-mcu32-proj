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
    osc_t oscillocope = {512, 100, 0, 0, 0};
    read_t adc_read = {0, 0};
    state_machine_t state_machine = WAIT_SW1;

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
    BSP_LCD_DrawString(15, 0, "TRIG", LCD_YELLOW, LCD_BLACK);

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

        BSP_Joystick_Input(&js_read.x, &js_read.y, &js_read.select);
        sw1_read.last_read = sw1_read.new_read;
        sw1_read.new_read = BSP_Button1_Input();
        sw2_read.last_read = sw2_read.new_read;
        sw2_read.new_read = BSP_Button2_Input();

        switch (state_machine)
        {
            static uint32_t timer = 0;
            static uint8_t menu_pos = 0;

            case WAIT_SW1:
                if ((0 != sw1_read.last_read) && (0 == sw1_read.new_read))
                {
                    timer = systick_counter;
                    state_machine = DEBOUNCE_SW1_0;
                }
            break;

            case DEBOUNCE_SW1_0: /*  debounce timer */
                
                if (50 < (systick_counter - timer))
                {
                    /*  checa se o botao continua pressionado */
                    if (0 == BSP_Button1_Input())
                    {
                        state_machine = SHOW_MENU_OPTION;
                        timer = systick_counter;
                    }
                    else
                    {
                        /* se o botao estiver solto, volta a aguardar */
                        state_machine = WAIT_SW1;
                    }
                    
                }
            break;

            case SELECT_FUNCTION:
                if ((300 > js_read.x) && (1000 > (systick_counter - timer)))
                {
                    if (--menu_pos == 0xFF)
                    {
                        menu_pos = 2;
                    }
                    state_machine = SHOW_MENU_OPTION;
                    timer = systick_counter;
                }
                else if ((700 < js_read.x) && (1000 > (systick_counter - timer)))
                {
                    if (++menu_pos == 3)
                    {
                        menu_pos = 0;
                    }
                    state_machine = SHOW_MENU_OPTION;
                    timer = systick_counter;
                }
                if ((0 != sw1_read.last_read) && (0 == sw1_read.new_read))
                {
                    state_machine = DEBOUNCE_SW1_1;
                }
                else if (1000 < (systick_counter - timer))
                {
                    state_machine = WAIT_SW1;
                }


            break;

            case DEBOUNCE_SW2:
   
                if (50 < (systick_counter - timer))
                {
                    /*  checa se o botao continua pressionado */
                    if (0 == BSP_Button2_Input())
                    {
                        state_machine = WAIT_SW1;
                        timer = systick_counter;
                    }
                    else
                    {
                        /* se o botao estiver solto, volta a aguardar */
                        state_machine = SHOW_MENU_OPTION;
                    }
                    
                }
            break;

            case SHOW_MENU_OPTION:                
                switch (menu_pos)
                {
                    case 0:
                        BSP_LCD_DrawString(1, 0, "VOLT", LCD_BLACK, LCD_Gray25);
                        BSP_LCD_DrawString(7, 0, "PERIOD", LCD_YELLOW, LCD_BLACK);
                        BSP_LCD_DrawString(15, 0, "TRIG", LCD_YELLOW, LCD_BLACK);
                        state_machine = DEBOUNCE_MENU;               
                        timer = systick_counter;                             
                    break;

                    case 1:
                        BSP_LCD_DrawString(1, 0, "VOLT", LCD_YELLOW, LCD_BLACK);
                        BSP_LCD_DrawString(7, 0, "PERIOD", LCD_BLACK, LCD_Gray25);
                        BSP_LCD_DrawString(15, 0, "TRIG", LCD_YELLOW, LCD_BLACK);
                        state_machine = DEBOUNCE_MENU;            
                        timer = systick_counter;                                
                    break;

                    case 2:
                        BSP_LCD_DrawString(1, 0, "VOLT", LCD_YELLOW, LCD_BLACK);
                        BSP_LCD_DrawString(7, 0, "PERIOD", LCD_YELLOW, LCD_BLACK);
                        BSP_LCD_DrawString(15, 0, "TRIG", LCD_BLACK, LCD_Gray25);
                        state_machine = DEBOUNCE_MENU;              
                        timer = systick_counter;          
                    break;
                
                    default:
                        state_machine = WAIT_SW1;
                    break;
                }                               

            break;

            case DEBOUNCE_MENU:
                if (250 < (systick_counter - timer))
                {
                    state_machine = SELECT_FUNCTION;
                }
            break;

            case DEBOUNCE_SW1_1:
                if (50 < (systick_counter - timer))
                {
                    /*  checa se o botao continua pressionado */
                    if (0 == BSP_Button1_Input())
                    {
                        switch (menu_pos)
                        {
                            case 0:
                                BSP_LCD_DrawString(1, 0, "VOLT", LCD_BLACK, LCD_RED);
                                state_machine = DEBOUNCE_MENU;               
                                timer = systick_counter;                             
                            break;

                            case 1:
                                BSP_LCD_DrawString(7, 0, "PERIOD", LCD_BLACK, LCD_RED);
                                state_machine = DEBOUNCE_MENU;            
                                timer = systick_counter;                                
                            break;

                            case 2:
                                BSP_LCD_DrawString(15, 0, "TRIG", LCD_BLACK, LCD_RED);
                                state_machine = DEBOUNCE_MENU;              
                                timer = systick_counter;          
                            break;
                        
                            default:
                                state_machine = WAIT_SW1;
                            break;
                        }                
                        state_machine = SHOW_MENU_OPTION;
                        timer = systick_counter;
                    }
                    else
                    {
                        /* se o botao estiver solto, volta a aguardar */
                        state_machine = SHOW_MENU_OPTION;
                    }
                    
                }
            break;
                   
            default:
            break;
        }

        if (0 == (systick_counter % (oscillocope.timer / 10)))
        {
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
        }
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
