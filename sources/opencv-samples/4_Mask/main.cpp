// https://docs.opencv.org/4.x/d7/d37/tutorial_mat_mask_operations.html
// https://raw.githubusercontent.com/opencv/opencv/4.x/samples/cpp/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

static void help()
{
    cout << endl
        <<  "This program shows how to filter images with mask: the write it yourself and the filter2d way. " << endl
        <<  "Usage: ./main.out <image_path> [g|G -- grayscale] " << endl << endl;
}

//! [headers]
void Sharpen(const Mat& myImage,Mat& Result);
//! [headers]

int main( int argc, char* argv[])
{
    if (argc < 2)
    {
        help();
    	printf("Not enough parameters\n");
    	return -1;
    }
    if (argc > 3)
    {
        help();
        printf("Too many parameters\n");
        return -1;
    }

	const char* filename = argv[1];

    Mat src, dst0, dst1;

    if (argc == 3)
    {
        if (!strcmp("g", argv[2]) || !strcmp("G", argv[2]))
        {
            src = imread( samples::findFile( filename ), IMREAD_GRAYSCALE);
        }
        else
        {
            printf("Unexpected parameter given");
            help();
            return -1;
        }
    }
    else
    {
        src = imread( samples::findFile( filename ), IMREAD_COLOR);
    }

    if (src.empty())
    {
        cerr << "Can't open image ["  << filename << "]" << endl;
        return EXIT_FAILURE;
    }
    
    double t = (double)getTickCount();
    Sharpen( src, dst0 );
    t = ((double)getTickCount() - t)/getTickFrequency();
    cout << "Hand written function time passed in seconds: " << t << endl;

    //![kern]
    Mat kernel = (Mat_<char>(3,3) << 0, -1,  0,
                                    -1,  5, -1,
                                     0, -1,  0);
    //![kern]

    t = (double)getTickCount();

    //![filter2D]
    filter2D( src, dst1, src.depth(), kernel );
    //![filter2D]
    t = ((double)getTickCount() - t)/getTickFrequency();
    cout << "Built-in filter2D time passed in seconds:     " << t << endl;

    namedWindow("Input", WINDOW_AUTOSIZE);
    namedWindow("Output 1", WINDOW_AUTOSIZE);
    namedWindow("Output 2", WINDOW_AUTOSIZE);

	imshow( "Input", src );
    imshow( "Output 1", dst0 );
    imshow( "Output 2", dst1 );

    cout << "Press a key to quit" << endl;
    waitKey(0);
    return EXIT_SUCCESS;
}

//! [basic_method]
void Sharpen(const Mat& myImage, Mat& Result)
{
    //! [8_bit]
    CV_Assert(myImage.depth() == CV_8U);  // accept only uchar images
    //! [8_bit]

    //! [create_channels]
    const int nChannels = myImage.channels();
    Result.create(myImage.size(), myImage.type());
    //! [create_channels]

    //! [basic_method_loop]
    for(int j = 1 ; j < myImage.rows-1; ++j)
    {
        const uchar* previous = myImage.ptr<uchar>(j - 1);
        const uchar* current  = myImage.ptr<uchar>(j    );
        const uchar* next     = myImage.ptr<uchar>(j + 1);

        uchar* output = Result.ptr<uchar>(j);

        for(int i = nChannels; i < nChannels * (myImage.cols - 1); ++i)
        {
            *output++ = saturate_cast<uchar>(5*current[i]
                         -current[i-nChannels] - current[i+nChannels] - previous[i] - next[i]);
        }
    }
    //! [basic_method_loop]

    //! [borders]
    Result.row(0).setTo(Scalar(0));
    Result.row(Result.rows-1).setTo(Scalar(0));
    Result.col(0).setTo(Scalar(0));
    Result.col(Result.cols-1).setTo(Scalar(0));
    //! [borders]
}
//! [basic_method]
