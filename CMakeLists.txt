cmake_minimum_required(VERSION 3.11)

project(MCStatus VERSION 1.0)

include(FetchContent)

add_definitions(-g)

set(SOURCES
	mcstatus.c
	errors.c
	protocols/java.c
	protocols/legacy_java.c
	utils/tcp.c
	utils/chat_parser.c
	utils/ms.c
	utils/text_converter.c
)

IF (MSVC)

	add_subdirectory(${PROJECT_SOURCE_DIR}/lib/wingetopt)
	add_compile_options(/I ${PROJECT_SOURCE_DIR}/lib/wingetopt/src)

	set(SOURCES ${SOURCES}
		lib/wingetopt/src/getopt.c
	)

ENDIF()

add_executable(mcstatus ${SOURCES})

target_link_libraries(mcstatus PRIVATE cjson)
target_link_libraries(mcstatus PRIVATE cjson_utils)

