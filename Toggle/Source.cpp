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
#include "Frame.h"

//--------------------------------------------------
// Function Prototypes
//--------------------------------------------------
void Run(NVLib::Parameters * parameters);
Mat LoadDepth(const string& folder, int index);

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

    logger.Log(1, "Loading up depth file");
    Mat depth = LoadDepth(inputFolder, index1);

}

//--------------------------------------------------
// Entry Point
//--------------------------------------------------

/**
 * @brief Loader helper for the depth map
 * @param folder The folder that we are loading from 
 * @param index The index of the file that is being loaded
 * @return Mat The resultant depth values
 */
Mat LoadDepth(const string& folder, int index) 
{
    auto fileName = stringstream(); fileName << "depth_" << setw(4) << setfill('0') << index << ".tiff";
    auto path = NVLib::FileUtils::PathCombine(folder, fileName.str());

    Mat result = imread(path, IMREAD_UNCHANGED);
    if (result.empty()) throw runtime_error("Unable to load file: " + path);

    return result;
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
