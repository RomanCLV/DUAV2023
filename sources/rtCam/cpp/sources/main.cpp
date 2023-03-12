#include "main.hpp"

using namespace cv;
using namespace std;

bool hasToBreak = false;

void signalHandler(int signal)
{
    hasToBreak = true;
}

inline void help()
{
    cout << "" << endl;
    cout << "--- Function -------   --- KEY -----" << endl;
    cout << "" << endl;
    cout << "    Clear detection        C" << endl;
    cout << "    Display duration       D" << endl;
    cout << "    Decrease option        Left Arrow" << endl;
    cout << "    Increase option        Right Arrow" << endl;
    cout << "    Help                   H" << endl;
    cout << "    Pause                  SPACE BAR" << endl;
    cout << "    Quit                   ESC" << endl;
    cout << "    Display config         I" << endl;
    cout << "    Reset config           R" << endl;
    cout << "    Save config            S" << endl;
    cout << "" << endl;
    cout << " Select an option:" << endl;
    cout << "" << endl;
    cout << "    Gaussian blur          G" << endl;
    cout << "    Threshold              T" << endl;
    cout << "    Kernel size            K" << endl;
    cout << "    Detection area         A" << endl;
    cout << "" << endl;
    cout << "----------------------------------" << endl;
}

inline bool fileExists(const string& name)
{
    struct stat buffer;   
    return (stat (name.c_str(), &buffer) == 0); 
}

inline bool removeFile(const string& name)
{
    if (fileExists(name))
    {
        return remove(name.c_str()) == 0;
    }
    return false;
}

inline double getTime()
{
    return (double)getTickCount();
}

inline double getTimeDiff(const double time_ori)
{
    return std::round((getTime() - time_ori) / getTickFrequency() * 1000000.0) / 1000.0;
}

void parseArgs(int argc, char** argv, std::map<std::string, std::vector<std::string>>& args) 
{
    std::string currentOption;
    for (int i = 1; i < argc; ++i) 
    {
        std::string arg = argv[i];
        if (arg[0] == '-') 
        {
            // New option detected
            currentOption = arg.substr(1);
            args[currentOption] = std::vector<std::string>();
        }
        else
        {
            // Argument for current option
            args[currentOption].push_back(arg);
        }
    }
}

inline unsigned int myWaitKey(unsigned int millis)
{
    return cv::waitKeyEx(millis) & 0xFFFF;
}

