#include "hoermannE4.h"
#include "crc16.h"

bool HoermannE4Class::send(HoermannE4Command cmd)
{
    CoreMutex m(&sendMutex);

    if (sendState.sendCommandState == FINISHED && millis() - sendState.sendCommandStateChangedAt > sendState.sendCommandDelayMs)
    {
        sendState.sendCommand = cmd;
        sendState.sendCommandState = INIT;
        sendState.sendCommandStateChangedAt = millis();
        return true;
    }
    else
    {
        return false;
    }
}

bool HoermannE4Class::validateCrc16(size_t len)
{

    uint16_t crc = 0;
    crc = receiveBuffer[len + 1];
    crc <<= 8;
    crc = crc + receiveBuffer[len];

    uint16_t calcCrc16 = crc16(receiveBuffer, len);

    return calcCrc16 == crc;
}

void HoermannE4Class::printReceiveBuffer()
{
    char hexBuf[sizeof(receiveBuffer) * 2];
    char logBuf[sizeof(hexBuf) + 100];

    toHex(receiveBuffer, bytesRead, hexBuf);

    sniprintf(logBuf, sizeof(logBuf), "Received: %s", hexBuf);

    //Log.log(logBuf);
}

void HoermannE4Class::restartReading()
{
    bytesRead = 0;
    currentState = E4_READING;
    currentStateChangedAt = millis();
}

void HoermannE4Class::handleSend(uint8_t *b1, uint8_t *b2, uint8_t *b3)
{
    CoreMutex m(&sendMutex);

    switch (sendState.sendCommandState)
    {
    case INIT:
    {
        switch (sendState.sendCommand)
        {
        case E4_CMD_LIGHT:
        {
            Log.log("Sending E4_CMD_LIGHT 1");

            *b1 = 0x08;
            *b2 = 0x00;
            *b3 = 0x02;
            break;
        }

        case E4_CMD_CLOSE:
        {
            Log.log("Sending E4_CMD_CLOSE 1");
            *b1 = 0x01;
            *b2 = 0x20;
            *b3 = 0x00;
            break;
        }
        case E4_CMD_OPEN:
        {
            Log.log("Sending E4_CMD_OPEN 1");
            *b1 = 0x01;
            *b2 = 0x10;
            *b3 = 0x00;
            break;
        }
        case E4_CMD_TOGGLE:
        {
            Log.log("Sending E4_CMD_TOGGLE 1");
            *b1 = 0x01;
            *b2 = 0x40;
            *b3 = 0x00;
            break;
        }
        case E4_CMD_HALV_OPEN:
        {
            Log.log("Sending E4_CMD_HALV_OPEN 1");
            *b1 = 0x01;
            *b2 = 0x00;
            *b3 = 0x04;
            break;
        }
        case E4_CMD_VENT:
        {
            Log.log("Sending E4_CMD_VENT 1");
            *b1 = 0x01;
            *b2 = 0x00;
            *b3 = 0x40;
            break;
        }

        default:
            break;
        }

        sendState.sendCommandState = FIRST_CMD_SENDT;
        sendState.sendCommandStateChangedAt = millis();
        break;
    }

    case FIRST_CMD_SENDT:
    {
        if (millis() - sendState.sendCommandStateChangedAt > sendState.sendCommandDelayMs)
        {
            switch (sendState.sendCommand)
            {
            case E4_CMD_LIGHT:
            {
                Log.log("Sending E4_CMD_LIGHT 2");
                *b1 = 0x10;
                *b2 = 0x00;
                *b3 = 0x02;
                break;
            }

            case E4_CMD_CLOSE:
            {
                Log.log("Sending E4_CMD_CLOSE 2");
                *b1 = 0x02;
                *b2 = 0x20;
                *b3 = 0x00;
                break;
            }
            case E4_CMD_OPEN:
            {
                Log.log("Sending E4_CMD_OPEN 2");
                *b1 = 0x02;
                *b2 = 0x10;
                *b3 = 0x00;
                break;
            }
            case E4_CMD_TOGGLE:
            {
                Log.log("Sending E4_CMD_TOGGLE 2");
                *b1 = 0x02;
                *b2 = 0x40;
                *b3 = 0x00;
                break;
            }
            case E4_CMD_HALV_OPEN:
            {
                Log.log("Sending E4_CMD_HALV_OPEN 2");
                *b1 = 0x02;
                *b2 = 0x00;
                *b3 = 0x04;
                break;
            }
            case E4_CMD_VENT:
            {
                Log.log("Sending E4_CMD_VENT 2");
                *b1 = 0x02;
                *b2 = 0x00;
                *b3 = 0x40;
                break;
            }

            default:
                break;
            }

            sendState.sendCommandState = SECOND_CMD_SENDT;
            sendState.sendCommandStateChangedAt = millis();
        }
        break;
    }

    case SECOND_CMD_SENDT:
    {
        if (millis() - sendState.sendCommandStateChangedAt > sendState.sendCommandDelayMs)
        {
            sendState.sendCommandState = FINISHED;
            sendState.sendCommandStateChangedAt = millis();
        }
        break;
    }

    default:
        break;
    }
}

