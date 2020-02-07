# Nintendo Switch Gamepad Gadget

Emulate a Nintendo Switch compatible USB Gamepad (Hori HoriPAD) on the USB
port. Receive gamepad event input on Serial1 from another computer. Works
on Adafruit Trinket M0.

The data format in the serial port looks like this.

```
<STX> <length> <type> 8 bytes of binary data <ETX>
0x02    0x09    0x02                          0x03
```

The 8 bytes of binary data is the gamepad USB HID report.

```
14 buttons                 Bits 0..13
8 way direction pad        Center=0x0f, 0=North, 1=NEast, 2=East, etc.
left analog thumbstick     X=0..128..255, Y=0..128..255
right analog thumbstick    X=0..128..255, Y=0..128..255

struct NSGamepadEventData
{
  uint16_t  buttons;
  uint8_t   dPad;
  uint8_t   leftXAxis;
  uint8_t   leftYAxis;
  uint8_t   rightXAxis;
  uint8_t   rightYAxis;
  uint8_t   filler;
}__attribute__((packed));
```

## Using the Gadget

To use the gadget with a computer such as a Raspberry Pi, connect the Trinket
M0 TX/RX to RX/TX of a CP2104 USB serial adaptor.

[CP2104 breakout board](https://www.adafruit.com/product/3309)
[Trinket M0](https://www.adafruit.com/product/3500)

Trinket M0  |CP2104
------------|---------
GND         |GND
TX          |RX
RX          |TX

Plugs the CP2104 into a Raspberry Pi. Plugs the Trinket M0 running NSGadget.ino
into a Nintendo Switch. Be sure to change the Trinket M0 VID/PID as directed in
the NSGamepad example.

* python/nsgpadserial.py

Class that handles formatting data to send over USB serial to the Trinket M0.

* python/nsgamepad_test.py

Example to exercise nsgpadserial.py.

* python/nsgamepad_sniffer.py

Example using nsgpadserial.py to pass through gamepad events from a real
HoriPAD gamepad to the NSGadget emulator. The Pi becomes a Pi-in-the-middle
gamepad sniffer. Record and playback of gamepad events is possible but not
implemented.

```
    HoriPAD -> Raspberry Pi -> CP2104 -> Trinket M0 -> Nintendo Switch
               nsgamepad_                NSGadget
                 sniffer.py
```

* python/nsgamepad_le3dp.py

Example using nsgpadserial.py to use a Logitech Extreme 3D Pro flightstick
as a game controller.

```
    Logitech -> Raspberry Pi -> CP2104 -> Trinket M0 -> Nintendo Switch
    Extreme     nsgamepad_                NSGadget
    3D Pro        le3dp.py
```
