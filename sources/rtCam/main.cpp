// https://www.selfmadetechie.com/how-to-create-a-webcam-video-capture-using-opencv-c

#include <stdio.h>
#include <csignal>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <opencv2/opencv.hpp>
#include <string>
using namespace cv;
using namespace std;

bool hasToBreak = false;

void signalHandler(int signal)
{
    hasToBreak = true;
}

inline void help()
{
    printf("\n");
    printf("--- Function -------   --- KEY -----\n");
    printf("\n");
    printf("    Clear detection        C\n");
    printf("    Decrease threshold     Left  Arrow\n");
    printf("    Decrease kernel size   Down  Arrow\n");
    printf("    Display duration       D\n");
    printf("    Increase threshold     Right Arrow\n");
    printf("    Increase kernel size   Up    Arrow\n");
    printf("    Help                   H\n");
    printf("    Pause                  SPACE BAR\n");
    printf("    Quit                   ESC\n");
    printf("    Reset config           R\n");
    printf("\n");
    printf("----------------------------------\n");
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

inline bool fileExists(const string& name)
{
    struct stat buffer;   
    return (stat (name.c_str(), &buffer) == 0); 
}

inline double getTime()
{
    return (double)getTickCount();
}

inline double getTimeDiff(const double time_ori)
{
    return std::round((getTime() - time_ori) / getTickFrequency() * 1000000.0) / 1000.0;
}

bool arrayContains(int const arraySize, const unsigned int array[], const unsigned int val)
{
    for (int i = 0; i < arraySize; i++)
    {
        if (array[i] == val)
        {
            cout << val << " is in array" << endl;
            return true;
        }
    }
    return false;
}

int main(int argc, char** argv)
{   
    // Install a signal handler
    std::signal(SIGINT, signalHandler);

    std::map<std::string, std::vector<std::string>> args;
    parseArgs(argc, argv, args);

    VideoCapture* cap(nullptr);

    char k = 0;
    const char ARROW_KEYS_COUNT = 4;
    const unsigned int arrow_keys[ARROW_KEYS_COUNT] = { 81, 82, 83, 84 };
    double duration = 0.0;

    double fps = 0.0;
    unsigned int frame_to_take = 0;
    unsigned int frame_count = 0;
    unsigned int frame_wait_delay = 0;

    Mat images[2];
    Mat frame;
    Mat previous_frame;

    int channelsImage1;
    int rowsImage1;
    int colsImage1;
    int depthImage1;
    int channelsImage2;
    int rowsImage2;
    int colsImage2;
    int depthImage2;

    Mat gray_image1;
    Mat gray_image2;
    Mat diff_image;

    bool debug = false;
    bool display = false;
    bool pause = false;
    bool read_next_frame = true;
    bool display_duration = false;
    bool detection_enabled = true;
    bool clear_detection = false;

    bool image_mode = false;
    bool video_mode = false;

    // to display red rectangle
    vector<vector<Point>> contours;
    Scalar red(0, 0, 255);
    unsigned int detect_min_area = 300;

    // Pour la binarisation de l'image
    Mat threshold_image;
    unsigned int threshold_value = 70;   // seuil de binarisation

    // Pour l'ouverture morphologique
    unsigned int kernel_size = 6;
    Mat kernel = getStructuringElement(MORPH_RECT, Size(kernel_size, kernel_size));
    Mat mask;

    Mat result;

    string window_prev_frame = "Previous frame";
    string window_curr_frame = "Current frame";
    string window_mask = "Mask";
    string window_result = "Result";

    if (args.count("db"))
    {
        debug = true;
    }
    if (debug || args.count("dd"))
    {
        display_duration = true;
    }
    if (debug || args.count("d"))
    {
        display = true;
    }
    if (args.count("i"))
    {
        if (args["i"].size() != 2)
        {
            printf("Wrong usage of image option: -i img1 img2\n");
            return -1;
        }
        for (int i = 0; i < 2; i++)
        {
            Mat image = imread(args["i"][i]);
            if ( !image.data )
            {
                cout << "Could not read the image:" << args["i"][i] << endl;
                return 0;
            }
            images[i] = image;
        }
        image_mode = true;
    }
    else if (args.count("v"))
    {
        if (args["v"].size() != 1)
        {
            printf("Wrong usage of video option: -v vid\n");
            return -1;
        }
        cap = new VideoCapture(args["v"][0]);
        if (!cap->isOpened())
        {
            cout << "Can't open video " << args["v"][0] << endl;
            return 0;
        }
        video_mode = true;
    }
    else
    {
        printf("Trying to open /dev/video0 with CAP_V4L2\n");
        cap = new VideoCapture(0, cv::CAP_V4L2);

        if (!cap->isOpened()) 
        {
            cout << "Can not read the device" << endl;
            return 0;
        }
    }

    if (cap != nullptr)
    {
        fps = cap->get(cv::CAP_PROP_FPS);
        frame_to_take = (int)(fps / 4);
        frame_count = frame_to_take;
        frame_wait_delay = (int)(1000.0 / fps);
        cout << "fps: " << fps << endl;
        cout << "frame took every: " << frame_to_take << endl;
        cout << "frame wait delay: " << frame_wait_delay << " ms" << endl;
    }

    string RTH_PATH = "../../automate/RTH";
    if (debug || image_mode || video_mode)
    {
        RTH_PATH = "../RTH";
    }

    if (fileExists(RTH_PATH))
    {
        int result = remove(RTH_PATH.c_str());
        if (result != 0)
        {
            cout << "Cannot remove RTH file." << endl;
            return -1;            
        }
    }

    printf("\npress [H] to print the help\n");

    cout << endl << "threshold: " << threshold_value << "\tkernel: " << kernel_size << endl << endl;

    if (display)
    {
        namedWindow(window_prev_frame);
        namedWindow(window_curr_frame);
        namedWindow(window_mask);
    }
    namedWindow(window_result);
    waitKey(500);

    while (true)
    {
        if (hasToBreak)
        {
            break;
        }
        k = 0;

        if (pause)
        {
            k = (char)waitKey(500);
        }
        else
        {
            duration = getTime();
            if (image_mode)
            {
                previous_frame = images[0];
                frame = images[1];
            }
            else
            {
                if (read_next_frame)
                {
                    *cap >> frame;
                }
                else
                {
                    read_next_frame = true;
                    frame_count = frame_to_take;
                }
            }

            if (!frame.empty()) 
            {
                if (frame_count >= frame_to_take)
                {
                    frame_count = 1;

                    if (!previous_frame.empty())
                    {
                        if (frame.channels() == previous_frame.channels() && 
                            frame.rows == previous_frame.rows && 
                            frame.cols == previous_frame.cols && 
                            frame.depth() == previous_frame.depth())
                        {
                            cvtColor(previous_frame, gray_image1, COLOR_BGR2GRAY);
                            cvtColor(frame, gray_image2, COLOR_BGR2GRAY);
                            //GaussianBlur(gray_image1, gray_image1, Size(5, 5), 0);
                            //GaussianBlur(gray_image2, gray_image2, Size(5, 5), 0);

                            absdiff(gray_image1, gray_image2, diff_image);
                                
                            // way 1: threshold, morpho
                            threshold(diff_image, threshold_image, threshold_value, 255, THRESH_BINARY);
                            morphologyEx(threshold_image, mask, MORPH_OPEN, kernel);

                            // way 2: Canny filter
                            //Canny(diff_image, mask, 100, 200);

                            // Trouver tous les contours dans l'image
                            contours.clear();
                            findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
                            
                            // Trouver le plus grand contour dans l'image
                            double maxArea = -1;
                            int maxAreaIdx = -1;
                            for (int i = 0; i < contours.size(); i++)
                            {
                                double area = contourArea(contours[i]);
                                if (area > maxArea)
                                {
                                    maxArea = area;
                                    maxAreaIdx = i;
                                }
                            }

                            frame.copyTo(result);
                            bool something_detected = false;

                            // Dessiner un rectangle autour du plus grand contour
                            if (maxArea >= detect_min_area)
                            {
                                Rect rect = boundingRect(contours[maxAreaIdx]);
                                rectangle(result, rect, red, 2);
                                something_detected = true;
                            }

                            if (display)
                            {
                                imshow(window_prev_frame, previous_frame);
                                imshow(window_curr_frame, frame);
                                imshow(window_mask, mask);
                                imshow(window_result, result);
                                if (debug)
                                {
                                    k = (char)waitKey(0);
                                }
                            }
                            else
                            {
                                imshow(window_result, result);
                            }

                            if (something_detected)
                            {
                                // TODO: detection_enabled in rtCam or in automate.py ??
                                if (detection_enabled)
                                {
                                    // printf("Object detected!\n");
                                    if (!fileExists(RTH_PATH))
                                    {
                                        ofstream file(RTH_PATH.c_str());
                                        file.close();
                                    }
                                }
                            }
                            else
                            {
                                if (!arrayContains(ARROW_KEYS_COUNT, arrow_keys, k))
                                {
                                    frame.copyTo(previous_frame);
                                }
                            }

                            if (clear_detection)
                            {
                                clear_detection = false;
                                printf("detection cleared\n");
                                frame.copyTo(previous_frame);
                            }

                            if (display_duration)
                            {
                                cout << getTimeDiff(duration) << " ms" << endl;
                            }
                        }
                        else
                        {
                            printf("frames haven't the same dimensions or depth!\n");
                            frame.copyTo(previous_frame);
                        }

                        if (image_mode)
                        {
                            k = (char)waitKey(0);
                        }
                    }
                    else
                    {
                        frame.copyTo(previous_frame);
                    }

                    if (k == 0)
                    {
                        k = (char)waitKey(1);
                    }
                }
                else
                {
                    frame_count++;
                    k = (char)waitKey(frame_wait_delay);
                }
            }
            else
            {
                if (video_mode)
                {
                    break;
                }
                printf("frame is empty\n");
                k = (char)waitKey(500);
            }
        }

        if (hasToBreak ||                                                  // due to Ctrl+C
            k == 27 ||                                                      // ESC key
            cv::getWindowProperty(window_result, WND_PROP_AUTOSIZE) == -1)  // window is closed
        {
            break;
        }
        else if (k == 72 || k == 104)   // H | h
        {
            help();
        }
        else if (k == 68 || k == 100)   // D | d
        {
            display_duration = !display_duration;
            cout << "display duration: " << display_duration << endl;
        }
        else if (k == 67 || k == 99)   // C | c
        {
            clear_detection = true;
        }
        else if (k == 82 || k == 114)   // R | r
        {
            threshold_value = 70;
            kernel_size = 6;
            kernel = getStructuringElement(MORPH_RECT, Size(kernel_size, kernel_size));
            cout << "display duration: " << display_duration << endl;
        }
        else if (k == 32)
        {
            pause = !pause;
        }
        else if (k == 81)            // Left Arrow
        {
            if (threshold_value > 0)
            {
                threshold_value--;
                cout << endl << "threshold: " << threshold_value << "\tkernel: " << kernel_size << endl << endl;
                read_next_frame = false;
            }
        }
        else if (k == 83)       // Right Arrow
        {
            if (threshold_value < 255)
            {
                threshold_value++;
                cout << endl << "threshold: " << threshold_value << "\tkernel: " << kernel_size << endl << endl;
                read_next_frame = false;
            }
        }
        else if (k == 84)       // Down Arrow
        {
            if (kernel_size > 1)
            {
                kernel_size--;
                kernel = getStructuringElement(MORPH_RECT, Size(kernel_size, kernel_size));
                cout << endl << "threshold: " << threshold_value << "\tkernel: " << kernel_size << endl << endl;
                read_next_frame = false;
            }
        }
        else if (k == 82)       // Up Arrow
        {
            if (kernel_size < 255)
            {
                kernel_size++;
                kernel = getStructuringElement(MORPH_RECT, Size(kernel_size, kernel_size));
                cout << endl << "threshold: " << threshold_value << "\tkernel: " << kernel_size << endl << endl;
                read_next_frame = false;
            }
        }
        if (image_mode && read_next_frame)
        {
            break;
        }
    }


    if (cap != nullptr)
    {
        cap->release();
        delete cap;
        cap = nullptr;
    }
    cv::destroyAllWindows();
    return 0;
}
