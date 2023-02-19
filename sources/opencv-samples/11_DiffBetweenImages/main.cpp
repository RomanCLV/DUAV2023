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

int main(int argc, char** argv) {

    // Install a signal handler
    std::signal(SIGINT, signal_handler);

    if (argc != 3)
    {
        printf("Usage: ./main.out <image_path_1> <image_path_2>");
        return -1;
    }

    // Charger les deux images
    Mat image1 = imread(argv[1]);
    Mat image2 = imread(argv[2]);

    if (!image1.data)
    {
        cout << "Could not read the image " << argv[1] << endl;
        return -1;
    }
    if (!image2.data)
    {
        cout << "Could not read the image " << argv[2] << endl;
        return -1;
    }

    printf("\n");
    printf("press Left Arrow to decrease the threshold value\n");
    printf("press Rigth Arrow to increase the threshold value\n\n");
    printf("press Up Arrow to increase the kernel size\n");
    printf("press Down Arrow to decrease the kernel size\n");
    printf("press ESC to quit\n\n");

    char k;

    string window1 = "Image 1";
    string window2 = "Image 2";
    string window3 = "Différence";
    string window4 = "Différence (ouverture morphologique)";

    Mat gray_image1;
    Mat gray_image2;
    Mat diff_image;

    // Pour la binarisation de l'image
    Mat threshold_image;
    int threshold_value = 30;   // seuil de binarisation

    // Pour l'ouverture morphologique
    Mat opened_image;
    int kernel_size = 3;
    Mat kernel = getStructuringElement(MORPH_RECT, Size(kernel_size, kernel_size));

    double t;
    bool hasToCompute = true;

    // Convertir les images en niveaux de gris

    t = getTime();
    cvtColor(image1, gray_image1, COLOR_BGR2GRAY);
    cout << "conversion en gris image 1 : " << getTimeDiff(t) << " ms" << endl;

    t = getTime();
    cvtColor(image2, gray_image2, COLOR_BGR2GRAY);
    cout << "conversion en gris image 2 : " << getTimeDiff(t) << " ms" << endl;

    t = getTime();
    
    // Calculer la différence absolue entre les deux images
    absdiff(gray_image1, gray_image2, diff_image);
    
    cout << "différence absolue : " << getTimeDiff(t) << " ms" << endl;

    namedWindow(window1, WINDOW_NORMAL);
    namedWindow(window2, WINDOW_NORMAL);
    namedWindow(window3, WINDOW_NORMAL);
    namedWindow(window4, WINDOW_NORMAL);

    imshow(window1, image1);
    imshow(window2, image2);

    while (1)
    {
        if (hasToCompute)
        {
            hasToCompute = false;
            cout << endl;
            cout << "threshold : " << threshold_value << endl;
            cout << "kernel size : " << kernel_size << endl;
            
            t = getTime();
            
            // Appliquer un seuil pour obtenir une image binaire
            threshold(diff_image, threshold_image, threshold_value, 255, THRESH_BINARY);

            cout << "threshold : " << getTimeDiff(t) << " ms" << endl;

            t = getTime();

            // Appliquer une ouverture morphologique
            morphologyEx(threshold_image, opened_image, MORPH_OPEN, kernel);

            cout << "ouverture morphologique : " << getTimeDiff(t) << " ms" << endl;

            imshow(window3, threshold_image);
            imshow(window4, opened_image);
        }

        k = (char)waitKey(0);

        if (hasToBreak ||                                                   // due to Ctrl+C
            k == 27 ||                                                      // ESC key
            cv::getWindowProperty(window1, WND_PROP_AUTOSIZE) == -1 ||
            cv::getWindowProperty(window2, WND_PROP_AUTOSIZE) == -1 ||
            cv::getWindowProperty(window3, WND_PROP_AUTOSIZE) == -1 ||
            cv::getWindowProperty(window4, WND_PROP_AUTOSIZE) == -1)    // window is closed
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
        else if (k == 84)  // Down Arrow
        {
            if (kernel_size > 0)
            {
                kernel_size--;
                kernel = getStructuringElement(MORPH_RECT, Size(kernel_size, kernel_size));
                hasToCompute = true;
            }
        }
        else if (k == 82)  // Up Arrow
        {
            if (kernel_size < 255)
            {
                kernel_size++;
                kernel = getStructuringElement(MORPH_RECT, Size(kernel_size, kernel_size));
                hasToCompute = true;
            }
        }
    }
    destroyAllWindows();
    return 0;
}
