//--------------------------------------------------
// Utilities: A set of utilities for the processing of incoming arguments
//
// @author: Wild Boar
//
// @date: 2022-03-11
//--------------------------------------------------

#pragma once

#include <iostream>
using namespace std;

#include <NVLib/Formatter.h>
#include <NVLib/StringUtils.h>
#include <NVLib/Parameters/ParameterLoader.h>

namespace NVL_Module
{
	class ArgUtils
	{
	public:
		static string GetString(NVLib::Parameters * parameters, const string& key);
		static int GetInteger(NVLib::Parameters * parameters, const string& key);
		static double GetDouble(NVLib::Parameters * parameters, const string& key);
		static bool GetBoolean(NVLib::Parameters * parameters, const string& key);
	};
}
