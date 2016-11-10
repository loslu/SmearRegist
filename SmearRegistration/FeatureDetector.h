#pragma once

#include <string>
#include <functional>
#include <map>
#include <opencv2\opencv.hpp>
#include "Util.h"

namespace loscv
{
	typedef los::NamedPtr<cv::FeatureDetector> PDector;
class FeatureDetectorFactory
{
	static std::map<std::string, std::function<PDector(std::string, std::string)>> s_table;
public:
	static PDector Create(const std::string &_name, const std::string &_paras);
};

}