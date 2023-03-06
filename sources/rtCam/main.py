import cv2 as cv
import argparse
import sys
import os
import signal
from time import time
import numpy as np

global has_to_break


def sigint_handler(signal, frame):
    global has_to_break
    has_to_break = True


def help():
    print("")
    print("--- Function -------   --- KEY -----")
    print("")
    print("    Clear detection        C")
    print("    Decrease threshold     Left  Arrow")
    print("    Decrease kernel size   Down  Arrow")
    print("    Display duration       D")
    print("    Increase threshold     Right Arrow")
    print("    Increase kernel size   Up    Arrow")
    print("    Help                   H")
    print("    Pause                  SPACE BAR")
    print("    Quit                   ESC")
    print("    Reset config           R")
    print("")
    print("----------------------------------")


def main():

    global has_to_break

    has_to_break = False

    args = parser.parse_args()

    cap = None
    k = 0
    arrow_keys = [81, 82, 83, 84]
    duration = 0.0

    fps = 0
    frame_to_take = 0
    frame_count = 0
    frame_wait_delay = 0

    images = []
    previous_frame = None

    gray_image1 = None
    gray_image2 = None
    diff_image = None

    debug = False
    display = False
    pause = False
    read_next_frame = True
    display_duration = False
    detection_enabled = True
    clear_detection = False

    # to display red rectangle
    contours=[]
    red = (0, 0, 255)
    detect_min_area = 500

    # Pour la binarisation de l'image
    threshold_image = None
    threshold_value = 70   # seuil de binarisation

    # Pour l'ouverture morphologique
    result = None
    kernel_size = 6
    kernel = np.ones((kernel_size, kernel_size), np.uint8)
    mask = None

    window_prev_frame = "Previous frame"
    window_curr_frame = "Current frame"
    window_mask = "Mask"
    window_result = "Result"

    if args.debug:
        debug = True

    if debug or args.display_duration:
        display_duration = True

    if debug or args.display:
        display = True

    if args.image:
        images = [None, None]
        for i in range(2):
            path = cv.samples.findFile(args.image[i])
            if not path:
                sys.exit(f"Cannot find: {args.image[i]}")
            images[i] = cv.imread(path)
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
        # if fps = 60, so frame_to_take = 15, so we take a frame every 15 => (we take a frame every 0.25 sec)
        frame_to_take = int(fps / 4)
        # not initialized to 1 to take a frame directly
        frame_count = frame_to_take
        frame_wait_delay = int(1000 / fps)  # if fps = 30, delay = 33 ms
        print(f"fps: {fps}")
        print(f"frame took every: {frame_to_take}")
        print(f"frame wait delay: {frame_wait_delay} ms")


    RTH_PATH = "../automate/RTH"
    if debug or args.image or args.video:
        RTH_PATH = "./RTH"

    if os.path.exists(RTH_PATH):
        os.remove(RTH_PATH)

    print("press [H] to print the help")
    print(f"\nthreshold: {threshold_value}\tkernel: {kernel_size}\n")

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

        if pause:
            k = cv.waitKey(500) & 0xFF

        else:
            duration = time()

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

            if ret and frame is not None:

                if frame_count >= frame_to_take:
                    frame_count = 1

                    # if we have a previous frame
                    if previous_frame is not None:

                        # check dimensions and depth
                        if frame.shape == previous_frame.shape and frame.dtype == previous_frame.dtype:

                            gray_image1 = cv.cvtColor(previous_frame, cv.COLOR_BGR2GRAY)
                            gray_image2 = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
                            # gray_image1 = cv.GaussianBlur(gray_image1, (5, 5), 0)
                            # gray_image2 = cv.GaussianBlur(gray_image2, (5, 5), 0)
                            diff_image = cv.absdiff(gray_image1, gray_image2)

                            # way 1 : threshold, morpho
                            _, threshold_image = cv.threshold(diff_image, threshold_value, 255, cv.THRESH_BINARY)
                            mask = cv.morphologyEx(threshold_image, cv.MORPH_OPEN, kernel)
                            
                            # way 2 : Canny filter
                            # mask = cv.Canny(diff_image, 100, 200)
                           
                            # Trouver tous les contours dans l'image
                            contours, _ = cv.findContours(mask, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)

                            # Trouver le plus grand contour dans l'image
                            maxArea = -1
                            maxAreaId = -1
                            for i in range(len(contours)):
                                area = cv.contourArea(contours[i])
                                #_, _, w, h = cv.boundingRect(contours[maxAreaId])
                                #area = w * h

                                if debug:
                                    print(f"area {i}: {area}")
                                if area > maxArea:
                                    maxArea = area
                                    maxAreaId = i

                            result = frame.copy()
                            something_detected = False

                            # Dessiner un rectangle autour du plus grand contour
                            if maxArea >= detect_min_area:
                                x, y, w, h = cv.boundingRect(contours[maxAreaId])
                                cv.rectangle(result, (x, y), (x+w, y+h), red, 2)
                                something_detected = True

                            if display:
                                cv.imshow(window_prev_frame, previous_frame)
                                cv.imshow(window_curr_frame, frame)
                                cv.imshow(window_mask, mask)
                                cv.imshow(window_result, result)
                                if debug:
                                    k = cv.waitKey(0) & 0xFF

                            else:
                                cv.imshow(window_result, result)

                            if something_detected:
                                # TODO: detection_enabled in rtCam or in automate.py ??
                                if detection_enabled:
                                    # print("Object detected!")
                                    if not os.path.exists(RTH_PATH):
                                        f = open(RTH_PATH, "x")  # create RTH file
                                        f.close()
                            else:
                                if k not in arrow_keys:
                                    previous_frame = frame

                            if clear_detection:
                                clear_detection = False
                                print("detection cleared")
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
                            k = cv.waitKey(0) & 0xFF

                    else:
                        # replace last frame with the current
                        previous_frame = frame

                    if k == 0:
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

        if k == 72 or k == 104:     # H | h
            help()

        elif k == 68 or k == 100:   # D | d
            display_duration = not display_duration
            print(f"display duration: {display_duration}\n")

        elif k == 67 or k == 99:   # C | c
            clear_detection = True

        elif k == 82 or k == 114:   # R | r
            threshold_value = 70
            kernel_size = 6
            kernel = np.ones((kernel_size, kernel_size), np.uint8)
            print(f"\nthreshold: {threshold_value}\tkernel: {kernel_size}\n")


        elif k == 32:               # SPACE BAR
            pause = not pause

        elif k == 81:  # Left Arrow
            if threshold_value > 0:
                threshold_value -= 1
                print(f"\nthreshold: {threshold_value}\tkernel: {kernel_size}\n")
                read_next_frame = False

        elif k == 83:  # Right Arrow
            if threshold_value < 255:
                threshold_value += 1
                print(f"\nthreshold: {threshold_value}\tkernel: {kernel_size}\n")
                read_next_frame = False

        elif k == 84:  # Down Arrow
            if kernel_size > 1:
                kernel_size -= 1
                kernel = np.ones((kernel_size, kernel_size), np.uint8)
                print(f"\nthreshold: {threshold_value}\tkernel: {kernel_size}\n")
                read_next_frame = False

        elif k == 82:  # Up Arrow
            if kernel_size < 255:
                kernel_size += 1
                kernel = np.ones((kernel_size, kernel_size), np.uint8)
                print(f"\nthreshold: {threshold_value}\tkernel: {kernel_size}\n")
                read_next_frame = False

        if images and read_next_frame:
            break

    if cap:
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
