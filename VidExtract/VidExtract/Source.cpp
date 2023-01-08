//--------------------------------------------------
// Startup code module
//
// @author: Wild Boar
//
// @date: 2022-03-24
//--------------------------------------------------

#include "Logger.h"
#include "Engine.h"

//--------------------------------------------------
// Function Prototypes
//--------------------------------------------------
NVLib::Parameters * GetParameters(int argc, char ** argv);

//--------------------------------------------------
// Execution entry point
//--------------------------------------------------

/**
 * Main Method
 * @param argc The count of the incomming arguments
 * @param argv The number of incomming arguments
 */
int main(int argc, char ** argv) 
{
    auto logger = NVL_Module::Logger();

    try
    {
        auto parameters = GetParameters(argc, argv);
        NVL_Module::Engine(parameters, &logger).Run();
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

    return EXIT_SUCCESS;
}

//--------------------------------------------------
// Retrieve the parameters
//--------------------------------------------------

/**
 * @brief Retrieve the parameters that we are getting
 * @param argc The number of arguments
 * @param argv The values of the arguments
 * @return NVLib::Parameters* The parameters that we are using
 */
NVLib::Parameters * GetParameters(int argc, char ** argv) 
{
    if (argc != 5) throw runtime_error("USAGE: VidExtract <video_file> <frame_step> <working_folder> <unique_name>"); 

    auto parameters = new NVLib::Parameters();
    parameters->Add("video_file", argv[1]);
    parameters->Add("frame_step", argv[2]);
    parameters->Add("working_folder", argv[3]);
    parameters->Add("unique_name", argv[4]);

    return parameters;
}