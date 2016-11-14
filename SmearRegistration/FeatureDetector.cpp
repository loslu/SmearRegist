#include "FeatureDetector.h"
#include <opencv2/nonfree/nonfree.hpp>

using namespace std;
using namespace loscv;
using namespace los;

PDector CreateSURF(const string &_name, const string &_paras);
PDector CreateSIFT(const string &_name, const string &_paras);
PDector CreateBRISK(const string &_name, const string &_paras);
PDector CreateORB(const string &_name, const string &_paras);

map<string, function<PDector(string, string)>> FeatureDetectorFactory::s_table =
{
	{"SURF", bind(CreateSURF, placeholders::_1, placeholders::_2)},
	{"SIFT", bind(CreateSIFT, placeholders::_1, placeholders::_2)},
	{"BRISK", bind(CreateBRISK, placeholders::_1, placeholders::_2)},
	{"ORB", bind(CreateORB, placeholders::_1, placeholders::_2)}
};

PDector CreateSURF(const string &_name, const string &_paras)
{
	PDector detector(_name);


	//parse parameters

	detector = make_shared<cv::SURF>();
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

PDector CreateSIFT(const string &_name, const string &_paras)
{
	PDector detector(_name);


	//parse parameters

	detector = make_shared<cv::SIFT>();
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

PDector CreateBRISK(const string &_name, const string &_paras)
{
	PDector detector(_name);


	//parse parameters

	detector = make_shared<cv::BRISK>();
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

PDector CreateORB(const string &_name, const string &_paras)
{
	PDector detector(_name);

	int nfeatures = 500;
	float scaleFactor = 1.2f;
	int nlevels = 8;
	int edgeThreshold = 31;
	int firstLevel = 0;
	int WTA_K = 2;
	int scoreType = cv::ORB::HARRIS_SCORE;
	int patchSize = 31;

	//parse parameters

	detector = make_shared<cv::ORB>(nfeatures, scaleFactor, nlevels, edgeThreshold, firstLevel, WTA_K, scoreType, patchSize);
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

PDector FeatureDetectorFactory::Create(const string &_name, const string &_paras)
{
	auto iter = s_table.find(_name);
	if (iter != s_table.end())
	{
		return iter->second(_name, _paras);
	}

	return PDector(_name);
}
