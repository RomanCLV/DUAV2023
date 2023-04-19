#include "main.hpp"

using namespace cv;
using namespace std;
using namespace boost::asio;


volatile bool hasToBreak = false;

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

void createDirIfNotExists(const char* folderName)
{
    struct stat st = { 0 };
    if (stat(folderName, &st) == -1) 
    {
        mkdir(folderName, 0700);
    }
}

inline double getTime()
{
    return (double)getTickCount();
}

inline double getTimeDiff(const double timeStart)
{
    return (getTime() - timeStart) / getTickFrequency() * 1000.0;
}

inline double round3(const double value)
{
    return std::round(value * 1000.0) / 1000.0;
}

inline unsigned int myWaitKey(unsigned int millis)
{
    return cv::waitKeyEx(millis) & 0xFFFF;
}

std::string concatenateFolderNameWithDate(const char* folderName, const char* endName) 
{
    // Obtenir la date et l'heure actuelles
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    // Convertir la date et l'heure actuelles en chaîne de caractères
    char dateTimeString[20];
    std::strftime(dateTimeString, sizeof(dateTimeString), "%Y_%m_%d_%H_%M_%S", std::localtime(&currentTime));

    // Concaténer le nom de dossier avec la date et l'heure actuelles et le nom de fichier
    std::ostringstream oss;
    oss << folderName << "/" << dateTimeString << endName;
    return oss.str();
}

bool isSshConnected(const bool displayProccesInfo)
{
    bool isSsh = false;
    pid_t pid = getpid();

    while (pid != 0)
    {
        PROCTAB* proc_tab = openproc(PROC_FILLSTAT | PROC_FILLSTATUS | PROC_FILLMEM | PROC_PID, &pid);
        proc_t* proc_ptr;

        if (proc_ptr = readproc(proc_tab, NULL))
        {
            if (displayProccesInfo)
            {
                std::cout << "Parent PID: " << proc_ptr->tid << ", Name: " << proc_ptr->cmd << std::endl;
            }
            if (!isSsh)
            {
                if (!strcmp(proc_ptr->cmd, "ssh") || !strcmp(proc_ptr->cmd, "sshd"))
                {
                    isSsh = true;
                }
            }
            pid = proc_ptr->ppid;
        } 
        else 
        {
            std::cerr << "Impossible de lire les informations du processus avec le PID : " << pid << std::endl;
            break;
        }

        freeproc(proc_ptr);
        closeproc(proc_tab);
    }
    return isSsh;
}

void sendFrameUdpSplit(const cv::Mat frame, ip::udp::socket* sock, const ip::udp::endpoint endpoint, int max_packet_size) 
{
    vector<uchar> data;
    imencode(".jpg", frame, data);
    int num_packets = (data.size() + max_packet_size - 1) / max_packet_size;

    for (int i = 0; i < num_packets; i++) 
    {
        vector<uchar> packet_data;
        int start = i * max_packet_size;
        int end = min((i + 1) * max_packet_size, (int)data.size());
        packet_data.assign(data.begin() + start, data.begin() + end);

        vector<uchar> header(8, 0);
        header[4] = (uchar)num_packets;
        header[5] = (uchar)(num_packets >> 8);
        header[6] = (uchar)i;
        header[7] = (uchar)(i >> 8);

        vector<uchar> packet(header.size() + packet_data.size());
        copy(header.begin(), header.end(), packet.begin());
        copy(packet_data.begin(), packet_data.end(), packet.begin() + header.size());

        boost::system::error_code error;
        sock->send_to(buffer(packet), endpoint, 0, error);
        if (error) 
        {
            std::cerr << "Error sending UDP packet: " << error.message() << std::endl;
            break;
        }
    }
}

void releaseVideoCapture(cv::VideoCapture* videoCapture)
{
    if (videoCapture != nullptr)
    {
        videoCapture->release();
        delete videoCapture;
        videoCapture = nullptr;
    }
}

