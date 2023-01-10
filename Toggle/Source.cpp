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
#include <NVLib/Math3D.h>
#include <NVLib/DisplayUtils.h>
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
Mat GetCloud(NVL_App::Calibration& calibration, Mat& depth, Mat& pose);
Mat GetImage(NVL_App::Calibration& calibration, Mat& color, Mat& cloud);

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
    cout << calibration.GetCamera();

    logger.Log(1, "Loading up frames");
    auto frame1 = NVL_App::Frame(inputFolder, index1);
    auto frame2 = NVL_App::Frame(inputFolder, index2);

    logger.Log(1, "Loading up depth file");
    Mat depth = LoadDepth(inputFolder, index1);

    logger.Log(1, "Generate a point cloud");
    Mat pose =  frame2.GetPose().inv() * frame1.GetPose();
    Mat cloud = GetCloud(calibration, depth, pose);

    logger.Log(1, "Generate a color image");
    Mat image = GetImage(calibration, frame1.GetImage(), cloud);

    logger.Log(1, "Show the toggle image");
    NVLib::DisplayUtils::ShowToggleImages("Toggle", frame2.GetImage(), image, 1000);
}

//--------------------------------------------------
// Get Cloud
//--------------------------------------------------

/**
 * @brief Add the functionality to generate the point cloud
 * @param calibration The provided camera calibration variables
 * @param depth The depth map that we are getting 
 * @param pose The relative pose
 * @return Mat The output cloud
 */
Mat GetCloud(NVL_App::Calibration& calibration, Mat& depth, Mat& pose) 
{
    cout << pose << endl;

    Mat result = Mat_<Vec3d>::zeros(depth.size());
    auto depthData = (float *) depth.data;
    auto cloudData = (double *) result.data;

    for (auto row = 0; row < result.rows; row++) 
    {
        for (auto column = 0; column < result.cols; column++) 
        {
            auto index = column + row * result.cols;

            auto Z = depthData[index];
            if (Z <= 0) continue;

            auto point = NVLib::Math3D::UnProject(calibration.GetCamera(), Point2d(column, row), Z);
            auto tPoint = NVLib::Math3D::TransformPoint(pose, point);

            cloudData[index * 3 + 0] = tPoint.x;
            cloudData[index * 3 + 1] = tPoint.y;
            cloudData[index * 3 + 2] = tPoint.z;
        }
    }

    return result;
}

//--------------------------------------------------
// Generate a test image
//--------------------------------------------------

/**
 * @brief Generate a test image
 * @param calibration The calibration parameters that we are working with
 * @param color The color image
 * @param cloud The point cloud image
 * @return Mat The resultant image
 */
Mat GetImage(NVL_App::Calibration& calibration, Mat& color, Mat& cloud) 
{
    Mat result = Mat_<Vec3b>::zeros(color.size());
    Mat depth = Mat_<double>(color.size()); depth.setTo(1000);

    auto cloudData = (double *) cloud.data;
    auto depthData = (double *) depth.data;

    for (auto row = 0; row < result.rows; row++) 
    {
        for (auto column = 0; column < result.cols; column++) 
        {
            auto index = column + row * result.cols;

            auto X = cloudData[index * 3 + 0];
            auto Y = cloudData[index * 3 + 1];
            auto Z = cloudData[index * 3 + 2];
            if (Z <= 0) continue;

            auto point = NVLib::Math3D::Project(calibration.GetCamera(), Point3d(X, Y, Z));
            auto u = (int)round(point.x); auto v = (int)round(point.y);

            if (u < 0 || u >= result.cols || v < 0 || v >= result.rows) continue;

            auto index2 = u + v * result.cols;    

            if (depthData[index2] < Z) continue;

            result.data[index2 * 3 + 0] = color.data[index * 3 + 0];
            result.data[index2 * 3 + 1] = color.data[index * 3 + 1];
            result.data[index2 * 3 + 2] = color.data[index * 3 + 2];
            depthData[index2] = Z;  
        }
    }

    return result;
}


//--------------------------------------------------
// Loader Helper
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
