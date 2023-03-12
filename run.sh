#!/bin/bash

clear

# Path of files
path_main="./sources/rtCam/cpp/build/main.out"
# path_main="./sources/rtCam/python/main.py"
path_automate="./sources/automate/automate.py"

# Check if the user is sudo
if [ "$EUID" -ne 0 ]; then
  echo "Error: This script required sudo rights"
  exit 1
fi

# Check files exist
if [ ! -f "$path_main" ]; then
  echo "Error : folder not found:" $path_main
  exit 1
fi

if [ ! -f "$path_automate" ]; then
  echo "Error : file not found:" $path_automate
  exit 1
fi

# Lancer les programmes dans une nouvelle console

echo Starting: main.out
gnome-terminal -- sh -c "$path_main" &
# gnome-terminal -- sh -c "python3 $path_main" &

echo Starting: automate.py 32 33
gnome-terminal -- sh -c "sudo python3 $path_automate 32 33" &

# Waiting for the end of the execution of the two programs
wait
