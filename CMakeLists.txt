cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)
set(proj_name beast-http)
project(${proj_name} CXX)

include(ExternalProject)

find_package(Boost 1.67 COMPONENTS system thread regex)
find_package(OpenSSL COMPONENTS SSL Crypto)

set(exec_name ${proj_name})
add_executable(${exec_name} src/main.cpp src/response.hpp src/server.hpp)

include(ExternalProject)
ExternalProject_Add(
        BeastHttp
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/BeastHttp
        INSTALL_COMMAND cmake -E echo "SKIP INSTALL"
        )
ExternalProject_Get_Property(BeastHttp SOURCE_DIR)
set(BEASTHTTP_INCLUDE_PATH ${SOURCE_DIR}/include)
message("BEASTHTTP_INCLUDE_PATH: ${BEASTHTTP_INCLUDE_PATH}")
#add_subdirectory(vendor/BeastHttp)
#set(BeastHttp_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vendor/BeastHttp/include)
include_directories(${BEASTHTTP_INCLUDE_PATH}) # for clion

target_include_directories(${exec_name} PRIVATE
        ${Boost_INCLUDE_DIR} ${BEASTHTTP_INCLUDE_PATH})
target_link_libraries(${exec_name} PUBLIC ${Boost_LIBRARIES})
