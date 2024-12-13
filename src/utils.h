#ifndef _UTILS_H
#define _UTILS_H
#include <Arduino.h>

void writeSerial16Bytes(unsigned char dst[], int dstOffset);
void writeCharArray(char src[], int srcLength, unsigned char dst[], int dstOffset);

void writeUint32(unsigned int src, unsigned char dst[], int dstOffset);
void writeUint16(uint16_t src, unsigned char dst[], int dstOffset);
void writeInt32(int src, unsigned char dst[], int dstOffset);
void writeUint8(unsigned char src, unsigned char dst[], int dstOffset);
void writeBool(bool src, unsigned char dst[], int dstOffset);

unsigned int toUInt(unsigned char src[], int srcOffset);
int toInt(unsigned char src[], int srcOffset);

uint16_t toUint16_t(unsigned char src[], int srcOffset);

void toHex(uint8_t *src, size_t srcLen, char *dstBuf);
int fromHex(char *src, uint8_t *dstBuf, size_t dstLen);
int toHexChar(int value);
int fromHexChar(char c);

#endif