//=============================================================================
//
// akaze_features.cpp
// Authors: Pablo F. Alcantarilla (1), Jesus Nuevo (2)
// Institutions: Georgia Institute of Technology (1)
//               TrueVision Solutions (2)
// Date: 16/09/2013
// Email: pablofdezalc@gmail.com
//
// AKAZE Features Copyright 2013, Pablo F. Alcantarilla, Jesus Nuevo
// All Rights Reserved
// See LICENSE for the license information
//=============================================================================

/**
 * @file akaze_features.cpp
 * @brief Main program for detecting and computing binary descriptors in an
 * accelerated nonlinear scale space
 * @date Sep 16, 2013
 * @author Pablo F. Alcantarilla, Jesus Nuevo
 *
 * Modification:
 * 16/11/2013: David Ok (david.ok8@gmail.com)
 *
 */

#include "akaze_features.h"
#include "cmdLine.h"

using namespace std;
using namespace cv;

int main( int argc, char *argv[] )
{
  // Variables
  AKAZEOptions options;
  string image_path, key_path;

  // Variable for computation times.
  double t1 = 0.0, t2 = 0.0, tdet = 0.0, tdesc = 0.0;

  // Parse the input command line options
  if (!parse_input_options(options,image_path,key_path,argc,argv))
    return -1;

  if (options.verbosity) {
    cout << "Check AKAZE options:" << endl;
    cout << options << endl;
  }

  // Try to read the image and if necessary convert to grayscale.
  Mat img = imread(image_path,0);
  if (img.data == NULL) {
    cout << "Error: cannot load image from file:" << endl << image_path << endl;
    return -1;
  }

  // Convert the image to float to extract features.
  Mat img_32;
  img.convertTo(img_32,CV_32F,1.0/255.0,0);

  // Don't forget to specify image dimensions in AKAZE's options.
  options.img_width = img.cols;
  options.img_height = img.rows;

  // Extract features.
  vector<KeyPoint> kpts;
  t1 = getTickCount();
  AKAZE evolution(options);
  evolution.Create_Nonlinear_Scale_Space(img_32);
  evolution.Feature_Detection(kpts);
  t2 = getTickCount();
  tdet = 1000.0*(t2-t1) / getTickFrequency();

  // Compute descriptors.
  Mat desc;
  t1 = getTickCount();
  evolution.Compute_Descriptors(kpts,desc);
  t2 = getTickCount();
  tdesc = 1000.0*(t2-t1) / getTickFrequency();

  // Summarize the computation times.
  evolution.Show_Computation_Times();
  evolution.Save_Scale_Space();
  cout << "Number of points: " << kpts.size() << endl;
  cout << "Time Detector: " << tdet << " ms" << endl;
  cout << "Time Descriptor: " << tdesc << " ms" << endl;

  // Save keypoints in ASCII format.
  if (!key_path.empty())
    save_keypoints(&key_path[0],kpts,desc,true);

  // Check out the result visually.
  Mat img_rgb = cv::Mat(Size(img.cols,img.rows),CV_8UC3);
  cvtColor(img,img_rgb,CV_GRAY2BGR);
  draw_keypoints(img_rgb,kpts);
  imshow(image_path, img_rgb);
  waitKey(0);
}

//*************************************************************************************
//*************************************************************************************

/**
 * @brief This function parses the command line arguments for setting KAZE parameters
 * @param options Structure that contains KAZE settings
 * @param img_name Name of the input image
 * @param kfile Name of the file where the keypoints where be stored
 */
