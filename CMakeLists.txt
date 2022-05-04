cmake_minimum_required(VERSION 3.0)

project(MCStatus VERSION 1.0)

add_executable(mcstatus
	mcstatus.c
	errors.c
	protocols/java.c
	protocols/legacy_java.c
	protocols/tcp.c
	protocols/chat_parser.c
	lib/cJSON/cJSON.c
	lib/cJSON/cJSON_Utils.c
)

include_directories(mcstatus "${PROJECT_SOURCE_DIR}/lib/cJSON")

