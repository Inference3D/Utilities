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
void SaveImage(const string& folder, const string& fileName, Mat& image);
Mat StereoMatch(NVLib::StereoFrame& frame) ;

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
    SaveImage(outputFolder, "left.jpg", stereoFrame->GetLeft());
    SaveImage(outputFolder, "right.jpg", stereoFrame->GetRight());

    //NVLib::DisplayUtils::ShowStereoFrame("Frame", *stereoFrame, 1000);
    //waitKey(30);
  
    logger.Log(1, "Performing Stereo Matching");
    Mat disparityMap = StereoMatch(*stereoFrame);
    SaveImage(outputFolder, "disparity.tiff", disparityMap);

    //NVLib::DisplayUtils::ShowFloatMap("Disparity", disparityMap, 1000);
    //waitKey();

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

    //cout << calibration->GetCamera() << endl;
    //cout << calibration->GetDistortion().t() << endl;
    //cout << pose << endl;

    Mat rotation = NVLib::PoseUtils::GetPoseRotation(pose);
    auto translation = NVLib::PoseUtils::GetPoseTranslation(pose);
    //cout << rotation << endl;
    //cout << "[" << translation[0] << " " << translation[1] << " " << translation[2] << "]" << endl;

    Mat R1, R2, P1, P2, Q;

    stereoRectify(  calibration->GetCamera(), 
                    calibration->GetDistortion(), 
                    calibration->GetCamera(),
                    calibration->GetDistortion(),
                    calibration->GetImageSize(),
                    rotation,
                    translation,
                    R1, R2, P1, P2, Q, 
                    CALIB_ZERO_DISPARITY, -1, calibration->GetImageSize()); 

    // cout << R1 << endl << R2 << endl << P1 << endl << P2 << endl;

    Mat mapX1, mapX2, mapY1, mapY2;

    initUndistortRectifyMap(calibration->GetCamera(), calibration->GetDistortion(), R1, P1, calibration->GetImageSize(), CV_32FC2, mapX1, mapY1);
    initUndistortRectifyMap(calibration->GetCamera(), calibration->GetDistortion(), R2, P2, calibration->GetImageSize(), CV_32FC2, mapX2, mapY2);

    Mat image1; remap(frame1.GetImage(), image1, mapX1, mapY1, INTER_CUBIC);
    Mat image2; remap(frame2.GetImage(), image2, mapX2, mapY2, INTER_CUBIC);

    return new NVLib::StereoFrame(image1, image2);
}

//--------------------------------------------------
// Perform stereo matching
//--------------------------------------------------

/**
 * @brief Perform stereo matching
 * @param frame The frame that we are matching
 * @return Mat The resultant disparity map
 */
Mat StereoMatch(NVLib::StereoFrame& frame) 
{
    auto matcher = StereoSGBM::create(0, 16 * 16, 3, 200, 2400, 1, 0, 5, 200, 2, StereoSGBM::MODE_SGBM);
    Mat disparityMap; matcher->compute(frame.GetLeft(), frame.GetRight(), disparityMap);

    Mat result = Mat::zeros(disparityMap.size(), CV_32FC1);

    auto input = (short*)disparityMap.data;
    auto output = (float*)result.data;

    for (auto row = 0; row < disparityMap.rows; row++)
    {
        for (auto column = 0; column < disparityMap.cols; column++)
        {
            auto index = column + row * disparityMap.cols;
            auto disparity = (float)input[index];
            output[index] = disparity / 16.0F;
        }
    }
    return result;
}

//--------------------------------------------------
// Entry Point
//--------------------------------------------------

/**
 * @brief Add the logic to save the image to disk
 * @param folder The folder that we are saving to
 * @param fileName The name of the file that we are saving
 * @param image The image that is being saved
 */
void SaveImage(const string& folder, const string& fileName, Mat& image) 
{
    auto path = NVLib::FileUtils::PathCombine(folder, fileName);
    imwrite(path, image);
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
