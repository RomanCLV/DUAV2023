#!/bin/bash

clear

# Chemins des fichiers
path_main="./sources/rtCam/cpp/build/main.out"
path_automate="./sources/automate/automate.py"

# Check if the user is sudo
if [ "$EUID" -ne 0 ]; then
  echo "This script required sudo rights"
  exit 1
fi

# Check files exist
if [ ! -f "$chemin_main" ]; then
  echo "Error : file not found: " $chemin_main
  exit 1
fi

if [ ! -f "$chemin_automate" ]; then
  echo "Error : file not found: " $chemin_automate
  exit 1
fi

# Lancer les programmes dans une nouvelle console
echo Starting: "$chemin_automate"
gnome-terminal -- "$chemin_main"

echo Starting: python3 "$chemin_automate" ../rtCam/cpp/build/RTH 32 33
gnome-terminal -- sudo python3 "$chemin_automate" ../rtCam/cpp/build/RTH 32 33
