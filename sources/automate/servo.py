# -*- coding: utf-8 -*-
"""
Servo module
Copyright ©2023 ESTI'AERO
By Roman Clavier

Module used to control a Servo.

Usefully links:

RPi.GPIO : https://sourceforge.net/p/raspberry-gpio-python/wiki/Examples/
"""

import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BOARD)  # Use Board numeration mode
GPIO.setwarnings(False)  # Disable warnings


def is_integer(var):
    """
    Returns True if the given variable is an integer, else returns False.
    ----------
    Parameters
    ----------
        var : object
            The variable to test
    """
    return isinstance(var, int)


def is_float(var):
    """
    Returns True if the given variable is a float, else returns False.
    ----------
    Parameters
    ----------
        var : object
            The variable to test
    """
    return isinstance(var, float)


def check_angle(angle: float):
    """
    Returns True if the angle is a number between 0.0 and 180.0, else returns False.
    
    ----------
    Parameters
    ----------
        angle : float
            The wanted angle
    """
    return (is_integer(angle) or is_float(angle)) and 0 <= angle <= 180:    


def angle_to_percent(angle: float):
    """
    Compute percent from angle. If angle is not a number between 0 and 180, returns False, else returns a number between 0 and 100.
    ----------
    Parameters
    ----------
        angle : float
            The wanted angle
    """
    if not check_angle(angle):
        return False
    """
    ratio = (12.5 - 4) / 180  # Calcul ratio from angle to percent
    angle_as_percent = angle * ratio
    return 4 + angle_as_percent
    """
    return (angle * 100) / 180


def GPIO_cleanup():
    """
    Clean up all GPIO.
    """
    GPIO.cleanup()


class Servo:
    def __init__(self, gpio: int, frequency: int):
        """
        Constructor.
        ----------
        Parameters
        ----------
            gpio : int (from 1 to 41)
                The pin used to control the servo
            frequency : int (greater than 0)
                Frequency of communication
        """

        # self._min_voltage = 4
        # self._max_voltage = 12.5
        self._started = False
        self._log_listeners = []
        self._pwm = None

        if is_integer(gpio) and 0 < gpio < 41:
            self._gpio = gpio
        else:
            raise ValueError(f"gpio must be an integer between 1 and 41. Given: {gpio}")

        if is_integer(frequency) and frequency > 0:
            self._frequency = frequency
        else:
            raise ValueError(f"frequency must be an integer greater than 0. Given: {frequency}")

    def get_name(self):
        return f"Servo {self._gpio}"

    # region log
    def add_log_listener(self, listener):
        """
        Add a log listener.
        ----------
        Parameters
        ----------
            listener: an entity who must have a log(o) function
                The listener to add
        """
        if listener not in self._log_listeners:
            self._log_listeners.append(listener)

    def remove_log_listener(self, listener):
        """
        Remove a log listener.
        ----------
        Parameters
        ----------
            listener:
                The listener to remove
        """
        if listener in self._log_listeners:
            self._log_listeners.remove(listener)

    def _log(self, o):
        """
        Log a message by calling all log_listeners
        """
        if len(self._log_listeners) == 0:
            print(o)
        else:
            for i in range(len(self._log_listeners)):
                try:
                    self._log_listeners[i].log(o)
                except AttributeError:
                    print(f"The {str(self._log_listeners[i])} log listener has not log function")

    # endregion
    
    def start(self, start_position=90.0):
        """
        Start pwm communication
        ----------
        Parameters
        ----------
            start_position: float (from 0.0 to 180.0)
                The default servo position.
        """
        if self._started:
            self._log(f"{self.get_name()} is already started")
            return

        if not check_angle(start_position):
            raise ValueError(f"angle must be a float from 0.0 to 180.0. Given: {angle}")

        self._log(f"{self.get_name()} is starting...")

        GPIO.setup(self._gpio, GPIO.OUT)
        self._pwm = GPIO.PWM(self._gpio, self._frequency)

        self._started = True
        self._log(f"PWM initialized to pin {self._gpio} at frequency {self._frequency}")

        self._pwm.start(angle_to_percent(start_position))
        time.sleep(1)
    
    def move(self, angle: float):
        """
        Move servo
        ----------
        Parameters
        ----------
            angle: float (from 0.0 to 180.0)
                The wanted angle.
        """
        if not self._started:
            self._log("Can not move because servo is not started")
            return
        percent = angle_to_percent(angle)  # returns False or an number between 0 and 100
        if percent or percent == 0:        # case percent == 0 because if angle = 0, so percent = 0, and so 0 is like False (but we accept the value 0)
            self._pwm.ChangeDutyCycle(percent)

    def close(self, clean_up=True):
        """
        Stop GPIO
        ----
        Parameters:
            clean_up: bool
                Clean up the current GPIO
        """
        if not self._started:
            self._log("Can not close because GPIO is not started")
            return

        self._log("PWM communication closing...")
        self._pwm.stop()
        self._log("PWM communication closed")
        self._started = False
        self._pwm = None
        if clean_up:
            GPIO.clean_up(self._gpio)
            self._log(f"{self.get_name()} GPIO cleaned up.")
