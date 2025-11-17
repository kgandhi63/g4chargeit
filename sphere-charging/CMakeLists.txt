#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.12)
project(charging_sphere)

#----------------------------------------------------------------------------
# Find Required Packages
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

find_package(g4pbc REQUIRED NO_MODULE)
find_package(ROOT REQUIRED)
find_package(OpenMP REQUIRED)

# --- MODIFIED: Manually find FFTW3 to work around broken system configs ---
find_path(FFTW3_INCLUDE_DIR fftw3.h
  HINTS ENV FFTW_DIR
  PATH_SUFFIXES include)
find_library(FFTW3_LIBRARY_CORE NAMES fftw3
  HINTS ENV FFTW_DIR
  PATH_SUFFIXES lib lib64)
find_library(FFTW3_LIBRARY_OMP NAMES fftw3_omp
  HINTS ENV FFTW_DIR
  PATH_SUFFIXES lib lib64)

# Group the libraries together
set(FFTW3_LIBRARIES ${FFTW3_LIBRARY_CORE} ${FFTW3_LIBRARY_OMP})

if(NOT FFTW3_INCLUDE_DIR OR NOT FFTW3_LIBRARIES)
  message(FATAL_ERROR "Could not find FFTW3. Please load the fftw module and set FFTW_DIR if necessary.")
endif()


#----------------------------------------------------------------------------
# Setup include directories and compile definitions
#
include(${Geant4_USE_FILE})
include_directories(${g4pbc_INCLUDE_DIRS})
include_directories(${ROOT_INCLUDE_DIRS})
include_directories(${FFTW3_INCLUDE_DIR}) # Add FFTW headers from manual find
include_directories(${PROJECT_SOURCE_DIR}/include)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/outputlogs)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/geometry)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/root)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/fieldmaps)

#----------------------------------------------------------------------------
# Add the executable
#
add_executable(charging_sphere charging_sphere.cc ${sources} ${headers})

# --- Add compile option for AVX/native instructions ---
target_compile_options(charging_sphere PRIVATE -march=native)


# --- MODIFIED: Consolidate all linking into a single, modern command ---
target_link_libraries(charging_sphere
  PRIVATE
    ${Geant4_LIBRARIES}
    g4pbc::g4pbc
    ${ROOT_LIBRARIES}
    ${FFTW3_LIBRARIES}   # Use variable from manual find
    OpenMP::OpenMP_CXX   # Link OpenMP for multithreading
  )

#----------------------------------------------------------------------------
# Copy scripts and data to the build directory
#
set(charging_scripts
    macro.mac
    testsolarwind.mac
    testphotons.mac
    testwangphotons.mac
    vis.mac
  )

foreach(_script ${charging_scripts})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

file(GLOB PACE_SCRIPTS ${CMAKE_SOURCE_DIR}/pacescripts/*)
file(GLOB DISTRIBUTIONS ${CMAKE_SOURCE_DIR}/distributions/*.txt)
file(COPY ${PACE_SCRIPTS} DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${DISTRIBUTIONS} DESTINATION ${CMAKE_BINARY_DIR})

file(GLOB GEOMETRY ${CMAKE_SOURCE_DIR}/geometry/*)
file(COPY ${GEOMETRY} DESTINATION ${CMAKE_BINARY_DIR}/geometry/)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS charging_sphere DESTINATION bin)

