/**
 * Author: Kevin Heleodoro
 * Date: April 3, 2024
 * Purpose: Entrypoint for the Augmented Reality Museum application
 */

#include <iostream>
#include <opencv2/opencv.hpp>

#include "../include/cmdparser.hpp"
#include "../include/utils.h"

using namespace std;

void
configure_parser(cli::Parser& parser)
{
  parser.set_optional<string>(
    "p",
    "path",
    "./paintings",
    "Set path for directory containing images. Defaults to ./paintings "
    "directory which contains a handful of assorted artworks.");
}

/**
 * The main loop of the code which will turn the camera on, attempt to read
 * ArUco markers and display images when a marker is found.
 */
int
main(int argc, char* argv[])
{
  cout << "Welcome to the Augmented Reality application!" << endl;

  cli::Parser parser(argc, argv);
  configure_parser(parser);
  parser.run_and_exit_if_error();

  cout << "Command line arguments successfully parsed..." << endl;

  auto path = parser.get<string>("p");

  cout << "Path: " << path << endl;
  return 0;
}