void releaseVideoWriter(cv::VideoWriter* videoWriter)
{
    if (videoWriter != nullptr)
    {
        videoWriter->release();
        delete videoWriter;
        videoWriter = nullptr;
    }
}

void closeSocket(ip::udp::socket* sock)
{
    if (sock != nullptr)
    {
        sock->close();
        delete sock;
        sock = nullptr;
    }
}

void sysExitMessage()
{
    cout << "Press a key to close..." << endl;
    cin.ignore();
}

void parseArgs(int argc, char** argv, std::map<std::string, std::vector<std::string>>& args) 
{
    std::string currentOption;
    for (int i = 1; i < argc; ++i) 
    {
        std::string arg = argv[i];
        if (arg[0] == '-') 
        {
            int index = arg[1] == '-' ? 2 : 1;
            // New option detected
            currentOption = arg.substr(index);
            args[currentOption] = std::vector<std::string>();
        }
        else
        {
            // Argument for current option
            args[currentOption].push_back(arg);
        }
    }
}

int main(int argc, char** argv)
{   
    // Install a signal handler
    std::signal(SIGINT, signalHandler);

    std::map<std::string, std::vector<std::string>> args;
    parseArgs(argc, argv, args);

    if (args.count("h") || args.count("help"))
    {
        cout << "./main.out [-d] [-do] [-dd] [-db] [-i img1 img2] [-v vid] [-sd] [-srwd] [-sr] [-sm] [-udp] [-ip] [-port]" << endl << endl;
        cout << "-d    --display                         Display a window of the resulting frame" << endl;
        cout << "-do   --display_opt                     Enable all optional windows: Previous frame, Current frame, Mask, Result" << endl;
        cout << "-dd   --display_duration                Display the process duration to compute a frame" << endl;
        cout << "-db   --debug                           Enable all windows, display durayion, additionnal logs, pause on every frame" << endl;
        cout << "-i    --image                           Process on the two given images" << endl;
        cout << "-v    --video                           Process on the given video" << endl;
        cout << "-sd   --save_detection                  Save detected objects into a video file" << endl;
        cout << "-srwd --save_result_without_detection   Save the resulting frames without the rectangle of detection into a video file" << endl;
        cout << "-sr   --save_result                     Save the resulting frames into a video file" << endl;
        cout << "-sm   --save_mask                       Save the mask frames into a video file" << endl;
        cout << "-udp  --udp                             Enable the UDP stream (will read the `config.yaml` ip and port)" << endl;
        cout << "-ip   --ip                              Specify an ip address for UDP stream (udp automatically enabled)" << endl;
        cout << "-port --port                            Specify a port for UDP stream (udp automatically enabled)" << endl;
        help();
        return 0;
    }

    VideoCapture* cap(nullptr);
    VideoWriter* videoDetection(nullptr);
    VideoWriter* videoResultWithoutDetection(nullptr);
    VideoWriter* videoResult(nullptr);
    VideoWriter* videoMask(nullptr);

    ip::udp::socket* sock(nullptr);
    io_service ioService;
    ip::udp::endpoint remoteEndpoint;

    unsigned int k = 0;
    const string RTH_PATH = "./RTH";
    Option selectedOption = Option::None;
    double duration = 0.0;
    double durationAverage = 0.0;
    unsigned int durationAverageCount = 0;

    double fps = 0.0;
    unsigned int frameWidth = 0;
    unsigned int frameHeight = 0;
    unsigned int frameToTake = 0;
    unsigned int frameCount = 0;
    unsigned int frameWaitDelay = 0;

    Mat images[2];
    
    Mat frame;
    Mat previousFrame;

    Mat grayImage;

    Mat gaussianImage;
    Mat previousGaussianImage;

    Mat diffImage;

    bool debug = false;
    bool display = true;
    bool displayWindows = false;
    bool pause = false;
    bool readNextFrame = true;
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
    Mat tmpMask;
    Mat result;

    string windowPrevFrame = "Previous frame";
    string windowCurrFrame = "Current frame";
    string windowMask = "Mask";
    string windowResult = "Result";

    Config config;
    if (!config.read())
    {
        sysExitMessage();
        return -1;
    }

    if (args.count("d") || args.count("display"))
    {
        config.setDisplayOptionalWindows(true);
    }
    if (args.count("do") || args.count("display_opt"))
    {
        config.setDisplayOptionalWindows(true);
    }
    if (args.count("dd") || args.count("display_duration"))
    {
        config.setDisplayDuration(true);
    }
    if (args.count("db") || args.count("debug"))
    {
        config.setDebug(true);
    }
    if (args.count("sd") || args.count("save_detection"))
    {
        config.setSaveDetection(true);
    }
    if (args.count("srwd") || args.count("save_result_without_detection"))
    {
        config.setSaveResultWithoutDetection(true);
    }
    if (args.count("sr") || args.count("save_result"))
    {
        config.setSaveResult(true);
    }
    if (args.count("sm") || args.count("save_mask"))
    {
        config.setSaveMask(true);
    }
    if (args.count("udp"))
    {
        config.setUdpEnabled(true);
    }

    if (args.count("ip"))
    {
        config.setUdpEnabled(true);
        config.setUdpIp(args["ip"][0]);
    }

    if (args.count("port"))
    {
        config.setUdpEnabled(true);
        int port;
        try 
        {
            port = std::stoi(args["port"][0]);
        }
        catch (const std::invalid_argument& e) 
        {
            std::cerr << "Error: port is not a number. Given: " << args["port"][0] << std::endl;
            return 1;
        }
        catch (const std::out_of_range& e) 
        {
            std::cerr << "Error: the given port (" << args["port"][0] << ") is out of range for an integer" << std::endl;
            return 1;
        }
        config.setUdpPort(port);
    }

    debug = config.getDebug();
    display = config.getDisplay();
    displayWindows = config.getDisplayOptionalWindows();

    if (debug)
    {
        display = true;
        displayWindows = true;
    }

    if (isSshConnected(false))
    {
        debug = false;
        display = false;
        displayWindows = false;
        printf("Script launched via SSH. debug, display and display optional windows automatically disabled\n");
    }
    
    if (config.getUdpEnabled())
    {
        if (!isValidIp(config.getUdpIp()))
        {
            cerr << "The address " << config.getUdpIp() << " is invalid! Please give an address like X.X.X.X where X is in [0-255]" << endl;
            return 1;
        }

        if (!isValidPort(config.getUdpPort()))
        {
            cerr << "The port " << config.getUdpPort() << " is invalid! Please give a port from 1024 to 65535" << endl;
            return 1;
        }

        sock = new ip::udp::socket(ioService, ip::udp::endpoint(ip::udp::v4(), 0));
        remoteEndpoint = ip::udp::endpoint(ip::address::from_string(config.getUdpIp()), config.getUdpPort());
    }

    if (args.count("i") || args.count("image"))
    {
        string keyName = args.count("i") ? "i" : "image";
        if (args[keyName].size() != 2)
        {
            closeSocket(sock);
            cout << "Wrong usage of image option: -i img1 img2" << endl;
            sysExitMessage();
            return -1;
        }
        for (int i = 0; i < 2; i++)
        {
            Mat image = imread(args[keyName][i]);
            if ( !image.data )
            {
                closeSocket(sock);
                cout << "Could not read the image:" << args[keyName][i] << endl;
                sysExitMessage();
                return 0;
            }
            images[i] = image;
        }
        imageMode = true;
    }
    else if (args.count("v") || args.count("video"))
    {
        string keyName = args.count("v") ? "v" : "video";
        if (args[keyName].size() != 1)
        {
            closeSocket(sock);
            cout << "Wrong usage of video option: -v vid" << endl;
            sysExitMessage();
            return -1;
        }
        cap = new VideoCapture(args[keyName][0]);
        if (!cap->isOpened())
        {
            closeSocket(sock);
            cout << "Can't open video " << args[keyName][0] << endl;
            sysExitMessage();
            return -1;
        }
        videoMode = true;
    }
    else
    {
        printf("Trying to open /dev/video0 with CAP_V4L2\n");
        cap = new VideoCapture(0, cv::CAP_V4L2);

        if (!cap->isOpened()) 
        {
            closeSocket(sock);
            cout << "Can not read the device" << endl;
            if (isSshConnected(false))
            {
                cout << "With a SSH connection, please use sudo" << endl;   
            }
            sysExitMessage();
            return -1;
        }
    }

    if (cap != nullptr)
    {
        fps = cap->get(cv::CAP_PROP_FPS);
        frameWidth = cap->get(cv::CAP_PROP_FRAME_WIDTH);
        frameHeight = cap->get(cv::CAP_PROP_FRAME_HEIGHT);
        frameToTake = (int)(fps / 4);
        frameCount = frameToTake;
        frameWaitDelay = (int)(1000.0 / fps);
        cout << "fps: " << fps << endl;
        cout << "frame took every: " << frameToTake << endl;
        cout << "frame wait delay: " << frameWaitDelay << " ms" << endl;

        const char* outputVideosFolder = "output_videos";
        int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');

        if (config.getSaveDetection())
        {
            createDirIfNotExists(outputVideosFolder);
            const string fileName = concatenateFolderNameWithDate(outputVideosFolder, "_output_detection.avi");
            videoDetection = new VideoWriter(fileName, codec, frameToTake, Size(frameWidth, frameHeight));
        }
        if (config.getSaveResultWithoutDetection())
        {
            createDirIfNotExists(outputVideosFolder);
            const string fileName = concatenateFolderNameWithDate(outputVideosFolder, "_output_result_without_detection.avi");
            videoResultWithoutDetection = new VideoWriter(fileName, codec, frameToTake, Size(frameWidth, frameHeight));
        }
        if (config.getSaveResult())
        {
            createDirIfNotExists(outputVideosFolder);
            const string fileName = concatenateFolderNameWithDate(outputVideosFolder, "_output_result.avi");
            videoResult = new VideoWriter(fileName, codec, frameToTake, Size(frameWidth, frameHeight));
        }
        if (config.getSaveMask())
        {
            createDirIfNotExists(outputVideosFolder);
            const string fileName = concatenateFolderNameWithDate(outputVideosFolder, "_output_mask.avi");
            videoMask = new VideoWriter(fileName, codec, frameToTake, Size(frameWidth, frameHeight));
        }
    }

    if (fileExists(RTH_PATH) && !removeFile(RTH_PATH))
    {
        cout << "Cannot remove " << RTH_PATH << " file." << endl;
        releaseVideoCapture(cap);
        releaseVideoWriter(videoDetection);
        releaseVideoWriter(videoResultWithoutDetection);
        releaseVideoWriter(videoResult);
        releaseVideoWriter(videoMask);
        closeSocket(sock);
        sysExitMessage();
        return -1;
    }

    cout << endl << "config:" << endl;
    cout << "debug: " << (config.getDebug() ? "true" : "false") << endl;
    cout << "display: " << (config.getDisplay() ? "true" : "false") << endl;
    cout << "display optional windows: " << (config.getDisplayOptionalWindows() ? "true" : "false") << endl;
    cout << "display duration: " << (config.getDisplayDuration() ? "true" : "false") << endl;
    cout << "save detection: " << (config.getSaveDetection() ? "true" : "false") << endl;
    cout << "save result without detection: " << (config.getSaveResultWithoutDetection() ? "true" : "false") << endl;
    cout << "save result: " << (config.getSaveResult() ? "true" : "false") << endl;
    cout << "save mask: " << (config.getSaveMask() ? "true" : "false") << endl;
    cout << "udp: " << (config.getUdpEnabled() ? "true" : "false") << endl;
    if (config.getUdpEnabled())
    {
        cout << "  to:" << config.getUdpIp() << ":" << to_string(config.getUdpPort()) << endl;
    }
    config.display('\n', '\0');

    cout << "press [H] to print the help" << endl << endl;

    if (display || displayWindows)
    {
        if (displayWindows)
        {
            namedWindow(windowPrevFrame);
            namedWindow(windowCurrFrame);
            namedWindow(windowMask);
            namedWindow(windowResult);
        }
        else if (display)
        {
            namedWindow(windowResult);
        }
        waitKey(500);
    }

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
                            if (config.getGaussianBlur() == 0)
                            {
                                // convert a gray frame direclty into the gaussian frame
                                cvtColor(frame, gaussianImage, COLOR_BGR2GRAY);
                            }
                            else
                            {
                                // convert frame into gray and then apply a gauss filter
                                cvtColor(frame, grayImage, COLOR_BGR2GRAY);
                                cv::GaussianBlur(grayImage, gaussianImage, config.getGaussianKernel(), 0);
                            }

                            absdiff(previousGaussianImage, gaussianImage, diffImage);
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
                                if (config.getSaveDetection())
                                {
                                    videoDetection->write(result);
                                }
                            }

                            if (display || displayWindows)
                            {
                                if (displayWindows)
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
                                else if (display)
                                {
                                    imshow(windowResult, result);
                                }
                            }

                            if (config.getUdpEnabled())
                            {
                                sendFrameUdpSplit(result, sock, remoteEndpoint);
                            }

                            if (config.getSaveMask())
                            {
                                cvtColor(mask, tmpMask, COLOR_GRAY2BGR);
                                videoMask->write(tmpMask);
                            }
                            if (config.getSaveResultWithoutDetection())
                            {
                                videoResultWithoutDetection->write(frame);
                            }
                            if (config.getSaveResult())
                            {
                                videoResult->write(result);
                            }

                            if (somethingDetected)
                            {
                                if (!fileExists(RTH_PATH))
                                {
                                    // create RTH file and close it
                                    ofstream file(RTH_PATH.c_str());
                                    file.close();
                                }
                            }
                            else
                            {
                                if (!configChanged)
                                {
                                    frame.copyTo(previousFrame);
                                    gaussianImage.copyTo(previousGaussianImage);
                                }
                            }

                            if (clearDetection)
                            {
                                clearDetection = false;
                                cout << "detection cleared" << endl;
                                frame.copyTo(previousFrame);
                                gaussianImage.copyTo(previousGaussianImage);
                            }

                            duration = getTimeDiff(duration);
                            durationAverage = ((durationAverage * durationAverageCount) + duration) / (double)(durationAverageCount + 1);
                            durationAverageCount++;

                            if (config.getDisplayDuration())
                            {
                                cout << round3(duration) << " ms" << endl;
                            }
                        }
                        else
                        {
                            cout << "frames haven't the same dimensions or depth!" << endl;
                            frame.copyTo(previousFrame);
                            gaussianImage.copyTo(previousGaussianImage);
                        }

                        if (imageMode)
                        {
                            k = myWaitKey(0);
                        }
                    }
                    else
                    {
                        frame.copyTo(previousFrame);
                        cvtColor(frame, previousGaussianImage, COLOR_BGR2GRAY);
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
            ((display || displayWindows) && cv::getWindowProperty(windowResult, WND_PROP_AUTOSIZE) == -1))  // window is closed
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

    cout << "\nAverage duration: " << round3(durationAverage) << " ms (" << durationAverageCount << " samples)" << endl;

    releaseVideoCapture(cap);
    releaseVideoWriter(videoDetection);
    releaseVideoWriter(videoResultWithoutDetection);
    releaseVideoWriter(videoResult);
    releaseVideoWriter(videoMask);
    closeSocket(sock);

    if (display || displayWindows)
    {
        cv::destroyAllWindows();
    }

    removeFile(RTH_PATH);

    return 0;
}
