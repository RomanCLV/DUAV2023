# https://learnopencv.com/read-write-and-display-a-video-using-opencv-cpp-python/

import cv2 as cv
import argparse
import sys
import signal
import os
from datetime import datetime


global has_to_break


def sigint_handler(signal, frame):
    global has_to_break
    has_to_break = True


def main():
    global has_to_break
    has_to_break = False
    
    args = parser.parse_args()

    print("Trying to open /dev/video0 with CAP_V4L2")
    cap = cv.VideoCapture(0, cv.CAP_V4L2)  # Add cv::CAP_V4L2 to fix: Embedded video playback halted; module v4l2src0 reported: Failed to allocate required memory.

    if not cap.isOpened():
        sys.exit("Can not read the device")

    print("Press ESC to quit and S to take a screenshot")
    screenshot_count = 0
    screenshot_name = datetime.today().strftime('%Y-%m-%d %H:%M:%S')
    window_name = "Camera Capture"
    cv.namedWindow(window_name)
    cv.waitKey(500)
    k = 0
    while True:
          
        ret, frame = cap.read()
        
        if not (ret == False or frame is None):
            if args.grayscale:
                frame = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
            cv.imshow(window_name, frame)
            k = cv.waitKey(1) & 0xFF

        else:
            print("frame is empty")
            k = cv.waitKey(500) & 0xFF

        # break the loop if Ctrl+C or key ESC
        # cv.getWindowProperty(window_name, cv.WND_PROP_VISIBLE) == -1:  # window is closed
        if has_to_break or k == 27:
            break

        if k == 83 or k == 115:   # S | s
            if frame is not None:
                screenshot_count += 1
                path = f"./screenshot/{screenshot_name}_{screenshot_count}.jpg"
                if not os.path.exists("./screenshot"):
                    os.mkdir("./screenshot")
                cv.imwrite(path, frame)
                print(path)

    print("Capture done")
    # After the loop release the cap object
    cap.release()

    # Destroy all the windows
    cv.destroyAllWindows()


if __name__ == '__main__':
    signal.signal(signal.SIGINT, sigint_handler)
    parser = argparse.ArgumentParser(description="Camera Capture main.py CLI")
    parser.add_argument("-g", "--grayscale", action="store_true", help="capture the camera with GRAYSCALE")
    main()
