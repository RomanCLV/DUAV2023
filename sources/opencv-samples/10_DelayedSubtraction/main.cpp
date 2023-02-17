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
    printf("--- Function -------   --- KEY ---\n");
    printf("\n");
    printf("    Display duration       D\n");
    printf("    Grayscale              G\n");
    printf("    Help                   H\n");
    printf("    Pause                  SPACE BAR\n");
    printf("    Quit                   ESC\n");
    printf("\n");
    printf("----------------------------------\n");
}

int main(int argc, char** argv)
{   
    // Install a signal handler
    std::signal(SIGINT, signal_handler);

    if ( argc != 2 )
    {
        printf("Usage: ./main.out frame_delta\n");
        printf("Read the README.md\n");
        return -1;
    }

    std::istringstream frame_delta_str(argv[1]);
    int frame_delta;
    if (!(frame_delta_str >> frame_delta)) 
    {
        cout << "Unable to parse " << argv[1] << " into an integer." << endl;
        return -1;
    }

    if (frame_delta < 1)
    {
        printf("frame_delta must be a positiv number!\n");
        return -1;
    }

    printf("Trying to open /dev/video0 with CAP_V4L2\n");
    VideoCapture cap(0, cv::CAP_V4L2);  // Add cv::CAP_V4L2 to fix: Embedded video playback halted; module v4l2src0 reported: Failed to allocate required memory.

    if (!cap.isOpened()) 
    {
        cout << "Can not read the device" << endl;
        return 0;
    }

    int frame_delta_count = 0;
    double delay;
    std::queue<cv::Mat> saved_frames;
    char k;
    bool pause = false;
    bool grayscale = false;
    bool display_subtract = false;
    bool display_duration = false;

    int channelsImage1;
    int rowsImage1;
    int colsImage1;
    int depthImage1;
    int channelsImage2;
    int rowsImage2;
    int colsImage2;
    int depthImage2;

    string window1 = "Current Frame";
    string window2 = "Subtraction";

    printf("press [H] to print the help\n");
    namedWindow(window1);
    namedWindow(window2);
    waitKey(500);

    while (1)
    {
        if (pause)
        {
            k = (char)waitKey(1);
        }
        else
        {
            Mat frame;
            cap >> frame;

            if (!frame.empty()) 
            {
                if (grayscale)
                {
                    cvtColor(frame, frame, COLOR_BGR2GRAY);
                }
                cv::imshow(window1, frame);   // display frame

                if (display_subtract)
                {
                    delay = (double)getTickCount();

                    Mat old_frame = saved_frames.front();
                    saved_frames.pop();

                    channelsImage1 = frame.channels();
                    rowsImage1 = frame.rows;
                    colsImage1 = frame.cols;
                    depthImage1 = frame.depth();

                    channelsImage2 = old_frame.channels();
                    rowsImage2 = old_frame.rows;
                    colsImage2 = old_frame.cols;
                    depthImage2 = old_frame.depth();

                    if (channelsImage1 != channelsImage2 || rowsImage1 != rowsImage2 || colsImage1 != colsImage2 || depthImage1 != depthImage2)
                    {
                        cout << "Not the same dimensions or depth!   ";
                        cout << "current frame: (" << rowsImage1 << "," << colsImage1 << "," << channelsImage1 << ") (" << depthImage1 << ")   ";
                        cout << "previous frame: (" << rowsImage2 << "," << colsImage2 << "," << channelsImage2 << ") (" << depthImage2 << ")" << endl;
                    }
                    else
                    {
                        Mat subtracted;
                        cv::subtract(old_frame, frame, subtracted);
                        cv::imshow(window2, subtracted);  // display saved frame

                        if (display_duration)
                        {
                            delay = 1000 * ((double)getTickCount() - delay) / getTickFrequency();
                            cout << "Subtraction duration: " << (int)(delay * 1000) / 1000.0 << " ms" << endl;
                        }
                    }
                }
                else
                {
                    frame_delta_count++;
                    display_subtract = frame_delta_count == frame_delta;
                }
                saved_frames.push(frame);     // save frame
                k = (char)waitKey(1);
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
        else if (k == 71 || k == 103)   // G | g
        {
            grayscale = !grayscale;
        }
        else if (k == 68 || k == 100)   // D | d
        {
            display_duration = !display_duration;
        }
        else if (k == 32)               // SPACE BAR
        {
            pause = !pause;
        }
    }
    printf("Capture done\n");
    cap.release();
    cv::destroyAllWindows();
    return 0;
}
