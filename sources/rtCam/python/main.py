import cv2 as cv
import argparse
import sys
import os
import signal
from time import time
import numpy as np
from config import Config
from enum import Enum


class Option(Enum):
    none = 0,
    gaussian_blur = 1,
    threshold = 2,
    kernel_size = 3,
    detection_area = 4


global has_to_break


def sigint_handler(signal, frame):
    global has_to_break
    has_to_break = True


def help():
    print("")
    print("--- Function -------   --- KEY -----")
    print("")
    print("    Clear detection        C")
    print("    Display duration       D")
    print("    Decrease option        Left Arrow")
    print("    Increase option        Right Arrow")
    print("    Help                   H")
    print("    Pause                  SPACE BAR")
    print("    Quit                   ESC")
    print("    Display config         I")
    print("    Reset config           R")
    print("    Save config            S")
    print("")
    print(" Select an option:")
    print("")
    print("    Gaussian blur          G")
    print("    Threshold              T")
    print("    Kernel size            K")
    print("    Detection area         A")
    print("")
    print("----------------------------------")


def waitKey(millis: int):
    return cv.waitKeyEx(millis) & 0xFFFF


def getTimeDiff(timeStart: float):
    return 1000 * (time() - timeStart)


def round3(value: float):
    return round(value * 1000) / 1000.0;

def sys_exit(message: str, exit_code=-1):
    print(message)
    input("press entre to close...")
    sys.exit(exit_code)


