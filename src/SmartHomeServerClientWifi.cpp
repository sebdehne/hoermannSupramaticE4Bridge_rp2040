#include "SmartHomeServerClientWifi.h"
#include "hoermannE4.h"
#include "utils.h"

void SmartHomeServerClientWifiClass::scheduleReconnect()
{
    currentState = SmartHomeServer_WIFI_CONNECT;
    currentStateChangedAt = millis();
}

bool SmartHomeServerClientWifiClass::hasMessage()
{
    return currentState == SmartHomeServer_MESSAGE_RECEIVED;
}

void SmartHomeServerClientWifiClass::markMessageConsumed()
{
    currentState = SmartHomeServer_READING_DATA;
    currentStateChangedAt = millis();
}


void SmartHomeServerClientWifiClass::run()
{
    if (millis() - lastMsgFromServerAt > WIFI_RESET_IF_NO_MSG_FROM_SERVER_FOR)
    {
        Log.log("No msg from server, resetting wifi");
        currentState = SmartHomeServer_WIFI_CONNECT;
        currentStateChangedAt = millis();
        lastMsgFromServerAt = millis();
    }

    switch (currentState)
    {
    case SmartHomeServer_INIT:
    {

        // pinMode(NINA_RESETN, OUTPUT);
        // digitalWrite(NINA_RESETN, 1);
        // delay(100);
        // digitalWrite(NINA_RESETN, 0);
        // delay(100);

        if (WiFi.status() == WL_NO_MODULE)
        {
            Log.log("No wifi module detected");
            delay(10000);
            return;
        }

        static const char *expectedFirmeware = WIFI_FIRMWARE_LATEST_VERSION;
        static const char *actualFirmware = WiFi.firmwareVersion();

        if (strcmp(expectedFirmeware, actualFirmware) != 0)
        {
            char buf[1024];
            snprintf(buf, sizeof(buf), "Firmware upgrade needed: expected: %s, actual: %s", expectedFirmeware, actualFirmware);
            Log.log(buf);
            delay(10000);
            return;
        }

        currentState = SmartHomeServer_WIFI_CONNECT;
        currentStateChangedAt = millis();

        break;
    }
    case SmartHomeServer_WIFI_CONNECT:
    {
        WiFi.disconnect();
        currentState = SmartHomeServer_WIFI_CONNECTING;
        currentStateChangedAt = millis();
        break;
    }
    case SmartHomeServer_WIFI_CONNECTING:
        if (WiFi.begin(WIFI_SSID, WIFI_PASS) == WL_CONNECTED)
        {
            Log.log("Wifi Connected");

            // WiFi.localIP() hangs
            // IPAddress localIP = WiFi.localIP();
            // char buf[255];
            // sniprintf(buf, sizeof(buf), "SmartHomeServerClientWifi - wifi connected. Listening on: %u.%u.%u.%u:%u", localIP[0], localIP[1], localIP[2], localIP[3], LOCAL_UDP_PORT);
            // Log.log(buf);

            if (Udp.begin(LOCAL_UDP_PORT))
            {
                Log.log("Listening on UDP");
                currentState = SmartHomeServer_READING_DATA;
                currentStateChangedAt = millis();
            }
            else
            {
                Log.log("Udp.begin() - error");
            }
        }

        break;
    case SmartHomeServer_MESSAGE_RECEIVED:
        // 00: HoermannE4Command
        if (receivedMessageLengh == 1)
        {
            Log.log("Handling UDP request");
            HoermannE4Command cmd = (HoermannE4Command)receivedMessage[0];
            bool sendResult = false;
            if (cmd != E4_CMD_NONE)
            {
                sendResult = HoermannE4.send(cmd);
            }

            HoermannE4Broadcast lastReceivedBroadcast = HoermannE4.currentBroadcast();

            // send broadcast back as ACK
            uint8_t buf[12];
            size_t i = 0;
            buf[i++] = sendResult;
            buf[i++] = lastReceivedBroadcast.targetPos;
            buf[i++] = lastReceivedBroadcast.currentPos;
            buf[i++] = lastReceivedBroadcast.stateCode;
            buf[i++] = lastReceivedBroadcast.isVented;
            buf[i++] = lastReceivedBroadcast.motorSpeed;
            buf[i++] = lastReceivedBroadcast.light;
            buf[i++] = lastReceivedBroadcast.motorRunning;
            unsigned long delta = millis() - lastReceivedBroadcast.receivedAt;
            writeUint32(delta, buf, i++);
            i++;
            i++;
            i++;

            // send it
            Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
            Udp.write(buf, sizeof(buf));
            Udp.endPacket();

            lastMsgFromServerAt = millis();
        }
        else
        {
            Log.log("Ignoring UDP msg");
        }

        markMessageConsumed();

        break;
    case SmartHomeServer_READING_DATA:
        if (Udp.parsePacket())
        {
            IPAddress remoteIp = Udp.remoteIP();
            uint16_t remotePort = Udp.remotePort();
            char buf[255];
            sniprintf(buf, sizeof(buf), "Received msg from: %u.%u.%u.%u:%u", remoteIp[0], remoteIp[1], remoteIp[2], remoteIp[3], remotePort);
            Log.log(buf);

            receivedMessageLengh = Udp.read(receivedMessage, sizeof(receivedMessage));
            currentState = SmartHomeServer_MESSAGE_RECEIVED;
            currentStateChangedAt = millis();
        }
        break;

    default:
        break;
    }
}


SmartHomeServerClientWifiClass SmartHomeServerClientWifi;
