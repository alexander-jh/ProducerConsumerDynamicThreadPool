# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

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
CMAKE_COMMAND = /snap/clion/163/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/163/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/cmake-build-debug"

# Include any dependencies generated for this target.
include CMakeFiles/Project1.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/Project1.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Project1.dir/flags.make

CMakeFiles/Project1.dir/executor.c.o: CMakeFiles/Project1.dir/flags.make
CMakeFiles/Project1.dir/executor.c.o: ../executor.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/Project1.dir/executor.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Project1.dir/executor.c.o -c "/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/executor.c"

CMakeFiles/Project1.dir/executor.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Project1.dir/executor.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/executor.c" > CMakeFiles/Project1.dir/executor.c.i

CMakeFiles/Project1.dir/executor.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Project1.dir/executor.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/executor.c" -o CMakeFiles/Project1.dir/executor.c.s

CMakeFiles/Project1.dir/threads.c.o: CMakeFiles/Project1.dir/flags.make
CMakeFiles/Project1.dir/threads.c.o: ../threads.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/Project1.dir/threads.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Project1.dir/threads.c.o -c "/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/threads.c"

CMakeFiles/Project1.dir/threads.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Project1.dir/threads.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/threads.c" > CMakeFiles/Project1.dir/threads.c.i

CMakeFiles/Project1.dir/threads.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Project1.dir/threads.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/threads.c" -o CMakeFiles/Project1.dir/threads.c.s

CMakeFiles/Project1.dir/atomic_queue.c.o: CMakeFiles/Project1.dir/flags.make
CMakeFiles/Project1.dir/atomic_queue.c.o: ../atomic_queue.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/Project1.dir/atomic_queue.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Project1.dir/atomic_queue.c.o -c "/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/atomic_queue.c"

CMakeFiles/Project1.dir/atomic_queue.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Project1.dir/atomic_queue.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/atomic_queue.c" > CMakeFiles/Project1.dir/atomic_queue.c.i

CMakeFiles/Project1.dir/atomic_queue.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Project1.dir/atomic_queue.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/atomic_queue.c" -o CMakeFiles/Project1.dir/atomic_queue.c.s

CMakeFiles/Project1.dir/transform.c.o: CMakeFiles/Project1.dir/flags.make
CMakeFiles/Project1.dir/transform.c.o: ../transform.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/Project1.dir/transform.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Project1.dir/transform.c.o -c "/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/transform.c"

CMakeFiles/Project1.dir/transform.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Project1.dir/transform.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/transform.c" > CMakeFiles/Project1.dir/transform.c.i

CMakeFiles/Project1.dir/transform.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Project1.dir/transform.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/transform.c" -o CMakeFiles/Project1.dir/transform.c.s

# Object files for target Project1
Project1_OBJECTS = \
"CMakeFiles/Project1.dir/executor.c.o" \
"CMakeFiles/Project1.dir/threads.c.o" \
"CMakeFiles/Project1.dir/atomic_queue.c.o" \
"CMakeFiles/Project1.dir/transform.c.o"

# External object files for target Project1
Project1_EXTERNAL_OBJECTS = \
"/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/transformMat.o"

Project1: CMakeFiles/Project1.dir/executor.c.o
Project1: CMakeFiles/Project1.dir/threads.c.o
Project1: CMakeFiles/Project1.dir/atomic_queue.c.o
Project1: CMakeFiles/Project1.dir/transform.c.o
Project1: ../transformMat.o
Project1: CMakeFiles/Project1.dir/build.make
Project1: CMakeFiles/Project1.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_5) "Linking C executable Project1"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Project1.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Project1.dir/build: Project1
.PHONY : CMakeFiles/Project1.dir/build

CMakeFiles/Project1.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Project1.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Project1.dir/clean

CMakeFiles/Project1.dir/depend:
	cd "/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1" "/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1" "/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/cmake-build-debug" "/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/cmake-build-debug" "/home/ajw/Dropbox/OSU/CSE5541 - Parallel Computing/Project1/cmake-build-debug/CMakeFiles/Project1.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/Project1.dir/depend

