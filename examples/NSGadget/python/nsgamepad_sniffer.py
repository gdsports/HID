#!/usr/bin/python3
"""
Read from Hori HoriPAD (Nintendo Switch compatible) and write to
NSGadget.

HoriPAD -> Raspberry Pi -> NSGadget -> Nintendo Switch
"""
import os
from sys import exit
from struct import unpack
import array
from fcntl import ioctl
import serial
from nsgpadserial import NSGamepadSerial

Nsg = NSGamepadSerial()
Nsg.begin(serial.Serial('/dev/ttyUSB0', 8*115200, timeout=0))

# Open the Hori HoriPAD which is Nintendo Switch compatible
# joystick code based on https://gist.github.com/rdb/8864666
HoriPAD = False
for fn in os.listdir('/dev/input'):
    if fn.startswith('js'):
        print('/dev/input/%s' % (fn))
        jsdev = open('/dev/input/' + fn, 'rb')
        buf = array.array('B', [0] * 64)
        ioctl(jsdev, 0x80006a13 + (0x10000 * len(buf)), buf) # JSIOCGNAME(len)
        js_name = buf.tobytes().rstrip(b'\x00').decode('utf-8').upper()
        print('Device name: %s' % js_name)
        if 'HORIPAD' in js_name:
            HoriPAD = True
            break
        else:
            jsdev.close()

if not HoriPAD:
    print('HoriPAD not found')
    exit(1)

print('HoriPAD Found')

# Get number of axes and buttons
buf = array.array('B', [0])
ioctl(jsdev, 0x80016a11, buf) # JSIOCGAXES
num_axes = buf[0]
print('num_axes = %s' % num_axes)

buf = array.array('B', [0])
ioctl(jsdev, 0x80016a12, buf) # JSIOCGBUTTONS
num_buttons = buf[0]
print('num_buttons = %s' % num_buttons)

while True:
    evbuf = jsdev.read(8)
    if evbuf:
        time, value, type, number = unpack('IhBB', evbuf)
        if type & 0x01: # button event
            if value:
                Nsg.press(number)
            else:
                Nsg.release(number)

        if type & 0x02: # axis event
            axis = ((value + 32767) >> 8)
            if axis == 127:
                axis = 128
            # Axes 0,1 left stick X,Y
            if number == 0:
                Nsg.leftXAxis(axis)
            elif number == 1:
                Nsg.leftYAxis(axis)
            # Axes 2,3 right stick X,Y
            elif number == 2:
                Nsg.rightXAxis(axis)
            elif number == 3:
                Nsg.rightYAxis(axis)
            # Axes 4,5 directional pad X,Y
            elif number == 4:
                Nsg.dPadXAxis(axis)
            elif number == 5:
                Nsg.dPadYAxis(axis)
