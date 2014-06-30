# valid components are: main dx bristol

# find the Bristol include directory
find_path(BRISTOL_INCLUDE_DIR bristol/bristol.hpp
          PATHS
          /Users/dooz/projects/bristol/
          /projects/bristol/)

set(BRISTOL_FOUND TRUE) # will be set to false if one of the required modules is not found
          
# set library path
# todo: use the same output directory for both debug/release and use suffixes
set(FIND_BRISTOL_LIB_PATHS
  /Users/dooz/projects/bristol/
  /projects/bristol/
)

# search for the libraries for the required components
foreach(FIND_BRISTOL_COMPONENT ${BRISTOL_FIND_COMPONENTS})

    string(TOLOWER ${FIND_BRISTOL_COMPONENT} FIND_BRISTOL_COMPONENT_LOWER)
    string(TOUPPER ${FIND_BRISTOL_COMPONENT} FIND_BRISTOL_COMPONENT_UPPER)
    set(FIND_BRISTOL_COMPONENT_NAME bristol_${FIND_BRISTOL_COMPONENT_LOWER})

    # debug library
    find_library(BRISTOL_${FIND_BRISTOL_COMPONENT_UPPER}_LIBRARY_DEBUG
                 NAMES ${FIND_BRISTOL_COMPONENT_NAME}
                 PATH_SUFFIXES Debug
                 PATHS ${FIND_BRISTOL_LIB_PATHS})

    # release library
    find_library(BRISTOL_${FIND_BRISTOL_COMPONENT_UPPER}_LIBRARY_RELEASE
                 NAMES ${FIND_BRISTOL_COMPONENT_NAME}
                 PATH_SUFFIXES Release
                 PATHS ${FIND_BRISTOL_LIB_PATHS})

                
    # add to the global list of libraries
    set(BRISTOL_LIBRARIES ${BRISTOL_LIBRARIES} "${BRISTOL_${FIND_BRISTOL_COMPONENT_UPPER}_LIBRARY}")
endforeach()
