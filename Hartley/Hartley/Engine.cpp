//--------------------------------------------------
// Implementation code for the Engine
//
// @author: Wild Boar
//
// @date: 2022-10-09
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

     _folder = ArgUtils::GetString(_parameters, "folder");
    _startIndex = ArgUtils::GetInteger(parameters, "start");
	_loopCount = ArgUtils::GetInteger(parameters, "count");
    _uniqueName = ArgUtils::GetString(parameters, "unique_name");

    auto keys = vector<string>(); parameters->GetKeys(keys);
    for (auto& key : keys) 
    {
        (*logger) << "Input [" << key << "]: " << parameters->Get(key) << LoggerBase::End();
    }
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
    auto path = string("../HartleyLib/libHartleyLib.so");
    auto loader = DLLoader<ModuleBase>(path);

    // Perform loop execution
    for (auto i = 0; i < _loopCount; i++) 
    {     
        // Update the parameter for the point file        
        auto leftFile = stringstream(); leftFile << "left_" << setw(4) << setfill('0') << i << ".jpg";
        auto rightFile = stringstream(); rightFile << "right_" << setw(4) << setfill('0') << i << ".jpg";
        auto uniqueName = stringstream(); uniqueName << _uniqueName << "_" << setw(4) << setfill('0') << i;
        auto leftPath = NVLib::FileUtils::PathCombine(_folder, leftFile.str());
        auto rightPath = NVLib::FileUtils::PathCombine(_folder, rightFile.str());
        _parameters->Add("left_image", leftPath); _parameters->Add("right_image", rightPath);
        _parameters->Add("unique_name", uniqueName.str());

        // Perform the actual execution here
        loader.DLOpenLib();
        PerformExecute(loader);
        loader.DLCloseLib();
    }
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
