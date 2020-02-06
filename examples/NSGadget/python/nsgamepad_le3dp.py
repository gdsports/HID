#!/usr/bin/python3
"""
Read from Logitech Extreme 3D Pro and write to NSGadget.

LE3DP -> Raspberry Pi -> NSGadget -> Nintendo Switch
"""
from struct import unpack
import array
from fcntl import ioctl
import serial
from nsgpadserial import NSGamepadSerial

Nsg = NSGamepadSerial()
Nsg.begin(serial.Serial('/dev/ttyUSB0', 8*115200, timeout=0))

# Open the Logitech Extreme 3D Pro
# joystick code based on https://gist.github.com/rdb/8864666
jsdev = open('/dev/input/js0', 'rb')
buf = array.array('B', [0]*64)
ioctl(jsdev, 0x80006a13 + (0x10000 * len(buf)), buf) # JSIOCGNAME(len)
js_name = buf.tostring().rstrip(b'\x00').decode('utf-8')
print(js_name)

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
        if type & 0x80:
            print('(initial)')

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
