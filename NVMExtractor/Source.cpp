//--------------------------------------------------
// Extracts frame details from an NVM file and writes them to a set of pose files
//
// @author: Wild Boar
// @date: 2023-01-04
//--------------------------------------------------

#include <fstream>
#include <iostream>
using namespace std;

#include <NVLib/Formatter.h>
#include <NVLib/Logger.h>
#include <NVLib/StringUtils.h>
#include <NVLib/Math3D.h>
#include <NVLib/PoseUtils.h>
#include <NVLib/FileUtils.h>
#include <NVLib/Parameters/Parameters.h>

#include <opencv2/opencv.hpp>
using namespace cv;

#include "ArgReader.h"

//--------------------------------------------------
// View
//--------------------------------------------------

class View 
{
private:
    string _fileName;
    double _focal;
    Vec4d _quaternion;
    Vec3d _location;
    double _distortion;
public:
    View(const string& fileName, double focal, const Vec4d& quaternion, const Vec3d& location, double distortion) :
        _fileName(fileName), _focal(focal), _quaternion(quaternion), _location(location), _distortion(distortion) {}
    
    inline string& GetFileName() { return _fileName; }
    inline double& GetFocal() { return _focal; }
    inline Vec4d& GetQuaternion() { return _quaternion; }
    inline Vec3d& GetLocation() { return _location; }
    inline double& GetDistortion() { return _distortion; }
};

//--------------------------------------------------
// Function Prototypes
//--------------------------------------------------
void Run(NVLib::Parameters * parameters);
void ReadNVM(const string& path, vector<View *>& views, NVLib::Logger& logger);
void SavePose(const string& folder, View* view);
int GetFileIndex(const string fileName);

//--------------------------------------------------
// Execution Logic
//--------------------------------------------------

/**
 * Main entry point into the application
 * @param parameters The input parameters
 */
void Run(NVLib::Parameters * parameters) 
{
    if (parameters == nullptr) return; auto logger = NVLib::Logger(1);

    logger.StartApplication();

    logger.Log(1, "Loading incomming parameters");
    auto nvmFile = NVL_Utils::ArgReader::ReadString(parameters, "input");
    auto folder = NVL_Utils::ArgReader::ReadString(parameters, "output");

    logger.Log(1, "Loading views from the input file");
    auto views = vector<View *>(); ReadNVM(nvmFile, views, logger);

    logger.Log(1, "Saving pose files to disk");
    for (auto view : views) 
    {
        logger.Log(1, "Saving view: %s", view->GetFileName().c_str());
        SavePose(folder, view);
    }

    logger.Log(1, "Free up working variables");
    for (auto view : views) delete view; views.clear();

   logger.StopApplication();
}

//--------------------------------------------------
// Reader Functionality
//--------------------------------------------------

/**
 * @brief Read an input N-View Model file
 * @param path The path to the file
 * @param views The views that were extracted from the file
 * @param logger The logger that we are using to record our code
 */
void ReadNVM(const string& path, vector<View *>& views, NVLib::Logger& logger) 
{
    auto reader = ifstream(path);
    if (!reader.is_open()) throw runtime_error("Unable to open file: " + path);

    auto header = string(); getline(reader, header); 
    if (!NVLib::StringUtils::StartsWith(header, "NVM_V3")) throw runtime_error("The input file type does not appear to be correct");
    auto viewCount = 0; reader >> viewCount;
    logger.Log(1, "Number of views detected: %i", viewCount);

    for (auto i = 0; i < viewCount; i++) 
    {
        logger.Log(1, "Extracting view: %i", i);
        auto fileName = string(); reader >> fileName;
        auto focal = 0.0; reader >> focal;
        auto q = Vec4d(); reader >> q[0] >> q[1] >> q[2] >> q[3];
        auto c = Vec3d(); reader >> c[0] >> c[1] >> c[2];
        auto k = 0.0; reader >> k;
        auto zero = 0; reader >> zero;

        views.push_back(new View(fileName, focal, q, c, k));
    }

    reader.close();
}

//--------------------------------------------------
// Save Functionality
//--------------------------------------------------

/**
 * @brief Add the logic to save a given pose to disk
 * @param folder The folder that we are saving to
 * @param view The view that we are saving to
 */
void SavePose(const string& folder, View * view) 
{
    auto index = GetFileIndex(view->GetFileName());
    Mat rotation = NVLib::PoseUtils::Quaternion2Matrix(view->GetQuaternion());
    Mat pose = NVLib::PoseUtils::GetPose(rotation, view->GetLocation());

    auto fileName = stringstream(); fileName << "pose_" << setw(4) << setfill('0') << index << ".xml";
    auto path = NVLib::FileUtils::PathCombine(folder, fileName.str());
    auto writer = FileStorage(path, FileStorage::FORMAT_XML | FileStorage::WRITE);
    writer << "pose" << pose;
    writer.release();
}

/**
 * @brief Retrieve the index of the given file name
 * @param fileName The name of the file
 * @return int The resultant value
 */
int GetFileIndex(const string fileName) 
{
    auto namePart = stringstream();
    auto startFound = false;

    for (auto character : fileName) 
    {
        if (!startFound) 
        {
            if (character == '_') startFound = true;
        }
        else 
        {
            if (character == '.') break;
            namePart << character;
        }
    }
    
    if (namePart.str() == string()) throw runtime_error(NVLib::Formatter() << "No number found in file name: " << fileName);

    return NVLib::StringUtils::String2Int(namePart.str());
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
