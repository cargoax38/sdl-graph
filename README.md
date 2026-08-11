# sdl-graph
Making a small application to explore graph on a 3D environment.

# Dependencies

+ CMake > 3.23 (you can check your version with `cmake --version`)
+ SDL3 15.2.0 (latest version for now)

# Compiling

The first step is to make small modifications to tell to CMake where your SDL is located

## Windows

On the root folder of the projet, run `cmake -S . -B build -G Ninja`. The `-S` indicates that the current folder is the actualy the root file and the `-B` indicates where to put the build files (not present in that repository).
To compile the application, run `cmake --build build` (awalys inside the root file). The `.exe` should be located somewhere in the `build` folder.

To execute it, you will have to put a `SDL3.dll` next to the executable.
