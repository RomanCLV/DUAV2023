# 8. Dual Display Delayed

---

# Description

Display two windows, one with the current frame and one with a delayed frame.

---

# Python

## Usage

Assuming you're in the `DualDisplayDelayed` folder:

```
python3 main.py <fps> <frame_delta_delay>
```

`fps` of the camera

`frame_delta_delay` is the delay (in second) between the two displayed frames

## Example

Assuming you're in the `DualDisplayDelayed` folder:

```
python3 main.py 30 2
```
Indicate that your camera takes 30 frames per second, and the second window will start after 2 seconds, so with a delay of 30 f/s * 2 s = 60 frames

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
./main.out <fps> <frame_delta_delay>
```

`fps` of the camera

`frame_delta_delay` is the delay (in second) between the two displayed frames

## Example

Assuming you're in the `build` folder:

```
./maint.out 30 2
```
Indicate that your camera takes 30 frames per second, and the second window will start after 2 seconds, so with a delay of 30 f/s * 2 s = 60 frames
