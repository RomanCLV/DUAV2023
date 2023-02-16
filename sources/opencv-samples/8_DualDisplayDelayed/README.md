# 8. Dual Display Delayed

---

# Description

RealTime Camera detection using for drone.

---

# Python

## Usage

Assuming you're in the `DualDisplayDelayed` folder:

```
python3 main.py <fps> <frame_delta_delay>
```

`fps` is the frame per second

`frame_delta_delay` is the delay (in second) between the two displayed frames

## Example

Assuming you're in the `DualDisplayDelayed` folder:

```
python3 main.py 30 2
```
Indicate to take 30 frames per second, and the second window will start after 2 seconds, so with a delay of 30 f/s * 2 s = 60 frames

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

`fps` is the frame per second

`frame_delta_delay` is the delay (in second) between the two displayed frames

## Example

Assuming you're in the `DualDisplayDelayed` folder:

```
./maint.out 30 2
```
Indicate to take 30 frames per second, and the second window will start after 2 seconds, so with a delay of 30 f/s * 2 s = 60 frames
