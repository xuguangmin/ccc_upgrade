# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/flx/developer/workspace_c/ccc_upgrade

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/flx/developer/workspace_c/ccc_upgrade/build

# Include any dependencies generated for this target.
include src/main/CMakeFiles/ccc_upgrade.dir/depend.make

# Include the progress variables for this target.
include src/main/CMakeFiles/ccc_upgrade.dir/progress.make

# Include the compile flags for this target's objects.
include src/main/CMakeFiles/ccc_upgrade.dir/flags.make

src/main/CMakeFiles/ccc_upgrade.dir/main.c.o: src/main/CMakeFiles/ccc_upgrade.dir/flags.make
src/main/CMakeFiles/ccc_upgrade.dir/main.c.o: ../src/main/main.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/flx/developer/workspace_c/ccc_upgrade/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object src/main/CMakeFiles/ccc_upgrade.dir/main.c.o"
	cd /home/flx/developer/workspace_c/ccc_upgrade/build/src/main && arm-none-linux-gnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/ccc_upgrade.dir/main.c.o   -c /home/flx/developer/workspace_c/ccc_upgrade/src/main/main.c

src/main/CMakeFiles/ccc_upgrade.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ccc_upgrade.dir/main.c.i"
	cd /home/flx/developer/workspace_c/ccc_upgrade/build/src/main && arm-none-linux-gnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -E /home/flx/developer/workspace_c/ccc_upgrade/src/main/main.c > CMakeFiles/ccc_upgrade.dir/main.c.i

src/main/CMakeFiles/ccc_upgrade.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ccc_upgrade.dir/main.c.s"
	cd /home/flx/developer/workspace_c/ccc_upgrade/build/src/main && arm-none-linux-gnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -S /home/flx/developer/workspace_c/ccc_upgrade/src/main/main.c -o CMakeFiles/ccc_upgrade.dir/main.c.s

src/main/CMakeFiles/ccc_upgrade.dir/main.c.o.requires:
.PHONY : src/main/CMakeFiles/ccc_upgrade.dir/main.c.o.requires

src/main/CMakeFiles/ccc_upgrade.dir/main.c.o.provides: src/main/CMakeFiles/ccc_upgrade.dir/main.c.o.requires
	$(MAKE) -f src/main/CMakeFiles/ccc_upgrade.dir/build.make src/main/CMakeFiles/ccc_upgrade.dir/main.c.o.provides.build
.PHONY : src/main/CMakeFiles/ccc_upgrade.dir/main.c.o.provides

src/main/CMakeFiles/ccc_upgrade.dir/main.c.o.provides.build: src/main/CMakeFiles/ccc_upgrade.dir/main.c.o
.PHONY : src/main/CMakeFiles/ccc_upgrade.dir/main.c.o.provides.build

src/main/CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.o: src/main/CMakeFiles/ccc_upgrade.dir/flags.make
src/main/CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.o: ../src/main/ccc_upgrade.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/flx/developer/workspace_c/ccc_upgrade/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object src/main/CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.o"
	cd /home/flx/developer/workspace_c/ccc_upgrade/build/src/main && arm-none-linux-gnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.o   -c /home/flx/developer/workspace_c/ccc_upgrade/src/main/ccc_upgrade.c

src/main/CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.i"
	cd /home/flx/developer/workspace_c/ccc_upgrade/build/src/main && arm-none-linux-gnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -E /home/flx/developer/workspace_c/ccc_upgrade/src/main/ccc_upgrade.c > CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.i

src/main/CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.s"
	cd /home/flx/developer/workspace_c/ccc_upgrade/build/src/main && arm-none-linux-gnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -S /home/flx/developer/workspace_c/ccc_upgrade/src/main/ccc_upgrade.c -o CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.s

src/main/CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.o.requires:
.PHONY : src/main/CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.o.requires

src/main/CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.o.provides: src/main/CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.o.requires
	$(MAKE) -f src/main/CMakeFiles/ccc_upgrade.dir/build.make src/main/CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.o.provides.build
.PHONY : src/main/CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.o.provides

src/main/CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.o.provides.build: src/main/CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.o
.PHONY : src/main/CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.o.provides.build

# Object files for target ccc_upgrade
ccc_upgrade_OBJECTS = \
"CMakeFiles/ccc_upgrade.dir/main.c.o" \
"CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.o"

# External object files for target ccc_upgrade
ccc_upgrade_EXTERNAL_OBJECTS =

../bin/ccc_upgrade: src/main/CMakeFiles/ccc_upgrade.dir/main.c.o
../bin/ccc_upgrade: src/main/CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.o
../bin/ccc_upgrade: src/main/CMakeFiles/ccc_upgrade.dir/build.make
../bin/ccc_upgrade: src/main/CMakeFiles/ccc_upgrade.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable ../../../bin/ccc_upgrade"
	cd /home/flx/developer/workspace_c/ccc_upgrade/build/src/main && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ccc_upgrade.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/main/CMakeFiles/ccc_upgrade.dir/build: ../bin/ccc_upgrade
.PHONY : src/main/CMakeFiles/ccc_upgrade.dir/build

src/main/CMakeFiles/ccc_upgrade.dir/requires: src/main/CMakeFiles/ccc_upgrade.dir/main.c.o.requires
src/main/CMakeFiles/ccc_upgrade.dir/requires: src/main/CMakeFiles/ccc_upgrade.dir/ccc_upgrade.c.o.requires
.PHONY : src/main/CMakeFiles/ccc_upgrade.dir/requires

src/main/CMakeFiles/ccc_upgrade.dir/clean:
	cd /home/flx/developer/workspace_c/ccc_upgrade/build/src/main && $(CMAKE_COMMAND) -P CMakeFiles/ccc_upgrade.dir/cmake_clean.cmake
.PHONY : src/main/CMakeFiles/ccc_upgrade.dir/clean

src/main/CMakeFiles/ccc_upgrade.dir/depend:
	cd /home/flx/developer/workspace_c/ccc_upgrade/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/flx/developer/workspace_c/ccc_upgrade /home/flx/developer/workspace_c/ccc_upgrade/src/main /home/flx/developer/workspace_c/ccc_upgrade/build /home/flx/developer/workspace_c/ccc_upgrade/build/src/main /home/flx/developer/workspace_c/ccc_upgrade/build/src/main/CMakeFiles/ccc_upgrade.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/main/CMakeFiles/ccc_upgrade.dir/depend
