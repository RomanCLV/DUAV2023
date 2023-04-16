# https://learnopencv.com/read-write-and-display-a-video-using-opencv-cpp-python/

import cv2 as cv
import argparse
import sys
import signal
import socket


global has_to_break


def sigint_handler(signal, frame):
    global has_to_break
    has_to_break = True


def main():
    global has_to_break
    has_to_break = False
    
    display = False

    args = parser.parse_args()

    if args.display:
        display = True

    # Configure the socket for UDP
    UDP_IP = args.upd_address # IP address of the PC on the same network
    UDP_PORT = 5005           # port address
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    print("Trying to open /dev/video0 with CAP_V4L2")
    cap = cv.VideoCapture(0, cv.CAP_V4L2)  # Add cv::CAP_V4L2 to fix: Embedded video playback halted; module v4l2src0 reported: Failed to allocate required memory.

    if not cap.isOpened():
        sys.exit("Can not read the device")

    print("Press ESC to quit")
    window_name = "Camera Capture"
    
    if display:
        cv.namedWindow(window_name)
        cv.waitKey(500)
    k = 0

    while True:
          
        ret, frame = cap.read()
        
        if not (ret == False or frame is None):
            if display:
                cv.imshow(window_name, frame)

            # Encode the frame as a byte string
            encoded_frame = cv.imencode('.jpg', frame)[1].tobytes()

            # Send the encoded frame via UDP
            sock.sendto(encoded_frame, (UDP_IP, UDP_PORT))


            k = cv.waitKey(1) & 0xFF

        else:
            print("frame is empty")
            k = cv.waitKey(500) & 0xFF

        # break the loop if Ctrl+C or key ESC
        if has_to_break or k == 27:
            break

    print("Capture done")
    # After the loop release the cap object
    cap.release()

    # Destroy all the windows
    cv.destroyAllWindows()


if __name__ == '__main__':
    signal.signal(signal.SIGINT, sigint_handler)
    parser = argparse.ArgumentParser(description="Camera Capture UDP - Sender main.py CLI")
    parser.add_argument("upd_address", type=str, help="udp address to send")
    parser.add_argument("-d", "--display", action="store_true", help="display a window of the camera")
    main()
