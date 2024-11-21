# Install script for directory: /storage/coda1/p-zjiang33/0/shared/Grain-Charging-Simulations/g4pbc

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/storage/coda1/p-zjiang33/0/shared/Grain-Charging-Simulations/build-g4pbc")
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

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/storage/coda1/p-zjiang33/0/shared/Grain-Charging-Simulations/g4pbc/include/G4LogicalVolumePeriodic.hh"
    "/storage/coda1/p-zjiang33/0/shared/Grain-Charging-Simulations/g4pbc/include/G4ParticleChangeForPeriodic.hh"
    "/storage/coda1/p-zjiang33/0/shared/Grain-Charging-Simulations/g4pbc/include/G4PeriodicBoundaryBuilder.hh"
    "/storage/coda1/p-zjiang33/0/shared/Grain-Charging-Simulations/g4pbc/include/G4PeriodicBoundaryPhysics.hh"
    "/storage/coda1/p-zjiang33/0/shared/Grain-Charging-Simulations/g4pbc/include/G4PeriodicBoundaryProcess.hh"
    "/storage/coda1/p-zjiang33/0/shared/Grain-Charging-Simulations/build-g4pbc/g4pbc_export.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
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
    "/storage/coda1/p-zjiang33/0/shared/Grain-Charging-Simulations/build-g4pbc/libg4pbc.so.1.0.0"
    "/storage/coda1/p-zjiang33/0/shared/Grain-Charging-Simulations/build-g4pbc/libg4pbc.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libg4pbc.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libg4pbc.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/storage/coda1/p-zjiang33/0/shared/geant4-v11.3.0-vis/geant4-v11/3/0-vis/lib64:/usr/local/pace-apps/spack/packages/linux-rhel7-x86_64/gcc-10.3.0/xerces-c-3.2.3-4m2fkwaqkvkq7r2ldj4gtocitgybtpiy/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libg4pbc.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libg4pbc.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libg4pbc.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/storage/coda1/p-zjiang33/0/shared/Grain-Charging-Simulations/build-g4pbc/libg4pbc.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libg4pbc.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libg4pbc.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libg4pbc.so"
         OLD_RPATH "/storage/coda1/p-zjiang33/0/shared/geant4-v11.3.0-vis/geant4-v11/3/0-vis/lib64:/usr/local/pace-apps/spack/packages/linux-rhel7-x86_64/gcc-10.3.0/xerces-c-3.2.3-4m2fkwaqkvkq7r2ldj4gtocitgybtpiy/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libg4pbc.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/g4pbc/g4pbcTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/g4pbc/g4pbcTargets.cmake"
         "/storage/coda1/p-zjiang33/0/shared/Grain-Charging-Simulations/build-g4pbc/CMakeFiles/Export/lib/cmake/g4pbc/g4pbcTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/g4pbc/g4pbcTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/g4pbc/g4pbcTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/g4pbc" TYPE FILE FILES "/storage/coda1/p-zjiang33/0/shared/Grain-Charging-Simulations/build-g4pbc/CMakeFiles/Export/lib/cmake/g4pbc/g4pbcTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/g4pbc" TYPE FILE FILES "/storage/coda1/p-zjiang33/0/shared/Grain-Charging-Simulations/build-g4pbc/CMakeFiles/Export/lib/cmake/g4pbc/g4pbcTargets-noconfig.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/g4pbc" TYPE FILE FILES
    "/storage/coda1/p-zjiang33/0/shared/Grain-Charging-Simulations/build-g4pbc/g4pbcConfig.cmake"
    "/storage/coda1/p-zjiang33/0/shared/Grain-Charging-Simulations/build-g4pbc/g4pbcConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/storage/coda1/p-zjiang33/0/shared/Grain-Charging-Simulations/build-g4pbc/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
