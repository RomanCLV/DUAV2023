#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

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

    string window1 = "Image";
    string window2 = "Result";

    double t = getTime();

    // Convertir en niveaux de gris
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

    // Appliquer un seuillage adaptatif pour réduire le bruit
    cv::Mat threshold;
    cv::adaptiveThreshold(gray, threshold, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 2);

    // Appliquer la détection de contour
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    cv::findContours(threshold, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    // Créer un masque pour l'ombre
    cv::Mat shadow_mask = cv::Mat::zeros(image.size(), CV_8UC1);
    cv::drawContours(shadow_mask, contours, -1, cv::Scalar(255), 0);

    // Inverser le masque pour remplacer l'ombre par des pixels de l'image d'origine
    cv::Mat inverted_mask = cv::Scalar::all(255) - shadow_mask;
    cv::Mat image_no_shadow;
    cv::bitwise_or(image, image, image_no_shadow, inverted_mask);

    cout << "duration : " << getTimeDiff(t) << " ms" << endl;

    imshow(window1, image);
    imshow(window2, image_no_shadow);

    waitKey(0);
    destroyAllWindows();
    return 0;
}
