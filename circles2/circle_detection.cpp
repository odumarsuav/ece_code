#include "opencv2/opencv.hpp"
#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>

using namespace std;
using namespace cv;

int main(int argc, char * argv[])
{
	VideoCapture capture = VideoCapture(atoi(argv[1]));
	Mat image;
	capture >> image;//assume blank
	while(image.empty())
		capture>>image;
	imshow("Mars", image);

		
	int key = waitKey(50);

	while(key != 27)//escape key is 27 on Alex's linux box
	{
		
			capture>>image;//clear 5 deep buffer
			capture>>image;
			capture>>image;
			capture>>image;
			capture>>image;

			if(!image.empty())
			{
				Mat gray_image;
				cvtColor( image, gray_image, CV_BGR2GRAY );
				
				Mat blur_image;
				GaussianBlur( gray_image, blur_image, Size(9, 9), 10, 10 );

				Mat thresh_image;
				threshold(blur_image, thresh_image, 127, 255, 0);

				Mat inverted_image;
				bitwise_not(thresh_image, inverted_image);
				
				vector<vector<Point> > contours;
				Mat hierarchy; 
				Mat inverted_image2 = inverted_image.clone();
				findContours(inverted_image2, contours, hierarchy, 1, 2 ,Point());
				for(int i = 0; i < contours.size(); i++)
				{
					vector<Point> cnt = contours[i];
					vector<Point> hull;
					convexHull(cnt, hull, false, true);

					float radius;
					Point2f center;
					minEnclosingCircle(hull,  center,  radius);
					double cir = 3.14*2*((double)radius);
					double per = arcLength(hull,true);

					if (.9 * cir < per   )//threshold for circles
					{

						polylines(image, hull, true, Scalar(0,200,0),10, 8, 0 );
					}	

		
					
				}
				imshow( "Mars", image );
				
  			}
			key = waitKey(50);
	}
	return 0;
}
