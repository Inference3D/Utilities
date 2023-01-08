//--------------------------------------------------
// Defines a basic engine for a vanilla C++ project.
//
// @author: Wild Boar
//
// @date: 2022-03-24
//--------------------------------------------------

#pragma once

#include <iostream>
using namespace std;

#include <NVLib/Parameters/ParameterLoader.h>

#include <ModuleLib/LoggerBase.h>
#include <ModuleLib/ModuleBase.h>

#include "DLLoader.h"

namespace NVL_Module
{
	class Engine
	{
	private:
		NVLib::Parameters * _parameters;
		LoggerBase * _logger;
	public:
		Engine(NVLib::Parameters* parameters, LoggerBase * logger);
		~Engine();

		void Run();
	private:
		void PerformExecute(DLLoader<ModuleBase>& loader);
	};
}
