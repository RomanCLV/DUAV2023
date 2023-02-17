import cv2 as cv
import argparse
import sys
import os
import signal
from queue import Queue
from datetime import datetime

global has_to_break


def sigint_handler(signal, frame):
    global has_to_break
    has_to_break = True


def help():
    print("")
    print("--- Function ---   --- KEY ---")
    print("")
    print("    Help               H")
    print("    Pause              SPACE BAR")
    print("    Screenshot         S")
    print("    Quit               ESC")
    print("")
    print("------------------------------")


def main():
    global has_to_break
    has_to_break = False
    
    args = parser.parse_args()

    if args.fps <= 0:
        print(f"fps must be a positiv number!")
        exit(-1)
    if args.frame_delta_delay <= 0:
        print(f"frame_delta_delay must be a positiv number!")
        exit(-1)

    print("Trying to open /dev/video0 with CAP_V4L2")
    cap = cv.VideoCapture(0, cv.CAP_V4L2)  # Add cv::CAP_V4L2 to fix: Embedded video playback halted; module v4l2src0 reported: Failed to allocate required memory.

    if not cap.isOpened():
        sys.exit("Can not read the device")

    frame_delay = int(1000 / args.fps)
    frame_delta = int(args.fps * args.frame_delta_delay)
    frame_delta_count = 0
    frame_count_total = 0
    display_saved = False
    saved_frames = Queue(frame_delta)
    screenshot_count = 0
    screenshot_name = datetime.today().strftime('%Y-%m-%d %H:%M:%S')
    k = 0
    pause = False
    last_frame = None
    window1 = "Current Frame"
    window2 = "Previous Frame"

    print("press [H] to print the help")
    cv.namedWindow(window1)
    cv.namedWindow(window2)
    cv.waitKey(500)

    while True:
        
        if pause:
            k = cv.waitKey(frame_delay) & 0xFF
        else:
            ret, frame = cap.read()

            if ret and frame is not None:
                frame_count_total += 1
                cv.putText(frame, f"frame: {frame_count_total}", (30, 30), cv.FONT_HERSHEY_PLAIN, 2, (255, 255, 255), 2)
                cv.imshow(window1, frame)   # display frame
                if display_saved:
                    cv.imshow(window2, saved_frames.get())  # display saved frame
                else:
                    frame_delta_count += 1
                    display_saved = frame_delta_count == frame_delta    
                saved_frames.put(frame)     # save frame
                last_frame = frame
                k = cv.waitKey(frame_delay) & 0xFF
            else:
                print("frame is empty")
                k = cv.waitKey(500) & 0xFF

        # break the loop if Ctrl+C or key ESC
        if has_to_break or k == 27:
            break

        if k == 72 or k == 104:     # H | h
            help()

        elif k == 32:               # SPACE BAR
            if saved_frames.qsize() > 0:  # allows pause only if we already got a frame
                pause = not pause

        elif k == 83 or k == 115:   # S | s
            if last_frame is not None:
                screenshot_count += 1
                path = f"./screenshot/{screenshot_name}_{screenshot_count}.jpg"
                if not os.path.exists("./screenshot"):
                    os.mkdir("./screenshot")
                cv.imwrite(path, last_frame)
                print(path)


    print("Capture done")
    # After the loop release the cap object
    cap.release()

    # Destroy all the windows
    cv.destroyAllWindows()


if __name__ == '__main__':
    signal.signal(signal.SIGINT, sigint_handler)
    parser = argparse.ArgumentParser(description="Dual Display Delayed main.py CLI")
    parser.add_argument("fps", type=int, help="of the camera")
    parser.add_argument("frame_delta_delay", type=int, help="the delay (in second) between the two displayed frames")
    main()
