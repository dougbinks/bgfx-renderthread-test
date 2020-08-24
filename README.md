# bgfx-renderthread-test
BGFX Render Thread Test

This project is a test of using [bgfx](https://github.com/bkaradzic/bgfx) to render with multithreaded API thread and bgfx render thread with.

Supports Windows, Linux X11, OSX.

The backend defaults to OpenGL, but can be changed to other backends (or Count, which chooses default) by modifying the [preferredRenderer](https://github.com/dougbinks/bgfx-renderthread-test/blob/master/bgfx-renderthread-test.cpp#L24).

## Using CMake to create the project

From a command prompt in the root project directory:
1. `mkdir build`
1. `cd build`
1. `cmake ..` or for Xcode `cmake -G Xcode ..`
1. Either run `make all`, for Visual Studio open .sln file, or for Xcode open project
