#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>

using namespace std;
using namespace cv;

struct frame
{
	Mat image;
	String name = "";
};

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

	VideoCapture cam( 1 );
	if( !cam.isOpened() )
	{
		cout << "Cannot open the external camera, trying the internal\n";
		cam.open( 0 );
	}

	namedWindow("Control", CV_WINDOW_AUTOSIZE);

	int lowHue = 17;
	int highHue = 40;
	
	int lowSaturation = 81;
	int highSaturation = 255;
	
	int lowValue = 70;
	int highValue = 199;
	
	Scalar lowRed(46, 81, 255);
	Scalar highRed(0, 0, 255);
	
	Scalar lowYellow(13, 89, 194);
	Scalar highYellow(30, 204, 255);
	
	Scalar lowBlue(98, 0, 165);
	Scalar highBlue(118, 255, 255);

	//Create Track bars in window
	cvCreateTrackbar("Low Hue", "Control", &lowHue, 255); //Hue (0 - 179)
	cvCreateTrackbar("High Hue", "Control", &highHue, 255);
	cvCreateTrackbar("Low Saturation", "Control", &lowSaturation, 255); //Saturation (0 - 255)
	cvCreateTrackbar("High Saturation", "Control", &highSaturation, 255);
	cvCreateTrackbar("Low Value", "Control", &lowValue, 255); //Value (0 - 255)
	cvCreateTrackbar("High Value", "Control", &highValue, 255);

	Mat cap, frame, threshold, red, yellow;
	bool frame_available;

	while(1)
	{
		frame_available = cam.read(cap);
		if(!frame_available)
		{
			cout << "You broke some stuff, cutting out\n";
			return 1;
		}
		cvtColor(cap, frame, CV_BGR2HSV); //convert to HSV from RGB
	
		inRange(frame, Scalar(lowHue, lowSaturation, lowValue), Scalar(highHue, highSaturation, highValue), threshold); //threshold that thang for the "threshold" debug window	
		inRange(frame, lowRed, highRed, red);	
		inRange(frame, lowYellow, highYellow, yellow);

		cleanThresholdedImage(threshold);
		cleanThresholdedImage(red);
		cleanThresholdedImage(yellow);

		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		vector<Rect> bounding_rects;


		findContours(threshold, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
		for( int i = 0; i < contours.size(); i++ )
		{
			bounding_rects.push_back( boundingRect( Mat(contours[i]) ) );
		}
		if(bounding_rects.size() > 0)
		{
			Rect largest = largestRectInFrame(bounding_rects);
			rectangle( cap, largest, Scalar(150, 127, 200), 1, 8);
		}
		contours.clear();
		hierarchy.clear();
		bounding_rects.clear();

		imshow("cap", cap);


		waitKey(33);
	}

	return 0;
}