# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/davide/Scrivania/Primo-Assignment-middle-end/PrimoAssignement/Strenght-Reduction

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/davide/Scrivania/Primo-Assignment-middle-end/PrimoAssignement/Strenght-Reduction/build

# Include any dependencies generated for this target.
include CMakeFiles/StrRed.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/StrRed.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/StrRed.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/StrRed.dir/flags.make

CMakeFiles/StrRed.dir/Pass2.cpp.o: CMakeFiles/StrRed.dir/flags.make
CMakeFiles/StrRed.dir/Pass2.cpp.o: /home/davide/Scrivania/Primo-Assignment-middle-end/PrimoAssignement/Strenght-Reduction/Pass2.cpp
CMakeFiles/StrRed.dir/Pass2.cpp.o: CMakeFiles/StrRed.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/davide/Scrivania/Primo-Assignment-middle-end/PrimoAssignement/Strenght-Reduction/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/StrRed.dir/Pass2.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/StrRed.dir/Pass2.cpp.o -MF CMakeFiles/StrRed.dir/Pass2.cpp.o.d -o CMakeFiles/StrRed.dir/Pass2.cpp.o -c /home/davide/Scrivania/Primo-Assignment-middle-end/PrimoAssignement/Strenght-Reduction/Pass2.cpp

CMakeFiles/StrRed.dir/Pass2.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/StrRed.dir/Pass2.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/davide/Scrivania/Primo-Assignment-middle-end/PrimoAssignement/Strenght-Reduction/Pass2.cpp > CMakeFiles/StrRed.dir/Pass2.cpp.i

CMakeFiles/StrRed.dir/Pass2.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/StrRed.dir/Pass2.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/davide/Scrivania/Primo-Assignment-middle-end/PrimoAssignement/Strenght-Reduction/Pass2.cpp -o CMakeFiles/StrRed.dir/Pass2.cpp.s

# Object files for target StrRed
StrRed_OBJECTS = \
"CMakeFiles/StrRed.dir/Pass2.cpp.o"

# External object files for target StrRed
StrRed_EXTERNAL_OBJECTS =

libStrRed.so: CMakeFiles/StrRed.dir/Pass2.cpp.o
libStrRed.so: CMakeFiles/StrRed.dir/build.make
libStrRed.so: CMakeFiles/StrRed.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/davide/Scrivania/Primo-Assignment-middle-end/PrimoAssignement/Strenght-Reduction/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library libStrRed.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/StrRed.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/StrRed.dir/build: libStrRed.so
.PHONY : CMakeFiles/StrRed.dir/build

CMakeFiles/StrRed.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/StrRed.dir/cmake_clean.cmake
.PHONY : CMakeFiles/StrRed.dir/clean

CMakeFiles/StrRed.dir/depend:
	cd /home/davide/Scrivania/Primo-Assignment-middle-end/PrimoAssignement/Strenght-Reduction/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/davide/Scrivania/Primo-Assignment-middle-end/PrimoAssignement/Strenght-Reduction /home/davide/Scrivania/Primo-Assignment-middle-end/PrimoAssignement/Strenght-Reduction /home/davide/Scrivania/Primo-Assignment-middle-end/PrimoAssignement/Strenght-Reduction/build /home/davide/Scrivania/Primo-Assignment-middle-end/PrimoAssignement/Strenght-Reduction/build /home/davide/Scrivania/Primo-Assignment-middle-end/PrimoAssignement/Strenght-Reduction/build/CMakeFiles/StrRed.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/StrRed.dir/depend

