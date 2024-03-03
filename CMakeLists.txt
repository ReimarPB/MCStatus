cmake_minimum_required(VERSION 3.5)

project(MCStatus VERSION 1.0.0)

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

add_executable(mcstatus ${SOURCES})

IF (MSVC)
	FetchContent_Declare(
		wingetopt
		GIT_REPOSITORY	https://github.com/alex85k/wingetopt.git
		GIT_TAG		master
	)

	FetchContent_MakeAvailable(wingetopt)

	target_link_libraries(mcstatus PRIVATE wingetopt)
ENDIF()

FetchContent_Declare(
	cJSON
	GIT_REPOSITORY	https://github.com/DaveGamble/cJSON.git
	GIT_TAG		87d8f0961a01bf09bef98ff89bae9fdec42181ee # 1.17.7
)

FetchContent_MakeAvailable(cJSON)

target_link_libraries(mcstatus PRIVATE cjson)
target_link_libraries(mcstatus PRIVATE cjson_utils)

