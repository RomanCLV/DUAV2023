import cv2 as cv
import numpy as np
import argparse
import sys
import os
import signal
import socket
import struct
import ipaddress
import psutil
import traceback
from time import time
from datetime import datetime
from enum import Enum
import config as cnf


class Option(Enum):
    none = 0,
    gaussian_blur = 1,
    threshold = 2,
    kernel_size = 3,
    detection_area = 4


global has_to_break

global cap
global video_detection
global video_result_without_detection
global video_result
global video_mask
global sock
global sock2


def sigint_handler(signal, frame):
    global has_to_break
    has_to_break = True


def help():
    print("")
    print("--- Function -------   --- KEY -----")
    print("")
    print("    Clear detection        C")
    print("    Display duration       D")
    print("    Decrease option        Left Arrow")
    print("    Increase option        Right Arrow")
    print("    Help                   H")
    print("    Pause                  SPACE BAR")
    print("    Quit                   ESC")
    print("    Display config         I")
    print("    New config             N")
    print("    Reset config           R")
    print("    Save config            S")
    print("")
    print(" Select an option:")
    print("")
    print("    Gaussian blur          G")
    print("    Threshold              T")
    print("    Kernel size            K")
    print("    Detection area         A")
    print("")
    print("----------------------------------")


def display_full_config(config, debug, display, display_windows, display_duration, udp_address, udp_address2):
    print("\nconfig:")
    print(f"debug: {debug}")
    print(f"display: {display}")
    print(f"display optional windows: {display_windows}")
    print(f"display duration: {display_duration}")
    print(f"save detection: {config.get_save_detection()}")
    print(f"save result without detection: {config.get_save_result_without_detection()}")
    print(f"save result: {config.get_save_result()}")
    print(f"save mask: {config.get_save_mask()}")
    print(f"udp: {config.get_udp_enabled()}")
    if config.get_udp_enabled():
        print(f"  to  : {udp_address[0]}:{udp_address[1]}")
        print(f"  from: {udp_address2[0]}:{udp_address2[1]}")
        print(f"  auto change ip: {config.get_udp_auto_change_ip()}")

    config.display(sep='\n', start='')


def waitKey(millis: int):
    return cv.waitKeyEx(millis) & 0xFFFF


def getTimeDiff(timeStart: float):
    return 1000 * (time() - timeStart)


def round3(value: float):
    return round(value * 1000) / 1000.0;


def sys_exit(message: str, exit_code=-1):
    print(message)
    input("press entre to close...")
    sys.exit(exit_code)


def create_folder_if_not_exists(folder_name: str):
    if not os.path.exists(folder_name):
        os.makedirs(folder_name)


def is_ssh_connected(display_procces_info=True):
    is_ssh = False

    # PID du processus dont on veut afficher les parents
    current_process = psutil.Process()
    pid = os.getpid()

    if display_procces_info:
        print("current PID:", pid, "Name:", current_process.name())

    # Tant que le PID du processus parent n'est pas égal à 0 (le PID du processus init)
    while pid != 0:
        try:
            parent = psutil.Process(pid)
            pid = parent.ppid()

            if display_procces_info:
                print("Parent PID:", pid, "Name:", parent.name())
            
            if not is_ssh and parent.name() in ["ssh", "sshd"]:
                is_ssh = True
                    
        except psutil.NoSuchProcess:
            break
    return is_ssh


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


def listen_key_udp(sock, buffer_size=1024):
    value = 0
    try:
        data, addr = sock.recvfrom(buffer_size)
        value = struct.unpack("!i", data)[0]

        if value == 2424832:     # Left arraow
            value = 65361

        elif value == 2555904:   # Right arrow
            value = 65363
    
    except socket.timeout:
        pass

    return value


def release_cap_videos():
    global cap
    global video_detection
    global video_result_without_detection
    global video_result
    global video_mask

    if cap:
        cap.release()
        cap = None

    if video_detection:
        video_detection.release()
        video_detection = None

    if video_result_without_detection:
        video_result_without_detection.release()
        video_result_without_detection = None

    if video_result:
        video_result.release()
        video_result = None

    if video_mask:
        video_mask.release()
        video_mask = None


def before_exit():
    global sock
    global sock2

    release_cap_videos()
    cv.destroyAllWindows()

    if sock:
        sock.close()
        sock = None

    if sock2:
        sock2.close()
        sock2 = None


