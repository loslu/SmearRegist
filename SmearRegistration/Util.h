#pragma once
#include <string>
#include <memory>

namespace los
{

template<typename T>
class NamedPtr : public std::shared_ptr<T>
{
public:
	NamedPtr(const std::string &_name = ""): m_Name(_name) {}
	std::string m_Name;
	std::string m_ErrMsg = "Not initialized";
	int m_ErrCode = -1;

	NamedPtr& operator=(const std::shared_ptr<T> &_ptr)
	{
		std::shared_ptr<T>::operator =(_ptr);

		return *this;
	}
};

}