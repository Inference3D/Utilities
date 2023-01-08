//--------------------------------------------------
// Utility: Implementation of a logging utility
//
// @author: Wild Boar
//
// @date: 2022-03-24
//--------------------------------------------------

#pragma once

#include <sstream>
#include <iostream>
using namespace std;

#include <NVLib/StringUtils.h>
#include <ModuleLib/LoggerBase.h>

namespace NVL_Module
{
	class Logger : public LoggerBase
	{
        virtual void Write(const string& message) override 
        {
            auto dateString = NVLib::StringUtils::GetDateTimeString();
            cout << "[" << dateString << "] " << message << endl;
        } 
 	};
}
