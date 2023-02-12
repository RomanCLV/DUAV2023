# https://note.nkmk.me/en/python-opencv-videocapture-file-camera/

import cv2 as cv
import argparse
import os
import signal

global has_to_break


def sigint_handler(signal, frame):
    global has_to_break
    has_to_break = True


def main():
    global has_to_break
    has_to_break = False

    args = parser.parse_args()

    if not os.path.exists(args.video_path):
        print(f"Video {os.path.exists(args.video_path)} not found")

    cap = cv.VideoCapture(args.video_path)

    if not cap.isOpened():
        print(f"Can't open video {args.video_path}")
        exit(0)

    window_name = "Read Video"

    while True:
          
        ret, frame = cap.read()
        
        if ret == False or frame is None:
            break

        if args.grayscale:
            # Our operations on the frame come here
            frame = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
          
        cv.imshow(window_name, frame)

        if cv.waitKey(1) == 27 or has_to_break:  #  or cv.getWindowProperty(window_name, cv.WND_PROP_VISIBLE) == -1
            break

    print("Video successfully played")

    # After the loop release the cap object
    cap.release()

    # Destroy all the windows
    cv.destroyAllWindows()


if __name__ == '__main__':
    signal.signal(signal.SIGINT, sigint_handler)
    parser = argparse.ArgumentParser(description="Read Video main.py CLI")
    parser.add_argument("video_path", type=str, help="path to video file")
    parser.add_argument("-g", "--grayscale", action="store_true", help="read the video with GRAYSCALE")
    main()
