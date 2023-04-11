# Enable  an AccessPoint, SSH and VNC Viewer

## Why do that?
When you will install the RaspberryPi in the drone, you will not be able to controll your Raspberry Pi (bassicaly because you will not have any screen to see what you're doing).

Two options are available: VNC Viewer and SSH.

- VNC Viewer :
It's a remote control software that allows you to view and control a remote computer from another computer. It allows the user to view the remote computer screen, control the mouse and keyboard, and interact with applications and files remotely. 

- Secure Shell (SSH):
It's a secure communication protocol that allows a user to connect a remote computer securely over an insecure network such as the Internet. SSH provides a command line interface for executing commands on the remlote computer, transferring files and managing systems remotely.

In both case, both devices must be connected to a network, the easiest way to connect your devices are to be connect on the same network (even you can do without).

Moreover, when your Raspberry will be in the drone, it will can not connect itself to a network.

That's why enable an Access Point is interresting. The Raspberry Pi will generate its own network, and your desktop will connect to this network.

---

## Configure your Raspberry Pi as an AccessPoint

---

## VNC Viewer
Install VNC Viewer server side on your Raspberry Pi.
Install VNC Viewer client side on your desktop.
Watch a video how to connect your both device.

---

## How to manage SSH on your Raspberry Pi

Open the configuration Tool with:
```
sudo raspi-config
```

Move to > `Interface Options` > `SSH` > `Yes`

SSH is now enabled on your device.


To identify your local ip address, type: `ifconfig` and look at the `wlan0` tag. The IP starting with `192.168.X.X` is your local IP.

To use `ifconfig`, install `net-tools` using:

```
sudo apt install net-tools
```
