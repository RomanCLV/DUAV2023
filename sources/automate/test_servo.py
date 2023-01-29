#-- coding: utf-8 --

import time
import servo
import signal
import sys


def sigint_handler(signal, frame):
    close_servo()
    sys.exit(0)
    

def close_servo():
    print("close")
    servo.close()


signal.signal(signal.SIGINT, sigint_handler)

print("\nTest-Servo\n")

print("init on GPIO 12")
servo.init(32, 50)

print("Start to 0")
servo.start(0)
print("Started")

for i in range(3, 0, -1):
    print(i)
    time.sleep(1)
    
print("Run loop 3 times:")

delta = int(180 / 10)
pause = 0.5

for loop in range(1, 4):
    deg = 0
    
    print(f"\nLoop {loop}")
    while deg <= 180:
        print(f"Move: {deg}")
        servo.move(deg)
        deg = deg + delta
        time.sleep(pause)
        
    print(f"\nLoop {loop} reversed")
    deg = 180
    while deg >= 0:
        print(f"Move: {deg}")
        servo.move(deg)
        deg = deg - delta
        time.sleep(pause)
    print("\n")

close_servo()
