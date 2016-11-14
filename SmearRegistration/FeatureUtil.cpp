#include "FeatureUtil.h"
#include <time.h>
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

int EstimateAffine(const loscv::PDector &_dector, const loscv::PMatcher &_matcher, const loscv::PMatchFilter &_filter, const Mat &_img1, const Mat &_img2, Mat &_affine, cv::OutputArray _match)
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

	if (_match.needed())
	{
		Mat matchImg;
		if (!_affine.empty()) // filter outliers
		{
			vector<char> matchesMask(filteredMatches.size(), 0);
			vector<Point2f> points1; KeyPoint::convert(keyPoints1, points1, queryIdxs);
			vector<Point2f> points2; KeyPoint::convert(keyPoints2, points2, trainIdxs);
			Mat points1t; perspectiveTransform(Mat(points1), points1t, _affine);

			for (size_t i1 = 0; i1 < points1.size(); i1++)
			{
				if (norm(points2[i1] - points1t.at<Point2f>((int)i1, 0)) <= 3) // inlier
					matchesMask[i1] = 1;
			}
			//draw inliers
			drawMatches(_img1, keyPoints1, _img2, keyPoints2, filteredMatches, matchImg, Scalar(0, 255, 0), Scalar(255, 0, 0), matchesMask);

			cout << "Number of inliers: " << countNonZero(matchesMask) << endl;
		}
		else
			drawMatches(_img1, keyPoints1, _img2, keyPoints2, filteredMatches, matchImg);

		_match.create(matchImg.rows, matchImg.cols, CV_8UC3, -1, true);
		Mat match = _match.getMat();
		matchImg.copyTo(_match);
	}

	return 0;
}


int CalcTransform(const vector<Point2f> &_p1, const vector<Point2f> &_p2, cv::Point2f &_offset)
{
	vector<Point2f> off(_p1.size());
	for (size_t i = 0; i < _p1.size(); i++)
	{
		off[i].x = _p1[i].x - _p2[i].x;
		off[i].y = _p1[i].y - _p2[i].y;
	}

	const float diff = 1.5f;
	int maxCount = 0;
	float minDist = -1.f;
	_offset.x = 0;
	_offset.y = 0;
	for (size_t i = 0; i < off.size(); i++)
	{
		int count = 0;
		float dist = 0.f;
		for (size_t j = 0; j < off.size(); j++)
		{
			float dx = fabs(off[i].x - off[j].x);
			float dy = fabs(off[i].y - off[j].y);
			if (dx < diff && dy < diff)
			{
				++count;
				dist += sqrt(dx * dx + dy * dy);
			}
		}
		dist /= count;
		if (count > maxCount * 9 / 10 && count > off.size() / 10 && (dist < minDist || minDist < 0))
		{
			maxCount = count;
			minDist = dist;
			_offset.x = -off[i].x;
			_offset.y = -off[i].y;
		}
	}

	return maxCount;
}

int EstimateTranslation(const loscv::PDector &_dector, const loscv::PMatcher &_matcher, const loscv::PMatchFilter &_filter, const Mat &_img1, const Mat &_img2, cv::Point2f &_offset, cv::OutputArray _match)
{
	//feature detector
	clock_t b, a = clock();
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
	int count = CalcTransform(points1, points2, _offset);
	b = clock();
	cout << static_cast<double>(b - a) / CLOCKS_PER_SEC << endl;

	if (_match.needed())
	{
		Mat matchImg;
		vector<char> matchesMask(filteredMatches.size(), 0);
		vector<Point2f> points1; KeyPoint::convert(keyPoints1, points1, queryIdxs);
		vector<Point2f> points2; KeyPoint::convert(keyPoints2, points2, trainIdxs);

		Mat trans = Mat::eye(3, 3, CV_32FC1);
		trans.at<float>(0, 2) = _offset.x;
		trans.at<float>(1, 2) = _offset.y;

		Mat points1t; perspectiveTransform(Mat(points1), points1t, trans);

		for (size_t i1 = 0; i1 < points1.size(); i1++)
		{
			if (norm(points2[i1] - points1t.at<Point2f>((int)i1, 0)) <= 3) // inlier
				matchesMask[i1] = 1;
		}
		//draw inliers
		drawMatches(_img1, keyPoints1, _img2, keyPoints2, filteredMatches, matchImg, Scalar(0, 255, 0), Scalar(255, 0, 0), matchesMask);

		cout << "Number of inliers: " << countNonZero(matchesMask) << endl;

		_match.create(matchImg.rows, matchImg.cols, CV_8UC3, -1, true);
		Mat match = _match.getMat();
		matchImg.copyTo(_match);
	}

	return 0;
}
