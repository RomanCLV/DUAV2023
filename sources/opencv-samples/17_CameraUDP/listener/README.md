# 17. Camera Capture UDP - Listener

---

# Description

Receive the camera video stream via UDP.

---

# Python

## Usage

Assuming you're in the `CameraCaptureUDP/listener` folder:

```
python3 main.py ip port [-d|--display] [-o <file_name>.mp4] [-fps fps]
```

`ip` IP address to listen (default: 0.0.0.0)
`port` Port to listen
`-d|--display` Display video frames in a window
`-o|--output` Path to save the video file (.avi)
`-fps` Frames per second for the video (default: 30). Check your camera fps if you have to change it.

`cheatcode`: If you can't bind the socket to the Raspberry Pi address, try to use the default ip: 0.0.0.0

Available `fps`:
```
1
5
10
12
15
20
24
25
29.97
30
48
50
59.94
60
120
144
240
300
```
