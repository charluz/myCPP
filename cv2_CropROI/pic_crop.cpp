//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/gpu/gpu.hpp>
#include <opencv2/opencv.hpp>
//#include <opencv2/tracking.hpp>

#include <stdio.h>
#include <iostream>
#include <string>


using namespace cv;
using namespace std;



//#include <cstdio>
//#include <opencv2/opencv.hpp>
//using namespace cv;


void splitPathFile(const std::string& str, std::string& path, std::string& filename)
{
	//cout << "Splitting: " << str << '\n';
	std::size_t found = str.find_last_of("/\\");
	found = (found == string::npos) ? 0 : found;
	//cout << "found index= " << found << endl;
	path = str.substr(0, found);
	if (!found) {
		filename = str.substr(found);
	}
	else {
		filename = str.substr(found+1);
	}

	//std::cout << " path: " << str.substr(0,found) << '\n';
	//std::cout << " file: " << str.substr(found+1) << '\n';
	//std::cout << " path: " << path << '\n';
	//std::cout << " file: " << filename << '\n';
}


void splitBaseExt(const std::string& str, std::string& base, std::string& ext)
{
	//cout << "Splitting: " << str << '\n';
	std::size_t found = str.find_last_of(".");
	found = (found == string::npos) ? 0 : found;
	//cout << "found index= " << found << endl;
	base = str.substr(0, found);
	ext = str.substr(found+1);

	//std::cout << " path: " << str.substr(0,found) << '\n';
	//std::cout << " file: " << str.substr(found+1) << '\n';
	//std::cout << " base: " << base << '\n';
	//std::cout << " ext: " << ext << '\n';
}

int main(int argc, char **argv)
{
	if (argc < 4) {
		cout << "Usage: " << argv[0] << "image_file widt height" << endl;
		exit(-1);
	}

	std::string imgFile = argv[1];
	int cropW = std::atoi(argv[2]);
	int cropH = std::atoi(argv[3]);


	// Read image
	cout << "Reading image " << imgFile << endl;
	Mat img = imread(imgFile.c_str());
	int imgW = img.cols;	// or img.size().width
	int imgH = img.rows;	// or img.size().height

	cout << "Image size: " << imgW << "x" << imgH << endl;

	// Calculate starting coordinate of the ROI
	int cropX = (imgW - cropW) >> 1;
	int cropY = (imgH - cropH) >> 1;

	cout << "Cropping (x, y, w, h)= " << cropX << ", " << cropY << ", " << cropW << ", " << cropH << endl;

	// Initiate Cropped rectangle
	Rect cropRect(cropX, cropY, cropW, cropH);

	// Format Output filename
	std::string path, filename;
	splitPathFile(imgFile, path, filename);

	std::string base, ext;
	splitBaseExt(filename, base, ext);

	do {
		// Draw a rectangle onto image
		//	Scalar() define color
		//	1, 8, 0 : thickness, line type, shift position
		Mat showImg;
		img.copyTo(showImg);
		rectangle(showImg, cropRect, Scalar(255), 1, 8, 0);
		//namedWindow("Cropped", WINDOW_AUTOSIZE);
		//imshow("Cropped", showImg);
		//waitKey(0);
		imwrite(path+base+"_box."+ext, showImg);
	} while(0);

	Mat cropImg = img(cropRect);
	//imshow("Cropped", cropImg);
	//waitKey(0);
	imwrite(path+base+"_crop."+ext, cropImg);
}
