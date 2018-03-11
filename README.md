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
reboot

### verify that the driver is loaded

$ aplay -l
**** List of PLAYBACK Hardware Devices ****
...
card 1: sndtpahermesrpi [snd_tpa_hermes_rpi], device 0: TPA-Hermes-RPi HiFi tpa-hermes-rpi-dai-0 []
  Subdevices: 1/1
  Subdevice #0: subdevice #0

