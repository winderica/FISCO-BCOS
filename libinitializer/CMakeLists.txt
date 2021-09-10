file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

find_package(OpenSSL REQUIRED)
add_library(initializer ${SRC_LIST} ${HEADERS})
target_include_directories(initializer SYSTEM BEFORE PUBLIC ${BOOST_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIRS})
target_link_libraries(initializer PUBLIC channelserver rpc ledger TBB devcore eventfilter ${OPENSSL_LIBRARIES})
# install(TARGETS initializer RUNTIME DESTINATION bin ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
