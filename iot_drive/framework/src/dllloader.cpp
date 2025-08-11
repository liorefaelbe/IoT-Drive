#include <dlfcn.h>          // std::dlopen

#include "dllloader.hpp"    // Loader

using namespace std;
namespace ilrd_166_7
{

/**************************** Implementations *********************************/
/******************************* DllLoader ************************************/

DllLoader::~DllLoader()
{
    dlclose(m_handle);
}

void DllLoader::Load(const string& fileName)
{
    m_handle = dlopen(fileName.c_str(), RTLD_NOW);
}
}