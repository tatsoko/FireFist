
#include <stdio.h>
#include <iostream>


#include <opencv2/opencv.hpp> 
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/video/background_segm.hpp>
using namespace cv;
using namespace std;

#define WIDTH 640
#define HEIGHT 480

typedef vector<Point> contour_t;
typedef vector<contour_t> contour_vector_t;

static void on_trackbar(int pos, void* slider_value) {
	*((int*)slider_value) = pos;
}

int main()
{

	Mat frame;
	VideoCapture cap(0);

	if (!cap.isOpened()) {
		cout << "No webcam, using video file" << endl;
		cap.open("MarkerMovie.MP4");
		if (cap.isOpened() == false) {
			cout << "No video!" << endl;
			exit(0);
		}
	}

	const string contoursWindow = "FireFist";
	const string UI = "Threshold";
	namedWindow(contoursWindow, WINDOW_NORMAL);
	cv::resizeWindow("FireFist", WIDTH, HEIGHT);
	int slider_value = 80; //186
	createTrackbar(UI, contoursWindow, &slider_value, 255, on_trackbar, &slider_value);


	Mat imgFiltered;

	while (cap.read(frame)) {
		Mat grayScale;
		imgFiltered = frame.clone();
		cvtColor(frame, grayScale, COLOR_BGR2GRAY);
		blur(grayScale, grayScale, Size(12, 12));
		if (slider_value == 0) {
			adaptiveThreshold(grayScale, grayScale, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 33, 5);
		}
		else {
			threshold(grayScale, grayScale, slider_value, 255, THRESH_BINARY);
		}
		

		contour_vector_t contours;

		findContours(grayScale, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

		for (size_t k = 0; k < contours.size(); k++) {

			contour_t approx_contour;
			approxPolyDP(contours[k], approx_contour, arcLength(contours[k], true) * 0.02, true);
			contour_vector_t cov, aprox;
			cov.emplace_back(contours[k]);
			aprox.emplace_back(approx_contour);
			if (approx_contour.size() > 1) {
				drawContours(grayScale, cov, -1, Scalar(0, 255, 0), 4, 1);
				drawContours(grayScale, aprox, -1, Scalar(255, 0, 0), 4, 1);
				continue;
			}
		}
		imshow(contoursWindow, grayScale);

		if (waitKey(10) == 27) {
			break;
		}
	}

	destroyWindow(contoursWindow);

	return(0);

}

