aux_source_directory(. SRC_LIST)
file(GLOB HEADERS "*.h")

add_library(devcrypto ${SRC_LIST} ${HEADERS})
target_link_libraries(devcrypto PRIVATE Secp256k1 Cryptopp)
target_link_libraries(devcrypto PUBLIC devcore)
