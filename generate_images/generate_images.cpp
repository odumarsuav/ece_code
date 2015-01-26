#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
 #include "opencv2/objdetect/objdetect.hpp"
 #include "opencv2/highgui/highgui.hpp"
 #include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include "highgui.h"
using namespace std;
using namespace cv;

int image_number = 1;

void write_image(Mat image)
{
	vector<int> compression_params;
        compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(3);
	ostringstream oss;
	oss << "image" << image_number << ".png";
	imwrite(oss.str(),image,compression_params);
	image_number += 1;
}

int main(int argc, char * argv[])
{
	VideoCapture capture = VideoCapture(atoi(argv[1]));
	Mat image;
	capture >> image;//assume blank
	while(image.empty())
		capture>>image;
	imshow("Enter to take Picture/Escape To Quit", image);
	int key = waitKey(50);
	while(key != 27)//escape key is 27 on Alex's linux box
	{
		if(key < 0)//no key pressed
		{
			capture>>image;//clear 5 deep buffer
			capture>>image;
			capture>>image;
			capture>>image;
			capture>>image;
			imshow("Enter to take Picture/Escape To Quit", image);			
		}
		else//key pressed
		{
			capture>>image;//clear 5 deep buffer
			capture>>image;
			capture>>image;
			capture>>image;
			capture>>image;
			imshow("Enter to take Picture/Escape To Quit", image);
			write_image(image);
		}
		key = waitKey(50);
	}
	return 0;
}
