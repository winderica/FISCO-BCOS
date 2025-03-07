#------------------------------------------------------------------------------
# This file is part of FISCO-BCOS.
#
# FISCO-BCOS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# FISCO-BCOS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with FISCO-BCOS.  If not, see <http://www.gnu.org/licenses/>
#
# (c) 2016-2018 fisco-dev contributors.
#------------------------------------------------------------------------------
include(ExternalProject)
include(GNUInstallDirs)

include(ProcessorCount)
ProcessorCount(CORES)
if(CORES EQUAL 0)
  set(CORES 1)
elseif(CORES GREATER 2)
  set(CORES 2)
endif()

set(BOOST_CXXFLAGS "cxxflags=${MARCH_TYPE}")

if (APPLE)
    set(SED_CMMAND sed -i .bkp)
    set(BOOST_BOOTSTRAP_COMMAND ./bootstrap.sh COMMAND ${SED_CMMAND} "s/-fcoalesce-templates//g" ${CMAKE_SOURCE_DIR}/deps/src/boost/tools/build/src/tools/darwin.jam)
else()
    set(SED_CMMAND sed -i)
    set(BOOST_BOOTSTRAP_COMMAND ./bootstrap.sh)
endif()

set(BOOST_INSTALL_COMMAND ./b2 install --prefix=${CMAKE_SOURCE_DIR}/deps)
set(BOOST_BUILD_TOOL ./b2)
set(BOOST_LIBRARY_SUFFIX .a)

set(BOOST_LIB_PREFIX ${CMAKE_SOURCE_DIR}/deps/src/boost/stage/lib/libboost_)
set(BOOST_BUILD_FILES ${BOOST_LIB_PREFIX}chrono.a ${BOOST_LIB_PREFIX}date_time.a
        ${BOOST_LIB_PREFIX}random.a ${BOOST_LIB_PREFIX}regex.a
        ${BOOST_LIB_PREFIX}filesystem.a ${BOOST_LIB_PREFIX}system.a
        ${BOOST_LIB_PREFIX}unit_test_framework.a ${BOOST_LIB_PREFIX}log.a
        ${BOOST_LIB_PREFIX}thread.a ${BOOST_LIB_PREFIX}program_options.a
        ${BOOST_LIB_PREFIX}serialization.a)

ExternalProject_Add(boost
    PREFIX ${CMAKE_SOURCE_DIR}/deps
    DOWNLOAD_NO_PROGRESS 1
    URL https://nchc.dl.sourceforge.net/project/boost/boost/1.76.0/boost_1_76_0.tar.bz2
        https://osp-1257653870.cos.ap-guangzhou.myqcloud.com/FISCO-BCOS/FISCO-BCOS/deps/boost_1_76_0.tar.bz2
        https://raw.githubusercontent.com/FISCO-BCOS/LargeFiles/master/libs/boost_1_76_0.tar.bz2
    URL_HASH SHA256=f0397ba6e982c4450f27bf32a2a83292aba035b827a5623a14636ea583318c41
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ${BOOST_BOOTSTRAP_COMMAND}
    BUILD_COMMAND ${BOOST_BUILD_TOOL} stage
        ${BOOST_CXXFLAGS}
        threading=multi
        link=static
        variant=release
        address-model=64
        --with-chrono
        --with-date_time
        --with-filesystem
        --with-system
        --with-random
        --with-regex
        --with-test
        --with-thread
        --with-serialization
        --with-program_options
        --with-log
        --with-iostreams
        -s NO_BZIP2=1 -s NO_LZMA=1 -s NO_ZSTD=1
        -j${CORES}
    LOG_CONFIGURE 1
    LOG_BUILD 1
    LOG_INSTALL 1
    INSTALL_COMMAND ""
    # INSTALL_COMMAND ${BOOST_INSTALL_COMMAND}
    BUILD_BYPRODUCTS ${BOOST_BUILD_FILES}
)

ExternalProject_Get_Property(boost SOURCE_DIR)
set(BOOST_INCLUDE_DIR ${SOURCE_DIR})
set(BOOST_LIB_DIR ${SOURCE_DIR}/stage/lib)

add_library(Boost::system STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::system PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_system${BOOST_LIBRARY_SUFFIX})
set_property(TARGET Boost::system PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
add_dependencies(Boost::system boost)

add_library(Boost::chrono STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::chrono PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_chrono${BOOST_LIBRARY_SUFFIX})
add_dependencies(Boost::chrono boost)

add_library(Boost::date_time STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::date_time PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_date_time${BOOST_LIBRARY_SUFFIX})
set_property(TARGET Boost::date_time PROPERTY INTERFACE_LINK_LIBRARIES Boost::system)
add_dependencies(Boost::date_time boost)

add_library(Boost::regex STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::regex PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_regex${BOOST_LIBRARY_SUFFIX})
set_property(TARGET Boost::regex PROPERTY INTERFACE_LINK_LIBRARIES Boost::system)
add_dependencies(Boost::regex boost)

add_library(Boost::filesystem STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::filesystem PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_filesystem${BOOST_LIBRARY_SUFFIX})
set_property(TARGET Boost::filesystem PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(TARGET Boost::filesystem PROPERTY INTERFACE_LINK_LIBRARIES Boost::system)
add_dependencies(Boost::filesystem boost)

add_library(Boost::random STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::random PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_random${BOOST_LIBRARY_SUFFIX})
add_dependencies(Boost::random boost)

add_library(Boost::unit_test_framework STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::unit_test_framework PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_unit_test_framework${BOOST_LIBRARY_SUFFIX})
set_property(TARGET Boost::unit_test_framework PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
add_dependencies(Boost::unit_test_framework boost)

add_library(Boost::thread STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::thread PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_thread${BOOST_LIBRARY_SUFFIX})
set_property(TARGET Boost::thread PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(TARGET Boost::thread PROPERTY INTERFACE_LINK_LIBRARIES Boost::chrono Boost::date_time Boost::regex)
add_dependencies(Boost::thread boost)

add_library(Boost::program_options STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::program_options PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_program_options${BOOST_LIBRARY_SUFFIX})
set_property(TARGET Boost::program_options PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
add_dependencies(Boost::program_options boost)

add_library(Boost::log STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::log PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_log${BOOST_LIBRARY_SUFFIX})
set_property(TARGET Boost::log PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(TARGET Boost::log PROPERTY INTERFACE_LINK_LIBRARIES Boost::filesystem Boost::thread)
add_dependencies(Boost::log boost)

add_library(Boost::serialization STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::serialization PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_serialization${BOOST_LIBRARY_SUFFIX})
set_property(TARGET Boost::serialization PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
add_dependencies(Boost::serialization boost)

# for boost compress/base64encode
add_library(Boost::iostreams STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::iostreams PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_iostreams${BOOST_LIBRARY_SUFFIX})
set_property(TARGET Boost::iostreams PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
add_dependencies(Boost::iostreams boost)
unset(SOURCE_DIR)
