### execute all as root
sudo su

### install required packages
apt-get install raspberrypi-kernel-headers gcc make device-tree-compiler

### build and install modules and overal from the tpa-hermes-rpi-dev directory
make
make install

### (only once) append loading tpa-hermes-rpi overlay into config.txt
echo "dtoverlay=tpa-hermes-rpi" >> /boot/config.txt

### reboot
