#!/bin/bash

clear

echo process starting...

cd ./sources/

cd ./rtCam/
echo Start rtCam.out
./rtCam.out

cd ../automate/
echo Start automate.py
python3 automate.py -g 32
