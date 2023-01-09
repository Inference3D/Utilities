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
void GetH(NVL_App::Calibration * calibration, Mat& pose, Mat& H1, Mat& H2, Mat& Q);
NVLib::StereoFrame * Rectify(NVL_App::Frame& frame1, NVL_App::Frame& frame2, Mat& H1, Mat& H2);

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
    Mat rPose = frame2.GetPose() * frame1.GetPose().inv();
    Mat H1, H2, Q; GetH(&calibration, rPose, H1, H2, Q);

    logger.Log(1, "Perform the associated rectification");
    auto stereoFrame = Rectify(frame1, frame2, H1, H2);
    NVLib::DisplayUtils::ShowStereoFrame("Stereo", *stereoFrame, 1000);
    waitKey();
    delete stereoFrame;

}

//--------------------------------------------------
// Rectification
//--------------------------------------------------

/**
 * @brief Add the logic to perform rectification
 * @param calibration The given calibration properties
 * @param pose The relative pose matrix
 * @param H1 The first homography
 * @param H2 The second homography
 * @param Q The associate Q matrix
 */
void GetH(NVL_App::Calibration * calibration, Mat& pose, Mat& H1, Mat& H2, Mat& Q) 
{
    Mat rotation = NVLib::PoseUtils::GetPoseRotation(pose);
    auto translation = NVLib::PoseUtils::GetPoseTranslation(pose);

    Mat R1, R2, P1, P2;

    stereoRectify(  calibration->GetCamera(), 
                    calibration->GetDistortion(), 
                    calibration->GetCamera(),
                    calibration->GetDistortion(),
                    calibration->GetImageSize(),
                    rotation,
                    translation,
                    R1, R2, P1, P2, Q, 
                    0, 0); 

    H1 = P1(Range(0,3), Range(0,3)) * R1 * (calibration->GetCamera() * Mat_<double>::eye(3,3)).inv();
    H2 = P2(Range(0,3), Range(0,3)) * R2 * (calibration->GetCamera() * rotation).inv();
}

/**
 * @brief Perform the associated rectification
 * @param frame1 The first frame within the system
 * @param frame2 The second frame within the system
 * @param H1 The homography associated with the first frame
 * @param H2 The homography associated with the second frame
 * @return NVLib::StereoFrame* The frame result
 */
NVLib::StereoFrame * Rectify(NVL_App::Frame& frame1, NVL_App::Frame& frame2, Mat& H1, Mat& H2) 
{
    Mat image1; warpPerspective(frame1.GetImage(), image1, H1, frame1.GetSize(), INTER_CUBIC);
    Mat image2; warpPerspective(frame2.GetImage(), image2, H2, frame2.GetSize(), INTER_CUBIC);
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
