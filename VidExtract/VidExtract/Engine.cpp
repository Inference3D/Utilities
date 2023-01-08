//--------------------------------------------------
// Implementation code for the Engine
//
// @author: Wild Boar
//
// @date: 2022-03-24
//--------------------------------------------------

#include "Engine.h"
using namespace NVL_Module;

//--------------------------------------------------
// Constructor and Terminator
//--------------------------------------------------

/**
 * Main Constructor
 * @param parameters The input parameters for the application
 * @param logger The logger that the project will use
 */
Engine::Engine(NVLib::Parameters * parameters, LoggerBase * logger) : _parameters(parameters),  _logger(logger)
{
    (*logger) << "Parameter Count: " << parameters->Count() << LoggerBase::End();
}

/**
 * Main Terminator 
 */
Engine::~Engine() 
{
    delete _parameters;
}

//--------------------------------------------------
// Execution Entry Point
//--------------------------------------------------

/**
 * Entry point function
 */
void Engine::Run()
{
    // Setup
    auto path = string("../VidExtractLib/libVidExtractLib.so");
    auto loader = DLLoader<ModuleBase>(path);

    // Execute
    loader.DLOpenLib();
    PerformExecute(loader);
    loader.DLCloseLib();
}

//--------------------------------------------------
// Perform Execution
//--------------------------------------------------

/**
 * @brief Perform the module execution life cycle
 * @param loader The loader that we are using to launch the module
 */
void Engine::PerformExecute(DLLoader<ModuleBase>& loader) 
{
    auto module = loader.DLGetInstance();
    module->SetLogger(_logger);
    module->Initialize(*_parameters);
    module->Execute();
}
