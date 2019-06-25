#!/bin/bash
#====================#
#=  moos-ivp-jetyak =#
#====================#
cd ~/moos-ivp-jetyak/
git checkout builds-raspi
./build.sh
echo 'export PATH=$PATH:~/moos-ivp-bluerov/bin'  >> ~/.bashrc

#====================#
#=	ardupilot 	    =#
#====================#
cd ./ardupilot
git submodule update --init --recursive
sudo apt-get install python-matplotlib python-serial python-wxgtk3.0 python-wxtools python-lxml
sudo apt-get install python-scipy python-opencv ccache gawk git python-pip python-pexpect
sudo pip install future pymavlink MAVProxy
echo 'export PATH=$PATH:$HOME/ardupilot/Tools/autotest' >> ~/.bashrc
echo 'export PATH=/usr/lib/ccache:$PATH' >> ~/.bashrc 
source ~/.bashrc