//--------------------------------------------------
// Module: The core module logic for this project
//
// @author: Wild Boar
//
// @date: 2022-03-24
//--------------------------------------------------

#include <iostream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include <ModuleLib/ModuleBase.h>

#include <NVLib/StringUtils.h>
#include <NVLib/FileUtils.h>
#include <NVLib/ZipUtils.h>

namespace NVL_Module 
{

//--------------------------------------------------
// Class Definition
//--------------------------------------------------

class Module : public NVL_Module::ModuleBase
{
    private:
        string _videoFile;
        int _frameStep;
        string _workingFolder;
        string _uniqueName;
    public:
        Module(); 
        ~Module();

        virtual string GetModuleName() override { return "VidExtract"; }
        virtual void Initialize(NVLib::Parameters& parameters) override;
        virtual int Execute() override;
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
     * @brief Retree the module
     * @param module The module that we have freed
     */
    void Free(NVL_Module::ModuleBase * module) 
    {
        delete module;
    }
}
