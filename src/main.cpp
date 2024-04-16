/**
 * Author: Kevin Heleodoro
 * Date: April 3, 2024
 * Purpose: Entrypoint for the Augmented Reality Museum application. This
 * program uses OpenCV to detect an ArUco marker and overlay an image to it.
 */

#include <filesystem>
#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

#include "../include/ar_utils.h"
#include "../include/cmdparser.hpp"
// #include <ar_utils.h>

using namespace std;
using namespace cv;

namespace fs = std::__fs::filesystem;

Mat camMatrix, dCoeffs;
vector<Mat> rotationVectors, translationVectors;
vector<Mat> images;

// static vector<int, Mat> arucoImagePairs;

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

// vector<Mat>
// loadImagesFromDirectory(const string& path)
// {
//   ar_utils::printBorder();
//   cout << "Loading images from " << path << endl;
//   vector<Mat> images;

//   for (const auto& file : fs::directory_iterator(path)) {
//     try {
//       Mat image = imread(file.path().string());
//       Mat overlay;
//       if (image.empty()) {
//         cerr << "Failed to load image." << endl;
//         continue;
//       }

//       cout << "Overlay size: " << image.size() << endl;
//       double aspectX = (double)560 / image.cols;
//       double aspectY = (double)720 / image.rows;
//       resize(image, overlay, Size(), aspectX, aspectY, INTER_LINEAR);
//       cout << "Overlay resized: " << overlay.size() << endl;
//       images.push_back(overlay);

//     } catch (const Exception& e) {
//       cerr << e.what() << endl;
//     }
//   }

//   cout << "Loaded " << images.size() << "images" << endl;
//   return images;
// }

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
  warpPerspective(overlay, warpedOverlay, homography, src.size());

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

void
detectArucoMarker2(Mat& src, Mat& dest, Mat& overlay, Mat& objPoints)
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
      drawFrameAxes(
        dest, camMatrix, dCoeffs, rvecs[i], tvecs[i], markerSize * 0.5f);
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

// void
// detectArucoMarker(Mat& src, Mat& dest, Mat& overlay, Mat& objPoints)
// {
//   int markerSize = 200;
//   aruco::Dictionary dictionary =
//     aruco::getPredefinedDictionary(aruco::DICT_6X6_250);

//   vector<int> markerIds;
//   vector<vector<Point2f>> markerCorners, rejectedCandidates;
//   aruco::DetectorParameters detectorParams = aruco::DetectorParameters();
//   aruco::ArucoDetector detector(dictionary, detectorParams);

//   detector.detectMarkers(src, markerCorners, markerIds);
//   size_t nMarkers = markerCorners.size();
//   vector<Vec3d> rvecs(nMarkers), tvecs(nMarkers);

//   if (!markerIds.empty()) {

//     for (size_t i = 0; i < nMarkers; i++) {
//       solvePnP(objPoints,
//                markerCorners.at(i),
//                camMatrix,
//                dCoeffs,
//                rvecs.at(i),
//                tvecs.at(i),
//                SOLVEPNP_ITERATIVE);
//       drawFrameAxes(
//         dest, camMatrix, dCoeffs, rvecs[i], tvecs[i], markerSize * 0.5f);
//       overlayImage2(src,
//                     dest,
//                     overlay,
//                     markerCorners[i],
//                     rvecs[i],
//                     tvecs[i],
//                     camMatrix,
//                     dCoeffs);
//     }
//   }
// }

/**
 * @brief The main loop of the code which will turn the camera on, attempt to
 * read ArUco markers and display images when a marker is found.
 */
int
main(int argc, char* argv[])
{
  ar_utils::printBorder();
  cout << "Welcome to the Augmented Reality application!" << endl;

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
  auto calibrationFile = parser.get<string>("c");
  cout << "Utilizing calibration file found at " << calibrationFile << endl;

  try {
    ar_utils::loadCalibrationFile(
      calibrationFile, camMatrix, dCoeffs, rotationVectors, translationVectors);
  } catch (const Exception& e) {
    cerr << "Error loading calibration file: " << e.what() << endl;
    return -1;
  }

  auto printMarker = parser.get<bool>("a");
  if (printMarker) {
    srand(time(NULL));
    int random = 1 + (rand() % 250);
    ar_utils::createArucoMarker(random);
  }

  namedWindow("Main Window", WINDOW_AUTOSIZE);

  auto path = parser.get<string>("p");
  vector<Mat> images = ar_utils::loadImagesFromDirectory(path);
  if (images.empty()) {
    cerr << "Unable to load images" << endl;
    return -1;
  }

  int currentImageIndex = 0;
  Mat overlay = images[currentImageIndex];

  // set coordinate system
  int markerLength = 200;
  Mat objPoints = ar_utils::setCoordinateSystem(markerLength);
  // Mat objPoints(4, 1, CV_32FC3);
  // objPoints.ptr<Vec3f>(0)[0] =
  //   Vec3f(-markerLength / 2.f, markerLength / 2.f, 0);
  // objPoints.ptr<Vec3f>(0)[1] = Vec3f(markerLength / 2.f, markerLength / 2.f,
  // 0); objPoints.ptr<Vec3f>(0)[2] =
  //   Vec3f(markerLength / 2.f, -markerLength / 2.f, 0);
  // objPoints.ptr<Vec3f>(0)[3] =
  //   Vec3f(-markerLength / 2.f, -markerLength / 2.f, 0);

  ar_utils::printBorder();

  while (cap.grab()) {
    Mat frame, frameCopy;
    cap.retrieve(frame);
    frame.copyTo(frameCopy);

    detectArucoMarker2(frame, frameCopy, overlay, objPoints);

    imshow("Main Window", frameCopy);

    char key = (char)waitKey(10);
    if (key == 'q') {
      cout << "User terminated program" << endl;
      break;
    }

    else if (key == 's') {
      ar_utils::printBorder();
      ar_utils::screenshot(frameCopy);
    }

    else if (key == 'a') { // Left arrow
      if (currentImageIndex > 0) {
        currentImageIndex--;
        overlay = images[currentImageIndex];
      } else {
        currentImageIndex = images.size() - 1;
        overlay = images[currentImageIndex];
      }
    }

    else if (key == 'd') { // Right arrow
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