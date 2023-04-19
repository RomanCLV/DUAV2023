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
python3 ./main.py [-d] [-do] [-dd] [-db] [-i img1 img2] [-v vid] [-sd] [-srwd] [-sr] [-sm] [-udp] [-ip] [-port]
```

---

# Option details

`-d`    `--display`    						Display a window of the resulting frame
`-do`   `--display_opt`			 			Enable all optional windows: Previous frame, Current frame, Mask, Result.
`-dd`   `--display_duration`				Display the process duration to compute a frame. 
`-db`   `--debug`     					 	Enable all windows, display durayion, additionnal logs, pause on every frame
`-i`    `--image`  						   	Process on the two given images
`-v`    `--video`							Process on the given video 
`-sd`   `--save_detection`                  Save detected objects into a video file
`-srwd` `--save_result_without_detection`   Save the resulting frames without the rectangle of detection into a video file
`-sr`   `--save_result` 					Save the resulting frames into a video file
`-sm`   `--save_mask` 						Save the mask frames into a video file
`-udp`  `--udp`								Enable the UDP stream (will read the `config.yaml` ip and port)
`-ip`  	`--ip`								Specify an ip address for UDP stream (udp automatically enabled)
`-port` `--port`							Specify a port for UDP stream (udp automatically enabled)

If options `i` or `v` are not specified, the Camera (device 0) is used.

Command line arguments override the configuration parameters.

Example: if `display` is `false` in `config.yaml` but you specified the `-d` option, you will enable the display option for this run.

You can modify parameters while running the code.

Press H in the run to print the help (key bindings, ...).
