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
				GaussianBlur( grey_image2, blur_image2, Size(5,5),3, 3);
				Canny( blur_image2, detected_edges, 10, 200, 3 );

				vector<vector<Point> > contours2;
				findContours( detected_edges, contours2, hierarchy2, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
				cvtColor(detected_edges,detected_edges, CV_GRAY2BGR);
				Mat bgr_image2 = bgr_image.clone();

				int skip = 4;
				int continuity = 4;
				for(int i = 0; i < contours2.size(); i++)
				{
					int clockwise_count = 0, counterclockwise_count = 0;
					for(int j = 0; j + 2*skip < contours2[i].size(); j+=2*skip)
					{
						Point2f point_1 = contours2[i][j];
						Point2f point_2 = contours2[i][j + skip];
						Point2f point_3 = contours2[i][j + 2*skip];
						
						int dot_product = 
							(point_3.x - point_2.x) * (point_2.x - point_1.x) + 
							(point_3.y - point_2.y) * (point_2.y - point_1.y) ;
						int magnitude_product = 
						  sqrt( (point_3.x - point_2.x) * (point_3.x - point_2.x) + 
						         (point_3.y - point_2.y) * (point_3.y - point_2.y) ) *
						  sqrt( (point_2.x - point_1.x) * (point_2.x - point_1.x) + 
						         (point_2.y - point_1.y) * (point_2.y - point_1.y) );
						if(dot_product < .2 * magnitude_product || dot_product > .95 * magnitude_product)
						{
							clockwise_count = 0;
							counterclockwise_count = 0;
							continue;
						}

									
				
						if( (point_3.x - point_2.x) * (point_1.y - point_2.y) + 
						    (point_3.y - point_2.y) * (point_2.x - point_1.x) > 0)
						{
							clockwise_count = 0;
							counterclockwise_count++; 
							if(counterclockwise_count == continuity)
								for(int k = j - (continuity - 1) * 2*skip; k <= j; k+=2*skip)
								{	
							line(bgr_image2, contours2[i][k], contours2[i][k+skip], Scalar(00,00,200), 4, 8, 0);
							line(bgr_image2, contours2[i][k+skip], contours2[i][k+2*skip], Scalar(00,00,200), 4, 8, 0);
								}
							else if(counterclockwise_count > continuity)
							line(bgr_image2, contours2[i][j+skip], contours2[i][j+2*skip], Scalar(00,00,200), 4, 8, 0);
						}
						else
						{
							clockwise_count++;
							counterclockwise_count = 0;
							if(clockwise_count == continuity)
								for(int k = j - (continuity-1) * 2*skip; k <= j; k+=2*skip)
								{	
								line(bgr_image2, contours2[i][k], contours2[i][k+skip], Scalar(00,00,200), 4, 8, 0);
								line(bgr_image2, contours2[i][k+skip], contours2[i][k+2*skip], Scalar(00,00,200), 4,8, 0);
								}
							else if(clockwise_count > continuity)
								line(bgr_image2, contours2[i][j+skip], contours2[i][j+2*skip], Scalar(00,00,200), 4, 8, 0);
						}
					}
				}

				imshow("1",bgr_image2);


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
				imshow("2",thresh_image);
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


				imshow( "Mars2", bgr_image);	
  			}
			key = waitKey(1);
	}
	return 0;
}


