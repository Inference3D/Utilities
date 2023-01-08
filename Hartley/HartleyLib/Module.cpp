//--------------------------------------------------
// Module: Implementation logic of the module
//
// @author: Wild Boar
//
// @date: 2022-10-09
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
    _runner = nullptr;
}

/**
 * @brief Main Terminator
 */
Module::~Module()
{
    if (_runner != nullptr) delete _runner;
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
    // Indicate that the application has started
    Log() << GetModuleName() << " starting" << LoggerBase::End();

    // Set the internal variables
    try 
    {
        _runner = new Runner(parameters, &Log());

        _uniqueName = ReadString(parameters, "unique_name");
        _useZip = ReadBoolean(parameters, "zip");
        _outFolder = ReadString(parameters, "out_folder");
    }
    catch (runtime_error exception) 
    {
        Log() << "Parameter Load Failed: " << exception.what() << LoggerBase::End();
        throw runtime_error("module failed");
    }

    // Show the incoming parameters to the log screen
    Log() << "Input [unique_name]: " << _uniqueName << LoggerBase::End();
    Log() << "Input [zip]: " << _useZip << LoggerBase::End();
    Log() << "Input [out_folder]: " << _outFolder << LoggerBase::End(); 
}

//--------------------------------------------------
// Execution
//--------------------------------------------------

/**
 * @brief Main execution pathway
 * @return A given error code
 */
int Module::Execute() 
{
    Log() << "Execute called" << LoggerBase::End();
    _runner->Run();

    Log() << "Writing the result to disk" << LoggerBase::End();
    if (_useZip) WriteZipResult();
    else WriteResult(_outFolder);
    
    return EXIT_SUCCESS;
}

//--------------------------------------------------
// Write Results to disk
//--------------------------------------------------

/**
 * @brief Add the logic to write the result to disk
 * @param path The path that we are writing to 
 */
void Module::WriteResult(const string& path) 
{
    // Build fileNames
    auto leftFile = stringstream(); leftFile << _uniqueName << "_LEFT_rectified.png";
    auto rightFile = stringstream(); rightFile << _uniqueName << "_RIGHT_rectified.png";
    auto disparityFile = stringstream(); disparityFile << _uniqueName << "_disparity.tiff";

    // Build the path files
    auto leftPath = NVLib::FileUtils::PathCombine(path, leftFile.str());
    auto rightPath = NVLib::FileUtils::PathCombine(path, rightFile.str());
    auto disparityPath = NVLib::FileUtils::PathCombine(path, disparityFile.str());

    // Write the files to disk
    imwrite(leftPath, _runner->GetRectifiedLeft());
    imwrite(rightPath, _runner->GetRectifiedRight());
    imwrite(disparityPath, _runner->GetDisparity());
}

/**
 * @brief Add the logic to write the result to a zip file 
 */
void Module::WriteZipResult() 
{
    // Create a folder for files
    auto folderPath = NVLib::FileUtils::PathCombine(_outFolder, _uniqueName);
    if (NVLib::FileUtils::Exists(folderPath)) NVLib::FileUtils::RemoveAll(folderPath);
    NVLib::FileUtils::AddFolders(folderPath);

    // Save the files into the folder
    WriteResult(folderPath);

    // Zip the file
    auto zipFileName = stringstream(); zipFileName << _uniqueName << ".zip";
    auto zipPath = NVLib::FileUtils::PathCombine(_outFolder, zipFileName.str());
    NVLib::ZipUtils::Zip(zipPath, folderPath); 

    if (!NVLib::FileUtils::Exists(zipPath)) 
    {
        Log() << "Problem writing zip file!" << LoggerBase::End();
    }
    else 
    {
        Log() << "Zip File written to " << zipPath << LoggerBase::End();
    }

    // Delete the folder
    if (NVLib::FileUtils::Exists(folderPath)) NVLib::FileUtils::RemoveAll(folderPath);
}
