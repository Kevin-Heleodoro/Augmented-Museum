/**
 * Author: Kevin Heleodoro
 * Date: April 3, 2024
 * Purpose: Entrypoint for the Augmented Museum application. This
 * program uses OpenCV to detect ArUco markers and overlay images on top of
 * them. Users can pass in a directory of images to cycle through different
 * displays.
 */

#include <filesystem>
#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

#include "../include/ar_utils.h"
#include "../include/cmdparser.hpp"

using namespace std;
using namespace cv;

namespace fs = std::__fs::filesystem;

Mat camMatrix, dCoeffs;
vector<Mat> rotationVectors, translationVectors;
vector<Mat> images;

/**
 * @brief Configures the parameters being passed in through the command line.
 */
void
configureParser(cli::Parser& parser)
{
  parser.set_optional<string>(
    "p",
    "path",
    "bin/paintings",
    "Set path for directory containing images. Defaults to bin/paintings "
    "directory which contains a handful of assorted artworks.");

  parser.set_optional<string>("c",
                              "calibration",
                              "bin/calibration.xml",
                              "Path to camera calibration file");

  parser.set_optional<bool>(
    "a", "aruco", false, "If true, creates an ArUco marker and saves it");
}

/**
 * @brief Overlay a painting onto an ArUco marker
 */
void
overlayImage2(const Mat& src,
              Mat& dest,
              const Mat& overlay,
              const vector<Point2f>& markerCorners,
              const Vec3d& rvec,
              const Vec3d& tvec,
              const Mat& camMatrix,
              const Mat& dCoeffs)
{
  vector<Point3f> objectPoints = { Point3f(-overlay.cols, overlay.rows, 0),
                                   Point3f(overlay.cols, overlay.rows, 0),
                                   Point3f(overlay.cols, -overlay.rows, 0),
                                   Point3f(-overlay.cols, -overlay.rows, 0) };

  vector<Point2f> imagePoints;
  projectPoints(objectPoints, rvec, tvec, camMatrix, dCoeffs, imagePoints);

  vector<Point2f> overlayPoints = { Point2f(0, 0),
                                    Point2f(overlay.cols, 0),
                                    Point2f(overlay.cols, overlay.rows),
                                    Point2f(0, overlay.rows) };
  Mat homography = findHomography(overlayPoints, imagePoints);
  if (homography.empty()) {
    cerr << "Failed to compute homography matrix." << endl;
    return;
  }

  Mat warpedOverlay;
  warpPerspective(
    overlay,
    warpedOverlay,
    homography,
    src.size()); // This should be the size of the output image instead of src.

  Mat overlayMask = Mat::zeros(src.size(), CV_8UC1);
  vector<Point> overlayPolygon;
  for (const Point2f& p : imagePoints) {
    overlayPolygon.push_back(
      Point(static_cast<int>(p.x), static_cast<int>(p.y)));
  }
  fillConvexPoly(
    overlayMask, overlayPolygon.data(), overlayPolygon.size(), Scalar(255));

  Mat backgroundMask;
  bitwise_not(overlayMask, backgroundMask);

  Mat background;
  src.copyTo(background, backgroundMask);
  bitwise_and(warpedOverlay, warpedOverlay, overlayMask);

  add(background, warpedOverlay, dest);
}

/**
 * @brief Runs OpenCV's logic for detecting ArUco markers and overlays an image
 * on top of it. This function is intended for only 1 ArUco marker.
 */
void
detectAndOverlayMarker(Mat& src, Mat& dest, Mat& overlay, Mat& objPoints)
{
  int markerSize = 200;
  vector<int> markerIds;
  vector<vector<Point2f>> markerCorners, rejectedCandidates;

  aruco::Dictionary dictionary =
    aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
  aruco::DetectorParameters detectorParams = aruco::DetectorParameters();
  aruco::ArucoDetector detector(dictionary, detectorParams);

  detector.detectMarkers(src, markerCorners, markerIds);
  size_t nMarkers = markerCorners.size();
  vector<Vec3d> rvecs(nMarkers), tvecs(nMarkers);

  if (!markerIds.empty()) {

    for (size_t i = 0; i < nMarkers; i++) {
      solvePnP(objPoints,
               markerCorners.at(i),
               camMatrix,
               dCoeffs,
               rvecs.at(i),
               tvecs.at(i),
               SOLVEPNP_ITERATIVE);
      // drawFrameAxes(
      //   dest, camMatrix, dCoeffs, rvecs[i], tvecs[i], markerSize * 0.5f);
      overlayImage2(src,
                    dest,
                    overlay,
                    markerCorners[i],
                    rvecs[i],
                    tvecs[i],
                    camMatrix,
                    dCoeffs);
    }
  }
}

