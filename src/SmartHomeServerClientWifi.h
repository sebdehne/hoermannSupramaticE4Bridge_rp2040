#ifndef _SMARTHOME_CLIENT_WIFI_H
#define _SMARTHOME_CLIENT_WIFI_H

#include "logger.h"
#include <WiFiNINA.h>
#include "secrets.h"
#include "config.h"
#include <SPI.h>

enum SmartHomeServerClientWifiState
{
    SmartHomeServer_INIT,
    SmartHomeServer_WIFI_CONNECT,
    SmartHomeServer_WIFI_CONNECTING,
    SmartHomeServer_READING_DATA,
    SmartHomeServer_MESSAGE_RECEIVED,
};

class SmartHomeServerClientWifiClass
{
private:
    WiFiUDP Udp;
    SmartHomeServerClientWifiState currentState = SmartHomeServer_INIT;
    unsigned long currentStateChangedAt = millis();

    unsigned long lastMsgFromServerAt = millis();

public:
    uint8_t receivedMessage[255];
    size_t receivedMessageLengh;

    void scheduleReconnect();
    void run();
    bool hasMessage();
    void markMessageConsumed();
    void send(uint8_t *buf, size_t len);
};

extern SmartHomeServerClientWifiClass SmartHomeServerClientWifi;

#endif