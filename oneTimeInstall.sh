pip3 install pyserial
pip3 install python3_xlib
sudo cp piUserInterface.service /etc/systemd/system/piUserInterface.service
sudo systemctl enable piUserInterface.service
sudo systemctl start  piUserInterface.service
#reboot
