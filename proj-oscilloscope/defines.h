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
#define DEBUG_SHOW_READ_VALUE   1
#define DEBUG_LED               0
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
    DEBOUNCE_SW1_0,
    SELECT_FUNCTION,
    DEBOUNCE_SW2,
    SHOW_MENU_OPTION,
    DEBOUNCE_MENU,
    DEBOUNCE_SW1_1,
    CHANGE_VOLT,
    CHANGE_PERIOD,
    CHANGE_TRIGGER
} state_machine_t;

#endif
