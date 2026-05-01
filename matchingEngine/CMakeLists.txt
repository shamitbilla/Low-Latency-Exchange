cmake_minimum_required(VERSION 3.21) 
project(MatchingEngine LANGUAGES CXX)

find_package(Protobuf REQUIRED)
find_package(spdlog REQUIRED)
find_package(cppzmq REQUIRED)

file(GLOB PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/protos/*.proto")
file(GLOB_RECURSE ME_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp"
)

protobuf_generate_cpp(
	PROTO_SRCS
	PROTO_HDRS
	${PROTO_FILES}
	PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
	PROTOC_OPTIONS --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/protos
)

add_executable(matchingEngine
	${ME_SRC_FILES}
	${PROTO_SRCS}
	${PROTO_HDRS}
)

target_link_libraries(matchingEngine
	PRIVATE
		protobuf::libprotobuf
		spdlog::spdlog
		cppzmq
)

target_include_directories(matchingEngine
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/protos
		${CMAKE_CURRENT_SOURCE_DIR}/include
)
# --- For Multicast Server ---

file(GLOB_RECURSE MCS_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/multicastServer/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp"
)
add_executable(subscriber
    src/multicastServer/server.cpp
    ${PROTO_SRCS}
    ${PROTO_HDRS}
)

target_link_libraries(subscriber
    PRIVATE
        protobuf::libprotobuf
        spdlog::spdlog
        cppzmq           # MUST link cppzmq here
)

target_include_directories(subscriber
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/protos
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