def main(args):

    global has_to_break

    global cap
    global video_detection
    global video_result_without_detection
    global video_result
    global video_mask
    global sock
    global sock2

    has_to_break = False

    cap = None
    video_detection = None
    video_result_without_detection = None
    video_result = None
    video_mask = None
    sock = None
    sock2 = None
    is_ssh = is_ssh_connected(False)

    k = 0
    RTH_PATH = "./RTH"
    selected_option = Option.none
    duration = 0.0
    duration_average = 0.0
    duration_average_count = 0

    udp_address = None
    udp_address2 = None

    fps = 0
    frame_to_take = 0
    frame_count = 0
    frame_wait_delay = 0

    images = []

    previous_frame = None

    gray_image = None
    gaussian_image = None
    previous_gaussian_image = None

    diff_image = None

    pause = False
    read_next_frame = True
    something_detected = False
    clear_detection = False
    config_changed = False

    # to display red rectangle
    contours=[]
    max_area = -1.0
    max_area_id = -1
    current_area = 0.0

    threshold_image = None
    mask = None
    result = None

    window_prev_frame = "Previous frame"
    window_curr_frame = "Current frame"
    window_mask = "Mask"
    window_result = "Result"

    config = cnf.Config()
    config.read()

    if args.debug:
        config.set_debug(True)

    if args.display_duration:
        config.set_display_duration(True)

    if args.display:
        config.set_display(True)

    if args.display_opt:
        config.set_display_optional_windows(True)

    if args.save_detection:
        config.set_save_detection(True)

    if args.save_result_without_detection:
        config.set_save_result_without_detection(True)

    if args.save_result:
        config.set_save_result(True)

    if args.save_mask:
        config.set_save_mask(True)

    if args.udp:
        config.set_udp_enabled(True)

    if args.ip:
        config.set_udp_enabled(True)
        config.set_udp_ip(args.ip)

    if args.port:
        config.set_udp_enabled(True)
        config.set_udp_port(args.port)

    if args.port2:
        config.set_udp_enabled(True)
        config.set_udp_port2(args.port2)

    if args.auto_change_ip:
        config.set_udp_auto_change_ip(True)

    debug = config.get_debug()
    display = config.get_display()
    display_windows = config.get_display_optional_windows()
    display_duration = config.get_display_duration()

    if debug:
        display_windows = True
        display_duration = True

    if display_windows:
        display = True

    if is_ssh:
        display = False
        display_windows = False
        print("Script launched via SSH. display and display optional windows automatically disabled")

    if config.get_udp_enabled():
        if not cnf.is_valid_ip(config.get_udp_ip()):
            sys_exit(f"The address {config.get_udp_ip()} is invalid! Please give an address like X.X.X.X where X is in [0-255]")

        if not cnf.is_valid_port(config.get_udp_port()):
            sys_exit(f"The port {config.get_udp_port()} is invalid! Please give a port from 1024 to 65535")

        if not cnf.is_valid_port(config.get_udp_port2()):
            sys_exit(f"The port {config.get_udp_port2()} is invalid! Please give a port from 1024 to 65535")

        if config.get_udp_port() == config.get_udp_port2():
            sys_exit(f"The listened port ({config.get_udp_port()}) and the writed port ({config.get_udp_port2()}) must be different")

        udp_address  = (config.get_udp_ip(), config.get_udp_port())
        udp_address2 = (config.get_udp_ip(), config.get_udp_port2())
        print(f"Opening a socket to {udp_address[0]}:{udp_address[1]}")
        sock  = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        print(f"Opening a socket to {udp_address2[0]}:{udp_address2[1]}")
        sock2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock2.settimeout(0.005) # 5 ms

        try:
            sock2.bind(udp_address2)

        except socket.error as e:
            if e.errno == 99 and config.get_udp_auto_change_ip():
                print(f"Socket error: {e}")
                udp_address2 = ("0.0.0.0", config.get_udp_port2())
                print(f"Opening a socket to {udp_address2[0]}:{udp_address2[1]}")
                udp_address2  = ("0.0.0.0", config.get_udp_port2())
                sock2.bind(udp_address2)
            else:
                raise e

    if args.image:
        images = [None, None]
        for i in range(2):
            images[i] = cv.imread(args.image[i])
            if images[i] is None:
                sys_exit(f"Could not read the image {args.image[i]}")

    elif args.video:
        print(f"Trying to open: {args.video}")
        cap = cv.VideoCapture(args.video)
        if not cap.isOpened():
            cap.release()
            sys_exit("Can not read the video")

    else:
        print("Trying to open /dev/video0 with CAP_V4L2")
        cap = cv.VideoCapture(0, cv.CAP_V4L2)  # Add cv::CAP_V4L2 to fix: Embedded video playback halted; module v4l2src0 reported: Failed to allocate required memory.
        if not cap.isOpened():
            msg = "Can not read the device"
            if is_ssh:
                msg += "\nWith a SSH connection, please use sudo"
            sys_exit(msg)
    
    if cap is not None:
        frame_width = int(cap.get(cv.CAP_PROP_FRAME_WIDTH))
        frame_height = int(cap.get(cv.CAP_PROP_FRAME_HEIGHT))
        fps = cap.get(cv.CAP_PROP_FPS)
        # if fps = 60, so frame_to_take = 15, so we take a frame every 15 => (we take a frame every 0.25 sec)
        frame_to_take = int(fps / 4)
        # not initialized to 1 to take a frame directly
        frame_count = frame_to_take
        frame_wait_delay = int(1000 / fps)  # if fps = 30, delay = 33 ms

        print(f"fps: {fps}")
        print(f"frame took every: {frame_to_take}")
        print(f"frame wait delay: {frame_wait_delay} ms")

        date = datetime.now().strftime("%Y_%m_%d_%H_%M_%S")
        output_videos_folder = "output_videos"
        fourcc = cv.VideoWriter_fourcc(*'XVID')
        
        if config.get_save_detection():
            create_folder_if_not_exists(output_videos_folder)
            video_detection = cv.VideoWriter(f"{output_videos_folder}/{date}_output_detection.avi", fourcc, frame_to_take, (frame_width, frame_height))

        if config.get_save_result_without_detection():
            create_folder_if_not_exists(output_videos_folder)
            video_result_without_detection = cv.VideoWriter(f"{output_videos_folder}/{date}_output_result_without_detection.avi", fourcc, frame_to_take, (frame_width, frame_height))

        if config.get_save_result():
            create_folder_if_not_exists(output_videos_folder)
            video_result = cv.VideoWriter(f"{output_videos_folder}/{date}_output_result.avi", fourcc, frame_to_take, (frame_width, frame_height))

        if config.get_save_mask():
            create_folder_if_not_exists(output_videos_folder)
            video_mask = cv.VideoWriter(f"{output_videos_folder}/{date}_output_mask.avi", fourcc, frame_to_take, (frame_width, frame_height))

    if os.path.exists(RTH_PATH):
        os.remove(RTH_PATH)

    config_copy = config.copy()

    display_full_config(config, debug, display, display_windows, display_duration, udp_address, udp_address2)

    print("press [H] to print the help")

    if display:
        if display_windows:
            cv.namedWindow(window_result)
            cv.namedWindow(window_prev_frame)
            cv.namedWindow(window_curr_frame)
            cv.namedWindow(window_mask)

        else:
            cv.namedWindow(window_result)
        cv.waitKey(500)

    while True:

        if has_to_break:
            break

        k = 0

        if pause and not config_changed:
            k = waitKey(500)

        else:
            duration = time()

            if not pause:
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
            else:
                frame_count = frame_to_take

            if ret and frame is not None:

                if frame_count >= frame_to_take:
                    frame_count = 1

                    # if we have a previous frame
                    if previous_frame is not None:

                        # check dimensions and depth
                        if frame.shape == previous_frame.shape and frame.dtype == previous_frame.dtype:

                            if config.get_gaussian_blur() == 0:
                                # convert a gray frame direclty into the gaussian frame
                                gaussian_image = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
                            else:
                                # convert frame into gray and then apply a gauss filter
                                gray_image = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
                                gaussian_image = cv.GaussianBlur(gray_image, config.get_gaussian_kernel(), 0)

                            diff_image = cv.absdiff(previous_gaussian_image, gaussian_image)

                            _, threshold_image = cv.threshold(diff_image, config.get_threshold(), 255, cv.THRESH_BINARY)
                            mask = cv.morphologyEx(threshold_image, cv.MORPH_OPEN, config.get_kernel())
                            
                            # Trouver tous les contours dans l'image
                            contours, _ = cv.findContours(mask, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)

                            # Trouver le plus grand contour dans l'image
                            max_area = -1
                            max_area_id = -1
                            for i in range(len(contours)):
                                current_area = cv.contourArea(contours[i])
                                #_, _, w, h = cv.boundingRect(contours[max_area_id])
                                #area = w * h

                                if debug:
                                    print(f"area {i}: {current_area}")

                                if current_area > max_area:
                                    max_area = current_area
                                    max_area_id = i

                            result = frame.copy()
                            something_detected = False

                            # Dessiner un rectangle autour du plus grand contour
                            if max_area >= config.get_detection_area():
                                x, y, w, h = cv.boundingRect(contours[max_area_id])
                                cv.rectangle(result, (x, y), (x+w, y+h), config.get_rectangle_color(), 2)
                                something_detected = True
                                if config.get_save_detection():
                                    video_detection.write(result)

                            if display:
                                if display_windows:
                                    cv.imshow(window_prev_frame, previous_frame)
                                    cv.imshow(window_curr_frame, frame)
                                    cv.imshow(window_mask, mask)
                                    cv.imshow(window_result, result)
                                    
                                else:
                                    cv.imshow(window_result, result)

                            if debug:
                                if display:
                                    k = waitKey(0)                                            
                                else:
                                    if config.get_udp_enabled():
                                        while k == 0:
                                            k = listen_key_udp(sock2)

                            if config.get_udp_enabled():
                                send_frame_udp_split(result, udp_address, sock)

                            if config.get_save_result_without_detection():
                                video_result_without_detection.write(frame)

                            if config.get_save_result():
                                video_result.write(result)

                            if config.get_save_mask():
                                video_mask.write(cv.cvtColor(mask, cv.COLOR_GRAY2BGR))

                            if something_detected:
                                # print("object detected!")
                                if not os.path.exists(RTH_PATH):
                                    # create RTH file and close it automatically
                                    with open(RTH_PATH, 'x') as f:
                                        pass
                            else:
                                if not config_changed:
                                    previous_frame = frame.copy()
                                    previous_gaussian_image = gaussian_image.copy()

                            if clear_detection:
                                clear_detection = False
                                print("detection cleared")
                                previous_frame = frame.copy()
                                previous_gaussian_image = gaussian_image.copy()

                            duration = getTimeDiff(duration)
                            duration_average = ((duration_average * duration_average_count) + duration) / (duration_average_count + 1)
                            duration_average_count += 1

                            if display_duration:
                                print(f"{round3(duration)} ms")

                        else:
                            print("frames haven't the same dimensions or depth!")
                            # replace last frame with the current
                            previous_frame = frame.copy()
                            previous_gaussian_image = gaussian_image.copy()
                        
                        if images:
                            k = waitKey(0)

                    else:
                        # replace last frame with the current
                        previous_frame = frame.copy()
                        previous_gaussian_image = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)

                    if k == 0:
                        k = waitKey(1)

                else:
                    frame_count += 1
                    k = waitKey(frame_wait_delay)

            else:
                if args.video:
                    break
                print("frame is empty")
                k = waitKey(500)

        if config.get_udp_enabled():
            k1 = listen_key_udp(sock2)
            if k1 != 0:
                k = k1

        config_changed = False

        if has_to_break or k == 27:     # ESC
            break

        elif k == 32:                   # SPACE BAR
            pause = not pause

        elif k == 67 or k == 99:        # C | c
            clear_detection = True

        elif k == 68 or k == 100:       # D | d
            config.inverse_display_duration()
            read_next_frame = not (debug or images)
        
        elif k == 72 or k == 104:       # H | h
            help()
            read_next_frame = not (debug or images)

        elif k == 73 or k == 105:       # I | i
            config.display()
            print(f"selected option: {selected_option.name}")
            read_next_frame = not (debug or images)

        elif k == 82 or k == 114:       # R | r
            print("config reset\n")
            config.set_from(config_copy)
            config.display()
            config_changed = True
            read_next_frame = not (debug or images)

        elif k == 78 or k == 110:       # N | n
            print("new config")
            config.reset()
            display_full_config(config, debug, display, display_windows, display_duration, udp_address, udp_address2)
            config_changed = True
            read_next_frame = not (debug or images)

        elif k == 83 or k == 115:       # S | s
            config.save()

        elif k == 71 or k == 103:       # G | g
            selected_option = Option.gaussian_blur
            print(f"selected option: {selected_option.name}")
            config_changed = True
            read_next_frame = not (debug or images)

        elif k == 84 or k == 116:       # T | t
            selected_option = Option.threshold
            print(f"selected option: {selected_option.name}")
            config_changed = True
            read_next_frame = not (debug or images)

        elif k == 75 or k == 107:       # K | k
            selected_option = Option.kernel_size
            print(f"selected option: {selected_option.name}")
            config_changed = True
            read_next_frame = not (debug or images)

        elif k == 65 or k == 97:        # A | a
            selected_option = Option.detection_area
            print(f"selected option: {selected_option.name}")
            config_changed = True
            read_next_frame = not (debug or images)

        elif k == 65361:  # Left Arrow
            read_next_frame = False
            config_changed = True

            if selected_option == Option.none:
                print("No option selected. Press [H] to print the help")

            elif selected_option == Option.gaussian_blur:
                if config.decrease_gaussian_blur():
                    config.display()

            elif selected_option == Option.threshold:
                if config.decrease_threshold():
                    config.display()

            elif selected_option == Option.kernel_size:
                if config.decrease_kernel_size():
                    config.display()

            elif selected_option == Option.detection_area:
                if config.decrease_detection_area():
                    config.display()

        elif k == 65363:  # Right Arrow
            read_next_frame = False
            config_changed = True

            if selected_option == Option.none:
                print("No option selected. Press H to see help")

            elif selected_option == Option.gaussian_blur:
                if config.increase_gaussian_blur():
                    config.display()

            elif selected_option == Option.threshold:
                if config.increase_threshold():
                    config.display()

            elif selected_option == Option.kernel_size:
                if config.increase_kernel_size():
                    config.display()

            elif selected_option == Option.detection_area:
                if config.increase_detection_area():
                    config.display()

        if images and read_next_frame:
            break

    print(f"Average duration: {round3(duration_average)} ms ({duration_average_count} samples)")

    before_exit()
    
    if os.path.exists(RTH_PATH):
        os.remove(RTH_PATH)


