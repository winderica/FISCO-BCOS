include(ExternalProject)

ExternalProject_Add(rescue
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ""
    INSTALL_COMMAND ""
    BUILD_COMMAND cargo build --release
    COMMAND cargo build --release
    BINARY_DIR "${CMAKE_SOURCE_DIR}/rust"
    LOG_BUILD 1
)

add_library(RESCUE STATIC IMPORTED)
set(RESCUE_LIBRARY ${CMAKE_SOURCE_DIR}/rust/target/release/${CMAKE_STATIC_LIBRARY_PREFIX}rescue${CMAKE_STATIC_LIBRARY_SUFFIX})
set_property(TARGET RESCUE PROPERTY IMPORTED_LOCATION ${RESCUE_LIBRARY})
