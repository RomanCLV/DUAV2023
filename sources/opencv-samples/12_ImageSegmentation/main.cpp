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
    printf("press Left Arrow to decrease the threshold value\n");
    printf("press Rigth Arrow to increase the threshold value\n\n");
    printf("press ESC to quit\n\n");

    char k;

    string window1 = "Image";
    string window2 = "Result";

    Mat gray;

    // Pour la binarisation de l'image
    int threshold_value = 100;   // seuil de binarisation

    double t;
    bool hasToCompute = true;

    cvtColor(image, gray, cv::COLOR_BGR2GRAY);

    imshow(window1, image);

    while (1)
    {
        if (hasToCompute)
        {
            hasToCompute = false;
            cout << endl;
            cout << "threshold : " << threshold_value << endl;
            
            t = getTime();
            
            // Appliquer un seuillage binaire pour obtenir un masque de l'ombre
            Mat binary;
            threshold(gray, binary, threshold_value, 255, cv::THRESH_BINARY);

            // Inverser l'image binaire pour obtenir les régions de l'ombre
            Mat binary_inverse;
            bitwise_not(binary, binary_inverse);

            // Remplacer les pixels de l'ombre par des pixels de l'image d'origine
            Mat image_no_shadow;
            bitwise_or(image, image, image_no_shadow, binary_inverse);

            cout << "duration : " << getTimeDiff(t) << " ms" << endl;

            imshow(window2, image_no_shadow);
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
            if (threshold_value > 0)
            {
                threshold_value--;
                hasToCompute = true;
            }
        }
        else if (k == 83)  // Right Arrow
        {
            if (threshold_value < 255)
            {
                threshold_value++;
                hasToCompute = true;
            }
        }
    }
    destroyAllWindows();
    return 0;
}
