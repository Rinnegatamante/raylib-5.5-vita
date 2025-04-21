#******************************************************************************
#
#   raylib makefile
#
#   This file supports building raylib library for the following platforms:
#
#     > PLATFORM_DESKTOP
#         - Defaults to PLATFORM_DESKTOP_GLFW
#     > PLATFORM_DESKTOP_GLFW (GLFW backend):
#         - Windows (Win32, Win64)
#         - Linux (X11/Wayland desktop mode)
#         - macOS/OSX (x64, arm64)
#         - FreeBSD, OpenBSD, NetBSD, DragonFly (X11 desktop)
#     > PLATFORM_DESKTOP_SDL (SDL backend):
#         - Windows (Win32, Win64)
#         - Linux (X11/Wayland desktop mode)
#         - Others (not tested)
#     > PLATFORM_DESKTOP_RGFW (RGFW backend):
#         - Windows (Win32, Win64)
#         - Linux (X11 desktop mode)
#         - macOS/OSX (x64, arm64 (not tested))
#         - Others (not tested)
#     > PLATFORM_WEB:
#         - HTML5 (WebAssembly)
#     > PLATFORM_DRM:
#         - Raspberry Pi 0-5 (DRM/KMS)
#         - Linux DRM subsystem (KMS mode)
#     > PLATFORM_ANDROID:
#         - Android (ARM, ARM64)
#    PLATFORM_VITA:     PlayStation Vita
#
#   Many thanks to Milan Nikolic (@gen2brain) for implementing Android platform pipeline.
#   Many thanks to Emanuele Petriglia for his contribution on GNU/Linux pipeline.
#
#   Copyright (c) 2013-2024 Ramon Santamaria (@raysan5)
#
#   This software is provided "as-is", without any express or implied warranty. In no event
#   will the authors be held liable for any damages arising from the use of this software.
#
#   Permission is granted to anyone to use this software for any purpose, including commercial
#   applications, and to alter it and redistribute it freely, subject to the following restrictions:
#
#     1. The origin of this software must not be misrepresented; you must not claim that you
#     wrote the original software. If you use this software in a product, an acknowledgment
#     in the product documentation would be appreciated but is not required.
#
#     2. Altered source versions must be plainly marked as such, and must not be misrepresented
#     as being the original software.
#
#     3. This notice may not be removed or altered from any source distribution.
#
#**************************************************************************************************

# NOTE: Highly recommended to read the raylib Wiki to know how to compile raylib for different platforms
# https://github.com/raysan5/raylib/wiki

.PHONY: all clean install uninstall

# Define required environment variables
#------------------------------------------------------------------------------------------------
# Define target platform
PLATFORM             ?= PLATFORM_VITA

TARGET_PLATFORM = $(PLATFORM)


# Define required raylib variables
RAYLIB_VERSION        = 5.5.0
RAYLIB_API_VERSION    = 550

# Define raylib source code path
RAYLIB_SRC_PATH      ?= ../src

# Define output directory for compiled library, defaults to src directory
# NOTE: If externally provided, make sure directory exists
RAYLIB_RELEASE_PATH  ?= $(RAYLIB_SRC_PATH)

# Library type used for raylib: STATIC (.a) or SHARED (.so/.dll)
RAYLIB_LIBTYPE       ?= STATIC

# Build mode for library: DEBUG or RELEASE
RAYLIB_BUILD_MODE    ?= DEBUG

# Build output name for the library
RAYLIB_LIB_NAME      ?= raylib

# Define resource file for DLL properties
RAYLIB_RES_FILE      ?= ./raylib.dll.rc.data

# Define external config flags
# NOTE: It will override config.h flags with the provided ones,
# if NONE, default config.h flags are used
RAYLIB_CONFIG_FLAGS  ?= NONE

# To define additional cflags: Use make CUSTOM_CFLAGS=""

# Include raylib modules on compilation
# NOTE: Some programs like tools could not require those modules
RAYLIB_MODULE_AUDIO  ?= FALSE
RAYLIB_MODULE_MODELS ?= TRUE
RAYLIB_MODULE_RAYGUI ?= FALSE

# NOTE: Additional libraries have been moved to their own repos:
# raygui: https://github.com/raysan5/raygui
RAYLIB_MODULE_RAYGUI_PATH ?= $(RAYLIB_SRC_PATH)/../../raygui/src

# Use external GLFW library instead of rglfw module
USE_EXTERNAL_GLFW     ?= TRUE

# Enable support for X11 by default on Linux when using GLFW
# NOTE: Wayland is disabled by default, only enable if you are sure
GLFW_LINUX_ENABLE_WAYLAND  ?= FALSE
GLFW_LINUX_ENABLE_X11      ?= FALSE

# PLATFORM_DESKTOP_SDL: It requires SDL library to be provided externally
# WARNING: Library is not included in raylib, it MUST be configured by users
SDL_INCLUDE_PATH      ?= $(RAYLIB_SRC_PATH)/external/SDL2/include
SDL_LIBRARY_PATH      ?= $(RAYLIB_SRC_PATH)/external/SDL2/lib
SDL_LIBRARIES         ?= -lSDL2 -lSDL2main


