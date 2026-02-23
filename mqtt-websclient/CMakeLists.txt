cmake_minimum_required(VERSION 3.24)

project(mqtt-websclient-app)

add_subdirectory(mqttwebsclient)

set(SRC_FILE main.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILE})

target_link_libraries(${PROJECT_NAME} PRIVATE MQTTWebSClient)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)