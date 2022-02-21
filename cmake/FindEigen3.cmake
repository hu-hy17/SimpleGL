FIND_PATH( EIGEN3_INCLUDE_DIRS Eigen/Geometry
    $ENV{EIGEN3_DIR}/include
    /usr/local/include/eigen3
    /usr/local/include
    /usr/local/X11R6/include
    /usr/X11R6/include
    /usr/X11/include
    /usr/include/X11
    /usr/include/eigen3/
    /usr/include
    /opt/X11/include
    /opt/include 
    ${CMAKE_SOURCE_DIR}/libraries/eigen3)

SET(EIGEN3_FOUND "NO")
IF(EIGEN3_INCLUDE_DIRS)
	message(STATUS "eigen3 path found: ${EIGEN3_INCLUDE_DIRS}")
    SET(EIGEN3_FOUND "YES")
ENDIF()
