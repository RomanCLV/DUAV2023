import argparse
import cv2 as cv
import numpy as np
import socket
import struct
import sys
import os
import signal
import ipaddress


fps_choices = [1, 5, 10, 12, 15, 20, 24, 25, 29.97, 30, 48, 50, 59.94, 60, 120, 144, 240, 300]


def is_valid_ip(ip: str):
    try:
        ipaddress.ip_address(ip)
        return True
    except ValueError:
        return False


def is_valid_port(port: int):
    return 1024 <= port <= 65535


def recv_frame_whole(sock):
    try:
        data, addr = sock.recvfrom(65535)
        frame = np.frombuffer(data, dtype=np.uint8)
        return cv.imdecode(frame, cv.IMREAD_COLOR)
    except socket.timeout:
        return None


def recv_frame_sliced(sock):
    num_packets = 0
    packet_data = bytearray()

    while True:
        try:
            data, addr = sock.recvfrom(65535)
            num, offset = struct.unpack('!II', data[:8])
            chunk = data[8:]

            packet_data[offset * (len(data) - 8):] = chunk

            if len(packet_data) >= num * (len(data) - 8):
                break
        except socket.timeout:
            return None

    frame = np.frombuffer(packet_data, dtype=np.uint8)
    return cv.imdecode(frame, cv.IMREAD_COLOR)


def send_udp_key(value, sock, address):
    packed_value = struct.pack("!i", value)
    sock.sendto(packed_value, address)


def main(args):

    auto_change_ip = False
    if args.auto_change_ip:
        auto_change_ip = True

    udp_ip = args.ip
    udp_port = args.port
    udp_port2 = args.port2

    if not is_valid_ip(udp_ip):
        print(f"The address {udp_ip} is invalid! Please give an address like X.X.X.X where X is in [0-255]")
        sys.exit(1)

    if not is_valid_port(udp_port):
        print(f"The port {udp_port} is invalid! Please give a port from 1024 to 65535")
        sys.exit(1)

    if not is_valid_port(args.port2):
        print(f"The port {args.port2} is invalid! Please give a port from 1024 to 65535")
        sys.exit(1)

    if udp_port == udp_port2:
        print(f"The listened port ({udp_port}) and the writed port ({args.port2}) must be different")
        sys.exit(1)
        
    display = False
    write = False
    video = None
    frame_fps = 30

    if args.display:
        display = True

    if args.fps:
        frame_fps = args.fps

    if args.output:
        write = True
        file_extension = os.path.splitext(args.output)[1]
        if file_extension.lower() != ".avi":
            print("Error : the video file extension must be .avi")
            sys.exit(1)

    udp_address = (udp_ip, udp_port)
    udp_address2 = (udp_ip, udp_port2)

    print(f"Opening a socket to {udp_address[0]}:{udp_address[1]}")
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(3.0) # 3 seconds

    try:
        sock.bind(udp_address)

    except socket.error as e:
        print(f"Socket error: {e}")

        if (e.errno == 10049 or e.errno == 99) and auto_change_ip:
            print(f"Changing ip to listen to: 0.0.0.0")
            udp_address = ("0.0.0.0", udp_port)
            sock.bind(udp_address)
        else:
            sys.exit(1)

    print(f"Opening a socket to {udp_address2[0]}:{udp_address2[1]}")
    sock2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    window_name = f"Listening: {udp_address[0]}:{udp_address[1]} | Send: {udp_address2[0]}:{udp_address2[1]}"

    if display:
        cv.namedWindow(window_name)
        cv.waitKey(500)

    k = 0
    print(f"Listening: {udp_address[0]}:{udp_address[1]}")
    print(f"Send to:   {udp_address2[0]}:{udp_address2[1]}")
    print("press ESC to quit\n")

    try:
        while True:
            k = 0

            frame = recv_frame_sliced(sock)

            if frame is None:
                print("No frame received")
                k = cv.waitKeyEx(500)

            else:
                if write:
                    if video is None:
                        fourcc = cv.VideoWriter_fourcc(*'XVID')
                        video = cv.VideoWriter(args.output, fourcc, frame_fps, (frame.shape[1], frame.shape[0]))
                    video.write(frame)

                if display:
                    cv.imshow(window_name, frame)

                k = cv.waitKeyEx(1)

            if k != -1 and k != 27 and sock2:
                send_udp_key(k, sock2, udp_address2)

            if k == 27:
                break
                
    except KeyboardInterrupt:
        pass

    finally:
        if video:
            video.release()

        if display:
            cv.destroyAllWindows()

        sock.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='rtCam listener CLI')
    parser.add_argument('ip', type=str, help='IP address to listen')
    parser.add_argument('port', type=int, help='Port to listen')
    parser.add_argument('port2', type=int, help='Port to send the pressed key')
    parser.add_argument('-aci', '--auto_change_ip', action='store_true', help="Auto change ip to 0.0.0.0 if socket can't bind to the given ip")
    parser.add_argument('-o', '--output', type=str, help='Path to save the video file')
    parser.add_argument('-d', '--display', action='store_true', help='Display received frames in a window')
    parser.add_argument('-fps', '--fps', type=int, default=30, choices=fps_choices, help='Frames per second for the video (default: 30)')

    args = parser.parse_args()

    main(args)
