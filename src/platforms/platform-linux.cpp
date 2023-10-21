#include "platform.hpp"

#include <cstdio>
#include <dlfcn.h>
#include <sys/stat.h>

lib_file_state check_lib_file_state(char const* const dynamicLibraryFileRelativePath)
{
	struct stat file_stat;
	if (stat(dynamicLibraryFileRelativePath, &file_stat) == 0)
	{
		static __time_t lastDynamicLibraryChangeTime = 0;
		if (lastDynamicLibraryChangeTime == file_stat.st_mtime)
		{
			return lfs_unchanged;
		}
		lastDynamicLibraryChangeTime = file_stat.st_mtime;
		return lfs_changed;
	}
	return lfs_unknown;
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
