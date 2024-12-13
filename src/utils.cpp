#include "utils.h"
#include "logger.h"

#include <Arduino.h>

void writeSerial16Bytes(unsigned char dst[], int dstOffset)
{
    volatile uint32_t *ptr1 = (volatile uint32_t *)0x0080A00C;
    writeUint32(*ptr1, dst, dstOffset);
    volatile uint32_t *ptr = (volatile uint32_t *)0x0080A040;
    writeUint32(*ptr, dst, dstOffset + 4);
    ptr++;
    writeUint32(*ptr, dst, dstOffset + 8);
    ptr++;
    writeUint32(*ptr, dst, dstOffset + 12);
}

void writeCharArray(char src[], int srcLength, unsigned char dst[], int dstOffset)
{
    for (int i = 0; i < srcLength; i++)
    {
        dst[dstOffset + i] = src[i];
    }
}

void writeUint16(uint16_t src, unsigned char dst[], int dstOffset)
{
    dst[dstOffset + 0] = (src >> 8) & 0xFF;
    dst[dstOffset + 1] = src & 0xFF;
}
void writeUint32(unsigned int src, unsigned char dst[], int dstOffset)
{
    dst[dstOffset + 0] = (src >> 24) & 0xFF;
    dst[dstOffset + 1] = (src >> 16) & 0xFF;
    dst[dstOffset + 2] = (src >> 8) & 0xFF;
    dst[dstOffset + 3] = src & 0xFF;
}
void writeInt32(int src, unsigned char dst[], int dstOffset)
{
    dst[dstOffset + 0] = (src >> 24) & 0xFF;
    dst[dstOffset + 1] = (src >> 16) & 0xFF;
    dst[dstOffset + 2] = (src >> 8) & 0xFF;
    dst[dstOffset + 3] = src & 0xFF;
}
void writeUint8(unsigned char src, unsigned char dst[], int dstOffset)
{
    dst[dstOffset] = src;
}
void writeBool(bool src, unsigned char dst[], int dstOffset)
{
    if (src)
        dst[dstOffset] = 1;
    else
        dst[dstOffset] = 0;
}

unsigned int toUInt(unsigned char src[], int srcOffset)
{
    unsigned int result = 0;
    result = result + src[srcOffset + 0];
    result <<= 8;
    result = result + src[srcOffset + 1];
    result <<= 8;
    result = result + src[srcOffset + 2];
    result <<= 8;
    result = result + src[srcOffset + 3];
    return result;
}

int toInt(unsigned char src[], int srcOffset)
{
    int result = 0;
    result = result + src[srcOffset + 0];
    result <<= 8;
    result = result + src[srcOffset + 1];
    result <<= 8;
    result = result + src[srcOffset + 2];
    result <<= 8;
    result = result + src[srcOffset + 3];
    return result;
}

uint16_t toUint16_t(unsigned char src[], int srcOffset)
{
    uint16_t result = 0;
    result = result + src[srcOffset + 0];
    result <<= 8;
    result = result + src[srcOffset + 1];
    return result;
}

void toHex(uint8_t *src, size_t srcLen, char *dstBuf)
{
    uint8_t b;
    while (srcLen-- > 0)
    {
        b = *src++;
        *(dstBuf++) = toHexChar(b / 16);
        *(dstBuf++) = toHexChar(b % 16);
    }
    *(dstBuf) = 0;
}

int toHexChar(int value)
{
    if (value > 9)
    {
        return 65 + (value - 10);
    }
    else
    {
        return 48 + value;
    }
}

int fromHex(char *src, uint8_t *dstBuf, size_t dstLen)
{
    unsigned int bytesWritten = 0;
    while (bytesWritten < dstLen)
    {
        char msb = *src;
        char lsb = *(src + 1);

        if (msb == 0 || lsb == 0)
        {
            // end reached - done
            break;
        }

        int value = fromHexChar(msb);
        if (value < 0)
        {
            return -1;
        }
        int finalValue = value * 16;

        value = fromHexChar(lsb);
        if (value < 0)
        {
            return -1;
        }
        finalValue += value;

        *dstBuf = finalValue;
        dstBuf++;
        bytesWritten++;
        src++;
        src++;
    }

    return bytesWritten;
}

int fromHexChar(char c)
{
    if (c >= 'a' && c <= 'f')
    {
        return (c - 'a') + 10;
    }
    else if (c >= 'A' && c <= 'F')
    {
        return (c - 'A') + 10;
    }
    else if (c >= '0' && c <= '9')
    {
        return (c - '0');
    }
    Log.log("Could not parse hex string");
    return -1;
}