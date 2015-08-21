# Install script for TTS application
# Writen by Ho Tuan Vu - July 23, 2015

#!/bin/bash
echo Building TTS application...
cd build/
make clean
make
cd ..
echo Finished

