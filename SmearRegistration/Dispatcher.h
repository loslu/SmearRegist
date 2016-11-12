#pragma once

#include <fstream>
#include <list>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include "ArgParser.h"

class Dispatcher
{
	Dispatcher();
	Dispatcher(int argc, char *argv[]);
	Dispatcher(const Dispatcher &) = default;
	Dispatcher& operator=(const Dispatcher&) = default;

	void Initialize();
	int _Parse();

	void SetImgRange();
	void SetImgPath();
	void SetImgSuffix();
	void SetKeepParse();

	ArgParser m_Parser;
	int val = 0;
	bool m_KeepParse = false;
	//range of used image
public:
	int m_XBegin = 0, m_XEnd = m_XBegin + 1, m_YBegin = 0, m_YEnd = m_YBegin + 1;
	string m_PathName;
	string m_Suffix = ".jpg";

public:
	static Dispatcher& GetInstance(int argc, char *argv[]);
	~Dispatcher() = default;
	int Parse(istream &_inf);
	int Parse(const string &_para);
	int ParseFile(const string &_fileName);
	bool KeepParse() const;

	void Test1();

	void Test2(int *_val);
};

