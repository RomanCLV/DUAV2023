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
#include <opencv2/opencv.hpp>

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
inline bool fileExists(const std::string& name);
inline bool removeFile(const std::string& name);
void createDirIfNotExists(const char* folderName);
inline double getTime();
inline double getTimeDiff(const double timeStart);
inline double round3(double value);
inline unsigned int myWaitKey(unsigned int millis);
std::string concatenateFolderNameWithDate(const char* folderName, const char* endName);
void sysExitMessage();
void parseArgs(int argc, char** argv, std::map<std::string, std::vector<std::string>>& args);
int main(int argc, char** argv);
