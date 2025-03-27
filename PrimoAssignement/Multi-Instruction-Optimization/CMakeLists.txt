# anche qui, il CMAKEFILE vale per questa directory legata al passo

# alla fine di tutto però avremo una sola directory, con un solo CMAKEFILE, dove però dovremmo fare riferimento alle diverse classi presenti
# nel pass.cpp che includerà tutti e 3 i passi, e quindi tutte e 3 le classi.

cmake_minimum_required(VERSION 3.20)
project(MultiInstructioinOptimization)

#===============================================================================
# 1. LOAD LLVM CONFIGURATION
#===============================================================================
# Set this to a valid LLVM installation dir
set(LT_LLVM_INSTALL_DIR "" CACHE PATH "LLVM installation directory")

# Add the location of LLVMConfig.cmake to CMake search paths (so that
# find_package can locate it)
list(APPEND CMAKE_PREFIX_PATH "${LT_LLVM_INSTALL_DIR}/lib/cmake/llvm/")

find_package(LLVM CONFIG)
if("${LLVM_VERSION_MAJOR}" VERSION_LESS 19)
  message(FATAL_ERROR "Found LLVM ${LLVM_VERSION_MAJOR}, but need LLVM 19 or above")
endif()

# HelloWorld includes headers from LLVM - update the include paths accordingly
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})

#===============================================================================
# 2. BUILD CONFIGURATION
#===============================================================================
# Use the same C++ standard as LLVM does
set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

# LLVM is normally built without RTTI. Be consistent with that.
if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

#===============================================================================
# 3. ADD THE TARGET
#===============================================================================
add_library(MultInstrOpt SHARED Pass3.cpp)
# qui nel passo finale, che avrà tutte le ottimizazioni rick dice di mettere un nome a cazzo, tipo Assignment1 Shared nomePassofinale.cpp

# Allow undefined symbols in shared objects on Darwin (this is the default
# behaviour on Linux)
target_link_libraries(MultInstrOpt
  "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>")
