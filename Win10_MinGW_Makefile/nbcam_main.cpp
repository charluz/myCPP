#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>

using namespace cv;
using namespace std;

cv::VideoCapture cam;

int main(void)
{
	cv::Mat frame;
	cam.open(0);
	if (!cam.isOpened()) { //check if video device has been initialized
		cout << "cannot open camera !!";
	}

	while (1) {
		// get a new frame from camera - this is non-blocking per spec
		cam >> frame;
//		if (!cam.grab()) continue;
		if (!frame.empty()) {
			cout << "frame empty ..." << endl;
		}
		cv::imshow("cam", frame);

		waitKey(30);
	}
}
