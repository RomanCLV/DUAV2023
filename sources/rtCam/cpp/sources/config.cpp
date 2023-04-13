#include "config.hpp"

using namespace cv;
using namespace std;

Config::Config() :
	m_debug(false),
    m_displayOptionalWindows(false),
    m_displayDuration(false),
    m_saveDetection(false),
    m_saveResultWithoutDetection(false),
    m_saveResult(false),
    m_saveMask(false),
    m_detectionArea(500),
    m_gaussianBlur(0),
    m_threshold(70),
    m_kernelSize(6),
    m_rectangleColor(0, 255, 0) // BGR
{
	computeGaussianKernel();
	computeKernel();
}

Config::~Config()
{ }

// accessors

bool Config::getDebug() const
{
	return m_debug;
}

void Config::setDebug(const bool value)
{
	m_debug = value;
	if (m_debug)
	{
		setDisplayOptionalWindows(true);
		setDisplayDuration(true);
	}
}

bool Config::getDisplayOptionalWindows() const
{
	return m_displayOptionalWindows;
}

void Config::setDisplayOptionalWindows(const bool value)
{
	m_displayOptionalWindows = value;
}

bool Config::getDisplayDuration() const
{
	return m_displayDuration;
}

void Config::setDisplayDuration(const bool value)
{
	m_displayDuration = value;
}

bool Config::getSaveDetection() const
{
	return m_saveDetection;
}

void Config::setSaveDetection(const bool value)
{
	m_saveDetection = value;
}

bool Config::getSaveResultWithoutDetection() const
{
	return m_saveResultWithoutDetection;
}

void Config::setSaveResultWithoutDetection(const bool value)
{
	m_saveResultWithoutDetection = value;	
}

bool Config::getSaveResult() const
{
	return m_saveResult;
}

void Config::setSaveResult(const bool value)
{
	m_saveResult = value;
}

bool Config::getSaveMask() const
{
	return m_saveMask;
}

void Config::setSaveMask(const bool value)
{
	m_saveMask = value;
}

unsigned int Config::getDetectionArea() const
{
	return m_detectionArea;
}

void Config::setDetectionArea(const unsigned int value)
{
	if (value < 100 || value > 50000)
	{
		throw invalid_argument("detection area must be an integer in [100;50000]. Given: " + to_string(value));
	}
	m_detectionArea = value;
}

unsigned int Config::getGaussianBlur() const
{
	return m_gaussianBlur;
}

void Config::setGaussianBlur(const unsigned int value)
{
	if (value > 255 || (value != 0 && value % 2 == 0))
	{
		throw invalid_argument("gaussian blur must be an odd integer in [0;255]. Given: " + to_string(value));
	}
	m_gaussianBlur = value;
	computeGaussianKernel();
}

cv::Size Config::getGaussianKernel() const
{
	return m_gaussianKernel;
}

unsigned int Config::getThreshold() const
{
	return m_threshold;
}

void Config::setThreshold(const unsigned int value)
{
	if (value > 255)
	{
		throw invalid_argument("threshold must be an integer in [0;255]. Given: " + to_string(value));
	}
	m_threshold = value;
}

unsigned int Config::getKernelSize() const
{
	return m_kernelSize;
}

void Config::setKernelSize(const unsigned int value)
{
	if (value < 1 || value > 100)
	{
		throw invalid_argument("kernel size must be an integer in [1;100]. Given: " + to_string(value));
	}
	m_kernelSize = value;
	computeKernel();
}

cv::Mat Config::getKernel() const
{
	return m_kernel;
}

void Config::setRectangleColor(const unsigned int red, const unsigned int green, const unsigned int blue)
{
	if (blue > 255)
	{
		throw invalid_argument("blue must be an integer in [0;255]. Given: " + to_string(blue));
	}
	if (green > 255)
	{
		throw invalid_argument("green must be an integer in [0;255]. Given: " + to_string(green));
	}
	if (red > 255)
	{
		throw invalid_argument("red must be an integer in [0;255]. Given: " + to_string(red));
	}
	m_rectangleColor.val[0] = blue;
	m_rectangleColor.val[1] = green;
	m_rectangleColor.val[2] = red;
}

Scalar Config::getRectangleColor() const
{
	return m_rectangleColor;
}

// methods

void Config::inverseDisplayDuration()
{
	setDisplayDuration(!m_displayDuration);
}

