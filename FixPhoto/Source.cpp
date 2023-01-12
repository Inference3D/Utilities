//--------------------------------------------------
// Startup code module
//
// @author: Wild Boar
//
// @date: 2023-01-12
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
    if (parameters == nullptr) return; auto logger = NVLib::Logger(1);

    logger.StartApplication();

    logger.Log(1, "Loading parameters");
    auto inputFolder = NVL_Utils::ArgReader::ReadString(parameters, "input");
    auto outputFolder = NVL_Utils::ArgReader::ReadString(parameters, "output");
    auto focal = NVL_Utils::ArgReader::ReadDouble(parameters, "focal");
    auto cropRatio = NVL_Utils::ArgReader::ReadDouble(parameters, "crop_ratio");

    logger.Log(1, "Loading calibration parameters");
    auto calibration = NVL_App::Calibration(inputFolder);

    logger.Log(1, "Getting a list of files");
    auto files = vector<string>(); NVLib::FileUtils::GetFileList(inputFolder, files);
    for (auto i = files.begin(); i != files.end();) 
    {
        auto extension = NVLib::FileUtils::GetExtension(*i); 
        if (extension == "jpg") i++;
        else i = files.erase(i);
    }
    logger.Log(1, "Number of files detected: %i", files.size());

    logger.Log(1, "Generating ideal camera matrix");
    auto cx = calibration.GetSize().width / 2; auto cy = calibration.GetSize().height / 2;
    Mat idealCamera = (Mat_<double>(3,3) << focal, 0, cx, 0, focal, cy, 0, 0, 1);

    logger.Log(1, "Determining cropping values");
    auto halfWidth = (int)round(calibration.GetSize().width * cropRatio * 0.5); 
    auto halfHeight = (int)round(calibration.GetSize().height * cropRatio * 0.5);
    auto rect = Rect(cx - halfWidth, cy - halfWidth, halfWidth * 2, halfHeight * 2);
    logger.Log(1, "New Image Size: %i, %i", rect.width, rect.height);
    logger.Log(1, "CX = %i CY = %i", halfWidth, halfHeight);

    logger.Log(1, "Writing new calibration file");
    auto storagePath = NVLib::FileUtils::PathCombine(outputFolder, "calibration.xml");
    auto calibWriter = FileStorage(storagePath, FileStorage::FORMAT_XML | FileStorage::WRITE);
    Mat newCamera = (Mat_<double>(3,3) << focal, 0, halfWidth, 0, focal, halfHeight, 0, 0, 1); calibWriter << "camera" << newCamera;
    Mat newDistortion = (Mat_<double>(4,1) << 0, 0, 0, 0); calibWriter << "distortion" << newDistortion;
    auto newSize = rect.size(); calibWriter << "image_size" << newSize;    
    calibWriter.release();

    logger.Log(1, "Starting the processing of files");
    for (auto i = 0; i < files.size(); i++) 
    {
        logger.Log(1, "Processing image: %i", i);
        
        // Load the image
        Mat image = imread(files[i]); if (image.empty()) throw runtime_error("Image Load failed");

        // Make sure that the orientation is correct        
        if (image.cols != calibration.GetSize().width) 
        {
            logger.Log(1, "Orientation wrong! Flipping the image");
            rotate(image, image, ROTATE_90_CLOCKWISE);
        }

        // Removing distortion and changing camera matrix
        Mat uimage; undistort(image, uimage, calibration.GetCamera(), calibration.GetDistortion(), idealCamera);

        // Performing cropping
        uimage = uimage(rect);

        // Save the image to disk
        auto fileName = stringstream(); fileName << "image_" << setw(4) << setfill('0') << i << ".jpg";
        auto path = NVLib::FileUtils::PathCombine(outputFolder, fileName.str());
        imwrite(path, uimage);
    }




    logger.StopApplication();
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
