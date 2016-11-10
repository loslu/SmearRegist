#include <assert.h>
#include <sstream>
#include <iterator>
#include <iostream>

#include "Dispatcher.h"
#include "FeatureDetector.h"

Dispatcher::Dispatcher(int argc, char *argv[])
{
	//note that argv[0] is the program name
	if (argc > 1)
	{
		for (int i = 0; i < argc - 1; ++i)
		{
			m_Parser.push_back(argv[i + 1]);
		}
	}

	_ParseSettings();
}

Dispatcher::~Dispatcher()
{
}

int Dispatcher::AddSettings(istream &_inf)
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

	return 0;
}

void Dispatcher::Test1()
{
	cout << "test1" << endl;
}

void Dispatcher::Test2(int *_val)
{
	cout << "test2: " << *_val << endl;
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

int Dispatcher::_ParseSettings()
{
	m_Parser.AddArg("-dot", bind(&Dispatcher::Test1, this));
	m_Parser.AddArg("-doa", bind(&Dispatcher::Test2, this, &val));
	m_Parser.AddArg("-dos", [this]() {val = 1;  cout << "Test3: " << val << endl; });

	return m_Parser.Parse();

	/*ListAdvance(cur, m_vSettings);
	if (cur != m_vSettings.end())
	{
		m_vFeatureSetNames.push_back(*cur);
	}
	else
	{
		g_ConLog->error("invalid parameter at line %d in %s", __LINE__, __FILE__);
	}*/
	
}