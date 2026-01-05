#ifndef DISHWASHERCONTROLLER_HELPERS_H
#define DISHWASHERCONTROLLER_HELPERS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "qpn.h"

    /**
     * @brief Run the startup checks and emit the initial startup event if no faults were detected
     *
     * @param me Pointer to the Dishwasher QActive object
     */
    void Dishwasher_startup(QActive *me);

    /**
     * @brief Set anything which could cause harm to the operator with the door open to INACTIVE
     */
    void Dishwasher_handleDoorOpening(void);

    /**
     * @brief Check if the Dishwasher is ready to enter a wash cycle
     *
     * @return true if ready to wash, false if not
     */
    bool Dishwasher_isReadyToWash(void);

    /**
     * @return true if either MANUALWASH or MANUALRINSE is engaged
     */
    bool Dishwasher_isManualSwitchEngaged(void);

    /**
     * @return true if the water in the tank is hot enough, but not too hot
     */
    bool Dishwasher_waterInTankIsAtWashTemperature(void);

    /**
     *
     * @return true if the water in the tank is far too hot
     */
    bool Dishwasher_waterInTankIsOverSafeTemperature(void);

    /**
     * @brief Start the Timed Fill cycle. Timeouts are handled in QM, this just handles the outputs.
     */
    void Dishwasher_startTimedFill(void);
    /**
     * @brief Stop the Timed Fill cycle. Timeouts are handled in QM, this just handles the outputs.
     */
    void Dishwasher_stopTimedFill(void);

    /**
     * @brief Start the Wash cycle. Timeouts are handled in QM, this just handles the outputs.
     */
    void Dishwasher_startWashCycle(void);
    /**
     * @brief Stop the Wash cycle. Timeouts are handled in QM, this just handles the outputs.
     */
    void Dishwasher_stopWashCycle(void);

    /**
     * @brief Start the Rinse cycle. Timeouts are handled in QM, this just handles the outputs.
     */
    void Dishwasher_startRinseCycle(void);
    /**
     * @brief Stop the Rinse cycle. Timeouts are handled in QM, this just handles the outputs.
     */
    void Dishwasher_stopRinseCycle(void);

    /**
     * @brief Turn on the pump motor. Used for MANUALWASH.
     */
    void Dishwasher_turnOnPumpMotor(void);
    /**
     * @brief Turn off the pump motor. Used for MANUALWASH.
     */
    void Dishwasher_turnOffPumpMotor(void);

    /**
     * @brief Turn on the rinse valve. Used for MANUALRINSE.
     */
    void Dishwasher_turnOnRinseValve(void);
    /**
     * @brief Turn off the rinse valve. Used for MANUALRINSE.
     */
    void Dishwasher_turnOffRinseValve(void);

    void Dishwasher_enterFaultMode(void);
#ifdef __cplusplus
}
#endif

#endif // DISHWASHERCONTROLLER_HELPERS_H
