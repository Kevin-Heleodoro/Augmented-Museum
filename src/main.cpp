/**
 * Author: Kevin Heleodoro
 * Date: April 3, 2024
 * Purpose: Entrypoint for the Augmented Reality Museum application
 */

#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

#include "../include/cmdparser.hpp"
#include "../include/utils.h"

using namespace std;
using namespace cv;

Mat camMatrix, dCoeffs;
vector<Mat> rotationVectors, translationVectors;

/**
 * @brief prints out a border in the terminal to separate output sections
 */
void
printBorder()
{
  cout << "\n" << endl;
  cout << "-----------------------------------------------------" << endl;
  cout << "\n" << endl;
}

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
 * @brief loads the camera parameters found in filePath to calibrate the camera
 */
void
loadCalibrationFile(string filePath)
{
  cout << "Calibrating camera..." << endl;
  FileStorage fs;

  try {
    fs.open(filePath, FileStorage::READ);
  } catch (const Exception& e) {
    cerr << "Failed to open calibration file: " << e.what() << endl;
    return;
  }

  fs["camera_matrix"] >> camMatrix;
  fs["dist_coeffs"] >> dCoeffs;

  rotationVectors.clear();
  FileNode rvecNode = fs["rotation_vectors"];
  for (FileNodeIterator n = rvecNode.begin(); n != rvecNode.end(); ++n) {
    Mat tmp;
    *n >> tmp;
    rotationVectors.push_back(tmp);
  }

  fs.release();

  cout << "Loading Parameters" << endl;
  cout << "Camera Matrix: " << camMatrix << endl;
  cout << "Distortion Coefficients: " << dCoeffs << endl;
  cout << "Rotation Vectors: " << rotationVectors.size() << endl;
  cout << "Translation Vectors: " << translationVectors.size() << endl;
  cout << "Finished loading ..." << endl;
}

/**
 * @brief Creates a new Aruco marker and saves it to a file
 */
void
createArucoMarker(int markerId)
{
  cout << "Creating new Aruco marker..." << endl;
  Mat markerImage;
  aruco::Dictionary arucoDict =
    aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
  int markerSize = 200;
  int borderBits = 1;
  aruco::generateImageMarker(
    arucoDict, markerId, markerSize, markerImage, borderBits);
  string filename = "aruco_marker_" + to_string(markerId) + ".png";
  imwrite(filename, markerImage);
}

void
detectArucoMarker(Mat& src, Mat& dest, Mat& overlay)
{
  aruco::Dictionary dictionary =
    aruco::getPredefinedDictionary(aruco::DICT_6X6_250);

  vector<int> markerIds;
  vector<vector<Point2f>> markerCorners, rejectedCandidates;
  aruco::DetectorParameters detectorParams = aruco::DetectorParameters();
  aruco::ArucoDetector detector(dictionary, detectorParams);

  detector.detectMarkers(src, markerCorners, markerIds);

  // cout << "markerCorners: " << markerCorners.size() << endl;

  if (!markerIds.empty()) {
    aruco::drawDetectedMarkers(dest, markerCorners, markerIds);
    vector<Vec3d> rvecs, tvecs;

    // solvePnP()
  }
}

/**
 * @brief The main loop of the code which will turn the camera on, attempt to
 * read ArUco markers and display images when a marker is found.
 */
int
main(int argc, char* argv[])
{
  printBorder();
  cout << "Welcome to the Augmented Reality application!" << endl;

  cli::Parser parser(argc, argv);
  configureParser(parser);
  parser.run_and_exit_if_error();

  cout << "Command line arguments successfully parsed..." << endl;
  printBorder();

  auto path = parser.get<string>("p");
  cout << "Path to images: " << path << endl;

  VideoCapture cap(0);
  if (!cap.isOpened()) {
    cerr << "Error opening video stream..." << endl;
    return -1;
  }

  printBorder();
  auto calibrationFile = parser.get<string>("c");
  cout << "Utilizing calibration file found at " << calibrationFile << endl;

  try {
    loadCalibrationFile(calibrationFile);
  } catch (const Exception& e) {
    cerr << "Error loading calibration file: " << e.what() << endl;
    return -1;
  }

  auto printMarker = parser.get<bool>("a");
  if (printMarker) {
    int random = 1 + (rand() % 250);
    cout << "Creating ArUco marker ID: " << random << endl;
    createArucoMarker(random);
  }

  namedWindow("Main Window", WINDOW_AUTOSIZE);

  Mat overlay;
  try {
    overlay = cv::imread("bin/paintings/image_1.jpg");
  } catch (const Exception& e) {
    cerr << e.what() << endl;
    return -1;
  }

  Mat frame, frameCopy;

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

  while (true) {
    cap >> frame;
    frame.copyTo(frameCopy);

    detectArucoMarker(frame, frameCopy, overlay);

    imshow("Main Window", frameCopy);

    char key = (char)waitKey(10);
    if (key == 'q') {
      printBorder();
      cout << "User terminated program" << endl;
      break;
    }
  }

  printBorder();
  return 0;
}