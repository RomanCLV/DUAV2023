import argparse
import cv2 as cv
import numpy as np
import socket
import struct
import sys
import os
import signal


fps_choices = [1, 5, 10, 12, 15, 20, 24, 25, 29.97, 30, 48, 50, 59.94, 60, 120, 144, 240, 300]


def recv_frame_whole(sock):
    try:
        data, addr = sock.recvfrom(65535)
        frame = np.frombuffer(data, dtype=np.uint8)
        return cv.imdecode(frame, cv.IMREAD_COLOR)
    except socket.timeout:
        return None


def recv_frame_sliced(sock):
    num_packets, packet_data = 0, bytearray()

    while True:
        try:
            data, addr = sock.recvfrom(65535)
            num, chunk = struct.unpack('!II', data[:8]), data[8:]
            num_packets = max(num_packets, num)

            packet_data.extend(chunk)

            if len(packet_data) >= num_packets * (len(data) - 8):
                break
        except socket.timeout:
            return None

    frame = np.frombuffer(packet_data, dtype=np.uint8)
    return cv.imdecode(frame, cv.IMREAD_COLOR)


def main(args):

    udp_ip = args.ip
    udp_port = args.port

    display = False
    write = False
    video = None
    frame_fps = 30
    window_name = "window"

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

    window_name = f"Received from {udp_ip}:{udp_port}"

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((udp_ip, udp_port))
    # Définir un délai d'attente (timeout) pour le socket en secondes
    sock.settimeout(1.0)

    if display:
        cv.namedWindow(window_name, cv.WINDOW_NORMAL)
        cv.waitKey(500)

    try:
        while True:
            #if args.receive_split:
            #    frame = recv_frame_sliced(sock)
            #else:
            #    frame = recv_frame_whole(sock)
            frame = recv_frame_sliced(sock)

            if frame is None:
                print("No frame received")
                cv.waitKey(500)
                continue

            if write:
                if video is None:
                    fourcc = cv.VideoWriter_fourcc(*'XVID')
                    video = cv.VideoWriter(args.output, fourcc, frame_fps, (frame.shape[1], frame.shape[0]))
                video.write(frame)

            if display:
                cv.imshow(namedWindow, frame)
            
            if cv.waitKey(1) & 0xFF == 27:
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
    parser = argparse.ArgumentParser(description='Receive video frames via UDP.')
    parser.add_argument('ip', type=str, help='IP address to bind to')
    parser.add_argument('port', type=int, help='Port to bind to')
    parser.add_argument('-o', '--output', type=str, help='Path to save the video file')
    parser.add_argument('-d', '--display', action='store_true', help='Display received frames in a window')
    #parser.add_argument('-s', '--receive_split', action='store_true', help='Receive frames split into smaller packets')
    parser.add_argument('-fps', '--fps', type=int, default=30, fps_choices=fps_choices, help='Frames per second for the video (default: 30)')

    args = parser.parse_args()

    main(args)
