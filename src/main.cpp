#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

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

static hot_reload_result try_hot_reload(void** gameLibHandle, char const* const dynamicLibraryFileRelativePath)
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
		timespec ts = {.tv_sec = 0, .tv_nsec = 100000000};
		thrd_sleep(&ts, nullptr);
		printf("Unloading library\n");
		unload_dynamic_library(*gameLibHandle);
	}

	*gameLibHandle = load_dynamic_library(dynamicLibraryFileRelativePath);
	if (gameLibHandle != nullptr)
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

	unsigned char* persistentMemory = (unsigned char*)malloc(1024);

	static char const* const dynamicLibraryFileRelativePath = "./libdynamic.so";
	while (true)
	{
		hot_reload_result hotReloadResult = try_hot_reload(&dynamicLibraryHandle, dynamicLibraryFileRelativePath);
		switch (hotReloadResult)
		{
		case hrr_no_reload: {
			updateFunc(persistentMemory);

			// Very sloppy way to simulat ~60 fps
			timespec ts = {.tv_sec = 0, .tv_nsec = 16666666};
			thrd_sleep(&ts, nullptr);
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
