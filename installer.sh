#!/bin/bash

clear

# Check if the user is sudo
if [ "$EUID" -ne 0 ]; then
  echo "This script required sudo rights"
  exit 1
fi

echo installing requirements for OpenCV...

echo apt update
sudo apt update
echo " "

echo apt upgrade
sudo apt upgrade
echo " "

echo apt install -y cmake g++ wget unzip
sudo apt install -y cmake g++ wget unzip
echo " "

echo apt install libopencv-dev python3-opencv
sudo apt install libopencv-dev python3-opencv
pkg-config --modversion opencv4
echo " "

echo installing git
sudo apt-get install git
git --version
echo " "

echo apt install python3-pip
sudo apt install python3-pip
pip3 --version
echo " "

echo pip3 install RPi.GPIO
sudo pip3 install RPi.GPIO
echo " "

echo apt-get install libyaml-cpp-dev
sudo apt-get install libyaml-cpp-dev
echo " "

echo done
