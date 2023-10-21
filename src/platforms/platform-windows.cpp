#include "platform.hpp"

#include <windows.h>

// TODO_vlucki: review and test all code on a proper Windows machine

lib_file_state check_lib_file_state(char const* const dynamicLibraryFileRelativePath)
{
	WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
	if (GetFileAttributesExA(dynamicLibraryFileRelativePath, GetFileExInfoStandard, &fileAttributes))
	{
		static FILETIME lastDynamicLibraryChangeTime = {0, 0};
		FILETIME currentFileTime					 = fileAttributes.ftLastWriteTime;
		if (CompareFileTime(&lastDynamicLibraryChangeTime, &currentFileTime) == 0)
		{
			return lfs_unchanged;
		}
		lastDynamicLibraryChangeTime = currentFileTime;
		return lfs_changed;
	}
	return lfs_unknown;
}

lib_handle_t load_dynamic_library(char const* const libFilePath)
{
	lib_handle_t libHandle = LoadLibrary(libFilePath);
	return libHandle;
}

void unload_dynamic_library(lib_handle_t libHandle)
{
	FreeLibrary(libHandle);
}

void* load_func(lib_handle_t libHandle, char const* const funcName)
{
	void* func = GetProcAddress(libHandle, funcName);
	return func;
}
