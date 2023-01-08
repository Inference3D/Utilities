//--------------------------------------------------
// Startup code module
//
// @author: Wild Boar
//
// @date: 2022-10-09
//--------------------------------------------------

#include <iostream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include <NVLib/FileUtils.h>

#include "Logger.h"
#include "Engine.h"

//--------------------------------------------------
// Function Prototypes
//--------------------------------------------------
string GetParamKeys();
NVLib::Parameters * GetParameters(int argc, char ** argv);

//--------------------------------------------------
// Execution entry point
//--------------------------------------------------

/**
 * Main Method
 * @param argc The count of the incoming arguments
 * @param argv The number of incoming arguments
 */
int main(int argc, char ** argv) 
{
    auto logger = NVL_Module::Logger();
    logger << "Execution Starting" << NVL_Module::Logger::End();
    auto showLog = false;

    try
    {
        auto parameters = GetParameters(argc, argv);
        showLog = parameters->Count() > 0;
        if (showLog) NVL_Module::Engine(parameters, &logger).Run();
    }
    catch (runtime_error exception)
    {
        logger << "Error: " << exception.what() << NVL_Module::Logger::End();
        exit(EXIT_FAILURE);
    }
    catch (string exception)
    {
        logger << "Error: " << exception << NVL_Module::Logger::End();
        exit(EXIT_FAILURE);
    }

    if (showLog) logger << "Execution Complete" << NVL_Module::Logger::End();

    return EXIT_SUCCESS;
}

//--------------------------------------------------
// Parameter Extraction
//--------------------------------------------------

/**
 * Generate the parameter definition
 * @return The parameter definition as a string
 */
string GetParamKeys() 
{
    const char * keys = 
        "{help h usage ? |                       | Show help message }"
        "{@in_folder     | Input                 | The folder containing the input }"
        "{@unique_name   |                       | A unique name for the output file }" 
        "{@out_folder    | Output                | The location of the output folder }" 
        "{start          | 0                     | The index of the first image }"
        "{count          | 1                     | The number of files to process }"
        "{zip            | false                 | Put the output in a zip file }"; 

    return string(keys);
}

/**
 * @brief Retrieval of the actual parameters
 * @param argc The number of input parameters 
 * @param argv The values of the given parameters
 * @return NVLib::Parameters* The parameters that have been returned
 */
NVLib::Parameters * GetParameters(int argc, char ** argv) 
{
    auto parser = CommandLineParser(argc, argv, GetParamKeys());
    parser.about("Hartley v1.0.0");

    auto parameters = new NVLib::Parameters();

    parameters->Add("folder", parser.get<string>(0));

    auto uniqueName = parser.get<String>(1);
    if (uniqueName == string()) uniqueName = NVLib::FileUtils::MakeUniqueName(); 
    parameters->Add("unique_name", uniqueName);

    parameters->Add("out_folder", parser.get<String>(2));
    parameters->Add("start", parser.get<String>("start"));
    parameters->Add("count", parser.get<String>("count"));
    parameters->Add("zip", parser.get<String>("zip"));

    return parameters;
}
