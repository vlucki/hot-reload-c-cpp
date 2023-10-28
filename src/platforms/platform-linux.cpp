#include "platform.hpp"

#include <errno.h>
#include <unistd.h>
#include <cstdio>
#include <dlfcn.h>

lib_handle_t load_dynamic_library(char const* const libFilePath)
{
    lib_handle_t handle = dlopen(libFilePath, RTLD_NOW);
    char* error = dlerror();
    if (error != nullptr)
    {
        printf("Error loading dynamic library \"%s\":\n\t%s", libFilePath, error);
    }

    return handle;
}

void unload_dynamic_library(lib_handle_t libHandle)
{
    dlclose(libHandle);
    char* error = dlerror();
    if (error != nullptr)
    {
        printf("Error closing dynamic library handle:\n\t%s", error);
    }
}

void* load_func(void* libHandle, char const* const funcName)
{
	void* func = dlsym(libHandle, funcName);
    char* error = dlerror();
    if (error != nullptr)
    {
        printf("Error loading function \"%s\" from dynamic library:\n\t%s", funcName, error);
    }
    return func;
}

void thread_sleep(unsigned long ms)
{
    usleep(ms * 1000);
}

unsigned int open_file(FILE** outFile, char const* const fileName, char const* const accessFlags)
{
    errno = 0;
	*outFile = fopen(fileName,accessFlags);
    if (errno != 0)
    {
        printf("Error (%d) opening file \"%s\" with flags \"%s\" ", errno, fileName, accessFlags);
        outFile = nullptr;
    }
    return errno;
}
