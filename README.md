# DUAV2023

## Clone the project

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

---

## Quick install for Ubuntu

It will execute all commands in the following manual installation guide (install opencv for Ubuntu, required libraries for the python automate.py and rtCam.cpp, and also required package like git, pip, ssh-server, ...)

Run the `ubuntu_installer.sh`:

```
sudo bash ubuntu_installer.sh
```

Please have a look to the section `GPIO configuration`.

---

## Manually install OpenCV on Linux

### Installation

### Ubuntu

https://linuxhint.com/install-opencv-ubuntu/

```
sudo apt update
sudo apt upgrade
sudo apt-get install -y cmake g++ wget unzip
sudo apt-get install -y build-essential libgtk2.0-dev
sudo apt-get install -y libavcodec-dev libavformat-dev libswscale-dev
sudo apt-get install -y libopencv-dev python3-opencv
sudo apt-get install -y libboost-all-dev

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

## Python Automate

Python3.7 (or >=)

Module to control Servo: RPi.GPIO 0.7.1 

`RPi.GPIO` requires to be root (use `sudo` to run the python scripts).

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
You have to specify the GPIOs used.

Examples:

```
python3 test_servo.py 32        // to use PIN 32 (GPIO 12)

python3 test_servo.py 32 33     // to use GPIOs 12 and 13

python3 test_servo.py 32 33 -a  // to use GPIOs 12 and 13 and test at the same time
```

See https: https://github.com/RomanCLV/DUAV2023/blob/main/gpio.jpg to know which PIN corresponds to which GPIO.
YOU MUST SELECT A PWM GPIO!

If you declared several servos and want to be tested at the same time, add `-a` or `--all`.

`python3 test_servo.py -h` to know more options.

---

## Possible error for bash files

If you edit a bash file, be warn to the break line type:
- `CR LF` (Windows)
- `LF` (Unix) 
- `CR` (Macintosh)

Make sure that `LF` type is selected. Else you will get a `\r command not found` error occures. In this case, please execute:

```
sed -i 's/\r$//' <filename>
```
It will remove the `\r` character.

https://stackoverflow.com/questions/2613800/how-to-convert-dos-windows-newline-crlf-to-unix-newline-lf

---

## Compute C++ file

The required CPP script you must compute is located at `./sources/rtCam/`
All others script are samples to learn how to use OpenCV.
Nevertheless, script `./sources/opencv-samples/6_CameraCapture` can be usefull to check if the camera is well configured. (See rtCam section)

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

(Check the README.md of each sample to know how to use it)

---

## rtCam

To check if the camera is well configured, you can build the `./sources/opencv-samples/6_CameraCapture` sample and run it.
If a window is opened with your camera stream inside, all is working.

If it's not, read the `CameraError.md` and good luck :)

You will also need the `libyaml-cpp-dev` package.

Check if you have it:
```
dpkg -s libyaml-cpp-dev
```
If not, install it:
```
sudo apt-get install libyaml-cpp-dev
```

---

# Communication with the PixHawk4

MavLink protocol is used. We use `dronekit` python module to simplify the communication.

`dronekit` works on `Python3.8-`. If you use a higher version, it will not works because some code had been removed in the newer Pythons version.

To see your version:
```
python3 --version
```

To install Python3.7 (also works for Python3.8):

```
sudo apt-get install python3.7
sudo apt-get install python3.7-venv python3.7-dev
```

If you got the error `Package 'python3.7' has no installation candidate`, try this :

```
sudo apt install software-properties-common
sudo add-apt-repository ppa:deadsnakes/ppa
sudo apt-get install python3.7-venv python3.7-dev
```

Now you have two vesions of python.

```
python3 --version
python3.7 --version
```

That's not all, if you use `pip`, it will install in your default Python version. You have to create a new environment for this verion of Python.

```
python3.7 -m pip install [package]
```

Install `mavproxy`, `mavlink` and `dronekit` (in the python3.7 venv):

```
sudo python3.7 -m pip install mavproxy pymavlink dronekit dronekit-sitl
```

---

## QgroundControl

https://github.com/mavlink/qgroundcontrol/releases/tag/v4.1.5

In a terminal tab run :

```
dronekit-sitl [model] 
```

You can find out your models with:

```
dronekit-sitl --list 
```

For this example we're going to use the `copter` model, so we'll run :

```
dronekit-sitl copter
```

To simulate with QGroundControl, you can configure QGroundControl to open a udp/tcp port and then you can open a seconde terminal and run:

```
mavproxy.py --master tcp:127.0.0.1:5760 --out 127.0.0.1:14550
```

Don't forget to change the port.

You dronekit documentation (https://dronekit-python.readthedocs.io/en/latest/) to know how to build a script to connect / simulate your UAV.

---

# Ready to run (with sudo)

Now go back to the DUAV2023 folder and use:

```
chmod 755 run.sh
```

You can modify `run.sh` to change your GPIO (line 34) to suit your use.
And also to change the automate you want to use. (see in `sources/automate`)

Launch the script with 
```
sudo bash run.sh
```
or
```
sudo run.sh
```
