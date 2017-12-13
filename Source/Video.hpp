/*
 * This code is provided as part of "A Practical Introduction to Computer Vision with OpenCV"
 * by Kenneth Dawson-Howe © Wiley & Sons Inc. 2014.  All rights reserved.
 */
#ifndef VIDEO_HPP
#define VIDEO_HPP

#include "opencv2/core.hpp"

class MedianBackground
{
private:
	cv::Mat mMedianBackground;
	float**** mHistogram;
	float*** mLessThanMedian;
	float mAgingRate;
	float mCurrentAge;
	float mTotalAges;
	int mValuesPerBin;
	int mNumberOfBins;
public:
	MedianBackground( cv::Mat initial_image, float aging_rate, int values_per_bin );
	~MedianBackground();
	cv::Mat GetBackgroundImage();
	void UpdateBackground( cv::Mat current_frame );
	float getAgingRate();
};

#endif