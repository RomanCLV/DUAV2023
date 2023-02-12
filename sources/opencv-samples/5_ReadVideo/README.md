# 5. Read Video

---

# Description

Display an input video.

---

# Python

## Usage

Assuming you're in the ReadVideo folder:

```
python3 main.py <image_path> [-g]
```

If you add a -g parameter, the image is processed in gray scale

## Examples

Assuming you're in the ReadVideo folder:

```
python3 main.py ../resources/video.mp4
python3 main.py ../resources/video.mp4 -g
```

---

# CPP

## Build

```
mkdir build && cd build
cmake ..
make
```

## Usage

Assuming you're in the build folder:

```
./main.out <image_path> [g|G]
```

If you add a g|G parameter, the image is processed in gray scale

## Example

Assuming you're in the build folder:

```
./main.out ../../resources/video.mp4
./main.out ../../resources/video.mp4 g
```
