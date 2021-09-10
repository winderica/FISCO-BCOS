
file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

add_library(storage ${SRC_LIST} ${HEADERS})

target_link_libraries(storage PRIVATE blockverifier ethcore)
target_link_libraries(storage PUBLIC devcore TBB JsonCpp Boost::serialization Boost::thread RocksDB zdb ${CMAKE_DL_LIBS})
target_compile_options(storage PRIVATE -Wno-error)
