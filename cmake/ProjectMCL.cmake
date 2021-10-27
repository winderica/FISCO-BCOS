include(ExternalProject)

find_library(GMP_LIBRARIES gmp)
find_path(GMP_INCLUDE_DIR gmp.h)
add_library(Gmp UNKNOWN IMPORTED)
set_property(TARGET Gmp PROPERTY IMPORTED_LOCATION ${GMP_LIBRARIES})
set_property(TARGET Gmp PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${GMP_INCLUDE_DIR})

ExternalProject_Add(bls
    PREFIX ${CMAKE_SOURCE_DIR}/deps
    GIT_REPOSITORY https://github.com/winderica/bls
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
               # Build static lib but suitable to be included in a shared lib.
               -DCMAKE_POSITION_INDEPENDENT_CODE=${BUILD_SHARED_LIBS}
               ${_only_release_configuration}
               	-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
               	-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                -DBLS_ETH=ON
    LOG_CONFIGURE 1
    LOG_BUILD 1
    ${_overwrite_install_command}
    LOG_INSTALL 1
)

ExternalProject_Get_Property(bls INSTALL_DIR)
add_library(BLS STATIC IMPORTED)
set_property(TARGET BLS PROPERTY IMPORTED_LOCATION ${INSTALL_DIR}/lib/libbls384_256.a)
set_property(TARGET BLS PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${INSTALL_DIR}/include)
add_dependencies(BLS bls)

add_library(MCL STATIC IMPORTED)
set_property(TARGET MCL PROPERTY IMPORTED_LOCATION ${INSTALL_DIR}/lib/libmcl.a)
set_property(TARGET MCL PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${INSTALL_DIR}/include)
add_dependencies(MCL bls)

unset(INSTALL_DIR)

