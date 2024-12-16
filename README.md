# Hörmann Supramatic E4 Wifi Bridge for Arduino Nano RP2040 Connect

Firmware for the [Arduino Nano RP2040 Connect](https://store.arduino.cc/products/arduino-nano-rp2040-connect) which acts as a wifi bridge. Let's you
use simple IP/UDP to interact with the door opener.

- Uses the [arduino-pico SDK](https://arduino-pico.readthedocs.io/en/latest/)
- Listens on UDP port 9000
- When a UDP-package is received with 1-byte payload, then this byte is translated into a [HoermannE4Command](src/hoermannE4.h#L29) 
  and a response is sent back to the same IP+port as the request is received from.
- Use command 0 (Nop) to just query current state

This project somehwhat inspired by the work of [blog.dupas.be](https://blog.dupas.be/posts/hoermann-uap-hcp1/) - but I 
had to do my own reverse engineering of the Hörmann UAP1-HCP because the values didn't work for me.

## Hardware
- [Arduino Nano RP2040 Connect](https://store.arduino.cc/products/arduino-nano-rp2040-connect)
- Use a DC/DC converter to down-convert 24V to 5V for the arduino board. Make sure you use one with relative 
  low noise. At first, I tried a [ITZ0924S05](https://www.digikey.no/en/products/detail/xp-power/ITZ0924S05/6797901), but
  that module was way too noicy and didn't work. Now I am using [LM2596S-ADJ Power Module](https://www.amazon.com/gp/product/B00LTSC564?ref=ppx_pt2_dt_b_prod_image).
- And for the RS-248/TTL adapter, I am using one of [these](https://www.amazon.com/gp/product/B082Y19KV9?ref=ppx_pt2_dt_b_prod_image).

## Demo
Integrated into my [SmartHome app](https://github.com/sebdehne/SmartHomeServer), it looks like this:

<img src="doc/garageDoorDemo.gif" alt="Garage door demo" style="width:50%; height:auto;">
