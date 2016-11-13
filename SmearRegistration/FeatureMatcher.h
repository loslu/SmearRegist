#pragma once

#include <string>
#include <functional>
#include <map>
#include <opencv2\opencv.hpp>
#include "Util.h"

namespace loscv
{

typedef los::NamedPtr<cv::DescriptorMatcher> PMatcher;
class FeatureMatcherFactory
{
	static std::map<std::string, std::function<PMatcher(std::string, std::string)>> s_table;
public:
	static PMatcher Create(const std::string &_name, const std::string &_paras);
};

}

