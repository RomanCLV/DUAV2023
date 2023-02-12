import cv2 as cv
import argparse
import sys
import signal


global has_to_break


def sigint_handler(signal, frame):
    global has_to_break
    has_to_break = True


def main():
    global has_to_break
    has_to_break = False
    
    args = parser.parse_args()

    print("Trying to open /dev/video0 with CAP_V4L2")
    cap = cv.VideoCapture(0, cv.CAP_V4L2)

    if not cap.isOpened():
        sys.exit("Can not read the device")

    window_name = "Camera Capture"
    while True:
          
        ret, frame = cap.read()
        
        if ret == False or frame is None:
            print("frame is empty")
            if has_to_break:
                break
            cv.waitKey(500)
            continue

        if args.grayscale:
            frame = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
    
        cv.imshow(window_name, frame)

        if cv.waitKey(1) == 27 or has_to_break:  #  or cv.getWindowProperty(window_name, cv.WND_PROP_VISIBLE) == -1
            break

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
