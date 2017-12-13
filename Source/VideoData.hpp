#ifndef VIDEO_DATA_HPP
#define VIDEO_DATA_HPP

class VideoData{

public:

	int mFps;
	int mTotal_frames;
	int mStatic_at;
	int mPicked_up;
	int mEvents;
	double mTime1;
	double mTime2;
	Rect mBoundingBox;
	Point mTop_left;
	Point mBottom_right;

	VideoData(int fps, int total_frames, int static_at, int picked_up, Point top_left, Point bottom_right) 
		: mFps(fps), mTotal_frames(total_frames), mStatic_at(static_at), mPicked_up(picked_up), mEvents(2), 
			mTop_left(top_left), mBottom_right(bottom_right) {}

	bool operator==(const VideoData &other) 
	{
		 return (mFps == other.mFps &&
				mTotal_frames == other.mTotal_frames &&
				mStatic_at == other.mStatic_at &&
				mPicked_up == other.mPicked_up &&
				mTop_left == other.mTop_left &&
				mBottom_right == other.mBottom_right);
	}

};

#endif