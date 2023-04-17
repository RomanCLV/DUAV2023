import argparse
import cv2 as cv
import numpy as np
import socket
import struct
import sys
import os
import signal


def send_frame_udp(frame, address, sock, max_packet_size=65535):
    data = cv.imencode('.jpg', frame)[1].tobytes()
    num_packets = len(data) // max_packet_size + 1

    for i in range(num_packets):
        packet_data = data[i * max_packet_size: (i + 1) * max_packet_size]
        packet = struct.pack('!I', i) + packet_data
        sock.sendto(packet, address)


def send_frame_udp_split(frame, address, sock, max_packet_size=65535):
    data = cv.imencode('.jpg', frame)[1].tobytes()
    num_packets = len(data) // max_packet_size + 1

    for i in range(num_packets):
        packet_data = data[i * max_packet_size: (i + 1) * max_packet_size]
        packet = struct.pack('!II', num_packets, i) + packet_data
        sock.sendto(packet, address)


def main(args):
    
    udp_ip = args.ip
    udp_port = args.port

    write = False
    display = False
    video = None

    if args.display:
        display = True

    if args.output:
        write = True
        file_extension = os.path.splitext(args.output)[1]
        if file_extension.lower() != ".avi":
            print("Error : the video file extension must be .avi")
            sys.exit(1)

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
        cv.namedWindow(window_name, cv.WINDOW_NORMAL)
        cv.waitKey(500)

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    try:
        while True:
            ret, frame = cap.read()

            if not ret:
                print("frame is empty")
                cv.waitKey(500)
                continue
                
            if write:
                video.write(frame)

            # if args.send_split:
            #     send_frame_udp_split(frame, (udp_ip, udp_port), sock)
            # else:
            #     send_frame_udp(frame, (udp_ip, udp_port), sock)
            send_frame_udp_split(frame, (udp_ip, udp_port), sock)

            if display:
                cv.imshow(window_name, frame)
            
            if cv.waitKey(1) & 0xFF == 27:  # 27 is the ESC key
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
    parser.add_argument('-o', '--output', type=str, help='Path to save the video file')
    parser.add_argument('-d', '--display', action='store_true', help='Display video frames in a window')
    # parser.add_argument('-s', '--send_split', action='store_true', help='Send frames split into smaller packets')

    args = parser.parse_args()

    main(args)
