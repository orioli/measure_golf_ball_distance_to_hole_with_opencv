//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// ms visual studio 2015
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
VideoCapture cap(0); //capture the video from web cam

if (!cap.isOpened())  // if not success, exit program
{
cout << "Cannot open the web cam" << endl;
return -1;
}

namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

int iLowH = 0;
int iHighH = 179;

int iLowS = 0;
int iHighS = 255;

int iLowV = 0;
int iHighV = 255;

//Create trackbars in "Control" window
cvCreateTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
cvCreateTrackbar("HighH", "Control", &iHighH, 179);

cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
cvCreateTrackbar("HighS", "Control", &iHighS, 255);

cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
cvCreateTrackbar("HighV", "Control", &iHighV, 255);

RNG rng(12345);

while (true)
{
Mat imgOriginal;

bool bSuccess = cap.read(imgOriginal); // read a new frame from video

if (!bSuccess) //if not success, break loop
{
cout << "Cannot read a frame from video stream" << endl;
break;
}

Mat imgHSV;

cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

Mat imgThresholded;

inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

 //morphological opening (remove small objects from the foreground)
erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

//morphological closing (fill small holes in the foreground)
dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

imshow("Thresholded Image", imgThresholded); //show the thresholded image
imshow("Original", imgOriginal); //show the original image
// Detect blobs.
vector<vector<Point> > contours;
vector<Vec4i> hierarchy;
findContours(imgThresholded, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
vector<vector<Point> > contours_poly(contours.size());
vector<Rect> boundRect(contours.size());
vector<Point2f>center(contours.size());
vector<float>radius(contours.size());
if (contours.size() > 0) {
for (int i = 0; i < contours.size(); i++)
{
approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
boundRect[i] = boundingRect(Mat(contours_poly[i]));
minEnclosingCircle((Mat)contours_poly[i], center[i], radius[i]);
}


/// Draw circles
Mat drawing = Mat::zeros(imgThresholded.size(), CV_8UC3);
int maxi = 0;
for (int k = 1; k < radius.size(); k++) {
if (radius[maxi] < radius[k]) { maxi = k; }
}

circle(imgOriginal, center[maxi], (int)radius[maxi], CV_RGB(255, 255, 255), 2, 8, 0);
putText(imgOriginal, "Jose Berengueres ", cvPoint(30, 30),
FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 255, 255), 1, CV_AA);

int cx = imgOriginal.cols/2;
int cy = imgOriginal.rows/2;

putText(imgOriginal, "+", cvPoint(cx,cy),
FONT_HERSHEY_COMPLEX_SMALL, 2.8, cvScalar(255, 255, 255), 1, CV_AA);

putText(imgOriginal, "distance to Goal", cvPoint(cx+37, cy-54),
FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 255, 255), 1, CV_AA);

putText(imgOriginal, to_string(sqrt((center[maxi].y - cy)*(center[maxi].y - cy) +
(center[maxi].x - cx)*(center[maxi].x - cx))), cvPoint(cx + 37, cy - 37),
FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 255, 255), 1, CV_AA);


}

namedWindow("Contours", CV_WINDOW_AUTOSIZE);
imshow("Contours", imgOriginal);

if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
{
cout << "esc key is pressed by user" << endl;
break;
}
}

return 0;

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
