# 4. Mask

---

# Description

This program shows how to filter images with mask: the write it yourself and the filter2d way.

---

# Python

Assuming you're in the Mask folder:

```
python3 main.py <image_path> [-g]
```

If you add a -g parameter, the image is processed in gray scale

## Example

Assuming you're in the Mask folder:

```
python3 main.py ../resources/earth.jpeg
python3 main.py ../resources/earth.jpeg -g
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

## Examples

```
./main.out ../../resources/eath.jpeg
./main.out ../../resources/eath.jpeg -g
```
