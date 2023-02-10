# DUAV2023

## Install OpenCV on Linux

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

## Clone the project and configuration

If `git` is not installed:
```
sudo apt-get install git
git --version
```

You can now clone the project:

```
git clone https://github.com/RomanCLV/DUAV2023.git
cd ./DUAV2023/
```

## Python Automate

Python3.7 (or >=)

Module to control Servo: RPi.GPIO 0.7.1 

### Install Python librairies

If `pip3` is not installed:

```
sudo apt install python3-pip
pip3 --version
```

Now you can install RPi.GPIO 

```
sudo pip3 install RPi.GPIO
```

https://pypi.org/project/RPi.GPIO/

## GPIO configuration
You can use execute the python script `./sources/automate/test_servo.py` to verify that the communication between the Raspberry and the Servo works.
You have to specify the GPIO used.
You must be root to run the scipts. You `sudo` before the command if needed.

Example :

```
python3 test_servo.py -g 32
``` 
or
```
python3 test_servo.py --gpio 32
```

will define that PIN 32 (GPIO 12) is used.
See https: https://github.com/RomanCLV/DUAV2023/blob/main/gpio.jpg to know which PIN corresponds to which GPIO.
YOU MUST SELECT A PWM GPIO!

If several servos are used, no problem:

```
python3 test_servo.py -g 32 33
```

to use GPIOs 12 and 13.

---

## Possible error for bash files
About bash files, if the `\r command not found` error occures, please execute:

```
sed -i 's/\r$//' <filename>
```

It will remove the `\r` character.

---

## Compute C++ file

The required CPP script you must compute is located at `./sources/rtCam/`
All others script are samples to learn how to use OpenCV.
Nevertheless, script `./sources/opencv-samples/5_CameraCapture` can be usefull to check if the camera is well configured. (See rtCam section)

We are using `gcc` and `cmake` to build cpp files. So to build any cpp file, move into the folder where are located the main.cpp and execute:

```
mkdir build && cd build
cmake ..
make
```

You can now run the programm with

```
./main.out
```

(Check the README.md of each file to know how to use it)

---

## rtCam

To check if the camera is well configured, you can build the `./sources/opencv-samples/5_CameraCapture` sample and run it.
If a window is opened with your camera stream inside, all is working.

If not, they are many possibilities:
- Embedded video playback halted; module v4l2src0 reported: Failed to allocate required memory.
- Device not detected...

Here are some links we used to fix that. Good luck :)

### Raspberry Pi OS / Raspbian

First, check if camera works with the `raspistill` (or other built-in programm).

Can be usefull: https://www.xmodulo.com/install-raspberry-pi-camera-board.html

### Others OS (Ubunto, ...)

https://stackoverflow.com/questions/29583533/videocapture-open0-wont-recognize-pi-cam
https://wesleych3n.medium.com/enable-camera-in-raspberry-pi-4-with-64-bit-ubuntu-21-04-d97ce728db9d

---

## Ready to run

Now go back to the DUAV2023 folder and use:

```
chmod 755 run.sh
```

You can modify `run.sh` to change your GPIO (line 15) to suit your use.

Launch the script with 
```
bash run.sh
```
or
```
run.sh
```
