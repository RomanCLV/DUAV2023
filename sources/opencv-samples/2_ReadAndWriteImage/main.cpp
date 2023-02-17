// https://docs.opencv.org/4.x/db/deb/tutorial_display_image.html
// https://github.com/opencv/opencv/blob/4.x/samples/cpp/tutorial_code/introduction/display_image/display_image.cpp

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    string image_path;
    string saved_image_path;
    string mode_name;
    Mat img;

    if (argc < 3 || argc > 4) 
    {
        printf("Usage: ./main.out <image_path> <saved_image_path> [mode]\n");
        printf("Read the README.md\n");
        return -1;
    }

    image_path = samples::findFile(argv[1]);
    saved_image_path = argv[2];

    cv::ImreadModes mode = IMREAD_UNCHANGED;

    if (argc == 4)
    {   
        if (!strcmp(argv[3], "u") || !strcmp(argv[3], "U"))
        { 
            mode_name = "IMREAD_UNCHANGED";
        }
        else if (!strcmp(argv[3], "c") || !strcmp(argv[3], "C"))
        {
            mode =  IMREAD_COLOR;
            mode_name = "IMREAD_COLOR";
        }
        else if (!strcmp(argv[3], "g") || !strcmp(argv[3], "G"))
        {
            mode = IMREAD_GRAYSCALE;
            mode_name = "IMREAD_GRAYSCALE";
        }
        else
        {
            printf("Unexpected mode value\n");
            printf("Read the README.md\n");
            return -1;
        }
        cout << "Read mode: " << mode_name << endl;
    }

    img = imread(image_path, mode);

    if(img.empty())
    {
        cout << "Could not read the image: " << image_path << endl;
        return -1;
    }
    imshow("Read Image", img);
    /*
    printf("Press 's' to save the image or any other key to quit.\n");
    int k = waitKey(0); // Wait for a keystroke in the window
    if(k == 's')
    {
        printf("Image saved to ./img1.png\n");
        imwrite("img1.png", img);
    }*/
    cout << "Image saved to " << saved_image_path << endl;
    imwrite(saved_image_path, img);
    cout << "Press a key to quit" << endl;
    waitKey(0);
    cv::destroyAllWindows();
    return 0;
}