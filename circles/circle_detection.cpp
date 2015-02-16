#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;

/** @function main */
int main(int argc, char** argv)
{
  Mat src, src_gray, dst;

  /// Read the image
  src = imread( argv[1], 1 );

  if( !src.data )
    { return -1; }

  /// Convert it to gray
  cvtColor( src, src_gray, CV_BGR2GRAY );

GaussianBlur( src_gray, src_gray, Size(9, 9), 10, 10 );



  threshold(src_gray, dst, 127, 255, 0);

bitwise_not(dst, dst);

  vector<vector<Point> > contours;

  Mat hierarchy; 

Mat dst2 = dst.clone();
 findContours(dst, contours, hierarchy, 1, 2 ,Point());

for(int i = 0; i < contours.size(); i++)
{
vector<Point> cnt = contours[i];

vector<Point> hull;


convexHull(cnt, hull, false, true );
polylines(src, hull, true, Scalar(200,0,0),1, 8, 0 );

float radius;
Point2f center;
minEnclosingCircle(hull,  center,  radius);
if (arcLength(hull,true) >= 1.5*3.14*radius)

polylines(src, hull, true, Scalar(0,200,0),1, 8, 0 );

}

imshow( "Hough Circle Transform Demo", src );
  
  waitKey(0);
  return 0;
}
