
#include "logger.h"

Logger::Logger() {}

void Logger::log(const char *input)
{
#ifdef DEBUG

    size_t bufLen = strlen(input) + 12 + 1;
    char buf[bufLen];
    snprintf(buf, bufLen, "%lu: %s", millis(), input);

    Serial.println(buf);
#endif
}

bool Logger::isDebug() {
    return false;
    //return debug_condition_1 && debug_condition_2;
}

Logger Log;
