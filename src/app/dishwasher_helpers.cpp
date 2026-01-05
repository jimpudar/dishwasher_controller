#include "dishwasher_helpers.h"

#include "bsp.h"
#include "constants.h"
#include "debug.h"
#include "signals.h"

void Dishwasher_startup(QActive *me)
{
    if (Dishwasher_isManualSwitchEngaged())
    {
        DEBUG_PRINTLN("startup with manual engaged");
        QACTIVE_POST(me, STARTUP_FAULT_SIG, 0U);
        return;
    }

    // if the RTD loop is open, we emit RTD_FAULT_SIG from BSP
    if (Dishwasher_waterInTankIsOverSafeTemperature())
    {
        DEBUG_PRINTLN("startup over temp");
        QACTIVE_POST(me, OVER_TEMP_FAULT_SIG, 0U);
        return;
    }

    QACTIVE_POST(me, STARTUP_SIG, 0U);
}

void Dishwasher_handleDoorOpening()
{
    BSP_setOutputLogicalState(RELAY_MOTOR, INACTIVE);
    BSP_setOutputLogicalState(RELAY_FILL, INACTIVE);
    BSP_setOutputLogicalState(RELAY_DETERGENT, INACTIVE);
    BSP_setOutputLogicalState(RELAY_RINSEAID, INACTIVE);
}

bool Dishwasher_isReadyToWash()
{
    if (Dishwasher_isManualSwitchEngaged())
        return false;

    if (!Dishwasher_waterInTankIsAtWashTemperature())
        return false;

    if (!BSP_isFloatClosed())
        return false;

    if (!BSP_isDoorClosed())
        return false;

    return true;
}

bool Dishwasher_isManualSwitchEngaged() { return BSP_isManualRinseClosed() || BSP_isManualWashClosed(); }

bool Dishwasher_waterInTankIsAtWashTemperature()
{
    const int16_t temp = BSP_readTemperature();

    return temp > 65 && temp < 88;
}

bool Dishwasher_waterInTankIsOverSafeTemperature()
{
    const int16_t temp = BSP_readTemperature();

    return temp > 90;
}

void Dishwasher_turnOnPumpMotor()
{
    if (BSP_isFloatClosed())
    {
        BSP_setOutputLogicalState(RELAY_MOTOR, ACTIVE);
    }
}
void Dishwasher_turnOffPumpMotor() { BSP_setOutputLogicalState(RELAY_MOTOR, INACTIVE); }

void Dishwasher_turnOnRinseValve() { BSP_setOutputLogicalState(RELAY_FILL, ACTIVE); }
void Dishwasher_turnOffRinseValve() { BSP_setOutputLogicalState(RELAY_FILL, INACTIVE); }

void Dishwasher_startTimedFill()
{
    BSP_setOutputLogicalState(INDICATOR_FILL, ACTIVE);
    BSP_setOutputLogicalState(RELAY_FILL, ACTIVE);
}

void Dishwasher_stopTimedFill()
{
    BSP_setOutputLogicalState(INDICATOR_FILL, INACTIVE);
    BSP_setOutputLogicalState(RELAY_FILL, INACTIVE);
}

void Dishwasher_startWashCycle()
{
    BSP_setOutputLogicalState(RELAY_MOTOR, ACTIVE);
    BSP_setOutputLogicalState(RELAY_DETERGENT, ACTIVE);
}

void Dishwasher_stopWashCycle()
{
    BSP_setOutputLogicalState(RELAY_MOTOR, INACTIVE);
    BSP_setOutputLogicalState(RELAY_DETERGENT, INACTIVE);
}

void Dishwasher_startRinseCycle()
{
    BSP_setOutputLogicalState(INDICATOR_RINSE, ACTIVE);
    BSP_setOutputLogicalState(RELAY_FILL, ACTIVE);
    BSP_setOutputLogicalState(RELAY_RINSEAID, ACTIVE);
}

void Dishwasher_stopRinseCycle()
{
    BSP_setOutputLogicalState(INDICATOR_RINSE, INACTIVE);
    BSP_setOutputLogicalState(RELAY_FILL, INACTIVE);
    BSP_setOutputLogicalState(RELAY_RINSEAID, INACTIVE);
}

void Dishwasher_enterFaultMode()
{
    BSP_setOutputLogicalState(RELAY_MOTOR, INACTIVE);
    BSP_setOutputLogicalState(RELAY_FILL, INACTIVE);
    BSP_setOutputLogicalState(RELAY_DETERGENT, INACTIVE);
    BSP_setOutputLogicalState(RELAY_RINSEAID, INACTIVE);
    BSP_setOutputLogicalState(RELAY_HEATER, INACTIVE);

    while (1) {
        BSP_setOutputLogicalState(INDICATOR_FILL, ACTIVE);
        BSP_setOutputLogicalState(INDICATOR_READY, ACTIVE);
        BSP_setOutputLogicalState(INDICATOR_RINSE, ACTIVE);
        _delay_ms(250);
        BSP_setOutputLogicalState(INDICATOR_FILL, INACTIVE);
        BSP_setOutputLogicalState(INDICATOR_READY, INACTIVE);
        BSP_setOutputLogicalState(INDICATOR_RINSE, INACTIVE);
        _delay_ms(250);
    }
}