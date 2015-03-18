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

RotatedRect fitEllipse_edit( InputArray _points , double & error);
CvBox2D cvFitEllipse_edit( const CvArr* array,double & error );

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

				vector<vector<Point> > contours2;
				findContours( detected_edges, contours2, hierarchy2, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1, Point(0, 0) );
		
				Mat bgr_image2 = bgr_image.clone();
				for(int i = 0; i < contours2.size(); i++)
				{
					if(contours2[i].size() >=10){
						//find ellipse
						double error;
						RotatedRect rec = fitEllipse_edit(contours2[i], error);
						if(abs(rec.size.width - rec.size.height) < .2 * rec.size.width)
						{	
							cout << error << " " << contours2[i].size() << " " << error / contours2[i].size()<<endl;
							ellipse(bgr_image2, rec, Scalar(0,200,0), 3,8);
						}

					}
				}
				//find ellipse with most points in a vicinity
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


				imshow( "Mars2", bgr_image);	
  			}
			key = waitKey(1);
	}
	return 0;
}

RotatedRect fitEllipse_edit( InputArray _points , double & error)
{
    Mat points = _points.getMat();
    CV_Assert(points.checkVector(2) >= 0 &&
              (points.depth() == CV_32F || points.depth() == CV_32S));
    CvMat _cpoints = points;
    return cvFitEllipse_edit(&_cpoints, error);
}


CvBox2D cvFitEllipse_edit( const CvArr* array , double & error)
{
    CvBox2D box;
    cv::AutoBuffer<double> Ad, bd;
    memset( &box, 0, sizeof(box));

    CvContour contour_header;
    CvSeq* ptseq = 0;
    CvSeqBlock block;
    int n;

    if( CV_IS_SEQ( array ))
    {
        ptseq = (CvSeq*)array;
        if( !CV_IS_SEQ_POINT_SET( ptseq ))
            CV_Error( CV_StsBadArg, "Unsupported sequence type" );
    }
    else
    {
        ptseq = cvPointSeqFromMat(CV_SEQ_KIND_GENERIC, array, &contour_header, &block);
    }

    n = ptseq->total;
    if( n < 5 )
        CV_Error( CV_StsBadSize, "Number of points should be >= 5" );

    /*
     *  New fitellipse algorithm, contributed by Dr. Daniel Weiss
     */
    CvPoint2D32f c = {0,0};
    double gfp[5], rp[5], t;
    CvMat A, b, x;
    const double min_eps = 1e-8;
    int i, is_float;
    CvSeqReader reader;

    Ad.allocate(n*5);
    bd.allocate(n);

    // first fit for parameters A - E
    A = cvMat( n, 5, CV_64F, Ad );
    b = cvMat( n, 1, CV_64F, bd );
    x = cvMat( 5, 1, CV_64F, gfp );

    cvStartReadSeq( ptseq, &reader );
    is_float = CV_SEQ_ELTYPE(ptseq) == CV_32FC2;

    for( i = 0; i < n; i++ )
    {
        CvPoint2D32f p;
        if( is_float )
            p = *(CvPoint2D32f*)(reader.ptr);
        else
        {
            p.x = (float)((int*)reader.ptr)[0];
            p.y = (float)((int*)reader.ptr)[1];
        }
        CV_NEXT_SEQ_ELEM( sizeof(p), reader );
        c.x += p.x;
        c.y += p.y;
    }
    c.x /= n;
    c.y /= n;

    for( i = 0; i < n; i++ )
    {
        CvPoint2D32f p;
        if( is_float )
            p = *(CvPoint2D32f*)(reader.ptr);
        else
        {
            p.x = (float)((int*)reader.ptr)[0];
            p.y = (float)((int*)reader.ptr)[1];
        }
        CV_NEXT_SEQ_ELEM( sizeof(p), reader );
        p.x -= c.x;
        p.y -= c.y;

        bd[i] = 10000.0; // 1.0?
        Ad[i*5] = -(double)p.x * p.x; // A - C signs inverted as proposed by APP
        Ad[i*5 + 1] = -(double)p.y * p.y;
        Ad[i*5 + 2] = -(double)p.x * p.y;
        Ad[i*5 + 3] = p.x;
        Ad[i*5 + 4] = p.y;
    }

    cvSolve( &A, &b, &x, CV_SVD );

    // now use general-form parameters A - E to find the ellipse center:
    // differentiate general form wrt x/y to get two equations for cx and cy
    A = cvMat( 2, 2, CV_64F, Ad );
    b = cvMat( 2, 1, CV_64F, bd );
    x = cvMat( 2, 1, CV_64F, rp );
    Ad[0] = 2 * gfp[0];
    Ad[1] = Ad[2] = gfp[2];
    Ad[3] = 2 * gfp[1];
    bd[0] = gfp[3];
    bd[1] = gfp[4];
    cvSolve( &A, &b, &x, CV_SVD );

    // re-fit for parameters A - C with those center coordinates
    A = cvMat( n, 3, CV_64F, Ad );
    b = cvMat( n, 1, CV_64F, bd );
    x = cvMat( 3, 1, CV_64F, gfp );
    for( i = 0; i < n; i++ )
    {

        CvPoint2D32f p;
        if( is_float )
            p = *(CvPoint2D32f*)(reader.ptr);
        else
        {
            p.x = (float)((int*)reader.ptr)[0];
            p.y = (float)((int*)reader.ptr)[1];
        }
        CV_NEXT_SEQ_ELEM( sizeof(p), reader );
        p.x -= c.x;
        p.y -= c.y;
        bd[i] = 1.0;
        Ad[i * 3] = (p.x - rp[0]) * (p.x - rp[0]);
        Ad[i * 3 + 1] = (p.y - rp[1]) * (p.y - rp[1]);
        Ad[i * 3 + 2] = (p.x - rp[0]) * (p.y - rp[1]);
    }
    cvSolve(&A, &b, &x, CV_SVD);


	const CvMat* A_const = &A;
	const CvMat* b_const = 	&b;
	const CvMat* x_const = &x;
	Mat A_edit(A_const,false);
	Mat x_edit(x_const,false);
	Mat b_edit(b_const,false);
   error = norm(A_edit * x_edit - b_edit);

    // store angle and radii
    rp[4] = -0.5 * atan2(gfp[2], gfp[1] - gfp[0]); // convert from APP angle usage
    t = sin(-2.0 * rp[4]);
    if( fabs(t) > fabs(gfp[2])*min_eps )
        t = gfp[2]/t;
    else
        t = gfp[1] - gfp[0];
    rp[2] = fabs(gfp[0] + gfp[1] - t);
    if( rp[2] > min_eps )
        rp[2] = sqrt(2.0 / rp[2]);
    rp[3] = fabs(gfp[0] + gfp[1] + t);
    if( rp[3] > min_eps )
        rp[3] = sqrt(2.0 / rp[3]);

    box.center.x = (float)rp[0] + c.x;
    box.center.y = (float)rp[1] + c.y;
    box.size.width = (float)(rp[2]*2);
    box.size.height = (float)(rp[3]*2);
    if( box.size.width > box.size.height )
    {
        float tmp;
        CV_SWAP( box.size.width, box.size.height, tmp );
        box.angle = (float)(90 + rp[4]*180/CV_PI);
    }
    if( box.angle < -180 )
        box.angle += 360;
    if( box.angle > 360 )
        box.angle -= 360;

    return box;
}


