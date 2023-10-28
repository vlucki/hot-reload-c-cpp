#include "platform.hpp"

#include <sys/stat.h>


lib_file_state check_lib_file_state(char const* const dynamicLibraryFileRelativePath)
{
	struct stat file_stat;
	if (stat(dynamicLibraryFileRelativePath, &file_stat) == 0)
	{
		static time_t lastDynamicLibraryChangeTime = 0;
		if (lastDynamicLibraryChangeTime == file_stat.st_mtime)
		{
			return lfs_unchanged;
		}
		lastDynamicLibraryChangeTime = file_stat.st_mtime;
		return lfs_changed;
	}
	return lfs_unknown;
}

