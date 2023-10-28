#include "platform.hpp"

#include <stdio.h>
#include <windows.h>

lib_handle_t load_dynamic_library(char const* const libFilePath)
{
	HMODULE winLibHandle = LoadLibrary(libFilePath);

	DWORD error = GetLastError();
	if (error)
	{
		LPVOID errorMessage;
		DWORD errorMessageSize = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			error,
			0,
			(LPSTR)&errorMessage,
			0,
			NULL
		);

		if (errorMessageSize != 0) {
			printf("Failed to load DLL: %s\n", (LPSTR)errorMessage);
			LocalFree(errorMessage);
		} else {
			printf("Failed to load DLL with error code: %lu\n", error);
		}
	}

	lib_handle_t libHandle = (lib_handle_t)winLibHandle;
	return libHandle;
}

void unload_dynamic_library(lib_handle_t libHandle)
{
	HMODULE winLibHandle = (HMODULE)libHandle;
	FreeLibrary(winLibHandle);
}

void* load_func(lib_handle_t libHandle, char const* const funcName)
{
	HMODULE winLibHandle = (HMODULE)libHandle;
	FARPROC winFunc = GetProcAddress(winLibHandle, funcName);
	void* func = (void*)winFunc;
	if (func == nullptr)
	{
		DWORD error = GetLastError();
		LPVOID errorMessage;
        DWORD errorMessageSize = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            error,
            0,
            (LPSTR)&errorMessage,
            0,
            NULL
        );

		printf("Error loading function \"%s\" with error code: %lu\n", funcName, error);
        if (errorMessageSize != 0) 
		{
            printf("\t%s\n", (LPSTR)errorMessage);
            LocalFree(errorMessage);
        }
	}
	return func;
}

void thread_sleep(unsigned long ms)
{
	Sleep(ms);
}

unsigned int open_file(FILE** outFile, char const* const fileName, char const* const accessFlags)
{
	errno_t error = fopen_s(outFile, fileName,accessFlags);
	return error;
}
