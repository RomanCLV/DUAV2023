# 9. Images Subtraction

---

# Description

Show the difference between two given images.

---

# Python

## Usage

Assuming you're in the `ImagesSubtraction` folder:

```
python3 main.py <image1_path> <image2_path> [-g]
```

If you add a `-g` parameter, the image is processed in gray scale

## Example

Assuming you're in the `ImagesSubtraction` folder:

```
python3 main.py ../resources/img_sub_1.jpg  ../resources/img_sub_2.jpg
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
./main.out <image1_path> <image2_path> [g|G]
```

If you add a `g|G` parameter, the image is processed in gray scale

## Example

Assuming you're in the `build` folder:

```
./maint.out  ../../resources/img_sub_1.jpg  ../../resources/img_sub_2.jpg
```
