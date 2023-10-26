#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "static-lib/greeter.hpp"
#include "platform.hpp"

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

// TODO: use this to make a copy of the DLL before loading it
static int copy_file(char const* const old_filename, char  const* const new_filename)
{
	FILE  *ptr_old, *ptr_new;
	errno_t err = 0, err1 = 0;
	int  a;

	err = fopen_s(&ptr_old, old_filename, "rb");
	err1 = fopen_s(&ptr_new, new_filename, "wb");

	if(err != 0)
		return  -1;

	if(err1 != 0)
	{
		fclose(ptr_old);
		return  -1;
	}

	while(1)
	{
		a  =  fgetc(ptr_old);

		if(!feof(ptr_old))
			fputc(a, ptr_new);
		else
			break;
	}

	fclose(ptr_new);
	fclose(ptr_old);
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


	// TODO: unfortunately we can't just load the library and be done with it on windows, because it locks the dll
	// so we must try some other approach. Perhaps renaming it would work? Then we have to delete or something after unloading, assuming the system permits...
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

	static char const* const dynamicLibraryFileRelativePath = "./dynamic.dll";
	static char const* const dynamicLibraryFileCopyRelativePath = "./dynamic-copy.dll";
	while (true)
	{
		hot_reload_result hotReloadResult = try_hot_reload(&dynamicLibraryHandle, dynamicLibraryFileRelativePath, dynamicLibraryFileCopyRelativePath);
		switch (hotReloadResult)
		{
		case hrr_no_reload: {
			updateFunc(persistentMemory);

			// Very sloppy way to simulat ~60 fps
			thread_sleep(16);
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
