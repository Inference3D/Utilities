//--------------------------------------------------
// Startup code module
//
// @author: Wild Boar
//
// @date: 2023-01-10
//--------------------------------------------------

#include <iostream>
using namespace std;

#include <NVLib/Logger.h>
#include <NVLib/FileUtils.h>
#include <NVLib/PoseUtils.h>
#include <NVLib/DisplayUtils.h>
#include <NVLib/Model/StereoFrame.h>
#include <NVLib/Parameters/Parameters.h>

#include <opencv2/opencv.hpp>
using namespace cv;

#include "ArgReader.h"
#include "Calibration.h"
#include "Frame.h"

//--------------------------------------------------
// Function Prototypes
//--------------------------------------------------
void Run(NVLib::Parameters * parameters);
NVLib::StereoFrame * Rectify(NVL_App::Calibration * calibration, NVL_App::Frame& frame1, NVL_App::Frame& frame2);

//--------------------------------------------------
// Execution Logic
//--------------------------------------------------

/**
 * Main entry point into the application
 * @param parameters The input parameters
 */
void Run(NVLib::Parameters * parameters) 
{
    // Verify that we have some input parameters and add a helper logger
    if (parameters == nullptr) return; auto logger = NVLib::Logger(1);

    logger.Log(1, "Load up input parameters");
    auto inputFolder = NVL_Utils::ArgReader::ReadString(parameters, "input");
    auto outputFolder = NVL_Utils::ArgReader::ReadString(parameters, "output");
    auto index1 = NVL_Utils::ArgReader::ReadInteger(parameters, "index_1");
    auto index2 = NVL_Utils::ArgReader::ReadInteger(parameters, "index_2");

    logger.Log(1, "Loading calibration information");
    auto calibration = NVL_App::Calibration(inputFolder);

    logger.Log(1, "Loading up frames");
    auto frame1 = NVL_App::Frame(inputFolder, index1);
    auto frame2 = NVL_App::Frame(inputFolder, index2);

    logger.Log(1, "Determine the rectifying homographies");
    auto stereoFrame = Rectify(&calibration, frame1, frame2);
    NVLib::DisplayUtils::ShowStereoFrame("Frame", *stereoFrame, 1000);
    waitKey();
  
    logger.Log(1, "Performing Stereo Matching");
    auto matcher = StereoSGBM::create(0, 32 * 16, 3, 200, 2400, 1, 0, 5, 200, 2, StereoSGBM::MODE_SGBM);
    Mat disparityMap; matcher->compute(stereoFrame->GetLeft(), stereoFrame->GetRight(), disparityMap);

    NVLib::DisplayUtils::ShowFloatMap("Disparity", disparityMap, 1000);
    waitKey();

    logger.Log(1, "Releasing Variables");
    delete stereoFrame;
}

//--------------------------------------------------
// Rectification
//--------------------------------------------------

/**
 * @brief Perform rectification
 * @param calibration The calibration parameters 
 * @param frame1 The first frame
 * @param frame2 The second frame
 * @return NVLib::StereoFrame* The rectified images
 */
NVLib::StereoFrame * Rectify(NVL_App::Calibration * calibration, NVL_App::Frame& frame1, NVL_App::Frame& frame2)
{
    Mat pose = frame2.GetPose() * frame1.GetPose().inv();
    Mat rotation = NVLib::PoseUtils::GetPoseRotation(pose);
    auto translation = NVLib::PoseUtils::GetPoseTranslation(pose);

    Mat R1, R2, P1, P2, Q;

    stereoRectify(  calibration->GetCamera(), 
                    calibration->GetDistortion(), 
                    calibration->GetCamera(),
                    calibration->GetDistortion(),
                    calibration->GetImageSize(),
                    rotation,
                    translation,
                    R1, R2, P1, P2, Q, 
                    0, 0); 

    Mat mapX1, mapX2, mapY1, mapY2;

    initUndistortRectifyMap(calibration->GetCamera(), calibration->GetDistortion(), R1, P1, calibration->GetImageSize(), CV_32F, mapX1, mapY1);
    initUndistortRectifyMap(calibration->GetCamera(), calibration->GetDistortion(), R2, P2, calibration->GetImageSize(), CV_32F, mapX2, mapY2);

    Mat image1; remap(frame1.GetImage(), image1, mapX1, mapY1, INTER_CUBIC);
    Mat image2; remap(frame2.GetImage(), image2, mapX2, mapY2, INTER_CUBIC);

    return new NVLib::StereoFrame(image1, image2);
}

//--------------------------------------------------
// Entry Point
//--------------------------------------------------

/**
 * Main Method
 * @param argc The count of the incoming arguments
 * @param argv The number of incoming arguments
 * @return SUCCESS and FAILURE
 */
int main(int argc, char ** argv) 
{
    NVLib::Parameters * parameters = nullptr;

    try
    {
        parameters = NVL_Utils::ArgReader::GetParameters(argc, argv);
        Run(parameters);
    }
    catch (runtime_error exception)
    {
        cerr << "Error: " << exception.what() << endl;
        exit(EXIT_FAILURE);
    }
    catch (string exception)
    {
        cerr << "Error: " << exception << endl;
        exit(EXIT_FAILURE);
    }

    if (parameters != nullptr) delete parameters;

    return EXIT_SUCCESS;
}
