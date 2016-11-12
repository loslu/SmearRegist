#include "Dispatcher.h"

int main(int argc, char *argv[])
{
	auto &dispatcher = Dispatcher::GetInstance(argc, argv);
	while (dispatcher.KeepParse())
	{
		dispatcher.Parse(cin);
	}

	cout << dispatcher.m_XBegin << "\t" << dispatcher.m_XEnd << '\t' << dispatcher.m_YBegin << '\t' << dispatcher.m_YEnd << endl;

	return 0;
}