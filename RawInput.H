#include "opencv2/opencv.hpp"

#include "cxxopts.hpp"

#include <iostream>
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

double thresholdValue(Mat& image)
{
  int histSize = 256;

  float range[] = { 0, 256 } ;
  const float* histRange = { range };

  bool uniform = true; bool accumulate = false;

  Mat hist;

  calcHist( &image, 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate );

  int hist_w = 512; int hist_h = 400;
  int bin_w = cvRound( (double) hist_w/histSize );

  Mat histImage( hist_h, hist_w, CV_8UC3, Scalar(0,0,0) );

  // Normalize the result to [ 0, histImage.rows ]
  normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

  /// Draw for each channel
  for( int i = 1; i < histSize; i++ )
  {
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ),
                       Scalar( 255, 0, 0), 2, 8, 0  );
  }

  /// Display
  namedWindow("calcHist Demo", WINDOW_AUTOSIZE );
  imshow("calcHist Demo", histImage );

  waitKey(0);

  return 0;
}

namespace Wand {

  class RawInput {
  public:
    // cb(double x, double y, double timestamp)
    typedef function<void(double, double, double)> InputCb;

    RawInput() {
      cout << "RawInput : initialized" << endl;
    };

    void registerCallback( InputCb& cb ) {
      callback = cb;
    }

    void run( bool debug = false ) {
      cout << "RawInput::run" << endl;

      VideoCapture cap(0); // Open the default camera
      if(!cap.isOpened())
        return;

      if ( debug ) namedWindow("Patronus", 1);
      // if ( debug ) namedWindow("Clamped", 1);

      for( ;; ) {

        Mat frame, gray;
        cap >> frame; // get a new frame from camera
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        // Apply a generous Gaussian blur
        Mat blurred;
        GaussianBlur(gray, blurred, Size(69, 69), 0);

        // double thresh = thresholdValue( blurred );

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

              // TODO: Callback timestamp
              cout << "here" << endl;
              callback(r.center.x / frame.cols, r.center.y / frame.rows, 0.);
              cout << "woo" << endl;
            }
          }
        }

        if ( debug ) {
          // Draw our identified lights
          for ( const auto& p : lights ) {
            circle(frame, p, 25, Scalar(255, 0, 0), -1);
          }

          // imshow("Clamped", clamped);
          imshow("Patronus", frame);
        }

        waitKey(30);

      }

    }

  private:
    InputCb callback;
  };

};
