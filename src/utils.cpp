/**
 * Author: Kevin Heleodoro
 * Date: April 3, 2024
 * Purpose: Provide utilities to the main functionality of the application
 */

#include <iostream>

using namespace std;

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