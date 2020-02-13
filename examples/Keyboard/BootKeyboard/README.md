# SAMD BootKeyboard

Make an Arduino SAMD USB keyboard that works in BIOS/boot mode. BootKeyboard
should work on all Arduino and Adafruit SAMD21 and SAMD51 boards. This should
also work with Keyboard Video Mouse (KVM) switches. The changes are all
intended to make the SAMD board looks as much as possible like a real USB
keyboard. This includes removing the USB CDC ACM serial port. Double click on
the reset button to upload.

Tested on Arduino Zero, Arduino Nano 33 IOT, and Adafruit Metro M4 with 4
different BIOSes.

Patches are required to the Arduino and Adafruit SAMD board packages. See the
samd_patch.txt file for details.

Use the Arduino [Portable IDE](https://www.arduino.cc/en/Guide/PortableIDE)
feature to create a safe workspace. If this is not done, the board patches
affect all sketches.

