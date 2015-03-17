//if object is growing and it's a circle and it's black



/*
So for a simple RGB color image, HE should not be applied individually on each channel. Rather, it should be applied such that intensity values are equalized without disturbing the color balance of the image.
*/




#include "opencv2/opencv.hpp"
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;
using namespace cv;

int main(int argc, char * argv[])
{
	vector<Point2f> old_centers;
	vector<Point2f> new_centers;
	vector<int> centers_newage;
	vector<int> centers_oldage;

	VideoCapture capture = VideoCapture(atoi(argv[1]));
	Mat thresh_image,bgr_image, ycrcb_image, equalizedbgr_image,blur_image,detected_edges;

	
	capture >> bgr_image;//assume blank
	while(bgr_image.empty())
		capture>>bgr_image;


		
	int key = waitKey(50);

	while(key != 27)//escape key is 27 on Alex's linux box
	{
			capture>>bgr_image;//clear 5 deep buffer
			capture>>bgr_image;
			capture>>bgr_image;
			capture>>bgr_image;
			capture>>bgr_image;

			if(!bgr_image.empty())
			{
				cvtColor(bgr_image, ycrcb_image, CV_BGR2YCrCb);
				
				vector<Mat> channels;
			        split(ycrcb_image,channels);

				Ptr<CLAHE> clahe = createCLAHE();
				clahe->setClipLimit(20);
				clahe->setTilesGridSize(Size(4,4));
				clahe->apply(channels[0],channels[0]);


//			        equalizeHist(channels[0], channels[0]);//not adaptive

			        merge(channels,ycrcb_image);

			        cvtColor(ycrcb_image,equalizedbgr_image,CV_YCrCb2BGR);
								Mat hierarchy2; 
				Mat grey_image2,blur_image2,blur_image3;
				cvtColor(equalizedbgr_image, grey_image2, CV_BGR2GRAY);
				GaussianBlur( grey_image2, blur_image2, Size(25,25),3, 3);
				Canny( blur_image2, detected_edges, 10, 100, 3 );
				imshow("after", detected_edges);
				vector<vector<Point> > contours2;
				findContours( detected_edges, contours2, hierarchy2, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1, Point(0, 0) );
		
				Mat bgr_image2 = bgr_image.clone();
				int number_in_vicinity = 0;
				int ellipse_tightness = .1;
				for(int i = 0; i < contours2.size(); i++)
				{
					if(contours2[i].size() >=5){
						//find ellipse
						RotatedRect rec = fitEllipse(contours2[i]);
						ellipse(bgr_image2, rec, Scalar(0,200,0), 3,8);
						//get parameters for traversing ellipse
						double theta = CvBox2D(rec).angle;
						Point center = CvBox2D(rec).center;
						int width = CvBox2D(rec).size.width;
						int height = CvBox2D(rec).size.height;
						//for each point in the ellipse, check 8 neighbors for being points in any curve
						for(int j = 0; j < bgr_image.rows;j++)
							for(int k = 0; k < bgr_image.cols;k++)
							{	
								double a = (j - center.x) * cos(theta) + (k - center.y) * sin(theta);
								double b = (j - center.x) * sin(theta) - (k - center.y) * cos(theta);
								double c = (a * a / width / width + b * b / height / height);
								if(c < 1.1 || c > .9)
								circle(bgr_image2, Point(j,k), 2, Scalar(200, 0 ,0 ) , 2, 8,0);
							}
						
					}
				}
				//find ellipse with most points in a vicinity
			
			

				imshow("1",bgr_image2);
				waitKey(-1);


/*****************************/
				
				vector<Mat> bgr_channels;
				split(equalizedbgr_image,bgr_channels);

				Mat grey_image(equalizedbgr_image.rows, equalizedbgr_image.cols, CV_8UC1);
				for(int i=0; i < equalizedbgr_image.rows;i++)//parameters need to be experimentally defined
					for(int j=0; j < equalizedbgr_image.cols;j++)
					{
						bool black_flag = false;
						//if all colors are too low (black)
						if(bgr_channels[0].at<uchar>(i,j) < 20 && bgr_channels[1].at<uchar>(i,j) < 20 && bgr_channels[2].at<uchar>(i,j) < 20)
							black_flag = true;						
						//else if all colors are similar and below a higher threshold (dark grey)
  						else if(abs(bgr_channels[0].at<uchar>(i,j) - bgr_channels[1].at<uchar>(i,j)) < 20 && abs(bgr_channels[1].at<uchar>(i,j) - bgr_channels[2].at<uchar>(i,j)) < 20 && abs(bgr_channels[2].at<uchar>(i,j) - bgr_channels[0].at<uchar>(i,j)) < 20 && bgr_channels[0].at<uchar>(i,j) < 40 && bgr_channels[1].at<uchar>(i,j) < 40 && bgr_channels[2].at<uchar>(i,j) < 40)
							black_flag = true;
						if(black_flag)
							grey_image.at<uchar>(i,j) = 0;
						else							
							grey_image.at<uchar>(i,j) = 255;
	
					}
				
				//can do this commented code to see blur
				GaussianBlur( grey_image, blur_image, Size(21,21),3, 3);
				threshold(blur_image, thresh_image, 127, 255, 0);
			//	imshow("2",thresh_image);
				//threshold(grey_image, thresh_image, 127, 255, 0);

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
					
					double cir = 3.14159*2*((double)radius);
					double per = arcLength(hull,true);

					if (.9 * cir < per )//threshold for circles
					{
//						polylines(bgr_image, hull, true, Scalar(0,200,0),10, 8, 0 );
						new_centers.push_back(center);
						centers_newage.push_back(0);
						for(int j = 0; j < old_centers.size(); j++)
						{
							if(abs(old_centers.at(j).x - center.x) < 20 && abs(old_centers.at(j).y - center.y) < 20)
							{
								if(centers_oldage.at(j) <5)
								centers_oldage.at(j) = 5;

								if(centers_oldage.at(j) >= 2)
								{
									polylines(bgr_image, hull, true, Scalar(0,200,0),10, 8, 0 );
									cout << 300 / radius  << "ft";//distance metric
									if(center.x < bgr_image.size().width / 2 )
										cout << ", left";
									else
										cout << ", right";
									if(center.y < bgr_image.size().height / 2)
										cout << ", top" << endl;
									else
										cout << ", bottom" << endl;
								}
							}
							else if(centers_oldage.at(j)!=0)
							{
								centers_oldage.at(j) = centers_oldage.at(j)-1;
							}
						}
					}
				}

				old_centers.clear();
				old_centers.swap(new_centers);
				centers_oldage.clear();	
				centers_oldage.swap(centers_newage);


				//imshow( "Mars2", bgr_image);	
  			}
			key = waitKey(1);
	}
	return 0;
}


