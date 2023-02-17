import cv2 as cv
import argparse
import sys
import signal
from queue import Queue
from time import time

global has_to_break


def sigint_handler(signal, frame):
    global has_to_break
    has_to_break = True


def help():
    print("")
    print("--- Function -------   --- KEY ---")
    print("")
    print("    Display duration       D")
    print("    Grayscale              G")
    print("    Help                   H")
    print("    Pause                  SPACE BAR")
    print("    Quit                   ESC")
    print("")
    print("----------------------------------")


def main():
    global has_to_break
    has_to_break = False
    
    args = parser.parse_args()

    if args.frame_delta <= 0:
        print(f"frame_delta must be a positiv number!")
        exit(-1)

    frame_delta = args.frame_delta

    print("Trying to open /dev/video0 with CAP_V4L2")
    cap = cv.VideoCapture(0, cv.CAP_V4L2)  # Add cv::CAP_V4L2 to fix: Embedded video playback halted; module v4l2src0 reported: Failed to allocate required memory.

    if not cap.isOpened():
        sys.exit("Can not read the device")

    frame_delta_count = 0
    saved_frames = Queue(frame_delta)
    k = 0
    pause = False
    grayscale = False
    display_subtract = False
    display_duration = False
    window1 = "Current Frame"
    window2 = "Subtraction"

    print("press [H] to print the help")
    cv.namedWindow(window1)
    cv.namedWindow(window2)
    cv.waitKey(500)

    while True:
        
        if pause:
            k = cv.waitKey(500) & 0xFF
        else:
            ret, frame = cap.read()

            if ret and frame is not None:

                if grayscale:
                    frame = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)

                cv.imshow(window1, frame)   # display frame

                if display_subtract:
                    t = time()
                    old = saved_frames.get()

                    if frame.shape == old.shape and frame.dtype == old.dtype:
                        cv.imshow(window2, cv.subtract(old, frame))  # display saved frame
                        if display_duration:
                            t = 1000 * (time() - t)
                            print(f"subtract duration: {round(t * 1000) / 1000} ms")
                    else:
                        print(f"Not the same dimensions or data type!\tCurrent: {frame.shape} ({frame.dtype})\tOld: {old.shape} ({old.dtype})")
                    
                else:
                    frame_delta_count += 1
                    display_subtract = frame_delta_count == frame_delta    

                saved_frames.put(frame)     # save frame
                k = cv.waitKey(1) & 0xFF
            else:
                print("frame is empty")
                k = cv.waitKey(500) & 0xFF

        # break the loop if Ctrl+C or key ESC
        if has_to_break or k == 27:
            break

        if k == 72 or k == 104:     # H | h
            help()

        elif k == 71 or k == 103:   # G | g
            grayscale = not grayscale

        elif k == 68 or k == 100:   # D | d
            display_duration = not display_duration

        elif k == 32:               # SPACE BAR
            pause = not pause

    print("Capture done")
    cap.release()
    cv.destroyAllWindows()


if __name__ == '__main__':
    signal.signal(signal.SIGINT, sigint_handler)
    parser = argparse.ArgumentParser(description="Delayed Subtraction main.py CLI")
    parser.add_argument("frame_delta", type=int, help="the number of frames between the two frames being compared")
    main()
