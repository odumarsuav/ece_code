#include "highgui.h"
#include "cv.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char * argv[])
{
    CvCapture* capture = cvCaptureFromCAM(atoi(argv[1]));
    const IplImage* image = cvQueryFrame(capture);
    const IplImage* imagecopy = cvCloneImage(image);
    //CvVideoWriter* writer = cvCreateVideoWriter("video.avi", CV_FOURCC('P','I','M','2'), 100, cvGetSize(imagecopy), 0);
  /* if(writer == NULL)
    {
	perror("writer opening");
	return;
    }*/
    
    while(1)
    {
        const IplImage* image = cvQueryFrame(capture);
	const IplImage* imagecopy = cvCloneImage(image);
	IplImage* filteredimage = cvCloneImage(image);
	cvSobel(imagecopy, filteredimage, 1, 0, 3);
        cvShowImage("Mars", filteredimage);
	//int a = cvWriteFrame(writer, filteredimage);
	cvReleaseImage(&filteredimage);
	//printf("%i\n",a);
	if(cvWaitKey(10) != -1)
	{
	//cvReleaseVideoWriter(&writer);
		break;
	}
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