void HoermannE4Class::sendBuf(uint8_t *buf, size_t len)
{
    // sendBuf: len + 2
    uint8_t sendBuf[len + 2];
    memcpy(sendBuf, buf, len);

    // crc
    uint16_t crc = crc16(buf, len);
    sendBuf[len + 0] = (crc) & 0xff;
    sendBuf[len + 1] = (crc >> 8) & 0xff;

    char hexBuf[sizeof(sendBuf) * 2];
    toHex(sendBuf, sizeof(sendBuf), hexBuf);

    char logbuf[1024];
    sniprintf(logbuf, sizeof(logbuf), "Sending: %s", hexBuf);
    //Log.log(logbuf);

    Serial1.write(sendBuf, sizeof(sendBuf));
    // Serial1.flush(); // 4ms
}

void HoermannE4Class::handleMessage()
{
    txLen = 0;
    uint8_t func = receiveBuffer[1];
    if (func != 0x17 && func != 0x10)
    {
        char buf[255];
        sniprintf(buf, sizeof(buf), "Invalid func code %u received", func);
        Log.log(buf);
        return;
    }

    if (func == 0x10)
    {
        uint16_t writeOffset = receiveBuffer[2];
        writeOffset <<= 8;
        writeOffset = writeOffset + receiveBuffer[3];
        uint16_t writeLen = receiveBuffer[4];
        writeLen <<= 8;
        writeLen = writeLen + receiveBuffer[5];
        byteCount = receiveBuffer[6];

        bool crcIsValid = validateCrc16(func16_header_size + byteCount);
        if (!crcIsValid)
        {
            Log.log("Func16: Invalid CRC");
            restartReading();
            return;
        }

        // broadcast 00109D31000912430000004060000000000000100000010000720D
        if (receiveBuffer[0] == 0x00 && writeOffset == 0x9d31 && writeLen == 9 && byteCount == 18)
        {
            handleMessageBroadcast();
        }
        else
        {
            Log.log("Unknown 0x10 messgae");
            restartReading();
            return;
        }
    }

    if (func == 0x17 && receiveBuffer[0] == my_bus_id)
    {
        uint16_t readOffset = receiveBuffer[2];
        readOffset <<= 8;
        readOffset = readOffset + receiveBuffer[3];
        uint16_t readLen = receiveBuffer[4];
        readLen <<= 8;
        readLen = readLen + receiveBuffer[5];
        uint16_t writeOffset = receiveBuffer[6];
        writeOffset <<= 8;
        writeOffset = writeOffset + receiveBuffer[7];
        uint16_t writeLen = receiveBuffer[8];
        writeLen <<= 8;
        writeLen = writeLen + receiveBuffer[9];
        byteCount = receiveBuffer[10];

        bool crcIsValid = validateCrc16(func23_header_size + byteCount);
        if (!crcIsValid)
        {
            Log.log("Func23: Invalid CRC");
            restartReading();
            return;
        }

        // bus-scan
        if (readOffset == 0x9cb9 && readLen == 5 && writeOffset == 0x9c41 && writeLen == 3 && byteCount == 6)
        {
            Log.log("Handling bus-scan");
            uint8_t cnt = receiveBuffer[11];
            uint8_t cmd = receiveBuffer[12];

            int i = 0;
            txBuffer[i++] = receiveBuffer[0];
            txBuffer[i++] = 0x17;
            txBuffer[i++] = 10; // byteCount
            txBuffer[i++] = cnt;
            txBuffer[i++] = 0;
            txBuffer[i++] = cmd;
            txBuffer[i++] = 0x05;
            writeUint16(0x0430, txBuffer, i++);
            i++;
            writeUint16(0x1022, txBuffer, i++); // ff is wrong
            i++;
            writeUint16(0x3270, txBuffer, i++); // a8 45 is wrong
            i++;
            txLen = i;
        }

        // Command request
        else if (readOffset == 0x9cb9 && readLen == 8 && writeOffset == 0x9c41 && writeLen == 2 && byteCount == 4)
        {

            uint8_t b1 = 0;
            uint8_t b2 = 0;
            uint8_t b3 = 0;

            handleSend(&b1, &b2, &b3);

            uint8_t cnt = receiveBuffer[11];
            uint8_t cmd = receiveBuffer[12];
            int i = 0;
            txBuffer[i++] = receiveBuffer[0];
            txBuffer[i++] = 0x17;
            txBuffer[i++] = readLen * 2; // byteCount (16)
            txBuffer[i++] = cnt;
            txBuffer[i++] = 0x00;
            txBuffer[i++] = cmd;
            txBuffer[i++] = 0x01;
            txBuffer[i++] = b1;
            txBuffer[i++] = b2;
            txBuffer[i++] = b3;
            txBuffer[i++] = 0x00; // 10
            txBuffer[i++] = 0x00; // 11
            txBuffer[i++] = 0x00; // 12
            txBuffer[i++] = 0x00; // 13
            txBuffer[i++] = 0x00; // 14
            txBuffer[i++] = 0x00; // 15
            txBuffer[i++] = 0x00; // 16
            txBuffer[i++] = 0x00; // 17
            txBuffer[i++] = 0x00; // 18
            txLen = i;
        }

        // other request
        else if (readOffset == 0x9cb9 && readLen == 2 && writeOffset == 0x9c41 && writeLen == 2 && byteCount == 4)
        {
            uint8_t cnt = receiveBuffer[11];
            uint8_t cmd = receiveBuffer[12];

            Log.log("Handling other request");

            int i = 0;
            txBuffer[i++] = receiveBuffer[0];
            txBuffer[i++] = 0x17;
            txBuffer[i++] = readLen * 2; // byteCount (4)
            txBuffer[i++] = cnt;
            txBuffer[i++] = 0x00;
            txBuffer[i++] = cmd;
            txBuffer[i++] = 0xfd;
            txLen = i;
        }

        else
        {
            char buf[1024];
            Log.log(buf);
            sniprintf(buf, sizeof(buf), "Unsupported fun23-msg received: readOffset=%u, readLen=%u, writeOffset=%u, writeLen=%u", readOffset, readLen, writeOffset, writeLen);
            Log.log(buf);
        }
    }
}

