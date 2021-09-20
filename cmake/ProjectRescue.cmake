include(ExternalProject)

ExternalProject_Add(rescue
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ""
    INSTALL_COMMAND ""
    BUILD_COMMAND ${CMAKE_COMMAND} -E env CARGO_TARGET_DIR=${CMAKE_SOURCE_DIR}/deps/rescue cargo build --release
    COMMAND ${CMAKE_COMMAND} -E env CARGO_TARGET_DIR=${CMAKE_SOURCE_DIR}/deps/rescue cargo build --release
    BINARY_DIR "${CMAKE_SOURCE_DIR}/rust"
    BUILD_ALWAYS TRUE
    LOG_BUILD 1
)

add_library(RESCUE STATIC IMPORTED)
add_dependencies(RESCUE rescue)
set(RESCUE_LIBRARY ${CMAKE_SOURCE_DIR}/deps/rescue/release/${CMAKE_STATIC_LIBRARY_PREFIX}rescue${CMAKE_STATIC_LIBRARY_SUFFIX})
set_property(TARGET RESCUE PROPERTY IMPORTED_LOCATION ${RESCUE_LIBRARY})
