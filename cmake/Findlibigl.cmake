FIND_PATH( LIBIGL_INCLUDE_DIRS igl/AABB.h
    $ENV{LIBIGL_DIR}/include
    /usr/local/include/libigl
    /usr/local/include
    /usr/local/X11R6/include
    /usr/X11R6/include
    /usr/X11/include
    /usr/include/X11
    /usr/include/libigl/
    /usr/include
    /opt/X11/include
    /opt/include 
    ${CMAKE_SOURCE_DIR}/libraries/libigl/include)

SET(LIBIGL_FOUND "NO")
IF(LIBIGL_INCLUDE_DIRS)
	message(STATUS "libigl path found: ${LIBIGL_INCLUDE_DIRS}")
    SET(LIBIGL_FOUND "YES")
ENDIF()
