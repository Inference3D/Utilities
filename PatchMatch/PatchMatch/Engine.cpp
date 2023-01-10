//--------------------------------------------------
// Implementation code for the Engine
//
// @author: Wild Boar
//
// @date: 2023-01-10
//--------------------------------------------------

#include "Engine.h"
using namespace NVL_App;

//--------------------------------------------------
// Constructor and Terminator
//--------------------------------------------------

/**
 * Main Constructor
 * @param logger The logger that we are using for the system
 * @param parameters The input parameters
 */
Engine::Engine(NVLib::Logger* logger, NVLib::Parameters* parameters) 
{
    _logger = logger; _parameters = parameters;

    _logger->Log(1, "Loading general parameters");
    auto inputFolder = ArgUtils::GetString(parameters, "input_folder");
    auto index1 = ArgUtils::GetInteger(parameters, "index_1");
    auto index2 = ArgUtils::GetInteger(parameters, "index_2");

    _logger->Log(1, "Loading calibration");
    _calibration = new Calibration(inputFolder);

    _logger->Log(1, "Loading Frames");
    _frames.push_back(new Frame(inputFolder, index1));
    _frames.push_back(new Frame(inputFolder, index2));
}

/**
 * Main Terminator 
 */
Engine::~Engine() 
{
    delete _parameters;
    delete _calibration;
    for (auto frame : _frames) delete frame;
}

//--------------------------------------------------
// Execution Entry Point
//--------------------------------------------------

/**
 * Entry point function
 */
void Engine::Run()
{
    // TODO: Execution Logic
}