# Determine if the file has root access (only required to install raylib)
# "whoami" prints the name of the user that calls him (so, if it is the root user, "whoami" prints "root")
ROOT = $(whoami)

# By default we suppose we are working on Windows
HOST_PLATFORM_OS ?= LINUX
PLATFORM_OS = VITA
PLATFORM_SHELL = sh
GRAPHICS = GRAPHICS_API_OPENGL_ES2


# Define default C compiler and archiver to pack library: CC, AR
#------------------------------------------------------------------------------------------------

CC = arm-vita-eabi-gcc
AR = arm-vita-eabi-ar

# Define compiler flags: CFLAGS
#------------------------------------------------------------------------------------------------
#  -O1                      defines optimization level
#  -g                       include debug information on compilation
#  -s                       strip unnecessary data from build --> linker
#  -Wall                    turns on most, but not all, compiler warnings
#  -std=c99                 defines C language mode (standard C from 1999 revision)
#  -std=gnu99               defines C language mode (GNU C from 1999 revision)
#  -Wno-missing-braces      ignore invalid warning (GCC bug 53119)
#  -Wno-unused-value        ignore unused return values of some functions (i.e. fread())
#  -D_DEFAULT_SOURCE        use with -std=c99 on Linux and PLATFORM_WEB, required for timespec
#  -D_GNU_SOURCE            access to lots of nonstandard GNU/Linux extension functions
#  -Werror=pointer-arith    catch unportable code that does direct arithmetic on void pointers


# Additional flags for compiler (if desired)
#  -Wextra                  enables some extra warning flags that are not enabled by -Wall
#  -Wmissing-prototypes     warn if a global function is defined without a previous prototype declaration
#  -Wstrict-prototypes      warn if a function is declared or defined without specifying the argument types
#  -Werror=implicit-function-declaration   catch function calls without prior declaration

# Define include paths for required headers: INCLUDE_PATHS
# NOTE: Several external required libraries (stb and others)
#------------------------------------------------------------------------------------------------
INCLUDE_PATHS = -I. $(EXTRA_INCLUDE_PATHS)

# Define additional directories containing required header files
CFLAGS = -Wl,-q -Wall -Wno-incompatible-pointer-types -Wno-pointer-sign -O3 -nostartfiles -DVITA -D$(PLATFORM) -D$(GRAPHICS)
INCLUDE_PATHS = -I.
INCLUDE_PATHS += -I$(SDL_INCLUDE_PATH)

# Define library paths containing required libs: LDFLAGS
# NOTE: This is only required for dynamic library generation
#------------------------------------------------------------------------------------------------
LDFLAGS = $(CUSTOM_LDFLAGS) -L. -L$(RAYLIB_RELEASE_PATH)
LDFLAGS += -Wl,-soname,lib$(RAYLIB_LIB_NAME).so.$(RAYLIB_API_VERSION)
LDFLAGS += -L$(SDL_LIBRARY_PATH)

# Define libraries required on linking: LDLIBS
# NOTE: This is only required for dynamic library generation
#------------------------------------------------------------------------------------------------

LDLIBS = -lGL -lc -lm -lpthread -ldl -lrt
LDLIBS += $(SDL_LIBRARIES)

# Define source code object files required
#------------------------------------------------------------------------------------------------
OBJS = rcore.o \
       rshapes.o \
       rtextures.o \
       rtext.o \
       utils.o 

ifeq ($(RAYLIB_MODULE_MODELS),TRUE)
    OBJS += rmodels.o
endif
ifeq ($(RAYLIB_MODULE_AUDIO),TRUE)
    OBJS += raudio.o
endif
ifeq ($(RAYLIB_MODULE_RAYGUI),TRUE)
    OBJS += raygui.o
endif
# Define processes to execute
#------------------------------------------------------------------------------------------------
# Default target entry
all: raylib

# Compile raylib library
# NOTE: Release directory is created if not exist
raylib: $(OBJS)

# Compile raylib static library version $(RAYLIB_VERSION)
# WARNING: You should type "make clean" before doing this target.
	$(AR) rcs $(RAYLIB_RELEASE_PATH)/lib$(RAYLIB_LIB_NAME).a $(OBJS)
	@echo "raylib static library generated (lib$(RAYLIB_LIB_NAME).a) in $(RAYLIB_RELEASE_PATH)!"
# Compile all modules with their prerequisites

