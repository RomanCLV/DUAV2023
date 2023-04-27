# -*- coding: utf-8 -*-
"""
Automate module
Copyright ©2023 ESTI'AERO
By Roman Clavier

Module used to control the drone.
"""

from enum import Enum
import os
import time
import datetime
import random
import signal
import argparse

import servo as serv


class STATE(Enum):
    INIT = 0
    DO_NOTHING = 1
    EXPLORATION = 2
    MISSION = 3
    RETURN_TO_HOME = 4
    SHUTDOWN = 5


class Logger:

    def log(self, o, display=True):
        global start_time
        delay = datetime.datetime.fromtimestamp((get_millis() - start_time) / 1000.0)
        hour = delay.hour - 1
        minute = delay.minute
        second = delay.second
        milli = round(delay.microsecond / 1000)
        msg = f"{hour:02d}:{minute:02d}:{second:02d}.{milli:03d} : {o}"
        if display:
            print(msg)

        log_file = "logs/automate_fake.log"

        create_folder_if_not_exists("logs")
    
        if not os.path.exists(log_file):
            with open(log_file, "x") as file:
                pass
        
        with open(log_file, "a") as file:
            now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            file.write(f"{now}\t{msg}\n")


global run
global RTH_PATH
global request
global state
global start_time
global last_time_state_changed
global servos
global is_opened
global opened_date
global closed_tank_position
global opened_tank_position
global logger


def create_folder_if_not_exists(folder_name: str):
    if not os.path.exists(folder_name):
        os.makedirs(folder_name)


def get_millis():
    return round(time.time() * 1000)


def rth_file_exists():
    global RTH_PATH
    return os.path.exists(RTH_PATH)


def remove_rth_file():
    global RTH_PATH
    if rth_file_exists():
        os.remove(RTH_PATH)


def log(o):
    global logger
    logger.log(o)


def main(args):
    global run
    global RTH_PATH
    global request
    global state
    global start_time
    global last_time_state_changed
    global servos
    global is_opened
    global closed_tank_position
    global opened_tank_position
    global opened_date
    global logger

    run = True

    request = STATE.INIT
    state = STATE.INIT

    is_opened = False
    opened_date = 0
    closed_tank_position = 0
    opened_tank_position = 45

    start_time = 0
    last_time_state_changed = 0

    servos = []

    logger = Logger()

    print("current folder:", os.getcwd())

    RTH_PATH = "./RTH"

    if args.gpio and len(args.gpio) > 0:
        for gpio in args.gpio:
            servo = serv.Servo(abs(int(gpio)), 50, gpio[0] == '-')  # pin, frequency
            servo.add_log_listener(logger)
            servos.append(servo)
    else:
        raise Exception("Require at least one GPIO")

    remove_rth_file()

    start_time = get_millis()
    
    while run:
        # read GPS signal
        if run:
            time.sleep(0.100)    # delete after
        else:
            break

        automate_request()
        automate_state()
        
        if not run or state == STATE.SHUTDOWN:
            break
    
    before_exit()
    log("Automate exit")


def set_state(new_state):
    global state
    global last_time_state_changed
    state = new_state
    last_time_state_changed = get_millis()


def automate_request():
    global run
    global state
    global request

    if request == state and request != STATE.INIT:
        return
    
    if request == STATE.INIT:
        set_state(STATE.INIT)
    
    elif request == STATE.EXPLORATION:
        close_tanks()
        log("Exploration engaged")
        set_state(STATE.EXPLORATION)

    elif request == STATE.MISSION:
        close_tanks()
        log("Mission engaged")
        set_state(STATE.MISSION)

    elif request == STATE.DO_NOTHING:
        close_tanks()
        log("Do nothing engaged")
        set_state(STATE.DO_NOTHING)

    elif request == STATE.RETURN_TO_HOME:
        close_tanks()
        log("Return to home engaged")
        set_state(STATE.RETURN_TO_HOME)

    elif request == STATE.SHUTDOWN:
        close_tanks()
        set_state(STATE.SHUTDOWN)

    else:
        before_exit()
        log(f"Unexpected request value: {request}")
        run = False


def automate_state():
    global run
    global state
    global request
    global start_time
    global last_time_state_changed
    global servos
    global is_opened
    global closed_tank_position
    global opened_tank_position
    global opened_date

    delay = get_millis() - last_time_state_changed

    if state == STATE.INIT:
        for servo in servos:
            log(f"{servo.get_name()} starting...")

            try:
                servo.start(opened_tank_position if servo.get_is_inversed() else closed_tank_position)
            except RuntimeError as err:
                before_exit()
                raise err                

        log("Automate initialized")
        request = STATE.EXPLORATION
    
    elif state == STATE.EXPLORATION:
        close_tanks()
        
        if rth_file_exists():
            log("RTH file detected!")
            request = STATE.RETURN_TO_HOME
            

        elif delay > 5000:
            log("Exploration done")
            request = STATE.MISSION
    
    elif state == STATE.MISSION:
        if rth_file_exists():
        	close_tanks()
            log("RTH file detected!")
            request = STATE.RETURN_TO_HOME

        elif is_opened:
            if get_millis() - opened_date > 3000:
                close_tanks()
        else:

            if get_millis() - opened_date > 6000 and is_near_to():
                open_tanks()

        if delay > 15000:
            log("Mission done")
            request = STATE.DO_NOTHING

    elif state == STATE.RETURN_TO_HOME:
        close_tanks()
        if delay > 5000:
            log("Return to home done")
            request = STATE.DO_NOTHING

    elif state == STATE.DO_NOTHING:
        close_tanks()
        if delay > 3000:
            request = STATE.SHUTDOWN

    elif state == STATE.SHUTDOWN:
        log("Shutting down...")
        before_exit()
        remove_rth_file()

    else:
        before_exit()
        log(f"Unexpected state value: {state}")
        run = False


def is_near_to():
    return random.random() > 0.95


def close_tanks():
    global servos
    global is_opened
    global closed_tank_position
    global opened_tank_position

    if is_opened:
        is_opened = False
        for servo in servos:
            servo.move(opened_tank_position if servo.get_is_inversed() else closed_tank_position)
        log("Tanks closing...")


def open_tanks():
    global servos
    global is_opened
    global opened_tank_position
    global closed_tank_position
    global opened_date

    opened_date = get_millis()
    is_opened = True
    for servo in servos:
        servo.move(closed_tank_position if servo.get_is_inversed() else opened_tank_position)
    log("Tanks opening...")


def close_all_servo():
    global servos
    for servo in servos:
        log(f"{servo.get_name()} closing...")
        servo.close(True)  # clean up enabled
        log(f"{servo.get_name()} closed")

    serv.GPIO_cleanup()    # useless because each servo has already been closed and cleaned up, but to be sure...


def before_exit():
    close_tanks()
    close_all_servo()


def sigint_handler(signal, frame):
    global run
    run = False


if __name__ == "__main__":
    signal.signal(signal.SIGINT, sigint_handler)
    parser = argparse.ArgumentParser(description="automate.py CLI")
    parser.add_argument("gpio", type=str, nargs="+", help="set the servo GPIO PINs (if negative, set servo.is_inversed to True). Pin '-32' means pin 32 with inversed mode")
    args = parser.parse_args()

    try:
        main(args)

    except SystemExit:
        print("\nProcess finished")
        before_exit()
        input("Press a key to close...")
        
    except BaseException as e:
        print("\nUnexpected error occured")
        print(f"type: {type(e).__name__}")
        print(f"message: {e.args[0]}")
        before_exit()
        input("Press a key to close...")

