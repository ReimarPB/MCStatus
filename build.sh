#!/bin/sh
mkdir -p out
cd out
$CC ../mcstatus.c ../errors.c ../protocols/java.c ../protocols/legacy_java.c ../protocols/tcp.c ../protocols/chat_parser.c ../lib/cJSON/cJSON.c ../lib/cJSON/cJSON_Utils.c -o mcstatus
cd ..

