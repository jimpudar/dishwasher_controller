#include "active_objects.h"
#include "bsp.h"
#include "qpn.h"

#include "constants.h"
#include "qm_generated/dishwasher_sm.h"
#include "signals.h"
#include <Arduino.h>

enum
{
    BSP_TICKS_PER_SEC = 100, // number of system clock ticks in one second
    DEBOUNCE_COUNT = 5, // number of ticks an input needs to be stable to count

    NUM_OUTPUTS = 8,
    PIN_OUTPUT_RELAY_MOTOR = 2,
    PIN_OUTPUT_RELAY_HEATER = 3,
    PIN_OUTPUT_RELAY_FILL = 4,
    PIN_OUTPUT_RELAY_DETERGENT = 5,
    PIN_OUTPUT_RELAY_RINSEAID = 6,
    PIN_OUTPUT_INDICATOR_FILL = 7,
    PIN_OUTPUT_INDICATOR_RINSE = 8,
    PIN_OUTPUT_INDICATOR_READY = 9,

    NUM_INPUTS = 6,
    PIN_INPUT_SWITCH_DOOR = 10,
    PIN_INPUT_SWITCH_FLOAT = 11,
    PIN_INPUT_SWITCH_MANUALRINSE = A4,
    PIN_INPUT_SWITCH_MANUALWASH = A5,
    PIN_INPUT_SWITCH_TIMEDFILL = A6,
    PIN_INPUT_SWITCH_STOP = A7,

    PIN_ANALOG_INPUT_420MA_RTD = A0
};

typedef struct
{
    uint8_t pin;

    // Is this active HIGH (non-inverting) or active LOW (inverting)?
    uint8_t activePinState;

    // The default pin state as ACTIVE or INACTIVE
    uint8_t defaultLogicalState;
} OutputPinConfig;

static uint8_t pinLevel(uint8_t activePinState, uint8_t logicalState)
{
    return logicalState == ACTIVE ? activePinState : !activePinState;
}

static const OutputPinConfig outputs[] = {
    {PIN_OUTPUT_RELAY_MOTOR,     HIGH, INACTIVE},
    {PIN_OUTPUT_RELAY_HEATER,    HIGH, INACTIVE},
    {PIN_OUTPUT_RELAY_FILL,      HIGH, INACTIVE},
    {PIN_OUTPUT_RELAY_DETERGENT, HIGH, INACTIVE},
    {PIN_OUTPUT_RELAY_RINSEAID,  HIGH, INACTIVE},
    {PIN_OUTPUT_INDICATOR_FILL,  HIGH, INACTIVE},
    {PIN_OUTPUT_INDICATOR_RINSE, HIGH, INACTIVE},
    {PIN_OUTPUT_INDICATOR_READY, HIGH, INACTIVE},
};

typedef struct
{
    uint8_t pin;

    // What signal should be emitted when the switch moves from low to high
    uint8_t highSignal;

    // What signal should be emitted when the switch moves from high to low
    uint8_t lowSignal;

    // Routing to AOs
    bool routeToDishwasher;
    bool routeToHeater;
} InputPinConfig;

//   PIN                           HIGH SIG              LOW SIG                DW    HT
static const InputPinConfig inputs[] = {
    {PIN_INPUT_SWITCH_DOOR,        DOOR_OPEN_SIG,        DOOR_CLOSE_SIG,        true, false},
    {PIN_INPUT_SWITCH_FLOAT,       FLOAT_OPEN_SIG,       FLOAT_CLOSE_SIG,       true, true },
    {PIN_INPUT_SWITCH_MANUALRINSE, MANUALRINSE_OPEN_SIG, MANUALRINSE_CLOSE_SIG, true, false},
    {PIN_INPUT_SWITCH_MANUALWASH,  MANUALWASH_OPEN_SIG,  MANUALWASH_CLOSE_SIG,  true, false},
    {PIN_INPUT_SWITCH_TIMEDFILL,   TIMEDFILL_OPEN_SIG,   TIMEDFILL_CLOSE_SIG,   true, false},
    {PIN_INPUT_SWITCH_STOP,        STOP_OPEN_SIG,        STOP_CLOSE_SIG,        true, false},
};

void BSP_init(void)
{
    // Initialize the output pins
    for (size_t i = 0; i < NUM_OUTPUTS; i++)
    {
        pinMode(outputs[i].pin, OUTPUT);

        // Set the pin to its default logical state
        digitalWrite(outputs[i].pin, pinLevel(outputs[i].activePinState, outputs[i].defaultLogicalState));
    }

    // Initialize the input pins
    for (size_t i = 0; i < NUM_INPUTS; i++)
    {
        // We have hardware pullups on every input
        pinMode(inputs[i].pin, INPUT);
    }

    // Initialize the 4-20mA input pin
    pinMode(PIN_ANALOG_INPUT_420MA_RTD, INPUT);
}

ISR(TIMER2_COMPA_vect)
{
    QF_tickXISR(0); // process time events for tick rate 0

    // Debounce switch inputs
    static uint8_t stable[NUM_INPUTS] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
    static uint8_t last[NUM_INPUTS] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
    static uint8_t count[NUM_INPUTS] = {0, 0, 0, 0, 0, 0};

    for (uint8_t i = 0; i < NUM_INPUTS; i++)
    {
        uint8_t reading = digitalRead(inputs[i].pin);

        if (reading == last[i])
        {
            if (count[i] < DEBOUNCE_COUNT)
            {
                count[i]++;
            }
            if (count[i] >= DEBOUNCE_COUNT && reading != stable[i])
            {
                stable[i] = reading;

                if (stable[i] == LOW)
                {
                    if (inputs[i].routeToDishwasher) QACTIVE_POST_ISR(AO_Dishwasher, inputs[i].lowSignal, i);
                    //if (inputs[i].routeToHeater) QACTIVE_POST_ISR(AO_Heater, inputs[i].lowSignal, i);
                }
                else
                {
                    if (inputs[i].routeToDishwasher) QACTIVE_POST_ISR(AO_Dishwasher, inputs[i].highSignal, i);
                    //if (inputs[i].routeToHeater) QACTIVE_POST_ISR(AO_Heater, inputs[i].highSignal, i);
                }
            }
        }
        else
        {
            count[i] = 0;
        }
        last[i] = reading;
    }
}

void QF_onStartup(void)
{
    // set Timer2 in CTC mode, 1/1024 prescaler, start the timer ticking...
    TCCR2A = (1U << WGM21) | (0U << WGM20);
    TCCR2B = (1U << CS22) | (1U << CS21) | (1U << CS20); // 1/2^10
    ASSR &= ~(1U << AS2);
    TIMSK2 = (1U << OCIE2A); // enable TIMER2 compare Interrupt
    TCNT2 = 0U;

    // set the output-compare register based on the desired tick frequency
    OCR2A = (F_CPU / BSP_TICKS_PER_SEC / 1024U) - 1U;
}

void QV_onIdle(void)
{
    // called with interrupts DISABLED
    // Put the CPU and peripherals to the low-power mode. You might
    // need to customize the clock management for your application,
    // see the datasheet for your particular AVR MCU.
    SMCR = (0 << SM0) | (1 << SE); // idle mode, adjust to your project
    QV_CPU_SLEEP();                // atomically go to sleep and enable interrupts
}

Q_NORETURN Q_onAssert(char const Q_ROM *const module, int location)
{
    // implement the error-handling policy for your application!!!
    (void)module;
    (void)location;
    QF_INT_DISABLE(); // disable all interrupts
    QF_RESET();       // reset the CPU
    for (;;)
    {
    }
}