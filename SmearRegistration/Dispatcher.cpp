#include <assert.h>
#include <sstream>
#include <iterator>
#include <iostream>

#include "Dispatcher.h"
#include "FeatureDetector.h"

Dispatcher& Dispatcher::GetInstance(int argc, char *argv[])
{
	static Dispatcher dis(argc, argv);

	return dis;
}

void Dispatcher::Initialize()
{
	m_Parser.AddArg("-dot", bind(&Dispatcher::Test1, this));
	m_Parser.AddArg("-kp", bind(&Dispatcher::SetKeepParse, this));
	m_Parser.AddArg("-rg", bind(&Dispatcher::SetImgRange, this));
	m_Parser.AddArg("-ph", bind(&Dispatcher::SetImgPath, this));
	m_Parser.AddArg("-sf", bind(&Dispatcher::SetImgSuffix, this));
}

Dispatcher::Dispatcher()
{
	Initialize();

	_Parse();
}

Dispatcher::Dispatcher(int argc, char *argv[])
{
	Initialize();

	//note that argv[0] is the program name
	if (argc > 1)
	{
		for (int i = 0; i < argc - 1; ++i)
		{
			m_Parser.push_back(argv[i + 1]);
		}
	}

	_Parse();
}

int Dispatcher::Parse(istream &_inf)
{
	assert(!!_inf);

	while (!_inf.eof())
	{
		string sline;
		std::getline(_inf, sline);
		if (sline.size() == 0)
		{
			continue;
		}
		istringstream iss(sline);

		std::copy(istream_iterator<string>(iss), istream_iterator<string>(), std::back_inserter(m_Parser));
	}

	return _Parse();
}

void Dispatcher::Test1()
{
	cout << "test1" << endl;
}

void Dispatcher::SetKeepParse()
{
	m_Parser.AddArgVal(m_KeepParse);
}

void Dispatcher::SetImgRange()
{
	m_Parser.AddArgVal(m_XBegin, m_XEnd, m_YBegin, m_YEnd);
}

void Dispatcher::SetImgPath()
{
	m_Parser.AddArgVal(m_PathName);
}

void Dispatcher::SetImgSuffix()
{
	m_Parser.AddArgVal(m_PathName);
}

int EvaluateFeaturePoint(const loscv::PDector &_dector, const Mat &_img1, const Mat &_img2, const Mat &_homo, vector<KeyPoint> &_points1, vector<KeyPoint> &_points2)
{
	cout << "Extracting keypoints ..." << endl;
	vector<KeyPoint> keypoints1;

	_dector->detect(_img1, _points1);
	_dector->detect(_img2, _points2);

	float repeatability;
	int correspCount;
	cv::evaluateFeatureDetector(_img1, _img2, _homo, &_points1, &_points2, repeatability, correspCount);
	cout << "repeatability = " << repeatability << endl;
	cout << "correspCount = " << correspCount << endl;

	return 0;
}

//int EstimateAffine(const Mat &_transform)
//{
//	
//}

int Dispatcher::_Parse()
{
	return m_Parser.Parse();	
}

int Dispatcher::Parse(const string &_para)
{
	if (!_para.empty())
	{
		istringstream iss(_para);
		return Parse(iss);
	}

	return -1;
}

int Dispatcher::ParseFile(const string &_fileName)
{
	int status = -1;

	ifstream inf(_fileName, ios::in);
	if (inf)
	{
		status = Parse(inf);
	}

	inf.close();
	return status;
}

bool Dispatcher::KeepParse() const
{
	return m_KeepParse;
}