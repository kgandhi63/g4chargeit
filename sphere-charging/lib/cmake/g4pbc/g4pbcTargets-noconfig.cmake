#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "g4pbc::g4pbc" for configuration ""
set_property(TARGET g4pbc::g4pbc APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(g4pbc::g4pbc PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libg4pbc.so.1.0.0"
  IMPORTED_SONAME_NOCONFIG "libg4pbc.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS g4pbc::g4pbc )
list(APPEND _IMPORT_CHECK_FILES_FOR_g4pbc::g4pbc "${_IMPORT_PREFIX}/lib/libg4pbc.so.1.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
