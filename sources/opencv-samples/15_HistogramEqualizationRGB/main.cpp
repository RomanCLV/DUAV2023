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
    string window2 = "Equalized";

    double t = getTime();

    // Séparer les canaux de couleur
    vector<Mat> channels;
    split(image, channels);
    Mat b = channels[0];
    Mat g = channels[1];
    Mat r = channels[2];

    // Appliquer l'égalisation d'histogramme à chaque canal de couleur
    equalizeHist(b, b);
    equalizeHist(g, g);
    equalizeHist(r, r);

    // Fusionner les canaux de couleur en une seule image
    Mat equalized;
    merge(vector<Mat>({b, g, r}), equalized);

    cout << "duration : " << getTimeDiff(t) << " ms" << endl;

    imshow(window1, image);
    imshow(window2, equalized);

    waitKey(0);
    destroyAllWindows();
    return 0;
}
