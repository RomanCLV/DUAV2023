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


def send_frame(sock, udp_ip, udp_port, frame):
    # Encode the frame as a byte string
    encoded_frame = cv.imencode('.jpg', frame)[1].tobytes()

    # Send the encoded frame via UDP
    sock.sendto(encoded_frame, (udp_ip, udp_port))


def send_frame_splitted(sock, udp_ip, udp_port, frame, max_size=65507):
    if max_size > 65507 or max_size < 1:
        raise ValueError(f"max_size must be between 1 and 65507. Given {max_size}")
        
    # Diviser l'image en morceaux de n octets (la taille maximale autorisée pour les datagrammes UDP)    
    frame_bytes = frame.tobytes()
    n_chunks = len(frame_bytes) // max_size + 1
    chunks = [frame_bytes[i * max_size:(i + 1) * max_size] for i in range(n_chunks)]

    # Envoyer chaque morceau par UDP
    for i, chunk in enumerate(chunks):
        message = np.array([i, n_chunks], dtype=np.uint16).tobytes() + chunk
        sock.sendto(message, (udp_ip, udp_ip))


def main():
    global has_to_break
    has_to_break = False
    
    display = False
    write = False

    args = parser.parse_args()

    if args.display:
        display = True
        
    if args.output:
        write = True

    # Configure the socket for UDP
    UDP_IP   = args.udp_address # IP address of the PC on the same network
    UDP_PORT = args.udp_port    # port of the PC on the same network
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    print("Trying to open /dev/video0 with CAP_V4L2")
    cap = cv.VideoCapture(0, cv.CAP_V4L2)  # Add cv::CAP_V4L2 to fix: Embedded video playback halted; module v4l2src0 reported: Failed to allocate required memory.

    if not cap.isOpened():
        sys.exit("Can not read the device")
    
    video = None
    if write:
        frame_width = int(cap.get(cv.CAP_PROP_FRAME_WIDTH))
        frame_height = int(cap.get(cv.CAP_PROP_FRAME_HEIGHT))
        frame_fps = cap.get(cv.CAP_PROP_FPS) 
            
        video = cv.VideoWriter(args.output, cv.VideoWriter_fourcc('M','J','P','G'), frame_fps, (frame_width, frame_height))


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

            # Envoie la frame entiere : Erreur UDP : MessageTooLong
            # send_frame(sock, UDP_IP, UDP_PORT, frame)
            
            send_frame_splitted(sock, UDP_IP, UDP_PORT, frame)
            
            if write:
                video.write(frame)

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
    if video:
        video.release()

    # Destroy all the windows
    cv.destroyAllWindows()


if __name__ == '__main__':
    signal.signal(signal.SIGINT, sigint_handler)
    parser = argparse.ArgumentParser(description="Camera Capture UDP - Sender main.py CLI")
    parser.add_argument("udp_address", type=str, help="udp address to send")
    parser.add_argument("udp_port", type=int, help="udp port to send")
    parser.add_argument("-d", "--display", action="store_true", help="display a window of the camera")
    parser.add_argument("-o", "--output", type=str, help="write a video file")
    main()