/**
 * @brief Runs OpenCV's logic for detecting ArUco markers and overlays an image
 * on top of them. This function is intended for multiple ArUco markers.
 */
void
detectAndOverlayMultipleMarkers(Mat& src, Mat& dest, Mat& objPoints)
{
  vector<int> markerIds;
  vector<vector<Point2f>> markerCorners;
  aruco::Dictionary dictionary =
    aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
  aruco::DetectorParameters detectorParams = aruco::DetectorParameters();
  aruco::ArucoDetector detector(dictionary, detectorParams);
  detector.detectMarkers(src, markerCorners, markerIds);

  // ====
  size_t nMarkers = markerCorners.size();
  vector<Vec3d> rvecs(nMarkers), tvecs(nMarkers);
  // ====

  for (size_t i = 0; i < markerIds.size(); ++i) {
    int idx = markerIds[i] % images.size();
    Mat overlay = images[idx];

    // ====
    solvePnP(objPoints,
             markerCorners.at(i),
             camMatrix,
             dCoeffs,
             rvecs.at(i),
             tvecs.at(i),
             SOLVEPNP_ITERATIVE);
    // ====

    vector<Point2f> overlayCorners = { Point2f(0, 0),
                                       Point2f(overlay.cols, 0),
                                       Point2f(overlay.cols, overlay.rows),
                                       Point2f(0, overlay.rows) };
    Mat homography = findHomography(overlayCorners, markerCorners[i]);
    warpPerspective(
      overlay, dest, homography, dest.size(), INTER_LINEAR, BORDER_TRANSPARENT);
  }
}

/**
 * @brief The main loop of the code which will turn the camera on, attempt to
 * read ArUco markers and display images when a marker is found.
 */
int
main(int argc, char* argv[])
{
  ar_utils::printBorder();
  cout << "Welcome to the Augmented Museum application!" << endl;

  // Read CLI arguments
  cli::Parser parser(argc, argv);
  configureParser(parser);
  parser.run_and_exit_if_error();
  cout << "Command line arguments successfully parsed..." << endl;

  ar_utils::printBorder();

  VideoCapture cap(0);
  if (!cap.isOpened()) {
    cerr << "Error opening video stream..." << endl;
    return -1;
  }

  ar_utils::printBorder();

  // Load calibration file
  auto calibrationFile = parser.get<string>("c");
  cout << "Utilizing calibration file found at " << calibrationFile << endl;
  ar_utils::loadCalibrationFile(
    calibrationFile, camMatrix, dCoeffs, rotationVectors, translationVectors);

  // Print ArUco marker
  auto printMarker = parser.get<bool>("a");
  if (printMarker) {
    srand(time(NULL));
    int random = 1 + (rand() % 250);
    ar_utils::createArucoMarker(random);
  }

  // Load images
  auto path = parser.get<string>("p");
  images = ar_utils::loadImagesFromDirectory(path);
  int currentImageIndex = 0;
  Mat overlay = images[currentImageIndex];

  // Set coordinate system
  int markerLength = 200;
  Mat objPoints = ar_utils::setCoordinateSystem(markerLength);

  ar_utils::printBorder();

  namedWindow("Main Window", WINDOW_AUTOSIZE);

  while (cap.grab()) {
    Mat frame, frameCopy;
    cap.retrieve(frame);
    frame.copyTo(frameCopy);

    // if (images.size() == 1) {
    detectAndOverlayMarker(frame, frameCopy, overlay, objPoints);
    // } else {
    // detectAndOverlayMultipleMarkers(frame, frameCopy, objPoints);
    // }

    imshow("Main Window", frameCopy);

    char key = (char)waitKey(10);
    if (key == 'q') { // Quit
      cout << "User terminated program" << endl;
      break;
    }

    else if (key == 's') { // Screenshot
      ar_utils::printBorder();
      ar_utils::screenshot(frameCopy);
    }

    else if (key == 'a') { // Cycle left
      if (currentImageIndex > 0) {
        currentImageIndex--;
        overlay = images[currentImageIndex];
      } else {
        currentImageIndex = images.size() - 1;
        overlay = images[currentImageIndex];
      }
    }

    else if (key == 'd') { // Cycle right
      if (currentImageIndex < images.size() - 1) {
        currentImageIndex++;
        overlay = images[currentImageIndex];
      } else {
        currentImageIndex = 0;
        overlay = images[currentImageIndex];
      }
    }
  }

  ar_utils::printBorder();
  return 0;
}