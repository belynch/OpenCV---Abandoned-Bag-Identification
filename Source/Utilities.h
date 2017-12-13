/*
 * This code is provided as part of "A Practical Introduction to Computer Vision with OpenCV"
 * by Kenneth Dawson-Howe © Wiley & Sons Inc. 2014.  All rights reserved.
 */
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/video.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include <stdio.h>
#include <iostream>
#include <iostream>
#include "Video.hpp"

using namespace std;
using namespace cv;

Mat manualThreshold(Mat& gray_image, int currentThreshold);
Mat morphology(Mat& binary_image, int morph_type, int iterations);
bool withinRange(Point point1, Point point2, int range);
Mat medainBackgroundCalculation(MedianBackground &background, Mat &frame);
Rect largestRectFromMat(Mat &stats, int length);
Mat convertToBinary(Mat bgr_image, int threshold);
void consistentObject(Point current_centre, Point previous_centre, int range, int &occurences);