void HoermannE4Class::handleMessageBroadcast()
{
    CoreMutex m(&broadcastMutex);
    size_t offset = 2 + 2 + 2 + 1 + 2; // skip header + 2 bytes
    lastReceivedBroadcast.targetPos = receiveBuffer[offset + 0];
    lastReceivedBroadcast.currentPos = receiveBuffer[offset + 1];
    lastReceivedBroadcast.stateCode = (HoermannE4StateCode)receiveBuffer[offset + 2];
    lastReceivedBroadcast.isVented = receiveBuffer[offset + 3] == 61;
    lastReceivedBroadcast.motorSpeed = receiveBuffer[offset + 8];
    lastReceivedBroadcast.light = receiveBuffer[offset + 11] & (0x10 > 0);
    lastReceivedBroadcast.motorRunning = receiveBuffer[offset + 11] & (0x04 > 0);
    lastReceivedBroadcast.receivedAt = millis();
}

HoermannE4Broadcast HoermannE4Class::currentBroadcast()
{
    CoreMutex m(&broadcastMutex);
    HoermannE4Broadcast copy;
    copy = lastReceivedBroadcast;
    return copy;
}

void HoermannE4Class::run()
{
    switch (currentState)
    {
    case E4_INIT:
    {
        mutex_init(&broadcastMutex);
        mutex_init(&sendMutex);
        Serial1.begin(57600, SERIAL_8E1);
        restartReading();
        break;
    }
    case E4_READING:
    {
        bool readSomething = false;
        size_t bytesReadThisTime = 0;
        while (Serial1.available())
        {
            int read = Serial1.read();

            if (read < 0)
            {
                break;
            }

            if (bytesRead == sizeof(receiveBuffer))
            {
                Log.log("Read buffer full, flushing");
                restartReading();
                break;
            }
            receiveBuffer[bytesRead++] = read;
            bytesReadThisTime++;
            readSomething = true;
        }

        if (readSomething)
        {
            lastReadAt = micros();
        }
        else if (bytesRead > 0 && (micros() - lastReadAt) > messageSeperatePauseMicros)
        {
            printReceiveBuffer();
            handleMessage();

            if (txLen > 0)
            {
                delay(9 - (messageSeperatePauseMicros / 1000)); // TODO this is not non-blocking
                sendBuf(txBuffer, txLen);
                Log.log("Sendt");
            }

            restartReading();
        }

        break;
    }

    default:
        break;
    }
}

HoermannE4Class HoermannE4;
