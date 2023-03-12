# rtCam

---

# Description

RealTime Camera detection using for drone.
You can configure the `config.yaml` file to not have to write a long command line.

---

# Python

## Usage

Assuming you're in the `rtCam\python` folder:

```
python3 main.py [-dd] [-d] [-db] [-i img1 img2] [-v vid]
```

--- 

# Option details

`dd`: display duration - Display the process' duration to compute a frame. 
`d`:  display - Enable all optional windows: Previous frame, Current frame, Mask, Result. Result is displayed by default.
`db`: debug - Enable display duration option and display option. Also enable some logs (all area detected) and pause while running (between every frame)
`i`: image - Requires two images.
`v`: video - Requires one video.

If you select `i` option, the algorithm uses the given images. Same with the `v` option, but with video. If none of them is specified, the Camera (device 0) is used.

Command line arguments override the configuration parameters.
Example: if `display` is `false` in config.yaml but you gave the `-d` option, you will `enable` the display option for this run.

You can modify parameters while running the code.
Press H in the run to print the help.
