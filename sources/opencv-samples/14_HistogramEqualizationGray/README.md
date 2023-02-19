# 14. Histogram Equalization Gray

---

# Description

Histogram equalization is an image processing technique that rebalances the distribution of gray levels in an image.
This can help improve image quality by increasing contrast and improving the visibility of details.
This technique can be used to reduce the differences in brightness between the shadow and the rest of the image.

---

# Python

## Usage

Assuming you're in the `HistogramEqualizationGray` folder:

```
python3 main.py <image_path>
```

## Example

Assuming you're in the `HistogramEqualizationGray` folder:

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
