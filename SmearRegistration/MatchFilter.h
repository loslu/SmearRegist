#pragma once
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "Util.h"

namespace loscv
{

class MatchFilter;

typedef los::NamedPtr<MatchFilter>(*CreateFilterFunc)();
typedef los::NamedPtr<MatchFilter> PMatchFilter;
class FilterFactory
{
	FilterFactory() = default;
	FilterFactory(const FilterFactory&) = delete;
	void operator=(const FilterFactory&) = delete;

	std::unordered_map<std::string, CreateFilterFunc> m_HashMap;
public:
	static FilterFactory& GetInstance()
	{
		static FilterFactory s_Factory;
		return s_Factory;
	}

	PMatchFilter Create(const std::string &_name)
	{
		const auto iter = m_HashMap.find(_name);
		if (iter == m_HashMap.end())
		{
			return PMatchFilter(_name);
		}

		return iter->second();
	}

	void AddType(const std::string &_name, CreateFilterFunc _func)
	{
		m_HashMap[_name] = _func;
	}
};

class MatchFilter
{
public:
	static PMatchFilter Create(const std::string &_name)
	{
		return FilterFactory::GetInstance().Create(_name);
	}

	virtual int SetPara(const std::string &_paraStr)
	{
		//this function is disabled by default, if we want to set parameters for a filter, we need to overload this function.
		return -1;
	}

	//default match filter
	virtual void Filter(cv::DescriptorMatcher* descriptorMatcher, const cv::Mat& descriptors1, const cv::Mat& descriptors2, std::vector<cv::DMatch>& matches12) = 0;
};

class FilterRegister
{
public:
	FilterRegister(const std::string &_name, CreateFilterFunc _func)
	{
		FilterFactory::GetInstance().AddType(_name, _func);
	}
};

#define DECLARE_FILTER(NAME) public: static FilterRegister s_Register; \
static PMatchFilter CreateThis() { \
PMatchFilter filter(#NAME); \
filter = std::make_shared<NAME>(); \
return filter;}

#define IMPLEMENT_FILTER(NAME) FilterRegister NAME::s_Register(#NAME, CreateThis);

}
