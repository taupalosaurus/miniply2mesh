FIND_PATH(LIBMESHB_INCLUDE_DIR libmeshb7.h
  $ENV{LIBMESHB_DIR}/include
  /usr/local/include
  ~/build/libMeshb/sources
  )

FIND_LIBRARY(LIBMESHB_LIBRARY libmeshb.a
  $ENV{LIBMESHB_DIR}/lib
  /usr/local/lib
  ~/build/libMeshb/build/sources
  )

IF(LIBMESHB_INCLUDE_DIR)
  IF(LIBMESHB_LIBRARY)
    SET( LIBMESHB_LIBRARIES ${LIBMESHB_LIBRARY})
    SET( LIBMESHB_FOUND "YES" )
    message(STATUS ${LIBMESHB_LIBRARY})
  ENDIF(LIBMESHB_LIBRARY)
ENDIF(LIBMESHB_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBMESHB DEFAULT_MSG LIBMESHB_INCLUDE_DIR )

MARK_AS_ADVANCED( LIBMESHB_INCLUDE_DIR LIBMESHB_LIBRARY)