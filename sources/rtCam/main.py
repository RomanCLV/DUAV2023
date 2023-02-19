import cv2 as cv
import argparse
import sys
import os
import signal
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

    cap = None
    k = 0
    duration = 0.0

    fps = 0
    frame_to_take = 0
    frame_count = 0
    frame_wait_delay = 0

    images = []
    previous_frame = None

    display = False
    pause = False
    grayscale = False
    display_duration = False
    detection_enabled = True

    if args.display_duration or args.debug:
        display_duration = True

    if args.display or args.debug:
        display = True

    if args.image:
        images = [None, None]
        for i in range(2):
            path = cv.samples.findFile(args.image[i])
            if not path:
                sys.exit(f"Cannot find: {args.image[i]}")
            images[i] = cv.imread(path, cv.IMREAD_UNCHANGED)
            if images[i] is None:
                sys.exit(f"Could not read the image {path}")

    elif args.video:
        print(f"Trying to open: {args.video}")
        cap = cv.VideoCapture(args.video)
        if not cap.isOpened():
            sys.exit("Can not read the video")

    else:
        print("Trying to open /dev/video0 with CAP_V4L2")
        cap = cv.VideoCapture(0, cv.CAP_V4L2)  # Add cv::CAP_V4L2 to fix: Embedded video playback halted; module v4l2src0 reported: Failed to allocate required memory.
        if not cap.isOpened():
            sys.exit("Can not read the device")
    
    if cap is not None:
        fps = cap.get(cv.CAP_PROP_FPS)
        # if fps = 30, so frame_to_take = 15, so we take a frame every 15 => (we take a frame every 0.5 sec)
        frame_to_take = int(fps / 2)
        # not initialized to 1 to take a frame directly
        frame_count = frame_to_take
        frame_wait_delay = int(1000 / fps)  # if fps = 30, delay = 33 ms
        print(f"fps: {fps}")
        print(f"frame_to_take: {frame_to_take}")
        print(f"frame_wait_delay: {frame_wait_delay} ms")


    RTH_PATH = "../automate/RTH"
    if True and (args.image or args.video):      # TODO: remove True
        RTH_PATH = "./RTH"

    if os.path.exists(RTH_PATH):
        os.remove(RTH_PATH)

    window1 = "Current"
    window2 = "Previous"
    window3 = "Result"

    print("press [H] to print the help")
    if display:
        cv.namedWindow(window1)
        cv.namedWindow(window2)
        cv.namedWindow(window3)
    else:
        cv.namedWindow("rtCam")
    cv.waitKey(500)

    while True:

        if has_to_break:
            break

        if pause:
            k = cv.waitKey(500) & 0xFF
        else:
            ret = False
            frame = None

            if images:
                previous_frame = images[0]
                frame = images[1]
                ret = True
            else:
                ret, frame = cap.read()

            if ret and frame is not None:

                if frame_count >= frame_to_take:
                    frame_count = 1
                    duration = time()

                    if grayscale:
                        frame = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)

                    # TODO: blur frame ?

                    if display:
                        cv.imshow(window1, frame)

                    # if we have a previous frame
                    if previous_frame is not None:

                        if display:
                            cv.imshow(window2, previous_frame)

                        # check dimensions and depth
                        if frame.shape == previous_frame.shape and frame.dtype == previous_frame.dtype:

                            result = cv.subtract(previous_frame, frame)

                            if display:
                                cv.imshow(window3, result)

                            # TODO: check if there is light pixel.. Mask ? form recognition ? inRange() ?
                            something_detected = False

                            if something_detected:
                                # we don't change the "good" previous_frame until there is no more detected object

                                # TODO: detection_enabled in rtCam or in automate.py ??
                                if detection_enabled:
                                    print("Object detected!")
                                    if not os.path.exists(RTH_PATH):
                                        f = open(RTH_PATH, "x")  # create RTH file
                                        f.close()
                            else:
                                # replace last frame with the current
                                previous_frame = frame

                            if display_duration:
                                duration = 1000 * (time() - duration)
                                print(round(duration * 1000) / 1000, end=" ")
                                print("ms")
                        else:
                            print("frames haven't the same dimensions or depth!")
                            # replace last frame with the current
                            previous_frame = frame
                        if images:
                            cv.waitKey(0)
                            break
                    else:
                        # replace last frame with the current
                        previous_frame = frame

                    k = cv.waitKey(1) & 0xFF
                else:
                    frame_count += 1
                    k = cv.waitKey(frame_wait_delay) & 0xFF
            else:
                if args.video:
                    break
                print("frame is empty")
                k = cv.waitKey(500) & 0xFF

        if has_to_break or k == 27:  # ESC
            break

        if k == 72 or k == 104:  # H | h
            help()

        elif k == 71 or k == 103:  # G | g
            grayscale = not grayscale

        elif k == 68 or k == 100:  # D | d
            display_duration = not display_duration

        elif k == 32:  # SPACE BAR
            pause = not pause

    if cap:
        print("Capture done")
        cap.release()
    cv.destroyAllWindows()


if __name__ == '__main__':
    signal.signal(signal.SIGINT, sigint_handler)
    parser = argparse.ArgumentParser(description="RealTime Camera main.py CLI")
    parser.add_argument("-i", "--image", type=str, nargs=2, help="process with two given images")
    parser.add_argument("-v", "--video", type=str, help="process with a given video")
    parser.add_argument("-d", "--display", action="store_true", help="enable views. Takes resources! Not recommanded in production")
    parser.add_argument("-dd", "--display_duration", action="store_true", help="display duration will be automatically enabled")
    parser.add_argument("-db", "--debug", action="store_true", help="shortcut to -d and -dd")

    main()
