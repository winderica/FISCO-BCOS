include(ExternalProject)

ExternalProject_Add(mcl
    PREFIX ${CMAKE_SOURCE_DIR}/deps
    DOWNLOAD_NAME mcl-1.52.tar.gz
    DOWNLOAD_NO_PROGRESS 1
    URL https://github.com/herumi/mcl/archive/refs/tags/v1.52.tar.gz
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
               # Build static lib but suitable to be included in a shared lib.
               -DCMAKE_POSITION_INDEPENDENT_CODE=${BUILD_SHARED_LIBS}
               ${_only_release_configuration}
               	-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        		-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    LOG_CONFIGURE 1
    LOG_BUILD 1
    BUILD_COMMAND ""
    ${_overwrite_install_command}
    LOG_INSTALL 1
    BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libmcl.a
)

ExternalProject_Get_Property(mcl INSTALL_DIR)
add_library(MCL STATIC IMPORTED)
set(MCL_LIBRARY ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}mcl${CMAKE_STATIC_LIBRARY_SUFFIX})
set(MCL_INCLUDE_DIR ${INSTALL_DIR}/include)
file(MAKE_DIRECTORY ${MCL_INCLUDE_DIR})  # Must exist.
set_property(TARGET MCL PROPERTY IMPORTED_LOCATION ${MCL_LIBRARY})
set_property(TARGET MCL PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${MCL_INCLUDE_DIR})
add_dependencies(MCL mcl)
unset(INSTALL_DIR)

