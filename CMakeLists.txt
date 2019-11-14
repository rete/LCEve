########################################################
# cmake file for building LCEve
# @author Remi Ete, Desy
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
########################################################

# project !
project(
  LCEve
  VERSION 0.0.0
  LANGUAGES CXX
)

include( GNUInstallDirs )

### DEPENDENCIES ############################################################
find_package( ILCUTIL COMPONENTS ILCSOFT_CMAKE_MODULES REQUIRED )
# load default settings from ILCSOFT_CMAKE_MODULES
# nasty feature of iLCUtil: switch this OFF
set( USE_CXX11 OFF )
include( ilcsoft_default_settings )
if( NOT CMAKE_CXX_STANDARD )
  # minimum standard required
  set( CMAKE_CXX_STANDARD 14 )
endif()

set( ROOT_COMPONENTS ROOTEve ROOTWebDisplay )
find_package( LCIO REQUIRED )
find_package( ROOT 6.19 COMPONENTS ${ROOT_COMPONENTS} REQUIRED ) # work with master as of today
FIND_PACKAGE( DD4hep COMPONENTS DDParsers REQUIRED )

set( PROJECT_INCLUDE_DIRS ${LCIO_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${DD4hep_INCLUDE_DIRS} )
set( PROJECT_LIBRARIES ${LCIO_LIBRARIES} ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES} ${DD4hep_LIBRARIES} ${DD4hep_COMPONENT_LIBRARIES} )
foreach( comp ${ROOT_COMPONENTS} )
  list( APPEND PROJECT_LIBRARIES ${ROOT_${comp}_LIBRARY} )
endforeach()

include_directories( SYSTEM ${PROJECT_INCLUDE_DIRS} )
link_libraries( ${PROJECT_LIBRARIES} )

list( APPEND REQUIRED_INCLUDES
  REveManager.hxx
)

foreach( include_dir ${ROOT_INCLUDE_DIRS} )
  foreach( include_file ${REQUIRED_INCLUDES} )
    string( REPLACE "." "_" include_file_var ${include_file} )
    find_file( file_found_${include_file_var} ${include_file} PATHS ${include_dir} PATH_SUFFIXES ROOT NO_DEFAULT_PATH )
    message( STATUS " => Checking for file ${include_file}: ${file_found_${include_file_var}}" )
    if( NOT file_found_${include_file_var} )
      message( FATAL_ERROR "Couldn't find ROOT header file: ${include_file}" )
    endif()
  endforeach()
endforeach()

include_directories( source/include )

#
# LCEve library compilation
aux_source_directory( source/src library_sources )
add_library( LCEve_lib SHARED ${library_sources} )
set_target_properties( LCEve_lib PROPERTIES OUTPUT_NAME LCEve )
install( TARGETS LCEve_lib LIBRARY )
set( LINKDEF_FILE source/include/LinkDef.h )
set( DICTIONARY_HEADERS
  LCEve/EventNavigator.h
  LCEve/EventDisplay.h
)
root_generate_dictionary(
  LCEveDict ${DICTIONARY_HEADERS}
  MODULE LCEve_lib
  LINKDEF ${LINKDEF_FILE}
)

# LCEventDisplay executable compilation
add_executable( LCEventDisplay_bin source/main/LCEventDisplay.cc )
target_link_libraries( LCEventDisplay_bin LCEve_lib )
set_target_properties( LCEventDisplay_bin PROPERTIES OUTPUT_NAME LCEventDisplay )
install( TARGETS LCEventDisplay_bin RUNTIME )

# LCGeomViewer executable compilation
add_executable( LCGeomViewer_bin source/main/LCGeomViewer.cc )
target_link_libraries( LCGeomViewer_bin LCEve_lib )
set_target_properties( LCGeomViewer_bin PROPERTIES OUTPUT_NAME LCGeomViewer )
install( TARGETS LCGeomViewer_bin RUNTIME )

# display some variables and write them to cache
display_std_variables()
