file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

add_library(devcore ${SRC_LIST} ${HEADERS})

target_include_directories(devcore SYSTEM BEFORE PUBLIC ${BOOST_INCLUDE_DIR})
# set_source_files_properties(${SRC_LIST} ${HEADERS} PROPERTIES COMPILE_FLAGS -Wno-unused-function)
target_compile_options(devcore PRIVATE -Wno-error -Wno-unused-variable)

target_link_libraries(devcore PUBLIC LevelDB Boost::log Boost::filesystem Snappy TBB)
add_dependencies(devcore BuildInfo.h LevelDB)

# get_property(dirs TARGET devcore PROPERTY INCLUDE_DIRECTORIES)
# foreach(dir ${dirs})
#   message(STATUS "dir='${dir}'")
# endforeach()
