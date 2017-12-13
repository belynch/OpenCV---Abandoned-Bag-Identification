#include "Utilities.h"
#include "Video.hpp"
#include <iostream>
#include "VideoData.hpp"

#define RANGE 2
#define NUM_OCCURENCES 33
#define NUM_VIDEOS 2

void analyseVideo(VideoCapture video, string filename, int index);
void performanceTest();
float diceCoefficient(VideoData result, VideoData ground_truth);

VideoCapture* videos = new VideoCapture[NUM_VIDEOS];

vector<string> files;
vector<VideoData> ground_truth;
vector<VideoData> results;


int main(int argc, const char** argv)
{	
	files.push_back("../Media/ObjectAbandonmentAndRemoval1.avi");
	files.push_back("../Media/ObjectAbandonmentAndRemoval2.avi");

	ground_truth.push_back(VideoData(25, 717, 183, 509, Point(356,208), Point(390, 239)));
	ground_truth[0].mBoundingBox = Rect(356,208,34,31); // taken from the above points
	ground_truth.push_back(VideoData(25, 692, 215, 551, Point(287,261), Point(352, 323)));
	ground_truth[1].mBoundingBox = Rect(287,261,65,64); // taken from the above points


	results.push_back(VideoData(25, 0, 0, 0, Point(0,0), Point(0, 0)));
	results.push_back(VideoData(25, 0, 0, 0, Point(0,0), Point(0, 0)));

	for(int i = 0; i < NUM_VIDEOS; i++){
		analyseVideo(videos[i], files[i], i);
	}

	performanceTest();

	cvWaitKey(0);
}

/**
 *
 * Opens the video specified by the filename with a VideCapture object, performs
 * the operations on each frame, and fills the results of the video number 
 * specified by index
 *
**/
void analyseVideo(VideoCapture video, string filename, int index)
{

	cout << "beginning processsing video " + to_string(index) << endl;

	video.open(filename);

	Mat current_frame;
	video >> current_frame;

	double frame_rate = video.get(CV_CAP_PROP_FPS);
	cout << "frame rate: "<< frame_rate << endl;

	results[index].mFps = (int)frame_rate; //result gathering

	double time_between_frames = 25.0/frame_rate;
	cout << "time_between_frames: "<< time_between_frames << endl;

	float aging_rate_fast = 1.05f;
	float aging_rate_slow = 1.005f;

	MedianBackground background_fast( current_frame, aging_rate_fast, 1 );
	MedianBackground background_slow( current_frame, aging_rate_slow, 1 );

	int occurences = 0;
	bool found = false;
	bool found2 = false;


	Point current_centre;
	Point previous_centre;

	int frame_number = 0;
	Rect loc;
	int events = 0;

	while (!current_frame.empty())
    {
		//display every 2nd frame
		if((frame_number % 2) == 0)
		{
			previous_centre = current_centre;

			double duration = static_cast<double>(getTickCount());

			//MEDIAN BACKGROUND
			Mat foreground_image_fast = medainBackgroundCalculation(background_fast, current_frame);
			Mat foreground_image_slow = medainBackgroundCalculation(background_slow, current_frame);
			Mat difference;
			absdiff(foreground_image_slow, foreground_image_fast, difference);

			//CONVERT BGR TO BINARY
			Mat binary_image = convertToBinary(difference, 1);
			//REMOVE NOISE
			Mat image = morphology(binary_image, MORPH_OPEN, 1);

			//CONNECTED COMPONENTS
			Mat labels(image.size(), CV_32S);
			Mat centroids, stats;
			int numLabels = connectedComponentsWithStats(image, labels, stats, centroids);

			Mat result(binary_image.size(), CV_8UC3);
		
			//BOUNDING BOXE PROCESSING

			//ignore first bounding box (whole frame)
			if(numLabels > 1){

				//Identify the largest bounding box
				Rect largest = largestRectFromMat(stats, numLabels);
				rectangle(result, largest, Scalar(255,0,0));
				current_centre = Point(largest.x + largest.width/2, largest.y+largest.height/2);

				//CHECK IF CENTROID REMAINS CONSISTENT ACCROSS MULTIPLE FRAMES
				consistentObject(current_centre, previous_centre, RANGE, occurences);

				//DETECT EVENT
				if(occurences == NUM_OCCURENCES){
					occurences = 0;
					events++;
					cout << "event detected" << endl;

					//BAG FOUND
					if(!found) { 
						found = true;
						loc = largest;
						results[index].mStatic_at = frame_number;  //result gathering
						results[index].mTop_left = Point(largest.x, largest.y);
						results[index].mBottom_right = Point(largest.x + largest.width, largest.y + largest.height);
						results[index].mBoundingBox = loc;
						results[index].mTime1 = frame_number/frame_rate;
					}
					else if(!found2){
						found2 = true;
						results[index].mPicked_up = frame_number;
						results[index].mTime2 = frame_number/frame_rate;
					}
				}
			}
			//draws a rectangle were the object was found
			if(found){
				rectangle(current_frame, loc, Scalar(255,0,0));
			}

		
			imshow("result", result);
			imshow("current frame", current_frame);

			duration = static_cast<double>(getTickCount())-duration;
			duration /= getTickFrequency()/25.0;
			int delay = (time_between_frames>duration) ? ((int) (time_between_frames-duration)) : 1;
			char c = cvWaitKey(delay);
		}

			
		frame_number++;
		video >> current_frame;
	}
	results[index].mTotal_frames = frame_number; //result gathering
	results[index].mEvents = events;

	video.release();
	cout << "processing complete" << endl;
	cout << "There's a bit of a wait before the next section, not entierly sure why\n" << endl;
}

