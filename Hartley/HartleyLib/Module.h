//--------------------------------------------------
// Module: The core module logic for this project
//
// @author: Wild Boar
//
// @date: 2022-10-09
//--------------------------------------------------

#include <fstream>
#include <iostream>
using namespace std;

#include <NVLib/StringUtils.h>
#include <NVLib/FileUtils.h>
#include <NVLib/ZipUtils.h>

#include <opencv2/opencv.hpp>
using namespace cv;

#include <ModuleLib/ModuleBase.h>

#include "Runner.h" 

namespace NVL_Module 
{

//--------------------------------------------------
// Class Definition
//--------------------------------------------------

class Module : public NVL_Module::ModuleBase
{
    private:
        Runner * _runner;

		string _uniqueName;
		bool _useZip;
        string _outFolder;
    public:
        Module(); 
        ~Module();

        virtual string GetModuleName() override { return "Hartley"; }
        virtual void Initialize(NVLib::Parameters& parameters) override;
        virtual int Execute() override;
    private:
        void WriteResult(const string& path);
        void WriteZipResult();
    };
}

//--------------------------------------------------
// Create and Free
//--------------------------------------------------

extern "C" 
{
    /**
     * @brief Creates an instance of the module
     * @return ModuleBase* The instance returned
     */
    NVL_Module::ModuleBase * Create() 
    {
        return new NVL_Module::Module();
    }

    /**
     * @brief Free the module
     * @param module The module that we have freed
     */
    void Free(NVL_Module::ModuleBase * module) 
    {
        delete module;
    }
}
