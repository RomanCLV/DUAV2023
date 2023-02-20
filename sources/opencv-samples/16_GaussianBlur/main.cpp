#include <opencv2/opencv.hpp>
#include <iostream>
#include <csignal>

using namespace std;
using namespace cv;

bool hasToBreak = false;

void signal_handler(int signal)
{
    hasToBreak = true;
}

double getTime()
{
    return (double)getTickCount();
}

double getTimeDiff(double time_ori)
{
    return 1000 * (getTime() - time_ori) / getTickFrequency();
}

int main(int argc, char** argv) 
{
    // Install a signal handler
    std::signal(SIGINT, signal_handler);

    if (argc != 2)
    {
        printf("Usage: ./main.out <image_path>");
        return -1;
    }

    Mat image = imread(argv[1]);

    if (!image.data)
    {
        cout << "Could not read the image " << argv[1] << endl;
        return -1;
    }

    printf("\n");
    printf("press Left Arrow to decrease the blur size value\n");
    printf("press Rigth Arrow to increase the blur size value\n\n");
    printf("press ESC to quit\n\n");

    char k;

    string window1 = "Image";
    string window2 = "Blur";

    int blur_size = 5;

    double t;
    bool hasToCompute = true;

    imshow(window1, image);

    while (1)
    {
        if (hasToCompute)
        {
            hasToCompute = false;
            cout << endl;
            cout << "blur size : " << blur_size << endl;
            
            t = getTime();
            
            Mat blurred;

            GaussianBlur(image, blurred, cv::Size(blur_size, blur_size), 0);

            cout << "duration : " << getTimeDiff(t) << " ms" << endl;

            imshow(window2, blurred);
        }

        k = (char)waitKey(0);

        if (hasToBreak ||                                                   // due to Ctrl+C
            k == 27 ||                                                      // ESC key
            cv::getWindowProperty(window1, WND_PROP_AUTOSIZE) == -1 ||
            cv::getWindowProperty(window2, WND_PROP_AUTOSIZE) == -1)        // window is closed
        {
            break;
        }
        else if (k == 81)  // Left Arrow
        {
            if (blur_size > 1)
            {
                blur_size -= 2;
                hasToCompute = true;
            }
        }
        else if (k == 83)  // Right Arrow
        {
            if (blur_size < 101)
            {
                blur_size += 2;
                hasToCompute = true;
            }
        }
    }
    destroyAllWindows();
    return 0;
}
