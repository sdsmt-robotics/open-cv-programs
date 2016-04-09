
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>

#define AREA_THRESHOLD 500

using namespace std;
using namespace cv;

VideoCapture camera;

bool yellow_object_seen = false;
bool red_object_seen    = false;

Scalar lowRed(46, 81, 255);
Scalar highRed(0, 0, 255);
	
Scalar lowYellow(13, 89, 194);
Scalar highYellow(30, 204, 255);
	
Scalar lowBlue(98, 0, 165);
Scalar highBlue(118, 255, 255);

bool red_object_in_frame()
{
	return red_object_seen;
}

bool yellow_object_in_frame()
{
	return yellow_object_seen;
}


bool init_camera()
{
	camera.open( 1 );

	if( !camera.isOpened() )
	{
		cout << "Cannot open the external camera, trying the internal\n";
		camera.open( 0 );
		return 1;
	}
	return 0;
}

void cleanThresholdedImage(Mat& thresholdImage)
{
        //morphological opening (remove small objects from the foreground)
		erode( thresholdImage, thresholdImage, getStructuringElement(MORPH_ELLIPSE, Size(8, 8)) );
        dilate( thresholdImage, thresholdImage, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        

        //morphological closing (fill small holes in the foreground)
        dilate( thresholdImage, thresholdImage, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        erode( thresholdImage, thresholdImage, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );   
}

Rect largestRectInFrame(vector<Rect> rects)
{ 
	int largest_index = 0;

	for(int i = 0; i < rects.size(); i++)
	{
		if( rects[i].area() > rects[largest_index].area() )
		{
			largest_index = i;
		}
	}
	//cout << largest_index << endl;
	return rects[largest_index];
}

int main(int argc, char **argv)
{

	init_camera();

	namedWindow("Yellow Control", CV_WINDOW_AUTOSIZE);
	namedWindow("Red Control", CV_WINDOW_AUTOSIZE);

	int yellowLowHue = 17;
	int yellowHighHue = 50;
	int yellowLowSaturation = 50;
	int yellowHighSaturation = 255;	
	int yellowLowValue = 70;
	int yellowHighValue = 255;

	int redLowHue = 46;
	int redHighHue = 179;
	int redLowSaturation = 81;
	int redHighSaturation = 255;
	int redLowValue = 68;
	int redHighValue = 255;
	
	Scalar lowRed(46, 81, 255);
	Scalar highRed(0, 0, 255);
	
	Scalar lowYellow(17, 50, 68);
	Scalar highYellow(40, 255, 255);
	
	Scalar lowBlue(98, 0, 165);
	Scalar highBlue(118, 255, 255);

	//Create Track bars in window
	cvCreateTrackbar("Yellow Low Hue", "Yellow Control", &yellowLowHue, 179); //Hue (0 - 179)
	cvCreateTrackbar("Yellow High Hue", "Yellow Control", &yellowHighHue, 179);
	cvCreateTrackbar("Yellow Low Saturation", "Yellow Control", &yellowLowSaturation, 255); //Saturation (0 - 255)
	cvCreateTrackbar("Yellow High Saturation", "Yellow Control", &yellowHighSaturation, 255);
	cvCreateTrackbar("Yellow Low Value", "Yellow Control", &yellowLowValue, 255); //Value (0 - 255)
	cvCreateTrackbar("Yellow High Value", "Yellow Control", &yellowHighValue, 255);

	cvCreateTrackbar("Red Low Hue", "Red Control", &redLowHue, 179); //Hue (0 - 179)
	cvCreateTrackbar("Red High Hue", "Red Control", &redHighHue, 179);
	cvCreateTrackbar("Red Low Saturation", "Red Control", &redLowSaturation, 255); //Saturation (0 - 255)
	cvCreateTrackbar("Red High Saturation", "Red Control", &redHighSaturation, 255);
	cvCreateTrackbar("Red Low Value", "Red Control", &redLowValue, 255); //Value (0 - 255)
	cvCreateTrackbar("Red High Value", "Red Control", &redHighValue, 255);

	Mat cap, frame, threshold, red, yellow;
	bool frame_available;

	while(1)
	{
		frame_available = camera.read(cap);
		if(!frame_available)
		{
			cout << "You broke some stuff, cutting out\n";
			return 1;
		}
		cvtColor(cap, frame, CV_BGR2HSV); //convert to HSV from RGB
	
		//inRange(frame, Scalar(lowHue, lowSaturation, lowValue), Scalar(highHue, highSaturation, highValue), threshold); //threshold that thang for the "threshold" debug window	
		inRange(frame, Scalar(redLowHue, redLowSaturation, redLowValue), Scalar(redHighHue, redHighSaturation, redHighValue), red);	
		inRange(frame, Scalar(yellowLowHue, yellowLowSaturation, yellowLowValue), Scalar(yellowHighHue, yellowHighSaturation, yellowHighValue), yellow);

		//cleanThresholdedImage(threshold);
		cleanThresholdedImage(red);
		cleanThresholdedImage(yellow);

		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		vector<Rect> bounding_rects;

		//basic dealio for the thresholded image
		/*findContours(threshold, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
		for( int i = 0; i < contours.size(); i++ )
		{
			bounding_rects.push_back( boundingRect( Mat(contours[i]) ) );
		}
		if(bounding_rects.size() > 0)
		{
			Rect largest = largestRectInFrame(bounding_rects);
			rectangle( cap, largest, Scalar(150, 127, 200), 1, 8);
			if(largest.area() > AREA_THRESHOLD)
			{
				cout << "Threshold object center at: (" << (largest.x + largest.width/2)
				<< ", " << (largest.y + largest.height/2) << ")" << endl;
			}
		}
		contours.clear();
		hierarchy.clear();
		bounding_rects.clear();*/

		//same kit as above for the yellow
		Mat yellow_copy = yellow.clone();
		findContours(yellow_copy, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
		for( int i = 0; i < contours.size(); i++ )
		{
			bounding_rects.push_back( boundingRect( Mat(contours[i]) ) );
		}
		if(bounding_rects.size() > 0)
		{
			Rect largest = largestRectInFrame(bounding_rects);
			rectangle( yellow, largest, Scalar(175, 255, 255), 2, 8);
			rectangle( cap, largest, Scalar(0, 255, 255), 2, 8);
			if(largest.area() > AREA_THRESHOLD)
			{	
				cout << "Yellow object center at: (" << (largest.x + largest.width/2) 
				<< ", " << (largest.y + largest.height/2) << ")" << endl;
			}
		}
		contours.clear();
		hierarchy.clear();
		bounding_rects.clear();

		//same thing for the red
		Mat red_copy = red.clone();
		findContours(red_copy, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
		for( int i = 0; i < contours.size(); i++ )
		{
			bounding_rects.push_back( boundingRect( Mat(contours[i]) ) );
		}
		if(bounding_rects.size() > 0)
		{
			Rect largest = largestRectInFrame(bounding_rects);
			rectangle( red, largest, Scalar(175, 255, 255), 2, 8);
			rectangle( cap, largest, Scalar(0, 0, 255), 2, 8);
			if(largest.area() > AREA_THRESHOLD)
			{
				cout << "Red object center at: (" << (largest.x + largest.width/2)
				<< ", " << (largest.y + largest.height/2) << ")" << endl;
			}
		}
		contours.clear();
		hierarchy.clear();
		bounding_rects.clear();

		imshow("cap", cap);
		imshow("yellow", yellow);
		imshow("red", red);


		waitKey(33);
	}

	return 0;
}