#pragma once

#include <sstream>
#include "MatchFilter.h"
using namespace loscv;
using namespace std;
using namespace cv;

class CrossFilter : public MatchFilter
{
	int m_Knn;
public:
	DECLARE_FILTER(CrossFilter)

	CrossFilter(): m_Knn(1){}

	int SetPara(const string &_paraStr)
	{
		istringstream iss(_paraStr);
		iss >> m_Knn;

		return 0;
	}

	void Filter(DescriptorMatcher* descriptorMatcher, const Mat& descriptors1, const Mat& descriptors2, vector<DMatch>& filteredMatches12)
	{
		filteredMatches12.clear();
		vector<vector<DMatch> > matches12, matches21;
		descriptorMatcher->knnMatch(descriptors1, descriptors2, matches12, m_Knn);
		descriptorMatcher->knnMatch(descriptors2, descriptors1, matches21, m_Knn);
		for (size_t m = 0; m < matches12.size(); m++)
		{
			bool findCrossCheck = false;
			for (size_t fk = 0; fk < matches12[m].size(); fk++)
			{
				DMatch forward = matches12[m][fk];

				for (size_t bk = 0; bk < matches21[forward.trainIdx].size(); bk++)
				{
					DMatch backward = matches21[forward.trainIdx][bk];
					if (backward.trainIdx == forward.queryIdx)
					{
						filteredMatches12.push_back(forward);
						findCrossCheck = true;
						break;
					}
				}
				if (findCrossCheck) break;
			}
		}
	}
};