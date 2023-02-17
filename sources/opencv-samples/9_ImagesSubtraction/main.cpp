// https://docs.opencv.org/4.x/db/df5/tutorial_linux_gcc_cmake.html

#include <stdio.h>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    if ( argc < 3 || argc > 4 )
    {
        printf("Usage: ./main.out <image1_path> <image2_path> [g|G]\n");
        printf("Read the README.md\n");
        return -1;
    }
    cv::ImreadModes mode = IMREAD_UNCHANGED;
    if (argc == 4)
    {   
        if (!strcmp(argv[3], "g") || !strcmp(argv[3], "G"))
        { 
            mode = IMREAD_GRAYSCALE;
        }
        else
        {
            printf("Unexpected mode value\n");
            printf("Read the README.md\n");
            return -1;
        }
    }
    Mat image1;
    Mat image2;
    Mat subtracted;

    image1 = imread(argv[1], mode);
    image2 = imread(argv[2], mode);

    if ( !image1.data )
    {
        cout << "Could not read the image " << argv[1] << endl;
        return -1;
    }
    if ( !image2.data )
    {
        cout << "Could not read the image " << argv[2] << endl;
        return -1;
    }

    int channelsImage1 = image1.channels();
    int rowsImage1 = image1.rows;
    int colsImage1 = image1.cols;
    int depthImage1 = image1.depth();

    int channelsImage2 = image2.channels();
    int rowsImage2 = image2.rows;
    int colsImage2 = image2.cols;
    int depthImage2 = image2.depth();

    if (channelsImage1 != channelsImage2 || rowsImage1 != rowsImage2 || colsImage1 != colsImage2 || depthImage1 != depthImage2)
    {
        cout << "Not the same dimensions or depth!\t";
        cout << "image 1: (" << rowsImage1 << "," << colsImage1 << "," << channelsImage1 << ") (" << depthImage1 << ")\t";
        cout << "image 2: (" << rowsImage2 << "," << colsImage2 << "," << channelsImage2 << ") (" << depthImage2 << ")" << endl;
        return -1;
    }

    double delay = (double)getTickCount();
    cv::subtract(image1, image2, subtracted);
    delay = 1000 * ((double)getTickCount() - delay) / getTickFrequency();
    cout << "Subtraction delay: " << (int)(delay * 1000) / 1000.0 << " ms" << endl;

    imshow("Image 1", image1);
    imshow("Image 2", image2);
    imshow("Substract", subtracted);

    std::cout << "Press a key to quit" << std::endl;
    waitKey(0);
    destroyAllWindows();
    return 0;
}