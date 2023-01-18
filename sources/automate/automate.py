
from enum import Enum
import os
import time
import datetime


class STATE(Enum):
    INIT = 0
    DO_NOTHING = 1
    EXPLORATION = 2
    MISSION = 3
    RETURN_TO_HOME = 4
    SHUTDOWN = 5


global request
global state
global start_time
global last_time_state_changed
global has_to_open
global has_to_close
global is_opened


def get_millis():
    return round(time.time() * 1000)


def log(o):
    global start_time
    delay = datetime.datetime.fromtimestamp((get_millis() - start_time) / 1000.0)
    hour = delay.hour - 1
    minute = delay.minute
    second = delay.second
    milli = round(delay.microsecond / 1000)
    print(f"{hour:02d}:{minute:02d}:{second:02d}.{milli:03d} : {o}")


def rth_file_exists():
    return os.path.exists("RTH")


def remove_rth_file():
    if rth_file_exists():
        os.remove("RTH")


def main():
    global request
    global state
    global start_time
    global last_time_state_changed
    global has_to_open
    global has_to_close
    global is_opened

    request = STATE.INIT
    state = STATE.INIT

    has_to_open = False
    has_to_close = False
    is_opened = False

    start_time = 0
    last_time_state_changed = 0

    remove_rth_file()
    while True:
        # read GPS signal
        time.sleep(0.100)    # delete after
        automate_request()
        automate_state()
        
        if state == STATE.SHUTDOWN:
            break
        
    log("Automate exit")


def set_state(new_state):
    global state
    global last_time_state_changed
    state = new_state
    last_time_state_changed = get_millis()


def automate_request():
    global request
    global state

    if request == state and request != STATE.INIT:
        return
    
    if request == STATE.INIT:
        set_state(STATE.INIT)
        
    elif request == STATE.EXPLORATION:
        log("Ready to exploration")
        set_state(STATE.EXPLORATION)

    elif request == STATE.MISSION:
        log("Ready to mission")
        set_state(STATE.MISSION)

    elif request == STATE.DO_NOTHING:
        log("Do nothing engaged")
        set_state(STATE.DO_NOTHING)

    elif request == STATE.RETURN_TO_HOME:
        log("Return to home engaged")
        set_state(STATE.RETURN_TO_HOME)

    elif request == STATE.SHUTDOWN:
        set_state(STATE.SHUTDOWN)

    else:
        raise ValueError(f"Unexpected request value: {request}")


def automate_state():
    global state
    global request
    global start_time
    global last_time_state_changed

    delay = get_millis() - last_time_state_changed

    if state == STATE.INIT:
        start_time = get_millis()
        log("Automate initialized")
        request = STATE.EXPLORATION
        
    elif state == STATE.EXPLORATION:
        if rth_file_exists():
            log("RTH file detected!")
            request = STATE.RETURN_TO_HOME
            return

        if delay > 5000:
            log("Exploration done")
            request = STATE.MISSION
    
    elif state == STATE.MISSION:
        if rth_file_exists():
            log("RTH file detected!")
            request = STATE.RETURN_TO_HOME
            return

        if delay > 15000:
            log("Mission done")
            request = STATE.DO_NOTHING

    elif state == STATE.RETURN_TO_HOME:
        if delay > 5000:
            log("Return to home done")
            request = STATE.DO_NOTHING

    elif state == STATE.DO_NOTHING:
        if delay > 3000:
            request = STATE.SHUTDOWN

    elif state == STATE.SHUTDOWN:
        log("Shutting down...")
        remove_rth_file()

    else:
        raise ValueError(f"Unexpected state value: {state}")


def is_near_to():
    return False


if __name__ == "__main__":
    main()

