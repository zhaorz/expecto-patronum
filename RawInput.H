#include "opencv2/opencv.hpp"
#include "opencv2/video/background_segm.hpp"

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

    const double scale = .5;

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

      cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
      cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

      Ptr<BackgroundSubtractor> pMOG;          // MOG Background subtractor
      pMOG = createBackgroundSubtractorMOG2(); // MOG approach

      Mat frame, gray, blurred, clamped, fg;

      for ( ;; ) {
      // for ( int k = 0; k < 40; k++ ) {
      // for( int thresh = 230 ; thresh < 255; thresh += 3 ) {

        auto start = Clock::now();

        cap >> frame; // get a new frame from camera

        cvtColor(frame, gray, COLOR_BGR2GRAY);

        // Apply a generous Gaussian blur
        GaussianBlur(gray, blurred, Size(15, 15), 0);

        // Apply a threshold to extract very bright pixels
        threshold(blurred, clamped, 250, 255, THRESH_BINARY);

        pMOG->apply(clamped, fg);

        GaussianBlur(fg, blurred, Size(31, 31), 0);
        threshold(blurred, clamped, 240, 255, THRESH_BINARY);

        // Find contours
        vector< vector<Point> > contours;
        vector<Vec4i> hierarchy;
        findContours(clamped, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

        int nDetected = 0;

        // Keep track of 'round enough' contours
        for ( int i = 0; i < contours.size(); i++ ) {

          // TODO: Clean up, fitEllipse needs at least 5 points
          if ( contours[i].size() > 5 ) {
            // Construct bounding ellipse
            RotatedRect r = fitEllipse(contours[i]);

            float e = eccentricity(r);
            // if ( e < 0.8 ) {          // e = 0.8 corresponds to b = 0.6 * a
            if ( e < 0.85 ) {          // e = 0.8 corresponds to b = 0.7 * a
              // Flipping the x coordinate mirrors the movement
              callback((frame.cols - r.center.x) / frame.cols, r.center.y / frame.rows,
                       std::chrono::duration_cast<ms>(Clock::now().time_since_epoch()).count());
              nDetected++;
            }
          }
        }

        auto duration = Clock::now() - start;

        cout << "RawInput : frame processed ("
             << std::chrono::duration_cast<ms>(duration).count() << "ms)"
             << ", nDetected = " << nDetected
             << endl;

        // imwrite("frame-" + std::to_string(k) + ".jpg", frame);
        // imwrite("blurred-" + std::to_string(k) + ".jpg", blurred);
        // imwrite("clamped-" + std::to_string(k) + ".jpg", clamped);
        // imwrite("fg-" + std::to_string(k) + ".jpg", fg);

      }

      exit(0);

    }

  private:
    InputCb callback;
  };

};