bool Config::decreaseDetectionArea(const unsigned int decrement)
{
	if (decrement == 0)
	{
		throw invalid_argument("decrement must be an integer greater than or equal to 1. Given: " + to_string(decrement));
	}
	int res = m_detectionArea - decrement; // explicitly store the result in a int type
	if (res >= 100)
	{
		setDetectionArea(res);
		return true;
	}
	return false;
}

bool Config::increaseDetectionArea(const unsigned int increment)
{
	if (increment == 0)
	{
		throw invalid_argument("increment must be an integer greater than or equal to 1. Given: " + to_string(increment));
	}
	int res = m_detectionArea + increment; // explicitly store the result in a int type
	if (res <= 50000)
	{
		setDetectionArea(res);
		return true;
	}
	return false;
}

bool Config::decreaseGaussianBlur(unsigned int decrement)
{
	if (decrement == 0)
	{
		throw invalid_argument("decrement must be an integer greater than or equal to 1. Given: " + to_string(decrement));
	}
	if (m_gaussianBlur == 1)
	{
		decrement = 1;
	}
	int res = m_gaussianBlur - decrement;
	if (res >= 0)
	{
		setGaussianBlur(res);
		return true;
	}
	return false;
}

bool Config::increaseGaussianBlur(unsigned int increment)
{
	if (increment == 0)
	{
		throw invalid_argument("increment must be an integer greater than or equal to 1. Given: " + to_string(increment));
	}
	if (m_gaussianBlur == 0)
	{
		increment = 1;
	}
	int res = m_gaussianBlur + increment;
	if (res <= 255)
	{
		setGaussianBlur(res);
		return true;
	}
	return false;
}

bool Config::decreaseThreshold(const unsigned int decrement)
{
	if (decrement == 0)
	{
		throw invalid_argument("decrement must be an integer greater than or equal to 1. Given: " + to_string(decrement));
	}
	int res = m_threshold - decrement;
	if (res >= 0)
	{
		setThreshold(res);
		return true;
	}
	return false;
}

bool Config::increaseThreshold(const unsigned int increment)
{
	if (increment == 0)
	{
		throw invalid_argument("increment must be an integer greater than or equal to 1. Given: " + to_string(increment));
	}
	int res = m_threshold + increment;
	if (res <= 255)
	{
		setThreshold(res);
		return true;
	}
	return false;
}

bool Config::decreaseKernelSize(const unsigned int decrement)
{
	if (decrement == 0)
	{
		throw invalid_argument("decrement must be an integer greater than or equal to 1. Given: " + to_string(decrement));
	}
	int res = m_kernelSize - decrement;
	if (res >= 1)
	{
		setKernelSize(res);
		return true;
	}
	return false;
}

bool Config::increaseKernelSize(const unsigned int increment)
{
	if (increment == 0)
	{
		throw invalid_argument("increment must be an integer greater than or equal to 1. Given: " + to_string(increment));
	}
	int res = m_kernelSize + increment;
	if (res <= 100)
	{
		setKernelSize(res);
		return true;
	}
	return false;
}

void Config::computeGaussianKernel()
{
	m_gaussianKernel = Size(m_gaussianBlur, m_gaussianBlur);
}

void Config::computeKernel()
{
	m_kernel = getStructuringElement(MORPH_RECT, Size(m_kernelSize, m_kernelSize));
}

void Config::display(const char sep, const char start, const char end) const
{
	cout << start;
	cout << "gaussian blur: " << m_gaussianBlur << sep;
	cout << "threshold: " << m_threshold << sep;
	cout << "kernel size: " << m_kernelSize << sep;
	cout << "detection area: " << m_detectionArea << endl;
	cout << end;
}

