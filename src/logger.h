#ifndef _LOGGER_H
#define _LOGGER_H

#include <Arduino.h>
#include "config.h"

class Logger
{
private:
public:
    Logger();
    void log(const char *msg);
    bool isDebug();
};

extern Logger Log;

#endif
