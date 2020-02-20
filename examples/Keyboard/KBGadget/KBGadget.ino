/*
  Copyright (c) 2014-2015 NicoHood
  See the readme for credit to other people.

  Improved Keyboard example

  Shows how to use the new Keyboard API.

  See HID Project documentation for more information.
  https://github.com/NicoHood/HID/wiki/Keyboard-API#improved-keyboard

  Raspberry Pi 4 TBD????:
    * Turn off WiFi and BT to save power
    * Connect to hardware UART
*/

#include "HID-Project.h"

// On SAMD boards where the native USB port is also the serial console, use
// Serial1 for the serial console. This applies to all SAMD boards except for
// Arduino Zero and M0 boards.
#if (USB_VID==0x2341 && defined(ARDUINO_SAMD_ZERO)) || (USB_VID==0x2a03 && defined(ARDUINO_SAM_ZERO))
#define SerialDebug SERIAL_PORT_MONITOR
#else
#define SerialDebug Serial1
#endif

#define KB_UART    Serial1
#define kbbegin(...)      KB_UART.begin(__VA_ARGS__)
#define kbprint(...)      KB_UART.print(__VA_ARGS__)
#define kbwrite(...)      KB_UART.write(__VA_ARGS__)
#define kbprintln(...)    KB_UART.println(__VA_ARGS__)
#define kbread(...)       KB_UART.read(__VA_ARGS__)
#define kbreadBytes(...)  KB_UART.readBytes(__VA_ARGS__)
#define kbavailable(...)  KB_UART.available(__VA_ARGS__)
#define kbsetTimeout(...) KB_UART.setTimeout(__VA_ARGS__)

#define DEBUG_ON  0
#if DEBUG_ON
#define dbbegin(...)      SerialDebug.begin(__VA_ARGS__)
#define dbprint(...)      SerialDebug.print(__VA_ARGS__)
#define dbprintln(...)    SerialDebug.println(__VA_ARGS__)
#define dbwrite(...)      SerialDebug.write(__VA_ARGS__)
#else
#define dbbegin(...)
#define dbprint(...)
#define dbprintln(...)
#define dbwrite(...)
#endif

#ifdef ADAFRUIT_TRINKET_M0
// setup Dotstar LED on Trinket M0
#include <Adafruit_DotStar.h>
#define DATAPIN    7
#define CLOCKPIN   8
Adafruit_DotStar strip = Adafruit_DotStar(1, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
#endif

bool HID_MODE = false;

uint32_t elapsed_mSecs(uint32_t last_millis)
{
  uint32_t now = millis();
  if (now < last_millis) {
    return (now + 1) + (0xFFFFFFFF - last_millis);
  }
  else {
    return now - last_millis;
  }
}

const int pinLed = LED_BUILTIN;

const uint8_t STX = 0x02;
const uint8_t ETX = 0x03;

uint8_t kb_report(uint8_t *buffer, size_t buflen)
{
  static uint8_t kb_buffer[32];
  static uint8_t kb_buflen;
  static uint8_t kb_state=0;
  static uint8_t kb_expectedlen;
  static uint32_t timeout_ms;
  size_t bytesIn;

  int byt;

  while (kbavailable() > 0) {
    switch (kb_state) {
      case 0:
        dbprint(kb_state); dbprint(',');
        byt = kbread();
        if (byt != -1) {
          dbprintln(byt, HEX);
          if (byt == STX) {
            timeout_ms = millis();
            kb_state = 1;
            kb_buflen = 0;
          }
        }
        break;
      case 1:
        dbprint(kb_state); dbprint(',');
        byt = kbread();
        if (byt != -1) {
          dbprintln(byt, HEX);
          kb_buffer[0] = byt;
          kb_buflen = 1;
          kb_expectedlen = byt;
          if (kb_expectedlen > (sizeof(kb_buffer) - 1)) {
            kb_expectedlen = sizeof(kb_buffer) - 1;
          }
          kb_state = 2;
        }
        break;
      case 2:
        dbprint(kb_state); dbprint(',');
        byt = kbread();
        if (byt != -1) {
          dbprintln(byt, HEX);
          kb_buffer[1] = byt;
          kb_buflen = 2;
          kb_state = 3;
        }
        break;
      case 3:
        dbprint(kb_state); dbprint(',');
        bytesIn = kbreadBytes(&kb_buffer[kb_buflen], kb_expectedlen - kb_buflen + 1);
        dbprintln(bytesIn);
        if (bytesIn > 0) {
          kb_buflen += bytesIn;
          if (kb_buflen > kb_expectedlen) {
            kb_state = 4;
          }
        }
        break;
      case 4:
        dbprint(kb_state); dbprint(',');
        byt = kbread();
        if (byt != -1) {
          dbprintln(byt, HEX);
          if (byt == ETX) {
            if (kb_buflen > buflen) kb_buflen = buflen;
            memcpy(buffer, kb_buffer, kb_buflen);
            kb_state = 0;
            return kb_buflen;
          }
          kb_state = 0;
        }
        break;
      default:
        dbprintln("kbread: invalid state");
        break;
    }
  }
  // If STX seen and more than 50 ms, give up and go back to looking for STX
  if ((kb_state != 0) && (elapsed_mSecs(timeout_ms) > 50)) {
    kb_state = 0;
  }
  return 0;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(2, INPUT_PULLUP);
  delay(1);
  if (digitalRead(2) == LOW) {
    HID_MODE = true;
  }
  kbbegin( 115200 );
  kbsetTimeout(0);
  dbbegin( 115200 );
  dbprintln();

#ifdef ADAFRUIT_TRINKET_M0
  // Turn off built-in Dotstar RGB LED
  strip.begin();
  strip.clear();
  strip.show();
#endif

  // Sends a clean report to the host. This is important on any Arduino type.
  BootKeyboard.begin();
  Keyboard.begin();
}

void loop() {
  if (HID_MODE) {
    uint8_t kbData[32];
    uint8_t reportLen = kb_report(kbData, sizeof(kbData));
    if ((reportLen > 1) && (kbData[1] == 2)) {
      HID().SendReport(HID_REPORTID_KEYBOARD, &kbData[2], 8);
    }
  }
  else {
    while (kbavailable() > 0) {
      BootKeyboard.write(kbread());
    }
  }
}
