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
// Measurement
//--------------------------------------------------

class Measurement 
{
private:
    int _imageIndex;
    int _featureIndex;
    Point2d _location;
public:
    Measurement(int imageIndex, int featureIndex, const Point2d& location) : _imageIndex(imageIndex), _featureIndex(featureIndex), _location(location) {}

    inline int GetImageIndex() { return _imageIndex; }
    inline int GetFeatureIndex() { return _featureIndex; }
    inline Point2d& GetLocation() { return _location; }
};

//--------------------------------------------------
// ScenePoint
//--------------------------------------------------

class ScenePoint 
{
private:
    Point3d _location;
    Vec3i _color;
    vector<Measurement *> _measurements;
public:
    ScenePoint(const Point3d& location, const Vec3i& color) : _location(location), _color(color) {}
    ~ScenePoint() { for (auto measurement : _measurements) delete measurement; }
    inline void AddMeasure(int imageIndex, int featureIndex, const Point2d& location) { _measurements.push_back(new Measurement(imageIndex, featureIndex, location)); }
};

//--------------------------------------------------
// Function Prototypes
//--------------------------------------------------
void Run(NVLib::Parameters * parameters);
void ReadNVM(const string& path, vector<View *>& views, vector<ScenePoint *>& points, NVLib::Logger& logger);
Mat ExtractPose(View* view);
int SavePose(const string& folder, const string& fileName, Mat& pose);
int GetFileIndex(const string& fileName);
void SaveCalibration(const string& folder, Mat& camera, const Size& size);

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
    auto width = NVL_Utils::ArgReader::ReadInteger(parameters, "width");
    auto height = NVL_Utils::ArgReader::ReadInteger(parameters, "height");

    logger.Log(1, "Loading views from the input file");
    auto views = vector<View *>(); auto points = vector<ScenePoint *>(); ReadNVM(nvmFile, views, points, logger);

    logger.Log(1, "Saving pose files to disk");
    auto poses = vector<Mat>(views.size());
    for (auto view : views) 
    {
        logger.Log(1, "Saving pose: %s", view->GetFileName().c_str());

        Mat pose = ExtractPose(view);
        auto index = SavePose(folder, view->GetFileName(), pose);

        if (!poses[index].empty()) throw runtime_error("We appear to have a duplicate index!");

        poses[index] = pose;
    }

    logger.Log(1, "Verifying that the focals are the same");
    auto focal = views[0]->GetFocal();
    for (auto view : views) if (focal != view->GetFocal()) throw runtime_error("Inconsistent focal found - the system requires all the focals to be the same");
    logger.Log(1, "Focal check passed!");

    logger.Log(1, "Building new camera matrix");
    Mat camera = (Mat_<double>(3,3) << focal, 0, width * 0.5, 0, focal, height * 0.5, 0, 0, 1);

    logger.Log(1, "Saving new calibration file");
    SaveCalibration(folder, camera, Size(width, height));

    logger.Log(1, "Free up working variables");
    for (auto view : views) delete view; views.clear();
    for (auto point : points) delete point; points.clear();

    logger.StopApplication();
}

//--------------------------------------------------
// Reader Functionality
//--------------------------------------------------

/**
 * @brief Read an input N-View Model file
 * @param path The path to the file
 * @param views The views that were extracted from the file
 * @param points The scene points (that we will use for verification)
 * @param logger The logger that we are using to record our code
 */
void ReadNVM(const string& path, vector<View *>& views,  vector<ScenePoint *>& points, NVLib::Logger& logger) 
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
        auto k = 0.0; reader >> k; if (k != 0) throw runtime_error("The system currently expects distortion free");
        auto zero = 0; reader >> zero;

        views.push_back(new View(fileName, focal, q, c, k));
    }

    auto pointCount = 0; reader >> pointCount;
    logger.Log(1, "Number of scene points detected: %i", pointCount);

    for (auto i = 0; i < pointCount; i++) 
    {
        logger.Log(1, "Extracting scene point: %i", i);
        auto sceneLocation = Point3d(); reader >> sceneLocation.x >> sceneLocation.y >> sceneLocation.z;
        auto color = Vec3i(); reader >> color[0] >> color[1] >> color[2];
        auto measureCount = 0; reader >> measureCount;

        auto point = new ScenePoint(sceneLocation, color);

        for (auto j = 0; j < measureCount; j++) 
        {
            auto imageIndex = 0; reader >> imageIndex;
            auto featureIndex = 0; reader >> featureIndex;
            auto location = Point2d(); reader >> location.x >> location.y;

            point->AddMeasure(imageIndex, featureIndex, location);
        }

        points.push_back(point);
    }

    reader.close();
}

//--------------------------------------------------
// Save Functionality
//--------------------------------------------------

/**
 * @brief Add the logic to extract the pose from the given file
 * @param view The view that we are getting the pose for
 * @return Mat The resultant pose
 */
Mat ExtractPose(View* view) 
{
    Mat rotation = NVLib::PoseUtils::Quaternion2Matrix(view->GetQuaternion());
    auto translation = view->GetLocation();
    return NVLib::PoseUtils::GetPose(rotation, translation);
}

//--------------------------------------------------
// Save Functionality
//--------------------------------------------------

/**
 * @brief Add the logic to save a given pose to disk
 * @param folder The folder that we are saving to
 * @param fileName The name of the file that this represents
 * @param pose The pose that we are saving
 * @param index The index of the file
 */
int SavePose(const string& folder, const string& fileName, Mat& pose) 
{
    auto index = GetFileIndex(fileName);
    auto poseFile = stringstream(); poseFile << "pose_" << setw(4) << setfill('0') << index << ".xml";
    auto path = NVLib::FileUtils::PathCombine(folder, poseFile.str());
    auto writer = FileStorage(path, FileStorage::FORMAT_XML | FileStorage::WRITE);
    writer << "pose" << pose;
    writer.release();
    return index;
}

/**
 * @brief Retrieve the index of the given file name
 * @param fileName The name of the file
 * @return int The resultant value
 */
int GetFileIndex(const string& fileName) 
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

/**
 * @brief Saving the camera matrix to disk
 * @param folder The folder that we are saving to
 * @param camera The camera matrix
 * @param size The size that we are writing
 */
void SaveCalibration(const string& folder, Mat& camera, const Size& size) 
{
    auto path = NVLib::FileUtils::PathCombine(folder, "calibration.xml");
    auto writer = FileStorage(path, FileStorage::FORMAT_XML | FileStorage::WRITE);
    writer << "camera" << camera;
    Mat distortion = (Mat_<double>(4,1) << 0, 0, 0, 0);
    writer << "distortion" << distortion;
    writer << "image_size" << size;
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
