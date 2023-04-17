#include <iostream>
#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <csignal>
#include <unistd.h>
#include <getopt.h>
#include <algorithm>
#include <string>

using namespace std;
using namespace boost::asio;
using namespace cv;

volatile sig_atomic_t stop;

void signal_handler(int signum)
{
    stop = 1;
}

bool has_avi_extension(const string& filename)
{
    string ext = filename.substr(filename.find_last_of(".") + 1);
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return (ext == "avi");
}

void send_frame_udp(Mat& frame, ip::udp::socket& sock, ip::udp::endpoint& endpoint)
{
    vector<uchar> buf;
    imencode(".jpg", frame, buf);
    sock.send_to(buffer(buf), endpoint);
}

void send_frame_udp_split(Mat& frame, ip::udp::socket& sock, ip::udp::endpoint& endpoint, size_t max_packet_size = 65507) 
{
    vector<uchar> buf;
    imencode(".jpg", frame, buf);
    size_t num_packets = (buf.size() / max_packet_size) + 1;

    for (size_t i = 0; i < num_packets; ++i)
    {
        size_t start = i * max_packet_size;
        size_t end = min(start + max_packet_size, buf.size());
        sock.send_to(buffer(&buf[start], end - start), endpoint);
    }
}

int main(int argc, char* argv[])
{
    signal(SIGINT, signal_handler);
    string udp_ip;
    int udp_port;
    bool write = false;
    bool display = false;
    string output;
    string window_name = "Send to " + udp_port + ':' + to_string(udp_ip);

    int opt;
    while ((opt = getopt(argc, argv, "o:d")) != -1)
    {
        switch (opt) 
        {
            case 'o':
                write = true;
                output = optarg;
                if (!has_avi_extension(output))
                {
                    cerr << "Error : the video file extension must be .avi" << endl;
                    return 1;
                }
                break;
            case 'd':
                display = true;
                break;
            default:
                break;
        }
    }

    if (optind < argc) 
    {
        udp_ip = argv[optind++];
        udp_port = atoi(argv[optind]);
    } 
    else 
    {
        cerr << "Usage: " << argv[0] << " IP PORT [-o output] [-d]" << endl;
        return 1;
    }

    io_service io_service;
    ip::udp::socket sock(io_service, ip::udp::endpoint(ip::udp::v4(), 0));
    ip::udp::endpoint remote_endpoint(ip::address::from_string(udp_ip), udp_port);

    cout << "Frames will be send to " << udp_ip << ":" << udp_port << endl;

    cout << "Trying to open /dev/video0 with CAP_V4L2" << endl;
    VideoCapture cap(0, cv::CAP_V4L2);

    if (!cap.isOpened()) {
        cerr << "Can not read the device" << endl;
        return 1;
    }

    VideoWriter video_writer;
    if (write) 
    {
        int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
        double fps = cap.get(CAP_PROP_FPS);
        Size frame_size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT));
        video_writer.open(output, codec, fps, frame_size);
    }

    Mat frame;

    if (display) 
    {
        namedWindow(window_name, WINDOW_AUTOSIZE);
    }

    while (!stop) 
    {
        cap >> frame;

        if (frame.empty()) 
        {
            cerr << "frame is empty" << endl;
            waitKey(500);
            continue;
        }

        if (write)
        {
            video_writer.write(frame);
        }

        send_frame_udp_split(frame, sock, remote_endpoint);

        if (display) 
        {
            imshow(window_name, frame);
        }

        if (waitKey(1) == 27) 
        { // 27 is the ESC key
            break;
        }
    }

    cap.release();

    if (write) 
    {
        video_writer.release();
    }

    if (display)
    {
        destroyAllWindows();
    }

    sock.close();

    return 0;
}
