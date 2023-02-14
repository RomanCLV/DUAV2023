# 7. Camera Recorder

---

# Description

Display the camera video and save it into a specified AVI file.

Make sure `6_CameraCapture` sample is working before test this sample.

---

# Python

## Usage

Assuming you're in the `CameraRecorder` folder:

```
python3 main.py <output_path.avi> <video_fps> [-g]
```

If you add a `-g` parameter, the image is processed in gray scale

Available fps:
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

## Examples

Assuming you're in the `CameraRecorder` folder:

```
python3 main.py output.avi 30
python3 main.py output.avi 30 -g
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
./main.out <output_path.avi> <video_fps> [g|G]
```

If you add a `g|G` parameter, the image is processed in gray scale

Available fps:
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

## Examples

Assuming you're in the build folder:

```
./main.out output.avi 30
./main.out output.avi 30 g
```

---

## Check the output file

Open the video written to check if all is good.

### Possible error

Error occured on Ubuntu when trying to open the video file.
`The specified movie could not be found.`

You can use VLC to read the file to check if the file is valid.
```
sudo apt install vlc
```

To fix the Videos player:
```
 sudo apt remove gstreamer1.0-vaapi
```

If it still persists:
```
sudo apt install ubuntu-restricted-extras
```
Reboot.

https://askubuntu.com/questions/1406254/after-installing-ubuntu-22-04-the-default-video-player-is-unable-to-play-any-vi


If it still persists:
Add in the `/etc/environment` file the following line:
```
MESA_GL_VERSION_OVERRIDE=3.3
```

https://gitlab.gnome.org/GNOME/totem/-/issues/523

Reboot.

If it's still persists, it can be due to your OpenGL version. It's recommanded to use OpenGl 3

https://github.com/celluloid-player/celluloid/issues/311

To see OpenGL version:

https://askubuntu.com/questions/47062/terminal-command-to-show-opengl-version
```
sudo apt-get install mesa-utils

glxinfo | grep "OpenGL version"
```

Maybe try to upgrade OpenGL or force to use a newer version. Good luck :)
