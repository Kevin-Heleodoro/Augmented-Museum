#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

#ifndef AR_UTILS_H
#define AR_UTILS_H

namespace ar_utils {

/**
 * @brief Creates a new Aruco marker and saves it to a file
 */
void
createArucoMarker(int markerId);

/**
 * @brief Loads the camera parameters found in filePath to calibrate the camera
 */
void
loadCalibrationFile(std::string filePath,
                    cv::Mat& camMatrix,
                    cv::Mat& dCoeffs,
                    std::vector<cv::Mat>& rotationVectors,
                    std::vector<cv::Mat>& translationVectors);

/**
 * @brief Prints out a border in the terminal to separate output sections
 */
void
printBorder();

/**
 * @brief Saves a screenshot of the current frame
 */
void
screenshot(cv::Mat& frame);

/**
 * @brief Load images from a given directory into a vector of Mats
 */
std::vector<cv::Mat>
loadImagesFromDirectory(std::string path);

/**
 * @brief Set the coordinates for the ArUco marker
 */
cv::Mat
setCoordinateSystem(int markerSize);
}

#endif