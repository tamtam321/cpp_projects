# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/tam/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/192.6603.37/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/tam/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/192.6603.37/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/tam/Desktop/PPKE/7_felev/adatszerk/khf3/khf03_1.1/khf03

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/tam/Desktop/PPKE/7_felev/adatszerk/khf3/khf03_1.1/khf03/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/heavyhitters.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/heavyhitters.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/heavyhitters.dir/flags.make

CMakeFiles/heavyhitters.dir/main.cpp.o: CMakeFiles/heavyhitters.dir/flags.make
CMakeFiles/heavyhitters.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tam/Desktop/PPKE/7_felev/adatszerk/khf3/khf03_1.1/khf03/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/heavyhitters.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/heavyhitters.dir/main.cpp.o -c /home/tam/Desktop/PPKE/7_felev/adatszerk/khf3/khf03_1.1/khf03/main.cpp

CMakeFiles/heavyhitters.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/heavyhitters.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tam/Desktop/PPKE/7_felev/adatszerk/khf3/khf03_1.1/khf03/main.cpp > CMakeFiles/heavyhitters.dir/main.cpp.i

CMakeFiles/heavyhitters.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/heavyhitters.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tam/Desktop/PPKE/7_felev/adatszerk/khf3/khf03_1.1/khf03/main.cpp -o CMakeFiles/heavyhitters.dir/main.cpp.s

# Object files for target heavyhitters
heavyhitters_OBJECTS = \
"CMakeFiles/heavyhitters.dir/main.cpp.o"

# External object files for target heavyhitters
heavyhitters_EXTERNAL_OBJECTS =

heavyhitters: CMakeFiles/heavyhitters.dir/main.cpp.o
heavyhitters: CMakeFiles/heavyhitters.dir/build.make
heavyhitters: CMakeFiles/heavyhitters.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/tam/Desktop/PPKE/7_felev/adatszerk/khf3/khf03_1.1/khf03/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable heavyhitters"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/heavyhitters.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/heavyhitters.dir/build: heavyhitters

.PHONY : CMakeFiles/heavyhitters.dir/build

CMakeFiles/heavyhitters.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/heavyhitters.dir/cmake_clean.cmake
.PHONY : CMakeFiles/heavyhitters.dir/clean

CMakeFiles/heavyhitters.dir/depend:
	cd /home/tam/Desktop/PPKE/7_felev/adatszerk/khf3/khf03_1.1/khf03/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tam/Desktop/PPKE/7_felev/adatszerk/khf3/khf03_1.1/khf03 /home/tam/Desktop/PPKE/7_felev/adatszerk/khf3/khf03_1.1/khf03 /home/tam/Desktop/PPKE/7_felev/adatszerk/khf3/khf03_1.1/khf03/cmake-build-debug /home/tam/Desktop/PPKE/7_felev/adatszerk/khf3/khf03_1.1/khf03/cmake-build-debug /home/tam/Desktop/PPKE/7_felev/adatszerk/khf3/khf03_1.1/khf03/cmake-build-debug/CMakeFiles/heavyhitters.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/heavyhitters.dir/depend

