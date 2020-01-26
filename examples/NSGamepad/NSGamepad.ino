/*
   Gamepad controller for Nintendo Switch.

   The gamepad has 14 buttons, 2 analog joysticks, 1 8-way direction pad.
   Compared to the Nintendo Switch Pro controller, the NSGamepad does
   not have rumble motors, motion sensors (accel/gyro), and Bluetooth.

*/

#include "HID-Project.h"
#include <Bounce2.h>

#define NUM_BUTTONS 14
const uint8_t BUTTON_PINS[NUM_BUTTONS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

Bounce * buttons = new Bounce[NUM_BUTTONS];

void setup() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].attach( BUTTON_PINS[i] , INPUT_PULLUP  );       //setup the bounce instance for the current button
    buttons[i].interval(25);              // interval in ms
  }

  // Sends a clean report to the host. This is important on any Arduino type.
  NSGamepad.begin();
}

void loop() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    // Update the Bounce instance :
    buttons[i].update();
    // Button fell means button pressed
    if ( buttons[i].fell() ) {
      NSGamepad.press(i);
    }
    else if ( buttons[i].rose() ) {
      NSGamepad.release(i);
    }
  }

  NSGamepad.loop();
}
