// https://www.selfmadetechie.com/how-to-create-a-webcam-video-capture-using-opencv-c

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

    if ( argc < 2 || argc > 3 )
    {
        printf("Usage: ./main.out <video_path> [g|G]\n");
        printf("Read the README.md\n");
        return -1;
    }
    
    string video = cv::samples::findFile(argv[1]);
    VideoCapture cap(video);

    if (!cap.isOpened())
    {
        cout << "Can't open video " << video << endl;
        return 0;
    }

    bool convertToGray = false;
    if (argc == 3)
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

    Mat frame;
    string window_name = "Read Video";
    namedWindow(window_name);
    
    while (1)
    {
        // get frame from the video
        cap >> frame;

        if (frame.empty()) 
        {
            break;
        }

        if (convertToGray) 
        {
            // Our operations on the frame come here
            cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        }

        imshow(window_name, frame);

        // stop the program if
        if (hasToBreak ||                                                   // due to Ctrl+C
            waitKey(1) == 27 ||                                             // ESC key
            cv::getWindowProperty(window_name, WND_PROP_AUTOSIZE) == -1)    // window is closed
        {
            break;
        }
    }
    printf("Video successfully played\n");
    cap.release();
    cv::destroyAllWindows();
    return 0;
}