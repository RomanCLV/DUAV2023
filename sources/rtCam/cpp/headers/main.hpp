#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <csignal>
#include <cmath>
#include <ctime>
#include <chrono>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <map>
#include <vector>
#include <proc/readproc.h>
#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

#include "config.hpp"

enum Option
{
	None = 0,
	GaussianBlur,
	Threshold,
	KernelSize,
	DetectionArea
};

void signalHandler(int signal);
inline void help();
void displayFullConfig(
    const Config& config,
    const bool debug,
    const bool displayResult,
    const bool displayMask,
    const bool displayCurrentPrevious,
    const bool displayDuration, 
    const boost::asio::ip::udp::endpoint& remoteEndpoint, 
    const boost::asio::ip::udp::endpoint& remoteEndpoint2);
inline bool fileExists(const std::string& name);
inline bool removeFile(const std::string& name);
void createDirIfNotExists(const char* folderName);
inline double getTime();
inline double getTimeDiff(const double timeStart);
inline double round3(const double value);
inline unsigned int myWaitKey(unsigned int millis);
std::string concatenateFolderNameWithDate(const char* folderName, const char* endName);
bool isSshConnected(const bool displayProccesInfo = true);
cv::Mat combine2Images(const cv::Mat& img1, const cv::Mat& img2);
void combine2Images2(const cv::Mat& img1, const cv::Mat& img2, cv::Mat& dst);
void sendFrameUdpSplit(
	const cv::Mat frame,
	boost::asio::ip::udp::socket* sock, 
	const boost::asio::ip::udp::endpoint endpoint, 
	int max_packet_size = 65507);
unsigned int listenKeyUdp(boost::asio::ip::udp::socket* sock);
void releaseVideoCapture(cv::VideoCapture* videoCapture);
void releaseVideoWriter(cv::VideoWriter* videoWriter);
void closeSocket(boost::asio::ip::udp::socket* sock);
void sysExitMessage();
void parseArgs(int argc, char** argv, std::map<std::string, std::vector<std::string>>& args);
bool validArg(std::map<std::string, std::vector<std::string>>& args, std::string keyName, std::string keyFullName, int requiredValues);
int main(int argc, char** argv);
