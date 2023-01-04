//--------------------------------------------------
// Startup code module
//
// @author: Wild Boar
//
// @date: 2023-01-04
//--------------------------------------------------

#include <iostream>
using namespace std;

#include <NVLib/Logger.h>
#include <NVLib/FileUtils.h>
#include <NVLib/Parameters/Parameters.h>

#include <opencv2/opencv.hpp>
using namespace cv;

#include "ArgReader.h"

//--------------------------------------------------
// Calibration
//--------------------------------------------------
class Calibration 
{
private:
    Mat _camera;
    Mat _distortion;
    Size _size;
public:
    Calibration(Mat& camera, Mat& distortion, const Size& size) : _camera(camera), _distortion(distortion), _size(size) {}
    inline Mat& GetCamera() { return _camera; }
    inline Mat& GetDistortion() { return _distortion; }
    inline Size& GetSize() { return _size; }
};

//--------------------------------------------------
// Function Prototypes
//--------------------------------------------------
void Run(NVLib::Parameters * parameters);
Calibration * LoadCalibration(const string& folder);
Mat LoadImage(const string& folder, int index);
Mat LoadPose(const string& folder, int index);

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

    logger.Log(1, "Loading the given input files");
    auto inputFolder = NVL_Utils::ArgReader::ReadString(parameters, "input");
    auto outputFolder = NVL_Utils::ArgReader::ReadString(parameters, "output");
    auto index1 = NVL_Utils::ArgReader::ReadInteger(parameters, "index_1");
    auto index2 = NVL_Utils::ArgReader::ReadInteger(parameters, "index_2");

    logger.Log(1, "Loading the calibration details");
    auto calibration = LoadCalibration(inputFolder);

    logger.Log(1, "Loading images");
    Mat image1 = LoadImage(inputFolder, index1);
    Mat image2 = LoadImage(inputFolder, index2);

    logger.Log(1, "Loading poses");
    Mat pose1 = LoadPose(inputFolder, index1);
    Mat pose2 = LoadPose(inputFolder, index2);
    Mat relative = pose2 * pose1.inv();

    logger.Log(1, "Free Resources");
    delete calibration;

    logger.StopApplication();
}

//--------------------------------------------------
// Loader Helpers
//--------------------------------------------------

/**
 * @brief Add the functionality to load details from disk
 * @param folder The folder that we are loading from
 * @return Calibration* The calibration details that we have retrieved
 */
Calibration * LoadCalibration(const string& folder) 
{
    auto path = NVLib::FileUtils::PathCombine(folder, "Calibration.xml");
    auto reader = FileStorage(path, FileStorage::FORMAT_XML | FileStorage::READ);
    if (!reader.isOpened()) throw runtime_error("Unable to open: " + path);

    Mat camera; reader["camera"] >> camera; if (camera.empty()) throw runtime_error("Camera not found in the config");
    Mat distortion; reader["distortion"] >> distortion; if (distortion.empty()) throw runtime_error("Distortion matrixn not found in the config");
    Size size; reader["image_size"] >> size; if (size.width <= 0 || size.height <= 0) throw runtime_error("Unable to find a valid width within the system");

    auto result = new Calibration(camera, distortion, size);

    reader.release();

    return result;
}

/**
 * @brief Add the functionality to load an image from disk
 * @param folder The folder that we are loading from
 * @param index The index of the image that is being loaded
 * @return Mat The image as a matrix
 */
Mat LoadImage(const string& folder, int index) 
{
    auto fileName = stringstream(); fileName << "image_" << setw(4) << setfill('0') << index << ".jpg";
    auto path = NVLib::FileUtils::PathCombine(folder, fileName.str());
    Mat image = imread(path); if (image.empty()) throw runtime_error("Unable to find: " + path);
    return image;
}

/**
 * @brief Load the given pose from disk
 * @param folder The folder that we are loading from
 * @param index The index of the pose that we are loading
 * @return Mat The resultant pose
 */
Mat LoadPose(const string& folder, int index) 
{
    auto fileName = stringstream(); fileName << "pose_" << setw(4) << setfill('0') << index << ".xml";
    auto path = NVLib::FileUtils::PathCombine(folder, fileName.str());
    auto reader = FileStorage(path, FileStorage::FORMAT_XML | FileStorage::READ);
    Mat pose; reader["pose"] >> pose; if (pose.empty()) throw runtime_error("Unable to find the pose in the file");
    reader.release();
    return pose;
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
