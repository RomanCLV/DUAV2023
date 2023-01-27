#-- coding: utf-8 --

import RPi.GPIO as GPIO
import time

global _pwm_gpio
global _pwm_frequence
global _pwm

global _initialized
_initialized = False

global _started
_started = False

global _log_listeners
_log_listeners = []


def angle_to_percent (angle: int) :
    """
    Set function to calculate percent from angle
    ----------
    Parameters
    ----------
        angle : int
            The wanted angle
    """
    if angle > 180 or angle < 0 :
        return False

    start = 4
    end = 12.5
    ratio = (end - start) / 180 # Calcul ratio from angle to percent

    angle_as_percent = angle * ratio

    return start + angle_as_percent


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


def init(gpio: int, frequence: int):
    """
    Init function.
    ----------
    Parameters
    ----------
        gpio : int (from 1 to 41)
            The pin used to controll the servo
        frequence : int (greater than 0)
            Frenquence of communication
    """
    global _pwm_gpio
    global _pwm_frequence
    global _initialized
    
    if is_integer(gpio) and gpio > 0 and gpio < 41:
        _pwm_gpio = gpio
    else:
        raise ValueError(f"gpio must be an integer between 1 and 41. Given: {gpio}")
    
    if is_integer(frequence) and frequence > 0:
        _pwm_frequence = frequence
    else:
        raise ValueError(f"frequence must be an integer greater than 0. Given: {frequence}")
    
    _initialized = True


def add_log_listener(listener):
    """
    Add a log listener.
    ----------
    Parameters
    ----------
        listener: an entity who must have a log(o) function
            The listener to add
    """
    global _log_listeners
    
    if listener not in _log_listeners:
        _log_listeners.append(listener)


def remove_log_listener(listener):
    """
    Remove a log listener.
    ----------
    Parameters
    ----------
        listener:
            The listener to remove
    """
    global _log_listeners
    
    if listener in _log_listeners:
        _log_listeners.remove(listener)


def log(o):
    """
    Log a message by calling all log_listeners
    """
    for i in range(len(_log_listeners)):
        try:
            _log_listeners[i].log(o)
        except:
            print(f"The {str(_log_listeners[i])} log listener has not log function")


def start(start_position=90):
    """
    Start pwm communication
    ----------
    Parameters
    ----------
        start_position: int (from 0 to 180)
            The default position of servo.
    """
    global _pwm_gpio
    global _pwm_frequence
    global _pwm
    global _initialized
    global _started
    
    if _started:
        log("GPIO is already started")
        return
    
    if not _initialized:
        raise Exception("Servo not initialized! Call first the init() function")
    
    if not(is_integer(start_position) and start_position >= 0 and start_position <= 180):
        raise Exception(f"start_position must be an integer from 0 to 180. Given: {start_position}")
    
    log("GPIO starting...")
    GPIO.setmode(GPIO.BOARD) # Use Board numerotation mode
    GPIO.setwarnings(False)  # Disable warnings
    GPIO.setup(_pwm_gpio, GPIO.OUT)
    _pwm = GPIO.PWM(_pwm_gpio, _pwm_frequence)
    
    _started = True
    log(f"GPIO started to pin {_pwm_gpio} at frequence {_pwm_frequence}")
    
    # Init at 90°
    _pwm.start(angle_to_percent(start_position))
    time.sleep(1)


def move(angle):
    """
    Move servo
    ----------
    Parameters
    ----------
        angle: int (from 0 to 180)
            The wanted angle.
    """
    global _pwm
    global _started
    
    if not _started:
        log("Can not move because GPIO is not started")
        return
    
    if not(is_integer(angle) and angle >= 0 and angle <= 180):
        log(f"angle must be an integer from 0 to 180. Given: {angle}")
        return
    
    _pwm.ChangeDutyCycle(angle_to_percent(angle))


def close():
    """
    Close GPIO & cleanup
    """
    global _pwm
    global _started
    
    if not _started:
        log("Can not close because GPIO is not started")
        return
    
    log("GPIO closing...")
    _pwm.stop()
    GPIO.cleanup()
    log("GPIO closed")
