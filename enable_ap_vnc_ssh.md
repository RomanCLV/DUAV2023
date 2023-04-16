# Enable  an AccessPoint, SSH and VNC Viewer

## Why do that?
When you will install the RaspberryPi in the drone, you will not be able to controll your Raspberry Pi (bassicaly because you will not have any screen to see what you're doing).

Two options are available: VNC Viewer and SSH.

- VNC Viewer :
It's a remote control software that allows you to view and control a remote computer from another computer. It allows the user to view the remote computer screen, control the mouse and keyboard, and interact with applications and files remotely. 

- Secure Shell (SSH):
It's a secure communication protocol that allows a user to connect a remote computer securely over an insecure network such as the Internet. SSH provides a command line interface for executing commands on the remlote computer, transferring files and managing systems remotely.

In both case, both devices must be connected to a network, the easiest way to connect your devices are to be connect on the same network (even you can do without).

That's why enable an Access Point is interresting. The Raspberry Pi will generate its own network, and your desktop will connect to this network.
You can also enable an AccessPoint from your computer and connect the Raspberry to this network.

Important point to know:

When you're using your Raspberry Pi with a SSH connection, you will not be able to start the GTK service that allows the display.

That means that OpenCV will throw an `error` if you call `cv.namedWindow()`.

Please, read the `Enable SSH on your Raspberry Pi` paragraph.

---

## Configure your Raspberry Pi as an AccessPoint

You can turn on an AccessPoint by turning on the Wi-Fi Hostpot Active. Define a network name and a password.
Now you will be able to connect a desktop, a phone, ... to this access point.
Now, your devices are on the same network.

Warning: This method does not activate the Access Point automatically at startup. 

If you want the access point to be done automatically (highly recommended in our application), you have to do this:
```
nmcli con show
```
If your AccessPoint is called Hostpot (default name on Ubuntu), you should see it in the list.
To turn on it automatically:
```
nmcli con mod <connection-name> connection.autoconnect yes
```

---

## Enable SSH on your Raspberry Pi

```
sudo apt-get install net-tools ssh openssh-server
```

Open the configuration Tool with:
```
sudo raspi-config
```

Move to > `Interface Options` > `SSH` > `Yes`

SSH is now enabled on your device. Reboot the Raspberry Pi:
```
reboot
```

To identify your local ip address, type: `ifconfig` and look at the `wlan0` tag. The IP starting with `192.168.X.X` is your local IP.

To check if the SSH is working, you can connect to yourself with:
```
ssh <username_rasp>@localhost
```
It will ask you the password associate to this user.

If the remote desktop is connected to the same netword, it can access to the Raspberry Pi by using:
```
ssh <username_rasp>@<ip_address_rasp>
```
The first time, you will have to say that `yes` you want to connect.
It will ask you the password associate to this user.
To exit, type `exit` or `logout`.

You can now test (with progamms in folder `sources/test_ssh`) if a programm is launched with a SSH connection or not.
You will need to install `psutil` for Python and `libprocps-dev` for C++:

```
sudo pip3 install psutil
```

```
sudo apt-get install libprocps-dev
```

---

## VNC Viewer
Install VNC Viewer server side on your Raspberry Pi.
Install VNC Viewer client side on your desktop.
Watch a video how to connect your both device.
