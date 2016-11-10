#pragma once
#include <string>
#include <functional>
#include <map>
#include <list>
using namespace std;

class ArgParser
{
	string GetFront();
	void SetVal();

	map<string, function<void(void)>> m_Map;
	list<string> m_Reqs;
public:
	typedef string value_type;

	ArgParser();
	virtual ~ArgParser();

	int AddArg(const string &_name, function<void(void)> _func);

	template<typename T>
	int AddArgVal(const string &_name, T &_dst, size_t _argCount = 1)
	{

	}

	int Parse();
	void push_back(const string &_request);
};