int parse_input_options(AKAZEOptions& options, char* img_name, char *kfile,
                        int argc, char *argv[]) {

  // If there is only one argument return
  if (argc == 1){
    show_input_options_help(0);
    return -1;
  }
  // Set the options from the command line
  else if (argc >= 2) {
    // Load the default options
    options.soffset = DEFAULT_SCALE_OFFSET;
    options.omax = DEFAULT_OCTAVE_MAX;
    options.nsublevels = DEFAULT_NSUBLEVELS;
    options.dthreshold = DEFAULT_DETECTOR_THRESHOLD;
    options.diffusivity = DEFAULT_DIFFUSIVITY_TYPE;
    options.descriptor = DEFAULT_DESCRIPTOR;
    options.descriptor_size = DEFAULT_LDB_DESCRIPTOR_SIZE;
    options.descriptor_channels = DEFAULT_LDB_CHANNELS;
    options.descriptor_pattern_size = DEFAULT_LDB_PATTERN_SIZE;
    options.sderivatives = DEFAULT_SIGMA_SMOOTHING_DERIVATIVES;
    options.upright = DEFAULT_UPRIGHT;
    options.save_scale_space = DEFAULT_SAVE_SCALE_SPACE;
    options.save_keypoints = DEFAULT_SAVE_KEYPOINTS;
    options.verbosity = DEFAULT_VERBOSITY;
    strcpy(kfile,"../output/files/keypoints.txt");

    if (!strcmp(argv[1],"--help")) {
      show_input_options_help(0);
      return -1;
    }

    strcpy(img_name,argv[1]);

    for (int i = 2; i < argc; i++) {
      if (!strcmp(argv[i],"--soffset")) {
        i = i+1;
        if (i >= argc) {
          cout << "Error introducing input options!!" << endl;
          return -1;
        }
        else {
          options.soffset = atof(argv[i]);
        }
      }
      else if (!strcmp(argv[i],"--omax")) {
        i = i+1;
        if (i >= argc) {
          cout << "Error introducing input options!!" << endl;
          return -1;
        }
        else {
          options.omax = atof(argv[i]);
        }
      }
      else if (!strcmp(argv[i],"--dthreshold")) {
        i = i+1;
        if (i >= argc) {
          cout << "Error introducing input options!!" << endl;
          return -1;
        }
        else {
          options.dthreshold = atof(argv[i]);
        }
      }
      else if (!strcmp(argv[i],"--sderivatives")) {
        i = i+1;
        if (i >= argc) {
          cout << "Error introducing input options!!" << endl;
          return -1;
        }
        else {
          options.sderivatives = atof(argv[i]);
        }
      }
      else if (!strcmp(argv[i],"--nsublevels")) {
        i = i+1;
        if (i >= argc) {
          cout << "Error introducing input options!!" << endl;
          return -1;
        }
        else {
          options.nsublevels = atoi(argv[i]);
        }
      }
      else if (!strcmp(argv[i],"--diffusivity")) {
        i = i+1;
        if (i >= argc) {
          cout << "Error introducing input options!!" << endl;
          return -1;
        }
        else {
          options.diffusivity = atoi(argv[i]);
        }
      }
      else if (!strcmp(argv[i],"--descriptor")) {
        i = i+1;
        if (i >= argc) {
          cout << "Error introducing input options!!" << endl;
          return -1;
        }
        else {
          options.descriptor = atoi(argv[i]);

          if (options.descriptor < 0 || options.descriptor > 2) {
            options.descriptor = 2;
          }
        }
      }
      else if( !strcmp(argv[i],"--descriptor_channels") )
      {
        i = i+1;
        if( i >= argc )
        {
          cout << "Error introducing input options!!" << endl;
          return -1;
        }
        else
        {
          options.descriptor_channels = atoi(argv[i]);

          if( options.descriptor_channels <= 0 || options.descriptor_channels > 3 )
          {
            options.descriptor_channels = 3;
          }
        }
      }
      else if( !strcmp(argv[i],"--descriptor_size") )
      {
        i = i+1;
        if( i >= argc )
        {
          cout << "Error introducing input options!!" << endl;
          return -1;
        }
        else
        {
          options.descriptor_size = atoi(argv[i]);

          if( options.descriptor_size < 0 )
          {
            options.descriptor_size = 0;
          }
        }
      }
      else if( !strcmp(argv[i],"--save_scale_space") )
      {
        i = i+1;
        if( i >= argc )
        {
          cout << "Error introducing input options!!" << endl;
          return -1;
        }
        else
        {
          options.save_scale_space = (bool)atoi(argv[i]);
        }
      }
      else if( !strcmp(argv[i],"--upright") )
      {
        i = i+1;
        if( i >= argc )
        {
          cout << "Error introducing input options!!" << endl;
          return -1;
        }
        else
        {
          options.upright = (bool)atoi(argv[i]);
        }
      }

      else if( !strcmp(argv[i],"--verbose") )
      {
        options.verbosity = true;
      }
      else if( !strcmp(argv[i],"--output") )
      {
        options.save_keypoints = true;
        i = i+1;
        if( i >= argc )
        {
          cout << "Error introducing input options!!" << endl;
          return -1;
        }
        else
        {
          strcpy(kfile,argv[i]);
        }
      }
      else if( !strcmp(argv[i],"--help") )
      {
        // Show the help!!
        show_input_options_help(0);
        return -1;
      }
    }
  }

  return 0;
}
