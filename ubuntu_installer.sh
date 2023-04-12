#!/bin/bash

clear

# Check if the user is sudo
if [ "$EUID" -ne 0 ]; then
  echo "This script required sudo rights"
  exit 1
fi

echo ¤¤¤ DUAV2023 Ubuntu installer ¤¤¤

echo " "
echo =================== updating ===================
echo " "
sudo apt update
echo " "
echo =================== update - finished ===================
echo " "

echo " "
echo =================== upgrading ===================
echo " "
sudo apt upgrade
echo " "
echo =================== upgrade - finished ===================
echo " "

echo " "
echo =================== installing cmake g++ wget unzip ===================
echo " "
sudo apt install -y cmake g++ wget unzip

echo " "
echo =================== installing git libyaml-cpp-dev python3-pip ===================
sudo apt-get install git
sudo apt-get install libyaml-cpp-dev
sudo apt install python3-pip

echo " "
echo =================== installing net-tools openssh-server ===================
echo " "
sudo apt install net-tools
sudo apt-get install openssh-server

echo " "
echo =================== installing libopencv-dev python3-opencv ===================
echo " "
sudo apt install libopencv-dev python3-opencv

echo " "
echo =================== installing raspi-config ===================
echo " "
sudo apt-get install raspi-config

echo " "
echo =================== pip3 installing RPi.GPIO ===================
echo " "
sudo pip3 install RPi.GPIO


echo " "
echo =================== install - finished ===================
echo " "


echo " "
echo =================== versions ===================
echo " "
echo opencv:
pkg-config --modversion opencv4
echo git:
git --version
echo pip:
pip3 --version

echo ¤¤¤ DUAV2023 Ubuntu installer done ¤¤¤
