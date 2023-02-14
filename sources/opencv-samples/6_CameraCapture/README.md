# 6. Camera Capture

---

# Description

Display the camera video.

---

# Python

## Usage

Assuming you're in the `CameraCapture` folder:

```
python3 main.py [-g]
```

If you add a `-g` parameter, the image is processed in gray scale

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
./main.out [g|G]
```

If you add a `g|G` parameter, the image is processed in gray scale
