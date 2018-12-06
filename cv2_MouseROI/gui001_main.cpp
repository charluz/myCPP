#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
//#include <opencv2/gpu/gpu.hpp>

#include <stdio.h>

using namespace cv;
using namespace std;



#include <cstdio>
#include <opencv2/opencv.hpp>
using namespace cv;

class RectMouseSelect {
public:
	bool 	isDrawing;
	bool	isRedraw;
	Point	p1, p2;
	Rect 	rectSel	;

public:
	RectMouseSelect(int x=-1, int y=-1, int w=0, int h=0);
	void SetP1(int x, int y);
	void SetP2(int x, int y);
};

RectMouseSelect::RectMouseSelect(int x, int y, int w, int h)
{
	isDrawing = false;
	isRedraw  = false;
	rectSel.x = x;
	rectSel.y = y;
	rectSel.width = w;
	rectSel.height = h;
	p1.x = x;
	p1.y = y;
	p2.x = x + w;
	p2.y = y + h;
}

void RectMouseSelect::SetP1(int x, int y)
{
	p1.x = p2.x = x;
	p1.y = p2.y = y;
	rectSel.x = x;
	rectSel.y = y;
	rectSel.width	= 0;
	rectSel.height	= 0;
	isDrawing = true;
}

void RectMouseSelect::SetP2(int x, int y)
{
	if (!isDrawing) return;

	p2.x = x;
	p2.y = y;

	if (p2.x > p1.x) {
		if (p2.y > p1.y) {
			rectSel.width = p2.x - p1.x;
			rectSel.height = p2.y - p1.y;
			rectSel.x = p1.x;
			rectSel.y = p1.y;
		}
		else {
			rectSel.width = p2.x - p1.x;
			rectSel.height = p1.y - p2.y;
			rectSel.x = p1.x;
			rectSel.y = p2.y;
		}
	}
	else {
		// p2.x <= p1.x
		if (p2.y > p1.y) {
			rectSel.width = p1.x - p2.x;
			rectSel.height = p2.y - p1.y;
			rectSel.x = p2.x;
			rectSel.y = p1.y;
		}
		else {
			rectSel.width = p1.x - p2.x;
			rectSel.height = p1.y - p2.y;
			rectSel.x = p2.x;
			rectSel.y = p2.y;
		}
	}

}

void onMouse(int Event,int x,int y,int flags,void* param);
Point VertexLeftTop(-1,-1);
Point VertexRightDown(-1,-1);

class RectMouseSelect	msSel;

void onMouse(int Event,int x,int y,int flags,void* param){
    if(Event==CV_EVENT_LBUTTONDOWN){
//        VertexLeftTop.x = x;
//        VertexLeftTop.y = y;
//
        msSel.SetP1(x, y);
    }
    if(Event==CV_EVENT_LBUTTONUP){
//        VertexRightDown.x = x;
//        VertexRightDown.y = y;
//
        msSel.SetP2(x, y);
        msSel.isDrawing = false;
        msSel.isRedraw = true;
    }
    if (Event==CV_EVENT_MOUSEMOVE) {
    	msSel.isRedraw =  (msSel.isDrawing) ? true : false;
    	msSel.SetP2(x, y);
    }
}


int main(void)
{
    Mat src = imread("fruits.jpg",CV_LOAD_IMAGE_UNCHANGED);

    namedWindow("image",0);
    setMouseCallback("image",onMouse,NULL);

    imshow("image", src);

    while(true){
    	if (msSel.isRedraw) {
    		Mat canvas = src.clone();
    		rectangle(canvas, msSel.rectSel, Scalar(255,0,0), 2);
    		imshow("image", canvas);
    		msSel.isRedraw = false;
    	}
//
//        if(VertexLeftTop.x==-1 && VertexRightDown.x==-1){
//        	cout << "Redraw" << endl;
//            imshow("image", src);
//        }
//        if(VertexLeftTop.x!=-1 && VertexRightDown.x!=-1){
//        	cout << "-- A --" << endl;
//            rectangle(src, Rect(VertexLeftTop,VertexRightDown),Scalar(255,0,0),2);
//            VertexLeftTop.x = -1;
//            VertexLeftTop.y = -1;
//            VertexRightDown.x = -1;
//            VertexRightDown.y = -1;
//            imshow("image", src);
//        }
        if(cvWaitKey(33)==27){
            break;
        }
    }
    return 0;
}


