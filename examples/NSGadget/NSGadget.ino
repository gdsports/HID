/*
 * Nintendo Switch Gamepad Gadget
 *
 * Emulate a Nintendo Switch compatible USB Gamepad (Hori HoriPAD) on the USB
 * port. Receive gamepad event input on Serial1 from another computer. Works
 * on Adafruit Trinket M0.
 *
 * The format in the serial port looks like this.
 * <STX> <length> <type> 8 bytes of binary data <ETX>
 * 0x02    0x09    0x02                          0x03
 *
 * The 8 bytes of binary data is the gamepad USB HID report.
 * 14 buttons                 Bits 0..13
 * 8 way direction pad        Center=0x0f, 0=North, 1=NEast, 2=East, etc.
 * left analog thumbstick     X=0..128..255, Y=0..128..255
 * right analog thumbstick    X=0..128..255, Y=0..128..255
 *
 * struct NSGamepadEventData
 * {
 *   uint16_t  buttons;
 *   uint8_t   dPad;
 *   uint8_t   leftXAxis;
 *   uint8_t   leftYAxis;
 *   uint8_t   rightXAxis;
 *   uint8_t   rightYAxis;
 *   uint8_t   filler;
 * }__attribute__((packed));
 *
 * The NSGamepad library is in my fork of NicoHood's HID library.
 */
#include "HID-Project.h"  // https://github.com/gdsports/HID/tree/horipad

// On SAMD boards where the native USB port is also the serial console, use
// Serial1 for the serial console. This applies to all SAMD boards except for
// Arduino Zero and M0 boards.
#if defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAM_ZERO)
#define SerialDebug SERIAL_PORT_MONITOR
#else
#define SerialDebug Serial1
#endif

#define NS_UART    Serial1
#define nsbegin(...)      NS_UART.begin(__VA_ARGS__)
#define nsprint(...)      NS_UART.print(__VA_ARGS__)
#define nswrite(...)      NS_UART.write(__VA_ARGS__)
#define nsprintln(...)    NS_UART.println(__VA_ARGS__)
#define nsread(...)       NS_UART.read(__VA_ARGS__)
#define nsreadBytes(...)  NS_UART.readBytes(__VA_ARGS__)
#define nsavailable(...)  NS_UART.available(__VA_ARGS__)
#define nssetTimeout(...) NS_UART.setTimeout(__VA_ARGS__)

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

const uint8_t STX = 0x02;
const uint8_t ETX = 0x03;

uint8_t ns_report(uint8_t *buffer, size_t buflen)
{
  static uint8_t ns_buffer[32];
  static uint8_t ns_buflen;
  static uint8_t ns_state=0;
  static uint8_t ns_expectedlen;
  static uint32_t timeout_ms;
  size_t bytesIn;

  int byt;

  while (nsavailable() > 0) {
    switch (ns_state) {
      case 0:
        dbprint(ns_state); dbprint(',');
        byt = nsread();
        if (byt != -1) {
          dbprintln(byt, HEX);
          if (byt == STX) {
            timeout_ms = millis();
            ns_state = 1;
            ns_buflen = 0;
          }
        }
        break;
      case 1:
        dbprint(ns_state); dbprint(',');
        byt = nsread();
        if (byt != -1) {
          dbprintln(byt, HEX);
          ns_buffer[0] = byt;
          ns_buflen = 1;
          ns_expectedlen = byt;
          if (ns_expectedlen > (sizeof(ns_buffer) - 1)) {
            ns_expectedlen = sizeof(ns_buffer) - 1;
          }
          ns_state = 2;
        }
        break;
      case 2:
        dbprint(ns_state); dbprint(',');
        byt = nsread();
        if (byt != -1) {
          dbprintln(byt, HEX);
          ns_buffer[1] = byt;
          ns_buflen = 2;
          ns_state = 3;
        }
        break;
      case 3:
        dbprint(ns_state); dbprint(',');
        bytesIn = nsreadBytes(&ns_buffer[ns_buflen], ns_expectedlen - ns_buflen + 1);
        dbprintln(bytesIn);
        if (bytesIn > 0) {
          ns_buflen += bytesIn;
          if (ns_buflen > ns_expectedlen) {
            ns_state = 4;
          }
        }
        break;
      case 4:
        dbprint(ns_state); dbprint(',');
        byt = nsread();
        if (byt != -1) {
          dbprintln(byt, HEX);
          if (byt == ETX) {
            if (ns_buflen > buflen) ns_buflen = buflen;
            memcpy(buffer, ns_buffer, ns_buflen);
            ns_state = 0;
            return ns_buflen;
          }
          else if (byt == STX) {
            timeout_ms = millis();
            ns_state = 1;
            ns_buflen = 0;
            return 0;
          }
          ns_state = 0;
        }
        break;
      default:
        dbprintln("nsread: invalid state");
        break;
    }
  }
  // If STX seen and more than 2 ms, give up and go back to looking for STX
  if ((ns_state != 0) && (elapsed_mSecs(timeout_ms) > 2)) {
    ns_state = 0;
    static uint32_t timeout_error = 0;
    timeout_error++;
    digitalWrite(LED_BUILTIN, timeout_error & 1);
  }
  return 0;
}

void setup()
{
  // Turn off built-in LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  nsbegin( 2000000 );
  nssetTimeout(0);
  dbbegin( 115200 );
  dbprintln("NSGadget setup");

#ifdef ADAFRUIT_TRINKET_M0
  // Turn off built-in Dotstar RGB LED
  strip.begin();
  strip.clear();
  strip.show();
#endif

  NSGamepad.begin();
}

void loop()
{
  uint8_t nsData[32];
  uint8_t reportLen = ns_report(nsData, sizeof(nsData));
  if ((reportLen > 1) && (nsData[1] == 2)) {
    NSGamepad.write(&nsData[2]);
  }

  NSGamepad.loop();
}
