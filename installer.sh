#!/bin/bash

clear

# Check if the user is sudo
if [ "$EUID" -ne 0 ]; then
  echo "This script required sudo rights"
  exit 1
fi

echo ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤ DUAV2023 installer ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤

echo " "
echo ==================================== updating ====================================
echo " "
sudo apt update -y
echo " "
echo =============================== update - finished ================================
echo " "

echo " "
echo ==================================== upgrading ===================================
echo " "
sudo apt upgrade -y
echo " "
echo =============================== upgrade - finished ===============================
echo " "

echo " "
echo ======================== installing cmake g++ wget unzip =========================
echo " "
sudo apt-get install -y cmake g++ wget unzip

echo " "
echo ==================== installing build-essential libgtk2.0-dev ====================
echo " "
sudo apt-get install -y build-essential libgtk2.0-dev

echo " "
echo ============ installing libavcodec-dev libavformat-dev libswscale-dev ============
echo " "
sudo apt-get install -y libavcodec-dev libavformat-dev libswscale-dev

echo " "
echo =================== installing git libyaml-cpp-dev python3-pip ===================
echo " "
sudo apt-get install -y git libyaml-cpp-dev python3-pip

echo " "
echo ============= installing net-tools ssh openssh-server libprocps-dev ==============
echo " "
sudo apt-get install -y net-tools ssh openssh-server libprocps-dev

echo " "
echo ========================== installing libboost-all-dev ===========================
echo " "
sudo apt-get install -y libboost-all-dev

echo " "
echo ================= installing libopencv-dev python3-opencv psutil =================
echo " "
sudo apt-get install -y libopencv-dev python3-opencv psutil

echo " "
echo ============================ installing raspi-config =============================
echo " "
sudo apt-get install -y raspi-config

echo " "
echo ============================ pip3 installing RPi.GPIO ============================
echo " "
sudo pip3 install RPi.GPIO

echo " "
echo =============================== install - finished ===============================
echo " "

echo " "
echo ==================================== versions ====================================
echo " "
echo opencv:
pkg-config --modversion opencv4
echo git:
git --version
echo pip:
pip3 --version

echo " "
echo ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤ DUAV2023 installer done ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
