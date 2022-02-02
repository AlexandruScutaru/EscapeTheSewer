#ifndef LOGGER_H
#define LOGGER_H

//fairly bad implementation of a logger, but hey it does the trick... sort of
#ifdef SHOULD_LOG
    #if defined (ARDUINO) || defined (__AVR_ATmega328P__)
        #include <Arduino.h>
        #define LOG(msg) Serial.print(msg)
        #define LOG_LN(msg) Serial.println(msg)
    #else
        #include <iostream>
        #define LOG(msg) std::cout << msg;
        #define LOG_LN(msg) std::cout << msg << std::endl
    #endif
#else
    #define LOG(msg)
    #define LOG_LN(msg)
#endif

#endif LOGGER_H