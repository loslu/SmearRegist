#include <assert.h>
#include <sstream>
#include <iterator>
#include <iostream>

#include "Dispatcher.h"
#include "FeatureUtil.h"

Dispatcher& Dispatcher::GetInstance(int argc, char *argv[])
{
	static Dispatcher dis(argc, argv);

	return dis;
}

void Dispatcher::Initialize()
{
	m_Parser.AddArg("-kp", bind(&Dispatcher::SetKeepParse, this));
	m_Parser.AddArg("-rg", bind(&Dispatcher::SetImgRange, this));
	m_Parser.AddArg("-ph", bind(&Dispatcher::SetImgPath, this));
	m_Parser.AddArg("-sf", bind(&Dispatcher::SetImgSuffix, this));

	m_Parser.AddArg("-d", bind(&Dispatcher::AddDetector, this));
	m_Parser.AddArg("-m", bind(&Dispatcher::AddMatcher, this));
	m_Parser.AddArg("-f", bind(&Dispatcher::AddMatchFilter, this));
	m_Parser.AddArg("-dgc", bind(&Dispatcher::InvokeGrayAndColorEstimation, this));
	m_Parser.AddArg("-dt", bind(&Dispatcher::InvokeTranslationEstimation, this));
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

void Dispatcher::AddDetector()
{
	string name, para;
	m_Parser.AddArgVal(name, para);

	m_Dectors.push_back(loscv::FeatureDetectorFactory::Create(name, para));
}

void Dispatcher::AddMatcher()
{
	string name, para;
	m_Parser.AddArgVal(name, para);

	m_Matchers.push_back(loscv::FeatureMatcherFactory::Create(name, para));
}

void Dispatcher::AddMatchFilter()
{
	string name;
	m_Parser.AddArgVal(name);

	m_MatchFilters.push_back(loscv::MatchFilter::Create(name));
}

void Dispatcher::InvokeGrayAndColorEstimation()
{
	string para;
	m_Parser.AddArgVal(para);

	for (int i = m_XBegin; i < m_XEnd; ++i)
	{
		for (int j = m_YBegin; j < m_YEnd; ++j)
		{
			Mat img1 = GetColorImage(i, j);
			Mat img2 = GetGrayImage(i, j);

			for (auto dector : m_Dectors)
			{
				for (auto matcher : m_Matchers)
				{
					for (auto filter : m_MatchFilters)
					{
						Mat affine;
						Mat matchImg;
						EstimateAffine(dector, matcher, filter, img1, img2, affine, matchImg);
						m_AffineRets.push_back(affine);
						cout << affine << endl;
						imwrite(m_PathName + GetImgName(i, j) + "-match-" + dector.m_Name + "-" + matcher.m_Name + m_Suffix, matchImg);

						if (para == "show")
						{
							Mat dst;
							cv::warpAffine(img1, dst, affine(cv::Rect(0, 0, 3, 2)), cv::Size(img1.cols, img1.rows));
							imwrite(m_PathName + GetImgName(i, j) + "-affine-" + dector.m_Name + "-" + matcher.m_Name + m_Suffix, dst);
						}
					}
				}
			}
		}
	}
}

void Dispatcher::InvokeTranslationEstimation()
{
	string para;
	m_Parser.AddArgVal(para);

	//temporarily only horizontal translation is supported
	if (para != "hor")
	{
		cout << "not supported" << endl;
		return;
	}

	for (int i = m_XBegin; i < m_XEnd; ++i)
	{
		for (int j = m_YBegin; j < m_YEnd; ++j)
		{
			Mat img1 = GetColorImage(i, j);
			Mat img2 = GetColorImage(i + 1, j);

			for (auto dector : m_Dectors)
			{
				for (auto matcher : m_Matchers)
				{
					for (auto filter : m_MatchFilters)
					{
						cv::Point2f	offset;
						Mat matchImg;
						EstimateTranslation(dector, matcher, filter, img1, img2, offset, matchImg);
						
						cout << offset << endl;
						imwrite(m_PathName + GetImgName(i, j) + "-hor-match-" + dector.m_Name + "-" + matcher.m_Name + m_Suffix, matchImg);

						//horizontal translation must have offset.x < 0, because we choose the left image as img1
						if (offset.x < 0)
						{
							int dx = static_cast<int>(std::round(offset.x));
							int dy = static_cast<int>(std::round(offset.y));

							Mat dst = Mat::zeros(img2.rows + abs(dy), img2.cols - dx, img1.type());
							if (dy >= 0)
							{
								img1.copyTo(dst(cv::Rect(0, img2.rows - img1.rows + dy, img1.cols, img1.rows)));
								img2.copyTo(dst(cv::Rect(-dx, 0, img2.cols, img2.rows)));
							}
							else
							{
								img1.copyTo(dst(cv::Rect(0, 0, img1.cols, img1.rows)));
								img2.copyTo(dst(cv::Rect(-dx, -dy, img2.cols, img2.rows)));
							}
							imwrite(m_PathName + GetImgName(i, j) + "-hor-affine-" + dector.m_Name + "-" + matcher.m_Name + m_Suffix, dst);
						}
						else
						{
							cout << "invalid horizontal translation" << endl;
						}
					}
				}
			}
		}
	}
}

cv::Mat Dispatcher::GetColorImage(int _x, int _y)
{
	string name(m_PathName + GetImgName(_x, _y) + "Color" + m_Suffix);
	Mat img = imread(name, CV_LOAD_IMAGE_COLOR);
	if (!img.data)
	{
		cout << "error loading " << name << endl;
	}

	return img;
}

cv::Mat Dispatcher::GetGrayImage(int _x, int _y)
{
	string name(m_PathName + GetImgName(_x, _y) + "Black" + m_Suffix);
	Mat img = imread(name, CV_LOAD_IMAGE_GRAYSCALE);
	if (!img.data)
	{
		cout << "error loading " << name << endl;
	}

	return img;
}

string Dispatcher::GetImgName(int _x, int _y)
{
	string x;
	if (_x < 100)
	{
		x += "0";
	}
	if (_x < 10)
	{
		x += "0";
	}
	x += std::to_string(_x);

	string y;
	if (_y < 100)
	{
		y += "0";
	}
	if (_y < 10)
	{
		y += "0";
	}
	y += std::to_string(_y);

	return y + x;
}