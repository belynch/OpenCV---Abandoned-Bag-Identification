/*
 * This code is provided as part of "A Practical Introduction to Computer Vision with OpenCV"
 * by Kenneth Dawson-Howe © Wiley & Sons Inc. 2014.  All rights reserved.
 */
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/video.hpp"
#include "opencv2/highgui.hpp"
#include <stdio.h>
#include <iostream>
#include "Utilities.h"

using namespace std;
using namespace cv;


Mat manualThreshold(Mat& gray_image, int currentThreshold)
{
	Mat binary_image;
	int current_threshold = 160;
	int max_threshold = 255;
	threshold(gray_image, binary_image, currentThreshold, max_threshold, THRESH_BINARY);
	return binary_image;
}

Mat morphology(Mat& binary_image, int morph_type, int iterations)
{
	Mat result;
	morphologyEx(binary_image, result, morph_type, Mat(), Point(-1, -1), iterations);
	return result;
}

/**
 *
 * Returns true if the given points are within a given range
 *
**/
bool withinRange(Point point1, Point point2, int range){
	bool result = false;
	
	//Calculate Euclidian distance
	double distance = sqrt(((point2.x - point1.x) * (point2.x - point1.x))  + ((point2.y - point1.y)*(point2.y - point1.y)));

	if(distance <= range){
		result = true;
	}

	return result;
}

/**
 *
 * Calculates the foreground image of a frame given a MedianBackground object
 *
**/
Mat medainBackgroundCalculation(MedianBackground &background, Mat &frame){

	Mat background_image, foreground_image;

	background.UpdateBackground( frame );
	background_image = background.GetBackgroundImage();

	Mat median_difference;
	absdiff(background_image, frame, median_difference);
	cvtColor(median_difference, median_difference, CV_BGR2GRAY);
	threshold(median_difference, median_difference, 30, 255, THRESH_BINARY);
	foreground_image.setTo(Scalar(0, 0, 0));
	frame.copyTo(foreground_image, median_difference);

	return foreground_image;
}

/**
 *
 * Identifies the rectangle with the greatest area in a given matrix
 *
**/
Rect largestRectFromMat(Mat &stats, int length){
	Rect largest;
	for(int i = 1; i < length; i++){
		//IDENTIFY BOUNDING BOXES
		int width = stats.at<int>(i,CC_STAT_WIDTH);
		int height = stats.at<int>(i,CC_STAT_HEIGHT);
		Rect boundingBox = Rect(stats.at<int>(i,CC_STAT_LEFT), stats.at<int>(i,CC_STAT_TOP), 
								width, height);

		if((width * height) >= (largest.width * largest.height)){
			largest = boundingBox;
		}
	}
	return largest;
}

/**
 *
 * Converts a bgr image to a binary image, given a threshold
 *
**/
Mat convertToBinary(Mat bgr_image, int threshold){
	Mat greyscale;
	cvtColor(bgr_image, greyscale, CV_BGR2GRAY);

	Mat binary_image;
	binary_image = manualThreshold(greyscale, threshold);

	return binary_image;
}

/**
 *
 * Checks if the current point is within range of the previous point. If so, they are the same object
 * and the occurences value is incremented. If not, they are different objects and the occurences 
 * value is reset.
 *
**/
void consistentObject(Point current_centre, Point previous_centre, int range, int &occurences){
	if(withinRange(current_centre, previous_centre, 5)){
		//circle(result, current_centre, 3, Vec3b(0,0,255));
		occurences++;
	}
	else{
		occurences = 0;
	}
}