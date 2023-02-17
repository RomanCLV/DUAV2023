#include <stdio.h>
#include <sys/stat.h>
#include <csignal>
#include <ctime>
#include <iostream>
#include <sstream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

bool hasToBreak = false;

void signal_handler(int signal)
{
    hasToBreak = true;
}

void help()
{
    printf("\n");
    printf("--- Function ---   --- KEY ---\n");
    printf("\n");
    printf("    Help               H\n");
    printf("    Pause              SPACE BAR\n");
    printf("    Screenshot         S\n");
    printf("    Quit               ESC\n");
    printf("\n");
    printf("------------------------------\n");
}

int main(int argc, char** argv)
{   
    // Install a signal handler
    std::signal(SIGINT, signal_handler);

    if ( argc != 3 )
    {
        printf("Usage: ./main.out fps frame_delta_delay\n");
        printf("Read the README.md\n");
        return -1;
    }

    std::istringstream fps_str(argv[1]);
    std::istringstream frame_delta_delay_str(argv[2]);
    int fps;
    int frame_delta_delay;
    if (!(fps_str >> fps)) 
    {
        cout << "Unable to parse " << argv[1] << " into an integer." << endl;
        return -1;
    }
    if (!(frame_delta_delay_str >> frame_delta_delay)) 
    {
        cout << "Unable to parse " << argv[2] << " into an integer." << endl;
        return -1;
    }

    if (fps <= 0)
    {
        printf("fps must be a positiv number!\n");
        return -1;
    }
    if (frame_delta_delay <= 0)
    {
        printf("frame_delta_delay must be a positiv number!\n");
        return -1;
    }


    printf("Trying to open /dev/video0 with CAP_V4L2\n");
    VideoCapture cap(0, cv::CAP_V4L2);  // Add cv::CAP_V4L2 to fix: Embedded video playback halted; module v4l2src0 reported: Failed to allocate required memory.

    if (!cap.isOpened()) 
    {
        cout << "Can not read the device" << endl;
        return 0;
    }

    uint frame_delay = (uint)(1000 / fps);
    uint frame_delta = (uint)(fps * frame_delta_delay);
    uint frame_delta_count = 0;
    uint frame_count_total = 0;
    bool display_saved = false;
    std::queue<cv::Mat> saved_frames;
    uint screenshot_count = 0;

    std::time_t rawtime;
    std::tm* timeinfo;
    char buffer [80];

    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);

    std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);

    string screenshot_name = buffer;
    bool pause = false;
    char k;
    Mat last_frame;

    string window1 = "Current Frame";
    string window2 = "Previous Frame";

    printf("press [H] to print the help\n");
    namedWindow(window1);
    namedWindow(window2);
    waitKey(500);

    while (1)
    {
        if (pause)
        {
            k = (char)waitKey(frame_delay);
        }
        else
        {
            Mat frame;
            cap >> frame;

            if (!frame.empty()) 
            {
                frame_count_total++;
                cv::putText(frame, "frame: " + std::to_string(frame_count_total), cv::Point(30, 30), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 255, 255), 2);
                cv::imshow(window1, frame);   // display frame
                if (display_saved)
                {
                    cv::imshow(window2, saved_frames.front());  // display saved frame
                    saved_frames.pop();
                }
                else
                {
                    frame_delta_count++;
                    display_saved = frame_delta_count == frame_delta;
                }
                saved_frames.push(frame);     // save frame
                last_frame = frame;
                k = (char)waitKey(frame_delay);
            }
            else
            {
                printf("frame is empty\n");
                k = (char)waitKey(500);
            } 
        }
        
        // break the loop if
        if (hasToBreak ||                                               // due to Ctrl+C
            k == 27 ||                                                  // ESC key
            cv::getWindowProperty(window1, WND_PROP_AUTOSIZE) == -1 ||  // window1 is closed
            cv::getWindowProperty(window2, WND_PROP_AUTOSIZE) == -1)    // window2 is closed
        {
            break;
        }
        if (k == 72 || k == 104)        // H | h
        {
            help();
        }
        else if (k == 32)               // SPACE BAR
        {
            if (saved_frames.size() > 0)  // allows pause only if we got a frame
            {
                pause = !pause;
            }
        }
        else if (k == 83 || k == 115)   // S | s
        {
            if (saved_frames.size() > 0)
            {
                screenshot_count++;
                string path = "./screenshot/" + screenshot_name + "_" + std::to_string(screenshot_count) + ".jpg";
                mkdir("./screenshot", 0777);
                imwrite(path, last_frame);
                cout << path << endl;
            }
        }
    }
    printf("Capture done\n");
    cap.release();
    cv::destroyAllWindows();
    return 0;
}
