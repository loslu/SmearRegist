#pragma once

#include <fstream>
#include <list>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include "ArgParser.h"

class Dispatcher
{
	int _ParseSettings();
	int AddSettings(istream &_inf);

	ArgParser m_Parser;
	int val = 0;
public:
	Dispatcher(int argc, char *argv[]);
	~Dispatcher();

	void Test1();

	void Test2(int *_val);
};

