#pragma once

#include "MatchFilter.h"
using namespace loscv;
using namespace std;
using namespace cv;

class DefaultFilter : public MatchFilter
{
	DECLARE_FILTER(DefaultFilter)
public:
	void Filter(DescriptorMatcher* descriptorMatcher, const Mat& descriptors1, const Mat& descriptors2, vector<DMatch>& matches12)
	{
		descriptorMatcher->match(descriptors1, descriptors2, matches12);
	}
};