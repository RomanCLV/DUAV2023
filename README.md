# DUAV2023

Clone the project: `git clone https://github.com/RomanCLV/DUAV2023.git`

## Install OpenCV on Linux

The following lines come from:
https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html

Move to the rtCam folder:
`cd sources/rtCam/`

### Install minimal prerequisites (Ubuntu 18.04 as reference)
`sudo apt update && sudo apt install -y cmake g++ wget unzip`

### Download and unpack sources
`wget -O opencv.zip https://github.com/opencv/opencv/archive/4.x.zip`

`unzip opencv.zip`

### Create build directory
`mkdir -p build && cd build`

### Configure
`cmake  ../opencv-4.x`

### Build
Warning: The build process takes a lot of time (more than an hour, maybe 2 or 3).

`cmake --build .`

## Possible error for bash file
About bash files, if the `\r command not found` error occures, please execute:

`sed -i 's/\r$//' <filename>`

It will remove the `\r` character.

## Compute C++ file

Go back to the `rtCam` folder: `cd ..`

`chmod 755 compile.sh`

And execute the script: `bash compile.sh` or `compile.sh`

## Ready to run

Now go back to the root folder: `cd ../..`

`chmod 755 run.sh`

Launch the script with `run.sh` or `bash run.sh`
