#include "opencv2/opencv.hpp"

#include "cxxopts.hpp"

#include <iostream>
#include <cmath>
#include <vector>

using namespace cv;
using std::vector;
using std::cout;
using std::endl;

void drawRotatedRect(Mat& image, const RotatedRect& r, const Scalar& color)
{
  Point2f vertices[4];
  r.points(vertices);

  for (int i = 0; i < 4; i++) {
    line(image, vertices[i], vertices[(i+1)%4], color);
  }
}

// Eccentricity of a perfect square (i.e. bounding rect of a circle) is zero.
float eccentricity(const RotatedRect& r)
{
  float a = r.size.width;
  float b = r.size.height;

  if ( b > a ) std::swap(a, b);

  return std::sqrt(1. - (b * b) / (a * a));
}

int main(int argc, char** argv)
{
  cxxopts::Options options("Patronus", "Detect spells using computer vision");
  options.add_options()
    ("h,help", "Show help")
    ("d,debug", "Enable debugging")
    ;
  auto args = options.parse(argc, argv);

  if ( args.count("h") ) {
    cout << options.help({"", "Group"}) << endl;
    exit(0);
  }

  bool debug = false;
  if ( args.count("d") ) {
    debug = true;
    cout << "Debug enabled" << endl;
  }

  VideoCapture cap(0); // Open the default camera
  if(!cap.isOpened())
    return -1;

  namedWindow("Patronus", 1);

  if ( debug ) namedWindow("Clamped", 1);

  for( ;; ) {

    Mat frame, gray;
    cap >> frame; // get a new frame from camera
    cvtColor(frame, gray, COLOR_BGR2GRAY);

    // Apply a generous Gaussian blur
    Mat blurred;
    GaussianBlur(gray, blurred, Size(99, 99), 0);

    // Apply a threshold to extract very bright pixels
    Mat clamped;
    threshold(blurred, clamped, 248, 255, THRESH_BINARY);

    // Find contours
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(clamped, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

    // Keep track of 'round enough' contours
    vector<Point> lights;
    for ( int i = 0; i < contours.size(); i++ ) {
      drawContours(frame, contours, i, Scalar(0, 0, 255), 2, 8, hierarchy, 0, Point());

      // Construct bounding ellipse
      RotatedRect r = fitEllipse(contours[i]);

      float e = eccentricity(r);
      if ( e < 0.8 ) {          // e = 0.8 corresponds to b = 0.6 * a
        drawRotatedRect(frame, r, Scalar(0, 255, 0));
        lights.push_back(r.center);
      }
    }

    // Draw our identified lights
    for ( const auto& p : lights ) {
      circle(frame, p, 25, Scalar(255, 0, 0), -1);
    }

    if ( debug ) imshow("Clamped", clamped);

    imshow("Patronus", frame);

    if( waitKey(30) >= 0 ) break;

  }

  // the camera will be deinitialized automatically in VideoCapture destructor
  return 0;
}
