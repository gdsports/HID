#!/usr/bin/python3
"""
Exercise NSGamepadSerial class.
"""
from time import sleep
import serial
from nsgpadserial import NSGamepadSerial

Nsg = NSGamepadSerial()
# TODO port name from command line
Nsg.begin(serial.Serial('/dev/ttyUSB0', 8*115200, timeout=0))

while True:
    # Press and hold every button 0..13
    for x in range(0, 14):
        Nsg.press(x)
        sleep(0.1)
    sleep(1)
    # Release all buttons
    Nsg.releaseAll()
    sleep(1)
    # Press all 14 buttons at the same time
    Nsg.buttons(0x3fff)
    sleep(1)
    # Release all buttons
    Nsg.releaseAll()
    sleep(1)
    # Move directional pad in all directions
    # 0 = North, 1 = North-East, 2 = East, etc.
    for x in range(0, 8):
        Nsg.dPad(x)
        sleep(0.5)
    # Move directional pad to center
    Nsg.dPad(15)
