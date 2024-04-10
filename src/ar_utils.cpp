/**
 * Author: Kevin Heleodoro
 * Date: April 3, 2024
 * Purpose: Provide utilities to the main functionality of the application
 */

#include <ctime>
#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

#include "../include/ar_utils.h"

using namespace std;
using namespace cv;

namespace ar_utils {

/**
 * @brief Creates a new Aruco marker and saves it to a file
 */
void
createArucoMarker(int markerId)
{
  cout << "Creating new ArUco marker..." << endl;
  Mat markerImage;
  aruco::Dictionary arucoDict =
    aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
  int markerSize = 200;
  int borderBits = 1;
  aruco::generateImageMarker(
    arucoDict, markerId, markerSize, markerImage, borderBits);
  string filename = "aruco_marker_" + to_string(markerId) + ".png";
  imwrite(filename, markerImage);
  cout << "ArUco Marker created and saved as " << filename << endl;
}

/**
 * @brief loads the camera parameters found in filePath to calibrate the camera
 */
void
loadCalibrationFile(string filePath,
                    Mat& camMatrix,
                    Mat& dCoeffs,
                    vector<Mat>& rotationVectors,
                    vector<Mat>& translationVectors)
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
 * @brief Prints the usage message of the application.
 */
void
printUsageDetails()
{
  cout << "Usage: ./augment_reality.exe [options] (arguments)\n"
       << "Options:\n"
       << "  -a --aruco\t\tCreate new Aruco board \n"
       << "  -v --video\t\tInitiate video stream  \n"
       << "  -c --chessboard\tDetect and calibrate using chessboard\n"
       << "  -hc --harriscorner\tDetect Harris Corners\n"
       << "  -h or --help\t\tShow this help message\n"
       << endl;
}

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
 * @brief Saves a screenshot of the current frame
 */
void
screenshot(Mat& frame)
{
  time_t time_now = time(nullptr);
  char buffer[80];
  tm* ltm = localtime(&time_now);
  strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", ltm);

  string filename = "img/";
  filename += buffer;
  filename += ".png";

  imwrite(filename, frame);
  cout << "Screenshot saved! " << filename << endl;
}

}