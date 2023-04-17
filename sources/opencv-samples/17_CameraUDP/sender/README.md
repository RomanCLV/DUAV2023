# 17. Camera Capture UDP - Sender

---

# Description

Send the camera video stream via UDP.

---

# Python

## Usage

Assuming you're in the `CameraCaptureUDP/sender` folder:

```
python3 main.py udp_address udp_port [-d|--display] [-o <file_name>.mp4]
```

`udp_address` the address to listen
`udp_port` the port to listen
`-d|--display` display a window with the camera video
`-o|--output` write a video file. Please give a mp4 path.
