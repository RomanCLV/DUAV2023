import cv2 as cv
import numpy as np
import socket
import argparse
import signal


global has_to_break


def sigint_handler(signal, frame):
    global has_to_break
    has_to_break = True


def main():
    global has_to_break
    has_to_break = False

    args = parser.parse_args()

    # Configure the socket for UDP
    UDP_IP = args.udp_address  # IP address of the Raspberry Pi on the same network
    UDP_PORT = 5005
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((UDP_IP, UDP_PORT))

    while True:
        # Receive the encoded frame via UDP
        encoded_frame, address = sock.recvfrom(65507)

        # Decode the frame from a byte string
        frame = cv.imdecode(np.frombuffer(encoded_frame, dtype=np.uint8), cv.IMREAD_COLOR)

        # Display the frame
        cv.imshow('frame from ', frame)
        if cv.waitKey(1) & 0xFF == ord('q'):
            break

    # Release the resources
    cv.destroyAllWindows()


if __name__ == '__main__':
    signal.signal(signal.SIGINT, sigint_handler)
    parser = argparse.ArgumentParser(description="Camera Capture UDP - Receiver main.py CLI")
    parser.add_argument("udp_address", type=str, help="udp address to listen")
    main()

