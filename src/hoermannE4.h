#ifndef _HOERMANN_E4
#define _HOERMANN_E4

#include <Arduino.h>
#include "logger.h"
#include "utils.h"
#include <CoreMutex.h>

enum HoermannE4State
{
    E4_INIT,
    E4_READING
};

enum HoermannE4StateCode
{
    E4_STATE_STOPPED = 0x00,
    
    E4_STATE_OPENING = 0x01,
    E4_STATE_CLOSING = 0x02,
    E4_STATE_HALV_OPENING = 0x05,
    E4_STATE_HALV_VENTING = 0x09,

    E4_STATE_HALV_OPEN = 0x20,
    E4_STATE_HALV_CLOSED = 0x40,
    E4_STATE_HALV_HALV_OPEN = 0x80,    
};

enum HoermannE4Command {
    E4_CMD_NONE = 0,
    E4_CMD_OPEN = 1,
    E4_CMD_CLOSE = 2,
    E4_CMD_TOGGLE = 3,
    E4_CMD_LIGHT = 4,
    E4_CMD_VENT = 5,
    E4_CMD_HALV_OPEN = 6,
};

enum HoermannE4SendingState {
    INIT,
    FIRST_CMD_SENDT,
    SECOND_CMD_SENDT,
    FINISHED,
};

struct HoermannE4Broadcast
{
    uint8_t targetPos;
    uint8_t currentPos;
    HoermannE4StateCode stateCode;
    bool isVented;
    uint8_t motorSpeed;
    bool light;
    bool motorRunning;
    unsigned long receivedAt;
};

class HoermannE4Class
{
private:
    // config
    size_t func23_header_size = 11;
    size_t func16_header_size = 7;
    unsigned long messageSeperatePauseMicros = 2000;
    uint8_t my_bus_id = 2;

    // receiving
    unsigned long lastReadAt = 0;
    uint8_t receiveBuffer[255];
    size_t bytesRead = 0;

    // sending
    uint8_t txBuffer[255];
    size_t txLen = 0;
    HoermannE4Command sendCommand = E4_CMD_CLOSE;
    HoermannE4SendingState sendCommandState = FINISHED;
    unsigned long sendCommandStateChangedAt = millis();
    unsigned long sendCommandDelayMs = 700;
    

    void sendBuf(uint8_t *buf, size_t len);
    bool validateCrc16(size_t len);
    void restartReading();
    void handleMessage();
    void printReceiveBuffer();

    uint8_t byteCount = 0;

    HoermannE4State currentState = E4_INIT;
    unsigned long currentStateChangedAt = millis();

    mutex_t mutex;
    HoermannE4Broadcast lastReceivedBroadcast;
    void handleMessageBroadcast();

public:
    HoermannE4Broadcast currentBroadcast();

    void run();

    bool send(HoermannE4Command cmd);
};

extern HoermannE4Class HoermannE4;



#endif
