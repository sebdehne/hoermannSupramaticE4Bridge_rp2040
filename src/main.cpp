
#include <Arduino.h>
#include <WiFi.h>
#include "hoermannE4.h"
#include "SmartHomeServerClientWifi.h"

bool core1_separate_stack = true;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  SmartHomeServerClientWifi.run(); 
}

void setup1()
{
}

void loop1()
{
  HoermannE4.run();
}
