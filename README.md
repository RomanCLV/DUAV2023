# DUAV2023

## Possible error ofr bash file
About bash files, if the '\r command not found' error occures, please execute:

'sed -i 's/\r$//' <filename>'

It will remove the '\r' character.

## Compute C++ file

Then, compute the c++ file:
'cd ./sources/rtCam/'
'chmod 755 compile.sh'
And execute the script:
'bash compile.sh' or 'compile.sh'

## Ready to run

Now go back to the root folder.
'cd ../..'
'chmod 755 run.sh'

Launch the script with 'run.sh' or 'bash run.sh'
