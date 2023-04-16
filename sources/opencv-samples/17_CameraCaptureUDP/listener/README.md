# 17. Camera Capture UDP - Listener

---

# Description

Receive the camera video stream via UDP.

---

# Python

## Usage

Assuming you're in the `CameraCaptureUDP/listener` folder:

```
python3 main.py udp_address
```

`udp_address` the address to received

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
./main.out udp_address
```

`udp_address` the address to received
