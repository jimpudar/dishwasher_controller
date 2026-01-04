#ifndef DISHWASHERCONTROLLER_DEBUG_H
#define DISHWASHERCONTROLLER_DEBUG_H

#include <Arduino.h>

// Define this in platformio.ini or here
// #define DEBUG_ENABLED

#ifdef DEBUG_ENABLED
    #define DEBUG_INIT()        Serial.begin(115200)
    #define DEBUG_PRINT(x)      Serial.print(x)
    #define DEBUG_PRINTLN(x)    Serial.println(x)
#else
    #define DEBUG_INIT()        ((void)0)
    #define DEBUG_PRINT(x)      ((void)0)
    #define DEBUG_PRINTLN(x)    ((void)0)
#endif

#endif // DISHWASHERCONTROLLER_DEBUG_H
