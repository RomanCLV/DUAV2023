# https://learnopencv.com/read-write-and-display-a-video-using-opencv-cpp-python/

import cv2 as cv
import argparse
import sys
import signal
import time


global has_to_break


fps_choices = [1, 5, 10, 12, 15, 20, 24, 25, 29.97, 30, 48, 50, 59.94, 60, 120, 144, 240, 300]


def sigint_handler(signal, frame):
    global has_to_break
    has_to_break = True


def formate_time(t):
    return round(t * 1000) / 1000


def main():
    global has_to_break
    has_to_break = False
    
    args = parser.parse_args()

    print("Trying to open /dev/video0 with CAP_V4L2")
    cap = cv.VideoCapture(0, cv.CAP_V4L2)  # Add cv::CAP_V4L2 to fix: Embedded video playback halted; module v4l2src0 reported: Failed to allocate required memory.

    if not cap.isOpened():
        sys.exit("Can not read the device")

    # Default resolutions of the frame are obtained. The default resolutions are system dependent.
    # We convert the resolutions from float to integer.
    frame_width = int(cap.get(cv.CAP_PROP_FRAME_WIDTH))
    frame_height = int(cap.get(cv.CAP_PROP_FRAME_HEIGHT))
    frame_fps = cap.get(cv.CAP_PROP_FPS) 
    # Additional properties can be found here:
    # https://docs.opencv.org/3.4/d4/d15/group__videoio__flags__base.html
    # VideoCaptureProperties

    print(f"frame width: {frame_width}")
    print(f"frame height: {frame_height}")
    print(f"frame fps: {frame_fps}")
    print(f"video fps: {args.video_fps}")

    # Define the codec and create VideoWriter object. The output is stored in output_path file.
    # Define the fps to be equal to 10. Also frame size is passed.
    # FourCC is a 4-byte code used to specify the video codec.
    video = cv.VideoWriter(args.output_path, cv.VideoWriter_fourcc('M','J','P','G'), args.video_fps, (frame_width, frame_height))

    window_name = "Camera Capture"
    cv.namedWindow(window_name)
    cv.waitKey(500)
    k = 0
    frame_count = 0
    start_time = time.time()

    while True:
          
        ret, frame = cap.read()
        
        if not (ret == False or frame is None):
            if args.grayscale:
                frame = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)   # BGR  -> Gray : the image loses its color
                frame = cv.cvtColor(frame, cv.COLOR_GRAY2BGR)   # Gray -> BGR  : the image is still gray but the 3 channels are reconstituted
                                                                # If the 3 channels are not reconstituted, you will get "Could not demultiplex stream."
                                                                # when trying to open your vide.
            frame_count += 1
            # Write the frame into the file
            video.write(frame)

            cv.imshow(window_name, frame)

            k = cv.waitKey(1) & 0xFF
        else:
            print("frame is empty")
            k = cv.waitKey(500) & 0xFF

        # break the loop if Ctrl+C or key ESC
        # cv.getWindowProperty(window_name, cv.WND_PROP_VISIBLE) == -1:  # window is closed
        if has_to_break or k == 27:
            break

    video_duration = time.time() - start_time

    print("Capture done")
    # When everything done, release the video capture and video write objects
    cap.release()
    video.release()

    print(f"Recorded duration: {formate_time(video_duration)} secs")
    print(f"Frames recorded:   {frame_count}")
    print(f"Video duration:    {formate_time(frame_count / args.video_fps)} secs")

    # Destroy all the windows
    cv.destroyAllWindows()


if __name__ == '__main__':
    signal.signal(signal.SIGINT, sigint_handler)
    parser = argparse.ArgumentParser(description="Camera Capture main.py CLI")
    parser.add_argument("output_path", type=str, help="the file where the camera video is recorded")
    parser.add_argument("video_fps", type=float, choices=fps_choices, help="fps of the recorded video")
    parser.add_argument("-g", "--grayscale", action="store_true", help="capture the camera with GRAYSCALE")
    main()
