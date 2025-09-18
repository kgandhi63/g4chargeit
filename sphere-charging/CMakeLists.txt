#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
project(charging_sphere)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

find_package(g4pbc REQUIRED NO_MODULE)

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})
include_directories(${g4pbc_INCLUDE_DIRS})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include 
                    ${Geant4_INCLUDE_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/outputlogs)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/geometry)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/root)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(charging_sphere charging_sphere.cc ${sources} ${headers})
target_link_libraries(charging_sphere ${Geant4_LIBRARIES} )
target_link_libraries(charging_sphere g4pbc::g4pbc)

find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIRS})
target_link_libraries(charging_sphere ${ROOT_LIBRARIES})


#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build Hadr04. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#

set(charging_scripts
    macro.mac
    vis.mac
    electron_distribution.txt
    photon_distribution.txt
  )


foreach(_script ${charging_scripts})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

# Copy all contents of pacescripts/ into the build directory (not the folder itself)
file(GLOB PACE_SCRIPTS ${CMAKE_SOURCE_DIR}/pacescripts/*)
file(COPY ${PACE_SCRIPTS} DESTINATION ${CMAKE_BINARY_DIR})

# Copy all contents of pacescripts/ into the build directory (not the folder itself)
file(GLOB GEOMETRY ${CMAKE_SOURCE_DIR}/geometry/*)
file(COPY ${GEOMETRY} DESTINATION ${CMAKE_BINARY_DIR}/geometry/)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS charging_sphere DESTINATION bin)

