import argparse
import cv2 as cv
import numpy as np
import socket
import struct
import sys
import os
import signal
import ipaddress


def is_valid_ip(ip: str):
    try:
        ipaddress.ip_address(ip)
        return True
    except ValueError:
        return False


def is_valid_port(port: int):
    return 1024 <= port <= 65535


def send_frame_udp(frame, address, sock, max_packet_size=65507):
    data = cv.imencode('.jpg', frame)[1].tobytes()
    num_packets = len(data) // max_packet_size + 1

    for i in range(num_packets):
        packet_data = data[i * max_packet_size: (i + 1) * max_packet_size]
        packet = struct.pack('!I', i) + packet_data
        try:
            sock.sendto(packet, address)
        except socket.error as e:
            if e.errno == 90:  # MessageTooLong error code
                # norlally, we shouldn't have this error (thanks to max_packet_size), but to be sure...
                print("Packet too large, skipping...")
                continue
            else:
                raise e


def send_frame_udp_split(frame, address, sock, max_packet_size=65507):
    data = cv.imencode('.jpg', frame)[1].tobytes()
    num_packets = len(data) // max_packet_size + 1

    for i in range(num_packets):
        packet_data = data[i * max_packet_size: (i + 1) * max_packet_size]
        packet = struct.pack('!II', num_packets, i) + packet_data
        try:
            sock.sendto(packet, address)
        except socket.error as e:
            if e.errno == 90:  # MessageTooLong error code
                # norlally, we shouldn't have this error (thanks to max_packet_size), but to be sure...
                print("Packet too large, skipping...")
                continue
            else:
                raise e


def main(args):
    
    udp_ip = args.ip
    udp_port = args.port

    write = False
    display = False
    video = None

    if not is_valid_ip(udp_ip):
        print(f"The address {udp_ip} is invalid! Please give an address like X.X.X.X where X is in [0-255]")
        sys.exit(1)

    if not is_valid_port(udp_port):
        print(f"The port {udp_port} is invalid! Please give a port from 1024 to 65535")
        sys.exit(1)

    if args.display:
        display = True

    if args.output:
        write = True
        file_extension = os.path.splitext(args.output)[1]
        if file_extension.lower() != ".avi":
            print("Error : the video file extension must be .avi")
            sys.exit(1)

    print(f"Frames will be send to {udp_ip}:{udp_port}")

    print("Trying to open /dev/video0 with CAP_V4L2")
    cap = cv.VideoCapture(0, cv.CAP_V4L2)  # Add cv::CAP_V4L2 to fix: Embedded video playback halted; module v4l2src0 reported: Failed to allocate required memory.

    if not cap.isOpened():
        sys.exit("Can not read the device")

    if write:
        fourcc = cv.VideoWriter_fourcc(*'XVID')
        frame_width = int(cap.get(cv.CAP_PROP_FRAME_WIDTH))
        frame_height = int(cap.get(cv.CAP_PROP_FRAME_HEIGHT))
        frame_fps = cap.get(cv.CAP_PROP_FPS) 
        video = cv.VideoWriter(args.output, fourcc, frame_fps, (frame_width, frame_height))

    window_name = f"Send to {udp_ip}:{udp_port}"
    if display:
        cv.namedWindow(window_name)
        cv.waitKey(500)

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    k = 0
    print("press ESC to quit")

    try:
        while True:
            k = 0

            ret, frame = cap.read()

            if not ret:
                print("frame is empty")
                k = cv.waitKey(500) & 0xFF
            
            else:
                if write:
                    video.write(frame)

                send_frame_udp_split(frame, (udp_ip, udp_port), sock)

                if display:
                    cv.imshow(window_name, frame)
                k = cv.waitKey(1) & 0xFF

            if k == 27:  # 27 is the ESC key
                break

    except KeyboardInterrupt:
        pass

    finally:
        cap.release()

        if video:
            video.release()

        if display:
            cv.destroyAllWindows()

        sock.close()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Stream video frames via UDP.')
    parser.add_argument('ip', type=str, help='Destination IP address')
    parser.add_argument('port', type=int, help='Destination port')
    parser.add_argument('-o', '--output', type=str, help='Path to save the video file (.avi)')
    parser.add_argument('-d', '--display', action='store_true', help='Display video frames in a window')

    args = parser.parse_args()

    main(args)
