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
#include <NVLib/Parameters/Parameters.h>

#include <opencv2/opencv.hpp>
using namespace cv;

#include "ArgReader.h"
#include "Calibration.h"

//--------------------------------------------------
// Function Prototypes
//--------------------------------------------------
void Run(NVLib::Parameters * parameters);
Mat GetNewCamera(NVL_App::Calibration& calibration);

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

    logger.StartApplication();

    logger.Log(1, "Load up input parameters");
    auto inputFolder = NVL_Utils::ArgReader::ReadString(parameters, "input");
    auto outputFolder = NVL_Utils::ArgReader::ReadString(parameters, "output");
    auto count = NVL_Utils::ArgReader::ReadInteger(parameters, "count");

    logger.Log(1, "Loading calibration");
    auto calibration = NVL_App::Calibration(inputFolder);

    logger.Log(1, "Creating an output camera matrix");
    Mat camera = GetNewCamera(calibration);
    auto cameraData = (double *) camera.data;
    logger.Log(1, "Camera: %f %f %f %f", cameraData[0], cameraData[2], cameraData[4], cameraData[5]);

    logger.Log(1, "Processing files");
    for (auto i = 0; i < count; i++) 
    {
        logger.Log(1, "Processing image: %i", i);

        auto fileName = stringstream(); fileName << "image_" << setw(4) << setfill('0') << i << ".jpg";
        auto inputPath = NVLib::FileUtils::PathCombine(inputFolder, fileName.str());
        auto outputPath = NVLib::FileUtils::PathCombine(outputFolder, fileName.str());
        Mat image = imread(inputPath); if (image.empty()) throw runtime_error("Unable to open image: " + inputPath);

        Mat fixedImage; undistort(image, fixedImage, calibration.GetCamera(), calibration.GetDistortion(), camera);

        imwrite(outputPath, fixedImage);
    }

    logger.StopApplication();
}

//--------------------------------------------------
// Get a new calibration matrix
//--------------------------------------------------

/**
 * @brief Generate a new camera matrix
 * @param calibration The calibration parameters we are getting the new matrix for
 * @return Mat The new camera amtrix
 */
Mat GetNewCamera(NVL_App::Calibration& calibration) 
{
    auto kdata = (double *) calibration.GetCamera().data;

    auto fx = kdata[0];
    auto fy = kdata[4];
    auto newF = (int)round((fx + fy) * 0.5);

    auto cx = calibration.GetImageSize().width * 0.5;
    auto cy = calibration.GetImageSize().height * 0.5;

    return (Mat_<double>(3,3) << newF, 0, cx, 0, newF, cy, 0, 0, 1);
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