def main():

    global has_to_break

    has_to_break = False

    args = parser.parse_args()

    cap = None
    k = 0
    RTH_PATH = "./RTH"
    selected_option = Option.none
    duration = 0.0
    duration_average = 0.0
    duration_average_count = 0

    fps = 0
    frame_to_take = 0
    frame_count = 0
    frame_wait_delay = 0

    images = []
    previous_frame = None

    gray_image1 = None
    gray_image2 = None
    gaussian_image1 = None
    gaussian_image2 = None
    diff_image = None

    pause = False
    read_next_frame = True
    detection_enabled = True
    something_detected = False
    clear_detection = False
    config_changed = False

    # to display red rectangle
    contours=[]
    max_area = -1.0
    max_area_id = -1
    current_area = 0.0

    threshold_image = None
    mask = None
    result = None

    window_prev_frame = "Previous frame"
    window_curr_frame = "Current frame"
    window_mask = "Mask"
    window_result = "Result"

    config = Config()
    config.read()

    if args.debug:
        config.set_debug(True)

    if args.display_duration:
        config.set_display_duration(True)

    if args.display:
        config.set_display_optional_windows(True)

    debug = config.get_debug()
    display = config.get_display_optional_windows()

    if args.image:
        images = [None, None]
        for i in range(2):
            images[i] = cv.imread(args.image[i])
            if images[i] is None:
                sys_exit(f"Could not read the image {args.image[i]}")

    elif args.video:
        print(f"Trying to open: {args.video}")
        cap = cv.VideoCapture(args.video)
        if not cap.isOpened():
            cap.release()
            sys_exit("Can not read the video")

    else:
        print("Trying to open /dev/video0 with CAP_V4L2")
        cap = cv.VideoCapture(0, cv.CAP_V4L2)  # Add cv::CAP_V4L2 to fix: Embedded video playback halted; module v4l2src0 reported: Failed to allocate required memory.
        if not cap.isOpened():
            sys_exit("Can not read the device")
    
    if cap is not None:
        fps = cap.get(cv.CAP_PROP_FPS)
        # if fps = 60, so frame_to_take = 15, so we take a frame every 15 => (we take a frame every 0.25 sec)
        frame_to_take = int(fps / 4)
        # not initialized to 1 to take a frame directly
        frame_count = frame_to_take
        frame_wait_delay = int(1000 / fps)  # if fps = 30, delay = 33 ms
        print(f"fps: {fps}")
        print(f"frame took every: {frame_to_take}")
        print(f"frame wait delay: {frame_wait_delay} ms")

    if os.path.exists(RTH_PATH):
        os.remove(RTH_PATH)

    print("\nconfig:")
    print(f"debug: {debug}")
    print(f"display optional windows: {display}")
    print(f"display duration: {config.get_display_duration()}")
    config.display(sep='\n', start='')

    print("press [H] to print the help")

    if display:
        cv.namedWindow(window_prev_frame)
        cv.namedWindow(window_curr_frame)
        cv.namedWindow(window_mask)
    cv.namedWindow(window_result)
    cv.waitKey(500)

    while True:

        if has_to_break:
            break

        k = 0

        if pause and not config_changed:
            k = waitKey(500)

        else:
            duration = time()

            if not pause:
                if read_next_frame:
                    ret = False
                    frame = None

                    if images:
                        previous_frame = images[0]
                        frame = images[1]
                        ret = True
                    else:
                        ret, frame = cap.read()

                else:
                    read_next_frame = True
                    frame_count = frame_to_take
            else:
                frame_count = frame_to_take

            if ret and frame is not None:

                if frame_count >= frame_to_take:
                    frame_count = 1

                    # if we have a previous frame
                    if previous_frame is not None:

                        # check dimensions and depth
                        if frame.shape == previous_frame.shape and frame.dtype == previous_frame.dtype:

                            gray_image1 = cv.cvtColor(previous_frame, cv.COLOR_BGR2GRAY)
                            gray_image2 = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
                            if config.get_gaussian_blur() == 0:
                                diff_image = cv.absdiff(gray_image1, gray_image2)
                            else:
                                gaussian_image1 = cv.GaussianBlur(gray_image1, config.get_gaussian_kernel(), 0)
                                gaussian_image2 = cv.GaussianBlur(gray_image2, config.get_gaussian_kernel(), 0)
                                diff_image = cv.absdiff(gaussian_image1, gaussian_image2)

                            _, threshold_image = cv.threshold(diff_image, config.get_threshold(), 255, cv.THRESH_BINARY)
                            mask = cv.morphologyEx(threshold_image, cv.MORPH_OPEN, config.get_kernel())
                            
                            # Trouver tous les contours dans l'image
                            contours, _ = cv.findContours(mask, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)

                            # Trouver le plus grand contour dans l'image
                            max_area = -1
                            max_area_id = -1
                            for i in range(len(contours)):
                                current_area = cv.contourArea(contours[i])
                                #_, _, w, h = cv.boundingRect(contours[max_area_id])
                                #area = w * h

                                if debug:
                                    print(f"area {i}: {current_area}")
                                if current_area > max_area:
                                    max_area = current_area
                                    max_area_id = i

                            result = frame.copy()
                            something_detected = False

                            # Dessiner un rectangle autour du plus grand contour
                            if max_area >= config.get_detection_area():
                                x, y, w, h = cv.boundingRect(contours[max_area_id])
                                cv.rectangle(result, (x, y), (x+w, y+h), config.get_rectangle_color(), 2)
                                something_detected = True

                            duration = getTimeDiff(duration)
                            duration_average = ((duration_average * duration_average_count) + duration) / (duration_average_count + 1)
                            duration_average_count += 1

                            if config.get_display_duration():
                                print(f"{round3(duration)} ms")

                            if display:
                                cv.imshow(window_prev_frame, previous_frame)
                                cv.imshow(window_curr_frame, frame)
                                cv.imshow(window_mask, mask)
                                cv.imshow(window_result, result)
                                if debug:
                                    k = waitKey(0)

                            else:
                                cv.imshow(window_result, result)

                            if something_detected:
                                # TODO: detection_enabled in rtCam or in automate.py ??
                                if detection_enabled:
                                    # print("object detected!")
                                    if not os.path.exists(RTH_PATH):
                                        # create RTH file and close it automatically
                                        with open(RTH_PATH, 'x') as f:
                                            pass
                            else:
                                if not config_changed:
                                    previous_frame = frame

                            if clear_detection:
                                clear_detection = False
                                print("detection cleared")
                                previous_frame = frame

                        else:
                            print("frames haven't the same dimensions or depth!")
                            # replace last frame with the current
                            previous_frame = frame
                        
                        if images:
                            k = waitKey(0)

                    else:
                        # replace last frame with the current
                        previous_frame = frame

                    if k == 0:
                        k = waitKey(1)

                else:
                    frame_count += 1
                    k = waitKey(frame_wait_delay)

            else:
                if args.video:
                    break
                print("frame is empty")
                k = waitKey(500)

        config_changed = False

        if has_to_break or k == 27:     # ESC
            break

        elif k == 32:                   # SPACE BAR
            pause = not pause

        elif k == 67 or k == 99:        # C | c
            clear_detection = True

        elif k == 68 or k == 100:       # D | d
            config.inverse_display_duration()
            read_next_frame = not (debug or images)
        
        elif k == 72 or k == 104:       # H | h
            help()
            read_next_frame = not (debug or images)

        elif k == 73 or k == 105:       # I | i
            config.display()
            print(f"selected option: {selected_option.name}")
            read_next_frame = not (debug or images)

        elif k == 82 or k == 114:       # R | r
            config.reset()
            config.display()
            config_changed = True
            read_next_frame = not (debug or images)

        elif k == 83 or k == 115:       # S | s
            config.save()

        elif k == 71 or k == 103:       # G | g
            selected_option = Option.gaussian_blur
            print(f"selected option: {selected_option.name}")
            config_changed = True
            read_next_frame = not (debug or images)

        elif k == 84 or k == 116:       # T | t
            selected_option = Option.threshold
            print(f"selected option: {selected_option.name}")
            config_changed = True
            read_next_frame = not (debug or images)

        elif k == 75 or k == 107:       # K | k
            selected_option = Option.kernel_size
            print(f"selected option: {selected_option.name}")
            config_changed = True
            read_next_frame = not (debug or images)

        elif k == 65 or k == 97:        # A | a
            selected_option = Option.detection_area
            print(f"selected option: {selected_option.name}")
            config_changed = True
            read_next_frame = not (debug or images)

        elif k == 65361:  # Left Arrow
            read_next_frame = False
            config_changed = True

            if selected_option == Option.none:
                print("No option selected. Press [H] to print the help")

            elif selected_option == Option.gaussian_blur:
                if config.decrease_gaussian_blur():
                    config.display()

            elif selected_option == Option.threshold:
                if config.decrease_threshold():
                    config.display()

            elif selected_option == Option.kernel_size:
                if config.decrease_kernel_size():
                    config.display()

            elif selected_option == Option.detection_area:
                if config.decrease_detection_area():
                    config.display()

        elif k == 65363:  # Right Arrow
            read_next_frame = False
            config_changed = True

            if selected_option == Option.none:
                print("No option selected. Press H to see help")

            elif selected_option == Option.gaussian_blur:
                if config.increase_gaussian_blur():
                    config.display()

            elif selected_option == Option.threshold:
                if config.increase_threshold():
                    config.display()

            elif selected_option == Option.kernel_size:
                if config.increase_kernel_size():
                    config.display()

            elif selected_option == Option.detection_area:
                if config.increase_detection_area():
                    config.display()

        if images and read_next_frame:
            break

    print(f"Average duration: {duration_average} ms ({duration_average_count} samples)")

    if cap:
        cap.release()
    cv.destroyAllWindows()

    if os.path.exists(RTH_PATH):
        os.remove(RTH_PATH)


if __name__ == '__main__':
    signal.signal(signal.SIGINT, sigint_handler)
    parser = argparse.ArgumentParser(description="RealTime Camera main.py CLI")
    parser.add_argument("-i", "--image", type=str, nargs=2, help="process with two given images")
    parser.add_argument("-v", "--video", type=str, help="process with a given video")
    parser.add_argument("-d", "--display", action="store_true", help="enable views. Takes resources! Not recommanded in production")
    parser.add_argument("-dd", "--display_duration", action="store_true", help="display duration will be automatically enabled")
    parser.add_argument("-db", "--debug", action="store_true", help="shortcut to -d and -dd")

    try:
        main()
    except BaseException as e:
        print(e.args[0])
        input("press enter to close...")
