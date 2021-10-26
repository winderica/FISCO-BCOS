include(ExternalProject)

find_library(GMP_LIBRARIES gmp)
find_path(GMP_INCLUDE_DIR gmp.h)
add_library(Gmp UNKNOWN IMPORTED)
set_property(TARGET Gmp PROPERTY IMPORTED_LOCATION ${GMP_LIBRARIES})
set_property(TARGET Gmp PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${GMP_INCLUDE_DIR})

ExternalProject_Add(mcl
    PREFIX ${CMAKE_SOURCE_DIR}/deps
    GIT_REPOSITORY https://github.com/herumi/bls
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
               # Build static lib but suitable to be included in a shared lib.
               -DCMAKE_POSITION_INDEPENDENT_CODE=${BUILD_SHARED_LIBS}
               ${_only_release_configuration}
               	-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
               	-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                -DBLS_ETH=ON
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