void Config::save() const
{
	YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "config" << YAML::Value;
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "debug" << YAML::Value << m_debug;
    emitter << YAML::Key << "detection area" << YAML::Value << m_detectionArea;
    emitter << YAML::Key << "display duration" << YAML::Value << m_displayDuration;
    emitter << YAML::Key << "display optional windows" << YAML::Value << m_displayOptionalWindows;
    emitter << YAML::Key << "save detection" << YAML::Value << m_saveDetection;
    emitter << YAML::Key << "save result without detection" << YAML::Value << m_saveResultWithoutDetection;
    emitter << YAML::Key << "save result" << YAML::Value << m_saveResult;
    emitter << YAML::Key << "save mask" << YAML::Value << m_saveMask;
    emitter << YAML::Key << "gaussian blur" << YAML::Value << m_gaussianBlur;
    emitter << YAML::Key << "kernel size" << YAML::Value << m_kernelSize;
    emitter << YAML::Key << "rectangle color" << YAML::Value;
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "blue" << YAML::Value << m_rectangleColor.val[0];
    emitter << YAML::Key << "green" << YAML::Value << m_rectangleColor.val[1];
    emitter << YAML::Key << "red" << YAML::Value << m_rectangleColor.val[2];
    emitter << YAML::EndMap;
    emitter << YAML::Key << "threshold" << YAML::Value << m_threshold;
    emitter << YAML::EndMap;
    emitter << YAML::EndMap;

    std::ofstream fout("config.yaml");
    fout << emitter.c_str();
    fout.close();

    cout << "config saved as config.yaml" << endl;
}

template<typename T>
T Config::getValueFromYamlNode(const YAML::Node& node, const std::string& key) 
{
    if(YAML::Node valueNode = node[key])
    {
    	return valueNode.as<T>();
    }
    throw KeyNotFoundException(key);
}

bool Config::read()
{
	struct stat buffer;   
	if (stat ("config.yaml", &buffer) == 0)
	{
		YAML::Node root = YAML::LoadFile("config.yaml");
	    try
	    {
	    	YAML::Node configNode = getValueFromYamlNode<YAML::Node>(root, "config");
	    	YAML::Node colorNode = getValueFromYamlNode<YAML::Node>(configNode, "rectangle color");

	    	setDisplayDuration(getValueFromYamlNode<bool>(configNode, "display duration"));
	    	setDisplayOptionalWindows(getValueFromYamlNode<bool>(configNode, "display optional windows"));
			// read debug after because debug can change previous parameters (display optional windows, display duration)
	    	setDebug(getValueFromYamlNode<bool>(configNode, "debug"));
	    	setSaveDetection(getValueFromYamlNode<bool>(configNode, "save detection"));
	    	setSaveResultWithoutDetection(getValueFromYamlNode<bool>(configNode, "save result without detection"));
	    	setSaveResult(getValueFromYamlNode<bool>(configNode, "save result"));
	    	setSaveMask(getValueFromYamlNode<bool>(configNode, "save mask"));

	    	setDetectionArea(getValueFromYamlNode<unsigned int>(configNode, "detection area"));
	    	setGaussianBlur(getValueFromYamlNode<unsigned int>(configNode, "gaussian blur"));
	    	setThreshold(getValueFromYamlNode<unsigned int>(configNode, "threshold"));
	    	setKernelSize(getValueFromYamlNode<unsigned int>(configNode, "kernel size"));
	    	setRectangleColor(
	    		getValueFromYamlNode<unsigned int>(colorNode, "blue"),
	    		getValueFromYamlNode<unsigned int>(colorNode, "green"),
	    		getValueFromYamlNode<unsigned int>(colorNode, "red"));
	    }
	    catch(const KeyNotFoundException& e) 
	    {
	        cout << "Key error in config.yaml file!" << endl;
	    	cout << "Key not found: " << e.getKeyName() << endl;
	    	cout << "config reset" << endl;
	    	reset();
	    	save();
	    }
	    catch(const YAML::BadConversion& e) 
	    {
	        cerr << "Conversion error in config.yaml file!" << endl;
	    	cerr << e.what() << endl;
	    	return false;
	    }
	    catch (const std::invalid_argument& e)
	    {
	    	cerr << "Error in config.yaml file! Bad value given!" << endl;
	    	cerr << e.what() << endl;
	    	return false;
	    }
	    catch (const YAML::Exception& e)
	    {
	    	cerr << "Error in config.yaml file!" << endl;
	    	cerr << e.what() << endl;
	    	return false;
	    }
	}
	else
	{
		cout << "config not file found" << endl;
		save();
	}
	return true;
}

void Config::reset()
{
	setDebug(false);
    setDisplayOptionalWindows(false);
    setDisplayDuration(false);
    setSaveDetection(false);
    setSaveResultWithoutDetection(false);
    setSaveResult(false);
    setSaveMask(false);
    setDetectionArea(500);
    setGaussianBlur(0);
    setThreshold(70);
    setKernelSize(6);
    setRectangleColor(0, 255, 0);
}
