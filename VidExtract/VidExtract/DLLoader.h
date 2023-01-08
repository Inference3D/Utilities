//--------------------------------------------------
// Utility: A tool for loading and unloading dynamic link libraries
//
// @author: Wild Boar
//
// @date: 2022-03-24
//--------------------------------------------------

#pragma once

#include <iostream>
#include <dlfcn.h>
using namespace std;

#include <ModuleLib/LoaderBase.h>

namespace NVL_Module
{
    //--------------------------------------------------
    // Loader class definition
    //--------------------------------------------------

	template<class T> 
	class DLLoader : public LoaderBase<T>
	{
	private:
		void			*_handle;
		std::string		_pathToLib;
		std::string		_allocClassSymbol;
		std::string		_deleteClassSymbol;

	public:

        /**
         * @brief Main constructor
         * @param pathToLib The path to the library
         * @param allocClassSymbol The method to allocate the class
         * @param deleteClassSymbol The method to delete the class
         */
		DLLoader(const string &pathToLib, const string &allocClassSymbol = "Create", const string &deleteClassSymbol = "Free") :
			  _handle(nullptr), _pathToLib(pathToLib), _allocClassSymbol(allocClassSymbol),  _deleteClassSymbol(deleteClassSymbol)
		{
            // Extra initialization can go here
		}

        /**
         * @brief Main Terminator
         */
		virtual ~DLLoader() = default;

        /**
         * @brief Helper to open the library
         */
		virtual void DLOpenLib()
		{
			if (!(_handle = dlopen(_pathToLib.c_str(), RTLD_NOW | RTLD_LAZY))) 
            {
				std::cerr << dlerror() << std::endl;
			}
		}

        /**
         * @brief Helper to close the library
         */
		virtual void DLCloseLib() override
		{
			if (dlclose(_handle) != 0) 
            {
				std::cerr << dlerror() << std::endl;
			}
		}

        /**
         * @brief Generate an instance and return a shared point
         * @return std::shared_ptr The point that we have returned
         */
        virtual std::shared_ptr<T> DLGetInstance() override
        {
            using allocClass = T *(*)();
            using deleteClass = void (*)(T *);
        
            auto allocFunc = reinterpret_cast<allocClass>(dlsym(_handle, _allocClassSymbol.c_str()));
            auto deleteFunc = reinterpret_cast<deleteClass>(dlsym(_handle, _deleteClassSymbol.c_str()));
        
            if (!allocFunc || !deleteFunc) 
            {
                DLCloseLib();
                std::cerr << dlerror() << std::endl;
            }
        
            return std::shared_ptr<T>(allocFunc(), [deleteFunc](T *p){ deleteFunc(p); });            
        }
    };
} 
