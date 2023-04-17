# 17. Camera Capture UDP - Listener

---

# Description

Receive the camera video stream via UDP.

---

# Python

## Usage

Assuming you're in the `CameraCaptureUDP/listener` folder:

```
python3 main.py udp_address udp_port [-o <file_name>.mp4] [-fps fps] 
```

`udp_address` the address to listen
`udp_port` the port to listen
`-o|--output` write a video file. Please give a mp4 path.
`-fps` the fps of the saved video. 30 by default. Check your camera fps if you have to change it.

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
