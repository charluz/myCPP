/**
    Theme: Face Detection
    Compiler: Dev C++ 4.9.9.2
    Date: 100/04/26
    Author: ShengWen
    Blog: https://cg2010studio.wordpress.com/
*/


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
//#include <linux/fb.h>
//#include <sys/mman.h>
//#include <sys/ioctl.h>

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
//#include "opencv2/cv.hpp"
#include <iostream>

using namespace std;
using namespace cv;


void detectAndDraw( Mat& img, CascadeClassifier& cascade,
                    /*CascadeClassifier& nestedCascade,*/
                    double scale, bool tryflip );


// the minimum object size
int min_face_height = 50;
int min_face_width = 50;

int main( int argc , char ** argv )
{
    string image_name="./XD2.jpg";

    cout <<"--- A" << endl;

    Mat image = imread(image_name, CV_LOAD_IMAGE_COLOR);

    cout <<"--- B" << endl;

    // Load Haar cascade datgset
    string cascade_name = "./haarcascades/haarcascade_frontalface_default.xml";

    cout <<"--- C" << endl;

    CascadeClassifier cascade;

	if ( !cascade.load(cascade_name) )	{
		cerr << "ERROR: Could not load cascade classifier" << endl;
		return -1;
	}

    cout <<"--- D" << endl;

    if( !image.empty() ) {
        detectAndDraw( image, cascade, /*nestedCascade,*/ 1.0 /*scale*/, 0 /*tryflip*/);
        //waitKey(0);
    }
//
//
//    if (faces) {
//        for (int i=0; i<faces->total; ++i) {
//            // Setup two points that define the extremes of the rectangle,
//            // then draw it to the image
//            CvPoint point1, point2;
//            CvRect* rectangle = (CvRect*)cvGetSeqElem(faces, i);
//            point1.x = rectangle->x;
//            point2.x = rectangle->x + rectangle->width;
//            point1.y = rectangle->y;
//            point2.y = rectangle->y + rectangle->height;
//            cvRectangle(tempFrame, point1, point2, CV_RGB(255,0,0), 3, 8, 0);
//        }
//    }
//    // Save the image to a file
//    cvSaveImage("./result.jpg", tempFrame);
//    // Show the result in the window
//    cvNamedWindow("Face Detection Result", 1);
//    cvShowImage("Face Detection Result", tempFrame);
//    cvWaitKey(0);
//    cvDestroyWindow("Face Detection Result");
//    // Clean up allocated OpenCV objects
//    cvReleaseMemStorage(&facesMemStorage);
//    cvReleaseImage(&tempFrame);
//    cvReleaseHaarClassifierCascade(&cascade);
//    cvReleaseImage(&image_detect);
    return EXIT_SUCCESS;
}



void detectAndDraw( Mat& img, CascadeClassifier& cascade,
                    /*CascadeClassifier& nestedCascade,*/
                    double scale, bool tryflip )
{
    double t = 0;
    vector<Rect> faces, faces2;
    const static Scalar colors[] =
    {
        Scalar(255,0,0),
        Scalar(255,128,0),
        Scalar(255,255,0),
        Scalar(0,255,0),
        Scalar(0,128,255),
        Scalar(0,255,255),
        Scalar(0,0,255),
        Scalar(255,0,255)
    };
    Mat gray, smallImg;

    cvtColor( img, gray, COLOR_BGR2GRAY );
    double fx = 1 / scale;
    resize( gray, smallImg, Size(), fx, fx, INTER_LINEAR );
//imshow("resized", smallImg);
//imwrite("resized.jpg", smallImg);
    equalizeHist( smallImg, smallImg );
//imshow("eqHist", smallImg);
//imwrite("eqHist.jpg", smallImg);
	t = (double)getTickCount();
    cascade.detectMultiScale( smallImg, faces,
        1.3, 3/*2*/, 0
		//|CASCADE_DO_CANNY_PRUNING
        //|CASCADE_FIND_BIGGEST_OBJECT
        //|CASCADE_DO_ROUGH_SEARCH
        |CASCADE_SCALE_IMAGE,
        Size(30, 30) );

//    if ( tryflip ) {
//        flip(smallImg, smallImg, 1);
//        cascade.detectMultiScale( smallImg, faces2,
//                                 1.1, 2, 0
//                                 //|CASCADE_FIND_BIGGEST_OBJECT
//                                 //|CASCADE_DO_ROUGH_SEARCH
//                                 |CASCADE_SCALE_IMAGE,
//                                 Size(30, 30) );
//        for( vector<Rect>::const_iterator r = faces2.begin(); r != faces2.end(); ++r ) {
//            faces.push_back(Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
//        }
//    }
//
    t = (double)getTickCount() - t;
    printf( "detection time = %g ms, Face num = %lu \n", t*1000/getTickFrequency(), faces.size());


    for ( size_t i = 0; i < faces.size(); i++ )
    {
        Rect r = faces[i];
        Mat smallImgROI;
        vector<Rect> nestedObjects;
        Point center;
        Scalar color = colors[i%8];
        int radius;

        double aspect_ratio = (double)r.width/r.height;
//printf("Face[%d] --> aratio= %g \n", i);
        if ( 0.75 < aspect_ratio && aspect_ratio < 1.3 ) {
            center.x = cvRound((r.x + r.width*0.5)*scale);
            center.y = cvRound((r.y + r.height*0.5)*scale);
            radius = cvRound((r.width + r.height)*0.25*scale);
            circle( img, center, radius, color, 2, 8, 0 );
        }
        else {
            rectangle( img, cvPoint(cvRound(r.x*scale), cvRound(r.y*scale)),
                       cvPoint(cvRound((r.x + r.width-1)*scale), cvRound((r.y + r.height-1)*scale)),
                       color, 3, 8, 0);
        }

#if 0
        if ( nestedCascade.empty() ) {
            continue;
        }
        smallImgROI = smallImg( r );
        nestedCascade.detectMultiScale( smallImgROI, nestedObjects,
            1.1, 2, 0
            //|CASCADE_FIND_BIGGEST_OBJECT
            //|CASCADE_DO_ROUGH_SEARCH
            //|CASCADE_DO_CANNY_PRUNING
            |CASCADE_SCALE_IMAGE,
            Size(30, 30) );
        for ( size_t j = 0; j < nestedObjects.size(); j++ )
        {
            Rect nr = nestedObjects[j];
            center.x = cvRound((r.x + nr.x + nr.width*0.5)*scale);
            center.y = cvRound((r.y + nr.y + nr.height*0.5)*scale);
            radius = cvRound((nr.width + nr.height)*0.25*scale);
            circle( img, center, radius, color, 3, 8, 0 );
        }
#endif
    }
cout << "--- X" << endl;
//    imshow( "result", img ); waitKey(0);
    imwrite("/etc/bpi/result.jpg", img);
cout << "--- Y" << endl;
}

