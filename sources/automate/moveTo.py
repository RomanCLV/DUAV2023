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
import argparse

global all_servos


def sigint_handler(signal, frame):
    close_all_servo()
    exit(0)
    

def main():
    global all_servos

    all_servos = []
    args = parser.parse_args()

    if args.gpio and len(args.gpio) > 0:
        for gpio in args.gpio:
            servo = init_servo(gpio)
            start_servo(servo)
            servo.move(0)
            print(f"{servo.get_name()} moving to: 0")
            time.sleep(1)
            if args.angle:
                servo.move((int)args.angle)
                print(f"{servo.get_name()} moving to: {args.angle}")
            time.sleep(1)

    close_all_servo()


def init_servo(gpio):
    global all_servos
    print(f"\nInit servo on GPIO {gpio}")
    servo = serv.Servo(gpio, 50)
    all_servos.append(servo)
    return servo


def start_servo(servo, position=0.0):
    try:
        servo.start(position)
    except RuntimeError as err:
        print(str(err))
        close_all_servo()
        exit(-1)


    print(f"{servo.get_name()} started to 0°")


def close_all_servo():
    global all_servos
    while len(all_servos) > 0:
        close_servo(all_servos[-1])


def close_servo(servo):
    global all_servos
    print(f"{servo.get_name()} closing...")
    servo.close()
    print(f"{servo.get_name()} closed")
    all_servos.remove(servo)


if __name__ == '__main__':
    signal.signal(signal.SIGINT, sigint_handler)
    parser = argparse.ArgumentParser(description="test_servo.py CLI")
    parser.add_argument("gpio", type=int, nargs="+", help="set the communication GPIO PINs to test.")
    parser.add_argument("-a", "--angle", help="angle to move")
    main()
