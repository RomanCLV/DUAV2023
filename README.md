# DUAV2023

## Install OpenCV on Linux

Make sure that `Preferences>Configuration>Performance : GPU memory=256`


## Install OpenCV-4.x version (failed on a Raspberry3, didn't test on a Raspberry 4)

### Installation

Follow this guide :

https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html

### Warning
- Configure cmake takes few minutes.
- The build process takes a lot of time (from 4 to 7 hours on a RaspberryPi 3, probably less on a newer RaspberryPi)!
- If build failed, check this following link:

https://forum.opencv.org/t/not-able-to-build-opencv-from-source/1542

https://docs.opencv.org/4.x/d2/de6/tutorial_py_setup_in_ubuntu.html

## Install OpenCV-4.0.0 (failed on a Raspberry3, successful on a Raspberry 4)

Follow this guide :

https://robu.in/installing-opencv-using-cmake-in-raspberry-pi/

(Last command is `make` and not `Make`)

Or watch this video:

https://www.youtube.com/watch?v=rdBTLOx0gi4   (The guy follows the previous guide)

### Warning
- Configure cmake takes few minutes.
- The build process takes some time (at least 1h on a RaspberryPi 3, 30 minutes on a RaspberryPi 4)!

---

You can now clone the project:

`git clone https://github.com/RomanCLV/DUAV2023.git`

`cd ./DUAV2023/`


## Python Automate

Python3.7 is used.

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

Go to the `./sources/rtCam/` folder.

`chmod 755 compile.sh`

And execute the script: `bash compile.sh` or `compile.sh`

---

## Ready to run

Now go back to the DUAV2023 folder and use:

`chmod 755 run.sh`

You can modify `run.sh` to change your GPIO (line 15) to suit your use.

Launch the script with `bash run.sh` or `run.sh`
