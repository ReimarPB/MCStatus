#!/bin/sh
mkdir -p out
cd out
$CC ../mcstatus.c ../errors.c ../protocols/java.c ../protocols/legacy_java.c ../protocols/tcp.c ../protocols/chat_parser.c ../lib/cjson/cJSON.c ../lib/cjson/cJSON_Utils.c

cd ..

