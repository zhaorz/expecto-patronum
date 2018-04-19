#include "opencv2/opencv.hpp"

#include "cxxopts.hpp"

#include <iostream>
#include <chrono>
#include <cmath>
#include <functional>
#include <vector>

using namespace cv;
using std::function;
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

namespace Wand {

  struct RawInputEvent {
    double x;
    double y;
    long t;
  };

  class RawInput {
  public:
    // cb(double x, double y, double timestamp)
    typedef function<void(double, double, long)> InputCb;

    typedef std::chrono::milliseconds ms;
    typedef std::chrono::high_resolution_clock Clock;

    RawInput() {
      cout << "RawInput : initialized" << endl;
    };

    void registerCallback( InputCb& cb ) {
      callback = cb;
    }

    void run () {
      VideoCapture cap(0); // Open the default camera
      if ( !cap.isOpened() ) {
        cout << "RawInput : Failed to open camera, aborting" << endl;
        exit(1);
      }

      for( ;; ) {

        Mat frame, gray;
        cap >> frame; // get a new frame from camera
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        // Apply a generous Gaussian blur
        Mat blurred;
        GaussianBlur(gray, blurred, Size(69, 69), 0);

        // Apply a threshold to extract very bright pixels
        Mat clamped;
        threshold(blurred, clamped, 252, 255, THRESH_BINARY);

        // Find contours
        vector< vector<Point> > contours;
        vector<Vec4i> hierarchy;
        findContours(clamped, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

        // Keep track of 'round enough' contours
        vector<Point> lights;
        for ( int i = 0; i < contours.size(); i++ ) {
          drawContours(frame, contours, i, Scalar(0, 0, 255), 2, 8, hierarchy, 0, Point());

          // TODO: Clean up, fitEllipse needs at least 5 points
          if ( contours[i].size() > 5 ) {
            // Construct bounding ellipse
            RotatedRect r = fitEllipse(contours[i]);

            float e = eccentricity(r);
            // if ( e < 0.8 ) {          // e = 0.8 corresponds to b = 0.6 * a
            if ( e < 0.85 ) {          // e = 0.8 corresponds to b = 0.7 * a
              drawRotatedRect(frame, r, Scalar(0, 255, 0));
              lights.push_back(r.center);

              // Flipping the x coordinate mirrors the movement
              callback((frame.cols - r.center.x) / frame.cols, r.center.y / frame.rows,
                       std::chrono::duration_cast<ms>(Clock::now().time_since_epoch()).count());
            }
          }
        }

      }

    }

  private:
    InputCb callback;
  };

};
