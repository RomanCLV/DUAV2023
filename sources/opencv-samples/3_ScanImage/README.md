# 3. Scan Image

---

# Description

This program shows how to scan image objects in OpenCV (cv::Mat).
As use case we take an input image and divide the native color palette (255) with the input.
Shows C operator[] method, iterators and at function for on-the-fly item address calculation.

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
./main.out <image_path> <divide_with> [g|G]
```

If you add a `g|G` parameter, the image is processed in gray scale

## Examples

Assuming you're in the `build` folder:

```
./main.out ../../resources/fruits.jpg 10
./main.out ../../resources/earth.jpeg 10 g
```
