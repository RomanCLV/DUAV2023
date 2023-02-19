# 13. Edge Detection

---

# Description

Edge detection is a technique for detecting the edges of objects in an image.
To remove a shadow from an image, you can try to detect the edges of the shadow and replace the pixels 
corresponding to the shadow with pixel values from the original image.

---

# Python

## Usage

Assuming you're in the `EdgeDetection` folder:

```
python3 main.py <image_path>
```

## Example

Assuming you're in the `EdgeDetection` folder:

```
python3 main.py ../resources/shadow.jpg
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

Assuming you're in the `build` folder:

```
./main.out <image_path>
```

## Example

Assuming you're in the `build` folder:

```
./maint.out ../../resources/shadow.jpg
```
