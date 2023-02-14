# Camera Error

There are many sources of error:
- device not found or can't be opened
- every frames are empty

---

First of all, make sure you enabled the `Legacy Camera` in the `raspi-config`.
```
sudo raspi-config
```
If `raspi-config` is not a command, install it:
```
sudo apt install raspi-config
```
Then: `raspi-config > Interface Options > Legacy Camera`

https://www.xmodulo.com/install-raspberry-pi-camera-board.html

---

Raspberry Pi OS / Rasbian: check if camera works with `raspistill` or `libcamera`

---

Check if `/boot/config.txt` or `/boot/firmware/config.txt` contains:
```
start_x=1
gpu_mem=128 (can be 256)
camera_auto_detect=1
display_auto_detect=1
```

https://wesleych3n.medium.com/enable-camera-in-raspberry-pi-4-with-64-bit-ubuntu-21-04-d97ce728db9d

---

Check if `/dev/video0` exists.

Get a list of video capture devices:
```
v4l2-ctl --list-devices
```
In order to use the above command, you must install package v4l-utils before. 
```
sudo apt-get install v4l-utils
```

Can also use:
```
sudo ls -l /dev/video*
```

https://stackoverflow.com/questions/4290834/how-to-get-a-list-of-video-capture-devices-web-cameras-on-linux-ubuntu-c

---

Check if bcm2835-v4l2 works:
```
sudo modprobe bcm2835-v4l2
```

https://stackoverflow.com/questions/29583533/videocapture-open0-wont-recognize-pi-cam

---

Check v4l2 is loaded automatically. Edit `/etc/modules` and add `bcm2835-v4l2`

Mine looks like this:
```
# /etc/modules: kernel modules to load at boot time.
#
# This file contains the names of kernel modules that should be loaded
# at boot time, one per line. Lines beginning with "#" are ignored.

bcm2835-v4l2
```

Check that `/etc/modules-load.d/modules.conf` looks the same.

---

Check if the camera is available:
```
vcgencmd version
vcgencmd get_camera
```
If it failed:
```
sudo usermod -aG video <username>
```
reboot and retry the last command.

`vcgencmd get_camera` must return `detected=1 supported=1`

http://helloraspberrypi.blogspot.com/2020/11/run-vcgencmd-on-ubuntu-for-raspberry-pi.html
https://forum.opencv.org/t/videocapture-read-method-returns-empty-ret/7500/3
https://whatibroke.com/2022/12/03/raspberrypi-camera-not-detected-ubuntu-20-04-5-lts/
