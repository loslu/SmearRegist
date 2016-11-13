#include "FeatureUtil.h"
using namespace cv;
using namespace std;

int EvaluateFeaturePoint(const loscv::PDector &_dector, const loscv::PMatcher &_matcher, const loscv::PMatchFilter &_filter, const Mat &_img1, const Mat &_img2, const Mat &_homo)
{
	//feature detector
	vector<KeyPoint> keyPoints1, keyPoints2;
	_dector->detect(_img1, keyPoints1);
	_dector->detect(_img2, keyPoints2);

	float repeatability;
	int correspCount;
	cv::evaluateFeatureDetector(_img1, _img2, _homo, &keyPoints1, &keyPoints2, repeatability, correspCount);
	cout << "repeatability = " << repeatability << endl;
	cout << "correspCount = " << correspCount << endl;

	//feature descriptor
	Mat descriptors1, descriptors2;
	_dector->compute(_img1, keyPoints1, descriptors1);
	_dector->compute(_img2, keyPoints2, descriptors2);

	//descriptor matcher
	vector<Point2f> curve;
	cv::Ptr<DescriptorExtractor> dectorCopy = cv::Ptr<Feature2D>(_dector);
	cv::Ptr<DescriptorMatcher> matcherCopy = _matcher;
	Ptr<GenericDescriptorMatcher> gdm = new VectorDescriptorMatcher(dectorCopy, matcherCopy);
	evaluateGenericDescriptorMatcher(_img1, _img2, _homo, keyPoints1, keyPoints2, 0, 0, curve, gdm);

	//matches filter
	vector<DMatch> filteredMatches;
	_filter->Filter(_matcher.get(), descriptors1, descriptors2, filteredMatches);

	vector<int> queryIdxs(filteredMatches.size()), trainIdxs(filteredMatches.size());
	for (size_t i = 0; i < filteredMatches.size(); i++)
	{
		queryIdxs[i] = filteredMatches[i].queryIdx;
		trainIdxs[i] = filteredMatches[i].trainIdx;
	}

	vector<Point2f> points1;
	KeyPoint::convert(keyPoints1, points1, queryIdxs);
	vector<Point2f> points2;
	KeyPoint::convert(keyPoints2, points2, trainIdxs);
	Mat H12 = findHomography(Mat(points1), Mat(points2), CV_RANSAC, 3);

	return 0;
}

int EstimateAffine(const loscv::PDector &_dector, const loscv::PMatcher &_matcher, const loscv::PMatchFilter &_filter, const Mat &_img1, const Mat &_img2, Mat &_affine)
{
	//feature detector
	vector<KeyPoint> keyPoints1, keyPoints2;
	_dector->detect(_img1, keyPoints1);
	_dector->detect(_img2, keyPoints2);

	//feature descriptor
	Mat descriptors1, descriptors2;
	_dector->compute(_img1, keyPoints1, descriptors1);
	_dector->compute(_img2, keyPoints2, descriptors2);

	//descriptor matcher
	//matches filter
	vector<DMatch> filteredMatches;
	_filter->Filter(_matcher.get(), descriptors1, descriptors2, filteredMatches);

	vector<int> queryIdxs(filteredMatches.size()), trainIdxs(filteredMatches.size());
	for (size_t i = 0; i < filteredMatches.size(); i++)
	{
		queryIdxs[i] = filteredMatches[i].queryIdx;
		trainIdxs[i] = filteredMatches[i].trainIdx;
	}

	vector<Point2f> points1;
	KeyPoint::convert(keyPoints1, points1, queryIdxs);
	vector<Point2f> points2;
	KeyPoint::convert(keyPoints2, points2, trainIdxs);
	_affine = findHomography(Mat(points1), Mat(points2), CV_RANSAC, 3);

	return 0;
}