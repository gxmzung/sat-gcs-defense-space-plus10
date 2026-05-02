cmake_minimum_required(VERSION 3.20)
project(satgcs_cpp LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(satgcs_protocol
  protocol/Crc32.cpp
  protocol/TelemetryPacket.cpp
  math/AttitudeMath.cpp
  util/Logger.cpp
  util/HttpClient.cpp
)
target_include_directories(satgcs_protocol PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(satellite_device_sim apps/satellite_device_sim.cpp)
target_link_libraries(satellite_device_sim PRIVATE satgcs_protocol)
add_executable(telemetry_gateway apps/telemetry_gateway.cpp)
target_link_libraries(telemetry_gateway PRIVATE satgcs_protocol)
add_executable(protocol_tests tests/protocol_tests.cpp)
target_link_libraries(protocol_tests PRIVATE satgcs_protocol)
if(UNIX)
  target_link_libraries(satellite_device_sim PRIVATE pthread)
  target_link_libraries(telemetry_gateway PRIVATE pthread)
endif()
