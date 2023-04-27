#include <iostream>
#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <csignal>
#include <unistd.h>
#include <algorithm>
#include <string>
#include <map>
#include <vector>
#include <regex>


using namespace std;
using namespace boost::asio;
using namespace cv;

volatile sig_atomic_t stop;

void signal_handler(int signum)
{
    stop = 1;
}

bool is_valid_ip(const std::string& ip)
{
    regex ip_pattern(
        R"((?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?))");

    return regex_match(ip, ip_pattern);
}

bool is_valid_port(int port)
{
    return 1024 <= port && port <= 65535;
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

void send_frame_udp_split(cv::Mat frame, ip::udp::socket& sock, const ip::udp::endpoint endpoint, int max_packet_size = 65507) 
{
    vector<uchar> data;
    imencode(".jpg", frame, data);
    int num_packets = (data.size() + max_packet_size - 1) / max_packet_size;

    for (int i = 0; i < num_packets; i++) 
    {
        vector<uchar> packet_data;
        int start = i * max_packet_size;
        int end = min((i + 1) * max_packet_size, (int)data.size());
        packet_data.assign(data.begin() + start, data.begin() + end);

        vector<uchar> header(8, 0);
        header[4] = (uchar)num_packets;
        header[5] = (uchar)(num_packets >> 8);
        header[6] = (uchar)i;
        header[7] = (uchar)(i >> 8);

        vector<uchar> packet(header.size() + packet_data.size());
        copy(header.begin(), header.end(), packet.begin());
        copy(packet_data.begin(), packet_data.end(), packet.begin() + header.size());

        boost::system::error_code error;
        sock.send_to(buffer(packet), endpoint, 0, error);

        if (error) 
        {
            std::cerr << "Error sending UDP packet: " << error.message() << std::endl;
            break;
        }
    }
}

void parseArgs(int argc, char** argv, std::map<std::string, std::vector<std::string>>& args) 
{
    std::string currentOption;
    for (int i = 1; i < argc; ++i) 
    {
        std::string arg = argv[i];
        if (arg[0] == '-') 
        {
            // New option detected
            currentOption = arg.substr(1);
            args[currentOption] = std::vector<std::string>();
        }
        else
        {
            // Argument for current option
            args[currentOption].push_back(arg);
        }
    }
}

void displayArgs(std::map<std::string, std::vector<std::string>>& args)
{
    for (const auto& arg : args) 
    {
        std::cout << arg.first << endl;
        for (const auto& value : arg.second) 
        {
            cout << "\t" << value << endl;
        }
        cout << std::endl;
    }
}

int main(int argc, char* argv[])
{
    signal(SIGINT, signal_handler);
    map<string, vector<string>> args;
    parseArgs(argc, argv, args);

    string udp_ip;
    int udp_port;
    bool write = false;
    bool display = false;
    string output;
    string window_name = "Send to " + udp_ip + ':' + to_string(udp_port);

    if (argc < 3)
    {
        cerr << "Usage: " << argv[0] << " IP PORT [-o output] [-d]" << endl;
        return 1;
    }

    udp_ip = argv[1];
    udp_port = atoi(argv[2]);

    if (!is_valid_ip(udp_ip))
    {
        cerr << "The address " << udp_ip << " is invalid! Please give an address like X.X.X.X where X is in [0-255]" << endl;
        return 1;
    }

    if (!is_valid_port(udp_port))
    {
        cerr << "The port " << udp_port << " is invalid! Please give a port from 1024 to 65535" << endl;
        return 1;
    }

    if (args.count("o"))
    {
        write = true;
        output = args["o"][0];
        if (!has_avi_extension(output))
        {
            cerr << "Error : the video file extension must be .avi" << endl;
            return 1;
        }
    }

    if (args.count("d"))
    {
        display = true;
    }

    io_service io_service;
    ip::udp::socket sock(io_service, ip::udp::endpoint(ip::udp::v4(), 0));
    ip::udp::endpoint remote_endpoint(ip::address::from_string(udp_ip), udp_port);

    cout << "Frames will be send to " << udp_ip << ":" << udp_port << endl;

    cout << "Trying to open /dev/video0 with CAP_V4L2" << endl;
    VideoCapture cap(0, cv::CAP_V4L2);

    if (!cap.isOpened())
    {
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

    int k = 0;
    cout << "press ESC to quit" << endl << endl;

    while (!stop) 
    {
        cap >> frame;

        if (frame.empty()) 
        {
            cout << "frame is empty" << endl;
            k = waitKey(500) & 0xFF;
        }
        else
        {
            if (write)
            {
                video_writer.write(frame);
            }

            send_frame_udp_split(frame, sock, remote_endpoint, 64000);

            if (display) 
            {
                imshow(window_name, frame);
            }
            k = waitKey(1) & 0xFF;
        }
        if (k == 27) 
        {
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