/*
 * Function taken from chapter 8.6.3 of 'A Practical 
 * Introduction to Computer Vision with OpenCV' by
 * Kenneth Dawson-Howe.
 */
void performanceTest()
{
	cout << "beginning performance test" << endl;
	vector<float>coefficients;

	int FP = 0;
	int FN = 0; 
	int TP = 0;
	int TN = 0;

	for (int i=0; i < NUM_VIDEOS; i++){
		VideoData result = results[i];
		VideoData gt = ground_truth[i];

		cout << "Results vs Groundtruth " + to_string(i) << endl;
		cout << "res fps: " << result.mFps << "  gt fps: " << gt.mFps << endl;
		cout << "res frames: " << result.mTotal_frames << "  gt frames: " << gt.mTotal_frames << endl;
		cout << "res static: " << result.mStatic_at << "  gt static: " << gt.mStatic_at << endl;
		cout << "res top: " << result.mTop_left << "  gt top: " << gt.mTop_left << endl;
		cout << "res bottom: " << result.mBottom_right << "  gt bottom: " << gt.mBottom_right << endl;
		cout << "res events: " << result.mEvents << "  gt events: " << gt.mEvents << endl;

		//TP: the number of times an event is detected  which is in the ground truth.
		//FN: the number of times an event is not detected.  
		if(gt.mStatic_at == result.mStatic_at){
			TP++;
		} 
		else{
			FN++;
		} 
		if(gt.mPicked_up == result.mPicked_up){
			TP++;
		}
		else{
			FN++;
		}
			
		//FP: the number of times an event which is not in the ground truth is detected.
		FP = result.mEvents - gt.mEvents;
	
		//calculate dice coefficient
		float dice_coefficient = diceCoefficient(result, gt);
		coefficients.push_back(dice_coefficient);
	}

	double precision = ((double) TP) / ((double) (TP+FP));
	double recall = ((double) TP) / ((double) (TP+FN));

	cout << "\nFalse Positives: " << FP << endl;
	cout << "False Negatives: " << FN  << endl;
	cout << "True Positives: " << TP << endl;
	cout << "precision: " << precision << endl;
	cout << "recall: " << recall << endl;

	//dice coefficient
	float average_dice = 0.0f;
	for (unsigned int i=0; i < coefficients.size(); i++){
		cout << "dice coefficient" + to_string(i) + ": " << coefficients[i] << endl;
		average_dice += coefficients[i];
	}

	average_dice = average_dice / coefficients.size();
	cout << "average dice coefficient: " << average_dice << endl;

	//time for events to be detected
	for (int i=0; i < NUM_VIDEOS; i++){
		cout << "Video " + to_string(i) + ":: event1 time to be detected :: " << results[i].mTime1 << endl;
		cout << "Video " + to_string(i) + ":: event2 time to be detected :: " << results[i].mTime2 << endl;
	}

}

/**
 *
 * Calculates the dice coefficient of two sets
 *
**/
float diceCoefficient(VideoData result, VideoData ground_truth){

	Rect res = result.mBoundingBox;
	Rect gt = ground_truth.mBoundingBox; 
	Rect overlap = res & gt;

	return (float)( 2 * overlap.area() ) / (res.area()+ gt.area());
}