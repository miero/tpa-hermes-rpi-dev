### execute all as root
```
sudo su
```
### (first time only) install required packages
```
apt-get install raspberrypi-kernel-headers gcc make device-tree-compiler git
```
### (first time only) download Hermes-RPi driver sources
git clone https://github.com/miero/tpa-hermes-rpi-dev.git

### enter driver directory
```
cd tpa-hermes-rpi-dev
```
### (upgrade only) download current Hermes-RPi driver sources
```
git pull
```
### build and install modules and overlay file from the tpa-hermes-rpi-dev directory
```
make clean
make install
depmod -A
```
### (first time only on most RPi distros) append loading tpa-hermes-rpi overlay into config.txt
```
echo "dtoverlay=tpa-hermes-rpi" >> /boot/config.txt
```
### (first time only on DietPi) append loading tpa-hermes-rpi overlay into config.txt
```
echo "dtoverlay=tpa-hermes-rpi" >> /DietPi/config.txt
/DietPi/dietpi/func/dietpi-set_hardware soundcard hw:sndtpahermesrpi,0
```
### reboot
```
reboot
```
### verify that the driver is loaded
```
aplay -l
```
output of aplay command should contain sndtpahermesrpi device
```text
**** List of PLAYBACK Hardware Devices ****
...
card 1: sndtpahermesrpi [snd_tpa_hermes_rpi], device 0: TPA-Hermes-RPi HiFi tpa-hermes-rpi-dai-0 []
 Subdevices: 1/1
 Subdevice #0: subdevice #0
```
