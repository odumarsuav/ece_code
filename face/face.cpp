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


int main(int argc, char * argv[])
{
	VideoCapture capture = VideoCapture(atoi(argv[1]));
	Mat image;

	
	while(true)
	{
		capture >> image;
		while(image.empty())
			capture >> image;

		if(waitKey(10) == -1)
		{
			CascadeClassifier clfier = CascadeClassifier("/home/alex/balloon_tracking/classifier/soda_cascade/cascade.xml");
			vector<Rect> objects;		
			clfier.detectMultiScale(image, objects, 1.1, 3, 0, Size(), Size());
			cout<<objects.front()<<endl;
			Rect rect1 = objects[0];
			rectangle(image,rect1, Scalar( 50, 50, 50), 1,8,0);
			imshow("Mars", image);
		}
  	}
	return 0;
}
