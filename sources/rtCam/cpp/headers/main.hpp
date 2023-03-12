#include <stdio.h>
#include <sys/stat.h>
#include <csignal>
#include <cmath>
#include <iostream>
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
inline double getTime();
inline double getTimeDiff(const double time_ori);
inline unsigned int myWaitKey(unsigned int millis);
void parseArgs(int argc, char** argv, std::map<std::string, std::vector<std::string>>& args);
int main(int argc, char** argv);
