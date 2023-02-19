# 11. Diff between Images

---

# Description

Show the difference between two given images using the binary `threshold` and the `morphological opening`.

Explanation of the algorithm:

- `Absolute difference`

A pixel is normally written in (R,G,B) (OpenCV adopts the BGR convention and not RGB, but basically it is a triplet).
In RGB | BGR convention: minimum = 0 maximum = 255  
So there are 256 possibilities, and 2^8 = 256, so that's why we talk about color coded in 8 bits (or 2 bytes (1 byte = 8 bits))
We often find the hexadecimal code for colors since in base 16: 00 = 0 and FF = 255

Example in RGB convention:
Red	  : (255,   0,   0) 	#FF0000
Green : (  0, 255,   0) 	#00FF00
Blue  : (  0,   0, 255) 	#0000FF

A gray color is translated by R=G=B. In other words, an image in gray is made of pixels
whose three components are equal, we can simplify this triplet by a single number (between 0 and 255).
So we can subtract two gray images since each pixel is now a number and not a triplet.
Thus, we can speak of a difference of images.

- `Binarization of the image (threshold)`

By default, the threshold has been set to 30, which means that all pixels whose difference between the two images
is greater than 30 will be defined as white, and all other pixels will be defined as black.

It is important to choose an appropriate threshold for the images you are using. 
If the threshold is too high, you may miss important differences between the images. 
If the threshold is too low, you may detect minor differences or noise in the images.

In some cases, we need to get rid of this noise (spurious pixels, isolated, fine lines, etc. ...).

- `Morphological opening`

The morphological opening consists of the successive application of two morphological operators.
First we perform an erosion that allows us to remove the "isolated" pixels.
Then we perform a dilation that allows us to strengthen the dense groups of pixels.

---

# Python

## Usage

Assuming you're in the `DiffBetweenImages` folder:

```
python3 main.py <image1_path> <image2_path>
```

## Example

Assuming you're in the `DiffBetweenImages` folder:

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
./main.out <image1_path> <image2_path>
```

## Example

Assuming you're in the `build` folder:

```
./maint.out ../../resources/img_sub_1.jpg  ../../resources/img_sub_2.jpg
```
