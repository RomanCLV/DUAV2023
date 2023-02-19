# 12. Image Segmentation

---

# Description

Segmentation is a technique for dividing an image into distinct regions or objects. 
To remove shadow from an image, you can try segmenting the image using techniques such as 
thresholding segmentation or clustering segmentation. Once the shadow has been segmented, 
you can replace the pixels corresponding to the shadow with pixel values from the original image.

---

# Python

## Usage

Assuming you're in the `ImageSegmentation` folder:

```
python3 main.py <image_path>
```

## Example

Assuming you're in the `ImageSegmentation` folder:

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
