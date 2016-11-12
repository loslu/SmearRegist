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
	void AddArgVal(T& _dst)
	{
		string s = GetFront();
		if (!s.empty())
		{
			istringstream iss(s);
			iss >> _dst;
		}
	}

	template<typename T, typename... Args>
	void AddArgVal(T& _dst, Args&... _args)
	{
		string s = GetFront();
		if (!s.empty())
		{
			istringstream iss(s);
			iss >> _dst;
		}

		AddArgVal(_args...);
	}

	int Parse();
	void push_back(const string &_request);
};

