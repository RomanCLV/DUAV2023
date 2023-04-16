# 17. Camera Capture UDP - Sender

---

# Description

Send the camera video stream via UDP.

---

# Python

## Usage

Assuming you're in the `CameraCaptureUDP/sender` folder:

```
python3 main.py udp_address [-d|--display]
```

`udp_address` the address to send
If you add a `-d|--display` parameter, a window with if the camera video is opened

---

# CPP

## Build

```
mkdir build && cd build
cmake ..
make
```

## Usage

Assuming you're in the `build` folder:

```
./main.out udp_address [-d|--display]
```

`udp_address` the address to send
If you add a `-d|--display` parameter, a window with if the camera video is opened
