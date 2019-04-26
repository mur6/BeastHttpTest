cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)
set(proj_name beast-http)
project(${proj_name} CXX)

include(ExternalProject)

find_package(Boost 1.67 COMPONENTS system thread regex)
find_package(OpenSSL COMPONENTS SSL Crypto)

set(exec_name ${proj_name})
add_executable(${exec_name} main.cpp src/response.cpp)
set(BeastHttp_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vendor/BeastHttp/include)
# include_directories(${BeastHttp_INCLUDE_DIR}) # for clion
message("BeastHttp_INCLUDE_DIR: ${BeastHttp_INCLUDE_DIR}")
target_include_directories(${exec_name} PRIVATE
        ${Boost_INCLUDE_DIR} ${BeastHttp_INCLUDE_DIR})
target_link_libraries(${exec_name} PUBLIC ${Boost_LIBRARIES})
