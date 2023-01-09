//--------------------------------------------------
// Startup code module
//
// @author: Wild Boar
//
// @date: 2022-10-27
//--------------------------------------------------

#include <iostream>
using namespace std;

#include <NVLib/FileUtils.h>
#include <NVLib/SaveUtils.h>
#include <NVLib/PoseUtils.h>
#include <NVLib/Model/MonoCalibration.h>
#include <NVLib/Parameters/Parameters.h>

#include <opencv2/opencv.hpp>
using namespace cv;

#include <yaml-cpp/yaml.h>

#include "ArgReader.h"

//--------------------------------------------------
// Function Prototypes
//--------------------------------------------------
void Run(NVLib::Parameters * parameters);
NVLib::MonoCalibration * LoadCalibration(const string& folder, int index);
void SaveCalibration(const string& folder, NVLib::MonoCalibration * calibration);
Mat LoadImage(const string& folder, int index);
Mat LoadPose(const string& folder, int index);
void WriteImage(const string& folder, Mat& image, int index);
void WritePose(const string& folder, Mat& pose, int index);

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
    auto inputFolder = NVL_Utils::ArgReader::ReadString(parameters, "input");
    auto outputFolder = NVL_Utils::ArgReader::ReadString(parameters, "output");
    auto startIndex = NVL_Utils::ArgReader::ReadInteger(parameters, "start_index");
    auto elementCount = NVL_Utils::ArgReader::ReadInteger(parameters, "element_count");

    // Load the calibration and write calibration
    auto calibration = LoadCalibration(inputFolder, startIndex);
    if (calibration == nullptr) throw runtime_error("Unable to load calibration from disk");
    SaveCalibration(outputFolder, calibration);
    delete calibration;

    // Now process the files
    auto counter = 0;
    for (auto i = startIndex; i < (startIndex + elementCount); i++) 
    {
        Mat image = LoadImage(inputFolder, i);
        Mat pose = LoadPose(inputFolder, i);
        if (image.empty() || pose.empty()) continue;
        WriteImage(outputFolder,image, counter);
        WritePose(outputFolder, pose, counter);
        counter++;
    }
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

//--------------------------------------------------
// Calibration
//--------------------------------------------------

/**
 * @brief Load the calibration from disk
 * @param folder The folder that we are loading from
 * @param index The index of the file that we are loading 
 * @return NVLib::MonoCalibration * The calibration parameters from disk
 */
NVLib::MonoCalibration * LoadCalibration(const string& folder, int index) 
{
    // Construct the file name
    auto fileName = stringstream(); fileName << index << "_camera_info.yaml";

    // Construct the path
    auto path = NVLib::FileUtils::PathCombine(folder, fileName.str());

    // Create working variables
    Size imageSize; Mat D, K;

    // Defines the actual loading functionality
    try 
    {
        // Create a parser
        auto document = YAML::LoadFile(path);


        // Extract the image size
        auto height = document["height"].as<int>();
        auto width = document["width"].as<int>();
        imageSize = Size(width, height);

        // Load the camera data
        auto kvector = document["K"].as<vector<double>>();
        K = Mat_<double>(3,3); for (auto i = 0; i < 9; i++) ((double *)K.data)[i] = kvector[i];
        
        // Create a blank distortion vector
        D = Mat_<double>::zeros(4,1);
    }
    catch (YAML::ParserException exception) 
    {
        cerr << "Parsing failed: " << exception.what() << endl;
        return nullptr;
    }

    // Return Result
    return new NVLib::MonoCalibration(K, D, imageSize);
}

/**
 * @brief Add calibration handling to the system
 * @param folder The folder that we are saving to 
 * @param calibration The calibration details that we want to save to disk
 */
void SaveCalibration(const string& folder, NVLib::MonoCalibration * calibration) 
{
    // Build the save path
    auto path = NVLib::FileUtils::PathCombine(folder, "calibration.xml");

    // Write the file to disk
    auto writer = FileStorage(path, FileStorage::WRITE | FileStorage::FORMAT_XML);

    writer << "camera" << calibration->GetCamera();
    writer << "distortion" << calibration->GetDistortion();
    writer << "size" << calibration->GetImageSize();

    writer.release();
}

//--------------------------------------------------
// Load and Save images and poses
//--------------------------------------------------

/**
 * @brief Load an image
 * @param folder The folder that contains the images
 * @param index The index of the image that we are loading
 * @return Mat The matrix that we are loading
 */
Mat LoadImage(const string& folder, int index) 
{
    // Create the filename
    auto fileName = stringstream(); fileName << index << "_image_color.png";

    // Create the path
    auto path = NVLib::FileUtils::PathCombine(folder, fileName.str());

    // If the path does not exist then quit
    if (!NVLib::FileUtils::Exists(path)) return Mat();

    // Perform the load
    Mat image = imread(path);

    // Return the result
    return image;
}

/**
 * @brief Load the pose
 * @param folder The folder that we are loading from
 * @param index The index of the folder that we are loading from
 * @return Mat The matrix that we are loading
 */
Mat LoadPose(const string& folder, int index) 
{
    // Create the filename
    auto fileName = stringstream(); fileName << index << "_transform.yaml";

    // Create the path
    auto path = NVLib::FileUtils::PathCombine(folder, fileName.str());

    // If the path does not exist then quit
    if (!NVLib::FileUtils::Exists(path)) return Mat();

    // Declare the variables that we are working with
    Vec4d quaternion; Vec3d translation; 

    // Perform the load
    try 
    {
        // Create a parser
        auto document = YAML::LoadFile(path);

        // Extract the rotation
        quaternion[0] = document["rotation"]["w"].as<double>(); 
        quaternion[1] = document["rotation"]["x"].as<double>(); 
        quaternion[2] = document["rotation"]["y"].as<double>(); 
        quaternion[3] = document["rotation"]["z"].as<double>(); 

        // Extract the translation
        translation[0] = document["translation"]["x"].as<double>();
        translation[1] = document["translation"]["y"].as<double>();
        translation[2] = document["translation"]["z"].as<double>();
    }
    catch (YAML::ParserException exception) 
    {
        cerr << "ERROR: " << exception.what() << endl;
        return Mat();
    }

    // Quaternion to Rotation
    Mat rotation = NVLib::PoseUtils::Quaternion2Matrix(quaternion);

    // Pose Construction
    Mat pose = NVLib::PoseUtils::GetPose(rotation, translation);

    // Return the result
    return pose;
}

/**
 * @brief Write a given image to disk
 * @param folder The folder that we are loading from
 * @param image The image file that we are loading 
 * @param index The index that we are loading from
 */
void WriteImage(const string& folder, Mat& image, int index) 
{
    // Create the file name
    auto fileName = stringstream(); fileName << "image_" << setw(4) << setfill('0') << index << ".jpg";

    // Create the path
    auto path = NVLib::FileUtils::PathCombine(folder, fileName.str());

    // Write the file to disk
    imwrite(path, image);
}

/**
 * @brief Write the given pose 
 * @param folder The folder that we are loading from
 * @param pose The pose file that we are loading
 * @param index The index of the file that we are loading
 */
void WritePose(const string& folder, Mat& pose, int index) 
{
    // Create the file name
    auto fileName = stringstream(); fileName << "pose_" << setw(4) << setfill('0') << index << ".xml";

    // Create the path
    auto path = NVLib::FileUtils::PathCombine(folder, fileName.str());

    // Write the file to disk
    auto writer = FileStorage(path, FileStorage::FORMAT_XML | FileStorage::WRITE);
    writer << "pose" << pose;
    writer.release();
}