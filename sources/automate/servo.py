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
    Return True if the given variable is an integer, else return False.
    ----------
    Parameters
    ----------
        var : object
            The variable to test
    """
    return isinstance(var, int)


def GPIO_cleanup():
    """
    GPIO cleanup
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

        self._min_voltage = 4
        self._max_voltage = 12.5
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

    def angle_to_percent(self, angle: int):
        """
        Set function to calculate percent from angle
        ----------
        Parameters
        ----------
            angle : int
                The wanted angle
        """
        if angle > 180 or angle < 0:
            return False

        ratio = (self._max_voltage - self._min_voltage) / 180  # Calcul ratio from angle to percent
        angle_as_percent = angle * ratio
        return self._min_voltage + angle_as_percent

    def start(self, start_position=90):
        """
        Start pwm communication
        ----------
        Parameters
        ----------
            start_position: int (from 0 to 180)
                The default position of servo.
        """
        if self._started:
            self._log("GPIO is already started")
            return

        if not (is_integer(start_position) and 0 <= start_position <= 180):
            raise ValueError(f"start_position must be an integer from 0 to 180. Given: {start_position}")

        self._log("Servo is starting...")

        GPIO.setup(self._gpio, GPIO.OUT)
        self._pwm = GPIO.PWM(self._gpio, self._frequency)

        self._started = True
        self._log(f"PWM initialized to pin {self._gpio} at frequency {self._frequency}")

        # Init at 90°
        self._pwm.start(self.angle_to_percent(start_position))
        time.sleep(1)

    def move(self, angle: int):
        """
        Move servo
        ----------
        Parameters
        ----------
            angle: int (from 0 to 180)
                The wanted angle.
        """
        if not self._started:
            self._log("Can not move because servo is not started")
            return

        if not (is_integer(angle) and 0 <= angle <= 180):
            self._log(f"angle must be an integer from 0 to 180. Given: {angle}")
            return

        self._pwm.ChangeDutyCycle(self.angle_to_percent(angle))

    def close(self):
        """
        Close GPIO
        """
        if not self._started:
            self._log("Can not close because GPIO is not started")
            return

        self._log("PWM communication closing...")
        self._pwm.stop()
        self._log("PWM communication closed")
        self._started = False
