//--------------------------------------------------
// Startup code module
//
// @author: Wild Boar
//
// @date: 2023-01-02
//--------------------------------------------------

#include <iostream>
using namespace std;

#include <NVLib/Logger.h>
#include <NVLib/Parameters/Parameters.h>

#include <opencv2/opencv.hpp>
using namespace cv;

#include "ArgReader.h"
#include "MonoCalibration.h"

//--------------------------------------------------
// Function Prototypes
//--------------------------------------------------
void WriteCalibration(const string& path, NVL_Tools::MonoCalibration& calibration);
void Run(NVLib::Parameters * parameters);

//--------------------------------------------------
// Execution Logic
//--------------------------------------------------

/**
 * Main entry point into the application
 * @param parameters The input parameters
 */
void Run(NVLib::Parameters * parameters) 
{
    // Verify that we have some input parameters
    if (parameters == nullptr) return;

    // Load the parameters into variables
    auto inputFile = NVL_Utils::ArgReader::ReadString(parameters, "input");
    auto outputFile = NVL_Utils::ArgReader::ReadString(parameters, "output");

    // Create a logger
    auto logger = NVLib::Logger(1); 
    logger.StartApplication();

    // Load the calibration from disk
    logger.Log(1, "Loading caliabration");
    auto calibration = NVL_Tools::MonoCalibration(inputFile);
	
	logger.Log(1, "Saving the new calibration to disk...");
	WriteCalibration(outputFile, calibration);

    // Terminating the application
	logger.StopApplication();
}

//--------------------------------------------------
// Utilities
//--------------------------------------------------

/**
 * Write the resultant calibration to disk
 * @param path The path that we are saving to
 * @param cameraMatrix The new camera matrix
 * @param calibration The calibration variables that we are saving
 */
void WriteCalibration(const string& path, NVL_Tools::MonoCalibration& calibration) 
{
	auto writer = FileStorage(path, FileStorage::FORMAT_XML | FileStorage::WRITE);

	writer << "camera" << calibration.GetCameraMatrix();
	writer << "distortion" << calibration.GetDistortion();
	writer << "image_size" << calibration.GetImageSize();

	writer.release();
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
