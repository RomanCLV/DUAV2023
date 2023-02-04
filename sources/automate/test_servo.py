# -*- coding: utf-8 -*-
"""
Test servo module
Copyright ©2023 ESTI'AERO
By Roman Clavier

Module used to test the communication between a Raspberry and one (or several) Servo.
"""

import time
import servo as serv
import signal
import sys
import argparse

global all_servos


def sigint_handler(signal, frame):
    close_all_servo()
    sys.exit(0)
    

def main():
    global all_servos

    all_servos = []
    args = parser.parse_args()

    if args.gpio and len(args.gpio) > 0:
        test(args.gpio, args.all)
    else:
        print("Require at least one GPIO. Use -g or --gpio to set GPIO.")


def wait(sec):
    for i in range(sec, 0, -1):
        print(i)
        time.sleep(1)


def test(gpios, all_servo):
    print("\nTest-Servo\n")
    if all_servo:
        test_all(gpios)
    else:
        for gpio in gpios:
            test_one(gpio)
            serv.GPIO_cleanup()
    print("\nTest done successfully\n")


def test_one(gpio):
    servo = init_servo(gpio)
    start_servo(servo)
    wait(3)
    loop_servo()
    close_servo(servo)


def test_all(gpios):
    for gpio in gpios:
        init_servo(gpio)
    for servo in all_servos:
        start_servo(servo)
    wait(3)
    loop_servo()
    close_all_servo()


def init_servo(gpio):
    print(f"\nInit servo on GPIO {gpio}")
    servo = serv.Servo(gpio, 50)
    all_servos.append(servo)
    return servo


def start_servo(servo):
    servo.start(0)
    print(f"{servo.get_name()} started to 0°")


def loop_servo():
    global all_servos

    max_loop = 2
    print(f"Run loop {max_loop} times:")

    delta = int(180 / 10)
    pause = 0.5

    for loop in range(0, max_loop):
        deg = 0

        print(f"\nLoop {loop + 1}")
        while deg <= 180:
            print(f"Move: {deg}")

            for servo in all_servos:
                servo.move(deg)

            deg = deg + delta
            time.sleep(pause)

        print(f"\nLoop {loop + 1} reversed")
        deg = 180
        while deg >= 0:
            print(f"Move: {deg}")
            for servo in all_servos:
                servo.move(deg)

            deg = deg - delta
            time.sleep(pause)
        print("\n")


def close_all_servo():
    global all_servos
    while len(all_servos) > 0:
        close_servo(all_servos[-1])


def close_servo(servo):
    global all_servos
    print(f"{servo.get_name()} closing...")
    servo.close()
    all_servos.remove(servo)


if __name__ == '__main__':
    signal.signal(signal.SIGINT, sigint_handler)
    parser = argparse.ArgumentParser(description="test_servo.py CLI")
    parser.add_argument("-g", "--gpio", type=int, nargs="+", help="set the communication pin to test.")
    parser.add_argument("-a", "--all", action="store_true",
                        help="set that all servos are tested simultaneously (if several gpio are set). "
                             "By default, servos are tested one by one.")
    main()
