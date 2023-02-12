// https://www.selfmadetechie.com/how-to-create-a-webcam-video-capture-using-opencv-c

#include <stdio.h>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main(int argc, char** argv)
{   
    if ( argc != 2 )
    {
        printf("Usage: ./main.out <video_path>\n");
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

    Mat frame;
    string window_name = "Display video window";
    namedWindow(window_name);
    
    while (1)
    {
        // get frame from the video
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