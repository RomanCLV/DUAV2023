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

        log_file = "logs/automate_time.log"
        
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
global can_open
global is_opened
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
    global can_open
    global is_opened
    global closed_tank_position
    global opened_tank_position
    global logger

    run = True

    request = STATE.INIT
    state = STATE.DO_NOTHING

    can_open = False
    is_opened = False
    closed_tank_position = 0
    opened_tank_position = 45

    start_time = 0
    last_time_state_changed = 0

    servos = []

    logger = Logger()

    print("current folder:", os.getcwd())

    RTH_PATH = "./RTH"

    if not os.path.exists(args.file):
        raise Exception(f"File {args.file} not found")

    actions = read_file(args.file)

    if len(actions) == 0:
        raise Exception(f"No actions")

    #print(f"actions: {len(actions)}")
    #for action in actions:
    #    print(f"{action[0]} - {action[1]}")

    if args.gpio and len(args.gpio) > 0:
        for gpio in args.gpio:
            servo = serv.Servo(abs(int(gpio)), 50, gpio[0] == '-')  # pin, frequency
            servo.add_log_listener(logger)
            servos.append(servo)
    else:
        raise Exception("Require at least one GPIO")

    remove_rth_file()

    start_time = get_millis()
    last_time_state_changed = start_time

    action_index = 0
    action = ""

    while run and action_index < len(actions):

        delay = (get_millis() - start_time) / 1000

        if delay >= actions[action_index][0]:
            action = actions[action_index][1]
            action_index += 1

            if action == "init":
                request = STATE.INIT

            elif action == "exploration":
                request = STATE.EXPLORATION

            elif action == "mission":
                request = STATE.MISSION

            elif action == "rth":
                request = STATE.RETURN_TO_HOME

            elif action == "do_nothing":
                request = STATE.DO_NOTHING

            elif action == "shutdown":
                request = STATE.SHUTDOWN

            elif action == "open":
                can_open = True

            elif action == "close":
                can_open = False

        automate_request()
        automate_state()
        
        if not run or state == STATE.SHUTDOWN:
            break
    
    before_exit()
    log("Automate exit")


def read_file(filename):
    result = []
    valid_words = [
        "init",
        "exploration",
        "mission",
        "rth",
        "do_nothing",
        "shutdown",
        "open",
        "close"
    ]

    with open(filename, 'r') as file:
        for line in file:
            splited_line = line.strip().split(" ")
            time_str = splited_line[0]
            word = " ".join(splited_line[1:])

            if word not in valid_words:
                log(f"invalid action: {word}")
                print("valid actions:\n")
                for valid_word in valid_words:
                    print(valid_word)
                return []

            try:
                hh, mm, ss = map(int, time_str.split(":"))
                total_seconds = hh * 3600 + mm * 60 + ss
            except ValueError:
                print(f"format error of line : {line.strip()}")
                print("expected format: hh:mm:ss action")
                return []

            result.append((total_seconds, word))

    return result


def set_state(new_state):
    global state
    global last_time_state_changed
    state = new_state
    last_time_state_changed = get_millis()


def automate_request():
    global run
    global state
    global request

    if request == state:
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
    global servos
    global can_open
    global is_opened
    global closed_tank_position
    global opened_tank_position

    if state == STATE.INIT:
        for servo in servos:
            log(f"{servo.get_name()} starting...")

            try:
                servo.start(opened_tank_position if servo.get_is_inversed() else closed_tank_position)
            except RuntimeError as err:
                before_exit()
                raise err                

        log("Automate initialized")
        request = STATE.DO_NOTHING
    
    elif state == STATE.EXPLORATION:
        close_tanks()
        if can_open:
            if rth_file_exists():
                log("RTH file detected!")
                request = STATE.RETURN_TO_HOME
    
    elif state == STATE.MISSION:
        if can_open:
            if rth_file_exists():
                close_tanks()
                log("RTH file detected!")
                request = STATE.RETURN_TO_HOME

            elif not is_opened:
                open_tanks()
        else:
            if is_opened:
                close_tanks()

    elif state == STATE.RETURN_TO_HOME:
        close_tanks()
        request = STATE.DO_NOTHING

    elif state == STATE.DO_NOTHING:
        close_tanks()
        if (get_millis() - last_time_state_changed) > 120000:
            # after 2 minutes

            print("2 mins")
            request = STATE.SHUTDOWN

    elif state == STATE.SHUTDOWN:
        log("Shutting down...")
        before_exit()
        remove_rth_file()

    else:
        before_exit()
        log(f"Unexpected state value: {state}")
        run = False


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
    parser.add_argument("file", type=str, help="file that contains the times with action")
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

