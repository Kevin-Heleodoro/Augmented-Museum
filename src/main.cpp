/**
 * Author: Kevin Heleodoro
 * Date: April 3, 2024
 * Purpose: Entrypoint for the Augmented Reality Museum application
 */

#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

#include "../include/ar_utils.h"
#include "../include/cmdparser.hpp"

using namespace std;
using namespace cv;

Mat camMatrix, dCoeffs;
vector<Mat> rotationVectors, translationVectors;

/**
 * @brief Configures the parameters being passed in through the command line.
 */
void
configureParser(cli::Parser& parser)
{
  parser.set_optional<string>(
    "p",
    "path",
    "./paintings",
    "Set path for directory containing images. Defaults to ./paintings "
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
overlayImage(const Mat& src,
             Mat& dest,
             const Mat& overlay,
             const vector<Point2f>& points,
             const Size& overlaySize)
{
  // Center of ArUco Marker
  Point2f markerCenter = Point2f(0.f, 0.f);
  for (const auto& point : points) {
    cout << "adding " << point << "to markerCenter " << markerCenter << endl;
    markerCenter += point;
  }
  markerCenter *= (1.f / points.size());
  cout << "markerCenter: " << markerCenter << endl;

  // Define points where corners of overlay image should be placed after
  // transform.
  vector<Point2f> overlayPoints;
  overlayPoints.push_back(Point2f(markerCenter.x - overlaySize.width / 2.f,
                                  markerCenter.y - overlaySize.height / 2.f));
  overlayPoints.push_back(Point2f(markerCenter.x + overlaySize.width / 2.f,
                                  markerCenter.y - overlaySize.height / 2.f));
  overlayPoints.push_back(Point2f(markerCenter.x + overlaySize.width / 2.f,
                                  markerCenter.y + overlaySize.height / 2.f));
  overlayPoints.push_back(Point2f(markerCenter.x - overlaySize.width / 2.f,
                                  markerCenter.y + overlaySize.height / 2.f));
  // cout << "Overlay points: " << overlayPoints << endl;

  // Source corners for overlay image itself
  vector<Point2f> sourcePoints;
  sourcePoints.push_back(Point2f(0, 0));
  sourcePoints.push_back(Point2f(overlay.cols, 0));
  sourcePoints.push_back(Point2f(overlay.cols, overlay.rows));
  sourcePoints.push_back(Point2f(0, overlay.rows));

  // Homography matrix
  Mat homography = getPerspectiveTransform(sourcePoints, overlayPoints);

  // cout << "Overlay points size: " << overlayPoints.size() << endl;
  // cout << "Points size: " << points.size() << endl;

  // Wrap overlay to fit video feed
  Mat warpedOverlay;
  warpPerspective(overlay, warpedOverlay, homography, dest.size());

  // Mask for overlay image
  Mat overlayMask = Mat::zeros(dest.size(), CV_8UC1);
  vector<Point> overlayPolygon;

  // fillConvexPoly is expecting Point type of CV_32S
  for (const Point2f& p : overlayPoints) {
    overlayPolygon.push_back(
      Point(static_cast<int>(p.x), static_cast<int>(p.y)));
  }
  fillConvexPoly(overlayMask, overlayPolygon, Scalar(255));
  // cout << "overlayMask " << overlayMask.size() << endl;
  // cout << "overlayPoints " << overlayPoints << endl;

  // Invert mask for background
  Mat backgroundMask;
  bitwise_not(overlayMask, backgroundMask);

  // Prepare background and overlay for merge
  Mat background;
  src.copyTo(background, backgroundMask);
  warpedOverlay.copyTo(warpedOverlay, overlayMask);

  // Merge background and overlay
  add(background, warpedOverlay, dest);
}

void
detectArucoMarker(Mat& src, Mat& dest, Mat& overlay, Mat& objPoints)
{
  int markerSize = 200;
  aruco::Dictionary dictionary =
    aruco::getPredefinedDictionary(aruco::DICT_6X6_250);

  vector<int> markerIds;
  vector<vector<Point2f>> markerCorners, rejectedCandidates;
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
               tvecs.at(i));
    }
    aruco::drawDetectedMarkers(dest, markerCorners, markerIds);

    for (unsigned int i = 0; i < markerIds.size(); i++) {
      drawFrameAxes(
        dest, camMatrix, dCoeffs, rvecs[i], tvecs[i], markerSize * 1.5f, 2);
      Size overlaySize(markerCorners[i][0].x * 2, markerCorners[i][0].y * 2);
      // overlayImage(src, dest, overlay, markerCorners[i], overlaySize);
    }
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
  cout << "Welcome to the Augmented Reality application!" << endl;

  cli::Parser parser(argc, argv);
  configureParser(parser);
  parser.run_and_exit_if_error();

  cout << "Command line arguments successfully parsed..." << endl;
  ar_utils::printBorder();

  auto path = parser.get<string>("p");
  cout << "Path to images: " << path << endl;

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
    int random = 1 + (rand() % 250);
    ar_utils::createArucoMarker(random);
  }

  namedWindow("Main Window", WINDOW_AUTOSIZE);

  Mat overlay, image;
  try {
    image = cv::imread("bin/paintings/image_1.jpg");
    cout << "Overlay size: " << image.size() << endl;
    double aspectRatio = (double)200 / image.cols;
    resize(image, overlay, Size(), aspectRatio, aspectRatio, INTER_LINEAR);
    cout << "Overlay resized: " << overlay.size() << endl;
  } catch (const Exception& e) {
    cerr << e.what() << endl;
    return -1;
  }

  // set coordinate system
  int markerLength = 200;
  Mat objPoints(4, 1, CV_32FC3);
  objPoints.ptr<Vec3f>(0)[0] =
    Vec3f(-markerLength / 2.f, markerLength / 2.f, 0);
  objPoints.ptr<Vec3f>(0)[1] = Vec3f(markerLength / 2.f, markerLength / 2.f, 0);
  objPoints.ptr<Vec3f>(0)[2] =
    Vec3f(markerLength / 2.f, -markerLength / 2.f, 0);
  objPoints.ptr<Vec3f>(0)[3] =
    Vec3f(-markerLength / 2.f, -markerLength / 2.f, 0);

  ar_utils::printBorder();

  // while (true) {
  while (cap.grab()) {
    Mat frame, frameCopy;
    cap.retrieve(frame);
    // cap >> frame;
    frame.copyTo(frameCopy);

    detectArucoMarker(frame, frameCopy, overlay, objPoints);

    imshow("Main Window", frameCopy);

    char key = (char)waitKey(10);
    if (key == 'q') {
      ar_utils::printBorder();
      cout << "User terminated program" << endl;
      break;
    }
    if (key == 's') {
      ar_utils::printBorder();
      ar_utils::screenshot(frameCopy);
    }
  }

  ar_utils::printBorder();
  return 0;
}