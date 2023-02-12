// https://docs.opencv.org/4.x/db/da5/tutorial_how_to_scan_images.html
// https://github.com/opencv/opencv/blob/4.x/samples/cpp/tutorial_code/core/how_to_scan_images/how_to_scan_images.cpp

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <iostream>
#include <sstream>

using namespace std;
using namespace cv;

// headers
Mat& ScanImageAndReduceC(Mat& I, const uchar* table);
Mat& ScanImageAndReduceIterator(Mat& I, const uchar* table);
Mat& ScanImageAndReduceRandomAccess(Mat& I, const uchar * table);
// headers

static void help()
{
    cout
        << "\n--------------------------------------------------------------------------" << endl
        << "Usage: ./main.out <image_path> <divide_with> [g|G]"                         << endl
        << "if you add a g|G parameter the image is processed in gray scale"              << endl
        << "--------------------------------------------------------------------------"   << endl
        << endl;
}

int main( int argc, char* argv[])
{
    
    if (argc < 3)
    {
        help();
        cout << "Not enough parameters" << endl;
        return -1;
    }
    if (argc > 5) 
    {
        help();
        cout << "Not many parameters" << endl;
        return -1;
    }


    Mat I, R1, R2, R3, R4;
    if (argc == 4)
    {
        if (!strcmp(argv[3],"g") || !strcmp(argv[3],"G"))
        {
            I = imread(argv[1], IMREAD_GRAYSCALE);            
        }
        else 
        {
            help();
            printf("Unexpected parameter given\n");
            return -1;
        }
    }
    else
    {
        I = imread(argv[1], IMREAD_COLOR);
    }

    if (I.empty())
    {
        cout << "The image" << argv[1] << " could not be loaded." << endl;
        return -1;
    }

    R1 = I.clone();
    R2 = I.clone();
    R3 = I.clone();
    R4 = I.clone();

    //! [divideWith]
    int divideWith = 0; // convert our input string to number - C++ style
    stringstream s;
    s << argv[2];
    s >> divideWith;
    if (!s || !divideWith)
    {
        cout << "Invalid number entered for dividing. " << endl;
        return -1;
    }

    uchar table[256];
    for (int i = 0; i < 256; ++i)
    {
        table[i] = (uchar)(divideWith * (i/divideWith));
    }
    //! [divideWith]

    const int times = 100;
    double t;

    t = (double)getTickCount();

    for (int i = 0; i < times; ++i)
    {
        cv::Mat clone_i = R1.clone();
        R1 = ScanImageAndReduceC(clone_i, table);
    }

    t = 1000*((double)getTickCount() - t)/getTickFrequency();
    t /= times;

    cout << "Time of reducing with the C operator [] (averaged for "
         << times << " runs): " << t << " milliseconds."<< endl;

    t = (double)getTickCount();

    for (int i = 0; i < times; ++i)
    {
        cv::Mat clone_i = R1.clone();
        R2 = ScanImageAndReduceIterator(clone_i, table);
    }

    t = 1000*((double)getTickCount() - t)/getTickFrequency();
    t /= times;

    cout << "Time of reducing with the iterator (averaged for "
        << times << " runs): " << t << " milliseconds."<< endl;

    t = (double)getTickCount();

    for (int i = 0; i < times; ++i)
    {
        cv::Mat clone_i = R3.clone();
        R3 = ScanImageAndReduceRandomAccess(clone_i, table);
    }

    t = 1000*((double)getTickCount() - t)/getTickFrequency();
    t /= times;

    cout << "Time of reducing with the on-the-fly address generation - at function (averaged for "
        << times << " runs): " << t << " milliseconds."<< endl;

    //! [table-init]
    Mat lookUpTable(1, 256, CV_8U);
    uchar* p = lookUpTable.ptr();
    for( int i = 0; i < 256; ++i)
    {
        p[i] = table[i];
    }
    //! [table-init]

    t = (double)getTickCount();

    for (int i = 0; i < times; ++i)
    {
        //! [table-use]
        LUT(I, lookUpTable, R4);
        //! [table-use]
    }

    t = 1000*((double)getTickCount() - t)/getTickFrequency();
    t /= times;

    cout << "Time of reducing with the LUT function (averaged for "
        << times << " runs): " << t << " milliseconds."<< endl;

    imshow("Initiale image", I);
    imshow("C", R1);
    imshow("Iterator", R2);
    imshow("Random Access", R3);
    imshow("LUT", R4);

    cout << "Press a key to quit" << endl;
    waitKey(0);
    return 0;
}

//! [scan-c]
Mat& ScanImageAndReduceC(Mat& I, const uchar* const table)
{
    // accept only char type matrices
    CV_Assert(I.depth() == CV_8U);

    int channels = I.channels();

    int nRows = I.rows;
    int nCols = I.cols * channels;

    if (I.isContinuous())
    {
        nCols *= nRows;
        nRows = 1;
    }

    int i,j;
    uchar* p;
    for( i = 0; i < nRows; ++i)
    {
        p = I.ptr<uchar>(i);
        for ( j = 0; j < nCols; ++j)
        {
            p[j] = table[p[j]];
        }
    }
    return I;
}
//! [scan-c]

//! [scan-iterator]
Mat& ScanImageAndReduceIterator(Mat& I, const uchar* const table)
{
    // accept only char type matrices
    CV_Assert(I.depth() == CV_8U);

    const int channels = I.channels();
    switch(channels)
    {
        case 1:
        {
            MatIterator_<uchar> it, end;
            for( it = I.begin<uchar>(), end = I.end<uchar>(); it != end; ++it)
                *it = table[*it];
            break;
        }
        case 3:
        {
            MatIterator_<Vec3b> it, end;
            for( it = I.begin<Vec3b>(), end = I.end<Vec3b>(); it != end; ++it)
            {
                (*it)[0] = table[(*it)[0]];
                (*it)[1] = table[(*it)[1]];
                (*it)[2] = table[(*it)[2]];
            }
            break;
        }
    }

    return I;
}
//! [scan-iterator]

//! [scan-random]
Mat& ScanImageAndReduceRandomAccess(Mat& I, const uchar* const table)
{
    // accept only char type matrices
    CV_Assert(I.depth() == CV_8U);

    const int channels = I.channels();
    switch(channels)
    {
        case 1:
        {
            for( int i = 0; i < I.rows; ++i)
                for( int j = 0; j < I.cols; ++j )
                    I.at<uchar>(i,j) = table[I.at<uchar>(i,j)];
            break;
        }
        case 3:
        {
            Mat_<Vec3b> _I = I;

            for( int i = 0; i < I.rows; ++i)
            {
                for( int j = 0; j < I.cols; ++j )
                {
                   _I(i,j)[0] = table[_I(i,j)[0]];
                   _I(i,j)[1] = table[_I(i,j)[1]];
                   _I(i,j)[2] = table[_I(i,j)[2]];
                }    
            }
            I = _I;
            break;
        }
    }

    return I;
}
//! [scan-random]
