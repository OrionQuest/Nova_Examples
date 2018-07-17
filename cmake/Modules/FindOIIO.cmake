###########################################################################
# OpenImageIO
include(FindPackageHandleStandardArgs)

# If 'OPENIMAGEHOME' not set, use the env variable of that name if available
if (NOT OPENIMAGEIOHOME)
    if (NOT $ENV{OPENIMAGEIOHOME} STREQUAL "")
        set (OPENIMAGEIOHOME $ENV{OPENIMAGEIOHOME})
    endif ()
endif ()


if (NOT OpenImageIO_FIND_QUIETLY)
    MESSAGE ( STATUS "OPENIMAGEIOHOME = ${OPENIMAGEIOHOME}" )
endif ()

find_library ( OPENIMAGEIO_LIBRARY
               NAMES OpenImageIO
               HINTS ${OPENIMAGEIOHOME}
               PATH_SUFFIXES lib64 lib
               PATHS "${OPENIMAGEIOHOME}/lib" )
find_path ( OPENIMAGEIO_INCLUDE_DIR
            NAMES OpenImageIO/imageio.h
            HINTS ${OPENIMAGEIOHOME}
            PATH_SUFFIXES include )
find_program ( OPENIMAGEIO_BIN
               NAMES oiiotool
               HINTS ${OPENIMAGEIOHOME}
               PATH_SUFFIXES bin )
if (NOT OpenImageIO_FIND_QUIETLY)
    MESSAGE ( STATUS "OpenImageIO includes = ${OPENIMAGEIO_INCLUDE_DIR}" )
    MESSAGE ( STATUS "OpenImageIO library = ${OPENIMAGEIO_LIBRARY}" )
    MESSAGE ( STATUS "OpenImageIO bin = ${OPENIMAGEIO_BIN}" )
endif ()

#IF ( OPENIMAGEIO_INCLUDE_DIR AND OPENIMAGEIO_LIBRARY )
#    SET ( OPENIMAGEIO_FOUND TRUE )
#ELSE ()
#    SET ( OPENIMAGEIO_FOUND FALSE )
#ENDIF ()

# Handle REQUIRD argument, define *_FOUND variable
find_package_handle_standard_args(OPENIMAGEIO DEFAULT_MSG OPENIMAGEIO_LIBRARY OPENIMAGEIO_INCLUDE_DIR)

mark_as_advanced(OPENIMAGEIO_LIBRARY)
mark_as_advanced(OPENIMAGEIO_INCLUDE_DIR)
mark_as_advanced(OPENIMAGEIO_BIN)

# end OpenImageIO setup
###########################################################################
