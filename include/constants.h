#ifndef DISHWASHERCONTROLLER_CONSTANTS_H
#define DISHWASHERCONTROLLER_CONSTANTS_H

//============================================================================
// various constants for the application...
enum
{
    BSP_TICKS_PER_SEC = 100, // number of system clock ticks in one second
    TICKS_PER_MINUTE = (60U * BSP_TICKS_PER_SEC),
    TIMEDFILL_TIMEOUT_TICKS = 5U * TICKS_PER_MINUTE,
    WASHCYCLE_TIMEOUT_TICKS = 5U * TICKS_PER_MINUTE,
    RINSECYCLE_TIMEOUT_TICKS = 2U * TICKS_PER_MINUTE,
};

typedef enum
{
    INACTIVE = 0x0,
    ACTIVE = 0x1,
} LogicalState;

typedef enum
{
    RELAY_MOTOR = 0,
    RELAY_HEATER = 1,
    RELAY_FILL = 2,
    RELAY_DETERGENT = 3,
    RELAY_RINSEAID = 4,
    INDICATOR_FILL = 5,
    INDICATOR_RINSE = 6,
    INDICATOR_READY = 7,
} Output;

typedef enum
{
    SWITCH_DOOR = 0,
    SWITCH_FLOAT = 1,
    SWITCH_MANUALRINSE = 2,
    SWITCH_MANUALWASH = 3,
    SWITCH_TIMEDFILL = 4,
    SWITCH_STOP = 5
} Input;

//............................................................................
#endif // DISHWASHERCONTROLLER_CONSTANTS_H
