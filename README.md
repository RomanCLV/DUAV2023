# DUAV2023

## Install OpenCV on Linux

Make sure that `Preferences>Configuration>Performance : GPU memory=256`

### Installation

### Ubuntu

https://linuxhint.com/install-opencv-ubuntu/

```
sudo apt update
sudo apt upgrade
sudo apt install -y cmake g++ wget unzip
sudo apt install libopencv-dev python3-opencv
pkg-config --modversion opencv4
```

### Others: (Raspbian, Raspberry Pi OS, ...)
Good luck :)

Links that may be useful to you:

Official OpenCV documentation: https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html

Install OpenCV 4.0.0 : https://robu.in/installing-opencv-using-cmake-in-raspberry-pi/

(Last command is `make` and not `Make`)

Or watch this video:

https://www.youtube.com/watch?v=rdBTLOx0gi4   (The guy follows the previous guide)

---

You can now clone the project:

`git clone https://github.com/RomanCLV/DUAV2023.git`

`cd ./DUAV2023/`

## Python Automate

Python3.7 (or >=)

Module to control Servo: RPi.GPIO 0.7.1 

### Install Python librairies

`sudo pip3 install RPi.GPIO`

https://pypi.org/project/RPi.GPIO/

## GPIO configuration
You can use execute the python script `./sources/automate/test_servo.py` to verify that the communication between the Raspberry and the Servo works.
You have to specify the GPIO used.

Example :

`python3 test_servo.py -g 32` or `python3 test_servo.py --gpio 32`

will define that PIN 32 (GPIO 12) is used.
See https: https://github.com/RomanCLV/DUAV2023/blob/main/gpio.jpg to know which PIN corresponds to which GPIO.
YOU MUST SELECT A PWM GPIO!

If several servos are used, no problem:

`python3 test_servo.py -g 32 33`

to use GPIOs 12 and 13.

---

## Possible error for bash files
About bash files, if the `\r command not found` error occures, please execute:

`sed -i 's/\r$//' <filename>`

It will remove the `\r` character.

---

## Compute C++ file

The required CPP script you must compute is located at `./sources/rtCam/`
All others script are samples to learn how to use OpenCV.

We are using `cmake` to build cpp files. So to build any cpp file, move into the folder where are located the main.cpp and execute:

```
cmake .
make
```

---

## Ready to run

Now go back to the DUAV2023 folder and use:

`chmod 755 run.sh`

You can modify `run.sh` to change your GPIO (line 15) to suit your use.

Launch the script with `bash run.sh` or `run.sh`
