This is a very basic demonstration of hot-reloading on C/C++.

It does not aim to cover all corner cases nor be a drop-in replacement for more full-featured solutions such as jet-live or blink, but rather explore how such functionality may be achieved in as simple a manner as I could think of.

This approach has worked well enough in small, personal game projects (hence the "game" files being used as the hot-reloadable portion). How far would it scale to larger codebases? Probably not much, but no such tests were made so far.

By nature of its simplicity, 3 big assumptions or compromises exist:

1. The program is split in at lest 2 parts, one which will be the entry point (where the main function resides if you will) and another which will be hot-reloadable;
2. The hot-reloadable portion of the program must be compiled as a dynamic library;
3. The entry point and the hot-reloadable binaries are in the same folder

Other than that, it may be worth expanding a bit upon how the example itself performs the hot-reloading: namely by constantly checking the dynamic library for some change.

This approach was selected purely because, as previously stated, this has been used for game development. Games typically have an update loop running constantly, so it felt natural to just lean on this.
There are, however, ways to have the OS callback into your program notifying whenever a file changes, which probably would work better on other types of applications (non real-time). And, of course, it is always possible to await for some explicit user input telling the program to actually perform the reloading.

It is possible to move the loop inside the library and have it callback into the main application via a function pointer whenever hot-reloading is needed. There's no great reason behind not taking this approach other than the fact it felt a bit easier to do it as presented here.

