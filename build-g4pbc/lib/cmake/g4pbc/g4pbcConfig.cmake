
# replaced with code which makes the file relocatable

####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was g4pbcConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

# ${PACKAGE_PREFIX_DIR}/include/ defined for each PATH_VARS variable
# relocatable version of the input path variable
# Input can be relative or absolute path
set_and_check(g4cbp_INCLUDE_DIR ${PACKAGE_PREFIX_DIR}/include/)

#only do this once for the project
if(NOT TARGET g4pbc)
  include(${CMAKE_CURRENT_LIST_DIR}/g4pbcTargets.cmake)
endif()

set(g4cbp_INCLUDE_DIRS ${g4pbc_INCLUDE_DIR})
set(g4cbp_LIBRARIES g4pbc)
set(g4cbp_LIBRARY g4pbc::g4pbc)
