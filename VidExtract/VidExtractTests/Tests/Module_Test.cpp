//--------------------------------------------------
// Unit Tests for Confirming Module Launch
//
// @author: Wild Boar
//
// @date: 2022-03-24
//--------------------------------------------------

#include <gtest/gtest.h>

#include "../../VidExtract/DLLoader.h"

#include <ModuleLib/ModuleBase.h>
using namespace NVL_Module;

//--------------------------------------------------
// Function Prototypes
//--------------------------------------------------
string ReadName(DLLoader<ModuleBase>& loader);

//--------------------------------------------------
// Unit Tests
//--------------------------------------------------

/**
 * @brief Add the logic to load a module
 */
TEST(Module_Test, confirm_module_load) 
{
    // Setup
    //auto path = string("../VidExtractLib/libVidExtractLib.so");
    //auto loader = DLLoader<ModuleBase>(path);

    // Execute
    //loader.DLOpenLib();

    //auto name = ReadName(loader);    
    
    //loader.DLCloseLib();

    // Confirm
    //ASSERT_EQ(name, "VidExtract");
}

//--------------------------------------------------
// Helper Methods
//--------------------------------------------------

/**
 * @brief Add the functionality to read thename from a module
 * @param loader The loader
 * @return string The resultant string
 */
string ReadName(DLLoader<ModuleBase>& loader) 
{
    // Update this to read the name thru the logger

    //auto module = loader.DLGetInstance();
    //auto descriptor = module->GetDescriptor();
    //auto name = descriptor->Name(); 
    //delete descriptor;
    //return name;

    return string(); // Temp -> Remove this when done
}
