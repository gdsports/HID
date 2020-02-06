/*
Copyright (c) 2014-2015 NicoHood
See the readme for credit to other people.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

/*
 * This file is a version of GamepadAPI.h customized to make it
 * compatible with the Nintendo Switch.
 */

// Include guard
#pragma once

#include <Arduino.h>
#include "HID-Settings.h"

// Dpad directions
#define NSGAMEPAD_DPAD_CENTERED 0xF
#define NSGAMEPAD_DPAD_UP 0
#define NSGAMEPAD_DPAD_UP_RIGHT 1
#define NSGAMEPAD_DPAD_RIGHT 2
#define NSGAMEPAD_DPAD_DOWN_RIGHT 3
#define NSGAMEPAD_DPAD_DOWN 4
#define NSGAMEPAD_DPAD_DOWN_LEFT 5
#define NSGAMEPAD_DPAD_LEFT 6
#define NSGAMEPAD_DPAD_UP_LEFT 7


#define ATTRIBUTE_PACKED  __attribute__((packed, aligned(1)))

enum NSButtons {
  NSButton_Y = 0,
  NSButton_B,
  NSButton_A,
  NSButton_X,
  NSButton_LeftTrigger,
  NSButton_RightTrigger,
  NSButton_LeftThrottle,
  NSButton_RightThrottle,
  NSButton_Minus,
  NSButton_Plus,
  NSButton_LeftStick,
  NSButton_RightStick,
  NSButton_Home,
  NSButton_Capture,
  NSButton_Reserved1,
  NSButton_Reserved2
};

typedef union ATTRIBUTE_PACKED {
  uint16_t buttons;

  // 14 Buttons, 4 Axis, 1 D-Pads
  struct ATTRIBUTE_PACKED {
    uint8_t button1 : 1;
    uint8_t button2 : 1;
    uint8_t button3 : 1;
    uint8_t button4 : 1;
    uint8_t button5 : 1;
    uint8_t button6 : 1;
    uint8_t button7 : 1;
    uint8_t button8 : 1;

    uint8_t button9 : 1;
    uint8_t button10 : 1;
    uint8_t button11 : 1;
    uint8_t button12 : 1;
    uint8_t button13 : 1;
    uint8_t button14 : 1;
    uint8_t button15 : 1;
    uint8_t button16 : 1;

    uint8_t	dPad;

    uint8_t	leftXAxis;
    uint8_t	leftYAxis;

    uint8_t	rightXAxis;
    uint8_t	rightYAxis;
    uint8_t filler;
  };
} HID_NSGamepadReport_Data_t;

class NSGamepadAPI{
  public:
    inline NSGamepadAPI(void);

    inline void begin(void);
    inline void end(void);
    inline void loop(void);
    inline void write(void);
    inline void write(void *report);
    inline void press(uint8_t b);
    inline void release(uint8_t b);
    inline void releaseAll(void);

    inline void buttons(uint16_t b);
    inline void leftXAxis(uint8_t a);
    inline void leftYAxis(uint8_t a);
    inline void rightXAxis(uint8_t a);
    inline void rightYAxis(uint8_t a);
    inline void dPad(int8_t d);

    // Sending is public in the base class for advanced users.
    virtual void SendReport(void* data, int length) = 0;

  protected:
    HID_NSGamepadReport_Data_t _report;
    uint32_t startMillis;
};

// Implementation is inline
#include "NSGamepadAPI.hpp"