int main(int argc, char** argv)
{   
    // Install a signal handler
    std::signal(SIGINT, signalHandler);

    std::map<std::string, std::vector<std::string>> args;
    parseArgs(argc, argv, args);

    VideoCapture* cap(nullptr);

    unsigned int k = 0;
    const string RTH_PATH = "./RTH";
    Option selectedOption = Option::None;
    double duration = 0.0;

    double fps = 0.0;
    unsigned int frameToTake = 0;
    unsigned int frameCount = 0;
    unsigned int frameWaitDelay = 0;

    Mat images[2];
    Mat frame;
    Mat previousFrame;

    Mat grayImage1;
    Mat grayImage2;
    Mat gaussianImage1;
    Mat gaussianImage2;
    Mat diffImage;

    bool debug = false;
    bool display = false;
    bool pause = false;
    bool readNextFrame = true;
    bool detectionEnabled = true;
    bool somethingDetected = false;
    bool clearDetection = false;
    bool configChanged = false;

    bool imageMode = false;
    bool videoMode = false;

    // to display red rectangle
    vector<vector<Point>> contours;
    double maxArea = -1.0;
    int maxAreaId = -1;
    double currentArea = 0.0;

    int i = 0;

    Mat thresholdImage;
    Mat mask;
    Mat result;

    string windowPrevFrame = "Previous frame";
    string windowCurrFrame = "Current frame";
    string windowMask = "Mask";
    string windowResult = "Result";

    Config config;
    if (!config.read())
    {
        return -1;
    }

    if (args.count("db"))
    {
        config.setDebug(true);
    }
    if (args.count("dd"))
    {
        config.setDisplayDuration(true);
    }
    if (args.count("d"))
    {
        config.setDisplayOptionalWindows(true);
    }
    debug = config.getDebug();
    display = config.getDisplayOptionalWindows();

    if (args.count("i"))
    {
        if (args["i"].size() != 2)
        {
            cout << "Wrong usage of image option: -i img1 img2" << endl;
            return -1;
        }
        for (int i = 0; i < 2; i++)
        {
            Mat image = imread(args["i"][i]);
            if ( !image.data )
            {
                cout << "Could not read the image:" << args["i"][i] << endl;
                return 0;
            }
            images[i] = image;
        }
        imageMode = true;
    }
    else if (args.count("v"))
    {
        if (args["v"].size() != 1)
        {
            cout << "Wrong usage of video option: -v vid" << endl;
            return -1;
        }
        cap = new VideoCapture(args["v"][0]);
        if (!cap->isOpened())
        {
            cout << "Can't open video " << args["v"][0] << endl;
            return 0;
        }
        videoMode = true;
    }
    else
    {
        printf("Trying to open /dev/video0 with CAP_V4L2\n");
        cap = new VideoCapture(0, cv::CAP_V4L2);

        if (!cap->isOpened()) 
        {
            cout << "Can not read the device" << endl;
            return 0;
        }
    }

    if (cap != nullptr)
    {
        fps = cap->get(cv::CAP_PROP_FPS);
        frameToTake = (int)(fps / 4);
        frameCount = frameToTake;
        frameWaitDelay = (int)(1000.0 / fps);
        cout << "fps: " << fps << endl;
        cout << "frame took every: " << frameToTake << endl;
        cout << "frame wait delay: " << frameWaitDelay << " ms" << endl;
    }

    if (fileExists(RTH_PATH) && !removeFile(RTH_PATH))
    {
        cout << "Cannot remove " << RTH_PATH << " file." << endl;
        return -1;
    }

    cout << endl << "config:" << endl;
    cout << "debug: " << (config.getDebug() ? "true" : "false") << endl;
    cout << "display optional windows: " << (config.getDisplayOptionalWindows() ? "true" : "false") << endl;
    cout << "display duration: " << (config.getDisplayDuration() ? "true" : "false") << endl;
    config.display('\n', '\0');

    cout << "press [H] to print the help" << endl;

    if (display)
    {
        namedWindow(windowPrevFrame);
        namedWindow(windowCurrFrame);
        namedWindow(windowMask);
    }
    namedWindow(windowResult);
    waitKey(500);

    while (true)
    {
        if (hasToBreak)
        {
            break;
        }
        k = 0;

        if (pause && !configChanged)
        {
            k = myWaitKey(500);
        }
        else
        {
            duration = getTime();
            if (!pause)
            {
                if (readNextFrame)
                {
                    if (imageMode)
                    {
                        previousFrame = images[0];
                        frame = images[1];
                    }
                    else
                    {
                        *cap >> frame;
                    }
                }
                else
                {
                    readNextFrame = true;
                    frameCount = frameToTake;
                }
            }
            else
            {
                frameCount = frameToTake;
            }

            if (!frame.empty()) 
            {
                if (frameCount >= frameToTake)
                {
                    frameCount = 1;

                    if (!previousFrame.empty())
                    {
                        if (frame.channels() == previousFrame.channels() && 
                            frame.rows == previousFrame.rows && 
                            frame.cols == previousFrame.cols && 
                            frame.depth() == previousFrame.depth())
                        {
                            cvtColor(previousFrame, grayImage1, COLOR_BGR2GRAY);
                            cvtColor(frame, grayImage2, COLOR_BGR2GRAY);
                            if (config.getGaussianBlur() == 0)
                            {
                                absdiff(grayImage1, grayImage2, diffImage);
                            }
                            else
                            {
                                cv::GaussianBlur(grayImage1, gaussianImage1, config.getGaussianKernel(), 0);
                                cv::GaussianBlur(grayImage2, gaussianImage2, config.getGaussianKernel(), 0);
                                absdiff(gaussianImage1, gaussianImage2, diffImage);
                            }
                                
                            threshold(diffImage, thresholdImage, config.getThreshold(), 255, THRESH_BINARY);
                            morphologyEx(thresholdImage, mask, MORPH_OPEN, config.getKernel());

                            // Trouver tous les contours dans l'image
                            contours.clear();
                            findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
                            
                            // Trouver le plus grand contour dans l'image
                            maxArea = -1.0;
                            maxAreaId = -1;
                            for (i = 0; i < contours.size(); i++)
                            {
                                currentArea = contourArea(contours[i]);
                                if (debug)
                                {
                                    cout << "area: " << currentArea << endl;
                                }
                                if (currentArea > maxArea)
                                {
                                    maxArea = currentArea;
                                    maxAreaId = i;
                                }
                            }

                            frame.copyTo(result);
                            somethingDetected = false;

                            // Dessiner un rectangle autour du plus grand contour
                            if (maxArea >= config.getDetectionArea())
                            {
                                Rect rect = boundingRect(contours[maxAreaId]);
                                rectangle(result, rect, config.getRectangleColor(), 2);
                                somethingDetected = true;
                            }

                            if (config.getDisplayDuration())
                            {
                                cout << getTimeDiff(duration) << " ms" << endl;
                            }

                            if (display)
                            {
                                imshow(windowPrevFrame, previousFrame);
                                imshow(windowCurrFrame, frame);
                                imshow(windowMask, mask);
                                imshow(windowResult, result);
                                if (debug)
                                {
                                    k = myWaitKey(0);
                                }
                            }
                            else
                            {
                                imshow(windowResult, result);
                            }

                            if (somethingDetected)
                            {
                                // TODO: detectionEnabled in rtCam or in automate.py ??
                                if (detectionEnabled)
                                {
                                    //cout << "object detected!" << endl;
                                    if (!fileExists(RTH_PATH))
                                    {
                                        // create RTH file and close it
                                        ofstream file(RTH_PATH.c_str());
                                        file.close();
                                    }
                                }
                            }
                            else
                            {
                                if (!configChanged)
                                {
                                    frame.copyTo(previousFrame);
                                }
                            }

                            if (clearDetection)
                            {
                                clearDetection = false;
                                cout << "detection cleared" << endl;
                                frame.copyTo(previousFrame);
                            }
                        }
                        else
                        {
                            cout << "frames haven't the same dimensions or depth!" << endl;
                            frame.copyTo(previousFrame);
                        }

                        if (imageMode)
                        {
                            k = myWaitKey(0);
                        }
                    }
                    else
                    {
                        frame.copyTo(previousFrame);
                    }

                    if (k == 0)
                    {
                        k = myWaitKey(1);
                    }
                }
                else
                {
                    frameCount++;
                    k = myWaitKey(frameWaitDelay);
                }
            }
            else
            {
                if (videoMode)
                {
                    break;
                }
                cout << "frame is empty" << endl;
                k = myWaitKey(500);
            }
        }

        configChanged = false;

        if (hasToBreak ||                                                  // due to Ctrl+C
            k == 27 ||                                                      // ESC key
            cv::getWindowProperty(windowResult, WND_PROP_AUTOSIZE) == -1)  // window is closed
        {
            break;
        }
        
        else if (k == 32)               // SPACE BAR
        {
            pause = !pause;
        }
        
        else if (k == 67 || k == 99)    // C | c
        {
            clearDetection = true;
        }
        
        else if (k == 68 || k == 100)   // D | d
        {
            config.inverseDisplayDuration();
            readNextFrame = !(debug || imageMode);
        }
        
        else if (k == 72 || k == 104)   // H | h
        {
            help();
            readNextFrame = !(debug || imageMode);
        }
        
        else if (k == 82 || k == 114)   // R | r
        {
            config.reset();
            config.display();
            configChanged = true;
            readNextFrame = !(debug || imageMode);
        }

        else if (k == 83 || k == 115)   // S | s
        {
            config.save();
        }
       
        else if (k == 71 || k == 103)   // G | g
        {
            selectedOption = Option::GaussianBlur;
            cout << "selected option: Gaussian blur" << endl;
            configChanged = true;
            readNextFrame = !(debug || imageMode);
        }

        else if (k == 84 || k == 116)   // T | t
        {
            selectedOption = Option::Threshold;
            cout << "selected option: Threshold" << endl;
            configChanged = true;
            readNextFrame = !(debug || imageMode);
        }

        else if (k == 75 || k == 107)   // K | k
        {
            selectedOption = Option::KernelSize;
            cout << "selected option: Kernel size" << endl;
            configChanged = true;
            readNextFrame = !(debug || imageMode);
        }

        else if (k == 65 || k == 97)    // A | a
        {
            selectedOption = Option::DetectionArea;
            cout << "selected option: Detection area" << endl;
            configChanged = true;
            readNextFrame = !(debug || imageMode);
        }

        else if (k == 65361)            // Left Arrow
        {
            readNextFrame = false;
            configChanged = true;

            if (selectedOption == Option::None)
            {
                cout << "No option slected. Press [H] to print the help" << endl;
            }

            else if (selectedOption == Option::GaussianBlur)
            {
                if (config.decreaseGaussianBlur())
                {
                    config.display();
                }
            }

            else if (selectedOption == Option::Threshold)
            {
                if (config.decreaseThreshold())
                {
                    config.display();
                }
            }

            else if (selectedOption == Option::KernelSize)
            {
                if (config.decreaseKernelSize())
                {
                    config.display();
                }
            }

            else if (selectedOption == Option::DetectionArea)
            {
                if (config.decreaseDetectionArea())
                {
                    config.display();
                }
            }

        }

        else if (k == 65363)            // Right Arrow
        {
            readNextFrame = false;
            configChanged = true;

            if (selectedOption == Option::None)
            {
                cout << "No option slected. Press [H] to print the help" << endl;
            }

            else if (selectedOption == Option::GaussianBlur)
            {
                if (config.increaseGaussianBlur())
                {
                    config.display();
                }
            }

            else if (selectedOption == Option::Threshold)
            {
                if (config.increaseThreshold())
                {
                    config.display();
                }
            }

            else if (selectedOption == Option::KernelSize)
            {
                if (config.increaseKernelSize())
                {
                    config.display();
                }
            }

            else if (selectedOption == Option::DetectionArea)
            {
                if (config.increaseDetectionArea())
                {
                    config.display();
                }
            }
        }

        if (imageMode && readNextFrame)
        {
            break;
        }
    }

    if (cap != nullptr)
    {
        cap->release();
        delete cap;
        cap = nullptr;
    }
    cv::destroyAllWindows();

    removeFile(RTH_PATH);
    return 0;
}
