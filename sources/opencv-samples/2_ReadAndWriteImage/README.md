# 2. Read And Write Image

---

# Description

Display an input image with an option, and save it.

---

# Python

## Usage

Assuming you're in the `ReadAndWriteImage` folder:

```
python3 main.py <image_path> <saved_image_path> [-m mode]
```

`mode` is not required. 
`mode` defined how the image will be read.
	u | U	IMREAD_UNCHANGED
	c | C	IMREAD_COLOR
	g | G	IMREAD_GRAYSCALE
If `mode` is not set, IMREAD_UNCHANGED is used by default.


## Example

Assuming you're in the `ReadAndWriteImage` folder:

```
python3 main.py ../resources/earth.jpeg earth.png
python3 main.py ../resources/earth.jpeg earth.png -m g
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
./main.out <image_path> <saved_image_path> [mode]
```
`mode` is not required. 
`mode` defined how the image will be read.
	u | U	IMREAD_UNCHANGED
	c | C	IMREAD_COLOR
	g | G	IMREAD_GRAYSCALE
If `mode` is not set, IMREAD_UNCHANGED is used by default.

## Examples

Assuming you're in the `build` folder:

```
./main.out ../../resources/earth.jpeg earth.png
./main.out ../../resources/earth.jpeg earth.png g
```
