# Introduction

This is a _very basic_ demonstration of hot-reloading on C/C++.

It does not aim to cover all corner cases nor be a drop-in replacement for more full-featured solutions such as [jet-live](https://github.com/ddovod/jet-live) or [blink](https://github.com/crosire/blink), but rather explore how such functionality may be achieved in as simple a manner as I could think of.

This approach has worked well enough in small, personal game projects (hence the "game" files being used as the hot-reloadable portion). How far would it scale to larger codebases? Probably not much, but no such tests were made so far.

By nature of its simplicity, 2 big assumptions or compromises exist:

1. The program is split in at lest 2 parts, one which will be the entry point (where the main function resides if you will) and another which will be hot-reloadable;
2. The entry point and the hot-reloadable binaries are in the same folder.

# How does it work

As stated, the program is split in 2 (at least). The entry point is the actual executable, the thing that is run. The hot-reloadable portion is a dynamic library which the main program loads at runtime.

Since this example has been used with games, it takes advantage of the fact games (usually) have a constantly running loop. Each iteration, the code checks if the library file has changed and, if so, unloads and re-loads it.

The unloading is fairly simple: just pass the handle to whatever OS function exists. Loading, however, must be done in 2 steps:
1. Make a copy of the library, overwriting any pre-existing file;
2. Load the library using the copy;

This was required because Windows locks the DLL file until it is unloaded, thus preventing a new one from being created. By making a copy, we free up the original to be overwritten as required.

It may be noted that there are some platform-specific code for the file-related stuff as well. The choice to avoid std::filesystem in favor of a more C-like approach was done precisely to make the code work with C as well as C++. Same goes for the use of `malloc` and `free` in lieu of `new` and `delete`.

# Possible alternatives

There are ways to have the OS callback into your program notifying whenever a file changes, which probably would lessen the IO burden on each loop and definitely would work better on other types of applications (non real-time). But the impact felt negligible when put up against the ease of just leaning into the update loop. Still, this approach may be explored in the future if only for the sake of having another example.

Another option is to await for some explicit user input telling the program to perform the reloading, which would eliminate the need for any file-checking, but I found this to be a bit more tedious than just re-compiling and having it "magically work".

Finally, the whole loop could be moved to the library, which would call back into the main application via a function pointer whenever hot-reloading is needed. There's no great reason behind not taking this approach other than the fact it felt a bit easier to do it as presented here, but perhaps one could argue that the entire hot-reloading code is indeed something apart from the "real" application defined by the library and thus should stay out of it.

# Building from source

This project relies on [xmake](https://github.com/xmake-io/xmake) for building and all relevant configuration may be found within the [xmake.lua](https://github.com/vlucki/hot-reload-c-cpp/blob/main/xmake.lua) file. 

The [build.sh](https://github.com/vlucki/hot-reload-c-cpp/blob/main/build.sh) file exists only because sometimes I have to rely on different systems, so I can have it call whatever the project uses without changing my personal keybindings. 

This is probably a good time to mention most of the development has been done on Linux using [neovim](https://github.com/neovim/neovim), hence the .sh file. On Windows I've relied on [Visual Studio Code](https://github.com/microsoft/vscode) (relying on its [xmake plugin](https://github.com/xmake-io/xmake-vscode) for building and such) and mostly worked on the code specific to the platform (well, and tested to make sure it at least seemed to work).

