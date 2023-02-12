// https://docs.opencv.org/4.x/db/df5/tutorial_linux_gcc_cmake.html

#include <stdio.h>
#include <opencv2/opencv.hpp>
using namespace cv;

int main(int argc, char** argv)
{
    if ( argc != 2 )
    {
        printf("Usage: ./main.out <Image_Path>\n");
        printf("Read the README.md\n");
        return -1;
    }
    Mat image;
    image = imread( argv[1], IMREAD_UNCHANGED );
    if ( !image.data )
    {
        printf("Could not read the image\n");
        return -1;
    }
    namedWindow("Read Image", WINDOW_AUTOSIZE );
    imshow("Read Image", image);
    std::cout << "Press a key to quit" << std::endl;
    waitKey(0);
    return 0;
}