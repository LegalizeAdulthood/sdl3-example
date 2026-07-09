[![CMake workflow](https://github.com/LegalizeAdulthood/sdl3-example/actions/workflows/cmake.yml/badge.svg)](https://github.com/LegalizeAdulthood/sdl3-example/actions/workflows/cmake.yml)

# SDL3 Example

Sample code for the video [Modern GPU Programming with SDL3](https://www.youtube.com/watch?v=rnbA-0kQ4eM).

See also:
* [GPU category](https://wiki.libsdl.org/SDL3/CategoryGPU) in the SDL3 documentation
* [GPU C examples](https://github.com/TheSpydog/SDL_gpu_examples) on github

# Obtaining the Source

Use git to clone this repository, then update the vcpkg submodule to bootstrap
the dependency process.

```
git clone https://github.com/LegalizeAdulthood/sdl3-example
cd sdl3-exampleproject-template
git submodule init
git submodule update --depth 1
```

# Building

A CMake preset has been provided to perform the usual CMake steps of
configure, build and test.

```
cmake --workflow --preset default
```

Places the build outputs in a sibling directory of the source code directory, e.g. up
and outside of the source directory.

[Utah C++ Programmers](https://meetup.com/utah-cpp-programmers)\
[Past Topics](https://utahcpp.wordpress.com/past-meeting-topics/)\
[Future Topics](https://utahcpp.wordpress.com/future-meeting-topics/)
