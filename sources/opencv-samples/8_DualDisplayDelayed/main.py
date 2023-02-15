import cv2 as cv
import argparse
import sys
import signal
from queue import Queue

global has_to_break


def sigint_handler(signal, frame):
    global has_to_break
    has_to_break = True


def help():
    print("---- Touch ----")
    print("")
    print("Help\t\tH")
    print("Pause\t\tSPACE BAR")
    print("Quit\t\tESC")
    print("")
    print("----------------")


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
    k = 0
    pause = False
    window1 = "Current Frame"
    window2 = "Previous Frame"

    print("press [H] to print the help")
    cv.namedWindow(window1)
    cv.namedWindow(window2)
    cv.waitKey(500)

    while True:
        
        ret, frame = cap.read()
            
        if ret and frame is not None:
            if not pause:
                frame_count_total += 1
                cv.putText(frame, "frame: {0}".format(frame_count_total), (30, 30), cv.FONT_HERSHEY_PLAIN, 2, (255, 255, 255), 2)
                cv.imshow(window1, frame)   # display frame
                
                if display_saved:
                    cv.imshow(window2, saved_frames.get())  # display saved frame
                else:
                    frame_delta_count += 1
                    display_saved = frame_delta_count == frame_delta
                
                saved_frames.put(frame)     # save frame
            k = cv.waitKey(frame_delay) & 0xFF
        else:
            print("frame is empty")
            k = cv.waitKey(500) & 0xFF

        # break the loop if Ctrl+C or key ESC
        if has_to_break or k == 27:
            break

        if k == 32:   # SPACE BAR
            pause = not pause

        elif k == 104: # H
            help()

    print("Capture done")
    # After the loop release the cap object
    cap.release()

    # Destroy all the windows
    cv.destroyAllWindows()


if __name__ == '__main__':
    signal.signal(signal.SIGINT, sigint_handler)
    parser = argparse.ArgumentParser(description="rtCam main.py CLI")
    parser.add_argument("fps", type=int, help="fps of the display")
    parser.add_argument("frame_delta_delay", type=int, help="difference in seconds between compared frames")
    main()
