//--------------------------------------------------
// Unit Tests for Confirming Module Launch
//
// @author: Wild Boar
//
// @date: 2022-10-09
//--------------------------------------------------

#include <gtest/gtest.h>

#include "../../Hartley/DLLoader.h"

#include <ModuleLib/ModuleBase.h>
using namespace NVL_Module;

//--------------------------------------------------
// Unit Tests
//--------------------------------------------------

/**
 * @brief Add the logic to load a module
 */
TEST(Module_Test, confirm_module_load) 
{
    // Setup
    auto path = string("../HartleyLib/libHartleyLib.so");
    auto loader = DLLoader<ModuleBase>(path);

    // Execute
    loader.DLOpenLib();

    auto myModule = loader.DLGetInstance();
    auto name = myModule->GetModuleName();  
    
    loader.DLCloseLib();

    // Confirm
    ASSERT_EQ(name, "Hartley");
}
