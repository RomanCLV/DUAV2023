// https://www.selfmadetechie.com/how-to-create-a-webcam-video-capture-using-opencv-c
// https://learnopencv.com/read-write-and-display-a-video-using-opencv-cpp-python/

#include <stdio.h>
#include <cstdlib>
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

float formate_time(float f)
{
    return ((int)(f * 1000)) / 1000.0;
}

int main(int argc, char** argv)
{   
    // Install a signal handler
    std::signal(SIGINT, signal_handler);

    if ( argc < 3 || argc > 4 )
    {
        printf("Usage: ./main.out <output_path> <video_fps> [g|G]\n");
        printf("Read the README.md\n");
        return -1;
    }

    char * endPtr;
    float video_fps = strtof(argv[2], &endPtr);

    if ( endPtr == argv[2] )
    {
        printf("Cannot parse: %s to float\n", endPtr);
        return -1;
    } 

    const int fps_choices_length = 18;
    float fps_choices[fps_choices_length] = { 1, 5, 10, 12, 15, 20, 24, 25, 29.97, 30, 48, 50, 59.94, 60, 120, 144, 240, 300 };
    bool contains = false;
    for (int i = 0; i < fps_choices_length; i++)
    {
        contains = fps_choices[i] == video_fps;
        if (contains) 
        {
            break;
        }
    }

    if (!contains)
    {
        printf("Wrong fps given. Available values:\n");
        for (int i = 0; i < fps_choices_length; i++)
        {
            cout << fps_choices[i] << endl;
        }
        return 0;
    }

    bool convertToGray = false;
    if (argc == 4)
    {
        if (!strcmp("g", argv[3]) || !strcmp("G", argv[3]))
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

    printf("Trying to open /dev/video0 with CAP_V4L2\n");
    VideoCapture cap(0, cv::CAP_V4L2);  // Add cv::CAP_V4L2 to fix: Embedded video playback halted; module v4l2src0 reported: Failed to allocate required memory.

    if (!cap.isOpened()) 
    {
        printf("Can not read the device");
        return 0;
    }

    // Default resolutions of the frame are obtained.The default resolutions are system dependent.
    int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    float frame_fps = cap.get(cv::CAP_PROP_FPS);
    // Additional properties can be found here:
    // https://docs.opencv.org/3.4/d4/d15/group__videoio__flags__base.html
    // VideoCaptureProperties

    printf("frame width: %d\n", frame_width);
    printf("frame height: %d\n", frame_height);
    cout << "frame fps: " << frame_fps << endl;
    cout << "video fps: " << video_fps << endl;

    // Define the codec and create VideoWriter object.The output is stored in output file.
    // Define the fps to be equal to video_fps. Also frame size is passed.
    // FourCC is a 4-byte code used to specify the video codec.
    VideoWriter video(argv[1], cv::VideoWriter::fourcc('M','J','P','G'), video_fps, Size(frame_width, frame_height));

    string window_name = "Camera Capture";
    Mat frame;
    char k;
    namedWindow(window_name);
    waitKey(500);

    uint frame_count = 0;
    double video_duration;
    double start_time = (double)getTickCount();

    while (1)
    {
        cap >> frame;

        if (!frame.empty()) 
        {
            if (convertToGray) 
            {
                cvtColor(frame, frame, cv::COLOR_RGB2GRAY);     // BGR  -> Gray : the image loses its color
                cvtColor(frame, frame, cv::COLOR_GRAY2RGB);     // Gray -> BGR  : the image is still gray but the 3 channels are reconstituted
                                                                // If the 3 channels are not reconstituted, you will get "Could not demultiplex stream."
                                                                // when trying to open your vide.
            }
            frame_count++;                                                   
            // Write the frame into the file
            video.write(frame);

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
    video_duration = ((double)getTickCount() - start_time) / getTickFrequency();

    printf("Capture done\n");
    // When everything done, release the video capture and write object
    cap.release();
    video.release();

    cout << "Recorded duration: " << formate_time(video_duration) << " secs" << endl;
    cout << "Frames recorded:   " << frame_count << endl;
    cout << "Video duration:    " << formate_time(frame_count / video_fps) << " secs" << endl;

    cv::destroyAllWindows();
    return 0;
}
