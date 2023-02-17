# 10. Delayed Subtraction

---

# Description

This sample is a mix between samples 8 and 9.
It shows the difference with a delayed frame

---

# Python

## Usage

Assuming you're in the `DelayedSubtraction` folder:

```
python3 main.py <frame_delta>
```

`frame_delta` is the number of frames between the two frames being compared

## Example

Assuming you're in the `DelayedSubtraction` folder:

```
python3 main.py 30
```

The current image is compared to the previous 30. So, if your camera is at 30 frames per second, you compare the current image with the previous second.

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
./main.out <fps> <frame_delta>
```

`frame_delta` is the number of frames between the two frames being compared

## Example

Assuming you're in the `build` folder:

```
./maint.out 30
```

The current image is compared to the previous 30. So, if your camera is at 30 frames per second, you compare the current image with the previous second.
