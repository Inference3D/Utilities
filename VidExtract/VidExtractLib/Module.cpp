//--------------------------------------------------
// Module: Implementation logic of the module
//
// @author: Wild Boar
//
// @date: 2022-03-24
//--------------------------------------------------

#include "Module.h"
using namespace NVL_Module;

//--------------------------------------------------
// Constructor and Terminator
//--------------------------------------------------

/**
 * @brief Main Constructor
 */
Module::Module()
{
    // Extra implementation logic can go here
}

/**
 * @brief Main Terminator
 */
Module::~Module()
{
    // Terminator logic goes here
}

//--------------------------------------------------
// Initialization
//--------------------------------------------------

/**
 * @brief Initialize the given module
 * @param parameters The initialization parameters
 */
void Module::Initialize(NVLib::Parameters& parameters) 
{
    _videoFile = ReadString(parameters, "video_file");
    _frameStep = ReadInteger(parameters, "frame_step");
    _workingFolder = ReadString(parameters, "working_folder");
    _uniqueName = ReadString(parameters, "unique_name");

    Log() << "input [video_file]: " << _videoFile << LoggerBase::End();
    Log() << "input [frame_step]: " << _frameStep << LoggerBase::End();
    Log() << "input [working_folder]: " << _workingFolder << LoggerBase::End();
    Log() << "input [uniqueName]: " << _uniqueName << LoggerBase::End(); 
}

//--------------------------------------------------
// Execution
//--------------------------------------------------

/**
 * @brief Main execution pathway
 * @return SUCCESS or FAILURE
 */
int Module::Execute() 
{
    Log() << "Creating a temp folder" << LoggerBase::End();
    auto tempPath = NVLib::FileUtils::PathCombine(_workingFolder, _uniqueName);
    if (NVLib::FileUtils::Exists(tempPath)) NVLib::FileUtils::RemoveAll(tempPath);
    NVLib::FileUtils::AddFolders(tempPath);

    Log() << "Creating a video player" << LoggerBase::End();
    auto player = VideoCapture(_videoFile); 
    
    if (!player.isOpened()) 
    {
        Log() << "Unable to open: " << _videoFile << LoggerBase::End();
        return 1;
    }

    Log() << "Find estimation of the frame position" << LoggerBase::End();
    auto frameCount = player.get(CAP_PROP_FRAME_COUNT);
    Log() << "Full frame count: " << frameCount << LoggerBase::End();
    Log() << "Estimated frame count: " << (frameCount / _frameStep) << LoggerBase::End();

    auto index = 0;    
    
    while(true) 
    {
        Log() << "Processing Frame: " << index << LoggerBase::End();

        auto position = index * _frameStep;

        player.set(CAP_PROP_POS_FRAMES, position);

        Mat image; player >> image;
        if (image.empty()) break;

        auto fileName = stringstream(); fileName << "image_" << setw(4) << setfill('0') << index << ".jpg";
        auto path = NVLib::FileUtils::PathCombine(tempPath, fileName.str());

        imwrite(path, image);

        index++;
    }

    Log() << "Create the ZIP file" << LoggerBase::End();
    auto outfile = stringstream(); outfile << _uniqueName << ".zip";
    auto outPath = NVLib::FileUtils::PathCombine(_workingFolder, outfile.str());
    NVLib::FileUtils::RemoveAll(outPath); 
    NVLib::ZipUtils::Zip(outPath, tempPath);

    Log() << "Removing the temp folder" << LoggerBase::End();
    NVLib::FileUtils::RemoveAll(tempPath);

    Log() << "Process Complete!" << LoggerBase::End();

    // Return the success code
    return EXIT_SUCCESS;
}