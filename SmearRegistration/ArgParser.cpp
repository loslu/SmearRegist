#include "ArgParser.h"
#include <iostream>
using namespace std;

ArgParser::ArgParser()
{
}


ArgParser::~ArgParser()
{
}

void ArgParser::SetVal()
{

}

int ArgParser::AddArg(const string &_name, function<void(void)> _func)
{
	auto ans = m_Map.insert(make_pair(_name, _func));
	if (!ans.second)
	{
		return -1;
	}

	return 0;
}

void ArgParser::push_back(const string &_request)
{
	m_Reqs.push_back(_request);
}

string ArgParser::GetFront()
{
	if (m_Reqs.empty())
	{
		cout << "wrong para" << endl;
		return string();
	}

	string ans = m_Reqs.front();
	m_Reqs.pop_front();

	return ans;
}

int ArgParser::Parse()
{
	while (!m_Reqs.empty())
	{
		string cur = GetFront();
		auto iter = m_Map.find(cur);
		if (iter != m_Map.end())
		{
			(iter->second)();
		}
	}

	return 0;
}