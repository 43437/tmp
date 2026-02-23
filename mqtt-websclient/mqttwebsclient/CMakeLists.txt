cmake_minimum_required(VERSION 3.24)

project(MQTTWebSClient)

set(SRC_FILE
    ${CMAKE_CURRENT_LIST_DIR}/../../mqtt/mongoose.c
    ${CMAKE_CURRENT_LIST_DIR}/MQTTWebSClient.cpp)

add_library(${PROJECT_NAME} ${SRC_FILE})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../mqtt
    ${CMAKE_CURRENT_LIST_DIR})

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES MQTTWebSClient.hpp DESTINATION include)