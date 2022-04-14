:: Requires you to add C:\Program Files (x86)\Microsoft Visual Studio\*\*\VC to PATH
@echo off
setlocal
mkdir out 2> nul
cd out
call vcvarsall %PROCESSOR_ARCHITECTURE%
cl ../mcstatus.c ../errors.c ../protocols/java.c ../protocols/legacy_java.c ../protocols/tcp.c ../protocols/chat_parser.c ../lib/cJSON/cJSON.c ../lib/cJSON/cJSON_Utils.c
cd ..
endlocal
