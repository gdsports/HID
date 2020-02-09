"""
Interface to Nintendo Switch Gamepad Gadget (NSGadget.ino) via serial port.
"""
from struct import *
import array
from enum import IntEnum

# Direction pad names
class NSDPad(IntEnum):
    """NSDPad direction names"""
    CENTERED = 0xF
    UP = 0
    UP_RIGHT = 1
    RIGHT = 2
    DOWN_RIGHT = 3
    DOWN = 4
    DOWN_LEFT = 5
    LEFT = 6
    UP_LEFT = 7

# Button names
class NSButton(IntEnum):
    """NSButton names"""
    Y = 0
    B = 1
    A = 2
    X = 3
    LEFT_TRIGGER = 4
    RIGHT_TRIGGER = 5
    LEFT_THROTTLE = 6
    RIGHT_THROTTLE = 7
    MINUS = 8
    PLUS = 9
    LEFT_STICK = 10
    RIGHT_STICK = 11
    HOME = 12
    CAPTURE = 13

class NSGamepadSerial:
    """Nintendo Switch Gamepad Serial Interface"""
    left_x_axis = 128
    left_y_axis = 128
    right_x_axis = 128
    right_y_axis = 128
    my_buttons = 0
    d_pad = 15
    ser_port = 0
    dpad_x_axis = 128
    dpad_y_axis = 128
    compass_dir_x = array.array('B',
        [0,   0,   128, 255, 255, 255, 128, 0, 128, 128, 128, 128, 128, 128, 128, 128, 128])
    compass_dir_y = array.array('B',
        [128, 255, 255, 255, 128, 0,   0,   0, 128, 128, 128, 128, 128, 128, 128, 128, 128])

    def begin(self, serial_port):
        """Start NSGamepad"""
        self.ser_port = serial_port
        self.left_x_axis = 128
        self.left_y_axis = 128
        self.right_x_axis = 128
        self.right_y_axis = 128
        self.my_buttons = 0
        self.d_pad = 15
        self.dpad_x_axis = 128
        self.dpad_y_axis = 128
        self.write()
        return

    def end(self):
        """End NSGamepad"""
        self.ser_port.close()
        return

    def write(self):
        """Send NSGamepad state"""
        self.ser_port.write(pack('<BBBHBBBBBBB', 2, 9, 2, self.my_buttons, \
            self.d_pad, self.left_x_axis, self.left_y_axis, \
            self.right_x_axis, \
            self.right_y_axis, \
            0, 3))
        return

    def press(self, button_number):
        """Press button 0..13"""
        self.my_buttons |= (1<<button_number)
        self.write()
        return

    def release(self, button_number):
        """Release button 0..13"""
        self.my_buttons &= ~(1<<button_number)
        self.write()
        return

    def releaseAll(self):
        """Release all buttons"""
        self.my_buttons = 0
        self.write()
        return

    def buttons(self, buttons):
        """Set all buttons 0..13"""
        self.my_buttons = buttons
        self.write()
        return

    def leftXAxis(self, position):
        """Move left stick X axis 0..128..255"""
        self.left_x_axis = position
        self.write()
        return

    def leftYAxis(self, position):
        """Move left stick Y axis 0..128..255"""
        self.left_y_axis = position
        self.write()
        return

    def rightXAxis(self, position):
        """Move right stick X axis 0..128..255"""
        self.right_x_axis = position
        self.write()
        return

    def rightYAxis(self, position):
        """Move right stick Y axis 0..128..255"""
        self.right_y_axis = position
        self.write()
        return

    def map_dpad_xy(self, x, y):
        """Return direction pad number given axes x,y"""
        if x == 128:
            if y == 128:
                return 15   # Center
            elif y < 128:
                return 0    # North
            return 4        # South
        elif x < 128:
            if y == 128:
                return 6    # West
            elif y < 128:
                return 7    # North West
            return 5        # South West
        else:
            if y == 128:
                return 2    # East
            elif y < 128:
                return 1    # North East
            return 3        # South East

    def dPadXAxis(self, position):
        """Move right stick X axis 0..128..255"""
        if (position < 0 or position > 255):
            position = 128
        self.dpad_x_axis = position
        self.d_pad = self.map_dpad_xy(self.dpad_x_axis, self.dpad_y_axis)
        self.write()
        return

    def dPadYAxis(self, position):
        """Move right stick Y axis 0..128..255"""
        if (position < 0 or position > 255):
            position = 128
        self.dpad_y_axis = position
        self.d_pad = self.map_dpad_xy(self.dpad_x_axis, self.dpad_y_axis)
        self.write()
        return

    def dPad(self, position):
        """Move directional pad (0..7, 15)"""
        if position < 0 or position > 7:
            position = 15
        self.d_pad = position
        self.dpad_x_axis = self.compass_dir_x[position]
        self.dpad_y_axis = self.compass_dir_y[position]
        self.write()
        return
