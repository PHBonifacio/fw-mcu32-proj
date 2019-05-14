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
    osc_t oscillocope = {512, 1000, 0, 0, 0};
    read_t adc_read = {0, 0};

    state_machine_t sm =
	{
        .curr_state = WAIT_SW1,
        .last_state = WAIT_SW1
    };

    /*  INIT SYSTICK */
    SysTick_Config(SystemCoreClock / TICK);
    /*  INIT LEDS */
    #if DEBUG && DEBUG_LED
        BSP_RGB_D_Init(0X01, 0X01, 0X01);
    #endif

    BSP_Joystick_Init();
    BSP_Button1_Init();
    BSP_Button2_Init();

    BSP_LCD_Init();
    BSP_LCD_FillScreen(LCD_BLACK);

    BSP_LCD_Drawaxes(LCD_WHITE, LCD_BLACK, "5ms/d", "1V/d", LCD_Green, 0,
                     LCD_BLACK, 1023, 0);
    #if DEBUG && DEBUG_SHOW_READ_VALUE
        BSP_LCD_DrawString(0, 1, "X=    ", LCD_YELLOW, LCD_BLACK);
        BSP_LCD_DrawString(8, 1, "Y=    ", LCD_YELLOW, LCD_BLACK);
        BSP_LCD_DrawString(16, 1, "S=    ", LCD_YELLOW, LCD_BLACK);
    #endif

    //BSP_LCD_DrawString(1, 0, "VOLT", LCD_YELLOW, LCD_BLACK);
    //BSP_LCD_DrawString(7, 0, "PERIOD", LCD_YELLOW, LCD_BLACK);
    //BSP_LCD_DrawString(15, 0, "TRIG", LCD_YELLOW, LCD_BLACK);

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

        switch (sm.curr_state)
        {
            static uint32_t timer = 0;
            static pos_menu_t menu_pos = PRINT_INFO;
            static pos_menu_t config_pos = PRINT_INFO;

            /*  ESPERA ATÉ O SW1 SER PRESSIONADO PARA ENTRAR NO MENU */
            case WAIT_SW1:
                if ((0 != sw1_read.last_read) && (0 == sw1_read.new_read))
                {
                    timer = systick_counter;
                    sm.last_state = sm.curr_state;
                    sm.curr_state = DEBOUNCE_SW1;
                }
            break;

            /*  DEBOUNCE DO SW1, 50MS */
            case DEBOUNCE_SW1:                 
                if (50 < (systick_counter - timer))
                {
                    /*  checa se o botao continua pressionado */
                    if (0 == BSP_Button1_Input())
                    {
                        switch (sm.last_state)
                        {
                            case WAIT_SW1:
                                sm.curr_state = SHOW_MENU_OPTION;
                                menu_pos = PRINT_INFO;
                            break;

                            case SELECT_FUNCTION:
                                sm.curr_state = SELECTED_FUNCTION;
                            break;

                            default:
                            break;
                        }
                        timer = systick_counter;
                    }
                    else
                    {
                        /*  se o botao estiver solto, volta para o estado 
                            anterior */
                        sm.curr_state = sm.last_state;
                    }
                    
                }
            break;

            /*  UTILIZA O EIXO X DO JOYSTICK PARA SELECIONAR A FUNÇÃO QUE SERÁ 
                CONFIGURADA PELO MENU */
            case SELECT_FUNCTION:
                if ((300 > js_read.x) && \
                    (TIME_2_SECONDS > (systick_counter - timer)))
                {
                    if (--menu_pos == ERROR)
                    {
                        menu_pos = POS_TRIG;
                    }
                    sm.last_state = sm.curr_state;
                    sm.curr_state = SHOW_MENU_OPTION;
                    timer = systick_counter;
                }
                else if ((700 < js_read.x) && \
                    (TIME_2_SECONDS > (systick_counter - timer)))
                {
                    if (++menu_pos == PRINT_INFO)
                    {
                        menu_pos = POS_VOLT;
                    }
                    sm.last_state = sm.curr_state;
                    sm.curr_state = SHOW_MENU_OPTION;
                    timer = systick_counter;
                }
                else if ((0 != sw1_read.last_read) && (0 == sw1_read.new_read))
                {
                    sm.last_state = sm.curr_state;
                    sm.curr_state = DEBOUNCE_SW1;
                }
                else if ((0 != sw2_read.last_read) && (0 == sw2_read.new_read))
                {
                    sm.last_state = sm.curr_state;
                    sm.curr_state = DEBOUNCE_SW2;
                }
                else if (TIME_2_SECONDS < (systick_counter - timer))
                {
                    sm.curr_state = RETURN_DEFAULT;
                }
            break;

            case DEBOUNCE_SW2:   
                if (50 < (systick_counter - timer))
                {
                    /*  checa se o botao continua pressionado, se sim retorna ao 
                        menu inicial */
                    if (0 == BSP_Button2_Input())
                    {
                        switch (sm.last_state)
                        {
                            
                            case CHANGE_PERIOD:
                            case CHANGE_VOLT:
                            case CHANGE_TRIGGER:
                                sm.curr_state = SHOW_MENU_OPTION;
                                menu_pos = PRINT_INFO;
                            break;
                        
                            default:
                                sm.curr_state = RETURN_DEFAULT;
                            break;
                        }
                        
                        timer = systick_counter;
                    }
                    else
                    {
                        /* se o botao estiver solto, volta a aguardar */
                        sm.curr_state = sm.last_state;
                    }
                    
                }
            break;

            case SHOW_MENU_OPTION:                
                switch (menu_pos)
                {
                    case POS_VOLT:
                        BSP_LCD_DrawString(0, 0, ">", LCD_RED, LCD_BLACK);
                        BSP_LCD_DrawString(6, 0, " ", LCD_YELLOW, LCD_BLACK);
                        BSP_LCD_DrawString(14, 0, " ", LCD_YELLOW, LCD_BLACK);
                        sm.last_state = sm.curr_state;
                        sm.curr_state = DEBOUNCE_MENU;               
                        timer = systick_counter;                             
                    break;

                    case POS_PERIOD:
                        BSP_LCD_DrawString(0, 0, " ", LCD_YELLOW, LCD_BLACK);
                        BSP_LCD_DrawString(6, 0, ">", LCD_RED, LCD_BLACK);
                        BSP_LCD_DrawString(14, 0, " ", LCD_YELLOW, LCD_BLACK);
                        sm.last_state = sm.curr_state;
                        sm.curr_state = DEBOUNCE_MENU;            
                        timer = systick_counter;                                
                    break;

                    case POS_TRIG:
                        BSP_LCD_DrawString(0, 0, " ", LCD_YELLOW, LCD_BLACK);
                        BSP_LCD_DrawString(6, 0, " ", LCD_YELLOW, LCD_BLACK);
                        BSP_LCD_DrawString(14, 0, ">", LCD_RED, LCD_BLACK);
                        sm.last_state = sm.curr_state;
                        sm.curr_state = DEBOUNCE_MENU;              
                        timer = systick_counter;          
                    break;

                    case PRINT_INFO:
                        BSP_LCD_DrawString(1, 0, "VOLT  PERIOD  TRIG", LCD_YELLOW, \
                            LCD_BLACK);
                        menu_pos = POS_VOLT;
                    break;
                
                    default:
                        sm.curr_state = WAIT_SW1;
                    break;
                }                               

            break;

            case DEBOUNCE_MENU:
                if (250 < (systick_counter - timer))
                {
                    switch (sm.last_state)
                    {
                        case SHOW_MENU_OPTION:
                            sm.curr_state = SELECT_FUNCTION;
                        break;

                        case SELECTED_FUNCTION:
                            switch (menu_pos)
                            {
                                case POS_VOLT:
                                    sm.curr_state = SHOW_VOLT_OPTION;
                                    config_pos = PRINT_INFO;
                                break;

                                case POS_PERIOD:
                                    sm.curr_state = SHOW_PERIOD_OPTION;
                                    config_pos = PRINT_INFO;
                                break;

                                case POS_TRIG:
                                    sm.curr_state = SHOW_TRIGGER_OPTION;
                                    config_pos = PRINT_INFO;
                                break;

                                default:
                                    sm.curr_state = RETURN_DEFAULT;
                                    config_pos = PRINT_INFO;
                                break;
                            }
                        break;

                        case SHOW_PERIOD_OPTION:
                            sm.curr_state = CHANGE_PERIOD;
                            timer = systick_counter;
                        break;

                        case SHOW_VOLT_OPTION:
                            sm.curr_state = CHANGE_VOLT;
                            timer = systick_counter;
                        break;

                        case SHOW_TRIGGER_OPTION:
                            sm.curr_state = CHANGE_TRIGGER;
                            timer = systick_counter;
                        break;

                        default:
                            sm.curr_state = RETURN_DEFAULT;
                        break;
                    }
                    
                }
            break;

            case SELECTED_FUNCTION:
                switch (menu_pos)
                {
                    case 0:
                        BSP_LCD_DrawString(1, 0, "VOLT", LCD_BLACK, LCD_RED);
                        sm.last_state = sm.curr_state;
                        sm.curr_state = DEBOUNCE_MENU;
                        timer = systick_counter;
                    break;

                    case 1:
                        BSP_LCD_DrawString(7, 0, "PERIOD", LCD_BLACK, LCD_RED);
                        sm.last_state = sm.curr_state;
                        sm.curr_state = DEBOUNCE_MENU;
                        timer = systick_counter;
                    break;

                    case 2:
                        BSP_LCD_DrawString(15, 0, "TRIG", LCD_BLACK, LCD_RED);
                        sm.last_state = sm.curr_state;
                        sm.curr_state = DEBOUNCE_MENU;
                        timer = systick_counter;
                    break;

                    default:
                        sm.curr_state = WAIT_SW1;
                    break;
                }
                
                timer = systick_counter;
            break;

            case CHANGE_VOLT:
                if ((300 > js_read.x) && \
                    (TIME_2_SECONDS > (systick_counter - timer)))
                {
                    if (--config_pos == PRINT_INFO)
                    {
                        config_pos = CONFIG_3;
                    }
                    sm.last_state = sm.curr_state;
                    sm.curr_state = SHOW_VOLT_OPTION;
                    timer = systick_counter;
                }
                else if ((700 < js_read.x) && \
                    (TIME_2_SECONDS > (systick_counter - timer)))
                {
                    if (++config_pos == CONFIG_4)
                    {
                        config_pos = CONFIG_0;
                    }
                    sm.last_state = sm.curr_state;
                    sm.curr_state = SHOW_VOLT_OPTION;
                    timer = systick_counter;
                }
                else if ((0 != sw1_read.last_read) && (0 == sw1_read.new_read))
                {
                    sm.last_state = sm.curr_state;
                    sm.curr_state = DEBOUNCE_SW1;
                }
                if ((0 != sw2_read.last_read) && (0 == sw2_read.new_read))
                {
                    sm.last_state = sm.curr_state;
                    sm.curr_state = DEBOUNCE_SW2;
                }
                else if (TIME_2_SECONDS < (systick_counter - timer))
                {
                    sm.curr_state = RETURN_DEFAULT;
                }
            break;

            case CHANGE_PERIOD:
                if ((0 != sw2_read.last_read) && (0 == sw2_read.new_read))
                {
                    sm.last_state = sm.curr_state;
                    sm.curr_state = DEBOUNCE_SW2;
                }
                else if (TIME_2_SECONDS < (systick_counter - timer))
                {
                    sm.curr_state = RETURN_DEFAULT;
                }
            break;

            case CHANGE_TRIGGER:
                if ((0 != sw2_read.last_read) && (0 == sw2_read.new_read))
                {
                    sm.last_state = sm.curr_state;
                    sm.curr_state = DEBOUNCE_SW2;
                }
                else if (TIME_2_SECONDS < (systick_counter - timer))
                {
                    sm.curr_state = RETURN_DEFAULT;
                }
            break;

            case SHOW_VOLT_OPTION:
                switch (config_pos)
                {
                    case PRINT_INFO:
                        BSP_LCD_DrawString(1, 1, "10mV  0.1V  1V", LCD_YELLOW, \
                            LCD_BLACK);
                        config_pos = CONFIG_0;
                    break;

                    case CONFIG_0:
                        BSP_LCD_DrawString(1, 0, ">", LCD_RED, LCD_BLACK);
                        sm.last_state = sm.curr_state;
                        sm.curr_state = DEBOUNCE_MENU;
                    break;

                    case CONFIG_1:
                        BSP_LCD_DrawString(1, 6, ">", LCD_RED, LCD_BLACK);
                        sm.last_state = sm.curr_state;
                        sm.curr_state = DEBOUNCE_MENU;
                    break;

                    case CONFIG_2:
                        BSP_LCD_DrawString(1, 12, ">", LCD_RED, LCD_BLACK);
                        sm.last_state = sm.curr_state;
                        sm.curr_state = DEBOUNCE_MENU;
                    break;

                    case CONFIG_3:
                        BSP_LCD_DrawString(1, 0, ">", LCD_RED, LCD_BLACK);
                        sm.last_state = sm.curr_state;
                        sm.curr_state = DEBOUNCE_MENU;
                    break;
                
                    default:
                        BSP_LCD_DrawString(1, 0, "                    ", \
                            LCD_YELLOW, LCD_BLACK);
                        sm.curr_state = SHOW_MENU_OPTION;
                        menu_pos = PRINT_INFO;
                    break;
                }
            break;

            case SHOW_PERIOD_OPTION:
                switch (config_pos)
                {
                    case PRINT_INFO:
                        BSP_LCD_DrawString(1, 1, "10ms  0.1S  0.5S  1S", \
                            LCD_YELLOW, LCD_BLACK);
                        config_pos = CONFIG_1;
                    break;

                    case CONFIG_1:
                    break;

                    case CONFIG_2:
                    break;

                    case CONFIG_3:
                    break;

                    case CONFIG_4:
                
                    default:
                        BSP_LCD_DrawString(1, 0, "                    ", \
                            LCD_YELLOW, LCD_BLACK);
                        sm.curr_state = SHOW_MENU_OPTION;
                        menu_pos = PRINT_INFO;
                    break;
                }
            break;

            case SHOW_TRIGGER_OPTION:
                switch (config_pos)
                {
                    case PRINT_INFO:
                        BSP_LCD_DrawString(1, 1, "10mV  0.1V  1V", LCD_YELLOW, \
                            LCD_BLACK);
                        config_pos = CONFIG_1;
                    break;

                    case CONFIG_1:
                    break;

                    case CONFIG_2:
                    break;

                    case CONFIG_3:
                    break;

                    case CONFIG_4:
                
                    default:
                        BSP_LCD_DrawString(1, 0, "                    ", \
                            LCD_YELLOW, LCD_BLACK);
                        sm.curr_state = SHOW_MENU_OPTION;
                        menu_pos = PRINT_INFO;
                    break;
                }
            break;

            case RETURN_DEFAULT:
                BSP_LCD_DrawString(0, 0, "                     ", LCD_YELLOW, \
                    LCD_BLACK);
                timer = 0;
                sm.last_state = WAIT_SW1;
                sm.curr_state = WAIT_SW1;
            break;
        }

        if (0 == (systick_counter % (oscillocope.timer / 10)))
        {
            adc_read.last_read = adc_read.new_read;
            adc_read.new_read = getFunctionPoint(sinFunction, \
                0.1); 

            if ((0 != oscillocope.nxt_point) && (0 != oscillocope.status))
            {
                BSP_LCD_Plot_VLine(adc_read.last_read, adc_read.new_read, LCD_CYAN);

                BSP_LCD_Plot_VLine(oscillocope.trigger - 1, oscillocope.trigger, \
                    LCD_Gray25);

                if (0 == (oscillocope.nxt_point % 10))
                {
                    for (uint16_t i = 1; i <= 10; i ++)
                    {
                        BSP_LCD_Plot_VLine((100 * i) - 1, 100 * i, LCD_YELLOW);
                    }
                }

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
