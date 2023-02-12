// https://www.selfmadetechie.com/how-to-create-a-webcam-video-capture-using-opencv-c

#include <stdio.h>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main(int argc, char** argv)
{   
    if ( argc != 1 )
    {
        printf("Usage: ./main.out\n");
        printf("Read the README.md\n");
        return -1;
    }

    printf("Trying to open /dev/video0 with CAP_V4L2");
    VideoCapture cap(0, cv::CAP_V4L2);
    if (!cap.isOpened()) 
    {
        cout << "cannot read the device";
    }

    Mat frame;
    string window_name = "Display window";
    namedWindow(window_name);

    while (1)
    {
        cap >> frame;
        imshow(window_name, frame);

        // stop the program if
        if ((frame.rows == 0 || frame.cols == 0) ||                         // no more images
            waitKey(1) == 27 ||                                             // ESC key
            cv::getWindowProperty(window_name, WND_PROP_AUTOSIZE) == -1)    // window is closed
        {
            break;
        }
    }
    return 0;
}