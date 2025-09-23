# Install script for directory: /storage/home/hcoda1/5/avira7/r-zjiang33-0/geant4_workdir/Grain-Charging-Simulations/g4pbc

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/storage/home/hcoda1/5/avira7/r-zjiang33-0/geant4_workdir/Grain-Charging-Simulations/g4pbc")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/storage/home/hcoda1/5/avira7/r-zjiang33-0/geant4_workdir/Grain-Charging-Simulations/g4pbc/include/G4LogicalVolumePeriodic.hh"
    "/storage/home/hcoda1/5/avira7/r-zjiang33-0/geant4_workdir/Grain-Charging-Simulations/g4pbc/include/G4ParticleChangeForPeriodic.hh"
    "/storage/home/hcoda1/5/avira7/r-zjiang33-0/geant4_workdir/Grain-Charging-Simulations/g4pbc/include/G4PeriodicBoundaryBuilder.hh"
    "/storage/home/hcoda1/5/avira7/r-zjiang33-0/geant4_workdir/Grain-Charging-Simulations/g4pbc/include/G4PeriodicBoundaryPhysics.hh"
    "/storage/home/hcoda1/5/avira7/r-zjiang33-0/geant4_workdir/Grain-Charging-Simulations/g4pbc/include/G4PeriodicBoundaryProcess.hh"
    "/storage/home/hcoda1/5/avira7/r-zjiang33-0/geant4_workdir/Grain-Charging-Simulations/build-g4pbc/g4pbc_export.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libg4pbc.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libg4pbc.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/storage/home/hcoda1/5/avira7/r-zjiang33-0/geant4_workdir/Grain-Charging-Simulations/build-g4pbc/libg4pbc.so.1.0.0"
    "/storage/home/hcoda1/5/avira7/r-zjiang33-0/geant4_workdir/Grain-Charging-Simulations/build-g4pbc/libg4pbc.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libg4pbc.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libg4pbc.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/storage/coda1/p-zjiang33/0/shared/geant4-v11.3.0-vis-1/geant4-v11.3.0-vis/lib64:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/storage/home/hcoda1/5/avira7/r-zjiang33-0/geant4_workdir/Grain-Charging-Simulations/build-g4pbc/libg4pbc.so")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/g4pbc/g4pbcTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/g4pbc/g4pbcTargets.cmake"
         "/storage/home/hcoda1/5/avira7/r-zjiang33-0/geant4_workdir/Grain-Charging-Simulations/build-g4pbc/CMakeFiles/Export/6c2d30adc8db34c8a3cb9c14e4915911/g4pbcTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/g4pbc/g4pbcTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/g4pbc/g4pbcTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/g4pbc" TYPE FILE FILES "/storage/home/hcoda1/5/avira7/r-zjiang33-0/geant4_workdir/Grain-Charging-Simulations/build-g4pbc/CMakeFiles/Export/6c2d30adc8db34c8a3cb9c14e4915911/g4pbcTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/g4pbc" TYPE FILE FILES "/storage/home/hcoda1/5/avira7/r-zjiang33-0/geant4_workdir/Grain-Charging-Simulations/build-g4pbc/CMakeFiles/Export/6c2d30adc8db34c8a3cb9c14e4915911/g4pbcTargets-noconfig.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/g4pbc" TYPE FILE FILES
    "/storage/home/hcoda1/5/avira7/r-zjiang33-0/geant4_workdir/Grain-Charging-Simulations/build-g4pbc/g4pbcConfig.cmake"
    "/storage/home/hcoda1/5/avira7/r-zjiang33-0/geant4_workdir/Grain-Charging-Simulations/build-g4pbc/g4pbcConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
  file(WRITE "/storage/home/hcoda1/5/avira7/r-zjiang33-0/geant4_workdir/Grain-Charging-Simulations/build-g4pbc/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
