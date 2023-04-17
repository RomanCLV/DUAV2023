import cv2 as cv
import numpy as np
import socket
import argparse
import signal


global has_to_break


fps_choices = [1, 5, 10, 12, 15, 20, 24, 25, 29.97, 30, 48, 50, 59.94, 60, 120, 144, 240, 300]


def sigint_handler(signal, frame):
    global has_to_break
    has_to_break = True


def listen_frame(sock, max_size = 65507):
    if max_size > 65507 or max_size < 1:
        raise ValueError(f"max_size must be between 1 and 65507. Given {max_size}")
        
    # Receive the encoded frame via UDP
    encoded_frame, address = sock.recvfrom(max_size)

    # Decode the frame from a byte string
    return cv.imdecode(np.frombuffer(encoded_frame, dtype=np.uint8), cv.IMREAD_COLOR)


def listen_frame_splitted(sock, max_size = 65507):
    if max_size > 65507 or max_size < 1:
        raise ValueError(f"max_size must be between 1 and 65507. Given {max_size}")
        
    # Réception et reconstruction de l'image à partir des morceaux reçus
    img_bytes = b''
    n_chunks = None
    i = 0
    while True:
        data, addr = sock.recvfrom(max_size + 4)                            # On reçoit un message UDP
        seq_num, total_chunks = np.frombuffer(data[:4], dtype=np.uint16)    # On lit l'en-tête
        chunk = data[4:]
        if n_chunks is None:
            n_chunks = total_chunks
            img_bytes = b'\x00' * (n_chunks * max_size)                     # On initialise le buffer pour l'image complète
        img_bytes[seq_num * max_size:(seq_num + 1) * max_size] = chunk      # On ajoute le morceau au buffer
        i += 1
        if i == n_chunks:
            break

    # Convertir les octets en image OpenCV
    return cv.imdecode(np.frombuffer(img_bytes, dtype=np.uint8), cv.IMREAD_COLOR)


def main():
    global has_to_break
    has_to_break = False

    write = False
    video_fps = 30

    args = parser.parse_args()
    
    if args.output:
        write = True
    
    if args.fps:
        video_fps = args.fps
    
    video = None

    # Configure the socket for UDP
    UDP_IP   = args.udp_address  # IP address of the Raspberry Pi on the same network
    UDP_PORT = args.udp_port     # listened port
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((UDP_IP, UDP_PORT))

    window_name = f"Stream from {UDP_IP}:{UDP_PORT}"
    cv.namedWindow(window_name)
    k = 0
    
    while True:
        
        # Recoie la frame en entier.
        # frame = listen_frame(sock)
        
        # Recoie une frame découpée
        frame = listen_frame_splitted(sock)
        
        if write:
            if video is None:
                frame_height, frame_width, frame_channels = frame.shape
                video = video = cv.VideoWriter(args.output, cv.VideoWriter_fourcc('M','J','P','G'), video_fps, (frame_width, frame_height))
            video.write(frame)
            
        # Display the frame
        cv.imshow(window_name, frame)
        k = cv.waitKey(1) & 0xFF
        
        
        if has_to_break or k == 27:
            break

    # Release the resources
    cv.destroyAllWindows()


if __name__ == '__main__':
    signal.signal(signal.SIGINT, sigint_handler)
    parser = argparse.ArgumentParser(description="Camera Capture UDP - Receiver main.py CLI")
    parser.add_argument("udp_address", type=str, help="udp address to listen")
    parser.add_argument("udp_port", type=int, help="udp port to listen")
    parser.add_argument("-o", "--output", type=str, help="write a video file")
    parser.add_argument("-fps" "--fps", type=float, choices=fps_choices, help="fps of the output video")
    
    main()

