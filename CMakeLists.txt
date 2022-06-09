cmake_minimum_required(VERSION 3.0)

project(MCStatus VERSION 1.0)

set(SOURCES
	mcstatus.c
	errors.c
	protocols/java.c
	protocols/legacy_java.c
	utils/tcp.c
	utils/chat_parser.c
	utils/ms.c
	lib/cJSON/cJSON.c
	lib/cJSON/cJSON_Utils.c
)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/cJSON)

IF (MSVC)

	add_subdirectory(${PROJECT_SOURCE_DIR}/lib/wingetopt)
	add_compile_options(/I ${PROJECT_SOURCE_DIR}/lib/wingetopt/src)

	set(SOURCES ${SOURCES}
		lib/wingetopt/src/getopt.c
	)

ENDIF()

add_executable(mcstatus ${SOURCES})
