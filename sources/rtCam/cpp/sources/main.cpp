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
    cout << endl;
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
    cout << "    New config             N" << endl;
    cout << "    Reset config           R" << endl;
    cout << "    Save config            S" << endl;
    cout << endl;
    cout << " Select an option:" << endl;
    cout << endl;
    cout << "    Gaussian blur          G" << endl;
    cout << "    Threshold              T" << endl;
    cout << "    Kernel size            K" << endl;
    cout << "    Detection area         A" << endl;
    cout << endl;
    cout << "----------------------------------" << endl;
}

void displayFullConfig(
    const Config& config,
    const bool debug,
    const bool displayResult,
    const bool displayMask,
    const bool displayCurrentPrevious,
    const bool displayDuration, 
    const ip::udp::endpoint& remoteEndpoint, 
    const ip::udp::endpoint& remoteEndpoint2)
{
    cout << endl << "config:" << endl;
    cout << "debug: " << (debug ? "true" : "false") << endl;
    cout << "display result: " << (displayResult ? "true" : "false") << endl;
    cout << "display mask: " << (displayMask ? "true" : "false") << endl;
    cout << "display current previous: " << (displayCurrentPrevious ? "true" : "false") << endl;
    cout << "display duration: " << (displayDuration ? "true" : "false") << endl;
    cout << "save detection: " << (config.getSaveDetection() ? "true" : "false") << endl;
    cout << "save result without detection: " << (config.getSaveResultWithoutDetection() ? "true" : "false") << endl;
    cout << "save result: " << (config.getSaveResult() ? "true" : "false") << endl;
    cout << "save mask: " << (config.getSaveMask() ? "true" : "false") << endl;
    cout << "udp: " << (config.getUdpEnabled() ? "true" : "false") << endl;
    if (config.getUdpEnabled())
    {
        cout << "  to  : " << remoteEndpoint.address().to_string() << ":" << to_string(remoteEndpoint.port()) << endl;
        cout << "  from: " << remoteEndpoint2.address().to_string() << ":" << to_string(remoteEndpoint2.port()) << endl;
        cout << "  auto change ip: " << (config.getUdpAutoChangeIp() ? "true" : "false") << endl;
    }
    config.display('\n', '\0');
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

void combine2Images(const cv::Mat& img1, const cv::Mat& img2, cv::Mat& dst)
{
    if (
        dst.empty() ||
        dst.size() != img1.size() ||
        dst.type() != img1.type())
    {
        dst.create(img1.size(), img1.type());
    }

    for (int col = 0; col < dst.cols; col += 2) 
    {
        img1.col(col).copyTo(dst.col(col / 2)); 
        img2.col(col).copyTo(dst.col((dst.cols + col) / 2));
    }
}

void combine4Images(const Mat& img1, const Mat& img2, const Mat& img3, const Mat& img4, Mat& dst) 
{
    if (dst.empty() || dst.size() != img1.size() || dst.type() != img1.type())
    {
        dst.create(img1.size(), img1.type());
    }

    Mat resized1, resized2, resized3, resized4;
    Size newSize(img1.cols / 2, img1.rows / 2);
    resize(img1, resized1, newSize);
    resize(img2, resized2, newSize);
    resize(img3, resized3, newSize);
    resize(img4, resized4, newSize);

    resized1.copyTo(dst(Rect(0, 0, newSize.width, newSize.height)));
    resized2.copyTo(dst(Rect(newSize.width, 0, newSize.width, newSize.height)));
    resized3.copyTo(dst(Rect(0, newSize.height, newSize.width, newSize.height)));
    resized4.copyTo(dst(Rect(newSize.width, newSize.height, newSize.width, newSize.height)));
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

unsigned int listenKeyUdp(boost::asio::ip::udp::socket* sock, int timeout_ms)
{
    unsigned int value;
    
    uint32_t received_number;
    ip::udp::endpoint remote_endpoint;

    io_context& io_service = static_cast<io_context&>((sock->get_executor()).context());

    deadline_timer timer(io_service, boost::posix_time::milliseconds(timeout_ms));
    timer.async_wait([&](const boost::system::error_code& error) {
        if (!error) 
        {
            sock->cancel();
        }
    });

    sock->async_receive_from(buffer(reinterpret_cast<char*>(&received_number), sizeof(received_number)), remote_endpoint, [&](const boost::system::error_code& error, size_t) {
        if (!error) 
        {
            timer.cancel();
            uint32_t host_number = ntohl(received_number);
            if (host_number == 2424832)       // Left arrow
            {
                value = 65361;
            }
            else if (host_number == 2555904)  // Rigth arrow
            {
                value = 65363;
            }
            else
            {
                value = (unsigned int)host_number;
            }
        }
        else if (error == boost::asio::error::operation_aborted)
        {
            value = 0;
        }
        else
        {
            cout << "socket error: " << error << endl;
            value = 0;
        }
    });    

    try 
    {
        io_service.run();
        io_service.restart(); // reset ou restart ?
    }
    catch (exception& e)
    {
        cerr << "socket error: " << e.what() << endl;
    }
    return value;
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

bool validArg(std::map<std::string, std::vector<std::string>>& args, std::string keyName, std::string keyFullName, int requiredValues)
{
    string key;
    if (args.count(keyName))
    {
        key = keyName;
    }
    else if (args.count(keyFullName))
    {
        key = keyFullName;
    }
    else
    {
        throw invalid_argument("args doesn't contain key neither " + keyName + " nor " + keyFullName);
    }
    return args[key].size() == requiredValues;
}

int main(int argc, char** argv)
{   
    // Install a signal handler
    std::signal(SIGINT, signalHandler);

    std::map<std::string, std::vector<std::string>> args;
    parseArgs(argc, argv, args);

    if (args.count("h") || args.count("help"))
    {
        cout << "./main.out [-i img1 img2] [-v vid] [-dr] [-dm] [-dcp] [-da] [-dd] [-dbg] [-rot] [-sd] [-srwd] [-sr] [-sm] [-udp] [-ip] [-port] [-port2] [-aci]" << endl << endl;
        cout << "-i     --image                           Process on the two given images" << endl;
        cout << "-v     --video                           Process on the given video" << endl;
        cout << "-dr    --display_result                  Display a window of the resulting frame" << endl;
        cout << "-dm    --display_mask                    Display a window of the mask frame" << endl;
        cout << "-dcp   --display_current_previous        Display two windows of the current and the previous compared frames" << endl;
        cout << "-da    --display_all                     Enable all windows: Previous frame, Current frame, Mask, Result" << endl;
        cout << "-dd    --display_duration                Display the process duration to compute a frame" << endl;
        cout << "-dbg   --debug                           Enable all windows, display durayion, additionnal logs, pause on every frame" << endl;
        cout << "-rot   --rotate                          Rotate the frame of 180°" << endl;
        cout << "-sd    --save_detection                  Save detected objects into a video file" << endl;
        cout << "-srwd  --save_result_without_detection   Save the resulting frames without the rectangle of detection into a video file" << endl;
        cout << "-sr    --save_result                     Save the resulting frames into a video file" << endl;
        cout << "-sm    --save_mask                       Save the mask frames into a video file" << endl;
        cout << "-udp   --udp                             Enable the UDP stream (will read the `config.yaml` ip and port)" << endl;
        cout << "-ip    --ip                              Specify an ip address for UDP stream (udp automatically enabled)" << endl;
        cout << "-port  --port                            Specify a port for UDP stream (udp automatically enabled)" << endl;
        cout << "-port2 --port2                           Specify a port to listen the pressed key by UDP (udp automatically enabled)" << endl;
        cout << "-aci   --auto_change_ip                  Allow to set the ip address to 0.0.0.0 if the reader socket can't be bind to the given ip" << endl;
        help();
        return 0;
    }

    VideoCapture* cap(nullptr);
    VideoWriter* videoDetection(nullptr);
    VideoWriter* videoResultWithoutDetection(nullptr);
    VideoWriter* videoResult(nullptr);
    VideoWriter* videoMask(nullptr);

    io_service ioService;
    io_service ioService2;
    ip::udp::endpoint remoteEndpoint;
    ip::udp::endpoint remoteEndpoint2;
    ip::udp::socket* sock(nullptr);
    ip::udp::socket* sock2(nullptr);

    bool isSsh = isSshConnected(false);

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
    bool displayResult = false;
    bool displayMask = false;
    bool displayCurrentPrevious = false;
    bool displayAll = false;
    bool displayEnalbed = false;
    unsigned char displayedWindowsCount = 0;
    bool displayDuration = false;
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
    Mat result;
    Mat combinedImage;
    Mat blackFilledImage;

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

    if (args.count("dr") || args.count("display_result"))
    {
        if (validArg(args, "dr", "display_result", 0))
        {
            config.setDisplayResult(true);
        }
        else
        {
            cout << "Wrong usage of display result option: -dr | --display_result" << endl;
            sysExitMessage();
            return 1;
        }
    }
    if (args.count("dm") || args.count("display_mask"))
    {
        if (validArg(args, "dm", "display_mask", 0))
        {
            config.setDisplayMask(true);
        }
        else
        {
            cout << "Wrong usage of display mask option: -dm | --display_mask" << endl;
            sysExitMessage();
            return 1;
        }
    }
    if (args.count("dcp") || args.count("display_current_previous"))
    {
        if (validArg(args, "dcp", "display_current_previous", 0))
        {
            config.setDisplayCurrentPrevious(true);
        }
        else
        {
            cout << "Wrong usage of display current previous option: -dcp | --display_current_previous" << endl;
            sysExitMessage();
            return 1;
        }
    }
    if (args.count("da") || args.count("display_all"))
    {
        if (validArg(args, "da", "display_all", 0))
        {
            config.setDisplayAll(true);
        }
        else
        {
            cout << "Wrong usage of display all option: -da | --display_all" << endl;
            sysExitMessage();
            return 1;
        }
    }
    if (args.count("dd") || args.count("display_duration"))
    {
        if (validArg(args, "dd", "display_duration", 0))
        {
            config.setDisplayDuration(true);
        }
        else
        {
            cout << "Wrong usage of display duration option: -dd | --display_duration" << endl;
            sysExitMessage();
            return 1;
        }
    }
    if (args.count("dbg") || args.count("debug"))
    {
        if (validArg(args, "dbg", "debug", 0))
        {
            config.setDebug(true);
        }
        else
        {
            cout << "Wrong usage of debug option: -dbg | --debug" << endl;
            sysExitMessage();
            return 1;
        }
    }
    if (args.count("rot") || args.count("rotate"))
    {
        if (validArg(args, "rot", "rotate", 0))
        {
            config.setRotate(true);
        }
        else
        {
            cout << "Wrong usage of rotate option: -rot | --rotate" << endl;
            sysExitMessage();
            return 1;
        }
    }
    if (args.count("sd") || args.count("save_detection"))
    {
        if (validArg(args, "sd", "save_detection", 0))
        {
            config.setSaveDetection(true);
        }
        else
        {
            cout << "Wrong usage of save detection option: -sd | --save_detection" << endl;
            sysExitMessage();
            return 1;
        }
    }
    if (args.count("srwd") || args.count("save_result_without_detection"))
    {
        if (validArg(args, "srwd", "save_result_without_detection", 0))
        {
            config.setSaveResultWithoutDetection(true);
        }
        else
        {
            cout << "Wrong usage of save result without detection option: -srwd | --save_result_without_detection" << endl;
            sysExitMessage();
            return 1;
        }
    }
    if (args.count("sr") || args.count("save_result"))
    {
        if (validArg(args, "sr", "save_result", 0))
        {
            config.setSaveResult(true);
        }
        else
        {
            cout << "Wrong usage of save result option: -sr | --save_result" << endl;
            sysExitMessage();
            return 1;
        }
    }
    if (args.count("sm") || args.count("save_mask"))
    {
        if (validArg(args, "sm", "save_mask", 0))
        {
            config.setSaveMask(true);
        }
        else
        {
            cout << "Wrong usage of save mask option: -sm | --save_mask" << endl;
            sysExitMessage();
            return 1;
        }
    }
    if (args.count("udp"))
    {
        if (validArg(args, "udp", "udp", 0))
        {
            config.setUdpEnabled(true);
        }
        else
        {
            cout << "Wrong usage of udp option: -udp" << endl;
            sysExitMessage();
            return 1;
        }
    }
    if (args.count("ip"))
    {
        if (validArg(args, "ip", "ip", 1))
        {
            config.setUdpEnabled(true);
            config.setUdpIp(args["ip"][0]);
        }
        else
        {
            cout << "Wrong usage of ip option: -ip X.X.X.X" << endl;
            sysExitMessage();
            return 1;
        }
    }
    if (args.count("port"))
    {
        if (validArg(args, "port", "port", 1))
        {
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
            config.setUdpEnabled(true);
            config.setUdpPort(port);
        }
        else
        {
            cout << "Wrong usage of port option: -port port" << endl;
            sysExitMessage();
            return 1;
        }
    }
    if (args.count("port2"))
    {
        if (validArg(args, "port2", "port2", 1))
        {
            int port;
            try 
            {
                port = std::stoi(args["port2"][0]);
            }
            catch (const std::invalid_argument& e) 
            {
                std::cerr << "Error: port2 is not a number. Given: " << args["port2"][0] << std::endl;
                return 1;
            }
            catch (const std::out_of_range& e) 
            {
                std::cerr << "Error: the given port2 (" << args["port2"][0] << ") is out of range for an integer" << std::endl;
                return 1;
            }
            config.setUdpEnabled(true);
            config.setUdpPort2(port);
        }
        else
        {
            cout << "Wrong usage of port2 option: -port2 port2" << endl;
            sysExitMessage();
            return 1;
        }
    }
    if (args.count("aci") || args.count("auto_change_ip"))
    {
        if (validArg(args, "aci", "auto_change_ip", 0))
        {
            config.setUdpAutoChangeIp(true);
        }
        else
        {
            cout << "Wrong usage of auto change ip ption: -aci | --auto_change_ip" << endl;
            sysExitMessage();
            return 1;
        }
    }

    debug = config.getDebug();
    displayResult = config.getDisplayResult();
    displayMask = config.getDisplayMask();
    displayCurrentPrevious = config.getDisplayCurrentPrevious();
    displayAll = config.getDisplayAll();
    displayDuration = config.getDisplayDuration();

    if (debug)
    {
        displayAll = true;
        displayDuration = true;
    }
    if (displayAll)
    {
        displayResult = true;
        displayMask = true;
        displayCurrentPrevious = true;
    }

    if (displayResult)
    {
        displayedWindowsCount++;
    }
    if (displayMask)
    {
        displayedWindowsCount++;
    }
    if (displayCurrentPrevious)
    {
        displayedWindowsCount += 2;
    }

    if (isSsh)
    {
        displayResult = false;
        displayMask = false;
        displayCurrentPrevious = false;
        displayAll = false;
        cout << "Script launched via SSH. display windows automatically disabled" << endl;
    }
    
    displayEnalbed = displayResult || displayMask || displayCurrentPrevious;
    
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

        if (!isValidPort(config.getUdpPort2()))
        {
            cerr << "The port2 " << config.getUdpPort2() << " is invalid! Please give a port from 1024 to 65535" << endl;
            return 1;
        }

        if (config.getUdpPort() == config.getUdpPort2())
        {
            cerr << "The listened port (" << config.getUdpPort() << ") and the writed port (" << config.getUdpPort2() << ") must be different" << endl;
            return 1;
        }

        cout << "Opening a socket to " << config.getUdpIp() << ":" << config.getUdpPort() << endl;
        sock = new ip::udp::socket(ioService, ip::udp::endpoint(ip::udp::v4(), 0));
        remoteEndpoint = ip::udp::endpoint(ip::address::from_string(config.getUdpIp()), config.getUdpPort());

        cout << "Opening a socket to " << config.getUdpIp() << ":" << config.getUdpPort2() << endl;
        try
        {
            remoteEndpoint2 = ip::udp::endpoint(ip::address_v4::from_string(config.getUdpIp()), config.getUdpPort2());
            sock2 = new ip::udp::socket(ioService2, remoteEndpoint2);
        }
        catch (const boost::system::system_error& e)
        {
            if (e.code() == boost::asio::error::address_in_use ||
                typeid(e) == typeid(boost::wrapexcept<boost::system::system_error>)) 
            {
                cout << "socket error: " << e.what() << endl;
                if (config.getUdpAutoChangeIp())
                {
                    cout << "Opening a socket to 0.0.0.0:" << config.getUdpPort2() << endl;
                    remoteEndpoint2 = ip::udp::endpoint(ip::address_v4::from_string("0.0.0.0"), config.getUdpPort2());
                    sock2 = new ip::udp::socket(ioService2, remoteEndpoint2);
                }
            } 
            else 
            {
                closeSocket(sock);
                throw;
            }
        }
    }

    if (args.count("i") || args.count("image"))
    {
        if (validArg(args, "i", "image", 2))
        {
            string keyName = args.count("i") ? "i" : "image";
            for (int i = 0; i < 2; i++)
            {
                images[i] = imread(args[keyName][i]);
                if ( !images[i].data )
                {
                    closeSocket(sock);
                    closeSocket(sock2);
                    cout << "Could not read the image:" << args[keyName][i] << endl;
                    sysExitMessage();
                    return 0;
                }
            }
        }
        else
        {
            closeSocket(sock);
            closeSocket(sock2);
            cout << "Wrong usage of image option: -i img1 img2" << endl;
            sysExitMessage();
            return -1;
        }
        imageMode = true;
    }
    else if (args.count("v") || args.count("video"))
    {
        string keyName = args.count("v") ? "v" : "video";
        if (validArg(args, "v", "video", 1))
        {
            cap = new VideoCapture(args[keyName][0]);
            if (!cap->isOpened())
            {
                closeSocket(sock);
                closeSocket(sock2);
                cout << "Can't open video " << args[keyName][0] << endl;
                sysExitMessage();
                return -1;
            }
        }
        else
        {
            closeSocket(sock);
            closeSocket(sock2);
            cout << "Wrong usage of video option: -v vid" << endl;
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
            closeSocket(sock2);
            cout << "Can not read the device" << endl;
            if (isSsh)
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
        closeSocket(sock2);
        sysExitMessage();
        return -1;
    }

    Config configCopy = Config(config);

    displayFullConfig(config, debug, displayResult, displayMask, displayCurrentPrevious, displayDuration, remoteEndpoint, remoteEndpoint2);

    cout << "press [H] to print the help" << endl << endl;

    if (displayEnalbed)
    {
        if (displayResult)
        {
            namedWindow(windowResult);
        }
        if (displayMask)
        {
            namedWindow(windowMask);
        }
        if (displayCurrentPrevious)
        {
            namedWindow(windowPrevFrame);
            namedWindow(windowCurrFrame);
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
                        if (config.getRotate())
                        {
                            rotate(images[0], previousFrame, ROTATE_180);
                        }
                        else
                        {
                            previousFrame = images[0];
                        }
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
                            if (config.getRotate())
                            {
                                rotate(frame, frame, ROTATE_180);
                            }

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
                            cvtColor(mask, mask, COLOR_GRAY2BGR);

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

                            if (displayEnalbed)
                            {
                                if (displayResult)
                                {
                                    imshow(windowResult, result);
                                }
                                if (displayMask)
                                {
                                    imshow(windowMask, mask);
                                }
                                if (displayCurrentPrevious)
                                {
                                    imshow(windowPrevFrame, previousFrame);
                                    imshow(windowCurrFrame, frame);
                                }
                            }

                            if (debug)
                            {
                                if (displayEnalbed)
                                {
                                    k = myWaitKey(0);
                                }
                                else
                                {
                                    if (config.getUdpEnabled())
                                    {
                                        while (k == 0)
                                        {
                                            k = listenKeyUdp(sock2, 5);
                                        }
                                    }
                                }
                            }

                            if (config.getUdpEnabled())
                            {
                                switch (displayedWindowsCount)
                                {
                                    case 1:
                                        if (displayResult)
                                        {
                                            sendFrameUdpSplit(result, sock, remoteEndpoint);
                                        }
                                        else
                                        {
                                            sendFrameUdpSplit(mask, sock, remoteEndpoint);
                                        }
                                        break;

                                    case 2:
                                        if (displayResult)
                                        {
                                            combine2Images(result, mask, combinedImage);
                                            sendFrameUdpSplit(combinedImage, sock, remoteEndpoint);
                                        }
                                        else
                                        {
                                            combine2Images(previousFrame, frame, combinedImage);
                                            sendFrameUdpSplit(combinedImage, sock, remoteEndpoint);
                                        }
                                        break;

                                    case 3:
                                        if (displayResult)
                                        {
                                            combine4Images(result, blackFilledImage, previousFrame, frame, combinedImage);
                                            sendFrameUdpSplit(combinedImage, sock, remoteEndpoint);
                                        }
                                        else
                                        {
                                            combine4Images(blackFilledImage, mask, previousFrame, frame, combinedImage);
                                            sendFrameUdpSplit(combinedImage, sock, remoteEndpoint);
                                        }
                                        break;
                                    case 4:
                                        combine4Images(result, mask, previousFrame, frame, combinedImage);
                                        sendFrameUdpSplit(combinedImage, sock, remoteEndpoint);
                                        break;
                                }
                            }

                            if (config.getSaveMask())
                            {
                                videoMask->write(mask);
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

                            if (displayDuration)
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
                        if (blackFilledImage.empty())
                        {
                            blackFilledImage.create(frame.size(), frame.type());
                            blackFilledImage.setTo(Scalar::all(0));
                        }
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

        if (config.getUdpEnabled())
        {
            unsigned int k1 = listenKeyUdp(sock2, 5);
            if (k1 != 0)
            {
                k = k1;
            }
        }

        if (hasToBreak || k == 27)      // Ctrl+C || ESC
        {
            break;
        }

        if (
            displayEnalbed &&
            (
            (displayResult && cv::getWindowProperty(windowResult, WND_PROP_AUTOSIZE) == -1) || 
            (displayMask   && cv::getWindowProperty(windowMask, WND_PROP_AUTOSIZE)   == -1) || 
            (displayCurrentPrevious && 
                    (cv::getWindowProperty(windowCurrFrame, WND_PROP_AUTOSIZE) == -1 || 
                     cv::getWindowProperty(windowPrevFrame, WND_PROP_AUTOSIZE) == -1))
            ))
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
            displayDuration = config.getDisplayDuration();
            readNextFrame = !(debug || imageMode);
        }
        
        else if (k == 72 || k == 104)   // H | h
        {
            help();
            readNextFrame = !(debug || imageMode);
        }
        
        else if (k == 82 || k == 114)   // R | r
        {
            cout << "reset config" << endl;
            config.setFrom(configCopy);
            config.display();
            configChanged = true;
            readNextFrame = !(debug || imageMode);
        }

        else if (k == 78 || k == 110)   // N | n
        {
            cout << "new config" << endl;
            config.reset();
            displayFullConfig(config, debug, displayResult, displayMask, displayCurrentPrevious, displayDuration, remoteEndpoint, remoteEndpoint2);
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
    closeSocket(sock2);

    if (displayEnalbed)
    {
        cv::destroyAllWindows();
    }

    removeFile(RTH_PATH);

    return 0;
}
