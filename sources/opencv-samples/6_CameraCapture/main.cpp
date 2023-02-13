// https://www.selfmadetechie.com/how-to-create-a-webcam-video-capture-using-opencv-c
// https://learnopencv.com/read-write-and-display-a-video-using-opencv-cpp-python/

#include <stdio.h>
#include <csignal>
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

bool hasToBreak = false;

void signal_handler(int signal)
{
    hasToBreak = true;
}

int main(int argc, char** argv)
{   
    // Install a signal handler
    std::signal(SIGINT, signal_handler);

    if ( argc > 2 )
    {
        printf("Usage: ./main.out [g|G]\n");
        printf("Read the README.md\n");
        return -1;
    }

    printf("Trying to open /dev/video0 with CAP_V4L2\n");
    VideoCapture cap(0, cv::CAP_V4L2);  // Add cv::CAP_V4L2 to fix: Embedded video playback halted; module v4l2src0 reported: Failed to allocate required memory.

    if (!cap.isOpened()) 
    {
        cout << "Can not read the device" << endl;
        return 0;
    }

    bool convertToGray = false;
    if (argc == 2)
    {
        if (!strcmp("g", argv[2]) || !strcmp("G", argv[2]))
        {
            convertToGray = true;
        }
        else
        {
            printf("Unexpected parameter given\n");
            printf("Read the README.md\n");
            return -1;
        }
    }

    string window_name = "Camera Capture";
    Mat frame;
    char k;
    namedWindow(window_name);
    waitKey(500);

    while (1)
    {
        cap >> frame;
        // cout << "frame type: " << frame.type() << "\tframe empty: " << frame.empty() << "\tvisible: " << cv::getWindowProperty(window_name, WND_PROP_AUTOSIZE) << endl;

        if (!frame.empty()) 
        {
            if (convertToGray) 
            {
                cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
            }
            imshow(window_name, frame);
            k = (char)waitKey(1);
        }
        else
        {
            printf("frame is empty\n");
            k = (char)waitKey(500);
        }    
        
        // break the loop if
        if (hasToBreak ||                                                   // due to Ctrl+C
            k == 27 ||                                                      // ESC key
            cv::getWindowProperty(window_name, WND_PROP_AUTOSIZE) == -1)    // window is closed
        {
            break;
        }
    }

    printf("Capture done\n");
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
