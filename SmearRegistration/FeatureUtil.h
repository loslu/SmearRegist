#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "FeatureDetector.h"
#include "FeatureMatcher.h"
#include "MatchFilter.h"

int EvaluateFeaturePoint(const loscv::PDector &_dector, const loscv::PMatcher &_matcher, const loscv::PMatchFilter &_filter, const cv::Mat &_img1, const cv::Mat &_img2, const cv::Mat &_homo);
int EstimateAffine(const loscv::PDector &_dector, const loscv::PMatcher &_matcher, const loscv::PMatchFilter &_filter, const cv::Mat &_img1, const cv::Mat &_img2, cv::Mat &_affine, cv::OutputArray _matchImg = cv::noArray());

int EstimateTranslation(const loscv::PDector &_dector, const loscv::PMatcher &_matcher, const loscv::PMatchFilter &_filter, const cv::Mat &_img1, const cv::Mat &_img2, cv::Point2f &_offset, cv::OutputArray _match = cv::noArray());
