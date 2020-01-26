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
 * This file is a version of GamepadAPI.hpp customized to make it
 * compatible with the Nintendo Switch.
 */

// Include guard
#pragma once

NSGamepadAPI::NSGamepadAPI(void)
{
  // Empty
}

void NSGamepadAPI::begin(void){
  // release all buttons
  end();
  startMillis = millis();
}

void NSGamepadAPI::loop(void){
  uint32_t endMillis = millis();
  uint32_t deltaMillis;
  if (endMillis >= startMillis) {
    deltaMillis = (endMillis - startMillis);
  }
  else {
    deltaMillis = ((0xFFFFFFFFUL - startMillis) + endMillis);
  }
  if (deltaMillis >= 7) {
    write();
    startMillis = millis();
  }
}

void NSGamepadAPI::end(void){
  memset(&_report, 0x00, sizeof(_report));
  _report.leftXAxis = _report.leftYAxis = 0x7f;
  _report.rightXAxis = _report.rightYAxis = 0x7f;
  _report.dPad = NSGAMEPAD_DPAD_CENTERED;
  SendReport(&_report, sizeof(_report));
}

void NSGamepadAPI::write(void){
  SendReport(&_report, sizeof(_report));
}


void NSGamepadAPI::press(uint8_t b){
  _report.buttons |= (uint16_t)1 << (b - 1);
}


void NSGamepadAPI::release(uint8_t b){
  _report.buttons &= ~((uint16_t)1 << (b - 1));
}


void NSGamepadAPI::releaseAll(void){
  end();
}

void NSGamepadAPI::buttons(uint16_t b){
  _report.buttons = b;
}


void NSGamepadAPI::leftXAxis(uint8_t a){
  _report.leftXAxis = a;
}


void NSGamepadAPI::leftYAxis(uint8_t a){
  _report.leftYAxis = a;
}


void NSGamepadAPI::rightXAxis(uint8_t a){
  _report.rightXAxis = a;
}


void NSGamepadAPI::rightYAxis(uint8_t a){
  _report.rightYAxis = a;
}


void NSGamepadAPI::dPad(int8_t d){
  _report.dPad = d;
}
