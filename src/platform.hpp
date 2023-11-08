#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include <stdio.h>

#ifdef LINUX

#define LIB_PREFIX "lib"
#define LIB_EXTENSION ".so"

#else

#ifdef WINDOWS

#define LIB_PREFIX
#define LIB_EXTENSION ".dll"

#endif

#endif

enum lib_file_state : unsigned char
{
	lfs_unknown,
	lfs_unchanged,
	lfs_changed,
};

typedef void* lib_handle_t;
typedef unsigned int file_open_error_t;

file_open_error_t open_file(FILE** outFile, char const* const fileName, char const* const accessFlags);
lib_file_state check_lib_file_state(char const* const dynamicLibraryFileRelativePath);

lib_handle_t load_dynamic_library(char const* const libFilePath);
void unload_dynamic_library(lib_handle_t libHandle);

void* load_func(lib_handle_t libHandle, char const* const funcName);

#ifdef __cplusplus
template <typename funcT>
bool try_load_func(lib_handle_t handle, char const* const funcName, funcT* outFunc)
{
	*outFunc = (funcT)load_func(handle, funcName);
	return *outFunc != nullptr;
}
#define TRY_LOAD_FUNC(funcT, handle, func) try_load_func<funcT>(handle, #func, &func)
#else
#define TRY_LOAD_FUNC(funcT, handle, func) ((func = (funcT)load_func(handle, #func)) != nullptr)
#endif

void thread_sleep(unsigned long ms);

#endif // PLATFORM_HPP
