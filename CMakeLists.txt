cmake_minimum_required(VERSION 3.1)

foreach(policy
    CMP0074 # CMake 3.12
    )
  if(POLICY ${policy})
    cmake_policy(SET ${policy} NEW)
  endif()
endforeach()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

set(LIBRARY_NAME pydbc)

project(${LIBRARY_NAME})

# build in Release-mode by default if not explicitly set
if( NOT CMAKE_BUILD_TYPE )
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()


find_package(Python COMPONENTS Interpreter Development)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# Boost.Python must have static linking enabled explicitly per https://cmake.org/cmake/help/latest/module/FindBoost.html
add_definitions(-DBOOST_PYTHON_STATIC_LIB)

set(PYTHON_COMPONENT "python${Python_VERSION_MAJOR}${Python_VERSION_MINOR}")

find_package(Boost REQUIRED COMPONENTS "${PYTHON_COMPONENT}")

message(STATUS "lib dirs: ${Boost_LIBRARY_DIRS} py: ${Python_LIBRARY_DIRS} libs: ${Boost_LIBRARIES}")

set(SRC
  src/pydbc.cpp
  src/BinaryStream.cpp
)

# There is currently a bug in the version of cmake included with Visual Studio (3.12)
# which prevents us from using target_link_libraries() to add Boost::pythonXY to the
# library.  Until this is fixed, we will have to continue to use the deprecated package
# PythonLibs.  Once this is fixed, the two lines below can be replaced with something
# like:
# find_package(Python)
# Python_Add_library(${LIBRARY_NAME} MODULE ${SRC} src/pydbc.cpp)

find_package(PythonLibs)
PYTHON_ADD_MODULE(${LIBRARY_NAME} ${SRC})

target_compile_definitions(${LIBRARY_NAME} PUBLIC PYTHON)
include_directories(${Boost_INCLUDE_DIRS} ${Python_INCLUDE_DIRS} src)
target_link_libraries(${LIBRARY_NAME} ${Python_LIBRARIES} "Boost::${PYTHON_COMPONENT}")

#add_subdirectory(test)