//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// ms visual studio 2015
// Jose berengueres
//
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <windows.h>
#include <tchar.h> 
#include <strsafe.h>
#pragma comment(lib, "User32.lib")

using namespace cv;
using namespace std;

bool ListDirectoryContents(const wchar_t *sDir)
{
WIN32_FIND_DATA fdFile;
HANDLE hFind = NULL;

wchar_t sPath[2048];

//Specify a file mask. *.* = We want everything! 
wsprintf(sPath, L"%s\\*.*", sDir);

if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
{
wprintf(L"Path not found: [%s]\n", sDir);
return false;
}

do
{
//Find first file will always return "."
//    and ".." as the first two directories. 
if (wcscmp(fdFile.cFileName, L".") != 0
&& wcscmp(fdFile.cFileName, L"..") != 0)
{
//Build up our file path using the passed in 
//  [sDir] and the file/foldername we just found: 
wsprintf(sPath, L"%s\\%s", sDir, fdFile.cFileName);

//Is the entity a File or Folder? 
if (fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
{
wprintf(L"Directory: %s\n", sPath);
ListDirectoryContents(sPath); //Recursion, I love it! 
}
else {
wprintf(L"File: %s\n", sPath);
}
}
} while (FindNextFile(hFind, &fdFile)); //Find the next file. 

FindClose(hFind); //Always, Always, clean things up! 

return true;
}

int main(int argc, char** argv)
{
WIN32_FIND_DATA fdFile;
HANDLE hFind = NULL;
wchar_t sPath[2048];
const wchar_t sDir[200] = L"data";
//std::wcsncpy(myString, L"Another text", 1024);

namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

int iLowH = 28;
int iHighH = 48;
int iLowS = 0;
int iHighS = 128;
int iLowV = 168;
int iHighV = 255;
int cx = 263;
int cy = 378;

//Create trackbars in "Control" window
cvCreateTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
cvCreateTrackbar("HighH", "Control", &iHighH, 179);
cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
cvCreateTrackbar("HighS", "Control", &iHighS, 255);
//cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
//cvCreateTrackbar("HighV", "Control", &iHighV, 255);
cvCreateTrackbar("cx", "Control", &cx, 555); //Value (0 - 255)
cvCreateTrackbar("cy", "Control", &cy, 555);

RNG rng(12345);

//Specify a file mask. *.* = We want everything! 
wsprintf(sPath, L"%s\\*.JPG", sDir); 

if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
{
wprintf(L"Path not found: [%s]\n", sDir);
return false;
}

do
{
//Find first file will always return "."
//    and ".." as the first two directories. 
if (wcscmp(fdFile.cFileName, L".") != 0
&& wcscmp(fdFile.cFileName, L"..") != 0) {
//Build up our file path using the passed in 
//  [sDir] and the file/foldername we just found: 
wsprintf(sPath, L"%s\\%s", sDir, fdFile.cFileName);

//Is the entity a File or Folder? 
if (fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
{
wprintf(L"Directory: %s\n", sPath);
ListDirectoryContents(sPath); //Recursion, I love it! 
}
else {
//wprintf(L"File: %s\n", sPath);

////////////////////////////////////////////////////////////////////////////////
Mat imgOriginal0;
Mat imgOriginal;
wstring filename(sPath);
string sfilename(filename.begin(), filename.end());
imgOriginal0 = imread(sfilename, CV_LOAD_IMAGE_COLOR);
resize(imgOriginal0, imgOriginal, Size(), 0.25, 0.25, 1);

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

// imshow("Thresholded Image", imgThresholded); //show the thresholded image
// fw("Original", imgOriginal); //show the original image


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
// decided wich cluster of yellow blobs is the ball we want. ignore blobs where centery > cy + 250 or centery < cy -250 
for (int k = 1; k < radius.size(); k++) {
if (radius[maxi] < radius[k] && center[k].y < (cy + 250) && center[k].y > (cy - 250) ) { maxi = k; }
}

circle(imgOriginal, center[maxi], (int)radius[maxi], CV_RGB(255, 255, 255), 2, 8, 0);
putText(imgOriginal, "Jose Berengueres ", cvPoint(30, 30),
FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 255, 255), 1, CV_AA);


circle(imgOriginal, cvPoint(cx, cy), 12, CV_RGB(55, 55, 255), 2, 8, 0); // draw center

putText(imgOriginal, "----", cvPoint(cx, cy + 250),
FONT_HERSHEY_COMPLEX_SMALL, 2.8, cvScalar(255, 255, 255), 1, CV_AA);

putText(imgOriginal, "----", cvPoint(cx, cy - 250),
FONT_HERSHEY_COMPLEX_SMALL, 2.8, cvScalar(255, 255, 255), 1, CV_AA);


putText(imgOriginal, "distance to Goal", cvPoint(cx + 37, cy - 54),
FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 255, 255), 1, CV_AA);

putText(imgOriginal, to_string(sqrt((center[maxi].y - cy)*(center[maxi].y - cy) +
(center[maxi].x - cx)*(center[maxi].x - cx))), cvPoint(cx + 37, cy - 37),
FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 255, 255), 1, CV_AA);

cout << sfilename << "," << to_string(sqrt((center[maxi].y - cy)*(center[maxi].y - cy) +
(center[maxi].x - cx)*(center[maxi].x - cx))) << endl;

}

namedWindow("Contours", CV_WINDOW_AUTOSIZE);
imshow("Contours", imgOriginal);

if (waitKey(5000) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
{
cout << "esc key is pressed by user" << endl;
break;
}
///////////////////////////////////////// END OPENCV PART //////////////////////////
} // else folder
} // first if
} while (FindNextFile(hFind, &fdFile)); //Find the next file. 

FindClose(hFind); //Always, Always, clean things up! 



}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

