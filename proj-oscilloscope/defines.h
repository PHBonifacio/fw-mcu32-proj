#ifndef __DEFINES_H__
#define __DEFINES_H__

#ifndef rvmdk
#define rvmdk
#endif

#ifndef PART_TM4C123GH6PM
#define PART_TM4C123GH6PM
#endif

#ifndef TARGET_IS_TM4C123_RB1
#define TARGET_IS_TM4C123_RB1
#endif

#define DEBUG                   1
#define DEBUG_SHOW_READ_VALUE   0
#define DEBUG_LED               0

#define TICK                    1000UL
#define TIME_1_MSEC             1UL
#define TIME_1_SECOND           (1000 * TIME_1_MSEC)
#define TIME_2_SECONDS          (2 * TIME_1_SECOND)
typedef struct 
{
    uint16_t x;
    uint16_t y;
    uint8_t select;
} joystick_read_t;

typedef struct 
{
    uint16_t last_read;
    uint16_t new_read;
} read_t;

typedef struct 
{
    uint16_t trigger;
    uint16_t timer;
    uint16_t volt;
    uint8_t status;
    uint8_t nxt_point;
} osc_t;

typedef enum
{
    WAIT_SW1 = 0,
    DEBOUNCE_SW1,
    SELECT_FUNCTION,
    DEBOUNCE_SW2,
    SHOW_MENU_OPTION,
    DEBOUNCE_MENU,
    SELECTED_FUNCTION,
    CHANGE_VOLT,
    CHANGE_PERIOD,
    CHANGE_TRIGGER,
    SHOW_VOLT_OPTION,
    SHOW_PERIOD_OPTION,
    SHOW_TRIGGER_OPTION,
    RETURN_DEFAULT
} status_sm_t;

typedef enum
{
    POS_VOLT = 0,
    POS_PERIOD,
    POS_TRIG,
    PRINT_INFO,
    CONFIG_0,
    CONFIG_1,
    CONFIG_2,
    CONFIG_3,
    CONFIG_4,
    ERROR = 0xFF
} pos_menu_t;

typedef struct
{
    status_sm_t last_state;
    status_sm_t curr_state;
} state_machine_t;
#endif