if __name__ == '__main__':
    signal.signal(signal.SIGINT, sigint_handler)
    parser = argparse.ArgumentParser(description="RealTime Camera main.py CLI")
    parser.add_argument("-i", "--image", type=str, nargs=2, help="process with two given images")
    parser.add_argument("-v", "--video", type=str, help="process with a given video")
    parser.add_argument("-d", "--display", action="store_true", help="display a window with the resulting frame")
    parser.add_argument("-do", "--display_opt", action="store_true", help="display all optionnal windows (current frame, previous frame, mask, result)")
    parser.add_argument("-dd", "--display_duration", action="store_true", help="display duration will be automatically enabled")
    parser.add_argument("-db", "--debug", action="store_true", help="shortcut to -d and -dd")
    parser.add_argument("-sd", "--save_detection", action="store_true", help="save detected objects into a video file")
    parser.add_argument("-srwd", "--save_result_without_detection", action="store_true", help="save the resulting frames without the rectangle of detection into a video file")
    parser.add_argument("-sr", "--save_result", action="store_true", help="save the resulting frames into a video file")
    parser.add_argument("-sm", "--save_mask", action="store_true", help="save the mask frames into a video file")
    parser.add_argument("-udp", "--udp", action="store_true", help="enable the UDP stream")
    parser.add_argument("-ip", "--ip", type=str, help="destination IP address for UDP stream")
    parser.add_argument("-port", "--port", type=int, help="destination port for UDP stream")
    parser.add_argument("-port2", "--port2", type=int, help="listened port for UDP stream to listen")
    parser.add_argument('-aci', '--auto_change_ip', action='store_true', help="Auto change ip to 0.0.0.0 if socket can't bind to the given ip")
    args = parser.parse_args()

    try:
        main(args)
    except BaseException as e:
        before_exit()
        print(f"Error: {e}")
        traceback.print_exc()
        print("\n")
        input("press enter to close...")
