#ifndef LOGGING_H
#define LOGGING_H

//worst logger, but hey it does the trick... sortof
#ifdef SHOULD_LOG
    #include <iostream>
    #define LOG(msg) std::cout << msg << std::endl
#else
    #define LOG(msg)
#endif


#endif // LOGGING_H