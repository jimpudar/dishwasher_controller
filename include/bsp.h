#ifndef DISHWASHERCONTROLLER_BSP_H
#define DISHWASHERCONTROLLER_BSP_H
#include "constants.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void BSP_init(void);
    int16_t BSP_readTemperature(void);
    bool BSP_isDoorClosed(void);
    bool BSP_isFloatClosed(void);
    bool BSP_isManualRinseClosed(void);
    bool BSP_isManualWashClosed(void);
    void BSP_setOutputLogicalState(Output outputIdx, LogicalState logicalState);

#ifdef __cplusplus
}
#endif

#endif // DISHWASHERCONTROLLER_BSP_H
