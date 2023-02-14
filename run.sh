#!/bin/bash

clear

echo process starting...

cd ./sources/

cd ./rtCam/
echo Start ./rtCam.out
./rtCam.out

cd ../automate/
echo Start automate.py $1 $2
sudo python3 automate.py $1 $2