# Prerequisites of core module
rcore.o : platforms/*.c

# Compile core module
rcore.o : rcore.c raylib.h rlgl.h utils.h raymath.h rcamera.h rgestures.h
	$(CC) -c $< $(CFLAGS) $(INCLUDE_PATHS)

# Compile rglfw module
rglfw.o : rglfw.c
	$(CC) $(GLFW_OSX) -c $< $(CFLAGS) $(INCLUDE_PATHS)

# Compile shapes module
rshapes.o : rshapes.c raylib.h rlgl.h
	$(CC) -c $< $(CFLAGS) $(INCLUDE_PATHS)

# Compile textures module
rtextures.o : rtextures.c raylib.h rlgl.h utils.h
	$(CC) -c $< $(CFLAGS) $(INCLUDE_PATHS)

# Compile text module
rtext.o : rtext.c raylib.h utils.h
	$(CC) -c $< $(CFLAGS) $(INCLUDE_PATHS)

# Compile utils module
utils.o : utils.c utils.h
	$(CC) -c $< $(CFLAGS) $(INCLUDE_PATHS)

# Compile models module
rmodels.o : rmodels.c raylib.h rlgl.h raymath.h
	$(CC) -c $< $(CFLAGS) $(INCLUDE_PATHS)

# Compile audio module
raudio.o : raudio.c raylib.h
	$(CC) -c $< $(CFLAGS) $(INCLUDE_PATHS)

# Compile raygui module
# NOTE: raygui header should be distributed with raylib.h
raygui.o : raygui.c
	$(CC) -c $< $(CFLAGS) $(INCLUDE_PATHS)
raygui.c:

# Install generated and needed files to desired directories.
# On GNU/Linux and BSDs, there are some standard directories that contain extra
# libraries and header files. These directories (often /usr/local/lib and
# /usr/local/include) are for libraries that are installed manually
# (without a package manager). We'll use /usr/local/lib/raysan5 and /usr/local/include/raysan5
# for our -L and -I specification to simplify management of the raylib source package.
# Customize these locations if you like but don't forget to pass them to make
# for compilation and enable runtime linking with -rpath, LD_LIBRARY_PATH, or ldconfig.
# HINT: Add -L$(RAYLIB_INSTALL_PATH) -I$(RAYLIB_H_INSTALL_PATH) to your own makefiles.
# See below and ../examples/Makefile for more information.

# RAYLIB_INSTALL_PATH should be the desired full path to libraylib. No relative paths.
DESTDIR ?= $(VITASDK)/arm-vita-eabi
RAYLIB_INSTALL_PATH ?= $(DESTDIR)/lib
# RAYLIB_H_INSTALL_PATH locates the installed raylib header and associated source files.
RAYLIB_H_INSTALL_PATH ?= $(DESTDIR)/include

install :
	mkdir -p $(RAYLIB_INSTALL_PATH)
	mkdir -p $(RAYLIB_H_INSTALL_PATH)
	cp --update --verbose $(RAYLIB_RELEASE_PATH)/lib$(RAYLIB_LIB_NAME).a $(RAYLIB_INSTALL_PATH)/lib$(RAYLIB_LIB_NAME).a
	cp --update raylib.h $(RAYLIB_H_INSTALL_PATH)/raylib.h
	cp --update raymath.h $(RAYLIB_H_INSTALL_PATH)/raymath.h
	cp --update rlgl.h $(RAYLIB_H_INSTALL_PATH)/rlgl.h
	@echo "raylib development files installed/updated!"

# Remove raylib dev files installed on the system
# NOTE: see 'install' target.
uninstall :
	rm --force --interactive --verbose $(RAYLIB_H_INSTALL_PATH)/raylib.h
	rm --force --interactive --verbose $(RAYLIB_H_INSTALL_PATH)/raymath.h
	rm --force --interactive --verbose $(RAYLIB_H_INSTALL_PATH)/rlgl.h
	rm --force --interactive --verbose $(RAYLIB_H_INSTALL_PATH)/physac.h
	@echo "raylib development files removed!"

.PHONY: clean_shell_cmd clean_shell_sh

# Clean everything
clean:	clean_shell_$(PLATFORM_SHELL)
	@echo "removed all generated files!"

clean_shell_sh:
	rm -fv *.o $(RAYLIB_RELEASE_PATH)/lib$(RAYLIB_LIB_NAME).a $(RAYLIB_RELEASE_PATH)/lib$(RAYLIB_LIB_NAME).bc $(RAYLIB_RELEASE_PATH)/lib$(RAYLIB_LIB_NAME).so* raygui.c $(RAYLIB_RELEASE_PATH)/*-protocol.h $(RAYLIB_RELEASE_PATH)/*-protocol-code.h
ifeq ($(TARGET_PLATFORM),PLATFORM_ANDROID)
	rm -fv $(NATIVE_APP_GLUE)/android_native_app_glue.o
endif

# Set specific target variable
clean_shell_cmd: SHELL=cmd
clean_shell_cmd:
	del *.o /s
	cd $(RAYLIB_RELEASE_PATH) & \
	del lib$(RAYLIB_LIB_NAME).a /s & \
	del lib$(RAYLIB_LIB_NAME)dll.a /s & \
	del $(RAYLIB_LIB_NAME).dll /s & \
	del raygui.c /s & \
