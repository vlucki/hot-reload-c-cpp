#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "static-lib/greeter.hpp"
#include "platform.hpp"

#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x

// Fallback if not defined in build script
#ifndef DYNAMIC_LIB_NAME
#define DYNAMIC_LIB_NAME dynamic
#endif

#define LIB_NAME LIB_PREFIX STRINGIFY(DYNAMIC_LIB_NAME)
#define LIB_COPY_NAME LIB_NAME "-copy"

#define LIB_REL_PATH "./" LIB_NAME LIB_EXTENSION
#define LIB_COPY_REL_PATH "./" LIB_COPY_NAME LIB_EXTENSION

using updateFuncT = void (*)(unsigned char*);

updateFuncT updateFunc;
void print_name(char const* const name)
{
	printf("%s", name);
}

enum hot_reload_result : unsigned char
{
	hrr_no_reload,
	hrr_reload_failed,
	hrr_reload_succeeded
};

static int copy_file(char const* const originalFileName, char  const* const newFileName)
{
	FILE* originalFile = nullptr;
	FILE* newFile = nullptr;
	file_open_error_t errorOpeningOriginalFile = 0;
	file_open_error_t errorOpeningNewFile = 0;
	errorOpeningOriginalFile = open_file(&originalFile, originalFileName, "rb");
	errorOpeningNewFile 	 = open_file(&newFile, newFileName, "wb");

	if(errorOpeningOriginalFile != 0)
	{
		return  -1;
	}

	if(errorOpeningNewFile != 0)
	{
		fclose(originalFile);
		return  -1;
	}

	while (1)
	{
		int dataInOriginalFile = fgetc(originalFile);

		if(!feof(originalFile))
		{
			fputc(dataInOriginalFile, newFile);
		}
		else
		{
			break;
		}
	}

	fclose(newFile);
	fclose(originalFile);
	return  0;
}

static hot_reload_result try_hot_reload(void** gameLibHandle, char const* const dynamicLibraryFileRelativePath, char const* const dynamicLibraryFileCopyRelativePath)
{
	switch (check_lib_file_state(dynamicLibraryFileRelativePath))
	{
	case lfs_unknown:
		printf("Could not check if dynamic library changed\n\n");
		return hrr_reload_failed;
	case lfs_unchanged:
		return hrr_no_reload;
	case lfs_changed:
		printf("\n\nDetected change in dynamic library\n");
		break;
	}


	if (*gameLibHandle != nullptr)
	{
		// Slightly larger (0.1s) delay to ensure the file has finished being written
		thread_sleep(100);
		printf("Unloading library\n");
		unload_dynamic_library(*gameLibHandle);
	}

	copy_file(dynamicLibraryFileRelativePath, dynamicLibraryFileCopyRelativePath);

	*gameLibHandle = load_dynamic_library(dynamicLibraryFileCopyRelativePath);
	if (*gameLibHandle != nullptr)
	{
		printf("Dynamic library successfully loaded\n\n");

		using helloPrinterFuncT = void (*)(void (*)());
		using namePrinterFuncT	= void (*)(void (*)(char const* const));

		namePrinterFuncT hookUpLocalPrintNameFunction;
		helloPrinterFuncT hookUpStaticLibSaydHelloFunction;

		if (try_load_func(*gameLibHandle, "update", &updateFunc) &&
			try_load_func(*gameLibHandle, "load_name_printer", &hookUpLocalPrintNameFunction) &&
			try_load_func(*gameLibHandle, "load_hello_printer", &hookUpStaticLibSaydHelloFunction))
		{
			hookUpLocalPrintNameFunction(&print_name);
			hookUpStaticLibSaydHelloFunction(&greet);
			return hrr_reload_succeeded;
		}
		else
		{
			*gameLibHandle = nullptr;
		}
	}
	return hrr_reload_failed;
}

int main()
{
	void* dynamicLibraryHandle = nullptr;

#define PERSISTENT_MEM_SIZE 1024

	unsigned char* persistentMemory = (unsigned char*)malloc(PERSISTENT_MEM_SIZE);
	memset(persistentMemory, 0, PERSISTENT_MEM_SIZE);

	while (true)
	{
        static char const * const libRelPath = LIB_REL_PATH;
        static char const * const libCopyRelPath = LIB_COPY_REL_PATH;
		hot_reload_result hotReloadResult = try_hot_reload(&dynamicLibraryHandle, libRelPath, libCopyRelPath);
		switch (hotReloadResult)
		{
		case hrr_no_reload: {
			updateFunc(persistentMemory);

			// Very sloppy way to simulat ~60 fps
			thread_sleep(16);
			break;
		}
		case hrr_reload_succeeded:
			break;
		case hrr_reload_failed:
			return 1;
		}
	}

	free(persistentMemory);

	return 0;
}
