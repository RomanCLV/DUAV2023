#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;


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

    string window1 = "Image gray";
    string window2 = "Equalized";

    double t = getTime();

    // Convertir en niveaux de gris
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    
    // Appliquer l'égalisation d'histogramme
    cv::Mat equalized;
    cv::equalizeHist(gray, equalized);

    cout << "duration : " << getTimeDiff(t) << " ms" << endl;

    imshow(window1, image);
    imshow(window2, equalized);

    waitKey(0);
    destroyAllWindows();
    return 0;
}
