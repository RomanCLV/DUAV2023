#!/bin/bash

clear
cd ./sources/

cd ./rtCam/
file=rtCam.out
echo Start $file
./$file

cd ../automate/
file=automate.py
echo Start $file
python3 $file
