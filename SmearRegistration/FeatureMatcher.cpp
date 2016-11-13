#include "FeatureMatcher.h"

using namespace std;
using namespace loscv;
using namespace los;

PMatcher CreateBruteForce(const string &_name, const string &_paras);
PMatcher CreateFlann(const string &_name, const string &_paras);

map<string, function<PMatcher(string, string)>> FeatureMatcherFactory::s_table =
{
	{ "BF", bind(CreateBruteForce, placeholders::_1, placeholders::_2) },
	{ "FLANN", bind(CreateFlann, placeholders::_1, placeholders::_2) }
};

PMatcher CreateBruteForce(const string &_name, const string &_paras)
{
	PMatcher detector(_name);


	//parse parameters

	detector = make_shared<cv::BFMatcher>();
	if (!detector)
	{
		detector.m_ErrCode = -1;
		detector.m_ErrMsg = "construct error";
	}
	else
	{
		detector.m_ErrCode = 0;
	}

	return detector;
}

PMatcher CreateFlann(const string &_name, const string &_paras)
{
	PMatcher detector(_name);


	//parse parameters

	detector = make_shared<cv::FlannBasedMatcher>();
	if (!detector)
	{
		detector.m_ErrCode = -1;
		detector.m_ErrMsg = "construct error";
	}
	else
	{
		detector.m_ErrCode = 0;
	}

	return detector;
}

PMatcher FeatureMatcherFactory::Create(const string &_name, const string &_paras)
{
	auto iter = s_table.find(_name);
	if (iter == s_table.end())
	{
		return iter->second(_name, _paras);
	}

	return PMatcher(_name);
}
