#ifndef DEF_CONFIG
#define DEF_CONFIG

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include <yaml-cpp/yaml.h>
#include <sys/stat.h>
#include <string>
#include <regex>


bool isValidIp(const std::string& ip);
bool isValidPort(const unsigned int port);


class KeyNotFoundException : public std::exception
{
	public:
	    KeyNotFoundException(const std::string& keyName) : 
	    	m_keyName(keyName)
	    {
	    }

	    const char* what() const noexcept override
	    {
	        std::string message = "Key not found: " + m_keyName;
	        return message.c_str();
	    }

	    std::string getKeyName() const
	    {
	    	return m_keyName;
	    }

	private:
	    std::string m_keyName;
};


class Config 
{
	public:

		// constructor, copy constructor, desctructor

		Config();
		Config(const Config& config);
		~Config();

		// accessors

		bool getDebug() const;
		void setDebug(const bool value);
		void setDisplayResult(const bool value);
		bool getDisplayResult() const;
		void setDisplayMask(const bool value);
		bool getDisplayMask() const;
		void setDisplayCurrentPrevious(const bool value);
		bool getDisplayCurrentPrevious() const;
		bool getDisplayAll() const;
		void setDisplayAll(const bool value);
		bool getDisplayDuration() const;
		void setDisplayDuration(const bool value);
		void setRotate(const bool value);
		bool getRotate() const;
		bool getSaveDetection() const;
		void setSaveDetection(const bool value);
		bool getSaveResultWithoutDetection() const;
		void setSaveResultWithoutDetection(const bool value);
		bool getSaveResult() const;
		void setSaveResult(const bool value);
		bool getSaveMask() const;
		void setSaveMask(const bool value);
		unsigned int getDetectionArea() const;
		unsigned int getGaussianBlur() const;
		cv::Size getGaussianKernel() const;
		unsigned int getThreshold() const;
		unsigned int getKernelSize() const;
		cv::Mat getKernel() const;
		cv::Scalar getRectangleColor() const;
		void setRectangleColor(const unsigned int red, const unsigned int green, const unsigned int blue);
		void setRectangleColor(const cv::Scalar& colorBGR);
		bool getUdpEnabled() const;
		void setUdpEnabled(const bool value);
		std::string getUdpIp() const;
		void setUdpIp(const std::string& value);
		unsigned int getUdpPort() const;
		void setUdpPort(const unsigned int value);
		unsigned int getUdpPort2() const;
		void setUdpPort2(const unsigned int value);
		bool getUdpAutoChangeIp() const;
		void setUdpAutoChangeIp(const bool value);

		// methods

		void inverseDisplayDuration();
		bool decreaseDetectionArea(const unsigned int decrement=100);
		bool increaseDetectionArea(const unsigned int increment=100);
		bool decreaseGaussianBlur(unsigned int decrement=2);
		bool increaseGaussianBlur(unsigned int increment=2);
		bool decreaseThreshold(const unsigned int decrement=1);
		bool increaseThreshold(const unsigned int increment=1);
		bool decreaseKernelSize(const unsigned int decrement=1);
		bool increaseKernelSize(const unsigned int increment=1);
		void display(const char sep='\t', const char start='\n', const char end='\n') const;
		Config copy() const;
		void setFrom(const Config& config);
		void save() const;
		bool read();
		void reset();

	private:

		// accessors

		void setGaussianBlur(const unsigned int value);
		void setThreshold(const unsigned int value);
		void setKernelSize(const unsigned int value);
		void setDetectionArea(const unsigned int value);
		void setRectColor(const unsigned int red, const unsigned int green, const unsigned int blue);
		
		// methods

		void computeGaussianKernel();
		void computeKernel();
		template<typename T>
		T getValueFromYamlNode(const YAML::Node& node, const std::string& key);

	private:
    	bool m_debug;
    	bool m_displayResult;
    	bool m_displayMask;
    	bool m_displayCurrentPrevious;
    	bool m_displayAll;
    	bool m_displayDuration;
    	bool m_rotate;
    	bool m_saveDetection;
    	bool m_saveResultWithoutDetection;
    	bool m_saveResult;
    	bool m_saveMask;
    	unsigned int m_detectionArea;
    	unsigned int m_gaussianBlur;
    	cv::Size m_gaussianKernel;
    	unsigned int m_threshold;
    	unsigned int m_kernelSize;
    	cv::Mat m_kernel;
    	cv::Scalar m_rectangleColor;
    	bool m_udpEnabled;
    	std::string m_udpIp;
    	unsigned int m_udpPort;
    	unsigned int m_udpPort2;
    	bool m_udpAutoChangeIp;
};

#endif